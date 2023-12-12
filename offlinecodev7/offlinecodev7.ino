#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Servo.h>
#include <FS.h>
#include <TimeLib.h>

Servo servo;
WiFiUDP ntpUDP;

#define FIREBASE_HOST "frenzy-ecfc7-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "NvEgms7TtDDn0cliU7HsbktRPngvZd8yBFglhj6x"
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org",28800);

const int trigPin = D1;  //D1
const int echoPin = D2;  //D2
bool offlinetime = false;
FirebaseData feed;
String currentTime;

String schedules[]={"00:00","00:00","00:00","00:00","00:00","00:00","00:00","00:00","00:00","00:00"} ;
int spins[]{0,0,0,0,0,0,0,0,0,0};
int feednow = 0;

void manual()
{
      servo.write(0); // rotate clockwise
      delay(900); // allow to rotate for n micro seconds, you can change this to your need
      servo.write(180); // stop rotation
      delay(900); 
      servo.write(0); // stop rotation
      feednow = 0;
      Firebase.setInt(feed, "/feednow",feednow);
      Serial.println("Feeed now Manual success");
      recentfeed();
      ultrasonic();
      readbattery();
}

      void scheduling()
      {
          if( WiFi.status() == WL_CONNECTED && Firebase.get(feed,"/AFFV2"))
          {
              if(feed.dataType()=="null") // check if there is the diredtory path in the firebase.
                {
                  Serial.println("Path does not Exist:");
                }
                else
                { 
                    Serial.println("Path does Exist:");
                    int node = 0 ;
                    String timerPath = "";
                    Firebase.getInt(feed,"/node");
                    node = feed.to<int>();
                    Serial.println("Node Value : " + String(node));
                   for (int i = 0; i < node; i++) 
                   {
                      timeClient.update();  //time update in realtime comminication
                      timerPath = "/AFFV2/timers/timer" + String(i);
                      int hours = timeClient.getHours();
                      int minutes = timeClient.getMinutes();
                      currentTime = (hours < 10 ? "0" : "") + String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes);
                                    
                      Firebase.getString(feed, timerPath + "/schedule"); // Retrieve schedule value
                      schedules[i] = feed.to<String>();
                      Firebase.getInt(feed, timerPath + "/spin"); // Retrieve spin value
                      spins[i] = feed.to<int>();
                      Serial.println("Current Time : " + currentTime);
                      Serial.println("Schedule: " + schedules[i] );

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
                              Serial.println("Delay 1 minute"); //delay 1 minute
                              recentfeed();
                              ultrasonic();
                              delay(60000);   
                            }  
                   }
                    writeArrayToFile("/schedules.txt", schedules);
                    writeIntArrayToFile("/spins.txt", spins);    
                } 
                 feed.clear();   // Removes the the object inside fbdo
             } 
           readbattery();
      }

 
  void gettime()
  {
   timeClient.update();
   int hours = timeClient.getHours();
   int minutes = timeClient.getMinutes();
   currentTime = (hours < 10 ? "0" : "") + String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes);
   writeTimeFile("/read.txt", currentTime);
  }

  void writeTimeFile(const char *fileName, const String data) 
  {
      File file = SPIFFS.open(fileName, "w");
      if (file) {
        file.println(data);
        Serial.println("Offline Time Data : " + data);
        file.close();
        Serial.println("Offline Time Data written successfully");
      } else {
        Serial.println("Failed write Time Data Offline");
      }
  }

  void readTimeFromFile(const char *fileName) 
  {
    File file = SPIFFS.open(fileName, "r");
    if (file) {
      String timeString = file.readStringUntil('\n'); // Read the time data from the file
      file.close();
      int hours, minutes; // Parse the time data (assuming it's in "HH:MM" format)
      sscanf(timeString.c_str(), "%d:%d", &hours, &minutes);
      setTime(hours, minutes, 0, 1, 1, 1970); // Set the internal clock
      Serial.println("Time set from file successfully");

    } else {
      Serial.println("Failed to open Time file for reading");
    }
  }

  void setup() 
  {
    Serial.begin(9600);
    delay(1000);  
    Serial.println("Running SETUP");
    connectToWiFi();
    servo.attach(D3, 500, 2400); 
    servo.write(0);
    pinMode(trigPin, OUTPUT); 
    pinMode(echoPin, INPUT); 
    timeClient.begin();
    initializeSPIFFS();
  }

  void initializeSPIFFS() {
    if (SPIFFS.begin()) {
      Serial.println("SPIFFS initialization successful");
    } else {
      Serial.println("SPIFFS initialization failed");
      // Handle the failure, add additional error-handling code if needed
    }
  }

void offlinerun() {

  if (!offlinetime) {
    // Your code that should run only once
    // ...
    Serial.println("Set Offline Time");
    readTimeFromFile("/read.txt");
    // Set the flag to true, indicating that the code has run
    offlinetime = true;
    }

  unsigned long currenttime = now();
  int hours = (currenttime % 86400L) / 3600;
  int minutes = (currenttime % 3600) / 60;
  String StringNumber ="";

  for (int i = 0; i < 10; i++) { // Compare the current time with the array of valid times
    StringNumber = (hours < 10 ? "0" : "") + String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes); // Display the current time in "HH:MM" format
   
    Serial.println("Schedule Time: " + schedules[i]);
    Serial.println("Current Offline Time: " + StringNumber);  // Debug prints

    schedules[i].trim();
    StringNumber.trim();  // Trim spaces

    Serial.println("Trimmed Schedule Time: " + schedules[i]);
    Serial.println("Trimmed Current Offline Time: " + StringNumber); // Debug prints after trimming
    Serial.println("Trimmed Current Spins: " + spins[i]);

    if (schedules[i] == StringNumber) 
    { // Perform actions or logic for matching time
      
      Serial.println("Current time matches a valid time in the array!");
      for(int j = 0; j< spins[i] ; j++)
       {
        servo.write(0); // rotate clockwise
        delay(900); // allow to rotate for n micro seconds, you can change this to your need
        servo.write(180); // stop rotation
        Serial.println("Feeding");
        delay(900);      
       }
        Serial.println("Delay 1 minute"); //delay 1 minute
        delay(60000);
        
    }
    delay(2000);
  }
  RetryConnectWIFI();
}

void loop() 
{
    if(WiFi.status() != WL_CONNECTED){
      Serial.println("----------------Start of  Offline Loop----------------");
      readArrayFromFile("/schedules.txt", schedules);
      readIntArrayFromFile("/spins.txt", spins);
      offlinerun();
      Serial.println("----------------End of  Offline Loop----------------");
    }
    else if (WiFi.status() == WL_CONNECTED)
    { 
      Serial.println("============Start of  Online Loop============");
        offlinetime = false;
        gettime();
        Firebase.getInt(feed,"feednow");
        feednow = feed.to<int>();

        if (WiFi.status() == WL_CONNECTED && feednow==1) // Direct Feeding
        {
            manual();
        }
        else if (WiFi.status() == WL_CONNECTED)
        { // Scheduling feed
          scheduling(); 
        }
      feed.clear();
      Serial.println("============End of  Online Loop============");
    }
    else{
      Serial.println("===============Continue Offline===============");
    }
}

  void readArrayFromFile(const String& fileName, String array[]) 
  {
    const int arraySize = sizeof(schedules) / sizeof(schedules[0]);
    Serial.println("Reading File:");  // Open file for reading
    File file = SPIFFS.open(fileName, "r");
    if (!file) {
      Serial.println("File open failed");
      return;
    }

    for (int i = 0; i < arraySize; i++) {// Read strings from the file and update the array
      String line = file.readStringUntil('\n');
      array[i] = line;
    }
    Serial.println("Read Schedule[] Complete");
    file.close();  // Close the file
  }

  void writeArrayToFile(const String& fileName, String array[]) 
  {
    const int arraySize = sizeof(schedules) / sizeof(schedules[0]);
    File file = SPIFFS.open(fileName, "w"); // Open file for writing
    if (!file) {
      Serial.println("File open failed");
      return;
    }
  
    for (int i = 0; i < arraySize; i++) {   // Write each string to the file
      file.println(array[i]);
    }
    Serial.println("Schedule Successfully Saved Offline");
    file.close(); // Close the file
  }



  void writeIntArrayToFile(const String& fileName, int array[]) {
    // Open file for writing
  const int arraySize = sizeof(spins) / sizeof(spins[0]);
    File file = SPIFFS.open(fileName, "w");
    if (!file) {
      Serial.println("File open failed");
      return;
    }

    for (int i = 0; i < arraySize; i++) { // Write each integer to the file
      file.println(array[i]);
    }
    Serial.println("Spins Successfully Saved Offline");
    // Close the file
    file.close();
  }

// Function to read the file and update the integer array
void readIntArrayFromFile(const String& fileName, int array[]) 
{
  // Open file for reading
const int arraySize = sizeof(spins) / sizeof(spins[0]);
  File file = SPIFFS.open(fileName, "r");
  if (!file) {
    Serial.println("File open failed");
    return;
  }

  // Read integers from the file and update the array
  for (int i = 0; i < arraySize; i++) {
    String line = file.readStringUntil('\n');
    array[i] = line.toInt();
  }

    Serial.println("Read int[] Successfully");
  // Close the file
  file.close();
}


void ultrasonic()
{
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
    const String ssid1 = "5";
    const String password1 = "150169226";
    int attempts = 0;

    WiFi.begin(ssid1, password1);   // Attempt to connect to WiFi
    while (WiFi.status() != WL_CONNECTED && attempts < 10) {
    delay(1100);
    Serial.print(".");

    attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("\nConnected to WiFi: ");
      Serial.println(WiFi.localIP());
      Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
      gettime();
    } else {
      Serial.println("\nFailed to connect to WiFi. Please check your credentials");
    }
   
}

void RetryConnectWIFI() 
{
    const String ssid2 = "6";
    const String password2 = "12345678";
    Serial.println("Connecting to WiFi...");
    // Attempt to connect to WiFi
    WiFi.begin(ssid2, password2);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("\nConnected to WiFi: ");
      Serial.println(WiFi.localIP());
      gettime();
      Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    } else {
      Serial.println("\nFailed to connect to WiFi. Please check your credentials");

    }
  
}