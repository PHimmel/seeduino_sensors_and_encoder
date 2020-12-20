
// temp/humidity library
#include <DHT.h>
#include <DHT_U.h>

// DHT definitions
#define DHT_PIN 3
#define DHTTYPE DHT11

// init DHT object
DHT dht(DHT_PIN, DHTTYPE);

// OLED library
#include "ssd1306.h"
#include "nano_gfx.h"

// rotary variables
const int BUTTON = 10;
const int outputB = 9;
const int outputA = 8;

int counter = 0;
int aState;
int aLastState;
int location = 0;

SAppMenu menu;

const char *menuItems[] =
{
  "Direction",
  "Temp/Humidity"
};

void setup() {
  pinMode(outputA, INPUT);
  pinMode(outputB, INPUT);
  pinMode(BUTTON, INPUT);

  dht.begin();

  // Reads the initial state of the outputA
  aLastState = digitalRead(outputA);
  
  /* Select the font to use with menu and all font functions */
  ssd1306_setFixedFont(ssd1306xled_font6x8);

  // init
  ssd1306_128x64_i2c_init();

  //clear
  ssd1306_clearScreen();

  ssd1306_createMenu( &menu, menuItems, sizeof(menuItems) / sizeof(char *) );
  ssd1306_showMenu( &menu );
} // setup

void loop() {
  encoder();
  button();
} // loop()

void menu_select() {
  delay(1000);
  switch (ssd1306_menuSelection(&menu))
  {
    case 0 : {
        direction_selection();
        break;
      }
    case 1:
      ssd1306_clearScreen();
      temp_humid();
      delay(6000);
      break;

    default:
      break;
  }

  ssd1306_clearScreen( );
  ssd1306_showMenu(&menu);

  // needed - else button press state creates loop
  delay(500);

}

void encoder() {
  int aState = digitalRead(outputA);

  if (aState != aLastState) {
    if (digitalRead(outputB) != aState) {
      ssd1306_clearScreen( );
      ssd1306_menuUp(&menu);
      ssd1306_updateMenu(&menu);
    }
    else {
      ssd1306_clearScreen( );
      ssd1306_menuDown(&menu);
      ssd1306_updateMenu(&menu);
    }
    aLastState = aState;
  }
}

void button() {
  if (digitalRead(BUTTON) == LOW) {
    menu_select();
  }
}

void direction_selection() {

  while (digitalRead(BUTTON) != LOW) {
    aState = digitalRead(outputA); // Reads the "current" state of the outputA

    // If the previous and the current state of the outputA are different, that means a Pulse has occured
    if (aState != aLastState) {

      // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
      if (digitalRead(outputB) != aState) {
        ++counter;

        char buf[16]; // buffer
        sprintf(buf, "%d", counter);
        const char* direct = buf;

        ssd1306_printFixed(0,  8, "Direction", STYLE_NORMAL);
        ssd1306_printFixed(0,  16, direct, STYLE_NORMAL);
        delay(300);
        
      } else {
        --counter;
        char buf[16]; // buffer
        sprintf(buf, "%d", counter);
        const char* direct = buf;

        ssd1306_printFixed(0,  8, "Direction", STYLE_NORMAL);
        ssd1306_printFixed(0,  16, direct, STYLE_NORMAL);
        delay(300);
      }
    }
    aLastState = aState; // Updates the previous state of the outputA with the current state
    ssd1306_clearScreen();
  }
}

void temp_humid() {
  float temp = dht.readTemperature();
  temp = (temp * 9 / 5) + 32;

  float humid = dht.readHumidity();

  char buf[16]; // buffer
  sprintf(buf, "%.2f 'F", temp);
  const char* temp_ = buf;
  ssd1306_printFixed(0, 8, temp_, STYLE_NORMAL);

  char buff[16]; // buffer
  sprintf(buff, "%.2f R.H.", humid);
  const char* humid_ = buff;
  ssd1306_printFixed(0,  24, humid_, STYLE_NORMAL);
}

// attempt to reuse functionality -> used in at least 4 locations --- does not currently work
const char* get_direction() {
  char buf[16]; // buffer
  sprintf(buf, "%d", counter);
  const char* direct = buf;
  return direct;
}
