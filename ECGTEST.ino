#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <SPI.h>
#include <TouchScreen.h>


#define MINPRESSURE 200
#define MAXPRESSURE 1000

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define MAROON  0x7800




MCUFRIEND_kbv tft;

Adafruit_GFX_Button on_btn;

const int TFT_WIDTH = 480;
const int TFT_HEIGHT = 320;
uint16_t DisplayRepeat = TFT_WIDTH;
uint8_t ignoreBeats = 0;
int currentpage = 0;
uint8_t Trace[TFT_WIDTH];
int i = 0;
int yGap = 5;
int xGap = 5;
const int SamplePeriod = 5;
int j = 0;


const int XP = 6, XM = A2, YP = A1, YM = 7; 
const int TS_BOT = 907, TS_TOP = 136, TS_LEFT = 942, TS_RT = 139;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
int pixel_x, pixel_y;  
bool Touch_getXY(void)
{
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);   
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed) {
        pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width()); 
        pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
    }
    return pressed;
}




void setup() {
  uint16_t ID = tft.readID();
  Serial.begin(9600);
  tft.begin(0x9486);
  tft.setRotation(3);
  tft.fillScreen(0); 
  drawhomescreen();
  on_btn.initButton(&tft,  150, 255, 200, 100, CYAN, BLACK, WHITE, "ECG", 5);
  on_btn.drawButton(false);
}

void loop() {
 
  bool down = Touch_getXY();
    on_btn.press(down && on_btn.contains(pixel_x, pixel_y));
 
    if (on_btn.justPressed()) {
        on_btn.drawButton(true);  
        DrawGridLarge();
        
        currentpage=1;        
    }
    if(currentpage == 1){
      int ecgValue = analogRead(A5);
      DrawTraceLarge(ecgValue/2);
    }
  delay(10); 
}








void drawhomescreen(){
  tft.fillScreen(WHITE);
  showmsgXY(tft.width()/2-50, 10, 6, NULL, "TMI");
  showmsgXY(0, 100, 4, NULL,"LOW COST ECG MONITOR");
  showmsgXY2(300, 200, 2, NULL,"NAME 1");
  showmsgXY2(300, 230, 2, NULL,"NAME 2");
  showmsgXY2(300, 260, 2, NULL,"NAME 3");
  showmsgXY2(300, 290, 2, NULL,"NAME 4");
}

void DrawGridLarge(void)
{
  int i;
  tft.fillScreen(0);   
  for (i=0; i < tft.height(); i++)
    if (i % 8 == 0)
      tft.drawFastHLine(0, i, tft.width(), MAROON); 
  for (i=0; i < tft.width(); i++)
    if (i*25 % (1000/SamplePeriod) == 0)
      tft.drawFastVLine(i, 0, tft.height(), MAROON); 
      
  for (i=0; i <  tft.height(); i++)
    if (i % 40 == 0)
      tft.drawFastHLine(0, i, tft.width(), RED); 
  for (i=0; i < tft.width(); i++)
    if (i*5 % (1000/SamplePeriod) == 0)
     tft.drawFastVLine(i, 0,  tft.height(), RED);
}

void DrawTraceLarge(uint8_t y)
{
  static uint16_t x = 0;
  static uint8_t pt = 0;
  static uint8_t py = 0;
  int i;

  x++;
  x = x % DisplayRepeat;
  y = TFT_HEIGHT-y;
  
  if (x < TFT_WIDTH) {
    DrawGridVLine(x, pt, Trace[x]);
    if (y >= py)
      tft.drawFastVLine(x, py, y-py+1, TFT_WHITE); else
      tft.drawFastVLine(x, y, py-y+1, TFT_WHITE);
    py = y;

    pt = Trace[x];
    Trace[x] = y;
  }
}

void DrawGridVLine(int x, int y1, int y2)
{
  int y;
  
  if (y1 > y2)
    DrawGridVLine(x, y2, y1); else
  {
    if (x*5 % (1000/SamplePeriod) == 0)
      tft.drawFastVLine(x, y1, y2-y1+1, RED); else
    if (x*25 % (1000/SamplePeriod) == 0)
      tft.drawFastVLine(x, y1, y2-y1+1, MAROON); else
    {
      tft.drawFastVLine(x, y1, y2-y1+1, BLACK);   
      for (y=y1; y <= y2; y++)
        if (y % 40 == 0)
          tft.drawPixel(x,y,RED); else
        if (y % 8 == 0)
          tft.drawPixel(x,y,MAROON); 
    }
  }
}

void showmsgXY(int x, int y, int sz, const GFXfont *f, const char *msg)
{
    int16_t x1, y1;
    uint16_t wid, ht;
    tft.setFont(f);
    tft.setCursor(x, y);
    tft.setTextColor(RED);
    tft.setTextSize(sz);
    tft.print(msg);
    delay(1000);
}
void showmsgXY2(int x, int y, int sz, const GFXfont *f, const char *msg)
{
    int16_t x1, y1;
    uint16_t wid, ht;
    tft.setFont(f);
    tft.setCursor(x, y);
    tft.setTextColor(BLACK);
    tft.setTextSize(sz);
    tft.print(msg);
    delay(1000);
}
