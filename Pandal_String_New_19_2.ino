#include <FastLED.h>

#define LED_PIN_1     5
#define LED_PIN_2     6
#define LED_PIN_3     7
#define LED_PIN_4     8
#define LED_PIN_5     9
#define LED_PIN_6     10
#define LED_PIN_7     22
#define LED_PIN_8     24
#define LED_PIN_9     26
#define LED_PIN_10    28
#define LED_PIN_11    30
#define LED_PIN_12    32

//#define NUM_LEDS    171
#define NUM_LEDS    162
#define BRIGHTNESS  255
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define LED_STRIPS 3
#define LEDS_IN_STRIP (NUM_LEDS/LED_STRIPS)

CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

CRGBPalette16 targetPalette;

extern CRGBPalette16 myBuddhistFlagPalette;
extern const TProgmemPalette16 myBuddhistFlagPalette_p PROGMEM;
extern const TProgmemRGBPalette16 Snow_p PROGMEM;

// ---------------------- SETUP ----------------------
void setup() {
    delay( 3000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN_1, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.addLeds<LED_TYPE, LED_PIN_2, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.addLeds<LED_TYPE, LED_PIN_3, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.addLeds<LED_TYPE, LED_PIN_4, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.addLeds<LED_TYPE, LED_PIN_5, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.addLeds<LED_TYPE, LED_PIN_6, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.addLeds<LED_TYPE, LED_PIN_7, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip ); 
    FastLED.addLeds<LED_TYPE, LED_PIN_8, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip ); 
    FastLED.addLeds<LED_TYPE, LED_PIN_9, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip ); 
    FastLED.addLeds<LED_TYPE, LED_PIN_10, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip ); 
    FastLED.addLeds<LED_TYPE, LED_PIN_11, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );  
    FastLED.addLeds<LED_TYPE, LED_PIN_12, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip ); 
        
    FastLED.setBrightness( BRIGHTNESS );
    
    currentPalette = RainbowColors_p;
    currentBlending = LINEARBLEND;
}

// ---------------------- LOOP ----------------------
static uint8_t colorIndex = 0;
static uint8_t loopIndex = 0;
static uint8_t lastCheck = 0;

void loop()
{
    ChangePalettePeriodically();
    colorIndex = colorIndex + 5;
    Patterns(colorIndex);

    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void Patterns( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    uint8_t secondsCheck = (millis() / 1000) % 2;

    if (lastCheck != secondsCheck){
      loopIndex = loopIndex + 1;
    }
    lastCheck = secondsCheck;

    if (loopIndex > 5){
      loopIndex = 0;
    }
    
    switch(loopIndex){
      case 0:
        Pattern1(currentPalette, colorIndex, brightness, currentBlending);
        break;
      case 1:
        Pattern1(currentPalette, colorIndex, brightness, currentBlending);
        break;
      case 2:
        Pattern2(Snow_p, colorIndex, brightness, LINEARBLEND);
        break;
      case 3:
         sinelon(colorIndex);
        break;
      case 4:
        Pattern1(HeatColors_p, colorIndex, brightness, LINEARBLEND);
        break;
      case 5:
        Pattern2(LavaColors_p, colorIndex, brightness, LINEARBLEND);
        break;
    }
}
       
void Pattern1(CRGBPalette16 Palette, uint8_t colorIndex, uint8_t brightness, TBlendType Blending)
{  
  for( int i = 0; i < LEDS_IN_STRIP; i++) {
    for( int j = 0; j < LEDS_IN_STRIP/9; j++) {
        leds[i + j*9] = ColorFromPalette( Palette, colorIndex, brightness, Blending);
        leds[i + LEDS_IN_STRIP + j*9] = ColorFromPalette( Palette, colorIndex, brightness, Blending);
        leds[i + 2*LEDS_IN_STRIP + j*9] = ColorFromPalette( Palette, colorIndex, brightness, Blending);
    }
    FastLED.show();
    delay(100);
  }
  
  for( int i = 0; i < LEDS_IN_STRIP; i++) {
    for( int j = 0; j < LEDS_IN_STRIP/9; j++) {
        leds[i + j*9] = CRGB::Black;
        leds[i + LEDS_IN_STRIP + j*9] = CRGB::Black;
        leds[i + 2*LEDS_IN_STRIP + j*9] = CRGB::Black;
    }
    FastLED.show();
    delay(100);
  }
}

void Pattern2(CRGBPalette16 Palette, uint8_t colorIndex, uint8_t brightness, TBlendType Blending)
{
    for (int i = LEDS_IN_STRIP - 1; i >-1; i--) {
      for( int j = 0; j < LED_STRIPS; j++) {
        if((j % 2) == 0){
          leds[i+LEDS_IN_STRIP*j] = ColorFromPalette( Palette, colorIndex, brightness, Blending);
        }else if((j % 2) != 0){
          leds[LEDS_IN_STRIP*(j+1)-i-1] = ColorFromPalette( Palette, colorIndex, brightness, Blending);
        }
      }
      colorIndex += 3;
    }
}

void ChangePalettePeriodically()
{
    uint8_t secondHand = (millis() / 500) % 100;
    static uint8_t lastSecond = 99;
    
    if( lastSecond != secondHand) {
        lastSecond = secondHand;
        if( secondHand ==  0)  { targetPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
        if( secondHand == 10)  { targetPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  }
        if( secondHand == 20)  { targetPalette = myBuddhistFlagPalette_p;   currentBlending = LINEARBLEND; }
        if( secondHand == 30)  { targetPalette = myBuddhistFlagPalette_p;   currentBlending = NOBLEND; }
        if( secondHand == 40)  { targetPalette = RainbowStripeColors_p;     currentBlending = LINEARBLEND; }
        if( secondHand == 50)  { SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; }
        if( secondHand == 60)  { targetPalette = myBuddhistFlagPalette_p;       currentBlending = LINEARBLEND; }
        if( secondHand == 70)  { targetPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 80)  { targetPalette = myBuddhistFlagPalette_p;   currentBlending = LINEARBLEND; }
        if( secondHand == 90)  { SetupBlackAndWhiteStripedPalette();           currentBlending = NOBLEND;  }
    }
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; i++) {
        targetPalette[i] = CHSV( random8(), 255, random8());
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

void sinelon(uint8_t colorIndex)
{
  float hue = 0;
  if (colorIndex % 4 == 0) { hue = HUE_GREEN; }
  if (colorIndex % 4 == 1) { hue = HUE_ORANGE; }
  if (colorIndex % 4 == 2) { hue = HUE_YELLOW; }
  if (colorIndex % 4 == 3) { hue = HUE_RED; }

    // a colored dot sweeping back and forth, with fading trails
    fadeToBlackBy( leds, NUM_LEDS, 20);
    int pos = beatsin16(13,0,NUM_LEDS);
    leds[pos] += CHSV( hue, 255, 192);
}

// A palette of soft snowflakes with the occasional bright one
const TProgmemRGBPalette16 Snow_p PROGMEM =
{  0x304048, 0x304048, 0x304048, 0xE0F0FF,
   0x304048, 0x304048, 0x304048, 0x304048,
   0x304048, 0x304048, 0x304048, 0x304048,
   0x304048, 0x304048, 0x304048, 0xE0F0FF };
   
// takes up 64 bytes of flash.
const TProgmemPalette16 myBuddhistFlagPalette_p PROGMEM =
{
    CRGB::Green,
    CRGB::Yellow,
    CRGB::Red,
    CRGB::Gray,
    
    CRGB::Orange,
    CRGB::Green,
    CRGB::Yellow,
    CRGB::Red,
    
    CRGB::Gray,
    CRGB::Orange,
    CRGB::Green,
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
