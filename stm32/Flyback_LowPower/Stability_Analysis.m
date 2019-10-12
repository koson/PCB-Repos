clear all
close all
clc

len = 1000;
w = logspace(1,7,len);
s = 1j*w;
Vm = 5;

% Gvd Parameters
Go = 36.5;
Q  = 3.2;
wo = 2.5e3;

Gvd = (Go)./(1+(s./(Q*wo))+(s./wo).^2);

% % Plot Gvd
% figure
% semilogx(w,20*log10(abs(Gvd./Vm)))
% waitforbuttonpress()

% PD Compensator Parameters
fc = 10e3;
theta = 80*pi/180;
wz = fc*sqrt((1-sin(theta))/(1+sin(theta)));
wp = fc*sqrt((1+sin(theta))/(1-sin(theta)));
Gco = sqrt(wz/wp);

GcPD = Gco*(1+s/wz)./(1+s/wp);

% % Plot w/ PD
% figure
% semilogx(w,20*log10(abs(Gvd.*GcPD./Vm)))
% waitforbuttonpress()

% PI Compensator Parameters
Gcinf = 10;
wl = 1e3;

GcPI = Gcinf*(1+wl./s);

% Product of Terms forms T
T = (Gvd.*GcPD.*GcPI)./Vm;
TdB = 20*log10(abs(T));
Tphz = atan2(imag(T),real(T));

figure
semilogx(w,TdB,'g');
%figure
hold on
semilogx(w,Tphz*180/pi,'r');

% Pick R2; Rest Follows...
R2 = 200e3;

R1 = R2/Gco;
C2 = 1/(R1*wl);
C1 = 1/(R1*wz);
R3 = 1/(wp*C1);


