
// ---------------------------------------------------------------------------
// CONFIGURATION ITEMS
// Number of LEDs on the LedStrip (required to size the LedStrip memory buffer)
// 26, 87, 160
#define       _LED_COUNT_               25

// Number of active effects (used to size the effect array)
#define       _EFFECT_COUNT_            10

// Time [ms] an effect is displayed until its cycles to the next 
#define       _CYCLE_TIME_              ((unsigned long)(30000))

// Time [ms] to wait after a setting change before the new settings are saved
#define       _WAIT_SAVE_               ((unsigned long)(10000))

// ---------------------------------------------------------------------------

// Bit to indicate cycling mode
#define       _MODE_CYCLER_             (0x01)
#define       _EEPROM_OFFSET_           4
#define       _EEPROM_MARKER_           ((byte)(11))
#define       _EEPROM_RECORD_MARKER_    ((byte)(11))
#define       _EEPROM_RECORD_LENGTH_    16

// ---------------------------------------------------------------------------

struct Effect_t {
  LedEffect* pAppl;
  int        ParA;
  int        ParB;  
};

// ---------------------------------------------------------------------------
class TLEDStripShow {

  private:
    byte   _mode;
    byte   _effectIx;
    byte   _buffer[_EEPROM_RECORD_LENGTH_];
    TTimer TimerCycler;
    TTimer TimerSaving;
    
  public:
    TLEDStripShow() {};
    void run();
    void flash();
    
    void enableCycling();
    void disableCycling();
    byte isCyclingEnabled();

    void setMode(byte mode);
    byte getMode();

    byte setEffect(byte ix);
    byte nextEffect();
    byte getEffect();

    void delayedSaving();    
    void savingDone();    
    void loadSettings(byte ix);
    void saveSettings(byte ix);
    void loadProgram();
    void saveProgram();
};

