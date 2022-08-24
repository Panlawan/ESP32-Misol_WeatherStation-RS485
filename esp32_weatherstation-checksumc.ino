#include <ModbusMaster.h>

// #define MAX485_DE 25
int data;
String dataWeather;
String dataRead;
String trans;
int wdir;
float temp, humid, wspd, gspd, rain, uv, light, pres;
int readBin(char *s);
int WindDirection();
float sendCom(int sta, int sto);

#define RXD2 16
#define TXD2 17

void setup() {
  delay(2000);
  Serial.begin(9600);
  Serial.println("Start!");
  delay(200);
  pinMode(25, INPUT);
  pinMode(26, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  digitalWrite(MAX485_DE, 0);
  delay(200);
  Serial2.begin(9600, SERIAL_8N2);

  Serial.println("Leave setup!");
}

const int BUFFER_SIZE = 50;
char buf[BUFFER_SIZE];

void loop() {
  uint8_t sum = 0;
  while (!Serial2.available() > 0) {
    Serial.print(".");
    delay(500);
  }
  if (Serial2.available() > 0)
  {
    data = Serial2.readBytes(buf, BUFFER_SIZE);
    //    Serial.println(data);
    for (int i = 0; i < data; i++) {
      Serial.print(buf[i], HEX);
      dataWeather = String(buf[i], HEX);

      if (dataWeather == "0" || dataWeather == "1" || dataWeather == "2" || dataWeather == "3" || dataWeather == "4" || dataWeather == "5" || dataWeather == "6" || dataWeather == "7" || dataWeather == "8" || dataWeather == "9") {
        dataWeather = "0" + dataWeather;
      }
      if (dataWeather == "a" || dataWeather == "b" || dataWeather == "c" || dataWeather == "d" || dataWeather == "e" || dataWeather == "f") {
        dataWeather = "0" + dataWeather ;
      }

      dataRead += dataWeather;

      Serial.print(" ");
      if (i <= 15) {
        sum = sum + buf[i];
      }
    }
    if (sum == buf[16]) {
      Serial.println();
      Serial.println(dataRead);
      wdir = WindDirection();
      delay(10);
      temp = (sendCom(7, 10) - 400) / 10;
      delay(10);
      humid = sendCom(10, 12);
      delay(10);
      wspd = sendCom(12, 14) / 8 * 1.12;
      delay(10);
      gspd = sendCom(14, 16) * 1.12;
      delay(10);
      rain = sendCom(16, 20) * 0.3;
      delay(10);
      uv = sendCom(20, 24);
      delay(10);
      light = sendCom(24, 30) / 10;
      delay(10);
      pres = sendCom(34, 40) / 10;
      delay(10);
      dataRead = "";

      Serial.print("Wind dir: ");     Serial.print(wdir);     Serial.print("\t\t");
      Serial.print("Wind spd: ");     Serial.print(wspd);     Serial.print("\t\t");
      Serial.print("Gust spd: ");     Serial.println(gspd);

      Serial.print("Temperature: ");  Serial.print(temp);     Serial.print("\t\t");
      Serial.print("Humidity: ");     Serial.print(humid);    Serial.print("\t\t");
      Serial.print("Pressure: ");     Serial.println(pres);

      Serial.print("Rain: ");         Serial.print(rain);     Serial.print("\t\t");
      Serial.print("UV: ");           Serial.print(uv);       Serial.print("\t\t");
      Serial.print("Light: ");        Serial.println(light);
      Serial.print("\n\n");
    }
  }
}


int readBin(char *s) {
  int result = 0;
  while (*s) {
    result <<= 1;
    if (*s++ == '1') result |= 1;
  }
  return result;
}

String transCharToInt(String _dataSendser, int _start, int _stop) //char to int?
{
  String val = _dataSendser.substring(_start, _stop);
  return val;
}

int WindDirection()   //Wind Direction
{
  trans = transCharToInt(dataRead, 4, 7);
  int16_t x = strtol(trans.c_str(), NULL, 16);
  int num = x;
  delay(100);
  uint8_t bitsCount = sizeof( num ) * 3;
  char str[ bitsCount + 1 ];

  uint8_t i = 0;
  while ( bitsCount-- )
    str[ i++ ] = bitRead( num, bitsCount ) + '0';
  str[ i ] = '\0';
  String sText;
  for (int i = 0; i < 8; i++) {
    sText += str[i];
    if (i == 7) {
      char buf[10];
      sText.toCharArray(buf, 10);
      int values = readBin(buf);
      return values;
    }
  }
}

float sendCom(int sta, int sto)
{
  trans = transCharToInt(dataRead, sta, sto) ;

  char input[10];
  trans.toCharArray(input, 10);
  int x;
  char *endptr;

  x = strtol(input, &endptr, 16);
  String m = String(x, DEC);
  float xx = m.toFloat();
  return xx;
}
