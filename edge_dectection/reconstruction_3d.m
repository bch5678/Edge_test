clear;
clc;

ax=importdata('E:\Myreconstruction\edge_dectection\edge_dectection\5out\all.csv');

n=ax(:,1);
x=ax(:,2);
y=ax(:,3);

m=48;

phi=n./m*2*pi;
theta=atan(0.05);
d=x.*cos(theta)-y.*sin(theta);

xx=x-(1-cos(phi)).*d*cos(theta);
yy=(1-cos(phi)).*d*sin(theta)+y;
zz=d.*sin(phi);

axis_new=[xx yy zz];

recons=pointCloud(axis_new);
pcshow(recons);
pcwrite(recons,"recons2c.pcd");
