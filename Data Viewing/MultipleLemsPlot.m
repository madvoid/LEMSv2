%% MultipleLemsPlot.m
% Nipun Gunawardena
% Plot data from multiple LEMSv2 files

clear all, close all, clc

% Set saveFile to true if the figures should be saved as .png files
% Saves to the data folder by default
saveFile = true;



%% Get Files
[FileName, PathName, FilterIndex] = uigetfile('~/Downloads/*.csv', 'Select the Data File','MultiSelect','on');
if iscell(FileName)
    numFiles = length(FileName);
    fileNames = cell(size(FileName));
    lemsNames = cell(size(FileName));
    for i = 1:numFiles
        fileNames{i} = strcat(PathName,FileName{i});
        lemsNames{i} = FileName{i}(1:5);
    end
else
    numFiles = 1;
    fileNames{1} = strcat(PathName,FileName);
    lemsNames{1} = FileName(1:5);
end



%% Iterate through files and plot each
for i = 1:numFiles
    [Year,Month,Date,Hour,Minute,Second,Bat_Lvl,MLX_IR_C,MLX_Amb_C,Upper_Soil_Temp,Upper_Soil_Mois,Lower_Soil_Temp,Lower_Soil_Mois,Pressure,BMP_Amb,Wind_Dir,Wind_Spd,Sunlight,SHT_Amb_C,SHT_Hum_Pct] = importfile(fileNames{i});
    dates = datenum(Year, Month, Date, Hour, Minute, Second);
    figure('Name', lemsNames{i}, 'units', 'normalized', 'outerposition', [0 0 1 1])
    
    % Battery
    subplot(4,2,1)
    plot(dates, Bat_Lvl);
    dynamicDateTicks();
    % datetick();
    ylabel('Battery Level (Volts)');
    title(lemsNames{i});
    grid on
    
    % Temperature
    subplot(4,2,2)
    hold all
    %     plot(dates, BMP_Amb);
    %     plot(dates, MLX_Amb_C);
    plot(dates, MLX_IR_C);
    plot(dates, SHT_Amb_C);
    dynamicDateTicks();
    % datetick();
    ylabel('Temperature (Celsius)');
    %     legend('BMP Ambient', 'MLX Ambient', 'Surface', 'SHT Ambient');
    legend('Surface', 'Air');
    grid on
    
    % Soil Moisture
    subplot(4,2,7)
    hold all
    plot(dates, Lower_Soil_Mois);
    plot(dates, Upper_Soil_Mois);
    dynamicDateTicks();
    % datetick();
    ylabel('Soil Moisture (m^3/m^3)');
    legend('Lower Sensor', 'Upper Sensor');
    grid on
    
    % Soil Temperature
    subplot(4,2,4)
    hold all
    plot(dates, Lower_Soil_Temp);
    plot(dates, Upper_Soil_Temp);
    dynamicDateTicks();
    % datetick();
    ylabel('Soil Temp. (Celsius)');
    legend('Lower Sensor', 'Upper Sensor');
    grid on
    
    % Humidity
    subplot(4,2,5)
    plot(dates, SHT_Hum_Pct);
    dynamicDateTicks();
    % datetick();
    ylabel('Relative Humidity (%RH)');
    grid on
    
    % Sunlight
    subplot(4,2,3)
    plot(dates, Sunlight);
    dynamicDateTicks();
    % datetick();
    ylabel('Incoming Shortwave (W/m^2)');
    grid on
    
    % Wind Dir
    subplot(4,2,6);
    plot(dates, Wind_Dir);
    dynamicDateTicks();
    % datetick();
    ylabel('Wind Direction (^{\circ} from N)');
    
    % Wind Speed
    subplot(4,2,8);
    plot(dates, Wind_Spd);
    dynamicDateTicks();
    % datetick();
    ylabel('Wind Speed (m/s)');
    
    if saveFile
        saveName = strcat(PathName, lemsNames{i}, '.png');
        fig = gcf;
        fig.PaperPositionMode = 'auto';
        print(saveName,'-dpng','-r0')
    end
    
end