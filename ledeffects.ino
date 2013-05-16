#include <arduino.h>
#include "ledeffects.h"

// ----------------------------------------------------------------------
TCounter LedEffect::Hue    = TCounter();
TCounter LedEffect::Sat    = TCounter();
TCounter LedEffect::Val    = TCounter();
TCounter LedEffect::Vel    = TCounter();
TCounter LedEffect::Mover  = TCounter();
TTimer   LedEffect::TimerA = TTimer();
TTimer   LedEffect::TimerB = TTimer();
// ----------------------------------------------------------------------
void LedEffect::tuneParamUp(byte param) { 

  if (param == 0) param = getParamDefault();
  
  switch(param) {
    case _VEL_: 
      Vel.increment();
      TimerA.setTime(Vel.getValue()); 
      break;
      
    case _HUE_: 
      Hue.increment();
      break;
  }
}

// ----------------------------------------------------------------------
void LedEffect::tuneParamDown(byte param) { 

  if (param == 0) param = getParamDefault();
  
  switch(param) {
    case _VEL_: 
      Vel.decrement();
      TimerA.setTime(Vel.getValue()); 
      break;
      
    case _HUE_: 
      Hue.decrement();
      break;
  }
}

// ----------------------------------------------------------------------
void LedEffect::tuneValueUp() { 
  Val.increment();
}

// ----------------------------------------------------------------------
void LedEffect::tuneValueDown() { 
  Val.decrement();
}

// ----------------------------------------------------------------------
void LedEffect::init(int parA, int parB) {
  LedStrip.blank();
  TimerA.disable();
  TimerB.disable();
}

// ----------------------------------------------------------------------    
void LedEffect::update(int event) {
}

// ----------------------------------------------------------------------    
int LedEffect::run() {
  return TimerA.run() | TimerB.run();
}

// ----------------------------------------------------------------------
void LedEffect::save(byte *data) {
  byte paramMap = getParamMap();
  
  *(int *)(data)   = (paramMap & _HUE_) ? Hue.getValue() : 0;
  *(int *)(data+2) = (paramMap & _SAT_) ? Sat.getValue() : 0;  
  *(int *)(data+4) = (paramMap & _VAL_) ? Val.getValue() : 0;
  *(int *)(data+8) = (paramMap & _VEL_) ? Vel.getValue() : 0;
}

// ----------------------------------------------------------------------
void LedEffect::load(byte *data) {
  byte paramMap = getParamMap();
  
  if (paramMap & _HUE_) Hue.setValue(*(int *)(data));
  if (paramMap & _SAT_) Sat.setValue(*(int *)(data+2));
  if (paramMap & _VAL_) Val.setValue(*(int *)(data+4));
  if (paramMap & _VEL_) Vel.setValue(*(int *)(data+8));  
}

// ----------------------------------------------------------------------
void Fire::init(int parA, int parB) {
  
  LedEffect::init(parA, parB);  

  Hue.init(  0,   4,   0, 359, TCounter::CYCLER);
  Sat.init(255,   0,   0, 255, TCounter::FIXED);
  Val.init(200,   2, 100, 250, TCounter::UPDOWN);

  TimerA.init(80, &TimerAdapterA); 
  TimerB.init(20, &TimerAdapterB);   
}

// ----------------------------------------------------------------------
void Fire::update(int event) {
  
  switch(event) {
    case 0:
      for (int i=0; i < LedStrip.getCount(); i++) {
        LedStrip.setHSV(i, Hue.getValue()+random(10), Sat.getValue(), Val.getValue()-random(10,20));  
      }
      break;

    case 1:
      LedStrip.filter(0, LedStrip.getCount(), 8, LedStrip.get(random(0,LedStrip.getCount())));    
      break;
  }
}
// ----------------------------------------------------------------------
void Fader::init(int parA, int parB) {
  
  LedEffect::init(parA, parB);  

  Hue.init(  0,   4,   0, 359, TCounter::CYCLER);
  Sat.init(255,   0,   0, 255, TCounter::FIXED);
  Val.init(128,   2,   0, 255, TCounter::CYCLER);
  
  TimerA.init(70, &TimerAdapterA); 
  
  _faderPar   = parA;  
  _faderStep  = 0;
}

// ----------------------------------------------------------------------
void Fader::update(int event) {

  const byte fullCycle = 80;

  int  sinX;
  int  dv = 0;
  byte ix;
  
  if ( _faderStep >= fullCycle ) _faderStep = 0;
  
  if ( _faderStep < 20 ) {
    ix = _faderStep;
    sinX = pgm_read_byte(&sinT[ix]);
    dv   = sinX / _faderPar;
  }
  else if ( _faderStep < 40 ) {
    ix = 39 - _faderStep;
    sinX = pgm_read_byte(&sinT[ix]);    
    dv   = sinX / _faderPar;    
  }
  else if ( _faderStep < 60 ) {
    ix = _faderStep - 40;
    sinX = pgm_read_byte(&sinT[ix]);
    dv   = -sinX / _faderPar;    
  }
  else if ( _faderStep < 80 ) {
    ix = 79 - _faderStep;
    sinX = pgm_read_byte(&sinT[ix]);
    dv  = -sinX / _faderPar;    
  }  

  _faderStep++;

  LedStrip.setHSV(0, Hue.getValue(), Sat.getValue(), Val.getValue() + dv);
  LedStrip.fill(1, LedStrip.getCount()-1, LedStrip.get(0));
}

// ----------------------------------------------------------------------
void StaticFields::init(int parA, int parB) {

  LedEffect::init(parA, parB);  
    
  _fieldCount = parA;
  _colorMode  = parB;
  
  Hue.init(  0,   2,   0, 359, TCounter::CYCLER);
  Sat.init(255,   0,   0, 255, TCounter::FIXED);
  Val.init(200,   2, 100, 250, TCounter::UPDOWN);
  TimerA.init(100, &TimerAdapterA); 
}

// ----------------------------------------------------------------------
int StaticFields::colorStep(int scheme, int index) {

  int deltaHue;  
  const int analog[] = {0, 30, 30, -1};
  const int analogComplementary[] = {0, 150, 210,-1};  

  switch(scheme) {
    case 0:
      index = constrain(index, 0, 3);
      deltaHue =  analog[index];
      break;

    case 1:
      index = constrain(index, 0, 3);
      deltaHue = analogComplementary[index];
      break;
      
    default:
      deltaHue = 0;
      break;
  }

  return deltaHue;
}

// ----------------------------------------------------------------------
void StaticFields::update(int event) {
  
  int fieldLen  = LedStrip.getCount() / _fieldCount;
  int remainder = LedStrip.getCount() % _fieldCount;
  
  int  hue;
  int  pos = 0;
  int  len;
  byte count = 0;
  
  for (int i=0; i < _fieldCount; i++) {

    // Handle colors 
    hue = colorStep(_colorMode, count);
    if (hue < 0) {
          count = 0;
          hue = Hue.getValue() + colorStep(_colorMode, count);
    }
    else {
          count++;
          hue =Hue.getValue() + hue;
    }
    
    // Determine the segment lenght including portion of the remainder
    if (remainder != 0) {
      len = fieldLen + 1;
      remainder--;
    }
    else {
      len = fieldLen;
    }
    
    // Draw a color into the first position
    LedStrip.setHSV(pos, hue, Sat.getValue(), Val.getValue());

    // Fill up the segment with the color of the first position
    LedStrip.fill(pos, len, LedStrip.get(pos));

    // Move pos to next segment
    pos += len;
  }
}

// ----------------------------------------------------------------------
void DynamicFields::init(int parA, int parB) {

  StaticFields::init(parA, parB);
  
  // Change class behaviour from static to dynamic
  Vel.init(100, 10, 10, 300, TCounter::CYCLER);  

  TimerA.init(Vel.getValue(), &TimerAdapterA);  
}

// ----------------------------------------------------------------------
void DynamicFields::update(int event) {  

  // Add dynamic behaviour by incrementing the color
  Hue.increment();

  // Use parent class behaviour
  StaticFields::update(event);
}

// ----------------------------------------------------------------------
void RainbowA::init(int parA, int parB) {
  LedEffect::init(parA, parB);
  
  _angle = parA;
  Hue.init(  0,  10,   0, 358, TCounter::CYCLER);
  Sat.init(255,   0,   0, 255, TCounter::FIXED);
  Val.init(255,   0,   0, 255, TCounter::FIXED);   

  TimerA.init(100, &TimerAdapterA);
}

// ----------------------------------------------------------------------
void RainbowA::update(int event) {
  int pixelAngle = 0;
  
  for (int i = 0; i < LedStrip.getCount(); i++) {
    LedStrip.setHSV(i, Hue.getValue() + pixelAngle / LedStrip.getCount(), Sat.getValue(), Val.getValue());
    pixelAngle += _angle;
  }
}

// ----------------------------------------------------------------------
void RainbowB::init(int parA, int parB) {
  LedEffect::init(parA, parB);

  Hue.init(  0,parA,   0, 358, TCounter::CYCLER);
  Sat.init(255,   0,   0, 255, TCounter::FIXED);
  Val.init(255,   0,   0, 255, TCounter::FIXED);   
  Vel.init(100,   5,  10, 200, TCounter::CYCLER);

  TimerA.init(100, &TimerAdapterA);
}

// ----------------------------------------------------------------------
void RainbowB::update(int event) {
  Hue.increment();    
  LedStrip.shiftRight(0, LedStrip.getCount());
  LedStrip.setHSV(0, Hue.getValue(), Sat.getValue(), Val.getValue());
}

// ----------------------------------------------------------------------
void Slider::init(int parA, int parB) {

  LedEffect::init(parA, parB);  
    
  Hue.init( 20,   7,   0, 359, TCounter::CYCLER);
  Sat.init(255,   0,   0, 255, TCounter::FIXED);
  Val.init(255,   0,   0, 255, TCounter::FIXED);   
  Vel.init( 80,   5,  10, 200, TCounter::CYCLER);
  Mover.init(0, 1, 0, LedStrip.getCount()-1, TCounter::UPDOWN);

  TimerA.init(Vel.getValue(), &TimerAdapterA);
  TimerB.init(20, &TimerAdapterB);
}
// ----------------------------------------------------------------------
void Slider::update(int event) {

  switch(event) {
    case 0: // Motion
      Hue.increment();
      Mover.increment();
      LedStrip.setHSV(Mover.getValue(), Hue.getValue(), Sat.getValue(), Val.getValue());
      break;
      
    case 1:  // Filter
      PIXEL black = {0,0,0}; 
      LedStrip.filter(0, LedStrip.getCount(), 6, (byte *)(&black));  
      LedStrip.setHSV(Mover.getValue(), Hue.getValue(), Sat.getValue(), Val.getValue());
      break;
  }
}

