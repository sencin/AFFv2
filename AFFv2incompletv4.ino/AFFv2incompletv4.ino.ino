
#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Servo.h>

Servo servo;
WiFiUDP ntpUDP;


#define FIREBASE_HOST "frenzy-ecfc7-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "NvEgms7TtDDn0cliU7HsbktRPngvZd8yBFglhj6x"
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org",28800);

const String ssid = "5";
const String password = "150169226";

const int trigPin = D1;  //D1
const int echoPin = D2;  //D2

long duration;
int distance;

int analogInPin  = A0;  
int sensorValue; 
float voltage;
float bat_percentage;

FirebaseData timer,feed, fbdo;
String timerPath, schedule, spin, currentTime;

String schedules[]={"00:00","00:00","00:00","00:00","00:00"} ;

int spins[]{0,0,0,0,0,0};

int feednow=0;



void setup() 
{
  servo.attach(D3, 500, 2400); 
  servo.write(0);
  timeClient.begin();
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
  Serial.begin(9600);
  connectToWiFi();
  Serial.println();   
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
 
}

void loop() {
  readbattery();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connection lost. Reconnecting...");
    connectToWiFi();
  }
 
  
  timeClient.update();
  int hours = timeClient.getHours();
  int minutes = timeClient.getMinutes();

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
        for (int i = 0; i < 5; i++) 
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
          
        }
        runoffine();
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


void connectToWiFi() {
  Serial.println("Connecting to WiFi...");

  // Attempt to connect to WiFi
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("\nConnected to WiFi: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi. Please check your credentials");
  }
}

void runoffine(){
    
    int attempts = 0;
    const int maxAttempts = 10;
    bool successfulRun = false;

        while (attempts < maxAttempts && !successfulRun)
      {
          timeClient.update();
          int hours = timeClient.getHours();
          int minutes = timeClient.getMinutes();
          //get time and hours`
          currentTime = (hours < 10 ? "0" : "") + String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes);
          Serial.println("Current Time: " + currentTime);
          
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
          {
            for (int i = 0; i < 5; i++) 
             {
                if(schedules[i] == currentTime)
                {
                  
                    Serial.println("Schedule:" +schedules[i] );
                    
                      for(int j = 0; j< spins[i]; j++)
                      {
                        servo.write(0); // rotate clockwise
                        delay(900); // allow to rotate for n micro seconds, you can change this to your need
                        servo.write(180); // stop rotation
                        Serial.println("Feeding");
                        delay(900);      
                      }
                    Serial.println("Delay1 minute");
                    //delay 1 minute
                    recentfeed();
                    ultrasonic();
                    successfulRun = true;
                    delay(60000);
                          
                }
             }
        }


            delay(1000); // for example, wait 1 second between attempts
            attempts++;
            Serial.println(attempts);
     }
  
}
