#include <ESP8266WiFi.h>
#include <Servo.h>
#include "webPage.h"
#include <ESP8266HTTPClient.h>


char ssid[] = "Omid";          
char pass[] = "oM1234567890";  
WiFiServer server(80);

const char* serverAddress = "192.168.170.148";
const int serverPort = 3000; 
const String apiEndpoint = "/email?to=omiddeldar.om@gmail.com&storage=";
const long intervalMail = 24 * 60 * 60 * 1000;  
unsigned long previousMillis = 0;
bool sendEmail = true;
int trig = D6;
int echo = D5;
float duration, distance;  
int charge;
String request = "";
int LED_Pin = 13;
const int ldrPin = A0;  
int ldrValue = 0;       
Servo myservo;
int manualFeed = 0;
int appFeed = 0;
int boulStatus = 0;
unsigned long previousFeed = 0;    
unsigned long feedTiming = 60000;  
unsigned long startTime;
int countTimer = 1;
const int interval = 1000;  
void setup() {
  pinMode(LED_Pin, OUTPUT);
  myservo.attach(D1);
  Serial.begin(115200);
  Serial.println();
  Serial.println("Serial started at 115200");
  Serial.println("SMART_FEEDER_AUTOMATION");
  Serial.println();
  pinMode(trig, OUTPUT);  
  pinMode(echo, INPUT);   
  // Connect to a WiFi network
  Serial.print(F("Connecting to "));
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println(F("[CONNECTED]"));
  Serial.print("[IP ");
  Serial.print(WiFi.localIP());
  Serial.println("]");
  startTime = millis();  

  // start a server
  server.begin();
  Serial.println("Server started");



}  // void setup()

// send warning email for storage 
void SendWarningEmail() {
  if (sendEmail) {
    sendEmail = false;
    HTTPClient http;
    String url = "http://" + String(serverAddress) + ":" + String(serverPort) + apiEndpoint + charge;
    WiFiClient client;

    http.begin(client, url);

    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String payload = http.getString();
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println("Response payload: " + payload);
    } else {
      Serial.println("HTTP Request failed");
    }

    http.end();
  }
}

// measure the distance in storage
void ultrasonic() {
  digitalWrite(trig, LOW);           
  delayMicroseconds(2);              
  digitalWrite(trig, HIGH);          
  delayMicroseconds(10);             
  digitalWrite(trig, LOW);           
  duration = pulseIn(echo, HIGH);    
  distance = duration * 0.0343 / 2;  
  charge = 100 - ((distance * 100) / 20);
  if (charge < 0) {
    charge = 0;
  }
  if (charge < 20) {
    digitalWrite(LED_Pin, HIGH);
    SendWarningEmail();
  } else {
    digitalWrite(LED_Pin, LOW);
  }
  Serial.print("distance: ");
  Serial.println(distance);
}

// check the boul status(empty or full)
void boul() {
  ldrValue = analogRead(ldrPin);
  Serial.print("ldr Value: ");
  Serial.println(ldrValue);
  if (ldrValue > 400) {
    boulStatus = 0;
  } else {
    boulStatus = 1;
  }
}

// open the gate using sg90
void feed() {
  if (boulStatus == 0) {
    if (charge != 0) {
      myservo.write(180);
      // digitalWrite(LED_Pin, HIGH);
      delay(2000);
      myservo.write(0);
      // digitalWrite(LED_Pin, LOW);
    }
  } else {
  }
}


// send data to web every 5 second
void sendValues(WiFiClient client) {
  client.print(header);
  client.print(manualFeed);
  client.print("|");
  client.print(charge);
  client.print("|");
  if (boulStatus == 0) {
    client.print("Empty!");
  } else {
    client.print("Not empty!");
  }
  client.print("|");
  client.print(appFeed);
  client.print("|");
  client.print(countTimer);
  Serial.println("data sent");
}


void loop() {

  unsigned long currentMillis = millis();
  myservo.write(0);
  unsigned long warningMillis = millis();
  //check if its 24 hours
  if (warningMillis - previousMillis >= intervalMail) {
    sendEmail = true;
  }

  // Check if its time to feed
  if (currentMillis - startTime >= interval) {
    startTime = currentMillis;

    
    countTimer = (countTimer % 60) + 1;
  }
  if (countTimer == 60) {

    previousFeed = currentMillis;
    ultrasonic();
    boul();
    delay(500);
    feed();
    appFeed += 1;
  }

  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) { return; }

  // Read the first line of the request
  request = client.readStringUntil('\r');

  Serial.print("request: ");
  Serial.println(request);

  // request from web 
  if (request.indexOf("getValues") > 0) {
    ultrasonic();
    boul();
    sendValues(client);
  } else if (request.indexOf("FeedOn") > 0) {
    ultrasonic();
    boul();
    if (boulStatus == 0 && charge != 0) {
      manualFeed += 1;
      feed();
      countTimer = 1;
    }
    sendValues(client);

  } else {

    client.flush();
    client.print(header);
    client.print(html_1);

    delay(5);
  }


}  // void loop()
