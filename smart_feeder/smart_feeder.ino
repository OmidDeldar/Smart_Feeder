




#include <ESP8266WiFi.h>
#include <Servo.h>
#include "webpage.h"
// change these values to match your network
char ssid[] = "Omid";          //  your network SSID (name)
char pass[] = "oM1234567890";  //  your network password

WiFiServer server(80);
int trig = D6;
int echo = D5;
float duration, distance;  // declare two variable of type float for the time and the the distance
int charge;
String request = "";
int LED_Pin = 13;
const int ldrPin = A0;  // select the input pin for the potentiometer
int ldrValue = 0;       // variable to store the value coming from the sensor
Servo myservo;
int manualFeed = 0;
int appFeed = 0;
int boulStatus = 0;
unsigned long previousFeed = 0;    // will store last time the  action was executed
unsigned long feedTiming = 60000;  // interval for the first action in milliseconds (2 minutes)
unsigned long startTime;
int countTimer = 1;
const int interval = 1000;  // 1000 milliseconds = 1 second
void setup() {
  pinMode(LED_Pin, OUTPUT);
  myservo.attach(D1);
  Serial.begin(115200);
  Serial.println();
  Serial.println("Serial started at 115200");
  Serial.println("ESP8266_LED_CONTROL_AJAX_02");
  Serial.println();
  pinMode(trig, OUTPUT);  // initialize trig as an output
  pinMode(echo, INPUT);   // initialize echo as an input
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
  startTime = millis();  // Record the start time

  // start a server
  server.begin();
  Serial.println("Server started");

  // ultrasonic();
  // boul();
  // feed();

}  // void setup()


void ultrasonic() {
  digitalWrite(trig, LOW);           // set trig to LOW
  delayMicroseconds(2);              // wait 2 microseconds
  digitalWrite(trig, HIGH);          // set trig to HIGH
  delayMicroseconds(10);             // wait 10 microseconds
  digitalWrite(trig, LOW);           // set trig to LOW
  duration = pulseIn(echo, HIGH);    // use the function pulsein to detect the time of the echo when it is in a high state
  distance = duration * 0.0343 / 2;  // divide the time by 2 then multiply it by 0.0343
  charge = 100 - ((distance * 100) / 20);
  if (charge < 0) {
    charge = 0;
  }
  // if(charge < 20){
  //   digitalWrite(LED_Pin, HIGH);
  // }
  // else{
  //   digitalWrite(LED_Pin, LOW);
  // }
  Serial.println(distance);
}
void boul() {
  ldrValue = analogRead(ldrPin);
  Serial.println(ldrValue);
  if (ldrValue > 190) {
    boulStatus = 0;
  } else {
    boulStatus = 1;
  }
}

void feed() {
  if (boulStatus == 0) {
    myservo.write(180);
    digitalWrite(LED_Pin, HIGH);
    delay(2000);
    myservo.write(0);
    digitalWrite(LED_Pin, LOW);
  } else {
  }
}
void loop() {

  unsigned long currentMillis = millis();

  // Check if it's time to perform the first action
  if (currentMillis - startTime >= interval) {
    // Reset the timer
    startTime = currentMillis;



    // Increment the countTimer and reset to 1 if it reaches 60
    countTimer = (countTimer % 60) + 1;
  }
  if (countTimer == 60) {
    // Save the current time

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
  if (request.indexOf("getValues") > 0) {
    ultrasonic();
    boul();
    if (boulStatus == 0) {
      client.print(header);
      client.print(manualFeed);
      client.print("|");
      client.print(charge);
      client.print("|");
      client.print("Empty!");
      client.print("|");
      client.print(appFeed);
      client.print("|");
      client.print(countTimer);
      Serial.println("data sent");
    } else {
      client.print(header);
      client.print(manualFeed);
      client.print("|");
      client.print(charge);
      client.print("|");
      client.print("Not empty!");
      client.print("|");
      client.print(appFeed);
      client.print("|");
      client.print(countTimer);
      Serial.println("data sent");
    }
  } else if (request.indexOf("FeedOn") > 0) {
    ultrasonic();
    boul();
    manualFeed += 1;
    if (boulStatus == 0) {
      feed();
      countTimer = 1;
      client.print(header);
      client.print(manualFeed);
      client.print("|");
      client.print(charge);
      client.print("|");
      client.print("Empty!");
      client.print("|");
      client.print(appFeed);
      client.print("|");
      client.print(countTimer);
      Serial.println("data sent");
    } else {
      client.print(header);
      client.print(manualFeed);
      client.print("|");
      client.print(charge);
      client.print("|");
      client.print("Not empty!");
      client.print("|");
      client.print(appFeed);
      client.print("|");
      client.print(countTimer);
      Serial.println("data sent");
    }
    // charge = 100 - ((distance * 100) / 20);
    // Serial.println("charge: "+String(charge));
    // if (distance > 0) {
    //   client.print(header);
    //   client.print(distance);
    //   Serial.println("data sent");
    // }

  } else {

    client.flush();
    client.print(header);
    client.print(html_1);


    delay(5);
  }


  // The client will actually be disconnected when the function returns and 'client' object is detroyed
}  // void loop()
