% pid_tuning.m
% This script uses MATLAB's PID Tuner toolbox and Skogestad's method to tune
% the PID parameters and simulate the closed-loop system response.

% Parameters for the system (same as in step_response.m)
K = 1;                % System gain
T = 20;               % Time constant (seconds)
theta = 5;            % Time delay (seconds)

% Define the transfer function of the system (same as in step_response.m)
s = tf('s');
G = K * exp(-theta * s) / (T * s + 1);

% Create a PID controller using MATLAB's PID Tuner toolbox
pidController = pidtune(G, 'PID');  % Use automatic tuning to generate PID parameters

% Display the PID controller parameters
disp('PID Tuning Parameters (Skogestad''s Method):');
disp(pidController);

% Simulate the closed-loop response with the tuned PID controller
closedLoopSystem = feedback(pidController * G, 1);  % Unity feedback

% Define the time vector for simulation
t = 0:0.1:200;

% Simulate the step response of the closed-loop system
figure;
step(closedLoopSystem, t);  % Step response with PID control
title('Step Response with PID Control (Tuned)');
xlabel('Time (seconds)');
ylabel('Temperature (°C)');
grid on;

% Display the simulation result
disp('PID Tuning and Closed-Loop Simulation Complete!');
