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
const int FRANCE[3][3] =  {{255, 0, 0}, {255, 255, 255}, {0, 0, 255}};


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
boolean newCommand = true; // was a new command received

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


void truc(){
  int q = 5;
}

void france(void){
  for(int j=2; j >=0; j--){
    leds::module_hind.setPixelColor(2*j, leds::FRANCE[j][0],leds::FRANCE[j][1], leds::FRANCE[j][2]);
    leds::module_hind.setPixelColor(2*j+1, leds::FRANCE[j][0], leds::FRANCE[j][1], leds::FRANCE[j][2]);
  }
}

void init_france(boolean stay){
  for(int j=2; j >=0; j--){
    for(int w=0; w < 255; w+=5){
      leds::module_hind.clear();
      leds::module_hind.setPixelColor(2*j, w/float(255)*leds::FRANCE[j][0], w/float(255)*leds::FRANCE[j][1], w/float(255)*leds::FRANCE[j][2]);
      leds::module_hind.setPixelColor(2*j+1,  w/float(255)*leds::FRANCE[j][0], w/float(255)*leds::FRANCE[j][1], w/float(255)*leds::FRANCE[j][2]);
      leds::module_hind.show();
      delay(10);
    }
  }
  if (stay) france();
  leds::module_hind.show();
}


// EOF
