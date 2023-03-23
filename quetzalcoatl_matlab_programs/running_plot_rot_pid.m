
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
acc_pos_total = [];
w_total = [];
Eref_total = [];
u_total = [];

fwrite(dat,1,'uint8');

%цикл работает, пока не произойдет нажатие на клавишу 'f'
while(1)
        key = get(gcf,'CurrentCharacter');
        if (key == 'f')
            break;
        end
        
        w=fread(dat,4,'uint8');
        w=typecast(uint8([w(1), w(2),w(3),w(4)]), 'single');
        acc_pos=fread(dat,4,'uint8');
        acc_pos=typecast(uint8([acc_pos(1), acc_pos(2),acc_pos(3),acc_pos(4)]), 'single');
        Eref=fread(dat,1,'double');
        u=fread(dat,1,'double');
        
        t = [t t(end)+delta_t];
        
        w_total = [w_total w];
        acc_pos_total = [acc_pos_total acc_pos];
        Eref_total = [Eref_total Eref];
        u_total = [u_total u];
        
        subplot(4,1,1)
        addpoints(h1,t(end),double(w));
        subplot(4,1,2)
        addpoints(h2,t(end),double(acc_pos));
        subplot(4,1,3)
        addpoints(h3,t(end),double(Eref));        
        subplot(4,1,4)
        addpoints(h4,t(end),double(u));
        
        drawnow
    
end
fclose(dat);
close(gcf);
t = t(2:end);
save(path,'w_total','acc_pos_total','Eref_total','u_total','t');
disp 'Connection is closed!'

plot(t,w_total);
hold on
grid on
plot(t,acc_pos_total);
plot(t,Eref_total);
plot(t,u_total);
