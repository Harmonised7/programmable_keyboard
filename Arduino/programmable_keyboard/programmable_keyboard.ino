/*
  3rd year Project, Written by B00125142 Violet Concordia
*/

#include<ArduinoJson.h>

#define SERIAL_LENGTH_MAX 1024
//#define DEBUG

int rows, cols, buttonCount;

long lastSend;

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(200);
  DDRB |= (1 << 5);
  PORTB &= ~(1 << 5);
}

void loop()
{
//  if(millis() - lastSend > 5000)
//  {
//    Serial.println("Contrary to popular belief, Lorem Ipsum is not simply random text. It has roots in a piece of classical Latin literature from 45 BC, making it over 2000 years old. Richard McClintock, a Latin professor at Hampden-Sydney College in Virginia, looked up one of the more obscure Latin words, consectetur, from a Lorem Ipsum passage, and going through the cites of the word in classical literature, discovered the undoubtable source. Lorem Ipsum comes from sections 1.10.32 and 1.10.33 of 'de Finibus Bonorum et Malorum' (The Extremes of Good and Evil) by Cicero, written in 45 BC. This book is a treatise on the theory of ethics, very popular during the Renaissance. The first line of Lorem Ipsum, 'Lorem ipsum dolor sit amet..', comes from a line in section 1.10.32.");
//    lastSend = millis();
//  }
  
  while (Serial.available())
  {
    PORTB |= (1 << 5);
    String inputString = Serial.readString();
//    Serial.println("Read in");/
    Serial.println(inputString);
    parseAndLoadStringAsJson(inputString);
    delay(300);
    PORTB &= ~(1 << 5);
  }
}

void parseAndLoadStringAsJson(const String inputString)
{
  #ifdef DEBUG
  Serial.println("string");
  Serial.println(inputString);
  #endif
  const int inputLength = inputString.length();
  const char inputChars[inputLength];
  inputString.toCharArray(inputChars, inputLength);
  #ifdef DEBUG
  Serial.println("chars");
  Serial.println(inputString);
  #endif

  DynamicJsonDocument doc(SERIAL_LENGTH_MAX);
  DeserializationError err = deserializeJson(doc, inputChars);

  if (err)
  {
    switch (err.code())
    {
      case DeserializationError::InvalidInput:
      case DeserializationError::NoMemory:
        Serial.print("ERROR: ");
        Serial.println(err.c_str());
        return;
    }
  }
  deserializeJson(doc, inputChars);
  JsonObject json = doc.as<JsonObject>();

  #ifdef DEBUG
//  Serial.println("Read in json:");
//  serializeJson(doc, Serial);
//  Serial.println();
  #endif

  if(doc.containsKey("buttons"))
  {
    
    for(JsonPair obj : json)
    {
      #ifdef DEBUG
      Serial.print("parsed button ");
//      Serial.println(obj.getKey());
      #endif
    }
  }
  if (doc.containsKey("info"))
  {
    rows = doc["info"].getMember("row").as<int>();
    cols = doc["info"].getMember("col").as<int>();
    buttonCount = rows * cols;
    
    #ifdef DEBUG
    Serial.println("info parsed");
    #endif
  }

  #ifdef DEBUG
  debugPrint();
  #endif
}

void debugPrint()
{
  Serial.print("Count: ");
  Serial.println(buttonCount);

  Serial.print("Rows: ");
  Serial.println(rows);

  Serial.print("Cols: ");
  Serial.println(cols);
}
