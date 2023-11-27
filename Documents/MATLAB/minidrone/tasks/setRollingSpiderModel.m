function setRollingSpiderModel
% SETROLLINGSPIDERMODEL Helper function to set the quadcopter properties to
% the Rolling Spider model.

% Copyright 2017 The MathWorks, Inc.

% Obtain current project
p = slproject.getCurrentProject;
% Obtain project root
projectRoot = p.RootFolder;
% Set Rolling Spider model
model = 'RollingSpider';
% Save variabe to .MAT fie for persistence
save(fullfile(projectRoot,'mainModels','modelParrot.mat'),'model');
% Assign variables according to Rolling Spider
evalin('base','Vehicle.Airframe.mass = 0.068;');
evalin('base','Vehicle.Airframe.inertia = diag([0.0686e-3 0.092e-3 0.1366e-3]);');
evalin('base','Sensors.velocityToOpticalFlowGain = 1/20;');
assignin('base','model',model);