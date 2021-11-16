/*
  3rd year Project, Written by B00125142 Violet Concordia
*/

#include<ArduinoJson.h>

#define SERIAL_LENGTH_MAX 65536

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  while(Serial.available())
  {
    String inputString = Serial.readString();
    const int inputLength = inputString.length();
    char inputChars[inputLength];
    inputString.toCharArray(inputChars, inputLength);

//    Serial.println(inputString);
    Serial.println(inputLength);
//    Serial.println(inputChars);
    
    DynamicJsonDocument doc(inputLength);
    DeserializationError err = deserializeJson(doc, inputChars);
    if(err)
    {
      Serial.print("ERROR: ");
      Serial.println(err.c_str());
//      return;
    }

    inputString.toCharArray(inputChars, inputLength);
    deserializeJson(doc, inputChars);

    serializeJson(doc, Serial);
  }
}
