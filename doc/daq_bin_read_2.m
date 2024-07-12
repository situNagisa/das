%% init
clear;
clc;

%% load
fileID=fopen('unknown_2024-07-10-16-16-25.bin');% 填文件名和文件位置
temp1=fread(fileID,'int16');
fclose(fileID);
temp1Len=length(temp1);
temp2=reshape(temp1,[1,temp1Len]);
raw1=temp2(1,:);
figure(1)
plot(raw1(1,1:50000));
