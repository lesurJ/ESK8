//#include "MPU9250.h"
//#include <SPI.h>
//#include <Wire.h>
#include <Adafruit_NeoPixel.h>
//#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>

#include <VescUart.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define X 0
#define Y 1
#define Z 2

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
SoftwareSerial HM10(11, 12);
VescUart UART;

/*
namespace imu {
const int window_size = 5;
float g_x = 0;
float g_y = 0;
float g_z = 0;
float temp = 0;
float tab_x[window_size];
float tab_y[window_size];
float tab_z[window_size];
float average_x = 0;
float average_y = 0;
float average_z = 0;
int cnt = 0;
}


namespace imu2 {
const int MPU = 0x68;
MPU9250 IMU(Wire, 0x68);
float acc[3] = {0, 0, 0};
float acc_error[2] = {0, 0};
float acc_angle[2] = {0, 0};
float gyro[3] = {0, 0, 0};
float gyro_error[3] = {0, 0, 0};
float gyro_angle[3] = {0, 0, 0};
float roll, pitch, yaw;
float elapsed_t, current_t, previous_t;
int c = 0;
}
*/

/*
namespace skate {
const int window_size = 5;
const int pole_number = 14;
const int hall_pin = 10;
const float wheel_diam = 0.080; //80 mm
const float ratio = 2.4;
const int max_speed = 40;
float _speed = 0;
float averaged_speed = 0;
float tab_speed[window_size];
int cnt = 0;
}
*/


namespace leds {
const int LEDS_PIN_1 = 7;
const int LEDS_PIN_2 = 8;
const int LEDS_PIN_3 = 9;

const int LEDS_NUMBER = 12;
const int LEDS_NUMBER_HIND = 6;
const int NB_COLOR = 5;

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
const int LEFT = 0;
const int RIGHT = 1;
const int color[NB_COLOR][3] =  {{0, 255, 255}, {255, 0, 0}, {0, 255, 0}, {255, 0, 255}, {255, 255, 255}};

int selectedColor = 0;
unsigned long lastMillis = 0;
unsigned long blinkerMillisSpeed = 250;
boolean activated = false;
boolean leftBlinker = false;
boolean rightBlinker = false;
boolean blinkerState = false;
boolean rearLight = false;
boolean france = false;



int time1 = 0;
int rainbow_cnt = 0;
boolean set = false;
boolean brake_leds_activated = true;

//module1 is back_right; module2 is back-left
Adafruit_NeoPixel module1 = Adafruit_NeoPixel(LEDS_NUMBER, LEDS_PIN_1, NEO_GRB + NEO_KHZ800); //numb pixel; pin,
Adafruit_NeoPixel module2 = Adafruit_NeoPixel(LEDS_NUMBER, LEDS_PIN_2, NEO_GRB + NEO_KHZ800); //numb pixel; pin,
Adafruit_NeoPixel module_hind = Adafruit_NeoPixel(LEDS_NUMBER_HIND, LEDS_PIN_3, NEO_GRB + NEO_KHZ800); //numb pixel; pin,
}

namespace BLE {
const unsigned long BAUDRATE = 9600;
unsigned long cnt = 0;
char c = ' ';
String myString = "";
boolean newCode = false;
}

void setup() {
  /*
    Serial.begin(19200);
    Wire.begin();
    Wire.beginTransmission(imu2::MPU);
    Wire.write(0x6B);
    Wire.write(0x00);
    Wire.endTransmission(true);

    calibrate_imu_error();
    //initialization for the namespace imu
    int status = IMU.begin();
    if (status < 0) {
    Serial.println("IMU initialization unsuccessful");
    while (1) {}
    }
  */

  //initialization for the namespace leds
  leds::module1.begin();
  leds::module2.begin();
  leds::module_hind.begin();

  leds::module1.setBrightness(200);
  leds::module2.setBrightness(200);
  leds::module_hind.setBrightness(255);

  pinMode(leds::LEDS_PIN_1, OUTPUT);
  pinMode(leds::LEDS_PIN_2, OUTPUT);
  pinMode(leds::LEDS_PIN_3, OUTPUT);

  //initialization for the HM10
  HM10.begin(BLE::BAUDRATE);
  delay(10);


  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  displayCode("test");
  display.display();
  delay(1000);


  //Serial.begin(19200);
  //UART.setSerialPort(&Serial);
}



void loop() {
  communicateUpdate();
  updateLeds();
 
  

  delay(50);
}

void displayCode(String str) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(str);
  display.display();
}

void communicateUpdate(void) {
  if (HM10.available()) {
    while (HM10.available()) {
      BLE::c = HM10.read();
      if (BLE::c == '<') { //start of message : it is a new message
        BLE::myString = "";
      }
      BLE::myString = BLE::myString + String(BLE::c);
      if (BLE::c == '>') {
        BLE::newCode = true;
        BLE::myString.replace("<", " ");
        BLE::myString.replace(">", " ");
        BLE::myString.trim();
        //displayCode(BLE::myString);
        decodeString(BLE::myString);
      }
    }
  }

  /*
    else if (BLE::cnt == 1000) {
    BLE::cnt = 0;
    HM10.print("<" + String(random(10)) + "," + String(random(150)) + ">");
    }
    BLE::cnt += 1;
  */
}

void decodeString(String str) {
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
    //Default rear color is RED
    for (int i = 0; i < leds::LEDS_NUMBER_HIND; i++) {
      leds::module_hind.setPixelColor(i, 255, 0, 0);
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
  if (side == leds::LEFT) {
    for (int i = leds::LEDS_NUMBER_HIND - 1; i > leds::LEDS_NUMBER_HIND - 3; i--) {
      if (leds::blinkerState) {
        leds::module_hind.setPixelColor(i, 255, 255, 0);
      } else {
        leds::module_hind.setPixelColor(i, 0, 0, 0);
      }
    }
  }

}





















/*
  void update_imu() {
  imu::IMU.readSensor();

  imu::g_x = IMU.getAccelY_mss(); //x and y are inverted
  imu::g_y = IMU.getAccelX_mss();
  imu::g_z = IMU.getAccelZ_mss();
  imu::temp = IMU.getTemperature_C();

  imu::average_x = get_moving_average(imu::g_x, imu::tab_x, imu::cnt, imu::window_size);
  imu::average_y = get_moving_average(imu::g_y, imu::tab_y, imu::cnt, imu::window_size);
  imu::average_z = get_moving_average(imu::g_z, imu::tab_z, imu::cnt, imu::window_size);

  imu::cnt = (imu::cnt + 1) % imu::window_size;
  }
*/

/*
float get_moving_average(float val, float tab[], int cnt, int _size) {
  float average = 0.;
  tab[cnt] = val;
  for (int j = 0; j < _size; j++) {
    average += tab[j];
  }
  average = float(average) / float(_size);
  return average;
}


void calibrate_imu_error(void) {
  while (imu2::c < 200) {
    Wire.beginTransmission(imu2::MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(imu2::MPU, 6, true);
    imu2::acc[X] = (Wire.read() << 8 | Wire.read()) / 16384.0;
    imu2::acc[Y] = (Wire.read() << 8 | Wire.read()) / 16384.0;
    imu2::acc[Z] = (Wire.read() << 8 | Wire.read()) / 16384.0;

    imu2::acc_error[X] += ((atan((imu2::acc[Y]) / sqrt(pow(imu2::acc[X], 2) + pow(imu2::acc[Z], 2))) * 180 / PI));
    imu2::acc_error[Y] += ((atan((-imu2::acc[X]) / sqrt(pow(imu2::acc[Y], 2) + pow(imu2::acc[Z], 2))) * 180 / PI));
    imu2::c++;
  }

  imu2::acc_error[X] /= 200;
  imu2::acc_error[Y] /= 200;
  imu2::c = 0;

  while (imu2::c < 200) {
    Wire.beginTransmission(imu2::MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(imu2::MPU, 6, true);
    imu2::gyro[X] = Wire.read() << 8 | Wire.read();
    imu2::gyro[Y] = Wire.read() << 8 | Wire.read();
    imu2::gyro[Z] = Wire.read() << 8 | Wire.read();

    imu2::gyro_error[X] += imu2::gyro[X] / 131;
    imu2::gyro_error[Y] += imu2::gyro[Y] / 131;
    imu2::gyro_error[Z] += imu2::gyro[Z] / 131;
    imu2::c++;
  }

  imu2::gyro_error[X] /= 200;
  imu2::gyro_error[Y] /= 200;
  imu2::gyro_error[Z] /= 200;

  for (int i = 0; i < 2; i++) {
    Serial.print(imu2::acc_error[i]);
    Serial.print("  ");
  }
  for (int i = 0; i < 3; i++) {
    Serial.print(imu2::gyro_error[i]);
    Serial.print("  ");
  }
  Serial.println();
}

void update_angles(void) {

  Wire.beginTransmission(imu2::MPU);
  Wire.write(0x3B); //Address for accelerometer's data
  Wire.endTransmission(false);
  Wire.requestFrom(imu2::MPU, 6, true); //request 6 bytes from slave at address MPU
  imu2::acc[X] = (Wire.read() << 8 | Wire.read()) / 16384.0;
  imu2::acc[Y] = (Wire.read() << 8 | Wire.read()) / 16384.0;
  imu2::acc[Z] = (Wire.read() << 8 | Wire.read()) / 16384.0;

  imu2::acc_angle[X] = ((atan((imu2::acc[Y]) / sqrt(pow(imu2::acc[X], 2) + pow(imu2::acc[Z], 2))) * 180 / PI)) - imu2::acc_error[X];
  imu2::acc_angle[Y] = ((atan((-imu2::acc[X]) / sqrt(pow(imu2::acc[Y], 2) + pow(imu2::acc[Z], 2))) * 180 / PI)) - imu2::acc_error[Y];

  imu2::previous_t = imu2::current_t;
  imu2::current_t = millis();
  imu2::elapsed_t = (imu2::current_t - imu2::previous_t) / 1000;


  Wire.beginTransmission(imu2::MPU);
  Wire.write(0x43); //address for gyro's data
  Wire.endTransmission(false);
  Wire.requestFrom(imu2::MPU, 6, true);
  imu2::gyro[X] = (Wire.read() << 8 | Wire.read()) / 131 - imu2::gyro_error[X]; //131 comes from gyro_sensitivity check datasheet in arduino folder
  imu2::gyro[Y] = (Wire.read() << 8 | Wire.read()) / 131 - imu2::gyro_error[Y];
  imu2::gyro[Z] = (Wire.read() << 8 | Wire.read()) / 131 - imu2::gyro_error[Z];

  imu2::gyro_angle[X] += imu2::gyro[X] * imu2::elapsed_t;
  imu2::gyro_angle[Y] += imu2::gyro[Y] * imu2::elapsed_t;
  imu2::yaw += imu2::gyro[Z] * imu2::elapsed_t;

  imu2::roll = 0.96 * imu2::gyro_angle[X] + 0.04 * imu2::acc_angle[X];
  imu2::pitch = 0.96 * imu2::gyro_angle[Y] + 0.04 * imu2::acc_angle[Y];

  //Serial.print(imu2::yaw);
  //Serial.print("  ");
  //Serial.print(imu2::roll);
  //Serial.print("  ");
  //Serial.println(imu2::pitch);
}


*/








//
