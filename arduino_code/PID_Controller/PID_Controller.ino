#include <SoftwareSerial.h>
#include <PID_v1.h>
#include <max6675.h>

// Pin Definitions for MAX6675 Thermocouple
#define CLK_PIN 9
#define SO_PIN 10
#define CS_PIN 13

// Define Pins for Dimmer Control
#define DIMMER_PIN_1 2
#define DIMMER_PIN_2 3
#define DIMMER_PIN_3 4
#define DIMMER_PIN_4 5

// Create an instance of the thermocouple using the defined pins
MAX6675 thermocouple(CLK_PIN, CS_PIN, SO_PIN);

// PID Control Variables
double Setpoint, Input, Output;
double Kp = 2.0, Ki = 0.644, Kd = 1.548;  // PID coefficients
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

// Dimmer Control Combinations (4-pin combinations for 16 states)
const int dimmerCombinations[16][4] = {
  {HIGH, HIGH, HIGH, HIGH},  // 0
  {LOW, HIGH, HIGH, HIGH},   // 1
  {HIGH, LOW, HIGH, HIGH},   // 2
  {LOW, LOW, HIGH, HIGH},    // 3
  {HIGH, HIGH, LOW, HIGH},   // 4
  {LOW, HIGH, LOW, HIGH},    // 5
  {HIGH, LOW, LOW, HIGH},    // 6
  {LOW, LOW, LOW, HIGH},     // 7
  {HIGH, HIGH, HIGH, LOW},   // 8
  {LOW, HIGH, HIGH, LOW},    // 9
  {HIGH, LOW, HIGH, LOW},    // 10
  {LOW, LOW, HIGH, LOW},     // 11
  {HIGH, HIGH, LOW, LOW},    // 12
  {LOW, HIGH, LOW, LOW},     // 13
  {HIGH, LOW, LOW, LOW},     // 14
  {LOW, LOW, LOW, LOW}       // 15
};

// Function to Control the Dimmer
void setDimmer(int dim) {
  if (dim < 0 || dim > 15) {
    Serial.println("Error: Invalid dimmer value");
    return;
  }

  // Set the dimmer pins according to the selected combination
  digitalWrite(DIMMER_PIN_1, dimmerCombinations[dim][0]);
  digitalWrite(DIMMER_PIN_2, dimmerCombinations[dim][1]);
  digitalWrite(DIMMER_PIN_3, dimmerCombinations[dim][2]);
  digitalWrite(DIMMER_PIN_4, dimmerCombinations[dim][3]);
}

void setup() {
  // Initialize PID
  Setpoint = 43.0;  // Desired temperature in Celsius (you can change this value)

  // Initialize communication
  Serial.begin(9600);

  // Set up MAX6675 thermocouple
  pinMode(CLK_PIN, OUTPUT);
  pinMode(SO_PIN, INPUT);
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);  // Deselect the chip

  // Set up Dimmer Control Pins
  pinMode(DIMMER_PIN_1, OUTPUT);
  pinMode(DIMMER_PIN_2, OUTPUT);
  pinMode(DIMMER_PIN_3, OUTPUT);
  pinMode(DIMMER_PIN_4, OUTPUT);

  // PID Control Mode
  myPID.SetMode(AUTOMATIC);

  // Clear previous data and reset timer
  Serial.println("CLEARDATA");
  Serial.println("LABEL, Real Time, Time, Temperature, SetPoint");
  Serial.println("RESETTIMER");
}

void loop() {
  // Read temperature from the thermocouple
  Input = thermocouple.readCelsius();  // Get the current temperature

  // Compute the PID output
  myPID.Compute();

  // Map the PID output to the dimmer input (scaled to 0-15 range for control)
  int dimmerInput = Output / 16;

  // Control the dimmer based on the computed PID output
  setDimmer(dimmerInput);

  // Get current time and print data to Serial Monitor
  float timeElapsed = millis() * 0.001;  // Time in seconds
  Serial.print("DATA,TIME,");
  Serial.print(timeElapsed);
  Serial.print(",");
  Serial.print(Input);  // Current temperature
  Serial.print(",");
  Serial.println(Setpoint);  // Desired setpoint
}

int spiRead() {
  int value = 0;
  digitalWrite(CS_PIN, LOW);
  delay(2);
  digitalWrite(CS_PIN, HIGH);
  delay(220);
  digitalWrite(CS_PIN, LOW);
  digitalWrite(CLK_PIN, HIGH);
  delay(1);
  digitalWrite(CLK_PIN, LOW);

  // Read the 16-bit value from the thermocouple
  for (int i = 14; i >= 0; i--) {
    digitalWrite(CLK_PIN, HIGH);
    value += digitalRead(SO_PIN) << i;
    digitalWrite(CLK_PIN, LOW);
  }

  // Check for an error in reading the thermocouple
  if ((value & 0x04) == 0x04) return -1;  // Error check

  return value >> 3;  // Return the 12-bit temperature data
}
