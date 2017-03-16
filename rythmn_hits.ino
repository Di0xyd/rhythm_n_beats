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
bool beatCheck();

ESP8266WebServer server(80);
const char *ssid = "metronome_server";

long goodBeat = 0;
long badBeat = 0;
long bpm = 60;
long beatCounter = 0; // % 4 == 0 = blue (premier temps) / sinon autre-
const long msCountIn1Min = 60000;
long waitBtwNotes = msCountIn1Min / bpm;
long lastTime = 0;

// the setup routine runs once when you press reset:
void setup() {

  Serial.begin(9600);
  pinMode(D0, INPUT);
  pinMode(D5, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D3, OUTPUT);
  Serial.println("Starting WiFi.");
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

  WiFiManager wifiManager;
  //wifiManager.resetSettings();
  wifiManager.setAPCallback([](WiFiManager * manager) {});

  wifiManager.autoConnect(ssid);
  Serial.println("local ip");
  Serial.println(WiFi.localIP());
}

void setupServer() {
  server.on("/", handleRoot);
  server.on("/setbpm", setBpm);
  server.on("/getbpm", getBpm);
  server.begin();
  Serial.println("HTTP server started");
}

void metronome() {
  if ((millis() - lastTime) >= waitBtwNotes) {

    lastTime = millis();

    if (beatCounter % 4 == 0) {
      tone(D2, 440, 50);
      digitalWrite(D4, HIGH);
    }
    else {
      tone(D2, 300, 50);
      digitalWrite(D5, HIGH);
    }

    if (beatCheck()) {
      Serial.println(2000);
      goodBeat++;
    }

    digitalWrite(D5, LOW);
    digitalWrite(D4, LOW);

    beatCounter++;
    if (beatCounter > 3)
      beatCounter = 0;
  }

  if (beatCheck()) {
    Serial.println(1000);
    badBeat++;
  }
  Serial.println(1);

}

void flash(int pin) {
  digitalWrite(pin, HIGH);
  delay(20);
  digitalWrite(pin, LOW);
}

bool beatCheck() {
  long currentTime = millis();
  long sensorValue = 0;
  while (millis() <= currentTime + 50) {
    sensorValue += digitalRead(D0);
  }
  return sensorValue > 0;
}





