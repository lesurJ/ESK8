#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <VescUart.h>

SoftwareSerial HM10(11, 12);
VescUart UART;


namespace leds {
//I/O pins for LEDs signals
const int LEDS_PIN_1 = 7;
const int LEDS_PIN_2 = 8;
const int LEDS_PIN_3 = 9;

const int LEDS_NUMBER = 12;
const int LEDS_NUMBER_HIND = 6;
//colors for the LEDs
const int NB_COLOR = 5;
const int color[NB_COLOR][3] =  {{0, 255, 255}, {255, 0, 0}, {249, 209, 0}, {255, 0, 255}, {150, 150, 150}};

//codes for the LEDs to send over Bluetooth
const int LEDINDEX0 = 0;
const int LEDINDEX1 = 1;
const int LEDINDEX2 = 2;
const int LEDINDEX3 = 3;
const int LEDINDEX4 = 4;
const int LEDSON = 5;
const int LEDSOFF = 6;
const int LEFTBLINKERON = 7;
const int LEFTBLINKEROFF = 8;
const int RIGHTBLINKERON = 9;
const int RIGHTBLINKEROFF = 10;
const int REARLIGHTON = 11;
const int REARLIGHTOFF = 12;
const int FRANCEON = 13;
const int FRANCEOFF = 14;

const int TIME_THRESHOLD = 1000;
const int LEFT = 0; //define LEFT or RIGHT side for the rear light
const int RIGHT = 1;

int selectedColor = 0;
unsigned long lastMillis = 0;
unsigned long blinkerMillisSpeed = 250;
boolean activated = false; //are the LEDs strips activated
boolean leftBlinker = false;
boolean rightBlinker = false;
boolean blinkerState = false;
boolean rearLight = false;
boolean france = false;

//module1 is back_right; module2 is back-left, module hind is rear !
Adafruit_NeoPixel module1 = Adafruit_NeoPixel(LEDS_NUMBER, LEDS_PIN_1, NEO_GRB + NEO_KHZ800); //numb pixel; pin,
Adafruit_NeoPixel module2 = Adafruit_NeoPixel(LEDS_NUMBER, LEDS_PIN_2, NEO_GRB + NEO_KHZ800); //numb pixel; pin,
Adafruit_NeoPixel module_hind = Adafruit_NeoPixel(LEDS_NUMBER_HIND, LEDS_PIN_3, NEO_GRB + NEO_KHZ800); //numb pixel; pin,
}

namespace BLE {
const unsigned long BAUDRATE = 9600;
unsigned long cnt = 0;
char c = ' ';
String myString = "";
}

void setup() {
  //initialization for the namespace leds
  leds::module1.begin();
  leds::module2.begin();
  leds::module_hind.begin();

  leds::module1.setBrightness(200);
  leds::module2.setBrightness(200);
  leds::module_hind.setBrightness(200);

  pinMode(leds::LEDS_PIN_1, OUTPUT);
  pinMode(leds::LEDS_PIN_2, OUTPUT);
  pinMode(leds::LEDS_PIN_3, OUTPUT);

  //initialization for the HM10
  HM10.begin(BLE::BAUDRATE);
  delay(10);
}



void loop() {
  communicateUpdate();
  updateLeds();

  delay(50);
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
  //This function selects the appropriate colors and activations for the LEDs strips
  if (leds::activated) {
    int k = leds::selectedColor;
    for (int i = 0; i < leds::LEDS_NUMBER; i++) {
      leds::module1.setPixelColor(i, leds::color[k][0], leds::color[k][1], leds::color[k][2]);
      leds::module2.setPixelColor(i, leds::color[k][0], leds::color[k][1], leds::color[k][2]);
    }
  } else {
    leds::module1.clear();
    leds::module2.clear();
  }

  if (leds::rearLight) {
    for (int i = 0; i < leds::LEDS_NUMBER_HIND; i++) {
      leds::module_hind.setPixelColor(i, 255, 0, 0); //Default rear color is RED
    }
    if (leds::leftBlinker) {
      blinker(leds::LEFT);
    } else if (leds::rightBlinker) {
      blinker(leds::RIGHT);
    } else if (leds::france) {
      france();
    }
  } else {
    leds::module_hind.clear();
  }

  leds::module1.show();
  leds::module2.show();
  leds::module_hind.show();
}

void france(void) {
  //Display french flag on the rear light. Can easily display another flag ;)
  for (int i = 0; i < leds::LEDS_NUMBER_HIND; i++) {
    if (i < 2) leds::module_hind.setPixelColor(i, 255, 0, 0);
    else if (i < 4) leds::module_hind.setPixelColor(i, 255, 255, 255);
    else leds::module_hind.setPixelColor(i, 0, 0, 255);
  }
}

void blinker(int side) {
  //turn off the leds in the center
  /*
    for (int i = 2; i < 4; i++) {
      leds::module_hind.setPixelColor(i, 0, 0, 0);
    }*/
  //Activate blinker every X seconds
  if (millis() - leds::lastMillis > leds::blinkerMillisSpeed) {
    leds::lastMillis = millis();
    leds::blinkerState = !leds::blinkerState;
  }
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



//
