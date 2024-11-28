% step_response.m
% This script simulates the step response of the system for PID tuning.
% The transfer function of the system is assumed to be a first-order system
% with a delay, representing the thermal dynamics of the temperature control system.

% Parameters for the first-order transfer function
K = 1;                % System gain (you can adjust this based on your system)
T = 20;               % Time constant (s)
theta = 5;            % Time delay (s)

% Define the transfer function of the system
s = tf('s');
G = K * exp(-theta * s) / (T * s + 1);

% Define the time vector for simulation
t = 0:0.1:200;        % Simulation time (0 to 200 seconds)

% Simulate the step response of the system
figure;
step(G, t);           % Step response of the plant
title('Step Response of the Plant');
xlabel('Time (seconds)');
ylabel('Temperature (°C)');
grid on;

% Display the simulation result
disp('Step Response Simulation Complete!');
