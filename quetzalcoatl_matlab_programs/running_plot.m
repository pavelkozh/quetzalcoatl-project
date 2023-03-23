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
mfname = "initname";%название лога
path = strcat(logsfolder,mfname);
subplot(4,1,1)
h1 = animatedline;
subplot(4,1,2)
h2 = animatedline;
subplot(4,1,3)
h3 = animatedline;
subplot(4,1,4)
h4 = animatedline;
delta_t = 0.05;
t = -delta_t;
v_total = [];
torque_total = [];
acc_pos_total = [];
w_total = [];

fwrite(dat,1,'uint8');

%цикл работает, пока не произойдет нажатие на клавишу 'f'
while(1)
        key = get(gcf,'CurrentCharacter');
        if (key == 'f')
            break;
        end
        w=fread(dat,4,'uint8');
        w=typecast(uint8([w(1), w(2),w(3),w(4)]), 'single');
        torque=fread(dat,1,'int8');
        acc_pos=fread(dat,4,'uint8');
        acc_pos=typecast(uint8([acc_pos(1), acc_pos(2),acc_pos(3),acc_pos(4)]), 'single');
        v=fread(dat,1,'double');
        t = [t t(end)+delta_t];
        w_total = [w_total w];
        v_total = [v_total v];
        torque_total = [torque_total torque];
        acc_pos_total = [acc_pos_total acc_pos];
        subplot(4,1,1)
        addpoints(h1,t(end),double(w));
        subplot(4,1,2)
        addpoints(h2,t(end),double(torque));
        subplot(4,1,3)
        addpoints(h3,t(end),double(acc_pos));
        subplot(4,1,4)
        addpoints(h4,t(end),double(v));
        
        drawnow
    
end
fclose(dat);
t = t(2:end);
save(path,'w_total','torque_total','acc_pos_total','v_total','t');
disp 'Connection is closed!'