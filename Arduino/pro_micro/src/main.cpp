/*
  3rd year Project, Written by B00125142 Violet Concordia
*/

#include<Arduino.h>
#include<ArduinoJson.h>

#include<Wire.h>

#include<Keyboard.h>
#include<Mouse.h>

#include<MemoryFree.h>

#define SERIAL_LENGTH_MAX 1024
#define EEPROM_ADDRESS_IC2 0x50

#define DEBUG_1
#define DEBUG_2
//#define COUNT_MS
//#define JSON_ERROR_CHECK
//#define DYNAMICALLY_OPTIMIZE_JSON
//#define EEPROM_DEBUG_1
#define ROWS 4
#define COLS 6
#define BUTTON_COUNT 24
#define BUTTON_INFO_SIZE_ALLOCATION 2
#define INFO_OFFSET BUTTON_INFO_SIZE_ALLOCATION*BUTTON_COUNT
#define BUTTON_ALLOCATION 2096

//int rows, cols, buttonCount;

unsigned long lastSend;
boolean button = true, wasButton = true;

void processSerial(const char inputChars[]);
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
void sendHID(const char chars[], uint16_t gapMs);
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
    button = PINF & 1 << 5;
    if(button && !wasButton)
        processButton(0);
    wasButton = button;

    if(lastSend + 4200 < millis())
    {
        Serial.print(freeMemory());
        lastSend = millis();
    }

    while (Serial.available())
    {
        PORTB |= (1 << 5);
        String inputString = Serial.readString();
        Serial.println(inputString.length());
        Serial.println(" Bytes of Data Received...");
        int size = inputString.length()+1;
        char inputChars[size];
        //We will be dealing with arrays - convert the string to a char array
        inputString.toCharArray(inputChars, size);
        //Further handle the input Serial
        processSerial(inputChars);
        PORTB &= ~(1 << 5);
    }
}

void processSerial(const char inputChars[])
{
#ifdef DEBUG_2
    int size = getStringLength(inputChars);
#endif
    //Parse the Serial input into a json
    DynamicJsonDocument json = stringToJsonObject(inputChars);
    if(json.containsKey("i"))
    {
#ifdef DYNAMICALLY_OPTIMIZE_JSON
        //Free up the memory used by the char array
        delete inputChars;
        char tidiedJsonChars[SERIAL_LENGTH_MAX];
        serializeJson(json, tidiedJsonChars);
        saveButtonData(json["i"], reinterpret_cast<const byte*>(tidiedJsonChars));
#else
        saveButtonData(json["i"], reinterpret_cast<const byte*>(inputChars));
#endif
    }
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
    greetEeprom(address);   //Tell the EEPROM which address we are dealing with

    Wire.write(data);       //Tell the EEPROM what data we want written to the address
    Wire.endTransmission(); //Tell EEPROM the command is finished

    delay(5);           //Delay 5ms (The datasheet states a write takes a maximum of 5ms)
}

void readData(uint16_t address, byte buffer[], int size)
{
    while(size-- >= 0)  //For every specified count
    //Store the read data into the array's pointer's address
    //And then increment the array's pointer and the address
        *buffer++ = readData(address++);
}

void readData(uint16_t address, char buffer[], int size)
{
    while(size-- >= 0)  //For every specified count
    //Store the read data into the array's pointer's address
    //And then increment the array's pointer and the address
        *buffer++ = (char) readData(address++);
}

byte readData(uint16_t address)
{
    byte data = 0xFF;                           //Initialize read data as 255
    greetEeprom(address);                       //Tell EEPROM which piece of address is needed
    Wire.endTransmission();                     //Tell EEPROM the command is finished
    Wire.requestFrom(EEPROM_ADDRESS_IC2, 1);    //Request the data specified
    data = Wire.read();                         //Retrieve the data
    return data;                                //Return the read data
}

void greetEeprom(uint16_t address)
{
    Wire.beginTransmission(EEPROM_ADDRESS_IC2);    //Begin Transmission
    Wire.write((int)(address >> 8));            //Send Most Significant Byte
    Wire.write((int)(address & 0xff));          //Send Least Significant Byte
}

uint16_t getStringLength(const char inputChars[])
{
    int i = 0;                      //Initialize length at 0
    while(inputChars[i] != '\0')    //For every item in the array
        ++i;                        //Increment length
    return i;                       //Return length
}

void sendHID(const char chars[])
{
    const uint16_t length = getStringLength(chars); //Determine length of string
    for(int i = 0; i < length; ++i)                          //For every char
        Keyboard.write(chars[i]);                         //Write the char
}

void sendHID(const char chars[], const uint16_t gapMs)
{
#ifdef DEBUG_1
    Serial.print("Sending HID: ");
    Serial.println(chars);
#endif
    const uint16_t length = getStringLength(chars); //Determine length of string
    for(int i = 0; i < length; ++i)                          //For every char
    {
        Keyboard.write(chars[i]);                          //Write the char
        delay(gapMs);                                     //Wait specified amount of ms
    }
}

uint16_t getButtonAddress(uint8_t buttonIndex)
{
    //Do not allow allocating buttons beyond what the keyboard supports
    if(buttonIndex >= BUTTON_COUNT)
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
    Serial.print("Write was successful. Button ");
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
    uint16_t address = getButtonSizeAddress(buttonIndex);  //Determine where button size is stored
    uint16_t readSize = merge8To16(readData(address),   //Read two bytes as one unsigned int16
                                   readData(address + 1));
    return readSize < SERIAL_LENGTH_MAX ? readSize : SERIAL_LENGTH_MAX; //Return read value, capped at max serial length
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
//    Serial.print("capacity: ");
//    Serial.println(doc.capacity());
    if(doc.capacity() == 0)
        Serial.println("ERROR: 0 capacity json, ran out of memory!");
#ifdef JSON_ERROR_CHECK
    DeserializationError jsonError = deserializeJson(doc, inputChars);
    Serial.println("this?");
    if(jsonError)
    {
        switch (jsonError.code())
        {
            case DeserializationError::InvalidInput:
            case DeserializationError::NoMemory:
                Serial.print("ERROR: ");
                Serial.println(jsonError.c_str());
                DynamicJsonDocument json = DynamicJsonDocument(0).as<JsonObject>();
                return json;
        }
    }
#endif
    deserializeJson(doc, inputChars);
    return doc;
}

void actionJsonToHID(DynamicJsonDocument &json)
{
    if(json.containsKey("t"))
    {
        const char type = json.getMember("t").as<String>().charAt(0);
        Serial.print("type: ");
        Serial.println(type);
        switch(type)
        {
            case 'w':   //Handle Write action
            {
                if(json.containsKey("v"))
                {
                    const String str = json.getMember("v").as<String>();
                    const int keyDelay = json.containsKey("d") ? json.getMember("d").as<int>() : 0;
                    for(char c : str)
                    {
                        if(keyDelay)
                            delay(keyDelay);
                        Keyboard.write(c);
                    }
                }
                break;
            }

            case 'k':   //Handle Key action
            {
                if(json.containsKey("v"))
                {
                    const int key = json.getMember("v").as<int>();
                    Keyboard.press(key);
                    if(json.containsKey("d"))
                        delay(json.getMember("d").as<int>());
                    Keyboard.release(key);
                }
            }
                break;

            case 'c':   //Handle Click action
            {
                char mouseButton = MOUSE_LEFT;
                int times = 1;
                int clickDelay = 0;
                if(json.containsKey("v"))
                {
                    const String str = json.getMember("v").as<String>();
                    if(str.length() > 0)
                    {
                        switch(str.charAt(0))
                        {
                            case '1':
                                mouseButton = MOUSE_RIGHT;
                                break;

                            case '2':
                                mouseButton = MOUSE_MIDDLE;
                                break;
                        }
                    }
                }
                if(json.containsKey("x"))
                    times = json.getMember("x").as<int>();
                if(json.containsKey("d"))
                    clickDelay = json.getMember("d").as<int>();
                if(times < 1)
                    times = 1;
                while(times-- > 0)
                {
                    Mouse.begin();
                    Mouse.click(mouseButton);
                    delay(clickDelay);
                    Mouse.end();
                }
                break;
            }

            case 'm':   //Handle Mouse Move action

                break;
        }
    }
}

void buttonJsonToHID(DynamicJsonDocument &json)
{
    JsonArray actions =  json["a"];             //Retrieve list of actions
    for (DynamicJsonDocument action : actions)  //For every action
        actionJsonToHID(action);             //Process the action
}

void processButton(uint8_t buttonIndex)
{
//    Serial.print("Processing Button ");
//    Serial.println(buttonIndex);

#ifdef COUNT_MS
    unsigned long startMicros = micros();
#endif
    uint16_t size = readButtonSize(buttonIndex);    //Determine how much memory is needed to parse the saved data
//    Serial.print("size: ");
//    Serial.println(size);
    byte loadedData[size];                          //Allocate memory for the data to be read into
    readButtonData(buttonIndex, loadedData); //Read the data into the allocated memory



//    Serial.print("loadedData: ");
//    Serial.println(reinterpret_cast<const char*>(loadedData));

    //Parse the json

#ifdef COUNT_MS
    Serial.print("data read ms: ");
    Serial.println(((float)micros()-(float)startMicros)/(float)1000.0);
    startMicros = micros();
#endif
    DynamicJsonDocument json = stringToJsonObject(reinterpret_cast<const char*>(loadedData));


//    Serial.print("loaded json: ");
//    serializeJson(json, Serial);
//    Serial.println();

#ifdef COUNT_MS
    Serial.print("json parse ms: ");
    Serial.println(((float)micros()-(float)startMicros)/(float)1000.0);
    startMicros = micros();
#endif
    buttonJsonToHID(json);
#ifdef COUNT_MS
    Serial.print("HID send ms: ");
    Serial.println(((float)micros()-(float)startMicros)/(float)1000.0);
#endif
}