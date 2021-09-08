clc;
clear all;
close all;

myMQTT=mqtt('tcp://brokerurl','Username','yourUsername','Password','yourPassword','ClientID','clientId','Port',1883);

%Publish data

Topic = 'Led1/status';
Message = '0';

publish(myMQTT,Topic,Message,'Qos',0);
%n can be 0,1,2

%susbcribe

Topic2 = "ESP8266/data";
Data = subscribe(myMQTT,Topic2,'Qos',0);

pause(6);

x=0;
y=0;
z=0;

while true
   values=Data.MessageCount;
   if(values>0)
      op = jsondecode(read(Data));
      temp = op.parameters.temp;
      hum = op.parameters.humidity;
      ran = op.parameters.Random;
      
      x = [x,temp];
      y = [y,hum];
      z = [z,ran];
      
      subplot(3,1,1);
      plot(x);
      ylim([0,100]);
      grid ON;
      title("Temperature");
      
      subplot(3,1,2);
      plot(y);
      ylim([0,100]);
      grid ON;
      title("Humidity");
      
      subplot(3,1,3);
      plot(z);
      ylim([0,100]);
      grid ON;
      title("Random Number");
      
      drawnow
      pause(3)      
   end   
end
