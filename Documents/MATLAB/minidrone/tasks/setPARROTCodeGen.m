function setPARROTCodeGen()
% SETPARROTCODEGEN This function sets the appropriate settings
% for code generation for the controller. This is a private function, not
% meant to be used directly.

% Copyright 2017 The MathWorks, Inc.

% Check that the support package is installed
modelList = {'flightControlSystem'};

for k = 1:length(modelList)
    isFCSLoaded = bdIsLoaded(modelList{k});
    if ~isFCSLoaded
        load_system(modelList{k});
    end
    
    % Set the hardware target toolchain
    switch evalin('base','model')
        case 'Mambo'
            set_param(modelList{k},'HardwareBoard','PARROT Mambo');
        case 'RollingSpider'
            set_param(modelList{k},'HardwareBoard','PARROT Rolling Spider');
    end
    
    % Only save if model is not opened, just in case there are unsaved changes
    if ~isFCSLoaded
        save_system(modelList{k});
        bdclose(modelList{k});
    end
end
