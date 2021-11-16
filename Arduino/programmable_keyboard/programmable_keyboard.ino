/*
  3rd year Project, Written by B00125142 Violet Concordia
*/

#include<ArduinoJson.h>

#define SERIAL_LENGTH_MAX 1024
#define DEBUG

int rows, cols, buttonCount;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  while (Serial.available())
  {
    String inputString = Serial.readString();
    Serial.println("Read in");
    Serial.println(inputString);
    parseAndLoadCharArray(inputString);
  }
}

void parseAndLoadCharArray(const String inputString)
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
  Serial.println("Read in json:");
  serializeJson(doc, Serial);
  Serial.println();
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
