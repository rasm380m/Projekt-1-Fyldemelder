#include <SigFox.h>
#include <ArduinoLowPower.h>

int     trigPin = 0;
int     echoPin = 1;
int     transistor = 7;

float   totalDist;   //Total distance in CM
int     meanCalc;
int     percentage;
unsigned long sleepTime = (1000*60*60*24)-1;

// Tager count-antal målinger, og finder gennemsnittet af det.
int   HC_SR04Measure(int count){
      digitalWrite(transistor,LOW); //Boost converteren tændes, og derved tændes sensoren
      delay(1000);
      long t;
      int  d;
      int  mean;
      for(int i = 0;i<count;i++){
      digitalWrite(trigPin,HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin,LOW);
      
      t = pulseIn(echoPin,HIGH);
      d = d + (t*0.034)/2;
      delay(50);
      }
      mean = d/count;
      digitalWrite(transistor,HIGH);//Boost converteren slukkes, og derved slukkes sensoren
      return mean;
}

void setup() {
   LowPower.attachInterruptWakeup(RTC_ALARM_WAKEUP, dummy, CHANGE); //Der tilknyttes et interrupt.
   //in- og outputs defineres
   pinMode(trigPin,OUTPUT); 
   pinMode(echoPin,INPUT);
   pinMode(transistor,OUTPUT);
   delay(120000);
   
//Måling for at bestemme hvad 0% er.
//HC_SR04Measure(Antal målinger)      
      totalDist =  HC_SR04Measure(10);
   
}

void loop() {
      meanCalc = HC_SR04Measure(10);//Gennemsnitsværdien findes
      percentage = ((totalDist - meanCalc)/totalDist)*100; //Den procentvise fyldningsgrad udregnes
  
  //Procentværdien sendes med Sigfox
  SigFox.begin();
  SigFox.beginPacket();
  SigFox.print(percentage);
  SigFox.print("%");
  SigFox.endPacket();
  SigFox.end();
  //Controlleren sættes i Standby Mode
  LowPower.deepSleep(sleepTime);//24 hour-1mS
}

void dummy() {
    //Gør ingenting efter "wake-up"
}
