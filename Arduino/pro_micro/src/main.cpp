/*
  3rd year Project, Written by B00125142 Violet Concordia
*/

#include<Arduino.h>
#include<ArduinoJson.h>

#include<Wire.h>

#include<Keyboard.h>

#define SERIAL_LENGTH_MAX 512
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

unsigned long lastSend;
boolean button, wasButton;

void processSerial(const char inputChars[]);
void debugPrint();
void writeData(uint16_t address, byte data);
void writeData(uint16_t address, const byte data[], int size);
void writeData(uint16_t address, const char data[], int size);
byte readData(uint16_t address);
void readData(uint16_t address, byte buffer[], int size);
void greetEeprom(uint16_t address);
void saveButtonData(uint8_t buttonIndex, const byte data[]);
void readButtonData(uint8_t buttonIndex, byte buffer[], int size);
void readButtonData(uint8_t buttonIndex, byte buffer[]);
uint16_t readButtonSize(uint8_t buttonIndex);
void sendHID(const char chars[]);
void sendHID(const char chars[], uint16_t delay);
uint16_t getButtonAddress(uint8_t buttonIndex);
uint16_t getButtonSizeAddress(uint8_t buttonIndex);
uint16_t getStringLength(const char inputChars[]);
DynamicJsonDocument stringToJsonObject(const char inputChars[]);
void actionJsonToHID(DynamicJsonDocument &json);
void buttonJsonToHID(DynamicJsonDocument &json);
void processButton(uint8_t buttonIndex);

uint16_t merge8To16(uint8_t a, uint8_t b);

char debugMsg[64];

void setup()
{
    Serial.begin(115200);
    Serial.setTimeout(200);

    Wire.begin();
//    writeData(0, "32511523", 8);

    DDRF |= (1 << 5);
//    PORTB &= ~(1 << 2);

//    pinMode(A10, INPUT);
}

void loop()
{
    if(millis() % 5000 == 0)
    {
//        readData(0, debugMsg, 8);
//        Serial.println(millis());
    }

    button = PINF & 1 << 5;
    if(button && !wasButton)
        processButton(0);
    wasButton = button;

    if(lastSend + 6969 < millis())
    {
        Serial.println("I'm alive");
        lastSend = millis();
    }

    while (Serial.available())
    {
        PORTB |= (1 << 5);
        Serial.println("Why Hello There");
        String inputString = Serial.readString();
        int size = inputString.length()+1;
        char inputChars[size];
        inputString.toCharArray(inputChars, size);
        delete &inputString;
        processSerial(inputChars);
        PORTB &= ~(1 << 5);
    }
}

void processSerial(const char inputChars[])
{
#ifdef DEBUG_2
    Serial.println("chars:");
    Serial.println(inputChars);
    int size = getStringLength(inputChars);
    Serial.print("length: ");
    Serial.println(size);
#endif
    DynamicJsonDocument json = stringToJsonObject(inputChars);
    delete inputChars;
    serializeJson(json, Serial);
    Serial.println();
    if(json.containsKey("i"))
    {
        char tidiedJsonChars[SERIAL_LENGTH_MAX];
        serializeJson(json, tidiedJsonChars);
        saveButtonData(json["i"], reinterpret_cast<const byte*>(tidiedJsonChars));
    }
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

void writeData(uint16_t address, const char data[], int size)
{
    while(size-- >= 0)
        writeData(address++, *data++);
}

void writeData(uint16_t address, const byte data[], int size)
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

void readData(uint16_t address, byte buffer[], int size)
{
    while(size-- >= 0)
        *buffer++ = readData(address++);
}

void readData(uint16_t address, char buffer[], int size)
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

void sendHID(const char chars[])
{
    const uint16_t length = getStringLength(chars);
    for(int i = 0; i < length; ++i)
        Keyboard.write(chars[i]);
}

void sendHID(const char chars[], const uint16_t gapMs)
{
#ifdef DEBUG_1
    Serial.print("Sending HID: ");
    Serial.println(chars);
#endif
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

void saveButtonData(uint8_t buttonIndex, const byte data[])
{
#ifdef DEBUG_1
    Serial.print("Saving Button ");
    Serial.print(buttonIndex);
    Serial.println(": ");
    Serial.println(reinterpret_cast<const char *>(data));
#endif
    int size = (int) getStringLength(reinterpret_cast<const char*>(data));
    writeData(getButtonAddress(buttonIndex), data, size);
    unsigned int buttonSizeAddress = getButtonSizeAddress(buttonIndex);
    //Save button data length as two bytes
    writeData(buttonSizeAddress, size & 0xff);
    writeData(buttonSizeAddress+1, size >> 8);
#ifdef DEBUG_1
    Serial.print("Button ");
    Serial.print(buttonIndex);
    Serial.println(" saved.");
#endif
}

void readButtonData(uint8_t buttonIndex, byte buffer[], int size)
{
    readData(getButtonAddress(buttonIndex), buffer, size);
}

void readButtonData(uint8_t buttonIndex, byte buffer[])
{
    readData(getButtonAddress(buttonIndex), buffer, readButtonSize(buttonIndex));
}

uint16_t readButtonSize(uint8_t buttonIndex)
{
    uint16_t address = getButtonSizeAddress(buttonIndex);
    uint16_t readSize = merge8To16(readData(address),
                                   readData(address + 1));
    return readSize < SERIAL_LENGTH_MAX ? readSize : SERIAL_LENGTH_MAX;
}

uint16_t getButtonSizeAddress(uint8_t buttonIndex)
{
    return buttonIndex * BUTTON_INFO_SIZE_ALLOCATION;
}

uint16_t merge8To16(uint8_t a, uint8_t b)
{
    return a | ((uint16_t) b << 8);
}

DynamicJsonDocument stringToJsonObject(const char inputChars[])
{
    DynamicJsonDocument doc(SERIAL_LENGTH_MAX);
#ifdef DEBUG_1
    Serial.print("capacity: ");
    Serial.println(doc.capacity());
#endif

    const DeserializationError err = deserializeJson(doc, inputChars);
    if(err)
    {
        switch (err.code())
        {
            case DeserializationError::InvalidInput:
            case DeserializationError::NoMemory:
                Serial.print("ERROR: ");
                Serial.println(err.c_str());
                DynamicJsonDocument json = DynamicJsonDocument(0).as<JsonObject>();
                return json;
        }
    }
//#ifdef DEBUG_2
//    Serial.println("stringToJsonObject:");
//    serializeJson(doc, Serial);
//    Serial.println();
//#endif
    deserializeJson(doc, inputChars);
    return doc;
}

void actionJsonToHID(DynamicJsonDocument &json)
{
    if(json.containsKey("t"))
    {
        const char type = json.getMember("t").as<String>().charAt(0);
        switch(type)
        {
            case 'w':
                if(json.containsKey("s"))
                {
                    delay(1000);
                    const String str = json.getMember("s").as<String>();
                    const int keyDelay = json.containsKey("d") ? json.getMember("d").as<int>() : 0;
                    for(char c : str)
                    {
                        if(keyDelay)
                            delay(keyDelay);
                        Keyboard.write(c);
                    }
                }
                break;

            case 'k':

                break;

            case 'c':

                break;

            case 'm':

                break;
        }
    }
}

void buttonJsonToHID(DynamicJsonDocument &json)
{
    JsonArray actions =  json["a"];
    for (DynamicJsonDocument action : actions)
        actionJsonToHID(action);
}

void processButton(uint8_t buttonIndex)
{
    Serial.print("Processing Button ");
    Serial.println(buttonIndex);
    uint16_t size = readButtonSize(buttonIndex);
    Serial.print("size: ");
    Serial.println(size);
    byte loadedData[size];
    readButtonData(buttonIndex, loadedData);
    Serial.print("loadedData: ");
    Serial.println(reinterpret_cast<const char*>(loadedData));
    DynamicJsonDocument json = stringToJsonObject(reinterpret_cast<const char*>(loadedData));
    serializeJson(json, Serial);
    Serial.println();
    buttonJsonToHID(json);
}