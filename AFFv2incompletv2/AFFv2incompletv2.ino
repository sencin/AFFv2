
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
  
}

void loop() {

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

          Serial.println("CURRENT TIME: "+ currentTime);

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
            delay(60000);     
            }  
        }
      } 
    fbdo.clear();  
    // Removes the the object inside fbdo
    }
    timeClient.update();
    currentTime = (hours < 10 ? "0" : "") + String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes);
    Serial.println("CURRENT TIME: "+ currentTime);
    //prints the time before looping again
 }

}
