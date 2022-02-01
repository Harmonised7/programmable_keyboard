/*
  3rd year Project, Written by B00125142 Violet Concordia
*/

#include<Arduino.h>
#include<ArduinoJson.h>

#include<Wire.h>

#include<Keyboard.h>

#define SERIAL_LENGTH_MAX 16
#define EEPROM_ADDRESS_IC2 0x50

#define DEBUG_1
#define DEBUG_2
//#define EEPROM_DEBUG_1
#define ROWS 4
#define COLS 6
#define BUTTON_COUNT 24
#define BUTTON_INFO_SIZE_ALLOCATION 2
#define INFO_OFFSET BUTTON_INFO_SIZE_ALLOCATION*BUTTON_COUNT
#define BUTTON_ALLOCATION 2096

//int rows, cols, buttonCount;

long lastSend;

void processSerial(const char inputChars[]);
void debugPrint();
void writeData(uint16_t address, byte data);
void writeData(uint16_t address, const byte data[], uint16_t size);
void writeData(uint16_t address, const char data[], uint16_t size);
byte readData(uint16_t address);
void readData(uint16_t address, byte buffer[], uint16_t size);
void greetEeprom(uint16_t address);
void saveButtonData(unsigned buttonIndex, const byte data[]);
void readButtonData(unsigned buttonIndex, byte buffer[], uint16_t size);
void readButtonData(unsigned buttonIndex, byte buffer[]);
uint16_t readButtonSize(unsigned buttonIndex);
void sendHID(const char chars[]);
void sendHID(const char chars[], uint16_t delay);
uint16_t getButtonAddress(uint8_t buttonIndex);
uint16_t getButtonSizeAddress(unsigned buttonIndex);
uint16_t getStringLength(const char inputChars[]);
uint16_t getStringLength(const byte input[]);

uint16_t merge8To16(uint8_t a, uint8_t b);

char debugMsg[64];

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
    if(millis() % 5000 == 0)
    {
//        readData(0, debugMsg, 8);
//        Serial.println(millis());
    }
    while (Serial.available())
    {
        PORTB |= (1 << 5);
        Serial.println("Why Hello There");
        String inputString = Serial.readString();
        uint16_t size = inputString.length()+1;
        char inputChars[size];
        inputString.toCharArray(inputChars, size);
//        processSerial(inputString);
        PORTB &= ~(1 << 5);
    }
}

void processSerial(const char inputChars[])
{
#ifdef DEBUG_2
    Serial.println("chars:");
    Serial.println(inputChars);
    uint16_t size = getStringLength(inputChars);
//    for(int i = 0; i < size; ++i)
//    {
//        Serial.print(i);
//        Serial.print(": ");
//        Serial.println(inputChars[i]);
//    }
    Serial.print("length: ");
    Serial.println(size);
#endif

    DynamicJsonDocument doc(SERIAL_LENGTH_MAX);
#ifdef DEBUG_1
    Serial.print("capacity: ");
    Serial.println(doc.capacity());
#endif

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

    Serial.println(doc.containsKey("info"));
    if(doc.containsKey("info"))
    {
//        rows = doc["info"].getMember("row").as<int>();
//        cols = doc["info"].getMember("col").as<int>();
//        buttonCount = rows * cols;

#ifdef DEBUG_2
        Serial.println("info parsed");
#endif
    }

    if(doc.containsKey("buttons"))
    {
        JsonObject buttonsJson = json["buttons"];
        for(int i = 0; i < BUTTON_COUNT; i++)
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
//    Serial.print("Count: ");
//    Serial.println(buttonCount);
//
//    Serial.print("Rows: ");
//    Serial.println(rows);
//
//    Serial.print("Cols: ");
//    Serial.println(cols);
}

void writeData(uint16_t address, const char data[], uint16_t size)
{
    while(size-- >= 0)
        writeData(address++, *data++);
}

void writeData(uint16_t address, const byte data[], uint16_t size)
{
    while(size-- >= 0)
        writeData(address++, *data++);
}

void writeData(uint16_t address, byte data)
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

void readData(uint16_t address, byte buffer[], uint16_t size)
{
    while(size-- >= 0)
        *buffer++ = readData(address++);
}

void readData(uint16_t address, char buffer[], uint16_t size)
{
    while(size-- >= 0)
        *buffer++ = (char) readData(address++);
}

byte readData(uint16_t address)
{
    byte data = 0xFF;
    greetEeprom(address);
    Wire.endTransmission();
    Wire.requestFrom(EEPROM_ADDRESS_IC2, 1);
    data = Wire.read();
    return data;
}

void greetEeprom(uint16_t address)
{
    Wire.beginTransmission(EEPROM_ADDRESS_IC2);
    Wire.write((int)(address >> 8));   // Most Significant Byte
    Wire.write((int)(address & 0xff)); // Least Significant Byte
}

uint16_t getStringLength(const char inputChars[])
{
    int i = 0;
    while(inputChars[i] != '\0')
        ++i;
    return i;
}

uint16_t getStringLength(const byte input[])
{
    uint16_t i = 0;
    unsigned const int arraySize = sizeof(input);
    while(input[i] != '\0' && i < arraySize)
        ++i;
    return i;
}

void sendHID(const char chars[])
{
    const uint16_t length = getStringLength(chars);
    for(int i = 0; i < length; ++i)
        Keyboard.write(chars[i]);
}

void sendHID(const char chars[], const uint16_t gapMs)
{
    const uint16_t length = getStringLength(chars);
    for(int i = 0; i < length; ++i)
    {
        Keyboard.write(chars[i]);
        delay(gapMs);
    }
}

uint16_t getButtonAddress(uint8_t buttonIndex)
{
    //Do not allow allocating buttons beyond what the keyboard supports
    if(buttonIndex > BUTTON_COUNT)
        buttonIndex = BUTTON_COUNT-1;
    return INFO_OFFSET + buttonIndex*BUTTON_ALLOCATION;
}

void saveButtonData(unsigned buttonIndex, const byte data[])
{
    writeData(getButtonAddress(buttonIndex), data, getStringLength(data));
}

void readButtonData(unsigned buttonIndex, byte buffer[], uint16_t size)
{
    readData(getButtonAddress(buttonIndex), buffer, size);
}

void readButtonData(unsigned buttonIndex, byte buffer[])
{
    readData(getButtonAddress(buttonIndex), buffer, readButtonSize(buttonIndex));
}

uint16_t readButtonSize(unsigned buttonIndex)
{
    uint16_t readSize = getButtonSizeAddress(getButtonSizeAddress(buttonIndex));
    return readSize < BUTTON_ALLOCATION ? readSize : BUTTON_ALLOCATION;
}

uint16_t getButtonSizeAddress(unsigned buttonIndex)
{
    return merge8To16(readData(buttonIndex * BUTTON_INFO_SIZE_ALLOCATION),
                      readData(buttonIndex * BUTTON_INFO_SIZE_ALLOCATION + 1));
}

uint16_t merge8To16(uint8_t a, uint8_t b)
{
    return a | ((uint16_t) b << 8);
}