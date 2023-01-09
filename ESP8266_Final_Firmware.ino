/*
Author: Kanad H. Nemade
Version: V2.1
Desc: Firmware for ESP8266
Code 2 of 2
*/

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SoftwareSerial.h>
#include <SimpleTimer.h>

// char auth[] = "ezQUGZY-D7K9co19lxdH4e5pjtUSAtAG";
// ZeeBlynk server
char auth[] = "YOUR_AUTH_KEY"; // Enter your own auth code here

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Redmi";
char pass[] = "password";

SimpleTimer timer;

String myString; // complete message from arduino, which consistors of snesors data
char rdata;      // received charactors

int firstVal, secondVal, thirdVal, fourthVal, fifthVal; // sensors
// This function sends Arduino's up time every second to Virtual Pin (1).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V1, millis() / 1000);
}

void setup()
{
  // Debug console
  Serial.begin(9600);

  // Blynk.begin(auth, ssid, pass);
  // Zeeblynk Server
  Blynk.begin(auth, ssid, pass, "zeeblynk.com", 8080);

  timer.setInterval(1000L, sensorvalue1);
  timer.setInterval(1000L, sensorvalue2);
  timer.setInterval(1000L, sensorvalue3);
  timer.setInterval(1000L, sensorvalue4);
  timer.setInterval(1000L, sensorvalue5);
}

void loop()
{
  if (Serial.available() == 0)
  {
    Blynk.run();
    timer.run(); // Initiates BlynkTimer
  }

  if (Serial.available() > 0)
  {
    rdata = Serial.read();
    myString = myString + rdata;
    // Serial.print(rdata);
    if (rdata == '\n')
    {
      //  Serial.println(myString);
      // new code
      String l = getValue(myString, ',', 0);
      String m = getValue(myString, ',', 1);
      String n = getValue(myString, ',', 2);
      String o = getValue(myString, ',', 3);
      String p = getValue(myString, ',', 4);

      firstVal = l.toInt();
      secondVal = m.toInt();
      thirdVal = n.toInt();
      fourthVal = o.toInt();
      fifthVal = p.toInt();

      myString = "";
      // end new code
    }
  }
}

void sensorvalue1()
{
  int sdata = firstVal;
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, "LPG(in ppm): ", sdata);
  Blynk.virtualWrite(V20, sdata);
}
void sensorvalue2()
{
  int sdata = secondVal;
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V3, "CO(in ppm): ", sdata);
  Blynk.virtualWrite(V21, sdata);
}
void sensorvalue3()
{
  int sdata = thirdVal;
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V4, "Smoke(in ppm): ", sdata);
  Blynk.virtualWrite(V22, sdata);
}

void sensorvalue4()
{
  int sdata = fourthVal;
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, "NH4(in ppm): ", sdata);
  Blynk.virtualWrite(V23, sdata);
}

void sensorvalue5()
{
  int sdata = fifthVal;
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V6, "Benzene(in ppm): ", sdata);
  Blynk.virtualWrite(V24, sdata);
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}