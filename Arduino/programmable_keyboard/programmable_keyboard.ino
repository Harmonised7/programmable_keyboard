#include <ArduinoJson.h>

#define LED 13

//const String json = "[1, 2, 3, 4, 5]";

const char* input = "{\"key\": \"value\"}";

void setup()
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
}

void loop()
{
  
  if(Serial.available())
  {
    digitalWrite(LED, HIGH);
    String serialString = Serial.readString();
    Serial.println(serialString);

    DynamicJsonDocument doc(65535);
    DeserializationError err = deserializeJson(doc, input);

    serializeJson(doc, Serial);
    
    digitalWrite(LED, LOW);
  }
}
