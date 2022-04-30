/*
  3rd year Project, Written by B00125142 Violet Concordia
*/

#include<Arduino.h>
#include<ArduinoJson.h>

#include<Wire.h>

#include<Keyboard.h>
#include<Mouse.h>

#define SERIAL_LENGTH_MAX 756
#define EEPROM_ADDRESS_IC2 0x50

//#define DEBUG_1
//#define DEBUG_2
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

unsigned long lastSend;
boolean button = true, wasButton = true;
boolean debounce[BUTTON_COUNT];
uint8_t buttonRemap[24] = {5, 18, 19, 20, 21, 22,
                           23, 12, 13, 14, 15, 16,
                           17, 6, 7, 8, 9, 10,
                           11, 0, 1, 2, 3, 4};

void processSerial(const char inputChars[]);
double map(double input, double inLow, double inHigh, double outLow, double outHigh);

//EEPROM
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
uint16_t getButtonAddress(uint8_t buttonIndex);
uint16_t getButtonSizeAddress(uint8_t buttonIndex);
uint16_t getStringLength(const char inputChars[]);

//JSON Processing
DynamicJsonDocument stringToJsonObject(const char inputChars[]);
void actionJsonToHID(DynamicJsonDocument &json);
void buttonJsonToHID(DynamicJsonDocument &json);
void processButton(uint8_t buttonIndex);

//Keyboard helpers
void sendHID(const char chars[]);
void sendHID(const char chars[], uint16_t gapMs);

//Bit Manipulation
void setBit(volatile uint8_t *byte, uint8_t bit, bool value);
void setBit(uint8_t *byte, uint8_t bit, bool value);
void setBitHigh(volatile uint8_t *byte, uint8_t bit);
void setBitHigh(uint8_t *byte, uint8_t bit);
void setBitLow(volatile uint8_t *byte, uint8_t bit);
void setBitLow(uint8_t *byte, uint8_t bit);
boolean readBit(volatile uint8_t *byte, uint8_t bit);
void toggleBit(volatile uint8_t *byte, uint8_t bit);
void toggleBit(uint8_t *byte, uint8_t bit);

//Shift Register
void setSR(uint8_t value);
void selectButtonSR(uint8_t column, uint8_t row);
void setSRInput(boolean value);
void clockSR();
void clockSROut();

double log(double base, double value);

uint16_t merge8To16(uint8_t a, uint8_t b);

char debugMsg[64];

/**
 * MCU Setup code - only runs once.
 */
void setup()
{
    Serial.begin(115200);
    Serial.setTimeout(200);

    Wire.begin();

    //Set Button Read pin as pull down input
    setBitLow(&DDRB, 2);
    setBitLow(&PORTB, 2);
    //Set SER as output
    setBitHigh(&DDRB, 4);
    //Set SRCLK as output
    setBitHigh(&DDRE, 6);
    //Set RCLK as output
    setBitHigh(&DDRD, 4);
    //Sed LED as output
    setBitHigh(&DDRF, 5);
//    DDRF |= (1 << 5);
//    PORTB &= ~(1 << 2);

//    pinMode(A10, INPUT);
    delay(5000);
}

/**
 * MCU program - loops forever after initialization.
 */
void loop()
{
    for(uint8_t row = 0; row < ROWS; ++row)
    {
        for(uint8_t col = 0; col < COLS; ++col)
        {
            int index = col + row*COLS;
            selectButtonSR(col, row);
            boolean buttonValue = readBit(&PINB, 2);
            if(buttonValue && !debounce[index])
                processButton(buttonRemap[index]);
            debounce[index] = buttonValue;
        }
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

/**
 * Processes the received serial data into a json
 * object and deals with it depending on its'
 * contents. In most cases, writes data into
 * the EEPROM for the specified button.
 * @param inputChars raw received serial data
 */
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

/**
 * Writes data into the EEPROM
 * @param address memory address on the EEPROM
 * @param data data to be written
 * @param size length of the data to be written
 */
void writeData(uint16_t address, const char data[], int size)
{
    while(size-- >= 0)
        writeData(address++, *data++);
}

/**
 * Writes data into the EEPROM
 * @param address memory address on the EEPROM
 * @param data data to be written
 * @param size length of the data to be written
 */
void writeData(uint16_t address, const byte data[], int size)
{
    while(size-- >= 0)
        writeData(address++, *data++);
}

/**
 * Writes data into the EEPROM
 * @param address memory address on the EEPROM
 * @param data data to be written
 */
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

/**
 * Reads data from the EEPROM
 * @param address memory address on the EEPROM
 * @param buffer buffer that will receive the read data
 * @param size length of the data to be read
 */
void readData(uint16_t address, byte buffer[], int size)
{
    while(size-- >= 0)  //For every specified count
    //Store the read data into the array's pointer's address
    //And then increment the array's pointer and the address
        *buffer++ = readData(address++);
}

/**
 * Reads data from the EEPROM
 * @param address memory address on the EEPROM
 * @param buffer buffer that will receive the read data
 * @param size length of the data to be read
 */
void readData(uint16_t address, char buffer[], int size)
{
    while(size-- >= 0)  //For every specified count
    //Store the read data into the array's pointer's address
    //And then increment the array's pointer and the address
        *buffer++ = (char) readData(address++);
}

/**
 * Reads data from the EEPROM
 * @param address memory address on the EEPROM
 * @return the data at specified address
 */
byte readData(uint16_t address)
{
    byte data = 0xFF;                           //Initialize read data as 255
    greetEeprom(address);                       //Tell EEPROM which piece of address is needed
    Wire.endTransmission();                     //Tell EEPROM the command is finished
    Wire.requestFrom(EEPROM_ADDRESS_IC2, 1);    //Request the data specified
    data = Wire.read();                         //Retrieve the data
    return data;                                //Return the read data
}

/**
 * Greets the EEPROM by specifying which address
 * the context of the next instructions will be
 * @param address memory address on the EEPROM
 */
void greetEeprom(uint16_t address)
{
    Wire.beginTransmission(EEPROM_ADDRESS_IC2);    //Begin Transmission
    Wire.write((int)(address >> 8));            //Send Most Significant Byte
    Wire.write((int)(address & 0xff));          //Send Least Significant Byte
}

/**
 * Returns the index of the string terminator
 * @param inputChars input string
 * @return index of string terminator
 */
uint16_t getStringLength(const char inputChars[])
{
    int i = 0;                      //Initialize length at 0
    while(inputChars[i] != '\0')    //For every item in the array
        ++i;                        //Increment length
    return i;                       //Return length
}

/**
 * Sends HID input for every character
 * @param chars characters to be sent as HID
 */
void sendHID(const char chars[])
{
    const uint16_t length = getStringLength(chars); //Determine length of string
    for(int i = 0; i < length; ++i)                          //For every char
        Keyboard.write(chars[i]);                         //Write the char
}

/**
 * Sends HID input for every character with specified delay
 * @param chars characters to be sent as HID
 * @param gapMs delay between each character sending
 */
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

/**
 *
 * @param buttonIndex
 * @return
 */
uint16_t getButtonAddress(uint8_t buttonIndex)
{
    //Do not allow allocating buttons beyond what the keyboard supports
    if(buttonIndex >= BUTTON_COUNT)
        buttonIndex = BUTTON_COUNT-1;
    return INFO_OFFSET + buttonIndex*BUTTON_ALLOCATION;
}

/**
 * Saves data for a specific button index
 * @param buttonIndex the index of the button
 * @param data the data to be written for this button
 */
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
    uint8_t buttonSizeAddress = getButtonSizeAddress(buttonIndex);
    //Save button data length as two bytes
    writeData(buttonSizeAddress, size & 0xff);
    writeData(buttonSizeAddress+1, size >> 8);
#ifdef DEBUG_1
    Serial.print("Write was successful. Button ");
    Serial.print(buttonIndex);
    Serial.println(" saved.");
#endif
}

/**
 * Reads specified button data into the buffer
 * @param buttonIndex the index of the button
 * @param buffer buffer that is populated with the read data
 * @param size size of the data to be read
 */
void readButtonData(uint8_t buttonIndex, byte buffer[], int size)
{
    readData(getButtonAddress(buttonIndex), buffer, size);
}

/**
 * Reads specified button data into the buffer
 * @param buttonIndex the index of the button
 * @param buffer buffer that is populated with the read data
 */
void readButtonData(uint8_t buttonIndex, byte buffer[])
{
    readData(getButtonAddress(buttonIndex), buffer, readButtonSize(buttonIndex));
}

/**
 * Reads cached size of the button
 * @param buttonIndex index of the button
 * @return size of the stored data for this button
 */
uint16_t readButtonSize(uint8_t buttonIndex)
{
    uint16_t address = getButtonSizeAddress(buttonIndex);  //Determine where button size is stored
    uint16_t readSize = merge8To16(readData(address),   //Read two bytes as one uint8_t16
                                   readData(address + 1));
    return readSize < SERIAL_LENGTH_MAX ? readSize : SERIAL_LENGTH_MAX; //Return read value, capped at max serial length
}

/**
 * Determines the address of the button size cache
 * @param buttonIndex index of the button
 * @return address of the button size cache
 */
uint16_t getButtonSizeAddress(uint8_t buttonIndex)
{
    return buttonIndex * BUTTON_INFO_SIZE_ALLOCATION;
}

/**
 * Joins together two bytes into an unsigned 2 bytes length integer
 * @param a unsigned 8 bit integer
 * @param b unsigned 8 bit integer
 * @return unsigned 16 bit integer
 */
uint16_t merge8To16(uint8_t a, uint8_t b)
{
    return a | ((uint16_t) b << 8);
}

/**
 * Parses json from string
 * @param inputChars string input
 * @return json object
 */
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

/**
 * Processes json action into HID inputs
 * @param json json action object
 */
void actionJsonToHID(DynamicJsonDocument &json)
{
    if(json.containsKey("t"))
    {
        String stringType = json.getMember("t").as<String>();
        uint8_t stringTypeLength = stringType.length();
        const char type = stringType.charAt(0);
//        Serial.print("type: ");
//        Serial.println(type);
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
                    uint8_t times = 1;
                    if(json.containsKey("x"))
                        times = json.getMember("x").as<int>();
                    uint8_t keyDelay = 0;
                    if(json.containsKey("d"))
                        keyDelay = json.getMember("d").as<int>();
                    const int key = json.getMember("v").as<int>();

                    if(stringTypeLength == 1 || stringType.charAt(1) == 'd')
                    {
                        if(stringTypeLength == 1)
                        {
                            for(int i = 0; i < times; ++i)
                            {
                                Keyboard.press(key);
                                delay(keyDelay);
                                Keyboard.release(key);
                            }
                        }
                        else
                            Keyboard.press(key);
                    }
                    if(stringTypeLength == 1 || stringType.charAt(1) == 'u')
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
                boolean up = true, down = true;
                if(stringType.length() > 1)
                {
                    if(stringType.charAt(0) == 'u')
                        down = false;
                    else if(stringType.charAt(0) == 'd')
                        up = false;
                }
                if(times < 1)
                    times = 1;
                while(times-- > 0)
                {
                    Mouse.begin();
                    if(up && down)
                    {
//                        Serial.println("CLICK");
                        Mouse.click(mouseButton);
                    }
                    else
                    {
                        if(up)
                        {
//                            Serial.println("CLICK DOWN");
                            Mouse.press(mouseButton);
                        }
                        else
                        {
//                            Serial.println("CLICK UP");
                            Mouse.release(mouseButton);
                        }
                    }
                    delay(clickDelay);
                    Mouse.end();
                }
                break;
            }

            case 'm':   //Handle Mouse Move action
            {
                char x = 0, y = 0;
                int moveTime = 0;
                if(json.containsKey("d"))
                    moveTime = json.getMember("d").as<int>();
                if(json.containsKey("x"))
                    x = json.getMember("x").as<int8_t>();
                if(json.containsKey("y"))
                    y = json.getMember("y").as<int8_t>();
                Mouse.begin();
                if(moveTime > 0)
                {
                    double pos;
                    uint8_t xMoved = 0, yMoved = 0, xToMove = 0, yToMove = 0;
                    unsigned long currentMillis = millis(), startTime = currentMillis;
                    unsigned long gap = currentMillis - startTime;
                    while(gap < moveTime)
                    {
                        gap = currentMillis - startTime;

                        pos = map((double) gap, 0.0, moveTime-1.0, 0.0, x);
                        if(xMoved < pos)
                            xToMove = pos - xMoved;

                        pos = map((double) gap, 0.0, moveTime-1.0, 0.0, y);
                        if(yMoved < pos)
                            yToMove = pos - yMoved;

                        Mouse.move(xToMove, yToMove);
                        xMoved += xToMove;
                        yMoved += yToMove;
                        currentMillis = millis();
                    }
                }
                else
                    Mouse.move(x, y);
                Mouse.end();
                break;
            }

            case 'd':
            {
                int delayTime = 0;
                if(json.containsKey("v"))
                    delayTime = json.getMember("v").as<int>();
                delay(delayTime);
            }
                break;
        }
    }
}

/**
 * Separates button into its' actions and
 * processes each as an HID input
 * @param json json object
 */
void buttonJsonToHID(DynamicJsonDocument &json)
{
    JsonArray actions =  json["a"];             //Retrieve list of actions
    for (DynamicJsonDocument action : actions)  //For every action
        actionJsonToHID(action);             //Process the action
}

/**
 * Processes button by its' index from the stored EEPROM
 * @param buttonIndex the index of the button
 */
void processButton(uint8_t buttonIndex)
{
#ifdef DEBUG_1
    Serial.print("Processing Button ");
    Serial.println(buttonIndex);
#endif
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

/**
 * Sets the bit value for the pin that
 * gets clocked into the Shift Register
 * @param value bit value
 */
void setSRInput(boolean value)
{
    setBit(&PORTB, 4, value);
}

/**
 * Clocks the Shift Register
 */
void clockSR()
{
    setBitHigh(&PORTE, 6);
    setBitLow(&PORTE, 6);
}

/**
 * Clocks the shifted Shift Register inputs to outputs
 */
void clockSROut()
{
    setBitHigh(&PORTD, 4);
    delay(10);
    setBitLow(&PORTD, 4);
//    toggleBit(&PORTD, 4);
}

/**
 * Sets a specific bit of a byte
 * @param byte the byte
 * @param bit the bit position
 * @param value the new value
 */
void setBit(volatile uint8_t *byte, uint8_t bit, bool value)
{
    if(value)
        setBitHigh(byte, bit);
    else
        setBitLow(byte, bit);
}

/**
 * Sets a specific bit of a byte
 * @param byte the byte
 * @param bit the bit position
 * @param value the new value
 */
void setBit(uint8_t *byte, uint8_t bit, bool value)
{

    if(value)
        setBitHigh(byte, bit);
    else
        setBitLow(byte, bit);
}

/**
 * Sets a specific bit of a byte to high
 * @param byte the byte
 * @param bit the bit position
 */
void setBitHigh(uint8_t *byte, uint8_t bit)
{
    *byte |= (1 << bit);
}

/**
 * Sets a specific bit of a byte to high
 * @param byte the byte
 * @param bit the bit position
 */
void setBitHigh(volatile uint8_t *byte, uint8_t bit)
{
    *byte |= (1 << bit);
}

/**
 * Sets a specific bit of a byte to low
 * @param byte the byte
 * @param bit the bit position
 */
void setBitLow(uint8_t *byte, uint8_t bit)
{
    *byte &= ~(1 << bit);
}

/**
 * Sets a specific bit of a byte to low
 * @param byte the byte
 * @param bit the bit position
 */
void setBitLow(volatile uint8_t *byte, uint8_t bit)
{
    *byte &= ~(1 << bit);
}

/**
 * Reads a specific bit value from a byte
 * @param byte the byte
 * @param bit the bit to read
 * @return the value read
 */
boolean readBit(volatile uint8_t *byte, uint8_t bit)
{
    return *byte & (1 << bit);
}

/**
 * Performs a logarithmic calculation
 * @param base base value
 * @param value input value
 * @return output of the calculation
 */
double log(double base, double value)
{
    return log(value) / log(base);
}

/**
 * Toggles a specific bit from a byte
 * @param byte the byte
 * @param bit the bit to be toggled
 */
void toggleBit(volatile uint8_t *byte, uint8_t bit)
{
    *byte ^= 1 << bit;
}

/**
 * Toggles a specific bit from a byte
 * @param byte the byte
 * @param bit the bit to be toggled
 */
void toggleBit(uint8_t *byte, uint8_t bit)
{
    *byte ^= 1 << bit;
}

/**
 * Shifts a byte into the Shift Register
 * @param value the byte to be shifted in
 */
void setSR(uint8_t value)
{
    setBitLow(&PORTD, 4);
    for(uint8_t j = 7; j < 8; --j)
    {
//        Serial.print((value & 1 << j) != 0);
        setSRInput((value & 1 << j) != 0);
        clockSR();
    }
    setBitHigh(&PORTD, 4);
}

/**
 * Selects the specified button with the Shift Register
 * @param column button column
 * @param row button row
 */
void selectButtonSR(uint8_t column, uint8_t row)
{
    setSR(min(63, column) | (min(7, row) << 3));
}

/**
 * Performs the a mapping function
 * Converts input from range inLow-inHigh
 * to range of outLow-outHigh
 * @param input input
 * @param inLow range start of input
 * @param inHigh range end of input
 * @param outLow range start of output
 * @param outHigh range end of output
 * @return processed output
 */
double map(double input, double inLow, double inHigh, double outLow, double outHigh)
{
    if (inLow == inHigh)
        return outHigh;
    else
        return ((input - inLow) / (inHigh - inLow)) * (outHigh - outLow) + outLow;
}