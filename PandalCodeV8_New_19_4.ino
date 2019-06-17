#include <FastLED.h>

#define LED_PIN  5
#define BRIGHTNESS 255
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define LED_STRIPS 48
#define LEDS_IN_STRIP 28
#define NUM_LEDS  (LED_STRIPS*LEDS_IN_STRIP)

#define UPDATES_PER_SECOND 100

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

CRGBPalette16 targetPalette;

extern CRGBPalette16 myBuddhistFlagPalette;
extern const TProgmemPalette16 myBuddhistFlagPalette_p PROGMEM;

//extern const TProgmemRGBPalette16 RedOrange_p PROGMEM;
//extern const TProgmemRGBPalette16 Purples_p PROGMEM;
//extern const TProgmemRGBPalette16 temperature_gp PROGMEM;
extern const TProgmemRGBPalette16 Snow_p PROGMEM;

// XY Code BEGIN ***************
const uint8_t kMatrixWidth = LEDS_IN_STRIP;
const uint8_t kMatrixHeight = LED_STRIPS;
const bool    kMatrixSerpentineLayout = true;

uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;
  
  if( kMatrixSerpentineLayout == false) {
    i = (y * kMatrixWidth) + x;
  }

  if( kMatrixSerpentineLayout == true) {
    if( y & 0x01) {
      // Odd rows run backwards
      uint8_t reverseX = (kMatrixWidth - 1) - x;
      i = (y * kMatrixWidth) + reverseX;
    } else {
      // Even rows run forwards
      i = (y * kMatrixWidth) + x;
    }
  }
  
  return i;
}

CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
CRGB* const leds( leds_plus_safety_pixel + 1);

uint16_t XYsafe( uint8_t x, uint8_t y)
{
  if( x >= kMatrixWidth) return -1;
  if( y >= kMatrixHeight) return -1;
  return XY(x,y);
}
// XY Code END ***************

void setup() {
    delay( 3000 ); // power-up safety delay
    
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    
    FastLED.setBrightness(  BRIGHTNESS );
   
    currentPalette = RainbowColors_p;
    targetPalette = myBuddhistFlagPalette_p;
    
    currentBlending = LINEARBLEND;
}

static uint8_t startIndex = 0;
static uint8_t loopIndex = 0;
static uint8_t lastCheck = 0;
void loop()
{
    ChangePalettePeriodically();

    uint8_t maxChanges = 10; 
    nblendPaletteTowardPalette( currentPalette, targetPalette, maxChanges);
  
    startIndex = startIndex + 5; /* motion speed */
    Patterns(startIndex);
    
    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
}

//XY metrix code BEGIN
void XYmatrix()
{
    uint32_t ms = millis();
    int32_t yHueDelta32 = ((int32_t)cos16( ms * (27/1) ) * (350 / kMatrixWidth));
    int32_t xHueDelta32 = ((int32_t)cos16( ms * (39/1) ) * (310 / kMatrixHeight));
    DrawOneFrame( ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
    if( ms < 5000 ) {
      FastLED.setBrightness( scale8( BRIGHTNESS, (ms * 256) / 5000));
    } else {
      FastLED.setBrightness(BRIGHTNESS);
    }
}

void DrawOneFrame( byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8)
{
  byte lineStartHue = startHue8;
  for( byte y = 0; y < kMatrixHeight; y++) {
    lineStartHue += yHueDelta8;
    byte pixelHue = lineStartHue;      
    for( byte x = 0; x < kMatrixWidth; x++) {
      pixelHue += xHueDelta8;
      leds[ XYsafe(x, y)]  = CHSV( pixelHue, 255, 255);
    }
  }
}
//XY metrix code END

void Patterns( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    uint8_t secondsCheck = (millis() / 50000) % 2;

    if (lastCheck != secondsCheck){
      loopIndex = loopIndex + 1;
    }
    lastCheck = secondsCheck;

    if (loopIndex > 11){
      loopIndex = 0;
    }

    int m = 0;
    CRGB color = CRGB::Blue;
    
    switch(loopIndex){
//      case 1:
//        INOUT(HeatColors_p, colorIndex, brightness, LINEARBLEND);
//        break;
          
      case 8:
        INOUT(currentPalette, colorIndex, brightness, currentBlending);
        break;

      case 9:
        for( int i = 0; i < NUM_LEDS; i++) {
          leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
          colorIndex = colorIndex + 3;
        }
        break;

      case 7:
        XYmatrix();
        break;

      case 1:
        INOUT(LavaColors_p, colorIndex, brightness, LINEARBLEND);
        break;
        
      case 6:
        for( int i = LEDS_IN_STRIP-1; i >-1; i--) {
          for( int j = 0; j < LED_STRIPS; j++) {
            if((i>3 && i<10) || (i>13)){
              if((j % 2) == 0){
                leds[i+LEDS_IN_STRIP*j] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
              }else if(((j % 2) != 0) && (i<24)){
                leds[LEDS_IN_STRIP*(j+1)-i-1] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
              }
            }else if((i<=3) || (i>=10 && i<=13)){
              if((j % 2) == 0){
                leds[i+LEDS_IN_STRIP*j] = CRGB::White;
              }else if((j % 2) != 0){
                leds[LEDS_IN_STRIP*(j+1)-i-1] = CRGB::White;
              }
            }
          }
          colorIndex = colorIndex + 3; 
        }
        break;

      case 10:
        for( int i = LEDS_IN_STRIP-1; i >-1; i--) {
          for( int j = 0; j < LED_STRIPS; j++) {
            if((i<=2) || (i>=11 && i<=13)){
              if((j % 2) == 0){
                leds[i+LEDS_IN_STRIP*j] = CRGB::Grey;
              }else if((j % 2) != 0){
                leds[LEDS_IN_STRIP*(j+1)-i-1] = CRGB::Grey;
              }
            }
          }
          colorIndex = colorIndex + 3;
        } 
        m = 0;
        while(m<6){
          for( int j = 0; j < LED_STRIPS; j++) {
            for(int i = LEDS_IN_STRIP-1; i >-1; i--) {
              if((i>2 && i<11) || (i>13)){
                if((j % 2) == 0){
                  leds[i+LEDS_IN_STRIP*j] = ColorFromPalette( myBuddhistFlagPalette_p, colorIndex, brightness, currentBlending);
                }else if((j % 2) != 0){
                  leds[LEDS_IN_STRIP*(j+1)-i-1] = ColorFromPalette( myBuddhistFlagPalette_p, colorIndex, brightness, currentBlending);
                }
              }
            }
            FastLED.delay(10);
            colorIndex = colorIndex + 3; 
          }
          m++;
        }
        break;
        
      case 5:
        color = RandomColor();
        ClearAll();
        FlowerFill(color);
        FastLED.delay(2000);
        break; 

      case 11:
        ClearAll();
        DharmaChakra(0, colorIndex);
        FastLED.delay(2000);
        DharmaChakra(brightness, colorIndex);
        break;

      case 2:
        INOUT(Snow_p, colorIndex, brightness, LINEARBLEND);
        break;
      
      case 0:
        sinelon(brightness, colorIndex);
        break;

      case 3:
        color = RandomColor();
        ClearAll();
        FlowerFill(color);
        FastLED.delay(1000);
        color = RandomColor();
        FlowerFillRemain(color);
        FastLED.delay(1000);
        break; 

      case 4:   
        cylon(brightness, colorIndex);
        FastLED.delay(1000);
        break;
    }
}

void ClearAll(){
    for( int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Black;
    }
}

void ChangePalettePeriodically()
{
    uint8_t secondHand = (millis() / 500) % 100;
    static uint8_t lastSecond = 99;
    
    if( lastSecond != secondHand) {
        lastSecond = secondHand;
        if( secondHand ==  0)  { targetPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
        if( secondHand == 10)  { targetPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND;  }
        if( secondHand == 20)  { targetPalette = myBuddhistFlagPalette_p;   currentBlending = LINEARBLEND; }
        if( secondHand == 30)  { targetPalette = myBuddhistFlagPalette_p;   currentBlending = NOBLEND; }
        if( secondHand == 40)  { SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; }
        if( secondHand == 50)  { SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; }
        if( secondHand == 60)  { SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; }
        if( secondHand == 70)  { targetPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 80)  { targetPalette = myBuddhistFlagPalette_p;   currentBlending = LINEARBLEND; }
        if( secondHand == 90)  { SetupPurpleAndGreenPalette();             currentBlending = NOBLEND;  }
    }
}

void INOUT(CRGBPalette16 Palette, uint8_t colorIndex, uint8_t brightness, TBlendType Blending){
  for( int i = LEDS_IN_STRIP-1; i >-1; i--) {
    for( int j = 0; j < LED_STRIPS; j++) {
      if((j % 2) == 0){
        leds[i+LEDS_IN_STRIP*j] = ColorFromPalette( Palette, colorIndex, brightness, Blending);
      }else if((j % 2) != 0){
        leds[LEDS_IN_STRIP*(j+1)-i-1] = ColorFromPalette( Palette, colorIndex, brightness, Blending);
      }
    }
    colorIndex = colorIndex + 3;
  }
}

void DharmaChakra(uint8_t brightness,uint8_t colorIndex)
{
    for(int j = 0; j < LED_STRIPS; j++) {
      for(int i = 0; i <LEDS_IN_STRIP; i++) {
        if(((j % 6) <=1) || (i==0) || (i>=7 && i<=10) || (i>=21 && i<=24)){
          if((j % 2) == 0){
            if (brightness>0){
              leds[i+LEDS_IN_STRIP*j] = ColorFromPalette( myBuddhistFlagPalette_p, colorIndex, brightness, currentBlending);
            }else{
              leds[i+LEDS_IN_STRIP*j] = CRGB::Gold;
            }
          }else if((j % 2) != 0){
            if (brightness>0){
              leds[LEDS_IN_STRIP*(j+1)-i-1] = ColorFromPalette( myBuddhistFlagPalette_p, colorIndex, brightness, currentBlending);
            }else{
              leds[LEDS_IN_STRIP*(j+1)-i-1] = CRGB::Gold;
            }
          }
        }
      }
      if (brightness>0) FastLED.delay(100);
    } 
}

void FlowerFillRemain(CRGB color){
    for(int i = 0; i <LEDS_IN_STRIP; i++) {
      for(int j = 0; j < LED_STRIPS; j++) {
        switch(j % 6){

          case 0:
          case 4:
            if ((i>=20 && i<=27)){
                leds[i+LEDS_IN_STRIP*j] = color;
            }
            break;
          case 1:
          case 3:
            if((i>=24 && i<=25)|| (i==3)){
                leds[LEDS_IN_STRIP*(j+1)-i-1] = color;
            }
            break;
            
          case 2:
            break;

          case 5:
            if((i>=17 && i<=27)){
                leds[LEDS_IN_STRIP*(j+1)-i-1] = color;
            }
            break;
          break;
        } 
      }
      FastLED.delay(100);
    } 
}

void FlowerFill(CRGB color)
{   
//  CRGB color = RandomColor();
    for(int i = 0; i <LEDS_IN_STRIP; i++) {
      for(int j = 0; j < LED_STRIPS; j++) {
        switch(j % 6){

          case 0:
          case 4:
            if((i<=1)|| (i==5) || (i==7) || (i==8)){
                leds[i+LEDS_IN_STRIP*j] =  CRGB::Grey;
            }else if ((i>=8 && i<=19)|| (i==3)){
                leds[i+LEDS_IN_STRIP*j] = color;
            }
            break;
          case 1:
          case 3:
            if((i<=1)|| (i==5) || (i>=7 && i<=10)){
                leds[LEDS_IN_STRIP*(j+1)-i-1] = CRGB::Grey;
            }else if((i>=11 && i<=23)|| (i==3)){
                leds[LEDS_IN_STRIP*(j+1)-i-1] = color;
            }
            break;
            
          case 2:
            if((i<=1)|| (i==5) || (i>=7 && i<=11)){
                leds[i+LEDS_IN_STRIP*j] = CRGB::Grey;
            }else if( (i==3) || (i>=12 && i<=27) ){
                leds[i+LEDS_IN_STRIP*j] = color;
            }
            break;

          case 5:
            if((i<=1)|| (i==5)){
              leds[LEDS_IN_STRIP*(j+1)-i-1] = CRGB::Grey;
            }else if((i==3)){
              leds[LEDS_IN_STRIP*(j+1)-i-1] = color;
            }
          break;
        } 
      }
      FastLED.delay(100);
    } 
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; i++) {
        targetPalette[i] = CHSV( random8(), 255, random8());
    }
}

CRGB RandomColor(){
    uint8_t color =  random(1,5);

    switch (color){
      case 1: 
        return CRGB::Blue;
        break;
      case 2: 
        return CRGB::Yellow;
        break;
      case 3: 
        return CRGB::Red;
        break;
      case 4: 
        return CRGB::Gray;
        break;
      case 5: 
        return CRGB::Orange;
        break;
    }
}

void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    targetPalette[0] = CRGB::White;
    targetPalette[4] = CRGB::White;
    targetPalette[8] = CRGB::White;
    targetPalette[12] = CRGB::White;
    
}

void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB blue  = CHSV( HUE_BLUE, 255, 255);
    CRGB black  = CRGB::Black;
    
    targetPalette = CRGBPalette16(
                                   blue,  blue,  black,  black,
                                   purple, purple, black,  black,
                                   blue,  blue,  black,  black,
                                   purple, purple, black,  black );
}

// takes up 64 bytes of flash.
const TProgmemPalette16 myBuddhistFlagPalette_p PROGMEM =
{
    CRGB::Blue,
    CRGB::Yellow,
    CRGB::Red,
    CRGB::Gray,
    
    CRGB::Orange,
    CRGB::Blue,
    CRGB::Yellow,
    CRGB::Red,
    
    CRGB::Gray,
    CRGB::Orange,
    CRGB::Blue,
    //CRGB::Blue, 
    CRGB::Yellow,
    //CRGB::Yellow,
    CRGB::Red,
    //CRGB::Red,
    CRGB::Gray,
    //CRGB::Gray,
    CRGB::Orange,
    CRGB::Red
};

const TProgmemRGBPalette16 RedOrange_p PROGMEM =
{  CRGB::Red,  CRGB::Red,  CRGB::Red,  CRGB::Red, 
   CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Orange,
   CRGB::Red,  CRGB::Red,  CRGB::Red,  CRGB::Red, 
   CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Orange };
 
   // purple blue
const TProgmemRGBPalette16 Purples_p PROGMEM =
{  CRGB::Purple, CRGB::Magenta, CRGB::Blue, CRGB::DarkBlue, 
   CRGB::Purple, CRGB::Magenta, CRGB::Blue, CRGB::DarkBlue,
   CRGB::Purple, CRGB::Magenta, CRGB::Blue, CRGB::DarkBlue, 
   CRGB::Purple, CRGB::Magenta, CRGB::Blue, CRGB::DarkBlue };

// A palette of soft snowflakes with the occasional bright one
const TProgmemRGBPalette16 Snow_p PROGMEM =
{  0x304048, 0x304048, 0x304048, 0xE0F0FF,
   0x304048, 0x304048, 0x304048, 0x304048,
   0x304048, 0x304048, 0x304048, 0x304048,
   0x304048, 0x304048, 0x304048, 0xE0F0FF };

void sinelon(uint8_t brightness, uint8_t colorIndex)
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
//  leds[pos] += CHSV( HUE_BLUE, 255, 192);
  
  for( int j = 0; j < LED_STRIPS; j++) {
    int i = beatsin16(13,0,LEDS_IN_STRIP);
    if((j % 2) == 0){
      leds[i+LEDS_IN_STRIP*j] = ColorFromPalette( myBuddhistFlagPalette_p, colorIndex, brightness, LINEARBLEND);
    }else if((j % 2) != 0){
      leds[LEDS_IN_STRIP*(j+1)-i-1] = ColorFromPalette( myBuddhistFlagPalette_p, colorIndex, brightness, LINEARBLEND);
    }
  }
}

void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } }

void cylon(uint8_t brightness, uint8_t colorIndex)
{
  static uint8_t hue = 0;
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  
  for( int j = 0; j < LED_STRIPS; j++) {
    int i = beatsin16(30,0,LEDS_IN_STRIP);
    if((j % 2) == 0){
      leds[i+LEDS_IN_STRIP*j] = CHSV(hue++, 255, 255);
      if (j + 4 < LED_STRIPS){ leds[i+LEDS_IN_STRIP*(j + 4)] = CHSV(hue++, 255, 255);}
      if (j + 8 < LED_STRIPS){ leds[i+LEDS_IN_STRIP*(j + 8)] = CHSV(hue++, 255, 255);}
      if (j + 16 < LED_STRIPS){ leds[i+LEDS_IN_STRIP*(j + 16)] = CHSV(hue++, 255, 255);}
      if (j + 20 < LED_STRIPS){ leds[i+LEDS_IN_STRIP*(j + 20)] = CHSV(hue++, 255, 255);}
    }else if((j % 2) != 0){
      leds[LEDS_IN_STRIP*(j+1)-i-1] = CHSV(hue++, 255, 255);
      if (j + 4 < LED_STRIPS){ leds[LEDS_IN_STRIP*(j + 4 + 1)-i-1] = CHSV(hue++, 255, 255);}
      if (j + 8 < LED_STRIPS){ leds[LEDS_IN_STRIP*(j + 8 + 1)-i-1] = CHSV(hue++, 255, 255);}
      if (j + 16 < LED_STRIPS){ leds[LEDS_IN_STRIP*(j + 16 + 1)-i-1] = CHSV(hue++, 255, 255);}
      if (j + 20 < LED_STRIPS){ leds[LEDS_IN_STRIP*(j + 20 + 1)-i-1] = CHSV(hue++, 255, 255);}
    }
    FastLED.show();
    fadeall();
    delay(10);
  }
}
