
#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Servo.h>

Servo servo;

#define FIREBASE_HOST "frenzy-ecfc7-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "NvEgms7TtDDn0cliU7HsbktRPngvZd8yBFglhj6x"
#define WIFI_SSID "5"
#define WIFI_PASSWORD "150169226"

WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP, "asia.pool.ntp.org",28800);

const int trigPin = D1;  //D4
const int echoPin = D2;  //D3

long duration;
int distance;

int analogInPin  = A0;  
int sensorValue; 
float voltage;
float bat_percentage;

FirebaseData timer,feed, fbdo;
String timerPath;
String schedule;
String spin;
String schedules[]={"00:00","00:00","00:00"} ;
int spins[]{0,0,0};
String currentTime;
int i,feednow=0;

void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to a");
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());   
  timeClient.begin();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  servo.attach(D3, 500, 2400); // Pin attached to D3oi3579
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  
}

void loop() {
  readbattery();
  digitalWrite(LED_BUILTIN, HIGH);
  timeClient.update();
  
  int hours = timeClient.getHours();
  int minutes = timeClient.getMinutes();
  //get time and hours`


  currentTime = (hours < 10 ? "0" : "") + String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes);
  
  Firebase.getInt(feed,"feednow");
  feednow = feed.to<int>();

  if (feednow==1) // Direct Feeding
  {
      servo.write(0); // rotate clockwise
      delay(900); // allow to rotate for n micro seconds, you can change this to your need
      servo.write(180); // stop rotation
      feednow = 0;
      Firebase.setInt(feed, "/feednow",feednow);
      Serial.println("Fed");
      delay(900); 
      servo.write(0); // stop rotation
      recentfeed();
      ultrasonic();

  }
   
else
 { // Scheduling feed
    if(Firebase.get(fbdo,"/AFFV2"))
    {
      // check if there is the diredtory path in the firebase.
      if(fbdo.dataType()=="null")
      {
        Serial.println("Database does not Exist:");
      }
      else
      {  
        Serial.println("Database does Exist:");
        for (int i = 0; i < 3; i++) 
        {
          //time update in reaktime comminication
          timeClient.update();
          currentTime = (hours < 10 ? "0" : "") + String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes);
          timerPath = "/AFFV2/timers/timer" + String(i);   

          // Retrieve schedule value
          Firebase.getString(fbdo, timerPath + "/schedule");
          schedule = fbdo.to<String>();
          schedules[i] = schedule;  
         
          // Retrieve spin value
          Firebase.getInt(fbdo, timerPath + "/spin");
          spin = fbdo.to<String>();
          spins[i] = spin.toInt();

          if(schedules[i] == currentTime)
            {
            int spininput=0;
            
            Serial.println("Schedule:" +schedules[i] );
            spininput=i;
              for(int j = 0; j< spins[spininput] ; j++)
              {
                servo.write(0); // rotate clockwise
                delay(900); // allow to rotate for n micro seconds, you can change this to your need
                servo.write(180); // stop rotation
                Serial.println("Feeding" + j);
                delay(900);      
              }
            Serial.println("Delay1 minute");
            //delay 1 minute
            recentfeed();
            ultrasonic();
            delay(60000);     
            }  
        }
      } 
    fbdo.clear();  
    // Removes the the object inside fbdo
    }
    timeClient.update();
       //prints the time before looping again
 }

}


void ultrasonic(){
FirebaseData firebaseDataUltrasonic;

digitalWrite(trigPin, LOW);
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);

digitalWrite(trigPin, LOW);

// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);

// Calculating the distance
distance= duration*0.034/2;
// Prints the distance on the Serial Monitor
Serial.print("Distance: ");
Serial.println(distance);

Firebase.setInt(firebaseDataUltrasonic, "/distance", distance);
}



void recentfeed(){
timeClient.update();
int hours = timeClient.getHours();
int minutes = timeClient.getMinutes();
FirebaseData firebaserecentfeed;
currentTime = (hours < 10 ? "0" : "") + String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes);
Serial.println("RecentFeed : " + currentTime);
Firebase.setString(firebaserecentfeed, "/recentfeed", currentTime);
}



void readbattery(){
 FirebaseData readbat;
 sensorValue = analogRead(analogInPin);
  voltage = (((sensorValue * 3.3) / 1024) * 2 ); 
  bat_percentage = mapfloat(voltage, 2.8, 4.2, 0, 100); 
 
  if (bat_percentage >= 100)
  {
    bat_percentage = 100;
  }
  if (bat_percentage <= 0)
  {
    bat_percentage = 1;
  }
 
  Serial.print("Analog Value = ");
  Serial.print(sensorValue);
  Serial.print("\t Output Voltage = ");
  Serial.print(voltage);
  Serial.print("\t Battery Percentage = ");
  Serial.println(bat_percentage);
  String batPercentageStr = String(bat_percentage);
  Firebase.setString(readbat,"/battery", batPercentageStr);
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

