/****Includes****/
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <WiFiClient.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the &onfiguration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ESP8266mDNS.h>          //Allow custom URL
#include "html.h"

void setupServer();
void setupWifi();
void metronome();
void flash(int pin);
void handleRoot();
void setBpm();
void getBpm();
bool isBeat(int ms);

ESP8266WebServer server(80);
const char *ssid = "metronome_server";

long goodBeat = 0;                        // Counting the good beats
long bpm = 60;                            // Beats Per Minute
long beatCounter = 0;                     // Counting the beats for a measure (4/4 has 4 times)
const long msCountIn1Min = 60000;         // Numbers of ms in 1 sec
long waitBtwNotes = msCountIn1Min / bpm;  // time beetwen each note in ms
long lastBeatTime = 0;

// the setup routine runs once when you press reset:
void setup() {

  //setting the serial connection
  Serial.begin(9600);

  //Settings the pinout
  pinMode(D0, INPUT);   // Vibration sensor
  pinMode(D5, OUTPUT);  // B -> RGB LED
  pinMode(D4, OUTPUT);  // G -> RGB LED
  pinMode(D2, OUTPUT);  // Piezzo Speaker
  
  setupWifi();
  setupServer();
}

void loop() {

  metronome();
  server.handleClient();
}

void handleRoot() {
  server.send(200, "text/html", html_index);
}

void setBpm() {
  String bpm_str;
  if ( server.hasArg("bpm") ) {

    bpm_str = server.arg(0);
    //add function to init
    long old_bpm = bpm;
    bpm = bpm_str.toInt();

    if (bpm <= 0 || bpm > 250)  {
      bpm = old_bpm;
      server.send(400, "text/html", "error");
      return;
    }

    waitBtwNotes = msCountIn1Min / bpm;
  }

  server.send(200, "text/html", "OK");
}

void getBpm() {
  server.send(200, "text/html", String(bpm));
}

void setupWifi() {
  
  Serial.println("Starting wifi");
  WiFiManager wifiManager;
  wifiManager.setAPCallback([](WiFiManager * manager) {});

  wifiManager.autoConnect(ssid);
  Serial.println("local ip");
  Serial.println(WiFi.localIP());
  Serial.println("Wifi is setup");
}

void setupServer() {
  server.on("/", handleRoot);
  server.on("/setbpm", setBpm);
  server.on("/getbpm", getBpm);
  server.begin();
  Serial.println("HTTP server started");
}

void metronome() {

  long beatInterval = millis() - lastBeatTime;
  
  if (beatInterval >= waitBtwNotes) {

    lastBeatTime = millis();

    if (beatCounter % 4 == 0) {
      tone(D2, 440, 50);
      digitalWrite(D4, HIGH);
    }
    else {
      tone(D2, 300, 50);
      digitalWrite(D5, HIGH);
    }

    if (isBeat(100)) {
      goodBeat++;
    }

    digitalWrite(D5, LOW);
    digitalWrite(D4, LOW);

    beatCounter++;
    
    if (beatCounter > 3) {
      beatCounter = 0;
    }
  }

}

//checks if the sensors sensed a vibrating within the interval
bool isBeat(int ms) {

  long currentTime = millis();
  long sensorValue = 0;

  while (millis() <= currentTime + ms) {

    if (digitalRead(D0) == 1) {
      sensorValue = 1;
    }
  }

  return sensorValue == 1;
}





