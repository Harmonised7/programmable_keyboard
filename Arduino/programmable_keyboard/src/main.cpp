/*
  3rd year Project, Written by B00125142 Violet Concordia
*/

#include<Arduino.h>
#include<ArduinoJson.h>
#include<Wire.h>

#define SERIAL_LENGTH_MAX 1024
#define EEPROM_ADDRESS_IC2 0x50

#define DEBUG_1
#define DEBUG_2
#define EEPROM_DEBUG_1

int rows, cols, buttonCount;

long lastSend;

void parseAndLoadStringAsJson(const String inputString);
void debugPrint();
void writeData(int address, byte data);
void writeData(int address, const byte data[], int size);
void writeData(int address, const char data[], int size);
byte readData(int address);
void readData(int address, byte buffer[], int size);
void readData(int address, char buffer[], int size);
void greetEeprom(int address);

char debugMsg[128];

void setup()
{
    Serial.begin(115200);
    Serial.setTimeout(200);

    Wire.begin();
    writeData(0, "32511523", 8);

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
    if(millis() % 1000 == 0)
    {
        readData(0, debugMsg, 8);
        Serial.println(debugMsg);
    }
    while (Serial.available())
    {
        PORTB |= (1 << 5);
        String inputString = Serial.readString();
        parseAndLoadStringAsJson(inputString);
        delay(300);
        PORTB &= ~(1 << 5);
    }
}

void parseAndLoadStringAsJson(const String inputString)
{
    #ifdef DEBUG_1
    Serial.println("string");
    Serial.println(inputString);
    #endif
    const int inputLength = inputString.length();
    char inputChars[inputLength];
    inputString.toCharArray(inputChars, inputLength);
    #ifdef DEBUG_2
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

    #ifdef DEBUG_1
    Serial.println("Read in json:");
    serializeJson(doc, Serial);
    Serial.println();
    #endif

    #ifdef DEBUG_2
    Serial.println("Parsing");
    #endif

    if (doc.containsKey("info"))
    {
        rows = doc["info"].getMember("row").as<int>();
        cols = doc["info"].getMember("col").as<int>();
        buttonCount = rows * cols;

        #ifdef DEBUG_2
        Serial.println("info parsed");
        #endif
    }

    if(doc.containsKey("buttons"))
    {
        JsonObject buttonsJson = json["buttons"];
        for(int i = 0; i < buttonCount; i++)
        {
            String key = String(i);
            if(buttonsJson.containsKey(key))
            {
                #ifdef DEBUG_2
                JsonObject buttonJson = buttonsJson[key];
                Serial.print("parsed button ");
                Serial.println(i);
                #endif
            }
        }
    }

    #ifdef DEBUG_1
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

void writeData(int address, const char data[], int size)
{
    while(size-- >= 0)
    {
        Serial.println(size);
        writeData(address++, *data++);
    }
}

void writeData(int address, const byte data[], int size)
{
    while(size-- >= 0)
    {
        Serial.println(size);
        writeData(address++, *data++);
    }
}

void writeData(int address, byte data)
{
    #ifdef EEPROM_DEBUG_1
    sprintf(debugMsg, "writing %c to %d", data, address);
    Serial.println(debugMsg);
    #endif
    greetEeprom(address);

    Wire.write(data);
    Wire.endTransmission();

    delay(5);
}

void readData(int address, byte buffer[], int size)
{
    while(size-- >= 0)
        *buffer++ = readData(address++);
}

void readData(int address, char buffer[], int size)
{
    while(size-- >= 0)
        *buffer++ = (char) readData(address++);
}

byte readData(int address)
{
    byte data = 0xFF;
    greetEeprom(address);
    Wire.endTransmission();
    Wire.requestFrom(EEPROM_ADDRESS_IC2, 1);
    data = Wire.read();
    return data;
}

void greetEeprom(int address)
{
    Wire.beginTransmission(EEPROM_ADDRESS_IC2);
    Wire.write((int)(address >> 8));   // Most Significant Byte
    Wire.write((int)(address & 0xff)); // Least Significant Byte
}