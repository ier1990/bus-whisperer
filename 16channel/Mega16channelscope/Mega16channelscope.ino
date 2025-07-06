#include <Wire.h>
#include <pins_arduino.h>


//https://wiki.waynebooth.me/hobbie-docs/electronics/devices/microcontroller/uc-techniques
//https://www.avrfreaks.net/s/topic/a5C3l000000U7yCEAS/t044434
// from AVR035: Efficient C Coding for AVR
#define SETBIT(ADDRESS,BIT) (ADDRESS |= (1<<BIT))
#define CLEARBIT(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT))
#define FLIPBIT(ADDRESS,BIT) (ADDRESS ^= (1<<BIT))
#define CHECKBIT(ADDRESS,BIT) (ADDRESS & (1<<BIT))

#define SETBITMASK(x,y) (x |= (y))
#define bit_set(p,m) ((p) |= (BIT(m)))

#define CLEARBITMASK(x,y) (x &= (~y))
#define bit_clear(p,m) ((p) &= ~(BIT(m)))

#define FLIPBITMASK(x,y) (x ^= (y))
#define bit_flip(p,m) ((p) ^= (BIT(m)))

#define CHECKBITMASK(x,y) (x & (y))
#define bit_get(p,m) ((p) & (BIT(m)))

#define VARFROMCOMB(x, y) x
#define BITFROMCOMB(x, y) y

#define C_SETBIT(comb) SETBIT(VARFROMCOMB(comb), BITFROMCOMB(comb))
#define C_CLEARBIT(comb) CLEARBIT(VARFROMCOMB(comb), BITFROMCOMB(comb))
#define C_FLIPBIT(comb) FLIPBIT(VARFROMCOMB(comb), BITFROMCOMB(comb))
#define C_CHECKBIT(comb) CHECKBIT(VARFROMCOMB(comb), BITFROMCOMB(comb))

#define bit_write(c,p,m) (c ? bit_set(p,BIT(m)) : bit_clear(p,BIT(m)))
#define BIT(x) (0x01 << (x))
#define LONGBIT(x) ((unsigned long)0x00000001 << (x)) 


#define LOW 0
#define HIGH 1
#define FALSE 0
#define TRUE 1
#define OFF 0
#define ON 1


#define _INPUT(port,pin) DDR ## port &= ~(1<<pin)
#define _OUTPUT(port,pin) DDR ## port |= (1<<pin)
#define CLEAR(port,pin) PORT ## port &= ~(1<<pin)
#define SET(port,pin) PORT ## port |= (1<<pin)
#define TOGGLE(port,pin) PORT ## port ^= (1<<pin)
#define READ(port,pin) (PIN ## port & (1<<pin))


//Display
byte  ANALOG = ON; // ON for Arduino Auto Plotter
byte  DIGITAL = OFF;
byte  REGISTER = OFF;
//blinker
byte  globalblinkstate = OFF;



#define arduino2560DigitalBitMapLength 54
byte arduino2560DigitalBitMap[arduino2560DigitalBitMapLength]; 
const byte arduino2560DigitalBitMapDefaults[arduino2560DigitalBitMapLength] = {
    //[0] =WRITE PROTECT  ON/OFF or 1/0
    //[1] =digital IRQ    ON/OFF or 1/0
    //[2] =digital PWM    ON/OFF or 1/0
    //[3] =digital BLINK  ON/OFF or 1/0
    //[4] =fast           ON/OFF or 1/0 Gets more timeslices
    //[5] =digital HIGH/LOW      or 1/0
    //[6] =digital PULLUP ON/OFF or 1/0
    //[7] =digital OUTPUT/INPUT  or 1/0
            B10000000,  B10000000,  B00000000,  B00000000,  B00000000,  B00000000,  B00000000,  B00000000,  B00000000,  B0000000,//0-9 pin
            B00000000,  B00000000,  B00000000,  B00000000,  B00000000,  B00000000,  B00000000,  B00000000,  B00000000,  B0000000,//10-19 pin
            B10000000,  B10000000,  B00000000,  B00000000,  B00000000,  B00000000,  B00000000,  B00000000,  B00000000,  B0000000,//20-29 pin
            B00000000,  B00000000,  B00000000,  B00000000,  B00000000,  B00000000,  B00000000,  B00000000,  B00000000,  B0000000,//30-39 pin        
            B00000000,  B00000000,  B00000000,  B00000000,  B00000000,  B00000000,  B00000000,  B00000000,  B00000000,  B0000000,//40-49 pin
            B00000000,  B00000000,  B00000000,  B00000000//50-53 pin
};




//Create an integer arrays for data from the Digital Input
#define digital_pins_min 0
#define digital_pins_max 54
#define digital_safety_pins_min 2
#define digital_safety_pins_max 53
int digitalarray[digital_pins_max];









//Create an btye arrays for data from the serial in
//byte  commandline_max_len = 80;
#define commandline_max_len 80
byte commandline[commandline_max_len];
//byte LineOfText1[80];
unsigned long characterToStringPointer = 0;

int receivedchar; //var that will hold the bytes in read from the serialBuffer

void configurePinsFromDefaultBitmap() {
 // Copy the entire default bitmap into the live bitmap
  memcpy(arduino2560DigitalBitMap, arduino2560DigitalBitMapDefaults, arduino2560DigitalBitMapLength);

  for (int i = 0; i < arduino2560DigitalBitMapLength; i++) {
    // Copy the default value into the live array
    arduino2560DigitalBitMap[i] = arduino2560DigitalBitMapDefaults[i];

    byte data = arduino2560DigitalBitMap[i];

    if (!bit_get(data, 0)) { // Write Protect not set
      if (bit_get(data, 7)) {  // OUTPUT
        pinMode(i, OUTPUT);
        digitalWrite(i, bit_get(data, 5));  // HIGH or LOW
      } else {
        if (bit_get(data, 6)) pinMode(i, INPUT_PULLUP);
        else pinMode(i, INPUT);
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void ResetCommandLine()
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ResetCommandLine()
{
    characterToStringPointer = 0;
    for (int i = 0; i < commandline_max_len; i++)
    {
        commandline[i] = 0;;//'filess it up with commandline_max_len 0's  
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void blinker(int pin)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//http://code.google.com/p/digitalwritefast/downloads/list

void blinker(int pin)
{
    Serial.print("b");
    Serial.print(pin, DEC); // Send the pin value
    if (bit_get(arduino2560DigitalBitMap[pin], 3))    //turns off blink on 1 pin
    {
        bit_clear(arduino2560DigitalBitMap[pin], 3);
        bit_clear(arduino2560DigitalBitMap[pin], 5);
        bit_clear(arduino2560DigitalBitMap[pin], 7);
        Serial.println("=BLINK OFF"); // Add a space separator      
        digitalWrite(pin, LOW); //Sets the state of the pin to low to avoid Pullup state      
        pinMode(pin, INPUT);//turns pin state to input
        digitalWrite(pin, LOW); //Sets the state of the pin to low to avoid Pullup state

    }
    else if (!bit_get(arduino2560DigitalBitMap[pin], 3))//turns on blink
    {
        Serial.print("=BLINK ON"); // Add a space separator      
        Serial.println(); // Terminate message  
        globalblinkstate = ON; //turns on blink 
        bit_set(arduino2560DigitalBitMap[pin], 3);
        bit_set(arduino2560DigitalBitMap[pin], 5);
        bit_set(arduino2560DigitalBitMap[pin], 7);
        pinMode(pin, OUTPUT);//turns pin state to output
        digitalWrite(pin, bit_get(arduino2560DigitalBitMap[pin], 3)); //Sets the state of the pin to ledstat
        //turns off globalblinkstate
    }
    for (int i = 0; i < arduino2560DigitalBitMapLength; i++)
    {
        if (bit_get(arduino2560DigitalBitMap[i], 3))
        {
            globalblinkstate = ON;
            return;
        }
    }
    globalblinkstate = OFF;
    return;
}

void setup() {
  memcpy(arduino2560DigitalBitMap, arduino2560DigitalBitMapDefaults, arduino2560DigitalBitMapLength);
  Serial.begin(115200);
  configurePinsFromDefaultBitmap();
  ResetCommandLine();
}

void loop() {

  if (globalblinkstate == ON)//blink
    {
        for (int i = 0; i < arduino2560DigitalBitMapLength; i++)
        {
            byte data = arduino2560DigitalBitMap[i];   // fetch data from memory
            if ((!bit_get(data, 0)) && (bit_get(data, 3)) && (bit_get(data, 7)))
            {
                bit_flip(arduino2560DigitalBitMap[i], 5);
                digitalWrite(i, bit_get(arduino2560DigitalBitMap[i], 5));
            }
        }

    }


  Serial.print("<A>");  // start of analog
  for (int i = 0; i < 16; i++) {
    int val = analogRead(i);
    Serial.print(val);
    if (i < 15) Serial.print(",");
  }
  Serial.println("</A>");  // end of analog

  
  delay(60); // adjust for update speed


////////////////////////////////////////////
    //Start of Receive //Start of Receive 
    ////////////////////////////////////////////
    if (Serial.available())
    {
        receivedchar = Serial.read(); //read Serial
        //receivedchar = receivedchar.toLowerCase();
    }else {
      receivedchar = -1;
    }

    //  'If we Received anything do something with it
    if (receivedchar > -1)
    {
        if ((receivedchar == '/') || (commandline[0] == '/'))
        {
            CreateCommandLine(receivedchar);
        }
        else if (receivedchar == 'r') {
            //reset to default
        }

    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void digital_pin_change(int pin,int state,int IN_OUT)  SLOW Speed but has error correction
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void digital_pin_change(int pin, int state, int IN_OUT)
{
    if (IN_OUT == INPUT) { digitalWrite(pin, LOW); }//error correction for possible pullup state when changing pin states
    pinMode(pin, IN_OUT); //Changes pin state to Qutput
    digitalWrite(pin, state); //Sets the state of the pin 
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  void CreatelCommandLine(int keypress) 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CreateCommandLine(int keypress)
{
    int result;

    if ((keypress > -1) && (keypress != 13) && (keypress != 10))
    {
        if ((characterToStringPointer > 0) && (keypress == 8))// Backspace
        {
            commandline[--characterToStringPointer] = 0;
        }
        else if ((keypress) && (characterToStringPointer < commandline_max_len) && (keypress != 13))
        {
            commandline[characterToStringPointer] = keypress;//    'Write character to commandline
            //Serial.print(commandline[characterToStringPointer]);//prints the character just read
            Serial.print((char)commandline[characterToStringPointer]);//prints the character just read

            //Serial.print(receivedchar); //prints the character just read for debug
            characterToStringPointer++;
        }
    }
    else if (keypress == 13 || keypress == 10)
    {
        //characterToStringPointer now contains length of command line
        //add 0 at end and turn it into a string!
        characterToStringPointer++;
        commandline[characterToStringPointer] = 0;

        ///////////////////////////////////////////
        //Display Command line
        ///////////////////////////////////////////
        Serial.print("\nReceived: ");
        Serial.println((char*)commandline);


        ///////////////////////////////////////////
        //Run Command line
        ///////////////////////////////////////////
        //reseult = 
        if (strncmp((char*)commandline, "/wb", 3) == 0) {
            // Parse pin number after "/wb"
            int pin = atoi((char*)&commandline[3]);
            blinker(pin);
            Serial.print("Blink ON: Pin ");
            Serial.println(pin);            
        }
        else if (strncmp((char*)commandline, "/wh", 3) == 0) {
            // Parse pin number after "/wh"
            int pin = atoi((char*)&commandline[3]);
            if (pin >= 2 && pin < arduino2560DigitalBitMapLength) {
              bit_set(arduino2560DigitalBitMap[pin], 7);
              bit_set(arduino2560DigitalBitMap[pin], 5);
              digital_pin_change(pin, HIGH, OUTPUT);            
              Serial.print("Pin High: Pin ");
              Serial.println(pin);    
            }        
        }    
        else if (strncmp((char*)commandline, "/wl", 3) == 0) {
            // Parse pin number after "/wl"
            int pin = atoi((char*)&commandline[3]);
            if (pin >= 2 && pin < arduino2560DigitalBitMapLength) {
              bit_set(arduino2560DigitalBitMap[pin], 7);
              bit_set(arduino2560DigitalBitMap[pin], 5);
              digital_pin_change(pin, LOW, OUTPUT);            
              Serial.print("Pin LOW: Pin ");
              Serial.println(pin);    
            }        
        }    
        else if (strncmp((char*)commandline, "/wi", 3) == 0) {
          int pin = atoi((char*)&commandline[3]);
          if (pin >= 2 && pin < arduino2560DigitalBitMapLength) {
            bit_clear(arduino2560DigitalBitMap[pin], 7); // INPUT
            bit_clear(arduino2560DigitalBitMap[pin], 6); // no pullup
            pinMode(pin, INPUT);
            digitalWrite(pin, LOW); // safety reset
            Serial.print("Pin INPUT: ");
            Serial.println(pin);
          }
        }

        else if (strncmp((char*)commandline, "/wu", 3) == 0) {
          int pin = atoi((char*)&commandline[3]);
          if (pin >= 2 && pin < arduino2560DigitalBitMapLength) {
            bit_clear(arduino2560DigitalBitMap[pin], 7); // INPUT
            bit_set(arduino2560DigitalBitMap[pin], 6);   // pullup
            pinMode(pin, INPUT_PULLUP);
            Serial.print("Pin INPUT_PULLUP: ");
            Serial.println(pin);
          }
        }   
        else if (strncmp((char*)commandline, "/wd", 3) == 0) {
          int pin = atoi((char*)&commandline[3]);
          if (pin >= 2 && pin < arduino2560DigitalBitMapLength) {
            int val = digitalRead(pin);
            Serial.print("DigitalRead pin ");
            Serial.print(pin);
            Serial.print(": ");
            Serial.println(val);
          }
        }                         
        else if (strncmp((char*)commandline, "/wboff", 6) == 0) {
            // Optional: disable blink if user sends "/wboff7"
            globalblinkstate = OFF; //turns off blink 
            int pin = atoi((char*)&commandline[6]);
            if (pin >= 2 && pin < arduino2560DigitalBitMapLength) {
                bit_clear(arduino2560DigitalBitMap[pin], 3); // disable blink
                Serial.print("Blink OFF: Pin ");
                Serial.println(pin);
            }
        }
        else if (strncmp((char*)commandline, "/reset", 6) == 0) {
          globalblinkstate = OFF; // Stop all blinking
          configurePinsFromDefaultBitmap();  // Restore default states
          Serial.println("All pins reset to defaults.");
        }



        ///////////////////////////////////////////
        //Reset Command line
        ///////////////////////////////////////////
        ResetCommandLine();
    }
  //return result;
}







