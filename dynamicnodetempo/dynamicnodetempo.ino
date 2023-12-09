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

FirebaseData feed;
String timerPath, schedule, spin, currentTime;

String schedules[]={"00:00","00:00","00:00","00:00","00:00","00:00","00:00","00:00","00:00","00:00"} ;
int spins[]{0,0,0,0,0,0,0,0,0,0};
int feednow,node=0;

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

void loop() 
{
  readbattery();
   if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connection lost. Reconnecting...");
    connectToWiFi();
  }


  Firebase.getInt(feed,"feednow");
  feednow = feed.to<int>();

  if (feednow==1) // Direct Feeding
  {
      servo.write(0); // rotate clockwise
      delay(900); // allow to rotate for n micro seconds, you can change this to your need
      servo.write(180); // stop rotation
      feednow = 0;
      Firebase.setInt(feed, "/feednow",feednow);
      Serial.println("Feeed now Manual success");
      delay(900); 
      servo.write(0); // stop rotation
      recentfeed();
      ultrasonic();
  }

else
 { // Scheduling feed
    if(Firebase.get(feed,"/AFFV2"))
    {
      // check if there is the diredtory path in the firebase.
      if(feed.dataType()=="null")
      {
        Serial.println("Schedule does not Exist:");
      }
      else
      {  
        Serial.println("Schedule does Exist:");
        Firebase.getInt(feed,"/node");
        node = feed.to<int>();
        Serial.println("Node Value : " + String(node));
        for (int i = 0; i < node; i++) 
        {
            timeClient.update(); //time update in realtime comminication
            int hours = timeClient.getHours();
            int minutes = timeClient.getMinutes();
 
            currentTime = (hours < 10 ? "0" : "") + String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes);
            timerPath = "/AFFV2/timers/timer" + String(i);   
            
            Firebase.getString(feed, timerPath + "/schedule"); // Retrieve schedule value
            schedule = feed.to<String>();
            schedules[i] = schedule;  // Store schedule value
            
            Firebase.getInt(feed, timerPath + "/spin"); // Retrieve spin value
            spin = feed.to<String>(); // Store spin value in array
            spins[i] = spin.toInt();
            
            Serial.println("Current Time : " + currentTime);
            Serial.println("Schedule:" +schedules[i] );

            if(schedules[i] == currentTime)
              {
                for(int j = 0; j< spins[i] ; j++)
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
                delay(60000);     
              }  
        }
      } 
      feed.clear();   // Removes the the object inside fbdo
    }  
 }

}

void ultrasonic(){
  long duration;
  int distance;
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
Serial.print("Distance: " + distance);

Firebase.setString(firebaseDataUltrasonic, "/distance", "Remaining: " + String(distance) + "");
Firebase.setString(firebaseDataUltrasonic, "/messages/distance","Distance: " + String(distance) + " centimeter");
}

void recentfeed()
{
    timeClient.update();
    int hours = timeClient.getHours();
    int minutes = timeClient.getMinutes();
    FirebaseData firebaserecentfeed;
    currentTime = (hours < 10 ? "0" : "") + String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes);
    Serial.println("RecentFeed : " + currentTime);
    Firebase.setString(firebaserecentfeed, "/recentfeed", currentTime);
     Firebase.setString(firebaserecentfeed, "/messages/recent", "Recent Feed: " + String(currentTime) + " ");
}

void readbattery()
{
   int analogInPin  = A0;  
   int sensorValue; 
   float voltage;
   float bat_percentage;
   FirebaseData readbat;
     //Read analog input
  
    sensorValue = analogRead(analogInPin);  // Read analog input
    voltage = (sensorValue / 1024.0) * 4.27 * 4.2;  // Calculate voltage
    //Map the voltage to a percentage between 0 and 100

     bat_percentage = mapfloat(voltage, 3.0, 4.2, 0, 100);

     Serial.print("Battery Voltage: ");
     Serial.print(voltage, 2);  // Display voltage with 2 decimal places
     Serial.print("V\tBattery Percentage: ");
     Serial.print(bat_percentage, 2);  // Display percentage with 2 decimal places
     Serial.println("%");

     if(bat_percentage >= 100){
       Firebase.setString(readbat, "/battery", "Charging");
     }
     else if(bat_percentage < 1){
       Firebase.setString(readbat, "/battery", "1");
     }
     else{
      Firebase.setString(readbat, "/battery", String(bat_percentage));
      }
      Firebase.setString(readbat, "/messages/voltage", "Battery Voltage: " + String(voltage) + "V");
      Firebase.setString(readbat, "/messages/percentage", "Battery Percent: " + String(bat_percentage) + "%");
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void connectToWiFi() 
{
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

