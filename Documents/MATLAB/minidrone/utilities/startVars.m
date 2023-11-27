%% startVars.m - Initialize variables
% This script initializes variables and buses required for the model to
% work.

% Copyright 2013-2017 The MathWorks, Inc.

% Register variables in the workspace before the project is loaded
initVars = who;

% Bus definitions
slBusDefinitionCommand; 
slBusDefinitionSensors;
slBusDefinitionEnvironment;
slBusDefinitionStates;

% Enum definitions
slEnumDefinition;

% Sampling rate
Ts= 0.005;

% Simulation time
TFinal = 100;

% Geometric properties
thrustArm = 0.10795;

% Initial conditions
init.date = [2017 1 1 0 0 0];
init.posLLA = [0 0 0];
init.posNED = [0 0 0];
init.vb = [0 0 0];
init.euler = [0 0 0];
init.angRates = [0 0 0];

% Initialize States:
States = Simulink.Bus.createMATLABStruct('StatesBus');
States.V_body = init.vb';
States.Omega_body = init.angRates';
States.Euler = init.euler';
States.X_ned = init.posNED';
States.LLA = init.posLLA;
States.DCM_be = eye(3);

% Environment
rho = 1.184;
g = 9.81;

% Variables
% Load MAT file with model for persistence
load('modelParrot.mat');
% Obtain vehicle variables
vehicleVars;
% Obtain sensor variables
sensorsVars;
% Obtain command variables
commandVars;

% Simulation Settings
takeOffDuration = 1;

% Load linear airframe
load('linearizedAirframe.mat');

%% Custom Variables
% Add your variables here:
% myvariable = 0;

% Register variables after the project is loaded and store the variables in
% initVars so they can be cleared later on the project shutdown.
endVars = who;
initVars = setdiff(endVars,initVars);
clear endVars;
