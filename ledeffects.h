#ifndef _LEDEFFECTS_H_
#define _LEDEFFECTS_H_

#include <avr/pgmspace.h>
#include <Arduino.h>
#include <TCounter.h>
#include <TTimer.h>
#include "ledstrip.h"

PROGMEM const byte sinT[]  = { 0, 20, 40, 60, 79, 98, 116, 133, 150, 166, 180, 194, 206, 217, 227, 236, 243, 248, 252, 254, 255 };

#define _HUE_ 1
#define _SAT_ 2
#define _VAL_ 4
#define _VEL_ 8

// ---------------------------------------------------------------------------
// Adapters to handle effect timer events - used for function pointers
void TimerAdapterA();
void TimerAdapterB();
// ---------------------------------------------------------------------------
// Base class for all effects

class LedEffect {

  protected:
    static TCounter Hue;
    static TCounter Sat;
    static TCounter Val;
    static TCounter Vel;
    static TCounter Mover;
    static TTimer   TimerA;
    static TTimer   TimerB;	
    
    TLedStrip &LedStrip;

  public:
    LedEffect(TLedStrip& ledstrip):LedStrip(ledstrip) {};

    virtual void init(int parA, int parB);
    virtual void update(int event);
    virtual int  run();   
    virtual void save(byte *data); 
    virtual void load(byte *data);
    virtual void tuneParamUp  (byte param = 0);
    virtual void tuneParamDown(byte param = 0);
    virtual void tuneValueUp();
    virtual void tuneValueDown();    
    virtual byte getParamMap();
    virtual byte getParamDefault();
};
// ---------------------------------------------------------------------------
// Display the selected color and fade the brightness following an approximated
// sinus wave
// Effect Type: Dynamic
// ParA: fader depth
// ParB: fader unused

class Fader : public virtual LedEffect {
  private:
    int _faderCount;
    int _faderStep;
    int _faderPar;    
 
  public:
    Fader(TLedStrip& ledstrip):LedEffect(ledstrip) {};
    void init(int parA, int parB);
    void update(int event);
    byte getParamMap()     { return _HUE_| _VAL_; };
    byte getParamDefault() { return _HUE_; };
};

// ---------------------------------------------------------------------------
// Display the selected color and randomly flicker single pixels
// Effect Type: Dynamic
// Parameter: Speed/Velocity
// ParA: unused
// ParB: unused

class Fire : public virtual LedEffect {
  public:
    Fire(TLedStrip& ledstrip):LedEffect(ledstrip) {};
    void init(int parA, int parB);
    void update(int event);
    byte getParamMap()     { return _HUE_| _VAL_; };
    byte getParamDefault() { return _HUE_; };
};

// ---------------------------------------------------------------------------
// Display a static portion of the rainbow
// Effect Type: Static
// Parameter: Hue
// ParA: angle on the color wheel covered by the ledstrip
// ParB: unused

class RainbowA : public virtual LedEffect {
  private:
    int _angle;
    
  public:
    RainbowA(TLedStrip& ledstrip):LedEffect(ledstrip) {};
    void init(int parA, int parB);
    void update(int event);
    byte getParamMap()     { return _HUE_| _VAL_; };
    byte getParamDefault() { return _HUE_; };    
};
// ---------------------------------------------------------------------------
// Display a moving rainbow along the line
// Effect Type: Dynamic 
// Parameter: Speed/Velocity
// ParA: color step angle on the color wheel
// ParB: unused

class RainbowB : public virtual LedEffect {
  public:
    RainbowB(TLedStrip& ledstrip):LedEffect(ledstrip) {};
    void init(int parA, int parB);
    void update(int event);
    byte getParamMap()     { return _VAL_ | _VEL_; };
    byte getParamDefault() { return _VEL_; };
};
// ---------------------------------------------------------------------------
// Move a pixel with a changing color and a flare behind along the line
// Effect Type: Dynamic 
// Parameter: Speed/Velocity
// ParA: unused
// ParB: unused

class Slider : public virtual LedEffect {
  public:
    Slider(TLedStrip& ledstrip):LedEffect(ledstrip) {};
    void init(int parA, int parB);
    void update(int event);
    byte getParamMap()     { return _VAL_ | _VEL_; };
    byte getParamDefault() { return _VEL_; };        
};

// ---------------------------------------------------------------------------
// Display a number of fields with static colors 
// Effect Type: Static
// Parameter: Hue
// ParA: determines the number of fields
// ParB: derermines the  color combination scheme (analog or analog-complementary)

class StaticFields : public virtual LedEffect {
  private:
    int _fieldCount;
    int _colorMode;    

    int colorStep(int scheme, int index);

  public:
    StaticFields(TLedStrip& ledstrip):LedEffect(ledstrip) {};
    void init(int parA, int parB);
    void update(int event);
    byte getParamMap()     { return _HUE_| _VAL_; };
    byte getParamDefault() { return _HUE_; };  
};
// ---------------------------------------------------------------------------
// Display a number of fields with changing colors 
// Effect Type: Dynamic 
// Parameter: Speed/Velocity
// ParA: determines the number of fields
// ParB: derermines the  color combination scheme (analog or analog-complementary)

class DynamicFields : public StaticFields {

  public:
    DynamicFields(TLedStrip& ledstrip):LedEffect(ledstrip), StaticFields(ledstrip) {};
    void init(int parA, int parB);
    void update(int event);
    byte getParamMap()     { return _VAL_| _VEL_; };
    byte getParamDefault() { return _VEL_; };    
};
#endif
