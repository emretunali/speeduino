#ifndef GLOBALS_H
#define GLOBALS_H
#include <Arduino.h>

const byte ms_version = 20;
const byte signature = 20;
const byte data_structure_version = 2; //This identifies the data structure when reading / writing. 
const byte page_size = 128;

//Handy bitsetting macros
#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1<<(b)))
#define BIT_CHECK(var,pos) ((var) & (1<<(pos)))

//Define bit positions within engine virable
#define BIT_ENGINE_RUN      0     // Engine running
#define BIT_ENGINE_CRANK    1   // Engine cranking
#define BIT_ENGINE_ASE      2    // after start enrichment (ASE)
#define BIT_ENGINE_WARMUP   3  // Engine in warmup
#define BIT_ENGINE_ACC      4    // in TPS acceleration mode
#define BIT_ENGINE_DCC      5    // in deceleration mode
#define BIT_ENGINE_MAP      6    // in MAP acceleration mode
#define BIT_ENGINE_IDLE     7  // idle on

//Define masks for Squirt
#define BIT_SQUIRT_INJ1           0  //inj1 Squirt
#define BIT_SQUIRT_INJ2           1  //inj2 Squirt
#define BIT_SQUIRT_SCHSQRT        2  //Scheduled to squirt
#define BIT_SQUIRT_SQRTING        3  //Squirting
#define BIT_SQUIRT_INJ2SCHED      4
#define BIT_SQUIRT_INJ2SQRT       5  //Injector2 (Schedule2)
#define BIT_SQUIRT_BOOSTCTRLOFF   6  //Squirting Injector 2

#define SIZE_BYTE   8
#define SIZE_INT    16

//Table sizes
#define CALIBRATION_TABLE_SIZE 512
#define CALIBRATION_TEMPERATURE_OFFSET 40 // All temperature measurements are stored offset by 40 degrees. This is so we can use an unsigned byte (0-255) to represent temperature ranges from -40 to 215

#define SERIAL_BUFFER_THRESHOLD 32 // When the serial buffer is filled to greater than this threshold value, the serial processing operations will be performed more urgently in order to avoid it overflowing. Serial buffer is 64 bytes long, so the threshold is set at half this as a reasonable figure

//The status struct contains the current values for all 'live' variables
//In current version this is 64 bytes
struct statuses {
  volatile boolean hasSync;
  unsigned int RPM;
  byte MAP;
  byte TPS; //The current TPS reading (0% - 100%)
  byte TPSlast; //The previous TPS reading
  unsigned long TPS_time; //The time the TPS sample was taken
  unsigned long TPSlast_time; //The time the previous TPS sample was taken
  byte tpsADC; //0-255 byte representation of the TPS
  byte tpsDOT;
  byte VE;
  byte O2;
  int coolant;
  int cltADC;
  int IAT;
  int iatADC;
  int batADC;
  int O2ADC;
  int dwell;
  byte battery10; //The current BRV in volts (multiplied by 10. Eg 12.5V = 125)
  byte advance;
  byte corrections;
  byte TAEamount; //The amount of accleration enrichment currently being applied
  byte egoCorrection; //The amount of closed loop AFR enrichment currently being applied
  byte wueCorrection; //The amount of warmup enrichment currently being applied
  byte batCorrection; //The amount of battery voltage enrichment currently being applied
  byte afrTarget;
  unsigned long TAEEndTime; //The target end time used whenever TAE is turned on
  volatile byte squirt;
  volatile byte spark;
  byte engine;
  unsigned int PW; //In uS
  volatile byte runSecs; //Counter of seconds since cranking commenced (overflows at 255 obviously)
  volatile byte secl; //Continous 
  volatile int loopsPerSecond;
  int freeRAM;
  
  //Helpful bitwise operations:
  //Useful reference: http://playground.arduino.cc/Code/BitMath
  // y = (x >> n) & 1;    // n=0..15.  stores nth bit of x in y.  y becomes 0 or 1.
  // x &= ~(1 << n);      // forces nth bit of x to be 0.  all other bits left alone.
  // x |= (1 << n);       // forces nth bit of x to be 1.  all other bits left alone.
  
};


//Page 1 of the config - See the ini file for further reference
//This mostly covers off variables that are required for fuel
struct config1 {
  
  byte crankCold; //Cold cranking pulsewidth modifier. This is added to the fuel pulsewidth when cranking under a certain temp threshold (ms)
  byte crankHot; //Warm cranking pulsewidth modifier. This is added to the fuel pulsewidth when cranking (ms)
  byte asePct; //Afterstart enrichment (%)
  byte aseCount; //Afterstart enrichment cycles. This is the number of ignition cycles that the afterstart enrichment % lasts for
  byte wueValues[10]; //Warm up enrichment array (10 bytes)
  byte crankingPct; //Cranking enrichment
  byte pinMapping; // The board / ping mapping to be used
  byte unused96;
  byte unused97;
  byte taeColdA;
  byte tpsThresh;
  byte taeTime;
  byte tdePct;
  
  //Display config bits
  byte displayType : 3;
  byte display1 : 3;
  byte display2 : 2;
  byte display3 : 3;
  byte display4 : 2;
  byte display5 : 3;
  byte displayB1 : 4;
  byte displayB2 : 4;
  
  byte unused105;
  byte reqFuel;
  byte divider;
  byte alternate;
  byte injOpen;
  byte injOCfuel;
  byte injPwmP;
  byte injPwmT;
  byte unused113;
  int rpmk; //2 bytes
//36
  //config1 in ini
  byte mapType : 2;
  byte strokes : 1;
  byte injType : 1;
  byte nCylinders : 4; //Number of cylinders

  //config2 in ini  
  byte cltType : 2;
  byte matType : 2;
  byte nInjectors : 4; //Number of injectors
  

  //config3 in ini
  byte engineType : 1;
  byte egoType : 1;
  byte algorithm : 1; //"Speed Density", "Alpha-N"
  byte baroCorr : 1;
  byte injTiming : 2;
  
  byte primePulse;
  byte egoRPM;
  byte fastIdleT; //Fast idle temperature
  byte egoSwitch;
  byte taeColdM;
  byte tpsMin;
  byte tpsMax;
  byte unused1;

  //48
  
};

//Page 2 of the config - See the ini file for further reference
//This mostly covers off variables that are required for ignition
struct config2 {
  
  byte triggerAngle;
  byte FixAng;
  byte Trim;
  byte CrankAng;
  byte IgHold;
  
  byte Trig_plus : 2;
  byte TrigCrank : 1;
  byte IgInv : 1;
  byte oddfire : 4;
  
  byte IdleAdv;
  byte IdleAdvTPS;
  byte IdleAdvRPM;
  byte IdleAdvCLT; //The temperature below which the idle is advanced
  byte IdleDelayTime;
  byte StgCycles; //The number of initial cycles before the ignition should fire when first cranking
  byte dwellCont; //Fixed duty dwell control
  byte dwellCrank; //Dwell time whilst cranking
  byte dwellRun; //Dwell time whilst running 
  byte triggerTeeth; //The full count of teeth on the trigger wheel if there were no gaps
  byte triggerMissingTeeth; //The size of the tooth gap (ie number of missing teeth)
  byte crankRPM; //RPM below which the engine is considered to be cranking
  byte floodClear; //TPS value that triggers flood clear mode (No fuel whilst cranking)
  byte SoftRevLim; //Soft rev limit (RPM/100)
  byte SoftLimRetard; //Amount soft limit retards (degrees)
  byte SoftLimMax; //Time the soft limit can run
  byte HardRevLim; //Hard rev limit (RPM/100)
  byte taeBins[4]; //TPS based acceleration enrichment bins (%/s)
  byte taeValues[4]; //TPS based acceleration enrichment rates (% to add)
  byte wueBins[10]; //Warmup Enrichment bins (Values are in configTable1)
  byte dwellLimit;
  byte dwellCorrectionValues[6]; //Correction table for dwell vs battery voltage
  //Full - All 128bytes of this table are used

  
};

//Page 3 of the config - See the ini file for further reference
//This mostly covers off variables that are required for AFR targets and closed loop
struct config3 {
  
  byte egoAlgorithm : 2;
  byte egoType : 2;
  byte unused : 4;
  
  byte egoKP;
  byte egoKI;
  byte egoKD;
  byte egoTemp; //The temperature above which closed loop functions
  byte egoCount; //The number of ignition cylces per step
  byte egoDelta; //The step size (In %) when using simple algorithm
  byte egoLimit; //Maximum amount the closed loop will vary the fueling
  byte ego_min; //AFR must be above this for closed loop to function
  byte ego_max; //AFR must be below this for closed loop to function
  byte ego_sdelay; //Time in seconds after engine starts that closed loop becomes available
  byte egoRPM; //RPM must be above this for closed loop to function
  byte egoTPSMax; //TPS must be below this for closed loop to function
  byte egoLoadMax; //Load (TPS or MAP) must be below this for closed loop to function
  byte egoLoadMin; //Load (TPS or MAP) must be above this for closed loop to function
  byte voltageCorrectionBins[6]; //X axis bins for voltage correction tables
  byte injVoltageCorrectionValues[6]; //Correction table for injector PW vs battery voltage
  byte unused107;
  byte unused108;
  byte unused109;
  byte unused110;
  byte unused111;
  byte unused112;
  byte unused113;
  byte unused114;
  byte unused115;
  byte unused116;
  byte unused117;
  byte unused118;
  byte unused119;
  byte unused120;
  byte unused121;
  byte unused122;
  byte unused123;
  byte unused124;
  byte unused125;
  byte unused126;
  byte unused127;

  
};

byte pinInjector1; //Output pin injector 1
byte pinInjector2; //Output pin injector 2
byte pinInjector3; //Output pin injector 3 is on
byte pinInjector4; //Output pin injector 4 is on
byte pinInjector5; //Placeholder only - NOT USED
byte pinInjector6; //Placeholder only - NOT USED
byte pinInjector7; //Placeholder only - NOT USED
byte pinInjector8; //Placeholder only - NOT USED
byte pinCoil1; //Pin for coil 1
byte pinCoil2; //Pin for coil 2
byte pinCoil3; //Pin for coil 3
byte pinCoil4; //Pin for coil 4
byte pinTrigger; //The CAS pin
byte pinTrigger2; //The Cam Sensor pin
byte pinTPS;//TPS input pin
byte pinMAP; //MAP sensor pin
byte pinMAP2; //2nd MAP sensor (Currently unused)
byte pinIAT; //IAT sensor pin
byte pinCLT; //CLS sensor pin
byte pinO2; //O2 Sensor pin
byte pinBat; //O2 Sensor pin
byte pinDisplayReset; // OLED reset pin
byte pinTachOut; //Tacho output
byte pinSpareTemp1; // Future use only
byte pinSpareTemp2; // Future use only
byte pinSpareOut1; //Generic output
byte pinSpareOut2; //Generic output
byte pinSpareOut3; //Generic output
byte pinSpareOut4; //Generic output
byte pinSpareOut5; //Generic output
byte pinSpareOut6; //Generic output

// global variables // from speeduino.ino
extern struct statuses currentStatus; // from speeduino.ino
extern struct table3D fuelTable; //8x8 fuel map
extern struct table3D ignitionTable; //8x8 ignition map
extern struct table3D afrTable; //8x8 afr target map
extern struct table2D taeTable; //4 bin TPS Acceleration Enrichment map
extern struct table2D WUETable; //10 bin Warm Up Enrichment map (2D)
extern struct config1 configPage1;
extern struct config2 configPage2;
extern struct config3 configPage3;
extern unsigned long currentLoopTime; //The time the current loop started (uS)
extern unsigned long previousLoopTime; //The time the previous loop started (uS)
extern byte ignitionCount;
extern byte cltCalibrationTable[CALIBRATION_TABLE_SIZE];
extern byte iatCalibrationTable[CALIBRATION_TABLE_SIZE];
extern byte o2CalibrationTable[CALIBRATION_TABLE_SIZE];
extern volatile int toothHistory[512];
extern volatile int toothHistoryIndex;


#endif // GLOBALS_H
