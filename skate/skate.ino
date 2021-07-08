#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
//#include <VescUart.h>

// custom include
#include "header.h"

SoftwareSerial HM10(11, 12);
//VescUart UART;


void setup() {
  //initialization for the namespace leds
  pinMode(leds::LEDS_PIN_1, OUTPUT);
  pinMode(leds::LEDS_PIN_2, OUTPUT);
  pinMode(leds::LEDS_PIN_3, OUTPUT);
  
  leds::module1.begin();
  leds::module2.begin();
  leds::module_back.begin();

  leds::module1.setBrightness(200);
  leds::module2.setBrightness(200);
  leds::module_back.setBrightness(200);

  init_france(false);
  

  //initialization for the HM10
  HM10.begin(BLE::BAUDRATE);
  delay(10);
}



void loop() {
  getUpdate();
  updateLeds();
  delay(50);
}

void getUpdate(void) {
  //this function checks if a new code has been received over Bluetooth
  if (HM10.available()) {
    while (HM10.available()) {
      BLE::c = HM10.read();
      if (BLE::c == '<') { //I am using '<' as start of message code (SOM)
        BLE::myString = "";
      }
      BLE::myString = BLE::myString + String(BLE::c);
      if (BLE::c == '>') { //I am using '>' as end of message code (EOM)
        BLE::myString.replace("<", " ");
        BLE::myString.replace(">", " ");
        BLE::myString.trim();
        decodeString(BLE::myString);
      }
    }
  }

  // this part send local data over Bluetooth to the iPhone
  //else if (BLE::cnt == 1000) {
  /*
  if (BLE::cnt == 20) {
    //HM10.print("<" + String(random(10)) + "," + String(random(150)) + ">");
    BLE::cnt = 0;
    //UART.getVescValues();
    //HM10.print("<" + String(random(50)) +  "," + String(UART.data.inpVoltage) + "," + String(UART.data.rpm) + ">");


    if (false ){
      HM10.print("<" + String(random(50)) + ">");
      //HM10.print("<" + String(random(50)) +  "," + String(UART.data.inpVoltage) + "," + String(UART.data.rpm) + ">");
    }
    else{
      HM10.print("<" + String(10) + "," + String(random(50)) + "," + String(random(200)) + ">");
    }


  }
  */
  BLE::cnt += 1;
}

void decodeString(String str) {
  //given a code string, change the variables of the namespace LED according to the command received
  int code = str.toInt();
  switch (code) {
    case leds::LEDINDEX0:
      leds::previousColor = leds::selectedColor;
      leds::selectedColor = leds::LEDINDEX0;
      leds::newCommand = leds::SIDE;
      break;

    case leds::LEDINDEX1:
      leds::previousColor = leds::selectedColor;
      leds::selectedColor = leds::LEDINDEX1;
      leds::newCommand = leds::SIDE;
      break;

    case leds::LEDINDEX2:
      leds::previousColor = leds::selectedColor;
      leds::selectedColor = leds::LEDINDEX2;
      leds::newCommand = leds::SIDE;
      break;

    case leds::LEDINDEX3:
      leds::previousColor = leds::selectedColor;
      leds::selectedColor = leds::LEDINDEX3;
      leds::newCommand = leds::SIDE;
      break;

    case leds::LEDINDEX4:
      leds::previousColor = leds::selectedColor;
      leds::selectedColor = leds::LEDINDEX4;
      leds::newCommand = leds::SIDE;
      break;

    case leds::LEDSON:
      leds::activated = true;
      leds::newCommand = leds::SIDE;
      break;

    case leds::LEDSOFF:
      leds::activated = false;
      leds::newCommand = leds::SIDE;
      break;

    case leds::LEFTBLINKERON:
      leds::leftBlinker = true;
      leds::newCommand = leds::BACK;
      break;

    case leds::LEFTBLINKEROFF:
      leds::leftBlinker = false;
      leds::newCommand = leds::BACK;
      break;

    case leds::RIGHTBLINKERON:
      leds::rightBlinker = true;
      leds::newCommand = leds::BACK;
      break;

    case leds::RIGHTBLINKEROFF:
      leds::rightBlinker = false;
      leds::newCommand = leds::BACK;
      break;

    case leds::REARLIGHTON:
      leds::rearLight = true;
      leds::newCommand = leds::BACK;
      break;

    case leds::REARLIGHTOFF:
      leds::rearLight = false;
      leds::newCommand = leds::BACK;
      break;

    case leds::FRANCEON:
      leds::france = true;
      leds::newCommand = leds::BACK;
      break;

    case leds::FRANCEOFF:
      leds::france = false;
      leds::newCommand = leds::BACK;
      break;
  }
}

void updateLeds(void) {
  if (leds::newCommand == leds::SIDE){
    if (leds::activated) {
      changeLedColor(leds::selectedColor);
    } else {
      leds::module1.clear();
      leds::module2.clear();
    }
  }
  
  if (leds::newCommand == leds::BACK){
    if (leds::rearLight) {
      activateBackLight();
      if (leds::france) {
        init_france(true);
      } 
    } else {
      leds::module_back.clear();
    }
  }
  
  leds::module1.show();
  leds::module2.show();
  leds::module_back.show();

  leds::newCommand = leds::DISABLED;
}

void changeLedColor(int new_color){
  int newColor = leds::selectedColor;
  int prevColor = leds::previousColor;
  for (int i = 0; i < int(leds::LEDS_NUMBER/2); i++) {
    leds::module1.clear();
    leds::module2.clear();
    leds::module1.setPixelColor(i, leds::color[prevColor][0], leds::color[prevColor][1], leds::color[prevColor][2]);
    leds::module1.setPixelColor(leds::LEDS_NUMBER - 1 - i, leds::color[prevColor][0], leds::color[prevColor][1], leds::color[prevColor][2]);
    leds::module2.setPixelColor(i, leds::color[prevColor][0], leds::color[prevColor][1], leds::color[prevColor][2]);
    leds::module2.setPixelColor(leds::LEDS_NUMBER - 1 - i, leds::color[prevColor][0], leds::color[prevColor][1], leds::color[prevColor][2]);
    leds::module1.show();
    leds::module2.show();
    delay(20);
  }
  for (int i = int(leds::LEDS_NUMBER/2) - 1; i >= 0 ; i--) {
    leds::module1.setPixelColor(i, leds::color[new_color][0], leds::color[new_color][1], leds::color[new_color][2]);
    leds::module1.setPixelColor(leds::LEDS_NUMBER - 1 - i, leds::color[new_color][0], leds::color[new_color][1], leds::color[new_color][2]);
    leds::module2.setPixelColor(i, leds::color[new_color][0], leds::color[new_color][1], leds::color[new_color][2]);
    leds::module2.setPixelColor(leds::LEDS_NUMBER - 1 - i, leds::color[new_color][0], leds::color[new_color][1], leds::color[new_color][2]);
    leds::module1.show();
    leds::module2.show();
    delay(30);
  }
}

void activateBackLight(void){
  for (int i = 0; i < leds::LEDS_NUMBER_HIND; i++) {
    leds::module_back.setPixelColor(i, 255, 0, 0); //Default rear color is RED
  }
}

void updateBlinkers() {
  int side = 0;
  if (leds::leftBlinker) side = leds::LEFT;
  else side = leds::RIGHT;
  
  //Activate blinker every X seconds
  if (millis() - leds::lastMillis > leds::blinkerMillisSpeed) {
    leds::lastMillis = millis();
    leds::blinkerState = !leds::blinkerState;
    
    if (side == leds::RIGHT) {
      for (int i = 0; i < 2; i++) {
        if (leds::blinkerState) {
          leds::module_back.setPixelColor(i, 255, 255, 0);
        } else {
          leds::module_back.setPixelColor(i, 0, 0, 0);
        }
      }
    }
    else if (side == leds::LEFT) {
      for (int i = leds::LEDS_NUMBER_HIND - 1; i > leds::LEDS_NUMBER_HIND - 3; i--) {
        if (leds::blinkerState) {
          leds::module_back.setPixelColor(i, 255, 255, 0);
        } else {
          leds::module_back.setPixelColor(i, 0, 0, 0);
        }
      }
    }
    
  }
}


//
