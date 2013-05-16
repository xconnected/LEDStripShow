//#define DEBUG 1

#include <Arduino.h>
#include <EEPROM.h>
#include <FastSPI_LED2.h>
#include <rotary.h>
#include <TTimer.h>
#include <TCounter.h>
#include <TSwitch.h>
#include <TLedStrip.h>
#include "ledeffects.h"
#include "LEDStripShow.h"

// ---------------------------------------------------------------------------
// Hardware SPI - .652ms for an 86 led frame @8Mhz (3.1Mbps?), .913ms @4Mhz 1.434ms @2Mhz
// With the wait ordering reversed,  .520ms at 8Mhz, .779ms @4Mhz, 1.3ms @2Mhz
// WS2801Controller<11, 13, 10, 0> LED;

// Same Port, non-hardware SPI - 1.2ms for an 86 led frame, 1.12ms with large switch 
// WS2801Controller<12, 13, 10> LED;

// Different Port, non-hardware SPI - 1.47ms for an 86 led frame

// CONFIGURATION ITEM
WS2801Controller<7, 13, 10, 3> LED;

// ---------------------------------------------------------------------------
#ifdef DEBUG

int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

#endif

// ---------------------------------------------------------------------------
// CONFIGURATION ITEMS
// UI Objects
Rotary           UIRotary = Rotary(2, 3);
TSwitch          UIButton = TSwitch(4);
TSwitch          UISwitch = TSwitch(5);
TLedStrip        LedStrip(_LED_COUNT_);

// Controller Object
TLedStripShow  LightStripShow;

// ---------------------------------------------------------------------------
// Effect Objects

RainbowA      rainbowA (LedStrip);
RainbowB      rainbowB (LedStrip);
Slider        slider   (LedStrip);
Fader         fader    (LedStrip);
StaticFields  s_fields (LedStrip);
DynamicFields d_fields (LedStrip);
Fire          fire     (LedStrip);

// Effect Objects and Parameter Array
Effect_t Effect[_EFFECT_COUNT_] = {
  { &fader,     4, 0 },
  { &rainbowA, 90, 0 },
  { &rainbowB,  6, 0 },
  { &rainbowB,  2, 0 },
  { &s_fields,  3, 0 },
  { &s_fields,  3, 1 },  
  { &d_fields,  LedStrip.getCount(), 0 },
  { &d_fields,  LedStrip.getCount(), 1 },
  { &slider,    0, 0 },
  { &fire,      0, 0 }
};
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TimerAdapterA() {
  Effect[LightStripShow.getEffect()].pAppl->update(0);
}
// ---------------------------------------------------------------------------
void TimerAdapterB() {
  Effect[LightStripShow.getEffect()].pAppl->update(1);
}
// ---------------------------------------------------------------------------
void TimerAdapterCycler() {
  LightStripShow.nextEffect();
}
// ---------------------------------------------------------------------------
void TimerAdapterSaving() {
  LightStripShow.saveProgram();  
  LightStripShow.saveSettings(LightStripShow.getEffect());
  LightStripShow.savingDone();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void TLedStripShow::delayedSaving() {
  TimerSaving.init(_WAIT_SAVE_, &TimerAdapterSaving);  
};
// ---------------------------------------------------------------------------
void TLedStripShow::savingDone() {
  TimerSaving.disable();  
};
// ---------------------------------------------------------------------------
void TLedStripShow::enableCycling() {
  flash();
  _mode |= _MODE_CYCLER_; 
  TimerCycler.init(_CYCLE_TIME_, &TimerAdapterCycler);
};
// ---------------------------------------------------------------------------
void TLedStripShow::disableCycling() {
  _mode &= (~_MODE_CYCLER_);
  TimerCycler.disable();
};
// ---------------------------------------------------------------------------
byte TLedStripShow::isCyclingEnabled() {
  return _mode & _MODE_CYCLER_;
};
// ---------------------------------------------------------------------------
void TLedStripShow::setMode(byte mode) {
  if ( mode & _MODE_CYCLER_ ) enableCycling();
};
// ---------------------------------------------------------------------------
byte TLedStripShow::getMode() { 
  return _mode;
};
// ---------------------------------------------------------------------------
byte TLedStripShow::setEffect(byte ix) {

  _effectIx = ix;

  if ( _effectIx >= _EFFECT_COUNT_) {
    _effectIx = 0;
  }
  else if ( _effectIx <  0) {
    _effectIx = _EFFECT_COUNT_ - 1;  
  }

  if ( ix == _EFFECT_COUNT_ ) enableCycling();
  
  Effect[_effectIx].pAppl->init(Effect[_effectIx].ParA, Effect[_effectIx].ParB);
  loadSettings(_effectIx);

  return _effectIx;
};
// ---------------------------------------------------------------------------
byte TLedStripShow::nextEffect() {
  return setEffect(_effectIx+1);
};
// ---------------------------------------------------------------------------
byte TLedStripShow::getEffect() {
  return _effectIx;
};
// ---------------------------------------------------------------------------
void TLedStripShow::saveSettings(byte ix) {

#ifdef DEBUG  
  Serial.print("save #");
  Serial.print( ix );
  byte count = 0;  
#endif
  
  // Store record marker in buffer
  _buffer[0] = _EEPROM_RECORD_MARKER_;
  
  // Let effect object to write its config int buffer
  Effect[ix].pAppl->save(&_buffer[1]);
  
  // Determine memeory offset
  int offset = _EEPROM_OFFSET_ + ix * _EEPROM_RECORD_LENGTH_;
 
  // Write what is different
  for (int i=0; i < _EEPROM_RECORD_LENGTH_; i++) {  
    if ( EEPROM.read(offset + i) != _buffer[i] ) {
      EEPROM.write(offset + i, _buffer[i]);
#ifdef DEBUG        
      count++;
#endif      
    }
  }

#ifdef DEBUG  
  Serial.print(" -> ");
  Serial.println( count );
#endif
  
};
// ---------------------------------------------------------------------------
void TLedStripShow::loadSettings(byte ix) {

#ifdef DEBUG  
  Serial.print("load #");
  Serial.println( ix );
#endif
  
  // Determine memory offset
  int offset = _EEPROM_OFFSET_ + ix * _EEPROM_RECORD_LENGTH_;
  
  // Load effect config into buffer
  for (int i=0; i < _EEPROM_RECORD_LENGTH_; i++) {
    _buffer[i] = EEPROM.read(offset + i);
  }

  // Test record marker
  if ( _EEPROM_RECORD_MARKER_ == _buffer[0] ) {
    Effect[ix].pAppl->load(&_buffer[1]);
  }
  else {
    saveSettings(ix);
  }
}
// ---------------------------------------------------------------------------
void TLedStripShow::loadProgram() {  

  if ( EEPROM.read(0) == _EEPROM_MARKER_ ) {

    // Load effect ix
    setEffect(EEPROM.read(1));
    
    // Load mode
    setMode(EEPROM.read(2));
  }
  else {
    setEffect(_EFFECT_COUNT_);    
    saveProgram();
  }
}
// ---------------------------------------------------------------------------
void TLedStripShow::saveProgram() {  

  // Write EEPROM marker if different
  if ( EEPROM.read(0) != _EEPROM_MARKER_) EEPROM.write(0, _EEPROM_MARKER_);

  // Write program number if different
  if ( EEPROM.read(1) != getEffect() )    EEPROM.write(1, getEffect());

  // Write program number if different
  if ( EEPROM.read(2) != getMode() )      EEPROM.write(2, getMode());
}
// ---------------------------------------------------------------------------
void TLedStripShow::flash() {

  LedStrip.blank();
  LED.showRGB(LedStrip.get(0), LedStrip.getCount());
  delay(100);
  LedStrip.white();
  LED.showRGB(LedStrip.get(0), LedStrip.getCount());
  delay(200);  
  LedStrip.blank();
  LED.showRGB(LedStrip.get(0), LedStrip.getCount());
  delay(100);  
}  

// ---------------------------------------------------------------------------
void TLedStripShow::run() {
  TimerCycler.run();
  TimerSaving.run();

  if ( Effect[getEffect()].pAppl->run() ) {
    LED.showRGB(LedStrip.get(0), LedStrip.getCount());
  }  
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void setup() {
  
#ifdef DEBUG
  delay(1000);
  Serial.begin(38400);
  Serial.print("Setup (");
  Serial.print(freeRam());
  Serial.println(")");  
#endif
 
  // Setup SPI Interface
  LED.init();

  // Clean Ledstrip
  LedStrip.init();

  // Set effect
  LightStripShow.loadProgram();
  
  // Interrupt handler
  PCICR  |= (1 << PCIE2);
  PCMSK2 |= (1 << PCINT18) | (1 << PCINT19);
  sei();
  
#ifdef DEBUG
  Serial.println("start.");
#endif
}

// ---------------------------------------------------------------------------

void loop() { 

  // Process Application Timer
  LightStripShow.run();
  // Test and debounce button
  UIButton.run();
  // Test and debounce switch
  UISwitch.run();  

#ifdef DEBUG
  if ( UISwitch.get() && UISwitch.hasChanged() ) {
    Serial.println("switch");
  }
#endif
  
  // Watch Program selection UIButton
  if ( UIButton.get() && UIButton.hasChanged() ) {
#ifdef DEBUG
    Serial.println("push");
#endif
    if ( LightStripShow.isCyclingEnabled() ) {
      LightStripShow.disableCycling();
      LightStripShow.setEffect(0);
    } 
    else {
      LightStripShow.nextEffect();
    }
    LightStripShow.delayedSaving();
  }
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// Interrupt Service Routine to handle the rotary switch

ISR(PCINT2_vect) {
  unsigned char result = UIRotary.process();
  if (result) {
    if (result == DIR_CW) {
#ifdef DEBUG
      Serial.println("Right-Up");
#endif
      if ( UISwitch.get() ) {
        Effect[LightStripShow.getEffect()].pAppl->tuneValueUp();
      }
      else {
        Effect[LightStripShow.getEffect()].pAppl->tuneParamUp();
      }
      LightStripShow.delayedSaving();
    }
    else if (result == DIR_CCW) {
#ifdef DEBUG
      Serial.println("Left-Dwn");
#endif
      if ( UISwitch.get() ) {
        Effect[LightStripShow.getEffect()].pAppl->tuneValueDown();
      }
      else {
        Effect[LightStripShow.getEffect()].pAppl->tuneParamDown();
      }
      LightStripShow.delayedSaving();
    }
  }
}

