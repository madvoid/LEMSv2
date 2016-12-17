%% LemsPlot.m
% Nipun Gunawardena
% Plot data from a LEMSv2 File

clear all, close all, clc


%% Get File and Data
[FileName, PathName, FilterIndex] = uigetfile('~/Downloads', 'Select the Data File');
fullFileName = strcat(PathName, FileName);
% fullFileName = '~/Downloads/LEMS/LEMSL_00.CSV';       % UncommeXnt this line and comment above two to hard code file path
[Year,Month,Date,Hour,Minute,Second,Bat_Lvl,MLX_IR_C,MLX_Amb_C,Upper_Soil_Temp,Upper_Soil_Mois,Lower_Soil_Temp,Lower_Soil_Mois,Pressure,BMP_Amb,Wind_Dir,Wind_Spd,Sunlight,SHT_Amb_C,SHT_Hum_Pct] = importfile(fullFileName);


%% Get Dates
dates = datenum(Year, Month, Date, Hour, Minute, Second);


%% Plot Battery
figure()
hold on
plot(dates, Bat_Lvl);
dynamicDateTicks();
% datetick();
xlabel('Date');
ylabel('Battery Level (Volts)');
title('Battery Voltage');


%% Plot Temperature
figure()
hold all
plot(dates, BMP_Amb);
plot(dates, MLX_Amb_C);
plot(dates, MLX_IR_C);
plot(dates, SHT_Amb_C);
dynamicDateTicks();
% datetick();
xlabel('Date');
ylabel('Temperature (Celsius)');
title('Air/Surface Temperature');
legend('BMP Ambient', 'MLX Ambient', 'Surface', 'SHT Ambient');


%% Plot Soil Moisture
figure()
hold all
plot(dates, Lower_Soil_Mois);
plot(dates, Upper_Soil_Mois);
dynamicDateTicks();
% datetick();
xlabel('Date');
ylabel('Soil Moisture (m^3/m^3)');
title('Soil Moisture');
legend('Lower Sensor', 'Upper Sensor');


%% Plot Soil Temperature
figure()
hold all
plot(dates, Lower_Soil_Temp);
plot(dates, Upper_Soil_Temp);
dynamicDateTicks();
% datetick();
xlabel('Date');
ylabel('Soil Temperature (Celsius)');
title('Soil Temperature');
legend('Lower Sensor', 'Upper Sensor');


%% Plot Pressure
figure()
plot(dates, Pressure);
dynamicDateTicks();
% datetick();
xlabel('Date');
ylabel('Pressure (Pascals)');
title('Barometric Pressure');


%% Humidity
figure()
plot(dates, SHT_Hum_Pct);
dynamicDateTicks();
% datetick();
xlabel('Date');
ylabel('Relative Humidity (%RH)');
title('Relative Humidity');


%% Sunlight
figure()
plot(dates, Sunlight);
dynamicDateTicks();
% datetick();
xlabel('Date');
ylabel('Incoming Shortwave Radiation (W/m^2)');
title('Incoming Solar Radiation');


%% Wind Direction/Spd
figure()
subplot(2,1,1);
plot(dates, Wind_Dir);
dynamicDateTicks();
% datetick();
xlabel('Date');
ylabel('Wind Direction (Degrees from North)');
title('Wind Direction');

subplot(2,1,2);
plot(dates, Wind_Spd);
dynamicDateTicks();
% datetick();
xlabel('Date');
ylabel('Wind Speed (m/s)');
title('Wind Speed');
