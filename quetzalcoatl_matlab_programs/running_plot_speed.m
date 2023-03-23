clc
close all
clear all
global dat
delete(instrfind);
dat = serial('COM3', 'BaudRate', 115200);
dat.InputBufferSize = 4096;
fopen(dat);
set(dat, 'ByteOrder', 'littleEndian'); %'littleEndian'
disp 'Connection is ready!'
logsfolder = '.\logs\';%папка с логами
figure('Name','Введите название нового лога и кликните по фигуре')
mfname = text(0.1,.5,'');
mfname.Editing = 'on';
mfname.BackgroundColor = [1 1 1];
waitfor(mfname,'Editing','off');
mfname.BackgroundColor = [1 1 0];
mfname = mfname.String;
close(gcf);

% mfname = "initname";%название лога
path = strcat(logsfolder,mfname);
subplot(3,1,1)
h1 = animatedline;
subplot(3,1,2)
h2 = animatedline;
subplot(3,1,3)
h3 = animatedline;
delta_t = 0.05;
t = -delta_t;
s_px_filter_total = [];
s_px_total = [];
s_can_total = [];

fwrite(dat,1,'uint8');

%цикл работает, пока не произойдет нажатие на клавишу 'f'
while(1)
        key = get(gcf,'CurrentCharacter');
        if (key == 'f')
            break;
        end
        s_px_filter=fread(dat,1,'double');
        s_px=fread(dat,1,'double');
        s_can=fread(dat,1,'double');
        t = [t t(end)+delta_t];
        s_px_filter_total = [s_px_filter_total s_px_filter];
        s_px_total = [s_px_total s_px];
        s_can_total = [s_can_total s_can];
        subplot(3,1,1)
        addpoints(h1,t(end),double(s_px_filter));
        subplot(3,1,2)
        addpoints(h2,t(end),double(s_px));
        subplot(3,1,3)
        addpoints(h3,t(end),double(s_can));
        
        drawnow
    
end
fclose(dat);
close(gcf);
t = t(2:end);
save(path,'s_px_filter_total','s_px_total','s_can_total','t');
disp 'Connection is closed!'

plot(t,s_px_filter_total);
hold on
grid on
plot(t,s_px_total);
plot(t,s_can_total);

legend('px4 filter','px4 nonfilter','can');