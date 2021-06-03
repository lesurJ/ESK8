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
  leds::module_hind.begin();

  leds::module1.setBrightness(200);
  leds::module2.setBrightness(200);
  leds::module_hind.setBrightness(200);

  init_france(false);
  

  //initialization for the HM10
  HM10.begin(BLE::BAUDRATE);
  delay(10);
}



void loop() {
  communicateUpdate();
  if(leds::newCommand){
    updateLeds();
  }
  updateBlinkers();
  delay(5);
}

void communicateUpdate(void) {
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
        leds::newCommand = true;
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
      leds::selectedColor = leds::LEDINDEX0;
      break;

    case leds::LEDINDEX1:
      leds::selectedColor = leds::LEDINDEX1;
      break;

    case leds::LEDINDEX2:
      leds::selectedColor = leds::LEDINDEX2;
      break;

    case leds::LEDINDEX3:
      leds::selectedColor = leds::LEDINDEX3;
      break;

    case leds::LEDINDEX4:
      leds::selectedColor = leds::LEDINDEX4;
      break;

    case leds::LEDSON:
      leds::activated = true;
      break;

    case leds::LEDSOFF:
      leds::activated = false;
      break;

    case leds::LEFTBLINKERON:
      leds::leftBlinker = true;
      break;

    case leds::LEFTBLINKEROFF:
      leds::leftBlinker = false;
      break;

    case leds::RIGHTBLINKERON:
      leds::rightBlinker = true;
      break;

    case leds::RIGHTBLINKEROFF:
      leds::rightBlinker = false;
      break;

    case leds::REARLIGHTON:
      leds::rearLight = true;
      break;

    case leds::REARLIGHTOFF:
      leds::rearLight = false;
      break;

    case leds::FRANCEON:
      leds::france = true;
      break;

    case leds::FRANCEOFF:
      leds::france = false;
      break;
  }
}

void updateLeds(void) {
  leds::newCommand = false;
  //This function selects the appropriate colors and activations for the LEDs strips
  if (leds::activated) {
    //int k = leds::selectedColor;
    changeLedColor(leds::selectedColor);
    //for (int i = 0; i < leds::LEDS_NUMBER; i++) {
    //  leds::module1.setPixelColor(i, leds::color[k][0], leds::color[k][1], leds::color[k][2]);
    //  leds::module2.setPixelColor(i, leds::color[k][0], leds::color[k][1], leds::color[k][2]);
    //}
  } else {
    leds::module1.clear();
    leds::module2.clear();
  }

  if (leds::rearLight) {
    for (int i = 0; i < leds::LEDS_NUMBER_HIND; i++) {
        leds::module_hind.setPixelColor(i, 255, 0, 0); //Default rear color is RED
    }
    if (leds::france) {
      init_france(true);
    } 
  }
  else {
    leds::module_hind.clear();
  }

  leds::module1.show();
  leds::module2.show();
  leds::module_hind.show();
}

void changeLedColor(int new_color){
  for (int i = 0; i < int(leds::LEDS_NUMBER/2); i++) {
    leds::module1.clear();
    leds::module2.clear();
    leds::module1.setPixelColor(i, leds::color[new_color][0], leds::color[new_color][1], leds::color[new_color][2]);
    leds::module1.setPixelColor(leds::LEDS_NUMBER - 1 - i, leds::color[new_color][0], leds::color[new_color][1], leds::color[new_color][2]);
    leds::module2.setPixelColor(i, leds::color[new_color][0], leds::color[new_color][1], leds::color[new_color][2]);
    leds::module2.setPixelColor(leds::LEDS_NUMBER - 1 - i, leds::color[new_color][0], leds::color[new_color][1], leds::color[new_color][2]);
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
    delay(20);
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
          leds::module_hind.setPixelColor(i, 255, 255, 0);
        } else {
          leds::module_hind.setPixelColor(i, 0, 0, 0);
        }
      }
    }
    else if (side == leds::LEFT) {
      for (int i = leds::LEDS_NUMBER_HIND - 1; i > leds::LEDS_NUMBER_HIND - 3; i--) {
        if (leds::blinkerState) {
          leds::module_hind.setPixelColor(i, 255, 255, 0);
        } else {
          leds::module_hind.setPixelColor(i, 0, 0, 0);
        }
      }
    }
    
  }
}


//
