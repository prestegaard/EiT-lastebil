clear
clc
delete(instrfindall);

%User Defined Properties 
serialPort = 'COM3';            % define COM port #
plotTitle = 'Ultrasonic measurements';  % plot title
xLabel = 'Elapsed Time (s)';    % x-axis label
yLabel = 'Distanse [mm]';                % y-axis label
plotGrid = 'on';                % 'off' to turn off grid
min = -1.5;                     % set y-min
max = 520;                      % set y-max
scrollWidth = 10;               % display period in plot, plot entire data log if <= 0
delay = .0000001;                    % make sure sample faster than resolution

%Define Function Variables
time = zeros(1, 100000);
speed_data = zeros(1, 100000);
dist_data = zeros(1, 100000);
count = 0;

%Set up Plot
figure(1)
title(plotTitle,'FontSize',25);
p1 = subplot(1,4,1:3);
plotGraph = plot(time,dist_data(1,:),'-r',...
            'LineWidth',1,...
            'MarkerFaceColor','w',...
            'MarkerSize',2);
xlabel(xLabel,'FontSize',15);
ylabel(yLabel,'FontSize',15);

p2 = subplot(1,4,4);
plotBar = bar(speed_data(1), 'g');


xlabel('Time');
ylabel('Speed input');
axis(p1, [0 10 min max]);
grid(plotGrid);

%Open Serial COM Port
s = serial(serialPort, 'BaudRate', 115200);
disp('Close Plot to End Session');
fopen(s);
tic

while 1% ishandle(plotGraph) %&& ishandle(plotBar)  %Loop when Plot is Active

dat = fscanf(s); %Read Data from Serial as Float
dat = dat(14:end);
dat = strsplit(dat,', ');
dist = str2double(char(dat(1)));
speed = str2double(char(dat(2)));


if(~isempty(dist) && isfloat(dist))%& ~isempty(speed) && isfloat(speed)) %Make sure Data Type is Correct        
    count = count + 1;    
    time(count) = toc;    %Extract Elapsed Time in seconds
    speed_data(:,count) = speed; %Extract 1st Data Element         
    dist_data(:,count) = dist;

    %Set Axis according to Scroll Width
    if(scrollWidth > 0)
    hold on
    set(plotGraph,'XData',time(time > time(count)-scrollWidth),...
        'YData', dist_data(time > time(count)-scrollWidth));
    set(plotBar, 'YData', speed_data(1,count));
    
    axis(p1, [time(count)-scrollWidth time(count) min max]);
    axis(p2, [0 2 0 520]);
    else
    set(plotGraph,'XData',time,'YData',dist_data(1,:));
    set(plotBar,'YData',speed_data(count));

    axis(p1, [0 time(count) min max]);
    axis(p2, [0 2 0 520]);
    end

    %Allow MATLAB to Update Plot
    pause(delay);
end
end

%Close Serial COM Port and Delete useless Variables
fclose(s);

clear count dat delay max min plotGraph plotGraph1 plotGraph2 plotGrid...
    plotTitle s scrollWidth serialPort xLabel yLabel;

disp('Session Terminated');

prompt = 'Export Data? [Y/N]: ';
str = input(prompt,'s');
if str == 'Y' || strcmp(str, ' Y') || str == 'y' || strcmp(str, ' y')
    %export data
    csvwrite('accelData.txt',data);
    type accelData.txt;
else
end

clear str prompt;