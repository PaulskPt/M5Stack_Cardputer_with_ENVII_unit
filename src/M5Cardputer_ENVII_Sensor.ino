// M5Stack_Cardputer_ENVII_Sensor.ino
//
/* Example downloaded from: https://github.com/adafruit/Adafruit_BMP280_Library/blob/master/examples/bmp280_sensortest/bmp280_sensortest.ino */
/* @PaulskPt adapted this sketch for the M5Stack Cardputer (with M5 StampS3) */

#include <M5Cardputer.h>
#include <M5UnitENV.h>

SHT3X sht3x;
BMP280 bmp;

#define sda 2
#define scl 1

int dw = 0;  // display width
int dh = 0;  // display height
int title_h = 0; // title horizontal space from left side display
int std_text_size = 1;
int old_text_size = 0;
bool use_sht = false;
bool use_bmp = false;
unsigned int colour = 0;
int hori[] = {0, 80, 120, 200};
int vert[] = {20, 40, 60, 80, 100, 120};
String TITLE =  "ENVII TEST";

void disp_title(void)
{
  M5Cardputer.Display.setCursor(title_h+25, vert[0]-5);
  M5Cardputer.Display.print(TITLE);
}

void disp_frame()
{
    M5Cardputer.Display.fillScreen(BLACK);
    disp_title();
    M5Cardputer.Display.setCursor(0, vert[1]);
    M5Cardputer.Display.print("Temp SHT:");
    M5Cardputer.Display.setCursor(0, vert[2]);
    M5Cardputer.Display.print("Humidity:");
    M5Cardputer.Display.setCursor(0, vert[3]);
    M5Cardputer.Display.print("Temp BMP:");
    M5Cardputer.Display.setCursor(0, vert[4]);
    M5Cardputer.Display.print("Pressure:");
    M5Cardputer.Display.setCursor(0, vert[5]);
    M5Cardputer.Display.print("Approx alt:");
}

void msg_at_bottom(char* msg) {
  int cur_h = 10;
  int cur_v = 10;
  if (strlen(msg) > 0) {
    Serial.println(msg);
    M5Cardputer.Display.setTextSize(std_text_size);
    M5Cardputer.Display.setCursor(cur_h, cur_v + 200);
    M5Cardputer.Display.print(msg);
    delay(5000);
    M5Cardputer.Display.fillRect(cur_h, cur_v + 200, dw-5, 8, BLACK); //erase the LoRa init successful message
  }
}

void setup() {
  auto cfg = M5.config();
  cfg.output_power = true; 
  cfg.external_rtc  = false;  // default=false.
  M5Cardputer.begin(cfg, true);
  M5Cardputer.Power.begin();

  dw = M5Cardputer.Display.width();
  dh = M5Cardputer.Display.height();
  int le = sizeof(TITLE)/sizeof(TITLE[0]);
  title_h = (dh - le) / 4;

  M5Cardputer.Display.setRotation(1);
  M5Cardputer.Display.setTextFont(&fonts::FreeSerif9pt7b);
  M5Cardputer.Display.setTextColor(YELLOW);
  M5Cardputer.Display.setTextDatum(middle_center);
  M5Cardputer.Display.setTextPadding(M5Cardputer.Display.width() - 50);
 
  Serial.begin(115200);
  while ( !Serial ) delay(100);   // wait for native usb
  
  Wire.begin(sda, scl);

  if (!sht3x.begin(&Wire, SHT3X_I2C_ADDR, sda, scl, 400000U)) {
      Serial.println("Couldn't find SHT3X");
      while (1) delay(1);
  }
  else {
    use_sht = true;
  }

  if (!bmp.begin(&Wire, BMP280_I2C_ADDR, sda, scl, 400000U)) {
      Serial.println("Couldn't find BMP280");
      while (1) delay(1);
  }
  else {
    use_bmp = true;
  }

  /* Default settings from datasheet. */
  if (use_bmp) {
    bmp.setSampling(BMP280::MODE_NORMAL,   /* Operating Mode. */
                  BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  BMP280::FILTER_X16,      /* Filtering. */
                  BMP280::STANDBY_MS_500); /* Standby time. */
  }

  String nl = "\n\n";
  String s1 = nl + TITLE; // title;
  Serial.println(s1);

  disp_title();

  if (!use_sht)
    msg_at_bottom("Not using SHT3X)");    
  if (!use_bmp)
    msg_at_bottom("Not using BMP280)");
}

void loop() {
  char sht[]  = "-------- SHT3X  --------";
  char bmp2[] = "-------- BMP280 --------";
  char tmp[]  = "Temperature:   ";
  char tmpc[] = " C";
  char hum[]  = "Humidity:      ";
  char humpct[] = " % rH";
  char hpa[]  = "Pressure:     ";
  char mb[]   = " mb"; // is same as hPa or hectoPascal;
  char alt[]  = "Approx alt: ";
  char altm[] = " m";

  disp_frame();

  while (true)
  {
    M5Cardputer.Display.fillRect(hori[2], vert[1]-8, dw-5, dh-5, BLACK);  // wipe out the variable values
    
    if (use_sht && sht3x.update()) {
      Serial.println("----------SHT3X ----------");
      Serial.print(tmp);
      Serial.print(F(" "));
      Serial.print(sht3x.cTemp);
      Serial.println(tmpc);
      Serial.print(hum);
      Serial.print(F(" "));
      Serial.print(sht3x.humidity);
      Serial.println(humpct);
      Serial.println("--------------------------\r\n");

      M5Cardputer.Display.setCursor(hori[2], vert[1]); 
      M5Cardputer.Display.printf("%6.2f", sht3x.cTemp);
      M5Cardputer.Display.println(tmpc);

      M5Cardputer.Display.setCursor(hori[2], vert[2]);
      M5Cardputer.Display.printf("%6.2f", sht3x.humidity);
      M5Cardputer.Display.println(humpct);
    }

    if (use_bmp && bmp.update()) {
      Serial.println("----------BMP280----------");
      Serial.print(F(tmp));
      Serial.print(F(" "));
      Serial.print(bmp.cTemp);
      Serial.println(tmpc);

      Serial.print(F(hpa));
      Serial.print(bmp.pressure/100);
      Serial.println(mb);

      Serial.print(F(alt));
      Serial.print(F("    "));
      Serial.print(bmp.altitude);
      Serial.println(altm);
      Serial.println("--------------------------\r\n");
      
      M5Cardputer.Display.setCursor(hori[2], vert[3]);
      M5Cardputer.Display.printf("%6.2f",bmp.cTemp);
      M5Cardputer.Display.println(tmpc);

      M5Cardputer.Display.setCursor(hori[2], vert[4]);
      M5Cardputer.Display.printf("%6.2f", bmp.pressure/100);
      M5Cardputer.Display.println(mb);

      M5Cardputer.Display.setCursor(hori[2], vert[5]);
      M5Cardputer.Display.printf("%9.2f", bmp.altitude);
      M5Cardputer.Display.println(altm);
    }
    delay(5000);
  }
}
