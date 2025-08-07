
/*  Rui Santos & Sara Santos - Random Nerd Tutorials
    THIS EXAMPLE WAS TESTED WITH THE FOLLOWING HARDWARE:
    1) ESP32-2432S028R 2.8 inch 240×320 also known as the Cheap Yellow Display (CYD): https://makeradvisor.com/tools/cyd-cheap-yellow-display-esp32-2432s028r/
      SET UP INSTRUCTIONS: https://RandomNerdTutorials.com/cyd/
    2) REGULAR ESP32 Dev Board + 2.8 inch 240x320 TFT Display: https://makeradvisor.com/tools/2-8-inch-ili9341-tft-240x320/ and https://makeradvisor.com/tools/esp32-dev-board-wi-fi-bluetooth/
      SET UP INSTRUCTIONS: https://RandomNerdTutorials.com/esp32-tft/
    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/





#include <SPI.h>

/*  Install the "TFT_eSPI" library by Bodmer to interface with the TFT Display - https://github.com/Bodmer/TFT_eSPI
    *** IMPORTANT: User_Setup.h available on the internet will probably NOT work with the examples available at Random Nerd Tutorials ***
    *** YOU MUST USE THE User_Setup.h FILE PROVIDED IN THE LINK BELOW IN ORDER TO USE THE EXAMPLES FROM RANDOM NERD TUTORIALS ***
    FULL INSTRUCTIONS AVAILABLE ON HOW CONFIGURE THE LIBRARY: https://RandomNerdTutorials.com/cyd/ or https://RandomNerdTutorials.com/esp32-tft/   

//colors are defined in the TFT_eSPI.h file 
//but show up as reverse colors so corrected it

    Corrected Colors
    */
#include "TFT_eSPI.h"

// Install the "XPT2046_Touchscreen" library by Paul Stoffregen to use the Touchscreen - https://github.com/PaulStoffregen/XPT2046_Touchscreen
// Note: this library doesn't require further configuration
#include <XPT2046_Touchscreen.h>

#include <Wire.h>
#define I2C_SDA 22
#define I2C_SCL 27
#include <SparkFun_I2C_Mux_Arduino_Library.h> //Click here to get the library: http://librarymanager/All#SparkFun_I2C_Mux
QWIICMUX myMux;

#include "Arduino.h"

// Include the libraries for the Free Fonts
#include <FS.h>

#define TEXT "abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ 1234567890 !@#$%^&*()_+[]{}|;':,.<>?/`~"
#include "Free_Fonts.h" // Include the header file attached to this sketch

#include <TFT_eWidget.h>           // Widget library

TFT_eSPI tft = TFT_eSPI();

// Touchscreen pins
#define XPT2046_IRQ 36   // T_IRQ
#define XPT2046_MOSI 32  // T_DIN
#define XPT2046_MISO 39  // T_OUT
#define XPT2046_CLK 25   // T_CLK
#define XPT2046_CS 33    // T_CS

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define FONT_SIZE 2

// Set X and Y coordinates for center of display
int centerX = SCREEN_WIDTH / 2;
int centerY = SCREEN_HEIGHT / 2;


// Touchscreen coordinates: (x, y) and pressure (z)
int x, y, z;



// Print Touchscreen info about X, Y and Pressure (Z) on the Serial Monitor
void printTouchToSerial(int touchX, int touchY, int touchZ) {
  Serial.print("X = ");
  Serial.print(touchX);
  Serial.print(" | Y = ");
  Serial.print(touchY);
  Serial.print(" | Pressure = ");
  Serial.print(touchZ);
  Serial.println();
}

// Print Touchscreen info about X, Y and Pressure (Z) on the TFT Display
void printTouchToDisplay(int touchX, int touchY, int touchZ) {
  // Clear TFT screen
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);

  int centerX = SCREEN_WIDTH / 2;
  int textY = 80;
 
  String tempText = "X = " + String(touchX);
  tft.drawCentreString(tempText, centerX, textY, FONT_SIZE);

  textY += 20;
  tempText = "Y = " + String(touchY);
  tft.drawCentreString(tempText, centerX, textY, FONT_SIZE);

  textY += 20;
  tempText = "Pressure = " + String(touchZ);
  tft.drawCentreString(tempText, centerX, textY, FONT_SIZE);
}

int digitalToggle(byte pin){
  int result = !digitalRead(pin);
  digitalWrite(pin, result);
  return result;
}

void scan_i2c_v2(){
  Serial.println("I2C v2 Scanner. Scanning ..."); 
  byte count = 0; 
  for (byte i = 1; i < 127; i++) { 
    Wire.beginTransmission(i); 
    if (Wire.endTransmission() == 0) { 
      Serial.print("Found address: "); 
      Serial.print(i, DEC); 
      Serial.print(" (0x"); 
      Serial.print(i, HEX); 
      Serial.println(")"); 
      count++; 
      } 
  } 
  Serial.print("Found "); 
  Serial.print(count, DEC); 
  Serial.println(" device(s).");
}

void scan_I2C(){
  byte error, address;
  int nDevices;
  Serial.println("I2C v1 Scanner. Scanning ...");
  tft.drawCentreString("Scanning I2C...", centerX, 0, FONT_SIZE);
  nDevices = 0;
  int tmp = 15;
  String tempaddress = "I2C device found 0x";
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {                  
      if (address<16) {        
        tempaddress += "0";     
      }
      tempaddress = tempaddress + address;
      tempaddress += String(address, DEC); 
      tempaddress += " (0x"; 
      tempaddress += String(address, HEX); 
      tempaddress += ")";

      Serial.println(tempaddress);
      tft.drawCentreString(tempaddress, centerX, tmp, FONT_SIZE);
      tmp = tmp + 15;
      nDevices++;
    }
    else if (error==4) {
      tempaddress = "Unknow error at address 0x";
      if (address<16) {
        tempaddress = tempaddress + "0";
      }
      tempaddress = tempaddress + address;
      Serial.println(tempaddress);
      tft.drawCentreString(tempaddress, centerX, tmp, FONT_SIZE);
      tmp = tmp + 15;
    }    
  }
  if (nDevices == 0) {
    //end wire, start debugging?
    Wire.end();
    //Wire.begin(I2C_SDA, I2C_SCL);  
    pinMode(I2C_SCL, OUTPUT);
    pinMode(I2C_SDA, INPUT_PULLUP);
    
    delay(50);
    int result = LOW;
    while(true){
      
      Serial.println("No I2C devices found\n");
      tft.drawCentreString("No I2C devices found!", centerX, 15, FONT_SIZE);    
      float aread = analogRead(I2C_SCL) * 0.0008056640625;
      String tempText = " SCL = " + String(aread,2) + "v";
      Serial.println(tempText);
      tft.drawCentreString(tempText, centerX, 30, FONT_SIZE);   


      digitalToggle(4);
      digitalToggle(I2C_SCL);
      digitalToggle(I2C_SDA);
      delay(50);

      aread = analogRead(I2C_SDA) * 0.0008056640625;
      tempText = " SDA = " + String(aread,2) + "v";
      Serial.println(tempText);
      tft.drawCentreString(tempText, centerX, 45, FONT_SIZE);    
      
      if (touchscreen.tirqTouched() && touchscreen.touched()) {
        break;
      }
    }
    
  }
  else {
    Serial.println("done\n");
    tft.drawCentreString("done\n", centerX, 45, FONT_SIZE);
  }
  //delay(5000); 


}
float int_to_volts(uint16_t dac_value, uint8_t bits, float logic_level) {
  return (((float)dac_value / ((1 << bits) - 1)) * logic_level);
}









//create buttons
/*
screen is 320x240
x is 0 to 320
y is 0 to 240
font size 1 is 6x8
font size 2 is 12x16
font size 3 is 18x24

//Horizontal
320/(font size 1) = 53.33333333333333
320/(font size 2) = 26.66666666666667
320/(font size 3) = 17.77777777777778

//Vertical
240/(font size 1) = 30
240/(font size 2) = 15
240/(font size 3) = 10

"Scope/Curve trace" = 17 characters
17 characters * 6 = 102px
17 characters * 12 = 204px
17 characters * 16 = 272px

buttom width = font size * (number of characters + 2) 
buttom height = font size * 2
*/

void drawButton(int x, int y, String text, int textSize, int textColor, int buttonColor) {  
  // Set the text size
  tft.setTextSize(textSize);

  // Calculate the width and height of the text
  int textWidth = tft.textWidth(text);
  int textHeight = tft.fontHeight();

  // Add some padding around the text
  int padding = 10;
  int buttonWidth = textWidth + 2 * padding;
  int buttonHeight = textHeight + 2 * padding;

  // Ensure the button fits within the screen boundaries
  if (buttonWidth > SCREEN_WIDTH) {
    buttonWidth = SCREEN_WIDTH;
  }
  if (buttonHeight > SCREEN_HEIGHT) {
    buttonHeight = SCREEN_HEIGHT;
  }

  // Draw the button rectangle
  tft.drawRect(x, y, buttonWidth, buttonHeight, buttonColor);
  // Fill the button rectangle
  tft.fillRect(x, y, buttonWidth, buttonHeight, buttonColor);  

  // Draw the text centered within the button
  int textX = x + (buttonWidth - textWidth) / 2;
  int textY = y + (buttonHeight - textHeight) / 2;
  tft.setTextColor(textColor, buttonColor); 
  tft.setCursor(textX, textY);
  tft.print(text);
}


void printFreeMemory() {
  Serial.print("Free heap memory: ");
  Serial.print(ESP.getFreeHeap());
  Serial.println(" bytes");
}
void printChipInfo() {
  Serial.print("Chip model: ");
  Serial.println(ESP.getChipModel());
  Serial.print("Chip revision: ");
  Serial.println(ESP.getChipRevision());
  Serial.print("Number of cores: ");
  Serial.println(ESP.getChipCores());
  Serial.print("Flash size: ");
  Serial.print(ESP.getFlashChipSize() / (1024 * 1024));
  Serial.println(" MB");
}
void printResetReason() {
  esp_reset_reason_t reason = esp_reset_reason();
  Serial.print("Reset reason: ");
  switch(reason) {
    case ESP_RST_POWERON: Serial.println("Power on reset"); break;
    case ESP_RST_EXT: Serial.println("External reset"); break;
    case ESP_RST_SW: Serial.println("Software reset"); break;
    case ESP_RST_PANIC: Serial.println("Exception/panic"); break;
    case ESP_RST_INT_WDT: Serial.println("Interrupt watchdog"); break;
    case ESP_RST_TASK_WDT: Serial.println("Task watchdog"); break;
    case ESP_RST_WDT: Serial.println("Other watchdog"); break;
    case ESP_RST_DEEPSLEEP: Serial.println("Exiting deep sleep"); break;
    case ESP_RST_BROWNOUT: Serial.println("Brownout reset"); break;
    case ESP_RST_SDIO: Serial.println("SDIO reset"); break;
    default: Serial.println("Unknown reason"); break;
  }
}
void printUptime() {
  uint32_t uptime = millis() / 1000;
  uint32_t seconds = uptime % 60;
  uint32_t minutes = (uptime / 60) % 60;
  uint32_t hours = (uptime / 3600) % 24;
  uint32_t days = uptime / 86400;

  Serial.print("Uptime: ");
  if (days > 0) {
    Serial.print(days);
    Serial.print(" days ");
  }
  if (hours > 0) {
    Serial.print(hours);
    Serial.print(" hours ");
  }
  Serial.print(minutes);
  Serial.print(" minutes ");
  Serial.print(seconds);
  Serial.println(" seconds");
}


//touchscreen pause till touched
void waitTouch() {
  while (!touchscreen.touched()) {
    delay(100);
  }
}



int receivedchar;  //var that will hold the bytes in read from the serialBuffer
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//
// Setup
//
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////

void setup() {

  //delay(2000);

  Wire.setPins(I2C_SDA, I2C_SCL);  
  Wire.begin();

  Serial.begin(115200);  

  // Start the SPI for the touchscreen and init the touchscreen
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  // Set the Touchscreen rotation in landscape mode
  // Note: in some displays, the touchscreen might be upside down, so you might need to set the rotation to 3: touchscreen.setRotation(3);
  touchscreen.setRotation(1);

  // Start the tft display
  tft.init();

  // Set the TFT display rotation in landscape mode
  tft.setRotation(1);

  

  // Clear the screen before writing to it
  tft.fillScreen(TFT_BLACK);
  //setTextColor(fontColor, backgroundColor);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);

  // Set the text size
  tft.setTextSize(3);

  // Set the text wrap
  tft.setTextWrap(true);

  // Set the text cursor position center
  tft.setCursor(SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2);

  // Print the text on the TFT display
  tft.println("Booting...");
  delay(2000);
  tft.setTextSize(FONT_SIZE);
  // Clear the screen before writing to it
  tft.fillScreen(TFT_BLACK);
  
  // scan I2c bus & show to display
  tft.println("I2C Scanner..."); 
  byte count = 0;    
  for (byte i = 1; i < 127; i++) { 
    Wire.beginTransmission(i); 
    if (Wire.endTransmission() == 0) { 
      //tft.print("Found address: "); 
      //tft.print(i, DEC); 
      tft.print(" 0x"); 
      tft.print(i, HEX); 
      tft.print(", "); 
      count++; 
      }else{
        //tft.print("Unknow error at address 0x");
        //tft.print(i, HEX); 
        //tft.println(")"); 
      }
  }
  tft.println();

  //delay(1000);

    
  //scan_i2c_v2();
  //scan_I2C(); 
  printResetReason(); 
  
  tft.println("End Setup, tap screen");
  
  // Waits till Touchscreen is touched?
  waitTouch();
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(FONT_SIZE);

  

}



/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
//fps counter
int fps = 0;
int old_fps = 0;
int erase_old_fps = 0;
unsigned long lastTime = 0;
unsigned long lastTime1 = 0;
unsigned long currentTime = 0;
  //array to hold 2 strings?
  char *buttonText[] = {"Scope", "CurveTrace", "47ohm", "470ohm", "4.7kohm", "47kohm", "12vrelay"};
  char *tmpbuttonText[] = {"Scope/CurveTrace"};
/////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////
// void loop() {
/////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////
void loop() {  

  fps++;

  currentTime = millis();
//if the time since the last time the loop ran is greater than 100ms
  if (currentTime - lastTime > 100) { 
    //set the last time to the current time
    lastTime = currentTime;
    //do the stuff you want to do every 1/10th of a second
    //Serial.println("1/10th of a second has passed");
  }
  //if the time since the last time the loop ran is greater than 1000ms
  if (currentTime - lastTime1 > 1000) {
    //set the last time to the current time
    lastTime1 = currentTime;
    //do the stuff you want to do every second
    //Serial.println("1 second has passed");
    //Serial.print("fps=");Serial.println(fps);
    erase_old_fps = old_fps;
    old_fps = fps;
    fps = 0;

  }//




  
  //array to hold 2 functions?
  //int (*buttonFunction[])() = {pcf8575_Toggle(8), pcf8575_Toggle(7), pcf8575_Toggle(6), pcf8575_Toggle(5), pcf8575_Toggle(4), pcf8575_Toggle(3), pcf8575_Toggle(2)};
  //array to hold 2 x values?
  //int buttonX[] = {10, 10, 10, 10, 10, 10, 10};

  // Checks if Touchscreen was touched, and prints X, Y and Pressure (Z) info on the TFT display and Serial Monitor
  if (touchscreen.tirqTouched() && touchscreen.touched()) {
    // Get Touchscreen points
    TS_Point p = touchscreen.getPoint();
    // Calibrate Touchscreen points with map function to the correct width and height
    x = map(p.x, 200, 3700, 1, SCREEN_WIDTH);
    y = map(p.y, 240, 3800, 1, SCREEN_HEIGHT);
    z = p.z;

    printTouchToSerial(x, y, z);
    //printTouchToDisplay(x, y, z);

    // Control relay 1 on the PCAL9535A at address 0x27 (which is defined as A000 in the enum) 
    //controlRelay(0x27, 1, LOW); // Turn on relay 1 
    //delay(1000); // Wait for a second 
    //controlRelay(0x27, 1, HIGH); // Turn off relay 1     

//////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////
//scope to curve trace or curve trace to scope
  //scope to curve trace check button
  if( (x > 10) && (x < 110) && (y > 10) && (y < 60) ) {
    // default is Scope
    // if returned value is 1, then set to 0 which is the same as Scope
    // if returned value is 0, then set to 1 which is the same as CurveTrace
    tft.fillScreen(TFT_BLACK);
  }
//////////////////////////////////////////////////////
// end of scope to curve trace or curve trace to scope
//////////////////////////////////////////////////////




//////////////////////////////////////////////////////
// LM386
//////////////////////////////////////////////////////
  else if( (tmpbuttonText[0] == "CurveTrace") && (x > 10) && (x < 110) && (y > 70) && (y < 120) ) {
    // Control relay 6 on the PCAL9535A at address
    //what address?
    delay(1000); // Wait for a second    
  } 


//////////////////////////////////////////////////////
// 12v Bulb
//////////////////////////////////////////////////////
  else if( (tmpbuttonText[0] == "CurveTrace") && (x > 160) && (x < 260) && (y > 70) && (y < 120) ) {
    // Control relay 6 on the PCAL9535A at address
    delay(1000); // Wait for a second    
  }
//////////////////////////////////////////////////////
// end of 12v Bulb
////////////////////////////////////////////////////


//////////////////////////////////////////////////////
// 47ohm
//////////////////////////////////////////////////////
  else if( (tmpbuttonText[0] == "CurveTrace") && (x > 10) && (x < 110) && (y > 130) && (y < 180) ) {
    // Control relay 6 on the PCAL9535A at address
    delay(1000); // Wait for a second    
  }
//////////////////////////////////////////////////////
// end of 47ohm
//////////////////////////////////////////////////////



}//end of touchscreen.tirqTouched() && touchscreen.touched()


if(fps == 0){
  // Clear TFT screen
  //tft.fillScreen(TFT_BLACK);
  //tft.setTextColor(TFT_WHITE, TFT_BLACK);
  // Print Touchscreen info about X, Y and Pressure (Z) on the TFT Display
  //printTouchToDisplay(x, y, z);





//draw buttons
/*//scope to curve trace
  if( (x > 10) && (x < 110) && (y > 10) && (y < 60) ) {
  */
  // 1st button at x=10, y=10, width=100, height=50, text="Scope", text size=2
  //tft.drawRect(10, 10, 100, 50, TFT_BLACK);
  //tft.drawCentreString(tmpbuttonText[0], 60, 35, FONT_SIZE);
  drawButton(10, 10, tmpbuttonText[0], FONT_SIZE, TFT_WHITE, TFT_PURPLE);

  // 2nd button at x=10, y=70, width=100, height=50, text="CurveTrace", text size=2
  //else if( (x > 10) && (x < 110) && (y > 70) && (y < 120) ) {
  
  ///tft.drawRect(10, 70, 100, 50, TFT_BLACK);

  //tft.drawCentreString(tmpbuttonText[0], 60, 95, FONT_SIZE);
  // 7th button at x=10, y=370, width=100, height=50, text="relay", text size=2 
  //tft.drawRect(10, 370, 100, 50, TFT_BLACK);
  
  //tft.drawCentreString("12vrelay", 60, 95, FONT_SIZE);  

  //2nd button at x=10, y=70, width=100, height=50, text="CurveTrace", text size=2
  drawButton(SCREEN_WIDTH-65, 10, "OFF", FONT_SIZE, TFT_WHITE, TFT_RED);

if(tmpbuttonText[0] == "CurveTrace") {
  //green is purple  why is it backwards?
  //yellow is blue
  //blue is yellow
  //red is cyan
  //how do i fix it?

  //RELAYS 6 & 9 switches the scope curve trace FROM 12vac wall wart to LM386 amplifier & 555 timer
  drawButton(10, 70, "LM386", FONT_SIZE, TFT_WHITE, TFT_BLUE);
  
  drawButton(SCREEN_WIDTH/2, 70, "12v Bulb", FONT_SIZE, TFT_WHITE, TFT_BLUE);

  drawButton(10, 130, "47ohm", FONT_SIZE, TFT_WHITE, TFT_BLUE);
  
  drawButton(10, 190, "470ohm", FONT_SIZE, TFT_WHITE, TFT_BLUE);
  
  drawButton(SCREEN_WIDTH/2, 130, "4.7kohm", FONT_SIZE  , TFT_WHITE, TFT_BLUE);

  drawButton(SCREEN_WIDTH/2, 190, "47kohm", FONT_SIZE, TFT_WHITE, TFT_BLUE);

}//end of tmpbuttonText[0] == "CurveTrace"

//Erase old fps
if (erase_old_fps != old_fps){
  tft.fillRect(SCREEN_WIDTH - 50, SCREEN_HEIGHT - 20, 50, 10, TFT_BLACK);
  erase_old_fps = old_fps;
}

  //draw fps
  tft.setTextSize(1);
  tft.setCursor(SCREEN_WIDTH - 50, SCREEN_HEIGHT - 10);  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.print("fps");
  tft.print(old_fps);
  tft.setTextSize(FONT_SIZE);
  


}//end of fps == 0


  
   
//Estimated time to read the serial port is 1-2fps?
  if (Serial.available()) {
    receivedchar = Serial.read();  //read Serial

    if (receivedchar == 'r') {        
    Serial.println("Turning off 5V Supply for 500ms?");
    delay(1000); // Wait for 1 seconds 
    ESP.restart();     
    }

    if (receivedchar == 'm') {    
      printChipInfo();  
      printFreeMemory(); 
      printResetReason();  
      printUptime();  
    }


    if (receivedchar == '0') {
      Serial.println(char(receivedchar));
      scan_I2C();
      scan_i2c_v2();
      //maxI=0;
      //Serial.println("Going into Emergency Self Test in 1 sec....");
      //delay(1000);
    }
  }




}


