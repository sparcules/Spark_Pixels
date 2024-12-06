/*@file     SparkPixels_FastLED.ino
 * @authors  Kevin Carlborg
 * @version  2022.05.12
 * @date     12-May-2022
 * @brief    Added Scheduling Task called timerTimer to auto turn on your pixels
 *
 * @version  V0.2
 * @date     05-February-2020
 * @brief    Commented out SYSTEM_MODE(SEMI_AUTOMATIC) and Particle.connect()
 *           Defaulted PIXEL_PIN to D0 and NUM_LEDS to 512
 *
 * @authors  Kevin Carlborg
 * @version  V0.1
 * @date     06-September-2017
 * @brief    Neopixel strip Powered by the Particle Photon and the FastLED Library 
 *           Modes are toggeled by the Spark Pixels Android app
 *
 * @extended DemoReel100.ino from FASTLED Library
 * @functions: rainbow,confetti,sinelon,bpn,juggle
 * @author   Mark Kriegsman
 * @version  NA
 * @date     December 2014
 ******************************************************************************
 Copyright (c) 2015 Kevin Carlborg  All rights reserved.
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation, either
 version 3 of the License, or (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 You should have received a copy of the GNU Lesser General Public
 License along with this program; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/
 
#include <FastLED.h>
#include "math.h"

FASTLED_USING_NAMESPACE;

//Don't connect to the cloud first so we can turn on the lights right away
//SYSTEM_MODE(SEMI_AUTOMATIC);   
SYSTEM_THREAD(ENABLED);


//User Defines
#define BUILD_FILE_NAME             "FastPixelsTimer"
#define BUILD_REVISION              "2022.05.12"
#define GET_TEMP_ENABLED            FALSE    //Should we measure a temp sensor?
#define TIME_ZONE_OFFSET	        -5.0	//The offset to set your region's time correctly  
#define DEFAULT_BRIGHTNESS			255		//Set how bright you want your pixels on startup
#define DEFAULT_SPEED				5
#define SHUFFLE_ADVANCE_TIME        60000   //in milli Seconds
#define SUPPLY_MAX_CURRENT          15000   //in mA
#define TRANSITION_DELAY            30

//PIXEL Defines
#define NUM_LEDS   		    24
#define PIXEL_TYPE 		    NEOPIXEL //WS2812B  //WS2811
#define PIXEL_PIN			D2  

//FastLED array
CRGB leds[NUM_LEDS];

//Particle Defines
#define MAX_PUBLISHED_STRING_SIZE   622    //defined by Particle Industries
#define TEXT_LENGTH					63	   //Max character length passed to a Cloud Function

#ifndef PI
#define PI  3.1415926535
#endif

//Misc Defines
#define CLAMP_255(v) (v > 255 ? 255 : (v < 0 ? 0 : v))


/* ======================= ADD NEW MODE ID HERE. ======================= */
// Mode ID Defines
enum modes {
    OFF = 0,
	NORMAL,                //credit: Kevin Carlborg
	COLORALL,              //credit: Kevin Carlborg
	SHUFFLE,			   //credit: Kevin Carlborg
	IFTTTWEATHER,          //credit: Kevin Carlborg
	DIGI,            	   //credit: Kevin Carlborg
	CHEERLIGHTS,           //credit: Alex Hornstein, Werner Moecke (stability fixes), Kevin Carlborg(extra transition effects)
	RIPPLE,                //credit: Andrew Tuline - https://pastebin.com/LfBsPLRn
	RAINBOW,			   //credit: Mark Kriegsman - DemoReel100 example from fastled library
	CONFETTI,			   //credit: Mark Kriegsman - DemoReel100 example from fastled library
	SINELON,			   //credit: Mark Kriegsman - DemoReel100 example from fastled library
	JUGGLE,				   //credit: Mark Kriegsman - DemoReel100 example from fastled library
	BPM,				   //credit: Mark Kriegsman - DemoReel100 example from fastled library
	WIPE,				   //credit: Kevin Carlborg
	CRASH,                 //credit: Kevin Carlborg - previously called COLLIDE
	COLORCYCLE,            //credit: Kevin Carlborg
	BEATWAVE,              //credit: Andrew Tuline - https://github.com/atuline/FastLED-Demos/blob/master/beatwave/beatwave.ino
	BLENDWAVE,             //credit: Andrew Tuline - https://github.com/atuline/FastLED-Demos/blob/master/blendwave/blendwave.ino
	LIGHTENING,			   //credit: Andrew Tuline - https://github.com/atuline/FastLED-Demos/blob/master/lightnings/lightnings.ino
	NOISE16_1,			   //credit: Andrew Tuline - https://github.com/atuline/FastLED-Demos/blob/master/noise16_1/noise16_1.ino
	FIREFLY,               //credit: Mark Kriegsman - https://github.com/FastLED/FastLED/blob/master/examples/Fire2012/Fire2012.ino
	PALETTECROSSFADE,	   //credit: Andrew Tuline - https://github.com/atuline/FastLED-Demos/blob/master/palettecrossfade/palettecrossfade.ino
	THREESINE,			   //credit: Andrew Tuline - https://github.com/atuline/FastLED-Demos/blob/master/three_sin_pal_demo/three_sin_pal_demo.ino
	COLORPALETTE,          //credit: Mark Kriegsman- https://github.com/FastLED/FastLED/blob/master/examples/ColorPalette/ColorPalette.ino
	ICYFIRE,               //credit: Mark Kriegsman, customized by  Kevin Carlborg
	FIRENICE,			   //credit: Mark Kriegsman, customized by  Kevin Carlborg
	PINKYFIRE,			   //credit: Mark Kriegsman, customized by  Kevin Carlborg
	PICKAFIRE,			   //credit: Mark Kriegsman, customized by  Kevin Carlborg
	COLOREXPLOSION,        //credit: https://github.com/pololu/pololu-led-strip-arduino/blob/master/examples/LedStripXmas/LedStripXmas.ino
	RANDOMCOLORWALK,       //credit: https://github.com/pololu/pololu-led-strip-arduino/blob/master/examples/LedStripXmas/LedStripXmas.ino
	WARMWHITESHIMMER, 	   //credit: https://github.com/pololu/pololu-led-strip-arduino/blob/master/examples/LedStripXmas/LedStripXmas.ino
	TRADITIONALCOLORS, 	   //credit: https://github.com/pololu/pololu-led-strip-arduino/blob/master/examples/LedStripXmas/LedStripXmas.ino
	GRADIENT, 			   //credit: https://github.com/pololu/pololu-led-strip-arduino/blob/master/examples/LedStripXmas/LedStripXmas.ino
	BRIGHTTWINKLE, 		   //credit: https://github.com/pololu/pololu-led-strip-arduino/blob/master/examples/LedStripXmas/LedStripXmas.ino
	CHRISTMASLOOP,		   //Credit: Kevin Carlborg
	COLORTEMP,			   //credit: Mark Kriegsman, https://github.com/adafruit/FastLED/blob/master/examples/ColorTemperature/ColorTemperature.ino
	NOISE16_2,			   //credit: Andrew Tuline - https://github.com/atuline/FastLED-Demos/blob/master/noise16_2/noise16_2.ino
	NOISE16_3,			   //credit: Andrew Tuline - https://github.com/atuline/FastLED-Demos/blob/master/noise16_3/noise16_3.ino
	THEATERCHASE,          //credit: Neopixel Library, Kevin Carlborg port to FastPixel Library
	COLORWAVES,          
	CHRISTMAS_LIGHTS,    
	PICK2PALETTE,        

    INDEX = 99      //credit: Kevin Carlborg - Use for setup to see where sections start/stop. Only send integer values in the text box of the app
}; 

typedef struct modeParams {
   uint8_t 	 modeId;
   char      modeName[20];      //Tell the android app what the mode title is
   uint8_t   numOfColors;       //Tell the android app home many colors to expect. Max number is 6
   uint8_t   numOfSwitches;     //Tell the android app home many switches to expect. Max number is 4
   bool      textInput;         //Tell the android app if a text input is needed
} modeParams;

const uint8_t MAX_NUM_COLORS = 6;
const uint8_t MAX_NUM_SWITCHES = 4;

/* ======================= ADD NEW MODE STRUCT HERE. ======================= */
//modeId and modeName should be the same name to prevent confusion
//Use this struct array to neatly organize and correlate Mode name with number of colors neede d,
//  the number of switches needed, and whether a text input is needed or not
//The Android app uses these parameters to help populate the view 
//and to know how many colors and switches and text input to ask to update
modeParams modeStruct[] =
{ 
    /*     modeId             modeName 	        #Colors #Switches  textInput
     *     -------------      ------------------ ------ ---------  ----- */
        {  OFF,               "Off",                 0,      0,      FALSE  },
        {  NORMAL,            "Normal",              0,      0,      FALSE  },  //credit: Kevin Carlborg
        {  SHUFFLE,           "Shuffle",             0,      0,      FALSE  },  //credit: Kevin Carlborg
		{  BEATWAVE,          "BeatWave",            0,      0,      FALSE  },  //credit: Andrew Tuline
		{  BLENDWAVE,         "BlendWave",           0,      0,      FALSE  },  //credit: Andrew Tuline
        {  BPM,               "BPM",                 0,      0,      FALSE  },  //credit: Mark Kriegsman from DemoReel100 FastLED library
        {  CHEERLIGHTS,       "CheerLights",         0,      0,      FALSE  },  //credit: Alex Hornstein, Werner Moecke (stability fixes), Kevin Carlborg(extra transition effects)
		{  CHRISTMASLOOP,     "ChristmasLoop",       0,      0,      FALSE  },  //credit: Alex Hornstein, Werner Moecke (stability fixes), Kevin Carlborg(extra transition effects)
		{  COLOREXPLOSION,    "ColorExplosion",      0,      0,      FALSE  },  //credit: pololu.com
		{  RANDOMCOLORWALK,   "ColorWalk",           0,      0,      FALSE  },  //credit: pololu.com
		{  GRADIENT,          "Gradient",            0,      0,      FALSE  },  //credit: pololu.com
		{  TRADITIONALCOLORS, "TraditionalColors",   0,      0,      FALSE  },  //credit: pololu.com
		{  CHRISTMAS_LIGHTS,  "ChristmasLights",     0,      0,      FALSE  },  //credit: Kevin Carlborg
		{  BRIGHTTWINKLE,     "Twinkle",             0,      0,      FALSE  },  //credit: pololu.com
		{  WARMWHITESHIMMER,  "WarmWhiteShimmer",    0,      0,      FALSE  },  //credit: pololu.com
		{  PICK2PALETTE,      "PickTwo",             2,      0,      FALSE  },  //credit: Kevin Carlborg
		{  COLORALL,          "ColorAll",            1,      0,      FALSE  },  //credit: Kevin Carlborg
		{  COLORCYCLE,        "ColorCycle",          0,      0,      FALSE  },  //credit: Kevin Carlborg
		{  COLORPALETTE,      "ColorPalatte",        0,      1,      FALSE  },  //credit: Mark Kriegsman
	//	{  COLORWAVES,        "ColorWaves",          0,      0,      FALSE  },  //credit: Mark Kriegsman
	//	{  COLORTEMP,         "ColorTemp",           0,      0,      FALSE  },  //credit: Kevin Carlborg
        {  CONFETTI,          "Confetti",            0,      0,      FALSE  },  //credit: Mark Kriegsman from DemoReel100 FastLED library
		{  CRASH,             "Crash",               2,      2,      FALSE  },  //credit: Kevin Carlborg
		{  DIGI,              "Digi",                1,      3,      FALSE  },  //credit: Kevin Carlborg
		{  FIREFLY,           "FireFly",             0,      2,      FALSE  },  //credit: Mark Kriegsman - https://github.com/FastLED/FastLED/blob/master/examples/Fire2012/Fire2012.ino
	//	{  FIRENICE,          "Miser Bros",          0,      2,      FALSE  },  //credit: Mark Kriegsman - https://github.com/FastLED/FastLED/blob/master/examples/Fire2012/Fire2012.ino
	//	{  ICYFIRE,           "IcyFire",             0,      2,      FALSE  },  //credit: Mark Kriegsman - https://github.com/FastLED/FastLED/blob/master/examples/Fire2012/Fire2012.ino
	//	{  PINKYFIRE,         "PinkyFire",           0,      2,      FALSE  },  //credit: Mark Kriegsman - https://github.com/FastLED/FastLED/blob/master/examples/Fire2012/Fire2012.ino
	//	{  PICKAFIRE,         "PickAFire",           3,      0,      FALSE  },  //credit: Mark Kriegsman - https://github.com/FastLED/FastLED/blob/master/examples/Fire2012/Fire2012.ino
		{  INDEX,             "Index",               1,      0,      TRUE   },  //credit: Kevin Carlborg
        {  JUGGLE,            "Juggle",              0,      0,      FALSE  },  //credit: Mark Kriegsman from DemoReel100 FastLED library
		{  LIGHTENING,        "Lightening",          0,      0,      FALSE  },  //credit: Andrew Tuline
		{  NOISE16_1,         "Noise1",              0,      0,      FALSE  },  //credit: Andrew Tuline
		{  NOISE16_2,         "Noise2",              0,      0,      FALSE  },  //credit: Andrew Tuline
		{  NOISE16_3,         "Noise3",              0,      0,      FALSE  },  //credit: Andrew Tuline
	//	{  PALETTECROSSFADE,  "PaletteCrossFade",    0,      0,      FALSE  },  //credit: Andrew Tuline
        {  RAINBOW,           "Rainbow",             0,      1,      FALSE  },  //credit: Andrew Tuline - https://pastebin.com/LfBsPLRn
		{  RIPPLE,            "Ripple",              0,      0,      FALSE  },  //credit: Mark Kriegsman from DemoReel100 FastLED library
		{  SINELON,           "Sinelon",             0,      0,      FALSE  },  //credit: Mark Kriegsman from DemoReel100 FastLED library
		{  THEATERCHASE,      "TheaterChase",        0,      0,      FALSE  },  //credit: Neopixel Library, Kevin Carlborg port to FastPixel Library
		{  THREESINE,         "3Sine",               0,      0,      FALSE  },  //credit: Andrew Tuline
		{  WIPE,              "Wipe",                1,      4,      FALSE  },  //credit: Kevin Carlborg
};

typedef struct switchParams {
   uint8_t   modeId;
   char      switch1Title[20];
   char      switch2Title[20];
   char      switch3Title[20];
   char      switch4Title[20];
} switchParams;

/* ======================= ADD ANY REQUIRED SWITCH TITLES HERE ======================= */
// The Android app needs to know what the titles of the switches are
// The modeID MUST be the same as the modeID used up in the modeStruct[] array
// i.e. - If you declare a mode to have 3 switches in the modeStruct[] array above,
//          you should then have 3 titles declared here. 
switchParams switchTitleStruct[] = 
{ 
    /*     modeId           S1Title                S2Title                S3Title                S4Title 
     *     ---------------  ---------------------- ---------------------- ---------------------- ---------------------- */
		  {  CRASH,         "Color Sweep",         "Random Colors",       "",                    ""                  }, 
		  {  DIGI,          "Color Sweep",         "Random Colors",       "Fade In",             ""                  },
		  {  COLORPALETTE,  "No Blend",            "Random Blend",        "",                    ""                  },
		  {  FIREFLY,       "Roaring",             "Flickery",            "",                    ""                  }, 
		  {  FIRENICE,      "Roaring",             "Flickery",            "",                    ""                  }, 
		  {  ICYFIRE,       "Roaring",             "Flickery",            "",                    ""                  }, 
		  {  PINKYFIRE,     "Roaring",             "Flickery",            "",                    ""                  }, 
		  {  RAINBOW,       "Add Glitter",         "",                    "",                    ""                  },  
		  {  WIPE,          "Cycle",               "Color Sweep",         "Random Colors",       "Fade Out"          },
	  //  {  TEST,          "Switch Title 1",      "Switch Title 2",      "Switch Title 3",      "Switch Title 4"    },
};


/* ======================= ADD NEW AUX SWITCH ID HERE. ======================= */
typedef struct auxSwitchParams {
    uint8_t  auxSwitchId;
    bool     auxSwitchState;
    char     auxSwitchTitle[20];
    char     auxSwitchOnName[20];
    char     auxSwitchOffName[20];
} auxSwitchParams;

// AUX SWITCH Defines
#define AUXSWITCH_SIZE		sizeof(auxSwitchParams)							//62
#define MAX_AUX_SWITCHES    (int)(MAX_PUBLISHED_STRING_SIZE/AUXSWITCH_SIZE)	//10, Max allowed global integer values accessible through the cloud
enum auxSwitchIDs {
    ASO         = 0,
    LIGHTSENSOR,
    SHFL,
    RLM
};

/** ======================= ADD NEW AUX SWITCH STRUCT HERE. =======================
 * Use these switches to turn things on and off or toggle bewteen two options
 * @Param auxSwitchId        The ID of the switch     
 * @Param auxSwitchState     set the default switch state here
 * @Param auxSwitchTitle     Title/Description of the switch read in by the app
 * @Param auxSwitchOnTitle   Title of the ON/TRUE state of the switch read in by the app
 * @Param auxSwitchOffTitle  Title of the OFF/FALSE state of the switch read in by the app
 */
auxSwitchParams auxSwitchStruct[] = 
{ 
     /*    auxSwitchId      auxSwitchState  auxSwitchTitle         auxSwitchOnName       auxSwitchOffName    
     *     ---------------  --------------  ---------------------- ---------------------- ----------------------*/
	    {  SHFL,            FALSE,		   "Shuffle",             "ON",                  "OFF"                },
		//{  ASO,             TRUE,          "Auto Shut Off",       "ON",                  "OFF"                }, 
		{  RLM,             FALSE,		   "On Startup",          "Run Last Mode",       "Run Default Mode"	  }
	    //{  LIGHTSENSOR,     FALSE,         "Brightness Control",  "Light Sensor",        "App Controlled"     },
};

/* ========================= Local Aux Switch Flags =========================== */

bool autoShutOff;   //Should we shut off the lights at certain times? This is toggled from the app
                    //Configure the Auto Shut Off times in the loop() function 
bool rememberLastMode;   //Should we remember the last mode ran? This is toggled from the app and kept in EEPROM
bool brightnessControl;

/* ======================= ADD NEW AUX INTEGER ID HERE. ======================= */
typedef struct auxIntegerParams {
    uint8_t  auxIntegerId;
    int      auxIntegerValue;
    char     auxIntegerTitle[24];
} auxIntegerParams;

// AUX INTEGER Defines
#define AUXINT_SIZE			sizeof(auxIntegerParams)						//35
#define MAX_AUXINT			(int)(MAX_PUBLISHED_STRING_SIZE/AUXINT_SIZE)	//19 , Max allowed global integer values accessible through the cloud

enum auxIntegerIDs {
    INT_SHFL_ADVANCE_TIME =  0,
    INT_LED_PIN,
    //OUR_INT_HERE,
    //TEST1,
    //TEST2
};


/** ======================= ADD NEW AUX INTEGER STRUCT HERE. =======================
 * Use these switches to turn things on and off or toggle bewteen two options
 * @Param auxIntegerId        The ID of the integer     
 * @Param auxIntegerValue     set the default integer value here
 * @Param auxIntegerTitle     Title/Description of the integer read in by the app
 */
auxIntegerParams auxIntegerStruct[] = 
{ 
     /*   auxSwitchId            auxIntegerValue  auxIntegerTitle       *
     *    ---------------        ---------------  ---------------------- */
        { INT_SHFL_ADVANCE_TIME, 60,		      "Shuffle Skip Seconds"},
	 //	{ INT_LED_PIN, 	         PIXEL_PIN,		  "Pixel Drive Pin"},
     //   {  TEST1,  2000,		      			"Testing String Length 4"},
     // {  TEST2,  5,		          			"Testing #2"}
	 // {  YOUR_INT_HERE,    	22,		      	"Your Int Title"      }

};

/* 
 * Some really useful time defines and macros to help setup the timers
 * Thanks @bko
 * https://github.com/bkobkobko/SparkTimeAlarms/blob/master/firmware/TimeAlarms.h
 */
/* Useful Time Constants */
#define MILLIS_PER_SEC (1000UL)
#define SECS_PER_MIN   (60UL)
#define SECS_PER_HOUR  (3600UL)
#define SECS_PER_DAY   (SECS_PER_HOUR * 24UL)
#define MIN_PER_HOUR   (60UL)
#define DAYS_PER_WEEK  (7)
#define SECS_PER_WEEK  (SECS_PER_DAY * DAYS_PER_WEEK)
#define SECS_PER_YEAR  (SECS_PER_WEEK * 52UL)
#define SECS_YR_2000   (946684800UL) // the time at the start of y2k
/* Useful Macros for getting elapsed time */
#define seconds2Millis(_time_) (_time_ * MILLIS_PER_SEC)  
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)  
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN) 
#define numberOfHours(_time_) (( _time_% SECS_PER_DAY) / SECS_PER_HOUR)
#define dayOfWeek(_time_)  ((( _time_ / SECS_PER_DAY + 4)  % DAYS_PER_WEEK)+1) // 1 = Sunday
#define elapsedDays(_time_) ( _time_ / SECS_PER_DAY)  // this is number of days since Jan 1 1970
#define elapsedSecsToday(_time_)  (_time_ % SECS_PER_DAY)   // the number of seconds since last midnight 
// The following macros are used in calculating alarms and assume the clock is set to a date later than Jan 1 1971
// Always set the correct time before settting alarms
#define previousMidnight(_time_) (( _time_ / SECS_PER_DAY) * SECS_PER_DAY)  // time at the start of the given day
#define nextMidnight(_time_) ( previousMidnight(_time_)  + SECS_PER_DAY )   // time at the end of the given day 

/** ======================= TIMER STRUCTS HERE. =======================
 * @Param uint8_t  timerId    	The ID of the Timer  
 * 								We are only allowing 8 timers so ID 0b0000 to 0b0111
 *								Bit 3 will be used to signify whether the timer is active or not 
 *								Bit 0 to 2 = ID # 0 to 7
 *								Bit 3 = 0 > Disabled
 *								Bit 3 = 1 > Enabled
 * @Param uint16_t turnOnTime 	Combined hour and minute into one value, i.e. 0x8A7 = 2215 = 10:15pm  
 *                              NO_TRIGGER          = 0FFF
 *                              HAS_BEEN _TRIGGERED = 1xxx
 * 					    	    TRIGGERED_AT_DAWN   = 2xxx
 *							  	TRIGGERED_AT_DUSK   = 4xxx
 * @Param uint16_t turnOffTime	Combined hour and minute into one value, i.e. 0930 =  9:30am
 * @Param byte     daysOfWeek   Bit 0 = frequency   1 = Is One Shot, 0 = Is Repeating
 *                    			Bit 1 = Sunday      1 = Enable, 0 = Disable
 *                    			Bit 2 = Monday      1 = Enable, 0 = Disable
 *                    			Bit 3 = Tuesday     1 = Enable, 0 = Disable
 *                    			Bit 4 = Wednesday   1 = Enable, 0 = Disable
 *                    			Bit 5 = Thursday    1 = Enable, 0 = Disable
 *                    			Bit 6 = Friday      1 = Enable, 0 = Disable
 *                    			Bit 7 = Saturday    1 = Enable, 0 = Disable
 * @Param uint16_t brightness	Sets the brightness
 * @Param char     modeInfo[64] Same mode and parameter string that is sent to SetMode
 */
typedef struct timerParams {
    uint8_t  timerId;
    char     timerName[20];
    uint16_t turnOnTime;
    uint16_t turnOffTime;
    byte     daysOfWeek;
    uint8_t  brightness;
    char     modeInfo[64];
} timerParams;

// Timer Defines
#define TIMER_SIZE	        sizeof(timerParams)		                	//72
#define NUM_TIMERS	        8       //(int)(MAX_PUBLISHED_STRING_SIZE/TIMER_SIZE) 
#define NO_TRIGGER          0x0FFF    //should be a large value > 2359 - which is 11:59pm or 23:59
#define HAS_BEEN_TRIGGERED  0x1000
#define TRIGGERED_AT_DAWN   0x2000
#define TRIGGERED_AT_DUSK   0x4000
#define ID                  0
#define TIME_VAL            1
#define REFRESH_INTERVAL    SECS_PER_DAY * 2   // 2 days worth of Seconds
// Timer Trigger States
#define OFF_TRIGGER         0
#define ON_TRIGGER          1
#define REFRESH_TRIGGER     2
#define DEFAULT_TIMER_TIME  20000 //20 seconds

//Hold all timer info in a single struct array
timerParams timerStruct[NUM_TIMERS];  

//Keep track of the next timer ID and whether it's the On trigger or Off trigger
typedef struct  {
    uint8_t  timerIdx;
    uint8_t  triggerState;
    uint8_t  triggerDay;  
    uint16_t triggerTime;
} NextTimer;
NextTimer nextTimer;

volatile bool timerTriggered;
//char cBuff[75];    //Setup Timer Mode Info on a Trigger

//Preset speed @Param timerTurnOn
const int speedPresets[] = { 120, 100, 80, 70, 50, 30, 20, 10, 1};  //in mSec, slow to fast       

//Temp Sensor constants	
const double refVoltage = 3.3;    //Voltage used as reference for analogRead
const double MINFANTEMP = 100.0;  //Min Temp to turn fan on low speed in Deg Fahrenheit
const double MAXFANTEMP = 115.0;  //Max Temp to turn fan on max speed in Deg Fahrenheit
const double MAXTEMP = 140.0;     //Shut Off Temp in Deg Fahrenheit


//Time Interval constants       hh*mm*ss*ms    
#define LAST_TIMER_INTERVAL		 1*60*1000
#define oneHourInterval      	 1*60*60*1000  	//auto off in 1 hr when night time
#define oneDayInterval  	   	24*60*60*1000  	//time sync interval - 24 hours=
#define iftttWeatherInterval     8*60*1000  	//revert back to last mode for IFTTT Weather

//Program Flags
int currentModeID;	//This is the ID of the current mode selected - used in the case statement
int lastModeID;
int run;    	//Use this for modes that don't need to loop. Set the color, then stop sending commands to the pixels
volatile int stop;   	//Use this to break out of sequence loops when changing to a new mode
bool firstLap;
bool resetFlag;
bool shuffleMode;
bool runChristmasModes;
bool isBootingUp;
volatile bool stopShuffle;		//Set to TRUE when the Interrupt Timer shuffleTimer gets triggered

/**
  * Shuffle Mode Helpers
  * Use the array below to prevent shuffled modes from playing more than once without cycling through all the 
  * modes first. The array will get populated with values 0 through (the number of modes - 1). Then we'll shuffle
  * the array up and step through the array during the Shuffle mode. the shuffleIdx will keep track of our position 
  * in the modeShuffleOrder array.
  */
int shuffleIdx = 0;
int modeShuffleOrder[(int)(sizeof modeStruct / sizeof modeStruct[0])];

//Misc variables
int pixelPin = 0;
int speed;	//not to be confused with speedIndex below, this is the local speed (delay) value
unsigned int seed = 0; 
unsigned long previousMillis = 0;
unsigned long lastTimerCheckInterval = 0;
unsigned long lastCommandReceived = 0;
unsigned long lastSync = 0;
unsigned long timeStamp = 0;
float timeZone = 0.0;
uint8_t colorWheel;
uint8_t colorWheel2;
uint32_t defaultColor = 0xFFFF3C;	//The NORMAL mode color

typedef struct  {
    uint32_t  C1;
    uint32_t  C2; 
	uint32_t  C3;
    uint32_t  C4;
	uint32_t  C5;
    uint32_t  C6;	
} Colors;
Colors colors;

typedef struct  {
    bool  SW1;
    bool  SW2; 
	bool  SW3;
    bool  SW4;
} Switches;
Switches switches;

int lastBrightness;
unsigned int loopCount = 0;
unsigned int maxLoops;

//Spark Pin Defines
const int TEMP_SENSOR_PIN = A7;   //TMP36 sensor on this pin.
const int LIGHT_SENSOR_PIN = A6;  //Photo Resistor

//Spark Cloud Variables
int wifi = 0;   //used for general info and setup
int hour=0;    //used for general info and setup
int minute = 0;
int speedIndex;     				//Let the cloud know what speed preset we are using
int brightness;                     //How bright do we want these things anyway
double temp = 0.0;   
double pressure = 0.0;
double humidity = 0.0;
char modeNameList[MAX_PUBLISHED_STRING_SIZE] = "None";        //Holds all mode info comma delimited. Use this to populate the android app
char modeParamList[MAX_PUBLISHED_STRING_SIZE] = "None";
char auxSwitchList[MAX_PUBLISHED_STRING_SIZE] = "None";
char auxIntegerList[MAX_PUBLISHED_STRING_SIZE] = "None";
char timerList[MAX_PUBLISHED_STRING_SIZE] = "None";
char deviceInfo[MAX_PUBLISHED_STRING_SIZE] = "";
char currentModeName[64] = "None";  //Holds current selected mode
char textInputString[64] = "";           //Holds the Text for any mode needing a test input - only useful for a Neopixel Matrix
char debug[MAX_PUBLISHED_STRING_SIZE] = " ";                    //We might want some debug text for development
char debug2[MAX_PUBLISHED_STRING_SIZE] = " "; 

char colorValues[85];

// DEBUG Stuff
int lastMinTillNextTimer;

/* ============ SetMode return  Defines  ====================== */
#define NO_CHANGE           1000
#define BRIGHTNESS_SET      1001
#define SPEED_SET           1002
#define NOT_FOUND			"NOTFOUND"

/* ====================== EEPROM Definitions ====================== */
#define SUDO_CHECKSUM           18650
#define MAX_EEPROM_SIZE			2047	// the maximum available space in EEPROM storage (Photon)
#define CHECKSUM_SIZE			sizeof(uint16_t)
#define PERSIST_RESERVE_SIZE	sizeof(uint16_t)
#define PERSIST_COLOR_SIZE		sizeof(uint32_t)
#define PERSIST_SWITCH_SIZE		sizeof(bool)
#define PERSIST_SPEED_SIZE		sizeof(int)
#define PERSIST_BRIGHT_SIZE		sizeof(int)
#define PERSIST_LASTMODE_SIZE	sizeof(int)
#define PERSIST_AUXSW_SIZE		sizeof(uint32_t)
#define PERSIST_AUXINT_SIZE		sizeof(int)
#define PERSIST_TIMER_SIZE		sizeof(timerParams)
#define PERSIST_TZ_SIZE         sizeof(float)
#define PERSIST_PIXELPIN_SIZE   sizeof(int)

#define SUDO_CHECKSUM_ADDR		0												  // To keep track of what data has persistance
#define RESERVED_ADDR           SUDO_CHECKSUM_ADDR + CHECKSUM_SIZE
#define COLOR1_START_ADDR		RESERVED_ADDR + PERSIST_RESERVE_SIZE    	      // offset for the Color1 store
#define COLOR2_START_ADDR		COLOR1_START_ADDR + PERSIST_COLOR_SIZE    	      // offset for the Color2 store
#define COLOR3_START_ADDR		COLOR2_START_ADDR + PERSIST_COLOR_SIZE	          // offset for the Color3 store
#define COLOR4_START_ADDR		COLOR3_START_ADDR + PERSIST_COLOR_SIZE	          // offset for the Color4 store
#define COLOR5_START_ADDR		COLOR4_START_ADDR + PERSIST_COLOR_SIZE	          // offset for the Color5 store
#define COLOR6_START_ADDR		COLOR5_START_ADDR + PERSIST_COLOR_SIZE	          // offset for the Color6 store
#define SWITCH1_START_ADDR		COLOR6_START_ADDR + PERSIST_COLOR_SIZE   	      // offset for the Switch1 store
#define SWITCH2_START_ADDR		SWITCH1_START_ADDR + PERSIST_SWITCH_SIZE	      // offset for the Switch2 store
#define SWITCH3_START_ADDR		SWITCH2_START_ADDR + PERSIST_SWITCH_SIZE    	  // offset for the Switch3 store
#define SWITCH4_START_ADDR		SWITCH3_START_ADDR + PERSIST_SWITCH_SIZE    	  // offset for the Switch4 store
#define SPEED_START_ADDR		SWITCH4_START_ADDR + PERSIST_SWITCH_SIZE    	  // offset for the speedIndex store
#define BRIGHT_START_ADDR		SPEED_START_ADDR + PERSIST_SPEED_SIZE             // offset for the brightness store
#define LASTMODE_START_ADDR		BRIGHT_START_ADDR + PERSIST_BRIGHT_SIZE 		  // offset for the currentModeID store	
#define AUXSW_START_ADDR		LASTMODE_START_ADDR + PERSIST_LASTMODE_SIZE       // offset for the auxSwitchStruct switch store
#define AUXINT_START_ADDR		AUXSW_START_ADDR + PERSIST_AUXSW_SIZE             // offset for the auxIntStruct switch store
#define TIMER_START_ADDR		AUXINT_START_ADDR + PERSIST_AUXINT_SIZE*MAX_AUXINT// offset for the timer switch store	
#define TIMER_ZONE_ADDR         TIMER_START_ADDR + PERSIST_TIMER_SIZE*NUM_TIMERS
#define PIXEL_PIN_ADDR          TIMER_ZONE_ADDR + PERSIST_TZ_SIZE
#define END_TIMER_ADDR_CHK		PIXEL_PIN_ADDR + PERSIST_PIXELPIN_SIZE                 //									

/* ========================= CHEERLIGHTS mode defines ======================== */
#define POLLING_INTERVAL 3000   // how often the photon polls the cheerlights API
#define RESPONSE_TIMEOUT 500	// the timeout (in ms) to wait for a response from the cheerlights API
TCPClient client;       // a TCP instance to let us query the cheerlights API over TCP
String hostname, path;  // the URL and path to cheerlights' thingspeak directory
String response;        // the response read from querying cheerlights' thingspeak directory
CRGB lastCol;
bool connected;         // flag if we have a solid TCP connection
int requestTime, pollTime;


/* ========================= RIPPLE function defines ======================== */
int color;
int center = 0;
int step = -1;
int maxSteps = 16;
float fadeRate = 0.8;
int diff;
uint32_t currentBg = random(256);
uint32_t nextBg = currentBg;


/* ========================= DemoReel100 function defines ======================== */
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
volatile uint8_t gHue = 0; // rotating "base color" used by many of the patterns


/* ========================= Palette function defines ======================== */
CRGBPalette16 currentPalette;
CRGBPalette16 targetPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;


const TProgmemPalette16 christmasLightsPalette_p PROGMEM =
{
    CRGB::Red,  CRGB::Yellow, CRGB::Lime, 
    CRGB::Blue, CRGB::Cyan,   CRGB::Magenta,  
    CRGB::OrangeRed, 
    CRGB::Red,  CRGB::Yellow, CRGB::Lime, 
    CRGB::Blue, CRGB::Cyan,   CRGB::Magenta,
    CRGB::OrangeRed,
    CRGB::Lime, CRGB::Blue, 
};
  
/* ========================= BlendWave function defines ======================== */
CRGB clr1;
CRGB clr2;
uint8_t blendWaveSpeed;
uint8_t loc1;
uint8_t loc2;
uint8_t ran1;
uint8_t ran2;

/* ========================= ColorWave function defines ======================== */
/*https://gist.github.com/kriegsman/8281905786e8b2632aeb*/
/*#define SECONDS_PER_PALETTE 10
extern const TProgmemRGBGradientPalettePtr gGradientPalettes[];
extern const uint8_t gGradientPaletteCount;
// Current palette number from the 'playlist' of color palettes
uint8_t gCurrentPaletteNumber = 0;
CRGBPalette16 gCurrentPalette( CRGB::Black);
CRGBPalette16 gTargetPalette( gGradientPalettes[0] );
*/
/* ========================= Lightening function defines ======================== */
uint8_t flashes = 8;                                          //the upper limit of flashes per strike
unsigned int dimmer = 1;
uint8_t ledstart;                                             // Starting location of a flash
uint8_t ledlen;                                               // Length of a flash


/* ========================= Noise16_1 function defines ======================== */
uint8_t maxChanges;      // Number of tweens between palettes.

/* =========================Christmas Mode defines ======================== */
int christmasModes[] = {RANDOMCOLORWALK,COLOREXPLOSION,WARMWHITESHIMMER,TRADITIONALCOLORS,GRADIENT,BRIGHTTWINKLE, CHRISTMAS_LIGHTS};
int christmasModeCnt = 0;
	
/* ========================= FireFly2012 function defines ======================== */
  //You'll have to costomize this section to your need 
#define FORWARD		0
#define BACKWARD	1

#define SINK_START	1       // SEction 2 starting LED position
#define CAB2_START	2       // SEction 3 starting LED position
#define CAB3_START	3       // SEction 4 starting LED position

#define CAB1_COOLING	27
#define SINK_COOLING	20
#define CAB2_COOLING	22
#define CAB3_COOLING	30

//Define section ends
//The flames of all odd sections go forward and all even sections go backwards
#define section1aEnd SINK_START/2
#define section1bEnd SINK_START
#define section2aEnd ((CAB2_START-SINK_START)/2) + SINK_START
#define section2bEnd CAB2_START
#define section3aEnd ((CAB3_START-CAB2_START)/2) + CAB2_START
#define section3bEnd CAB3_START
#define section4aEnd ((NUM_LEDS-CAB3_START)/2) + CAB3_START
#define section4bEnd NUM_LEDS

//Define section Length - number of LEDs in each section
#define heat1aSize section1aEnd
#define heat1bSize section1bEnd - section1aEnd
#define heat2aSize section2aEnd - section1bEnd
#define heat2bSize section2bEnd - section2aEnd
#define heat3aSize section3aEnd - section2bEnd
#define heat3bSize section3bEnd - section3aEnd
#define heat4aSize section4aEnd - section3bEnd
#define heat4bSize section4bEnd - section4aEnd

byte heat1a[heat1aSize];	// Array of temperature readings at each simulation cell
byte heat1b[heat1bSize];	// Array of temperature readings at each simulation cell
byte heat2a[heat2aSize];	// Array of temperature readings at each simulation cell
byte heat2b[heat2bSize];	// Array of temperature readings at each simulation cell
byte heat3a[heat3aSize];	// Array of temperature readings at each simulation cell
byte heat3b[heat3bSize];	// Array of temperature readings at each simulation cell
byte heat4a[heat4aSize];	// Array of temperature readings at each simulation cell
byte heat4b[heat4bSize];	// Array of temperature readings at each simulation cell	


/* ========================= 3Sine function defines ======================== */
int wave1=0;                                                  // Current phase is calculated.
int wave2=0;
int wave3=0;
uint8_t lvl1 = 80;                                            // Any result below this value is displayed as 0.
uint8_t lvl2 = 80;
uint8_t lvl3 = 80;
uint8_t mul1 = 7;                                            // Frequency, thus the distance between waves
uint8_t mul2 = 6;
uint8_t mul3 = 5;

/* ========================= ColorTemperature function defines ======================== */
// FastLED v2.1 provides two color-management controls:
//   (1) color correction settings for each LED strip, and
//   (2) master control of the overall output 'color temperature' 
//
// THIS EXAMPLE demonstrates the second, "color temperature" control.
// It shows a simple rainbow animation first with one temperature profile,
// and a few seconds later, with a different temperature profile.
//
// The first pixel of the strip will show the color temperature.
//
// HELPFUL HINTS for "seeing" the effect in this demo:
// * Don't look directly at the LED pixels.  Shine the LEDs aganst
//   a white wall, table, or piece of paper, and look at the reflected light.
//
// * If you watch it for a bit, and then walk away, and then come back 
//   to it, you'll probably be able to "see" whether it's currently using
//   the 'redder' or the 'bluer' temperature profile, even not counting
//   the lowest 'indicator' pixel.
//
//
// FastLED provides these pre-conigured incandescent color profiles:
//     Candle, Tungsten40W, Tungsten100W, Halogen, CarbonArc,
//     HighNoonSun, DirectSunlight, OvercastSky, ClearBlueSky,
// FastLED provides these pre-configured gaseous-light color profiles:
//     WarmFluorescent, StandardFluorescent, CoolWhiteFluorescent,
//     FullSpectrumFluorescent, GrowLightFluorescent, BlackLightFluorescent,
//     MercuryVapor, SodiumVapor, MetalHalide, HighPressureSodium,
// FastLED also provides an "Uncorrected temperature" profile
//    UncorrectedTemperature;

#define TEMPERATURE_1 Tungsten100W
#define TEMPERATURE_2 OvercastSky

// How many seconds to show each temperature before switching
#define DISPLAYTIME 20
// How many seconds to show black between switches
#define BLACKTIME   3


/* ========================= Transition Definitions ========================= */
#define LINEAR		0
#define POLAR       1
#define RED         0
#define GREEN       1
#define BLUE        2
/*uint8_t clamp(unsigned value, unsigned lowClamp, unsigned highClamp);
void transitionAll(CRGB endColor, uint16_t method);
void transitionOne(CRGB endColor, uint16_t index, uint16_t method);
void transitionHelper(CRGB startColor, CRGB endColor, uint16_t index, uint16_t method, int16_t numSteps, int16_t step);
int16_t getTransitionStep(CRGB startColor, CRGB endColor, uint16_t method, int16_t numSteps, int16_t step, uint8_t whichColor);
*/
/* ========================= FadeTowardColor Definitions ========================= */
void fadeTowardColorOne(uint16_t index, const CRGB& bgColor, uint8_t fadeAmount);
void fadeTowardColorAll(  uint16_t start, uint16_t end, const CRGB& bgColor, uint8_t fadeAmount);
uint8_t fadeTowardColor( CRGB& cur, const CRGB& target, uint8_t amount);
uint8_t nblendU8TowardU8( uint8_t& cur, const uint8_t target, uint8_t amount);


/* ======================= Required Mode Prototypes =============================== */

void driveFire2012(CRGBPalette16 pal1, CRGBPalette16 pal2);
int Fire2012(CRGBPalette16 thisP, byte heat[], int num_leds, int startPixel, int endPixel, int cooling, bool direction);
int randomPixelFill(CRGB c);
int wrap(int step);
void ChangePalettePeriodically();
void FillLEDsFromPaletteColors(uint8_t colorIndex);
void SetupTotallyRandomPalette();
void SetupBlackAndWhiteStripedPalette();
void SetupPurpleAndGreenPalette();
void three_sin();
void FillLEDsFromPaletteColorsBlending( uint8_t colorIndex);
void addGlitter( fract8 chanceOfGlitter);
void beatWave();
void blendWave();
void bpm();
void cheerlights(void);
int colorAll(uint32_t c);
void colorCycle();
void colorPalette();
void confetti();
void crash(CRGB c1, CRGB c2);
void digi(CRGB c);
void driveFire2012();
void fireAndIce();
void firefly();
void icyBlueFire();
void iftttWeather(uint32_t c);
void index(CRGB c);
void juggle() ;
void lightening();
void noise16_1();
void noise16_2();
void noise16_3();
void one_color_allHSV(int ahue, int abright);
void paletteCrossFade();
void pickAFire(CRGB c1, CRGB c2, CRGB c3);
void pinkyFire();
void rainbow();
void ripple();
void sinelon();
void threeSine();
void wipe(CRGB c);
void driveChristmasModes();
void colorExplosion(unsigned char noNewBursts);
void colorExplosionColorAdjust(unsigned char *color, unsigned char propChance, unsigned char *leftColor, unsigned char *rightColor);
void brightTwinkleColorAdjust(unsigned char *color);
void randomColorWalk(unsigned char initializeColors, unsigned char dimOnly);
void randomWalk(unsigned char *val, unsigned char maxVal, unsigned char changeAmount, unsigned char directions);
void fade(unsigned char *val, unsigned char fadeTime);
void warmWhiteShimmer(unsigned char dimOnly);
void traditionalColors();
void gradient();
void brightTwinkle(unsigned char minColor, unsigned char numColors, unsigned char noNewBursts);
void colorTemperature();
void theaterChaseRainbow(void);
void colorWaves();
void colorwaves( CRGB* ledarray, uint16_t numleds, CRGBPalette16& palette);
void palettetest( CRGB* ledarray, uint16_t numleds, const CRGBPalette16& gCurrentPalette);

void christmasLightsPalette();

/* ======================= Spark Pixel Prototypes =============================== */
void arrayShuffle(int arrayToShuffle[], int arraySize);
void arrayShuffle(uint16_t arrayToShuffle[], uint16_t arraySize);
bool doesTimerHaveAnotherTriggerToday(uint8_t timerIdx);
int getAuxSwitchIndexFromID(int id);
int getModeIndexFromID(int id);
int getModeIndexFromName(String name);
String getModeNameFromID(int id);
int getSwitchTitleStructIndex(int modeId);
time_t getHoursFromTimerVal(uint16_t timeVal);
time_t getMinutesFromTimerVal(uint16_t timeVal);
time_t getSecondsFromTimerVal(uint16_t timerVal, uint16_t weekDayIdx);
int getWeather(void);
char* getWeekDayStr(void);
char* getMonthStr(void);
char* getTodaysWeekDayStr(void);
char* getTodaysMonthStr(void);
int hexToInt(char val);
bool is_number(const std::string& s);
int isTimerDayOfWeekValidToday(byte timerDaysOfWeek);
bool isTimerValidForWeekday(byte timerDaysOfWeek, uint16_t weekDay);
bool isThereEnoughRoomInModeParamList(int textSize);
int isValidMode(String newMode);
void makeAuxSwitchList(void);
void makeAuxIntegerList(void);
void makeDeviceInfo(void);
void makeModeList(void);
uint32_t rgb2Int(uint8_t r, uint8_t g, uint8_t b);
void resetShuffleMode(void);
void resetVariables(int modeIndex);
int runMode(int modeID);
int showPixels(void);
int setNewMode(int newMode);
void setRandomMode(void);
//void setDailyTimerTriggerOrder(void);
void setSpeed(int index);
void setNextTimerTrigger(void);
//void sortOnTimerOrder (uint8_t arrayToSort[], uint8_t arraySize);
//void sortOffTimerOrder(uint8_t arrayToSort[], uint8_t arraySize);
int textInput2Int(void);
int updateAuxInteger(int id);
int updateAuxSwitches(int id);
void storePixelPin(int pin);

/* ======================= Cloud Functions =============================== */
int FnRouter(String command);
int SetMode(String command);
int SetText(String command);
int SetTimer(String command);

/* ======================= Timer Defines =============================== */
void advanceShuffle();
void advanceHue();
void timerCallback();
void timerEventHandler();

Timer timerTimer(DEFAULT_TIMER_TIME, timerCallback);
Timer shuffleTimer(SHUFFLE_ADVANCE_TIME, advanceShuffle);	
Timer hueAdvanceTimer(20, advanceHue);

 
void myHandler(const char *event, const char *data) {
    sprintf(debug,"%s Event, door is %s", event, data); 
}


void setup() {
    //Declare Cloud Functions - Up to 15
    Particle.function("SetMode",       SetMode);            // Completely Necessary, Do Not Delete
    Particle.function("SetText",       SetText);            // Completely Necessary, Do Not Delete
	Particle.function("SetTimer",      SetTimer);           // Completely Necessary, Do Not Delete
    Particle.function("Function",      FnRouter);           // Completely Necessary, Do Not Delete

    //Delcare Cloud Variables - Up to 20
    Particle.variable("debug",         debug);              // Completely Necessary, Do Not Delete
    Particle.variable("debug2",        debug2);              // Optional
    Particle.variable("speed",         speedIndex);         // Completely Necessary, Do Not Delete
    Particle.variable("brightness",    brightness);         // Completely Necessary, Do Not Delete
    Particle.variable("modeList",      modeNameList);       // Completely Necessary, Do Not Delete
	Particle.variable("modeParmList",  modeParamList);      // Completely Necessary, Do Not Delete
	Particle.variable("mode",          currentModeName);    // Completely Necessary, Do Not Delete
	Particle.variable("auxSwtchList",  auxSwitchList);      // Completely Necessary, Do Not Delete
	Particle.variable("auxIntList",    auxIntegerList);     // Completely Necessary, Do Not Delete
	Particle.variable("timerList",     timerList);          // Completely Necessary, Do Not Delete
	Particle.variable("deviceInfo",    deviceInfo);         // Completely Necessary, Do Not Delete
	Particle.variable("temp",          temp);				// Optional
    Particle.variable("wifi",          wifi);               // Optional
    Particle.variable("hour",          hour);               // Optional   
    Particle.variable("minute",        minute);             // Optional 

    Particle.subscribe("DOOR_STATUS", myHandler);
    
	pinMode(TEMP_SENSOR_PIN,INPUT);
    //pinMode(LIGHT_SENSOR_PIN,INPUT);
    
	//Tell FastLED where all the LEDs are
	FastLED.addLeds<PIXEL_TYPE, PIXEL_PIN> (leds, 0, NUM_LEDS);
    
	//Serial.begin(9600);
	
	//Initialize
	colorWheel = colorWheel2 = 0;
	isBootingUp = TRUE;
    stop = autoShutOff = shuffleMode = runChristmasModes = FALSE;
	resetFlag = rememberLastMode = timerTriggered = FALSE;
	lastSync = lastCommandReceived = previousMillis = millis();	//Take a time stamp
	run = TRUE;
	
//DEBUG STUFF
    lastMinTillNextTimer=0;
    timeStamp = System.millis(); //debug
    hour = Time.hour();	//used to check for correct time zone
    minute = Time.minute();
    wifi = WiFi.RSSI();    //Sometime I want to see how good the wifi signal is
	
    delay( 1000 ); // power-up safety delay
    
    //Clear the cloud list variables
	memset(&modeNameList[0], 0, sizeof(modeNameList));
	memset(&modeParamList[0], 0, sizeof(modeParamList));
	readPersistedEEPROM();
	makeModeList();	     //Assemble Spark Cloud available modes variable
	makeAuxIntegerList();
	makeTimerList();
    makeDeviceInfo();
  
	//if(GET_TEMP_ENABLED)
	//	getTemperature();
   
	//populate the shuffle order array
	for(int i=0;i<(int)(sizeof modeStruct / sizeof modeStruct[0]);i++)
		modeShuffleOrder[i] = i;
}


void loop() {

    if(timerTriggered) {
        timerEventHandler();
        timerTriggered = FALSE;
    }
    
    if(run || shuffleMode) {
		stop = FALSE;
		runMode(currentModeID);
    }
    
    if(resetFlag) {
        resetFlag = FALSE;
        delay(200); //Need this here otherwise the Cloud Function returned response is null
        System.reset();
    }
	
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis > 1000) {
        previousMillis = currentMillis;
        hour = Time.hour();
        minute = Time.minute();
        wifi = WiFi.RSSI(); 
        
        if(GET_TEMP_ENABLED) {
            getTemperature();
        }
		
		if(currentMillis - lastTimerCheckInterval > LAST_TIMER_INTERVAL) {
		    makeDeviceInfo();  	//Keep up to date with my Device Info
		}
		
		//Put other timing stuff in here to speed up main loop
        if (currentMillis - lastSync > oneDayInterval) {
            // Request time time synchronization from the Spark Cloud
			Particle.syncTime();
            lastSync = currentMillis;
        }
    }
}


void makeDeviceInfo(void) {
    char cBuff[60];
    
    IPAddress myIp = WiFi.localIP();
    sprintf(deviceInfo,"Local IP Address:\"%d.%d.%d.%d\",",myIp[0], myIp[1], myIp[2], myIp[3]);

    sprintf(cBuff,"SSID:\"%s\",",WiFi.SSID());
    strcat(deviceInfo,cBuff);

    sprintf(cBuff,"WiFi Strength:\"%i\",",wifi);
    strcat(deviceInfo,cBuff);

    sprintf(cBuff,"Firmware ID:\"%s\",",BUILD_FILE_NAME);
    strcat(deviceInfo,cBuff);
    
    sprintf(cBuff,"Firmware Rev:\"%s\",",BUILD_REVISION);
    strcat(deviceInfo,cBuff);
        
    sprintf(cBuff,"Particle Build Version:\"%s\",",System.version().c_str());
    strcat(deviceInfo,cBuff);
    
    sprintf(cBuff,"Free Memory (bytes):\"%i\",", (int)System.freeMemory());
    strcat(deviceInfo,cBuff);
    
    sprintf(cBuff,"Current Time On Device:\"%i:%i:%i %s %s %i %i\",",Time.hour(),Time.minute(),Time.second(),getTodaysWeekDayStr(),getTodaysMonthStr(),Time.day(),Time.year());
    //sprintf(cBuff,"Current Time On Device:\"%s\",",Time.timeStr().c_str());
    strcat(deviceInfo,cBuff);
}

char* getTodaysWeekDayStr(void) {
    return getWeekDayStr(Time.weekday());
}
char* getWeekDayStr(int weekDay) {
   switch(weekDay) {
        case 1: return "Sun";
        case 2: return "Mon";
        case 3: return "Tue";
        case 4: return "Wed";
        case 5: return "Thu";
        case 6: return "Fri";
        case 7: return "Sat";
    }
    return "invalid day";
}

char* getTodaysMonthStr(void) {
	return getMonthStr(Time.month());
}
char* getMonthStr(int month) {
    switch(month) {
        case 1: return "Jan";
        case 2: return "Feb";
        case 3: return "Mar";
        case 4: return "Apr";
        case 5: return "May";
        case 6: return "Jun";
        case 7: return "Jul";
        case 8: return "Aug";
        case 9: return "Sep";
        case 10: return "Oct";
        case 11: return "Nov";
        case 12: return "Dec";
    }
	return "invalid month";
}

void makeModeList(void) {
    for(int i=0;i< (int)sizeof modeStruct / (int)sizeof modeStruct[0];i++) {
        char cNameBuff[20];
		char cParamBuff[60];
		if(strlen(modeNameList)+strlen(modeStruct[i].modeName)+1 <= MAX_PUBLISHED_STRING_SIZE) {
            sprintf(cNameBuff,"%s;",modeStruct[i].modeName );
		    strcat(modeNameList,cNameBuff);
		}
		else {
		    sprintf(debug,"Error: modeNameList has reached max size limit");
		}
		
		if(modeStruct[i].numOfColors==0 && modeStruct[i].numOfSwitches==0 && modeStruct[i].textInput == FALSE) {
		    if(isThereEnoughRoomInModeParamList(2)) {
			    strcat(modeParamList,"N;");
		    } else { return; }
		}
		else {
			if(modeStruct[i].numOfColors > 0) {
				if(modeStruct[i].numOfColors > MAX_NUM_COLORS) {
					modeStruct[i].numOfColors = MAX_NUM_COLORS;
				}
				if(isThereEnoughRoomInModeParamList(4)) {
    				sprintf(cParamBuff,"C:%i",modeStruct[i].numOfColors);
    				strcat(modeParamList,cParamBuff);
    				if(modeStruct[i].numOfSwitches == 0 && modeStruct[i].textInput == FALSE) {
    					strcat(modeParamList,";");
    				}
    				else{
    					strcat(modeParamList,",");
    				}
				} else { return; }
			}
			if(modeStruct[i].numOfSwitches > 0) {
			    int switchTitleStructIdx = getSwitchTitleStructIndex(modeStruct[i].modeId);
			    //if(switchTitleStructIdx == -1)
			    //       break;
			    if(switchTitleStructIdx != -1) {
    				if(modeStruct[i].numOfSwitches > MAX_NUM_SWITCHES) {
    					modeStruct[i].numOfSwitches = MAX_NUM_SWITCHES;
    				}
    				if(modeStruct[i].numOfSwitches >= 1) {
    				    sprintf(cParamBuff,"S:%i,\"%s\"",modeStruct[i].numOfSwitches,switchTitleStruct[switchTitleStructIdx].switch1Title);
    				    //consider this instead: strncat(modeParamList,cParamBuff,MAX_PUBLISHED_STRING_SIZE-strlen(modeParamList)-1);
    				    if(isThereEnoughRoomInModeParamList(strlen(cParamBuff)+1)) {
    					    strcat(modeParamList,cParamBuff);
    				    } else { return; }    
    				}
    				if(modeStruct[i].numOfSwitches >= 2) {
    					sprintf(cParamBuff,"\"%s\"",switchTitleStruct[switchTitleStructIdx].switch2Title);
    					if(isThereEnoughRoomInModeParamList(strlen(cParamBuff)+1)) {
    					    strcat(modeParamList,cParamBuff);
    					} else { return; }    
    				}
    				if(modeStruct[i].numOfSwitches >= 3) {
    					sprintf(cParamBuff,"\"%s\"",switchTitleStruct[switchTitleStructIdx].switch3Title);
    					if(isThereEnoughRoomInModeParamList(strlen(cParamBuff)+1)) {
    					    strcat(modeParamList,cParamBuff);
    					} else { return; }    
    				}
    				if(modeStruct[i].numOfSwitches >= 4) {
    					sprintf(cParamBuff,"\"%s\"",switchTitleStruct[switchTitleStructIdx].switch4Title);
    					if(isThereEnoughRoomInModeParamList(strlen(cParamBuff)+1)) {
    					    strcat(modeParamList,cParamBuff);
    					} else { return; }    
    				}
    			    if(modeStruct[i].textInput == FALSE) {
    					strcat(modeParamList,";");
    				}
    				else{
    					strcat(modeParamList,",");
    				}
			    } else {
			        sprintf(cParamBuff,"S:E;");
    			    if(isThereEnoughRoomInModeParamList(strlen(cParamBuff)+1)) {
    				    strcat(modeParamList,cParamBuff);
    				} else { return; }    
			    }
			}
			if(modeStruct[i].textInput == TRUE) {
			    if(isThereEnoughRoomInModeParamList(3)) {
				    strcat(modeParamList,"T:;");
			    } else { return; }    
			}
		} 
    }
}

bool isThereEnoughRoomInModeParamList(int textSize) {
    if(strlen(modeParamList) + textSize + 1 <= MAX_PUBLISHED_STRING_SIZE) {
        return TRUE;
    }
    int idx = strlen(modeParamList)-1;
	while(modeParamList[idx] != ',' && modeParamList[idx] != ';') {
		idx--;
	}
	modeParamList[idx] = ';';

    sprintf(debug,"Error: modeParamList has reached max size limit");
    
    return FALSE;
}

int getSwitchTitleStructIndex(int modeId) {
    uint16_t i;
    for(i=0;i< (int)sizeof switchTitleStruct / sizeof switchTitleStruct[0];i++) {
        if(switchTitleStruct[i].modeId == modeId)
            return i;
    }
    sprintf(debug,"Error: Missing Switch Titles for mode %s", modeStruct[getModeIndexFromID(modeId)].modeName);
    return -1;
}

// Uses the auxSwitchStruct to assemble the cloud attainable auxSwtchList variable
// Switch param order: "id,title,onName,offName,switchState;"
void makeAuxSwitchList(void) {
	memset(&auxSwitchList[0], 0, sizeof(auxSwitchList));
    for(uint16_t i=0;i< (int)sizeof auxSwitchStruct / sizeof auxSwitchStruct[0];i++) {
        char cNameBuff[62];
		if(strlen(auxSwitchList)+strlen(auxSwitchStruct[i].auxSwitchTitle)+strlen(auxSwitchStruct[i].auxSwitchOnName)+strlen(auxSwitchStruct[i].auxSwitchOffName)+9 <= MAX_PUBLISHED_STRING_SIZE) {
            sprintf(cNameBuff,"%i,%s,%s,%s,%i;",auxSwitchStruct[i].auxSwitchId,
                                                auxSwitchStruct[i].auxSwitchTitle,
                                                auxSwitchStruct[i].auxSwitchOnName,
                                                auxSwitchStruct[i].auxSwitchOffName,
                                                auxSwitchStruct[i].auxSwitchState ? 1 : 0 );
		    strcat(auxSwitchList,cNameBuff);
		}
		else {
		    sprintf(debug,"Error: auxSwitchList has reached max size limit");
		}
		
		//Update local Aux Switch variables
		if(-1 == updateAuxSwitches(auxSwitchStruct[i].auxSwitchId))
		    sprintf(debug,"Error: auxSwitch failed to update local variable");
    }
}


/**Update local Aux Switch variables
 *  @id the Aux Switch ID to update
 *  @return current state of the switch (0 or 1)
 *  @return -1 if Switch ID was not found
 */
int updateAuxSwitches(int id) {
    switch(id) {
        case ASO:
            return autoShutOff = auxSwitchStruct[getAuxSwitchIndexFromID(id)].auxSwitchState;
        case LIGHTSENSOR:
            return brightnessControl = auxSwitchStruct[getAuxSwitchIndexFromID(id)].auxSwitchState;
		 case RLM:
            return rememberLastMode = auxSwitchStruct[getAuxSwitchIndexFromID(id)].auxSwitchState;
		case SHFL:
			shuffleMode = auxSwitchStruct[getAuxSwitchIndexFromID(id)].auxSwitchState;
			run = TRUE;
			if(shuffleMode) { stopShuffle = TRUE; resetShuffleMode(); }
			else if( shuffleTimer.isActive()) { shuffleTimer.stop(); }
			return shuffleMode;
    }
    
    return -1;
}

// Uses the auxSwitchStruct to assemble the cloud attainable auxSwtchList variable
// Switch param order: "id,title,onName,offName,switchState;"
void makeAuxIntegerList(void) {
	memset(&auxIntegerList[0], 0, sizeof(auxIntegerList));
    for(uint16_t i=0;i< (int)sizeof auxIntegerStruct / sizeof auxIntegerStruct[0];i++) {
        char cNameBuff[62];
		if(strlen(auxIntegerList)+strlen(auxIntegerStruct[i].auxIntegerTitle)+9 <= MAX_PUBLISHED_STRING_SIZE) {
            sprintf(cNameBuff,"%i,%s,%i;",auxIntegerStruct[i].auxIntegerId,
                                          auxIntegerStruct[i].auxIntegerTitle,
                                          auxIntegerStruct[i].auxIntegerValue);
		    strcat(auxIntegerList,cNameBuff);
		}
		else {
		    sprintf(debug,"Error: auxIntegerList has reached max size limit");
		}
		
		//Update local Aux Switch variables
		if(-1 == updateAuxInteger(auxIntegerStruct[i].auxIntegerId))
		    sprintf(debug,"Error: auxInteger failed to update local variable");
    }
    //return auxIntegerStruct[i].auxIntegerId;
}

int updateAuxInteger(int id) {
    int updatedInt = auxIntegerStruct[getAuxIntegerIndexFromID(id)].auxIntegerValue;
    switch(id) {
		case INT_SHFL_ADVANCE_TIME:
			shuffleTimer.changePeriod(updatedInt*1000);    //put into milliseconds
			break;
	    case INT_LED_PIN:
	        pixelPin = updatedInt;
	        if(!isBootingUp)
	            stop = resetFlag = TRUE;   // Force a restart to re-init pixel pin
	        break;
	}
    return id;
}

// Uses the timerParams to assemble the cloud attainable timerList variable
// Switch param order: "id,title,onName,offName,switchState;"
void makeTimerList(void) {
	memset(&timerList[0], 0, sizeof(timerList));
    for(uint16_t i=0;i< NUM_TIMERS;i++) {
        char cNameBuff[100];
		//if(strlen(timerList)+strlen(auxIntegerStruct[i].auxIntegerTitle)+9 <= MAX_PUBLISHED_STRING_SIZE) {
            sprintf(cNameBuff,"%X,%s,%X,%X,%X,%X,%s;",timerStruct[i].timerId==0?i:timerStruct[i].timerId,
                                                   timerStruct[i].timerName,
											       timerStruct[i].turnOnTime,
												   timerStruct[i].turnOffTime,
												   timerStruct[i].daysOfWeek,
												   timerStruct[i].brightness,
												   timerStruct[i].modeInfo);
		    if(strlen(timerList)+strlen(cNameBuff)+1 <= MAX_PUBLISHED_STRING_SIZE) {
		        strcat(timerList,cNameBuff);
		    }
    }
    setNextTimerTrigger();
}

/* 
 * Callback for Timer Software Timer
 */
void timerCallback() {
	if(Time.weekday() == nextTimer.triggerDay && (nextTimer.triggerTime-getTimeValFromNowTime()) <= 0 ) {
        timerTriggered = TRUE;
        stop = TRUE;
         sprintf(debug,"TimerCallbackTime=%i:%i, currentMode=%s",Time.hour(),Time.minute(), currentModeName);
    }
     sprintf(debug2,"Day=%i, Tday=%i, NowTime=%i:%i, NextTimer=%f, MinTillNext=%i",Time.weekday(), nextTimer.triggerDay, Time.hour(),Time.minute(), nextTimer.triggerTime/60, nextTimer.triggerTime - getTimeValFromNowTime());
}

/* 
 *  Event handler for the next timerTimer Software timer (trigger).
 *  1. Checks Trigger state: Off, On, or Refresh and drives LEDs accordinly.
 *  2. Determines if this timer is a One Shot. If it is, then we need to deselect the day
 *     from the days of the week. We allow all of the days of the week to be triggered once for a
 *     One Shot timer. So we need to know which days are left to be triggered. 
 *  3. Calls setNextTimerTrigger to update the timerTimer Timer with the 
 *     number of millis till the next timer trigger.
 */
void timerEventHandler() {
	char cBuff[75];
	bool setMode = true;
	
	//Get the Timer Action
    if(nextTimer.triggerState == OFF_TRIGGER) {
		sprintf(cBuff,"M:%s,",getModeNameFromID(OFF).c_str());
    } else if(nextTimer.triggerState == ON_TRIGGER) {
        brightness = timerStruct[nextTimer.timerIdx].brightness;
		sprintf(cBuff,"M:%s,",timerStruct[nextTimer.timerIdx].modeInfo);
    } else if (nextTimer.triggerState == REFRESH_TRIGGER) {
        // Do Nothing other than the normal call to setNextTimerTrigger to update/refresh the trigger
        setMode = false;
    } 
    
	//Check to see if it's a one shot timer
    if(timerStruct[nextTimer.timerIdx].daysOfWeek&1 == 1) {
        //Check to see if we have another trigger today for this timer
        if(!doesTimerHaveAnotherTriggerToday(nextTimer.timerIdx)) {
            //Uncheck today from the daysOfWeek list
            timerStruct[nextTimer.timerIdx].daysOfWeek ^= (1 << Time.weekday());
            //Check if there are any other days left to be triggered
            if(timerStruct[nextTimer.timerIdx].daysOfWeek <= 1 ) {
                //There are no more days left, so lets deactivate the timer
                timerStruct[nextTimer.timerIdx].timerId &= 7;
            }
            //Update the timer list to reflect the changes
            makeTimerList();
        }
    }
    
    //Start the next timer
    setNextTimerTrigger();
    if(setMode)
        SetMode(cBuff);
    timeStamp = System.millis(); //debug
}

/**
 * Sets up the next timer and triggers the timerTimer Software Timer for it
 * First we look at each day and see what timers are valid for the day 
 * The day starts with today, if no timers are valid today, then we look at the next day, then the next day, etc.
 * We are only setting up one timer at any given time whether it's an on or off tigger. 
 */
void setNextTimerTrigger(void) {
    bool foundValidTimer = FALSE;
    uint16_t weekDayIdx, timerNum;
    time_t thisActiveTimerInSeconds=0;
	time_t nextActiveTimerInSeconds = 0xFFFFF;  //a number larger than 1 week worth of seconds

timerTimer.stop();

    // Lets look for the next valid timer
    // iterate through the days of the week, starting with today
    // weekDayIdx==0 is Today, weekDayIdx==1 is Tomorrow, etc
    for(weekDayIdx=0; weekDayIdx<=DAYS_PER_WEEK; weekDayIdx++) {
        // iterate through each of the timers
        for(timerNum=0; timerNum<NUM_TIMERS; timerNum++) {
            //Check to see if timer is enabled and valid for the given day
            int weekDay2Check = Time.weekday() + weekDayIdx;
            if(weekDay2Check > DAYS_PER_WEEK) { 
                //This handles wrapping around into next week
                weekDay2Check -= DAYS_PER_WEEK; 
            }
            // Is the timer activated = bit 4 for timerId is 1 (i.e. for timer 0, timerID == 1000b)
            // And has the day in question been selected for this timer 
            if( ((timerStruct[timerNum].timerId&8) == 8) && isTimerValidForWeekday(timerStruct[timerNum].daysOfWeek, weekDay2Check) ) {
                //make sure it's a valid ON timer
                if(timerStruct[timerNum].turnOnTime != NO_TRIGGER) {
    				//make sure we haven't passed the turn on time yet (i.e. if the timer was set to 8:00am but it's currently 10:00am)
    				thisActiveTimerInSeconds = getSecondsFromTimerVal(timerStruct[timerNum].turnOnTime, weekDayIdx);
    				if(thisActiveTimerInSeconds>Time.local() && (thisActiveTimerInSeconds-Time.local())<nextActiveTimerInSeconds) {
        			    nextActiveTimerInSeconds = thisActiveTimerInSeconds - Time.local();
        			    nextTimer.timerIdx = timerNum;
        			    nextTimer.triggerState = ON_TRIGGER;
        			    nextTimer.triggerDay = weekDay2Check;
        			    nextTimer.triggerTime = timerStruct[timerNum].turnOnTime;

        			    foundValidTimer = TRUE;
        			} 
                }
                //make sure it's a valid OFF timer
                if(timerStruct[timerNum].turnOffTime != NO_TRIGGER) {
        			//Off time could occur before the On timer (weird right?) so lets check that here too
        			thisActiveTimerInSeconds = getSecondsFromTimerVal(timerStruct[timerNum].turnOffTime, weekDayIdx);
        			if(thisActiveTimerInSeconds>Time.local() && (thisActiveTimerInSeconds-Time.local())<nextActiveTimerInSeconds) {
        			    nextActiveTimerInSeconds = thisActiveTimerInSeconds - Time.local();
        			    nextTimer.timerIdx = timerNum;
        			    nextTimer.triggerState = OFF_TRIGGER;
        			    nextTimer.triggerDay = weekDay2Check;
        			    nextTimer.triggerTime = timerStruct[timerNum].turnOffTime;

        			    foundValidTimer = TRUE;
        			}
                }
            }
		}
		if(foundValidTimer) {
		    timerTimer.start();
		    //Bail IF we found a valid timer
			break;
		}
    }
}

/**
 * Used for One Shot Timers to see if there is another trigger for today
 * i.e. if the current trigger is an On Trigger, then look to see if this timer has an Off Trigger
 *  Likewise if this is an Off Trigger, then look to see if this timer has an On Trigger
 * Must be called before next call to setNextTimerTrigger(void)
 */
bool doesTimerHaveAnotherTriggerToday(uint8_t timerIdx) {
    //At this point, nextTimer should be the current Timer
    if(nextTimer.triggerState == OFF_TRIGGER) {
        if(timerStruct[timerIdx].turnOnTime != NO_TRIGGER){
            if(getSecondsFromTimerVal(timerStruct[timerIdx].turnOnTime, 0) > Time.local())
                return TRUE;
        }
    }
    if(nextTimer.triggerState == ON_TRIGGER) {
        if(timerStruct[timerIdx].turnOffTime != NO_TRIGGER){
            if(getSecondsFromTimerVal(timerStruct[timerIdx].turnOffTime, 0) > Time.local())
                return TRUE;
        }
    }
    return FALSE;
}

/*
 * Determines the number of seconds (not millis) given the hour, minute and weekday in Unix time
 ******* @param timerVal:     upper byte is the hour, lower byte is the minutes********
 * @param timerVal:     Number of minutes from previous midnight - time zero for the day
 * @param numDaysFromNow:   The number of days past today, i.e. val of 1 is tomorrow
 */
time_t getSecondsFromTimerVal(uint16_t timerVal, uint16_t numDaysFromNow) {
    time_t timeBase = previousMidnight(Time.local()) + (time_t)numDaysFromNow * SECS_PER_DAY;
	return (time_t)(timeBase + (timerVal * SECS_PER_MIN)) ;
}


time_t getHoursFromTimerVal(uint16_t timeVal) {
    return (time_t)timeVal/60;    
}

time_t getMinutesFromTimerVal(uint16_t timeVal) {
    return (time_t)timeVal - getHoursFromTimerVal(timeVal)*60;    
}

uint16_t getTimeValFromNowTime(void) {
    return (Time.hour() * MIN_PER_HOUR) + Time.minute();
}


/**
 * Checks to see if the given weekday value (1-7) is equal to the packed timerDaysOfWeek value
 * @param timerDaysofWeek   The days of the week designated for the timer
 * @param weekDay           The day of the week that we are checking timerDaysofWeek against
 */
bool isTimerValidForWeekday(byte timerDaysOfWeek, uint16_t weekDay) {
    return (timerDaysOfWeek >> weekDay) & 1;
}

int isTimerDayOfWeekValidToday(byte timerDaysOfWeek) {
    return (timerDaysOfWeek >> Time.weekday()) & 1;
}


void readPersistedEEPROM(void) {
    //Check EEPROM for saved data
    int ebytes = EEPROM.length();
	uint16_t readChecksum = 0;
	EEPROM.get(SUDO_CHECKSUM_ADDR, readChecksum);
    if(readChecksum != SUDO_CHECKSUM) {
        initEEPROM();
	    return;
	} 
	
	// Read Colors
	EEPROM.get(COLOR1_START_ADDR, colors.C1);
	EEPROM.get(COLOR2_START_ADDR, colors.C2);
	EEPROM.get(COLOR3_START_ADDR, colors.C3);
	EEPROM.get(COLOR4_START_ADDR, colors.C4);
	EEPROM.get(COLOR5_START_ADDR, colors.C5);
	EEPROM.get(COLOR6_START_ADDR, colors.C6);
	
	// Read Switches
	EEPROM.get(SWITCH1_START_ADDR, switches.SW1);
	EEPROM.get(SWITCH2_START_ADDR, switches.SW2);
	EEPROM.get(SWITCH3_START_ADDR, switches.SW3);
	EEPROM.get(SWITCH4_START_ADDR, switches.SW4);
	
	// Initialize speed variable
	EEPROM.get(SPEED_START_ADDR, speedIndex);
	speed = speedPresets[speedIndex];
	
	// Initialize brightness variable
	EEPROM.get(BRIGHT_START_ADDR, brightness);
	
	// Initialize Aux Switches
	uint32_t auxSwBits = 0;
	EEPROM.get(AUXSW_START_ADDR, auxSwBits);
	unPackAuxSwitches((uint32_t)auxSwBits);
	makeAuxSwitchList();
	
	// Initialize currentModeID variable
	if(rememberLastMode) {
		EEPROM.get(LASTMODE_START_ADDR, currentModeID);
		setNewMode(getModeIndexFromID(currentModeID));
	} else {
		//transitionAll(defaultColor,POLAR);
		fadeTowardColorAll( 0, NUM_LEDS, defaultColor, 10);
		setNewMode(getModeIndexFromID(NORMAL));
	}
	
	// Initialize Aux Integers
	for(int i=0; i< (int)(sizeof(auxIntegerStruct) / sizeof(auxIntegerStruct[0])); i++) {
		int thisIntVal = 0;
		EEPROM.get(AUXINT_START_ADDR + i*sizeof(int), thisIntVal);
		auxIntegerStruct[i].auxIntegerValue = thisIntVal;
	}
	
	// Initialize Timers
    for(int i=0; i < (int)(sizeof(timerStruct) / sizeof(timerStruct[0])); i++) {
		EEPROM.get(TIMER_START_ADDR + i*sizeof(timerParams), timerStruct[i]);
	}
	
	// Initialize Time Zone
	EEPROM.get(TIMER_ZONE_ADDR, timeZone);
	if(timeZone > 14.0) {timeZone = 14.0;}
	else if(timeZone < -12.0) {timeZone = -12.0;}
	Time.zone(timeZone);
	
}

/* Check EEPROM area and initialize globals (if values were previoulsy set);
 * otherwise, initializes the respective EEPROM storage area with defaults.
 * Variables covered: speed, brightness, currentModeID, colors.C1/2/3/4, 
 * switch1/2/3/4, drawingBuffer[]
 */
inline void initEEPROM(void) {

	EEPROM.put(SUDO_CHECKSUM_ADDR, SUDO_CHECKSUM);
	
	// Initialize colors
	EEPROM.put(COLOR1_START_ADDR, colors.C1);
	EEPROM.put(COLOR2_START_ADDR, colors.C2);
	EEPROM.put(COLOR3_START_ADDR, colors.C3);
	EEPROM.put(COLOR4_START_ADDR, colors.C4);
	EEPROM.put(COLOR5_START_ADDR, colors.C5);
	EEPROM.put(COLOR6_START_ADDR, colors.C6);
	
	// Initialize switches
	EEPROM.put(SWITCH1_START_ADDR, switches.SW1);
	EEPROM.put(SWITCH2_START_ADDR, switches.SW2);
	EEPROM.put(SWITCH3_START_ADDR, switches.SW3);
	EEPROM.put(SWITCH4_START_ADDR, switches.SW4);

	// Initialize speed variable
	speedIndex = DEFAULT_SPEED; 
	EEPROM.put(SPEED_START_ADDR, speedIndex);
	speed = speedPresets[speedIndex];
	
	// Initialize brightness variable
	brightness = DEFAULT_BRIGHTNESS;
	EEPROM.put(BRIGHT_START_ADDR, brightness);
	lastBrightness = brightness;	

	// Initialize Aux Switches
	storeAuxSwitches();
	makeAuxSwitchList();
	
	// Initialize currentModeID variable
	currentModeID = getModeIndexFromID(NORMAL);
	EEPROM.put(LASTMODE_START_ADDR, currentModeID);
	setNewMode(getModeIndexFromID(currentModeID));
	lastModeID = currentModeID;		

	// Initialize Aux Integers
    storeAuxIntegers();
    
    // Initialize Timers
    storeTimers();
    
    // Initialize Time Zone
	timeZone = TIME_ZONE_OFFSET;
	EEPROM.put(TIMER_ZONE_ADDR, timeZone);
	Time.zone(timeZone);
	
}

inline void clearEEPROM(void) {
    for(int i = 0; i < MAX_EEPROM_SIZE; i++)
        EEPROM.write(i, 0xFF);
}

void storeAuxSwitches(void) {
    uint32_t auxSwBits = packAuxSwitches();
	EEPROM.put(AUXSW_START_ADDR, auxSwBits);
}

void storeAuxIntegers(void) {
	char tempchar[50];
    for(int i=0; i < (int)(sizeof(auxIntegerStruct) / sizeof(auxIntegerStruct[0])); i++) {
		EEPROM.put(AUXINT_START_ADDR + i*sizeof(int), auxIntegerStruct[i].auxIntegerValue);
	}
}

void storeTimers(void) {
    for(int i=0; i < (int)(sizeof(timerStruct) / sizeof(timerStruct[0])); i++) {
		EEPROM.put(TIMER_START_ADDR + i*sizeof(timerParams), timerStruct[i]);
	}
}

void storePixelPin(int pin) {
    EEPROM.put(PIXEL_PIN_ADDR, pixelPin);
}

uint32_t packAuxSwitches(void) {
	uint32_t tempInt = 0;
	for(uint32_t i=0;i<(int)(sizeof auxSwitchStruct / sizeof auxSwitchStruct[0]);i++) {
		uint8_t tempBit = auxSwitchStruct[i].auxSwitchState & 1;
		tempInt |= (tempBit << i);
	}
	return tempInt;
}

void unPackAuxSwitches(uint32_t packedInt) {
	for(uint32_t i=0;i<(int)(sizeof auxSwitchStruct / sizeof auxSwitchStruct[0]);i++) {
		auxSwitchStruct[i].auxSwitchState = (packedInt >> i) & 1;
	}
}


int runMode(int modeID) {
    
	if(shuffleMode) { 
		if(stopShuffle) {
			stopShuffle = FALSE;
			setRandomMode(); 
			shuffleTimer.start(); 
		}
	}
	
    switch (modeID) {
        case OFF:
    	    fadeTowardColorAll( 0, NUM_LEDS, CRGB::Black, 10);
    	    run = FALSE;
    		break;
		case BEATWAVE:
			beatWave();
			break;
		case BLENDWAVE:
			blendWave();
			break;
        case BPM:
			bpm();
			break;
		case CHEERLIGHTS:
		    cheerlights();
			break;
		case COLORALL:
    	    fadeTowardColorAll( 0, NUM_LEDS, colors.C1, 10);
    	    break;
		case COLORCYCLE:
			colorCycle();
			break;
		case CHRISTMASLOOP:
		case WARMWHITESHIMMER:
		case TRADITIONALCOLORS:
		case GRADIENT:	
		case BRIGHTTWINKLE:
		case COLOREXPLOSION:
		case RANDOMCOLORWALK:
		case CHRISTMAS_LIGHTS:
		    driveChristmasModes();
		    break;	
		case COLORPALETTE:
		    colorPalette();
		    break;
		case PICK2PALETTE:
		    pick2Palette();
		    break;
		case COLORWAVES:
		    colorWaves();
		    break;
		case COLORTEMP:
			colorTemperature();
			break;
    	case CONFETTI:
			confetti();
			break;
		case CRASH:
			crash(CRGB(colors.C1), CRGB(colors.C2));
			break;
		 case DIGI:
		    digi(CRGB(colors.C1));
		    break;
		case FIREFLY:
		    firefly();
		    break;
		case ICYFIRE:
			icyBlueFire();
		    break;
		case FIRENICE:
			fireAndIce();
		    break;
		case PINKYFIRE:
			 pinkyFire();
		    break;
		case PICKAFIRE:
			pickAFire(CRGB(colors.C1),CRGB(colors.C2),CRGB(colors.C3));
			break;
    	case IFTTTWEATHER:
		    iftttWeather(colors.C6);
            break;
        case INDEX:
            index(CRGB(colors.C1));
            break;
        case JUGGLE:
			juggle();
			break;
		case LIGHTENING:
			lightening();
			break;
		case NOISE16_1:
			noise16_1();
			break;
		case NOISE16_2:
			noise16_2();
			break;
		case NOISE16_3:
			noise16_3();
			break;
		case PALETTECROSSFADE:
			paletteCrossFade();
			break;
		case RAINBOW:
			rainbow();
			break;
		case RIPPLE:
			ripple();
			break;
		case SINELON:
			sinelon();
			break;
		case THEATERCHASE:
    	    theaterChaseRainbow();
    	    break;
		case THREESINE:
			threeSine();
			break;
		case WIPE:
			wipe(CRGB(colors.C1));
			break;
    	case NORMAL:
			fadeTowardColorAll( 0, NUM_LEDS, CRGB(defaultColor), 10);
    	    break;
    	default:
    		break;
    }
    return 1;
}


void resetVariables(int modeIndex) {
	if(!shuffleMode && shuffleTimer.isActive()) {
		shuffleTimer.stop();
	}
	firstLap = TRUE;
	runChristmasModes = FALSE;
	
    switch (modeIndex) {
	
		case BEATWAVE:
		case COLORPALETTE:
			currentPalette = RainbowColors_p;
			currentBlending = LINEARBLEND;
			break;
			
		case CHEERLIGHTS:
        	hostname = "api.thingspeak.com";
		    path = "/channels/1417/field/2/last.txt";
		    response = "";
		    pollTime = millis() + POLLING_INTERVAL;
		    lastCol = 0;
        	client.stop();
		    connected = client.connect(hostname, 80);
			switches.SW1 = switches.SW2 = switches.SW3 = switches.SW4 = FALSE;
		    break;
			
		case CHRISTMASLOOP:
			runChristmasModes = TRUE;
		case WARMWHITESHIMMER:	
		case TRADITIONALCOLORS:	
		case GRADIENT:		
		case BRIGHTTWINKLE:	
		case COLOREXPLOSION:
		case RANDOMCOLORWALK:
		case CHRISTMAS_LIGHTS:
		    loopCount = 0;
		    currentPalette = christmasLightsPalette_p;
            currentBlending = NOBLEND;
		    break;
			
		case CRASH:
		case DIGI:
		case WIPE:
			colorWheel = random(256);
			colorWheel2= colorWheel+42;
			fadeTowardColorAll( 0, NUM_LEDS, CRGB::Black, 20);
			break; 
			
	    case IFTTTWEATHER:
	    case INDEX:
		case COLORTEMP:
	    case LIGHTENING:
	    case FIREFLY:
		case ICYFIRE:
		case FIRENICE:
		case PINKYFIRE:
		case PICKAFIRE:
	        fadeTowardColorAll( 0, NUM_LEDS, CRGB::Black, 20);
	        break;
			
		case NOISE16_1:
		case NOISE16_2:
		case NOISE16_3:
			currentPalette = PartyColors_p;
			currentBlending = LINEARBLEND;
			maxChanges = 24;
			break;
			
		case PALETTECROSSFADE:
		case THREESINE:
			currentPalette= CRGBPalette16(CRGB::Black);
			targetPalette = PartyColors_p;
			currentBlending = LINEARBLEND; 
			fadeTowardColorAll( 0, NUM_LEDS, CRGB::Black, 20);
			break;
			
		case RIPPLE:
			center = 0;
			step = -1;
			maxSteps = 16;
			fadeRate = 0.8;
			currentBg = random(256);
			nextBg = currentBg;
			break;

		case RAINBOW:
		case CONFETTI:
		case SINELON:
		case JUGGLE:
		case BPM:
			gCurrentPatternNumber = 0;
			gHue = 0;
			break;
		case THEATERCHASE:
			setSpeed(0);
			break;	
		case SHUFFLE:
			resetShuffleMode();
			fadeTowardColorAll( 0, NUM_LEDS, CRGB::Black, 20);
			break;
		default:
			break;
    }    
	run = TRUE;
}


int getTemperature(void){
	double measuredVoltage = refVoltage * analogRead(TEMP_SENSOR_PIN) / 4095.0;
    temp = (measuredVoltage - 0.5) * 100.0;          //Celcius Temp
    temp = (temp * 9.0 / 5.0) + 32.0; //Convert to Fahrenheit
    
    return 1;
}


void arrayShuffle(int arrayToShuffle[], int arraySize) {
    uint16_t i; 

    for(i=0;i<arraySize;i++) {
        int r = random(0,arraySize);  // generate a random position
        int temp = arrayToShuffle[i]; 
        arrayToShuffle[i] = arrayToShuffle[r]; 
        arrayToShuffle[r] = temp;
    }
}

void arrayShuffle(uint16_t arrayToShuffle[], uint16_t arraySize) {
    uint16_t i; 

    for(i=0;i<arraySize;i++) {
        uint16_t r = random(0,arraySize);  // generate a random position
        uint16_t temp = arrayToShuffle[i]; 
        arrayToShuffle[i] = arrayToShuffle[r]; 
        arrayToShuffle[r] = temp;
    }
}

void setRandomMode(void) {
	colors.C1 = rgb2Int(random(256), random(256), random(256));
    colors.C2 = rgb2Int(random(256), random(256), random(256));
    colors.C3 = rgb2Int(random(256), random(256), random(256));
    colors.C4 = rgb2Int(random(256), random(256), random(256));
	switches.SW1 = random(2);
	switches.SW2 = random(2);
	switches.SW3 = random(2);
	switches.SW4 = random(2);
	
	do {
       //int randomModeIdx = random(0, (int)(sizeof(modeStruct) / sizeof(modeStruct[0])));
        //    currentModeID = modeStruct[randomModeIdx].modeId;
		/** The random technique above can have repeats every few cycles,
		  * so let's replace it and use the modeShuffleOrder array - guaranteeing each 
		  * mode gets played. 
		  */
		if(shuffleIdx >= (int)(sizeof modeShuffleOrder / sizeof modeShuffleOrder[0]))	
			resetShuffleMode();
		currentModeID = modeStruct[modeShuffleOrder[shuffleIdx]].modeId; 
		shuffleIdx++;
    }while((currentModeID == lastModeID)    	|| 
           (currentModeID == NORMAL)            || 
           (currentModeID == OFF)          		||    
           (currentModeID == CHEERLIGHTS)       || 
	       (currentModeID == COLORALL)          ||
           (currentModeID == IFTTTWEATHER)      ||
           (currentModeID == INDEX)             ||
           (currentModeID == LIGHTENING)        ||
	       (currentModeID == SHUFFLE));
	
	if(currentModeID == WIPE)
	    switches.SW1 = TRUE;


	setNewMode(getModeIndexFromID(currentModeID));
}

//Shuffle the deck
void resetShuffleMode(void) {
	shuffleIdx = 0;
	arrayShuffle(modeShuffleOrder,(int)(sizeof modeShuffleOrder / sizeof modeShuffleOrder[0]));
}

void advanceShuffle() {
    stopShuffle = TRUE;
}

void advanceHue() {
    gHue = (gHue+1) % 256;
}

//Used in all modes to set the brightness, show the lights, process Spark events and delay
int showPixels(void) {
    if(brightnessControl) {
		int lightSensorValue = analogRead(LIGHT_SENSOR_PIN);
		brightness = map(lightSensorValue,0,2500,0,255); // takes the value and scales it from 0-2500 to 0-255
	}
	FastLED.setBrightness(brightness);
    FastLED.show();
    Particle.process();    //process Spark events
	return 1;
}

int colorAll(uint32_t c) {
    fill_solid(leds, NUM_LEDS, c);   
    showPixels();
    return 1;
}

void cheerlights(void) {
    int red, green, blue;
    bool headers;
    char lastChar;
    
    if((millis()-pollTime)<=POLLING_INTERVAL) {
        if(stop) {client.stop(); return;}
	    if(!Particle.connected) {
	        Particle.connect();
	        waitFor(Particle.connected, 1000);
	    }
	    else {
            showPixels();
            delay(100);
	    }
    }
    else {
        if(connected) {
            pollTime=millis();
            client.print("GET ");
            client.print(path);
            client.println(" HTTP/1.0");
            client.print("Host: ");
            client.println(hostname);
            client.println("Content-Length: 0");
            client.println();
        }
        else {
            if(stop) {client.stop(); return;}
            client.stop();
		    response = "";
		    if(!Particle.connected) {
		        Particle.connect();
		        waitFor(Particle.connected, 1000);
		    }
		    else {connected = client.connect(hostname, 80);}
        }
    
        requestTime=millis();
        while((client.available()==0)&&((millis()-requestTime)<RESPONSE_TIMEOUT)) {
            if(stop) {client.stop(); return;}
            Particle.process();    //process Spark events
        };
        
        headers=TRUE;
        lastChar='\n';
        response="";
    	while(client.available()>0) {
            if(stop) {client.stop(); return;}
    		char thisChar=client.read();
    		if(!headers)
    		    response.concat(String(thisChar));
    		else {
    			if((thisChar=='\r')&&(lastChar=='\n')) {
        			headers=FALSE;
        			client.read();  //kill that last \n
    			}
    			lastChar=thisChar;  
    		}
            //itoa(client.available(), debug, 10);
    	}

        //if there's a valid hex color string from Cheerlights, update the color
        if(response.length()==7) {
            //convert the hex values from the response.body string into byte values
    		red=hexToInt(response.charAt(1))*16+hexToInt(response.charAt(2));
    		green=hexToInt(response.charAt(3))*16+hexToInt(response.charAt(4));
    		blue=hexToInt(response.charAt(5))*16+hexToInt(response.charAt(6));
			CRGB color = CRGB(red, green, blue);
        	//actually update the color on the cube, with a cute animation
        	 if(color != lastCol) {
            	lastCol = color;
        	    int which = random(0, 3);
        	    if(stop) {client.stop(); return;}
        	    switch(which) {
        	        case 0:
        	            wipe(color);
        	            break;
        	        case 1:
        	            randomPixelFill(color);
        	            break;
        	        case 2:
					default:
						//transitionAll(color,POLAR);
						fadeTowardColorAll( 0, NUM_LEDS, CRGB::Black, 10);
        	            break;
        	    }
        	 }
            //sprintf(debug, response);
            
        }
        else {
           //sprintf(debug, "no reply from host");
          	
            if(stop) {client.stop(); return;}
            client.stop();
		    response = "";
		    if(!Particle.connected) {
		        Particle.connect();
		        waitFor(Particle.connected, 1000);
		    }
		    else {connected = client.connect(hostname, 80);}
        }
    }
    showPixels();
    run = TRUE;
}

//Theatre-style crawling lights with rai j, q, i;nbow effect
void theaterChaseRainbow(void) {
  uint16_t j, q, i;
  for (j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (q=0; q < 3; q++) {
        for (i=0; i < NUM_LEDS; i=i+3) {
          if(i+q < NUM_LEDS)
            leds[i+q] =  CHSV( (i+j) % 255, 255, 255);    //turn every third pixel on
        }
        showPixels();
        if(stop || stopShuffle) {return;}
		delay(speed);
        for (i=0; i < NUM_LEDS; i=i+3) {
          if(i+q < NUM_LEDS)
             leds[i+q] = CRGB::Black;        //turn every third pixel off
        }
    }
  }
}

/** Allow ifttt.com to trigger this mode based off of a weather recipe. 
 *  Search for Spark Pixels on ifttt.com for an example.
 *  The IFTTT input must be: M:IFTTT WEATHER,C6:xxxxxx,
 *  Don't forget that last comma
 *  Where 0000FF is the hex value for whatever color you want in r-g-b order.
 *  Selecting this mode from the app, will do nothing.
 *  After ifttt.com triggers this mode, the mode will run for 10 minutes, then revert
 *  back to the last running mode.
 *  10 minute time interval is set by the iftttWeatherInterval variable, change this to what you want.
 *  This method will drive all LEDs to 'Breathe' the selected color, just like the RGB
 *  LED on the Photon.
 *  Breathing LED code credit: http://sean.voisen.org/blog/2011/10/breathing-led-with-arduino/
**/
void iftttWeather(uint32_t c) {
	if((millis() - lastCommandReceived) < iftttWeatherInterval) {
        int oldBrightness = brightness;
        float val = (exp(sin(millis()/2000.0*PI)) - 0.36787944)*108.0;  //set "breathing" brightness level
        brightness = (int)val;
		if(firstLap) {
			firstLap = FALSE;
			if(brightness > 10) {
				//transitionAll(CRGB(c),POLAR);
				fadeTowardColorAll( 0, NUM_LEDS, CRGB(c), 10);
			}
		} 
		else
			colorAll(c);
        brightness = oldBrightness;
    }
    else {
		//transitionAll(CRGB::Black,LINEAR);
		fadeTowardColorAll( 0, NUM_LEDS, CRGB::Black, 10);
        currentModeID = lastModeID;
        setNewMode(getModeIndexFromID(currentModeID));
    }
    run = TRUE;
}


/**
 * digi() helper function
 * Randomly fills the whole strip with a selected color or a random color
 * @param c: Next Color to populate
 * @switches.SW1 = Random Color Fill: Ignore the passed color and choose random colors for each pixel
 */
int randomPixelFill(CRGB c) {
    uint16_t i; 
    uint16_t stripLength = NUM_LEDS;	//Have to cast NUM_LEDS this way to calculate headLength properly - compiler issue? Weird!
	uint16_t turnOnNum = stripLength / 100 + 1;  //Number of pixels to turn on each iteration 
    CRGB col = c;
    uint16_t pixelFillOrder[NUM_LEDS];
    
    for(i=0; i<NUM_LEDS; i++) {
        pixelFillOrder[i]=i;
    }

    arrayShuffle(pixelFillOrder, (int)sizeof pixelFillOrder / sizeof pixelFillOrder[0]);

    for(i=0; i<NUM_LEDS; switches.SW3 ? i++ : i+=turnOnNum) {
        if(stop || stopShuffle) {return 0;}
        if(switches.SW2 && (c.r!=0 && c.g!=0 && c.b!=0)) { col = CHSV( random(256), 255, 255); }
		
        if(switches.SW3) {
            //transitionOne(c,pixelFillOrder[i],POLAR);
			fadeTowardColorOne(pixelFillOrder[i], c, 10);
        }
        else {
            for( uint16_t turnOnIncrement = 0; turnOnIncrement < turnOnNum; turnOnIncrement++) {
			    //For longer strings, light up more pixels each time
			    if(i+turnOnIncrement < NUM_LEDS)
                    leds[pixelFillOrder[i+turnOnIncrement]] =  col;
            }
            showPixels();
            delay(speed);
        }
    }
    return 1;
}

/**
 * Pick a random color, then randomly fill the whole strip with that color
 * But make sure the next color has some contrast from the current color
 * @randomPixelFill(): helper function to do the dirty work
 * @switches.SW1 = Color Sweep through the wheel
 * @switches.SW2 = Random Color Fill: Used in randomPixelFill() to fill the strip with random colors
 * @switches.SW3 = Slow Transition: 
 */
void digi(CRGB c) {
	CRGB col = c;
	if(switches.SW1)	{ 
		col = CHSV( colorWheel, 255, 255);

		colorWheel += 16;
	}
    //Particle.publish("digi", NULL, 60, PRIVATE);
    if(0 == randomPixelFill(col)) { return; }	//Populate the strip
	delay(400);
    if(0 == randomPixelFill(0x0)) { return; }		//Kill off the pixels
	
    run = TRUE;
}

void ripple() {
 
    if (currentBg == nextBg) {
      nextBg = random(256);
    } else if (nextBg > currentBg) {
      currentBg++;
    } else {
      currentBg--;
    }
    for(uint16_t l = 0; l < NUM_LEDS; l++) {
      leds[l] = CHSV(currentBg, 255, 255); 
    }
 
	if (step == -1) {
      center = random(NUM_LEDS);
	  color = random(256);
      step = 0;
	}
 
	if (step == 0) {
	  leds[center] = CHSV(color, 255, 255);
      step ++;
	} else {
	  if (step < maxSteps) {
		Serial.println(pow(fadeRate,step));
        leds[wrap(center + step)] = CHSV(color, 255, pow(fadeRate, step)*255);       
		leds[wrap(center - step)] = CHSV(color, 255, pow(fadeRate, step)*255);       
		if (step > 3) {
		  leds[wrap(center + step - 3)] = CHSV(color, 255, pow(fadeRate, step - 2)*255);
		  leds[wrap(center - step + 3)] = CHSV(color, 255, pow(fadeRate, step - 2)*255);
		}
		step ++;
      } else {
		step = -1;
	  }
    }
 
	showPixels();
	delay(speed);
}
  
int wrap(int step) {
  if(step < 0) return NUM_LEDS + step;
  if(step > NUM_LEDS - 1) return step - NUM_LEDS;
  return step;
}
  
// SET ALL LEDS TO ONE COLOR (HSV)
void one_color_allHSV(int ahue, int abright) {                
  for (int i = 0 ; i < NUM_LEDS; i++ ) {
    leds[i] = CHSV(ahue, 255, abright);
  }
}


void rainbow() {
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
  if(switches.SW1)
    addGlitter(80);
  showPixels();
  delay(speed);
  //EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
}

void addGlitter( fract8 chanceOfGlitter) {
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() {
  // random colored speckles that blink in and fade smoothly
  uint16_t stripLength = NUM_LEDS;	//Have to cast NUM_LEDS this way to calculate headLength properly - compiler issue? Weird!
  uint16_t turnOnNum = stripLength / 100 + 1;  //Number of pixels to turn on each iteration 
  fadeToBlackBy( leds, NUM_LEDS, 10);
  for( uint16_t count=0; count < turnOnNum; count++) {
	//lets turn on more LEDs for longer strips
	int pos = random16(NUM_LEDS);
	leds[pos] += CHSV( gHue + random8(64), 200, 255);
  }
  showPixels();
  delay(speed);
}

void sinelon() {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
  showPixels();
  delay(speed);
}

void bpm() {
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
  showPixels();
  delay(speed);
}

void juggle() {
    EVERY_N_SECONDS(5) {
     setSpeed(random(0,(int)(sizeof speedPresets / sizeof speedPresets[0])-1));
  }
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
  showPixels();
  delay(speed);
}


void wipe(CRGB c) {
	//The longer the string of LEDs, the longer it takes to wipe through. So let's turn on more LEDs each time to speed it up
	int16_t stripLength = NUM_LEDS;	//Have to cast NUM_LEDS this way to calculate headLength properly - compiler issue? Weird!
	int16_t headLength = stripLength / 100 + 1;
	//sprintf(debug,"NumLeds= %i,wipe length = %i", stripLength, headLength);
	//If Color Sweep, pre-load the color from the colorWheel Hue
	if(switches.SW2)	{ c = CHSV( colorWheel+=16, 255, 255); }
    else if(switches.SW3) { c = CHSV( random(256), 255, 255); }
	//Forward
	for( int16_t i = 0; i < NUM_LEDS; i+=headLength) {
		for( int16_t headIncrement = 0; headIncrement < headLength; headIncrement++) {
			//For longer strings, light up more pixels each time
			if(i+headIncrement < NUM_LEDS)
			    leds[i+headIncrement] = c;
		}
		if(stop || stopShuffle) {return;}
		showPixels();
		delay(speed);
	}
	
	// Cycle or Color Sweep
	if(switches.SW1 || switches.SW2 || switches.SW3) 
	{
		delay(300);
		if(switches.SW4) {	//Fade All out at the same time - No Sweep to next state
		    int tempSpeed = speed;
		    speed = speed < 70 ? 70 : speed;
			//transitionAll(CRGB::Black, LINEAR);
			fadeTowardColorAll( 0, NUM_LEDS, CRGB::Black, 10);
			speed = tempSpeed;
		} else {		//Sweep Backwards to black 
			for( int16_t i = NUM_LEDS-1; i >= 0; i-=headLength) {
				for( int16_t headIncrement = 0; headIncrement < headLength; headIncrement++) {
				    if(i-headIncrement >= 0)
					    leds[i-headIncrement] = CRGB::Black;
				}
				if(stop || stopShuffle) {return;}
				showPixels();
				delay(speed);
			}
		
		}
		run = TRUE;
	} else {
	    run = FALSE;
	}
}


//LEDs grow from each end of the strip, when they meet in the middle, they blow up into white, then fade out to the combined color
void crash(CRGB c1, CRGB c2) {

	int16_t stripLength = NUM_LEDS;	//Have to cast NUM_LEDS this way to calculate headLength properly - compiler issue? Weird!
	int16_t headLength = stripLength / 100 + 1;
	
	//Select the colors
	if(switches.SW1 || switches.SW2) {
    	if(switches.SW1) {


    	    colorWheel += 85;
    		colorWheel2 *= 43;
    	} else if(switches.SW2) {
    	    colorWheel  = random(256);
    		colorWheel2 = random(256);
    	}
    	c1 = CHSV(colorWheel, 255, 255);
    	c2 = CHSV(colorWheel2, 255, 255);
	}
	
	//for(int i=0; i<=NUM_LEDS/2; i++) { 

	for( int16_t i=0; i<=stripLength/2; i+=headLength) { 
		//Grow the two colors from either end of the strip


		for( int16_t headIncrement = 0; headIncrement < headLength; headIncrement++) {
			if(i+headIncrement <= stripLength/2) {
				leds[i+headIncrement] = c1;
				leds[NUM_LEDS-1-i-headIncrement] = c2;

			}
		}
		if(stop || stopShuffle) {return;}
		showPixels();
		delay(speed);


	}

    //Explode the colors
   // int tempSpeed = speed;
//	speed = 5;
	//transitionAll(CRGB::White, LINEAR);
//	fadeTowardColorAll( 0, NUM_LEDS, CRGB::White, 20);
	//delay(100);
	//transitionAll(c1 + c2, POLAR);
	fadeTowardColorAll( 0, NUM_LEDS, c1 + c2, 20);
	delay(200);
//	speed = 70;
	//transitionAll(CRGB::Black,LINEAR);
	fadeTowardColorAll( 0, NUM_LEDS, CRGB::Black, 30);
//	speed = tempSpeed;
    if(stop || stopShuffle) {return;}
//	showPixels();
	delay(speed);

	run = TRUE;
}

//Cycle through colors over all LEDs
void colorCycle() { //OK //-FADE ALL LEDS THROUGH HSV RAINBOW
   run = TRUE;


   fill_solid(leds, NUM_LEDS, CHSV(colorWheel++, 255, 255));
   /*for(int idex=0; idex<NUM_LEDS; idex++ ) {
      leds[idex] = CHSV(colorWheel, 255, 255);
	  colorWheel++;
	  if(stop || stopShuffle) {return;}
   }*/
   showPixels();
   delay(speed);
}

void beatWave() {
	EVERY_N_MILLISECONDS(100) {
		uint8_t maxChanges = 24; 
		nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);   // AWESOME palette blending capability.
	}
	EVERY_N_SECONDS(5) {                                        // Change the target palette to a random one every 5 seconds.
		targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 192, random8(128,255)), CHSV(random8(), 255, random8(128,255)));
	}
	
	uint8_t wave1 = beatsin8(9, 0, 255); 
	uint8_t wave2 = beatsin8(8, 0, 255);
	uint8_t wave3 = beatsin8(7, 0, 255);
	uint8_t wave4 = beatsin8(6, 0, 255);

	for (int i=0; i<NUM_LEDS; i++) {
		leds[i] = ColorFromPalette( currentPalette, i+wave1+wave2+wave3+wave4, 255, currentBlending); 
		if(stop || stopShuffle) {return;}
	}
	
    showPixels();
    delay(speed);
}


void blendWave()	{
	blendWaveSpeed = beatsin8(6,0,255);

	clr1 = blend(CHSV(beatsin8(3,0,255),255,255), CHSV(beatsin8(4,0,255),255,255), blendWaveSpeed);
	clr2 = blend(CHSV(beatsin8(4,0,255),255,255), CHSV(beatsin8(3,0,255),255,255), blendWaveSpeed);

	loc1 = beatsin16(10,0,NUM_LEDS-1);
  
	fill_gradient_RGB(leds, 0, clr2, loc1, clr1);
	fill_gradient_RGB(leds, loc1, clr2, NUM_LEDS-1, clr1);
	
	showPixels();
	delay(speed);
}


void lightening() {
  ledstart = random16(NUM_LEDS);                               // Determine starting location of flash
  ledlen = random16(NUM_LEDS*0.6, NUM_LEDS-ledstart);                        // Determine length of flash (not to go beyond NUM_LEDS-1)
  
  for (int flashCounter = 0; flashCounter < random8(3,flashes); flashCounter++) {
    if(flashCounter == 0) dimmer = 5;                         // the brightness of the leader is scaled down by a factor of 5
    else dimmer = random8(1,3);                               // return strokes are brighter than the leader
    
    fill_solid(leds+ledstart,ledlen,CHSV(255, 0, 255/dimmer));
    FastLED.show();                       // Show a section of LED's
    delay(random8(4,10));                                     // each flash only lasts 4-10 milliseconds
    fill_solid(leds+ledstart,ledlen,CHSV(255,0,0));           // Clear the section of LED's
    FastLED.show();
    
    if (flashCounter == 0) delay (150);                       // longer delay until next flash after the leader
    
	if(stop || stopShuffle) {return;}
    delay(50+random8(100));                                   // shorter delay between strokes  
  } // for()
  
  showPixels();
  delay(random8(speed)*100);   
}


void noise16_1() {
  EVERY_N_MILLISECONDS(50) {
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);  // Blend towards the target palette
  }

  EVERY_N_SECONDS(5) {             // Change the target palette to a random one every 5 seconds.
    targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 192, random8(128,255)), CHSV(random8(), 255, random8(128,255)));
  }
  
  uint16_t scale = 1000;                                      // the "zoom factor" for the noise

  for (uint16_t i = 0; i < NUM_LEDS; i++) {

    uint16_t shift_x = beatsin8(5);                           // the x position of the noise field swings @ 17 bpm
    uint16_t shift_y = millis() / 100;                        // the y position becomes slowly incremented
    

    uint16_t real_x = (i + shift_x)*scale;                    // the x position of the noise field swings @ 17 bpm
    uint16_t real_y = (i + shift_y)*scale;                    // the y position becomes slowly incremented
    uint32_t real_z = millis() * 20;                          // the z position becomes quickly incremented
    
    uint8_t noise = inoise16(real_x, real_y, real_z) >> 8;   // get the noise data and scale it down

    uint8_t index = sin8(noise*3);                           // map LED color based on noise data
    uint8_t bri   = noise;

    leds[i] = ColorFromPalette(currentPalette, index, bri, LINEARBLEND);   // With that value, look up the 8 bit colour palette value and assign it to the current LED.
	if(stop || stopShuffle) {return;}
  }
  showPixels();
  delay(speed);
}

void noise16_2() {
  EVERY_N_MILLISECONDS(50) {
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);  // Blend towards the target palette
  }

  EVERY_N_SECONDS(5) {             // Change the target palette to a random one every 5 seconds.
    targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 192, random8(128,255)), CHSV(random8(), 255, random8(128,255)));
  }
  uint8_t scale = 1000;                                       // the "zoom factor" for the noise

  for (uint16_t i = 0; i < NUM_LEDS; i++) {

    uint16_t shift_x = millis() / 10;                         // x as a function of time
    uint16_t shift_y = 0;

    uint32_t real_x = (i + shift_x) * scale;                  // calculate the coordinates within the noise field
    uint32_t real_y = (i + shift_y) * scale;                  // based on the precalculated positions
    uint32_t real_z = 4223;
    
    uint8_t noise = inoise16(real_x, real_y, real_z) >> 8;    // get the noise data and scale it down

    uint8_t index = sin8(noise*3);                            // map led color based on noise data
    uint8_t bri   = noise;

    leds[i] = ColorFromPalette(currentPalette, index, bri, LINEARBLEND);   // With that value, look up the 8 bit colour palette value and assign it to the current LED.

  }
  showPixels();
  delay(speed);
}


void noise16_3() {
  EVERY_N_MILLISECONDS(50) {
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);  // Blend towards the target palette
  }

  EVERY_N_SECONDS(5) {             // Change the target palette to a random one every 5 seconds.
    targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 192, random8(128,255)), CHSV(random8(), 255, random8(128,255)));
  }
  
  uint8_t scale = 1000;                                       // the "zoom factor" for the noise

  for (uint16_t i = 0; i < NUM_LEDS; i++) {

    uint16_t shift_x = 4223;                                  // no movement along x and y
    uint16_t shift_y = 1234;

    uint32_t real_x = (i + shift_x) * scale;                  // calculate the coordinates within the noise field
    uint32_t real_y = (i + shift_y) * scale;                  // based on the precalculated positions
    uint32_t real_z = millis()*2;                             // increment z linear

    uint8_t noise = inoise16(real_x, real_y, real_z) >> 7;    // get the noise data and scale it down

    uint8_t index = sin8(noise*3);                            // map led color based on noise data
    uint8_t bri   = noise;

    leds[i] = ColorFromPalette(currentPalette, index, bri, LINEARBLEND);   // With that value, look up the 8 bit colour palette value and assign it to the current LED.
  }
  showPixels();
  delay(speed);
}


void index(CRGB c) {

    leds[textInput2Int()] = c;
    showPixels();
}

int textInput2Int(void) {
    int returnValue = 0;
    if(is_number(textInputString)) {
        returnValue = atoi( textInputString );


    }
    return returnValue;
}

bool is_number(const std::string& s) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

void firefly() {
	driveFire2012(HeatColors_p, HeatColors_p);
}
void fireAndIce() {
	CRGBPalette16 icyBluePal = CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White);
	driveFire2012(HeatColors_p, icyBluePal);
}
void icyBlueFire() {
	CRGBPalette16 icyBluePal = CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White);
	driveFire2012(icyBluePal, icyBluePal);
}
void pinkyFire() {
	CRGBPalette16 pinkyFirePal = CRGBPalette16( CRGB::Black, CRGB::BlueViolet, CRGB::Fuchsia, CRGB::LightPink);
	driveFire2012(pinkyFirePal, pinkyFirePal);
}

void pickAFire(CRGB c1, CRGB c2, CRGB c3) {
	CRGBPalette16 pickAPal = CRGBPalette16( CRGB::Black, c1, c2, c3);
	driveFire2012(pickAPal, pickAPal);
}

void driveFire2012(CRGBPalette16 pal1, CRGBPalette16 pal2) {
	if(Fire2012(pal1, heat1a, heat1aSize,            0, section1aEnd, CAB1_COOLING, FORWARD ) == 0) {return;}//29
	if(Fire2012(pal2, heat1b, heat1bSize, section1aEnd, section1bEnd, CAB1_COOLING, BACKWARD) == 0) {return;}
	if(Fire2012(pal2, heat2a, heat2aSize, section1bEnd, section2aEnd, SINK_COOLING, FORWARD ) == 0) {return;}//20
	if(Fire2012(pal1, heat2b, heat2bSize, section2aEnd, section2bEnd, SINK_COOLING, BACKWARD) == 0) {return;}
	if(Fire2012(pal1, heat3a, heat3aSize, section2bEnd, section3aEnd, CAB2_COOLING, FORWARD ) == 0) {return;}//22
	if(Fire2012(pal2, heat3b, heat3bSize, section3aEnd, section3bEnd, CAB2_COOLING, BACKWARD) == 0) {return;}
	if(Fire2012(pal2, heat4a, heat4aSize, section3bEnd, section4aEnd, CAB3_COOLING, FORWARD ) == 0) {return;}//35
	if(Fire2012(pal1, heat4b, heat4bSize, section4aEnd, section4bEnd, CAB3_COOLING, BACKWARD) == 0) {return;}
	
	showPixels();
    delay(speed);
}


// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
//// 
// This basic one-dimensional 'fire' simulation works roughly as follows:
// There's a underlying array of 'heat' cells, that model the temperature
// at each point along the line.  Every cycle through the simulation, 
// four steps are performed:
//  1) All cells cool down a little bit, losing heat to the air
//  2) The heat from each cell drifts 'up' and diffuses a little
//  3) Sometimes randomly new 'sparks' of heat are added at the bottom
//  4) The heat from each cell is rendered as a color into the leds array
//     The heat-to-color mapping uses a black-body radiation approximation.
//
// Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
//
// This simulation scales it self a bit depending on NUM_LEDS; it should look
// "OK" on anywhere from 20 to 100 LEDs without too much tweaking. 
//
// I recommend running this simulation at anywhere from 30-100 frames per second,
// meaning an interframe delay of about 10-35 milliseconds.
//
// Looks best on a high-density LED setup (60+ pixels/meter).
//
//
// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100 
#define COOLING  50

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 		120
#define FLICKERY_FIRE	50
#define DEFAULT_FIRE	120
#define ROARING_FIRE	200

int Fire2012(CRGBPalette16 thisP, byte heat[], int num_leds, int startPixel, int endPixel, int cooling, bool direction) {
  // Step 1.  Cool down every cell a little
    for( int i = 0; i < num_leds; i++) {
      //heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / num_leds) + 2));
      heat[i] = qsub8( heat[i],  random8(0, cooling));
	  //heat[i] = qsub8( heat[i],  random8(0, textInput2Int()));
      //heat[i] = qsub8( heat[i],  random8(0, random8(1,3)));
	  if(stop || stopShuffle) {return 0;}
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= num_leds - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
	  if(stop || stopShuffle) {return 0;}
    }
    
	uint8_t sparking = DEFAULT_FIRE;
	if(switches.SW1)	
		sparking = ROARING_FIRE;
	else if(switches.SW2)
		sparking = FLICKERY_FIRE;
	EVERY_N_MILLISECONDS(5000) {
		sparking = random8(FLICKERY_FIRE, ROARING_FIRE);
	}
	
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < sparking ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    int heatIdx=0;
	CRGB color;
    for(int j=startPixel; j < endPixel; j++) {
		// Scale the heat value from 0-255 down to 0-240
		// for best results with color palettes.
		byte colorindex = scale8( heat[heatIdx], 240);
		color = ColorFromPalette( thisP, colorindex);
	  /*if(switches.SW1) {
		// = ColorFromPalette( HeatColors_p, colorindex);
		color = ColorFromPalette( RainbowColors_p, colorindex);
	  } else if(switches.SW2){
		color = ColorFromPalette( CRGBPalette16( CRGB::Black, CRGB::DarkGreen, CRGB::LimeGreen,  CRGB::White), colorindex);
	  } else if(switches.SW3){
		color = ColorFromPalette( CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White), colorindex);
	  } else if(switches.SW4){
		color = ColorFromPalette( myRedWhiteBluePalette_p, colorindex);
	  } else {
		 color = HeatColor( heat[heatIdx]);
	  }*/
      int pixelnumber;
      if( direction ) {
        pixelnumber = (endPixel-1) - heatIdx;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
      heatIdx++;
	  if(stop || stopShuffle) {return 0;}
    }
    return 1;
}


void paletteCrossFade() {
  
  ChangePalettePeriodically();
 
  // nblendPaletteTowardPalette() will crossfade current palette slowly toward the target palette.
  //
  // Each time that nblendPaletteTowardPalette is called, small changes
  // are made to currentPalette to bring it closer to matching targetPalette.
  // You can control how many changes are made in each call:
  //   - the default of 24 is a good balance
  //   - meaningful values are 1-48.  1=veeeeeeeery slow, 48=quickest
  //   - "0" means do not change the currentPalette at all; freeze

  EVERY_N_MILLISECONDS(100) {
    uint8_t maxChanges = 24;  //map(speed, 1, 120, 1, 48); 	
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);
  }

 EVERY_N_MILLISECONDS(speed) {
  static uint8_t startIndex = 0;
  startIndex += 1;                                 // motion speed
  FillLEDsFromPaletteColors(startIndex);
 }

 showPixels();
 //delay(speed);
}

void FillLEDsFromPaletteColors(uint8_t colorIndex) {
  
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette(currentPalette, colorIndex + sin8(i*16), 255);
    colorIndex += 3;
  }

} 


void ChangePalettePeriodically() {
  uint8_t secondHand = (millis() / 1000) % 60;  
    static uint8_t lastSecond = 99;  
    if( lastSecond != secondHand) {        
        lastSecond = secondHand;		
        if(random8(2))		//	currentBlending = LINEARBLEND;		//else			currentBlending = NOBLEND;			        
        if( secondHand ==  0)  { currentPalette = RainbowColors_p;         }
		if( secondHand == 10)  { currentPalette = OceanColors_p;           }
        if( secondHand == 15)  { currentPalette = RainbowStripeColors_p;   }
        if( secondHand == 20)  { SetupPurpleAndGreenPalette();         	   }
        if( secondHand == 25)  { SetupTotallyRandomPalette();          	   }
        if( secondHand == 30)  { SetupBlackAndWhiteStripedPalette();   	   }
        if( secondHand == 35)  { currentPalette = LavaColors_p;            }
        if( secondHand == 40)  { currentPalette = ForestColors_p;          }
        if( secondHand == 45)  { currentPalette = CloudColors_p;           }
        if( secondHand == 50)  { currentPalette = PartyColors_p;           }
        if( secondHand == 55)  { currentPalette = myRedWhiteBluePalette_p; }
    }
  /*EVERY_N_SECONDS( SECONDS_PER_PALETTE ) {
    gCurrentPaletteNumber = addmod8( gCurrentPaletteNumber, 1, gGradientPaletteCount);
    currentPalette = gGradientPalettes[ gCurrentPaletteNumber ];
  }*/
} 

void threeSine() {

  ChangePalettePeriodically();
  
  EVERY_N_MILLISECONDS(speed) {                           // FastLED based non-blocking delay to update/display the sequence.
    uint8_t maxChanges = 24;
    nblendPaletteTowardPalette( currentPalette, targetPalette, maxChanges);
    three_sin();                                              // Improved method of using non-blocking delay
  }
  showPixels();
  //delay(speed);
}

void three_sin() {
  
  wave1 += beatsin8(10,-4,4);
  wave2 += beatsin8(15,-2,2);
  wave3 += beatsin8(12,-3, 3);

  for (int k=0; k<NUM_LEDS; k++) {
    uint8_t tmp = sin8(mul1*k + wave1) + sin8(mul1*k + wave2) + sin8(mul1*k + wave3);
    leds[k] = ColorFromPalette(currentPalette, tmp, 255);
  }
} 

void pick2Palette(void) {
    static uint8_t startIndex = 0;
    startIndex = startIndex + speedIndex; /* motion speed */
	currentPalette = CRGBPalette16( CRGB(colors.C1),CRGB(colors.C2));
	currentBlending = NOBLEND;
    FillLEDsFromPaletteColorsBlending(startIndex);
    showPixels();
    delay(speed);
}

void colorPalette() {
    ChangePalettePeriodically();
    
    static uint8_t startIndex = 0;
    startIndex = startIndex + speedIndex; /* motion speed */
    if(switches.SW1)
		currentBlending = NOBLEND;
	else
	    currentBlending = LINEARBLEND;
	//else if(switches.SW2) 
	//	currentBlending = random8(2)?LINEARBLEND:NOBLEND;
	//currentPalette = myRedWhiteBluePalette_p;
	
    FillLEDsFromPaletteColorsBlending(startIndex);
    
    showPixels();
    delay(speed);
  //  FastLED.delay(speed);
    
}

void christmasLightsPalette() {
    static uint8_t startIndex = 0;
    EVERY_N_SECONDS( 5 ) {
      startIndex = startIndex + 1; /* motion speed */
    }
    FillLEDsFromPaletteColorsBlending( startIndex);
    showPixels();
}








void FillLEDsFromPaletteColorsBlending( uint8_t colorIndex) {
    uint8_t thisBrightness = 255;
    
    for( int i = 0; i < NUM_LEDS; i++) {
       // leds[i] = ColorFromPalette( currentPalette, colorIndex, thisBrightness, currentBlending);
        leds[i] = ColorFromPalette( currentPalette, colorIndex, thisBrightness, currentBlending);
        colorIndex += 8;
    }
}

void colorTemperature() {
  // draw a generic, no-name rainbow
  static uint8_t starthue = 0;
  fill_rainbow( leds + 5, NUM_LEDS - 5, --starthue, 20);

  // Choose which 'color temperature' profile to enable.
  uint8_t secs = (millis() / 1000) % (DISPLAYTIME * 2);
  if( secs < DISPLAYTIME) {
    FastLED.setTemperature( TEMPERATURE_1 ); // first temperature
    leds[0] = TEMPERATURE_1; // show indicator pixel
  } else {
    FastLED.setTemperature( TEMPERATURE_2 ); // second temperature
    leds[0] = TEMPERATURE_2; // show indicator pixel
  }

  // Black out the LEDs for a few secnds between color changes
  // to let the eyes and brains adjust
  if( (secs % DISPLAYTIME) < BLACKTIME) {
    memset8( leds, 0, NUM_LEDS * sizeof(CRGB));
  }
  
  showPixels();
  delay(speed);
}

void colorWaves() {
 /* EVERY_N_SECONDS( SECONDS_PER_PALETTE ) {
    gCurrentPaletteNumber = addmod8( gCurrentPaletteNumber, 1, gGradientPaletteCount);
    gTargetPalette = gGradientPalettes[ gCurrentPaletteNumber ];
  }

  EVERY_N_MILLISECONDS(40) {
    nblendPaletteTowardPalette( gCurrentPalette, gTargetPalette, 16);
  } 
  
  colorwaves( leds, NUM_LEDS, gCurrentPalette);

  showPixels();
  delay(speed);*/
}


// This function draws color waves with an ever-changing,
// widely-varying set of parameters, using a color palette.
void colorwaves( CRGB* ledarray, uint16_t numleds, CRGBPalette16& palette) 
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
 
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 300, 1500);
  
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;
  
  for( uint16_t i = 0 ; i < numleds; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;
    uint16_t h16_128 = hue16 >> 7;
    if( h16_128 & 0x100) {
      hue8 = 255 - (h16_128 >> 1);
    } else {
      hue8 = h16_128 >> 1;
    }

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
    uint8_t index = hue8;
    //index = triwave8( index);
    index = scale8( index, 240);

    CRGB newcolor = ColorFromPalette( palette, index, bri8);

    uint16_t pixelnumber = i;
    pixelnumber = (numleds-1) - pixelnumber;
    
    nblend( ledarray[pixelnumber], newcolor, 128);
  }
}

// Alternate rendering function just scrolls the current palette 
// across the defined LED strip.
void palettetest( CRGB* ledarray, uint16_t numleds, const CRGBPalette16& gCurrentPalette)
{
  static uint8_t startindex = 0;
  startindex-=speedIndex;
  //fill_palette( ledarray, numleds, startindex, (256 / NUM_LEDS) + 1, gCurrentPalette, 255, LINEARBLEND);
  fill_palette( ledarray, numleds, startindex, 32, gCurrentPalette, 255, LINEARBLEND);
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; i++) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
    
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Red, // 'white' is too bright compared to red and blue
    CRGB::Red,
    CRGB::Red,
    CRGB::Red,
    
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Gray,
    
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Blue
};


    
void driveChristmasModes() {
	
	maxLoops = 10000;
	
	if(runChristmasModes && loopCount == 0) {
		if(christmasModeCnt >= (int)(sizeof christmasModes / sizeof christmasModes[0])) {
			arrayShuffle(christmasModes,(int)(sizeof christmasModes / sizeof christmasModes[0]));
			christmasModeCnt = 0;
		}
		currentModeID = christmasModes[christmasModeCnt];
		christmasModeCnt++;
	}
			
	switch(currentModeID)
	{
		case RANDOMCOLORWALK:
			// start with alternating red and green colors that randomly walk
			// to other colors for 400 loopCounts, fading over last 80
			if (loopCount % 6 == 0) {
				seed = random(30000);
			}
			randomSeed(seed);
			if(runChristmasModes) 
			    maxLoops = 800;
			randomColorWalk(loopCount == 0 ? 1 : 0, loopCount > maxLoops - 80);
			break;
		  
		case COLOREXPLOSION:  
			// bursts of random color that radiate outwards from random points
			// for 630 loop counts; no burst generation for the last 70 counts
			// of every 200 count cycle or over the over final 100 counts
			// (this creates a repeating bloom/decay effect)
			if(runChristmasModes)
			    maxLoops = 1260;
			colorExplosion((loopCount % 200 > 130) || (loopCount > maxLoops - 100));
			break;
			
		case WARMWHITESHIMMER:
			// warm white shimmer for 300 loopCounts, fading over last 70
			if (loopCount % 6 == 0) {
				seed = random(30000);
			}
			randomSeed(seed);
			if(runChristmasModes)
			    maxLoops = 600;
			warmWhiteShimmer(loopCount > maxLoops - 70);
			break;
			
		case TRADITIONALCOLORS:
			// repeating pattern of red, green, orange, blue, magenta that
			// slowly moves for 400 loopCounts
			if(runChristmasModes)
			    maxLoops = 800;
			traditionalColors();
			break;
			
		case CHRISTMAS_LIGHTS:
	        if(runChristmasModes)
	            maxLoops = 1000;
	        christmasLightsPalette();
	        break;




		case GRADIENT:	
			// red -> white -> green -> white -> red ... gradiant that scrolls
			// across the strips for 250 counts; this pattern is overlaid with
			// waves of dimness that also scroll (at twice the speed)
			if(runChristmasModes)
			    maxLoops = 500;
			gradient();
			delay(6);  // add an extra 6ms delay to slow things down
			break;
	  
		case BRIGHTTWINKLE:
			// random LEDs light up brightly and fade away; it is a very similar
			// algorithm to colorExplosion (just no radiating outward from the
			// LEDs that light up); as time goes on, allow progressively more
			// colors, halting generation of new twinkles for last 100 counts.
			if(runChristmasModes)
			    maxLoops = 2400;
			if (loopCount < 600) {
				brightTwinkle(0, 1, 0);  // only white for first 400 loopCounts
			} else if (loopCount < 1300) {
				brightTwinkle(0, 2, 0);  // white and red for next 250 counts
			} else if (loopCount < 1800) {
				brightTwinkle(1, 2, 0);  // red, and green for next 250 counts
			} else {
				// red, green, blue, cyan, magenta, yellow for the rest of the time
				brightTwinkle(1, 6, loopCount > maxLoops - 200);
			}
			break;
	}
	
    showPixels();
    delay(speed);
    loopCount++;  // increment our loop counter/timer.

    if (loopCount >= maxLoops) {
        loopCount = 0;  // reset timer
		if(runChristmasModes)
			//transitionAll(CRGB::Black,LINEAR);
			fadeTowardColorAll( 0, NUM_LEDS, CRGB::Black, 10);
        run = TRUE;
    }
    
}

// ***** PATTERN WarmWhiteShimmer *****
// Credit: https://github.com/pololu/pololu-led-strip-arduino/blob/master/examples/LedStripXmas/LedStripXmas.ino
// This function randomly increases or decreases the brightness of the 
// even red LEDs by changeAmount, capped at maxBrightness.  The green
// and blue LED values are set proportional to the red value so that
// the LED color is warm white.  Each odd LED is set to a quarter the
// brightness of the preceding even LEDs.  The dimOnly argument
// disables the random increase option when it is TRUE, causing
// all the LEDs to get dimmer by changeAmount; this can be used for a
// fade-out effect.
void warmWhiteShimmer(unsigned char dimOnly)
{
  const unsigned char maxBrightness = 120;  // cap on LED brighness
  const unsigned char changeAmount = 2;   // size of random walk step

  for (int i = 0; i < NUM_LEDS; i += 2)
  {
    // randomly walk the brightness of every even LED
    randomWalk(&leds[i].red, maxBrightness, changeAmount, dimOnly ? 1 : 2);
    
    // warm white: red = x, green = 0.8x, blue = 0.125x
    leds[i].green = leds[i].red*4/5;  // green = 80% of red
    leds[i].blue = leds[i].red >> 3;  // blue = red/8
    
    // every odd LED gets set to a quarter the brighness of the preceding even LED
    if (i + 1 < NUM_LEDS)
    {
      leds[i+1] = CRGB(leds[i].red >> 2, leds[i].green >> 2, leds[i].blue >> 2);
    }
  }
}


// Credit: https://github.com/pololu/pololu-led-strip-arduino/blob/master/examples/LedStripXmas/LedStripXmas.ino
// This function fades val by decreasing it by an amount proportional
// to its current value.  The fadeTime argument determines the
// how quickly the value fades.  The new value of val will be:
//   val = val - val*2^(-fadeTime)
// So a smaller fadeTime value leads to a quicker fade.
// If val is greater than zero, val will always be decreased by
// at least 1.
// val is a pointer to the byte to be faded.
void fade(unsigned char *val, unsigned char fadeTime)
{
  if (*val != 0)
  {
    unsigned char subAmt = *val >> fadeTime;  // val * 2^-fadeTime
    if (subAmt < 1)
      subAmt = 1;  // make sure we always decrease by at least 1
    *val -= subAmt;  // decrease value of byte pointed to by val
  }
}

// Credit: https://github.com/pololu/pololu-led-strip-arduino/blob/master/examples/LedStripXmas/LedStripXmas.ino
// Helper function for adjusting the colors for the BrightTwinkle
// and ColorExplosion patterns.  Odd colors get brighter and even
// colors get dimmer.
void brightTwinkleColorAdjust(unsigned char *color)
{
  if (*color == 255)
  {
    // if reached max brightness, set to an even value to start fade
    *color = 254;
  }
  else if (*color % 2)
  {
    // if odd, approximately double the brightness
    // you should only use odd values that are of the form 2^n-1,
    // which then gets a new value of 2^(n+1)-1
    // using other odd values will break things
    *color = *color * 2 + 1;
  }
  else if (*color > 0)
  {
    fade(color, 4);
    if (*color % 2)
    {
      (*color)--;  // if faded color is odd, subtract one to keep it even
    }
  }
}


// Credit: https://github.com/pololu/pololu-led-strip-arduino/blob/master/examples/LedStripXmas/LedStripXmas.ino
// Helper function for adjusting the colors for the ColorExplosion
// pattern.  Odd colors get brighter and even colors get dimmer.
// The propChance argument determines the likelihood that neighboring
// LEDs are put into the brightening stage when the central LED color
// is 31 (chance is: 1 - 1/(propChance+1)).  The neighboring LED colors
// are pointed to by leftColor and rightColor (it is not important that
// the leftColor LED actually be on the "left" in your setup).
void colorExplosionColorAdjust(unsigned char *color, unsigned char propChance,
 unsigned char *leftColor, unsigned char *rightColor)
{
  if (*color == 31 && random(propChance+1) != 0)
  {
    if (leftColor != 0 && *leftColor == 0)
    {
      *leftColor = 1;  // if left LED exists and color is zero, propagate
    }
    if (rightColor != 0 && *rightColor == 0)
    {
      *rightColor = 1;  // if right LED exists and color is zero, propagate
    }
  }
  brightTwinkleColorAdjust(color);
}


// ***** PATTERN ColorExplosion *****
// Credit: https://github.com/pololu/pololu-led-strip-arduino/blob/master/examples/LedStripXmas/LedStripXmas.ino
// This function creates bursts of expanding, overlapping colors by
// randomly picking LEDs to brighten and then fade away.  As these LEDs
// brighten, they have a chance to trigger the same process in
// neighboring LEDs.  The color of the burst is randomly chosen from
// among red, green, blue, and white.  If a red burst meets a green
// burst, for example, the overlapping portion will be a shade of yellow
// or orange.
// When TRUE, the noNewBursts argument changes prevents the generation
// of new bursts; this can be used for a fade-out effect.
// This function uses a very similar algorithm to the BrightTwinkle
// pattern.  The main difference is that the random twinkling LEDs of
// the BrightTwinkle pattern do not propagate to neighboring LEDs.
void colorExplosion(unsigned char noNewBursts)
{
  // adjust the colors of the first LED
  colorExplosionColorAdjust(&leds[0].red, 9, (unsigned char*)0, &leds[1].red);
  colorExplosionColorAdjust(&leds[0].green, 9, (unsigned char*)0, &leds[1].green);
  colorExplosionColorAdjust(&leds[0].blue, 9, (unsigned char*)0, &leds[1].blue);

  for (int i = 1; i < NUM_LEDS - 1; i++)
  {
    // adjust the colors of second through second-to-last LEDs
    colorExplosionColorAdjust(&leds[i].red, 9, &leds[i-1].red, &leds[i+1].red);
    colorExplosionColorAdjust(&leds[i].green, 9, &leds[i-1].green, &leds[i+1].green);
    colorExplosionColorAdjust(&leds[i].blue, 9, &leds[i-1].blue, &leds[i+1].blue);
  }
  
  // adjust the colors of the last LED
  colorExplosionColorAdjust(&leds[NUM_LEDS-1].red, 9, &leds[NUM_LEDS-2].red, (unsigned char*)0);
  colorExplosionColorAdjust(&leds[NUM_LEDS-1].green, 9, &leds[NUM_LEDS-2].green, (unsigned char*)0);
  colorExplosionColorAdjust(&leds[NUM_LEDS-1].blue, 9, &leds[NUM_LEDS-2].blue, (unsigned char*)0);

  if (!noNewBursts)
  {
    // if we are generating new bursts, randomly pick one new LED
    // to light up
    for (int i = 0; i < 1; i++)
    {
      int j = random(NUM_LEDS);  // randomly pick an LED

      switch(random(7))  // randomly pick a color
      {
        // 2/7 chance we will spawn a red burst here (if LED has no red component)
        case 0:
        case 1:
          if (leds[j].red == 0)
          {
            leds[j].red = 1;
          }
          break;
        
        // 2/7 chance we will spawn a green burst here (if LED has no green component)
        case 2:
        case 3:
          if (leds[j].green == 0)
          {
            leds[j].green = 1;
          }
          break;

        // 2/7 chance we will spawn a white burst here (if LED is all off)
        case 4:
        case 5:
          if ((leds[j].red == 0) && (leds[j].green == 0) && (leds[j].blue == 0))
          {
            leds[j] = CRGB(1, 1, 1);
          }
          break;
        
        // 1/7 chance we will spawn a blue burst here (if LED has no blue component)
        case 6:
          if (leds[j].blue == 0)
          {
            leds[j].blue = 1;
          }
          break;
          
        default:
          break;
      }
    }
  }
}


// Credit: https://github.com/pololu/pololu-led-strip-arduino/blob/master/examples/LedStripXmas/LedStripXmas.ino
// This function applies a random walk to val by increasing or
// decreasing it by changeAmount or by leaving it unchanged.
// val is a pointer to the byte to be randomly changed.
// The new value of val will always be within [0, maxVal].
// A walk direction of 0 decreases val and a walk direction of 1
// increases val.  The directions argument specifies the number of
// possible walk directions to choose from, so when directions is 1, val
// will always decrease; when directions is 2, val will have a 50% chance
// of increasing and a 50% chance of decreasing; when directions is 3,
// val has an equal chance of increasing, decreasing, or staying the same.
void randomWalk(unsigned char *val, unsigned char maxVal, unsigned char changeAmount, unsigned char directions)
{
  unsigned char walk = random(directions);  // direction of random walk
  if (walk == 0)
  {
    // decrease val by changeAmount down to a min of 0
    if (*val >= changeAmount)
    {
      *val -= changeAmount;
    }
    else
    {
      *val = 0;
    }
  }
  else if (walk == 1)
  {
    // increase val by changeAmount up to a max of maxVal
    if (*val <= maxVal - changeAmount)
    {
      *val += changeAmount;
    }
    else
    {
      *val = maxVal;
    }
  }
}

// ***** PATTERN RandomColorWalk *****
// Credit: https://github.com/pololu/pololu-led-strip-arduino/blob/master/examples/LedStripXmas/LedStripXmas.ino
// This function randomly changes the color of every seventh LED by
// randomly increasing or decreasing the red, green, and blue components
// by changeAmount (capped at maxBrightness) or leaving them unchanged.
// The two preceding and following LEDs are set to progressively dimmer
// versions of the central color.  The initializeColors argument
// determines how the colors are initialized:
//   0: randomly walk the existing colors
//   1: set the LEDs to alternating red and green segments
//   2: set the LEDs to random colors
// When TRUE, the dimOnly argument changes the random walk into a 100%
// chance of LEDs getting dimmer by changeAmount; this can be used for
// a fade-out effect.
void randomColorWalk(unsigned char initializeColors, unsigned char dimOnly)
{
  const unsigned char maxBrightness = 255;  // cap on LED brightness
  const unsigned char changeAmount = 3;  // size of random walk step
  
  // pick a good starting point for our pattern so the entire strip
  // is lit well (if we pick wrong, the last four LEDs could be off)
  unsigned char start;
  switch (NUM_LEDS % 7)
  {
    case 0:
      start = 3;
      break;
    case 1:
      start = 0;
      break;
    case 2:
      start = 1;
      break;
    default:
      start = 2;
  }

  for (int i = start; i < NUM_LEDS; i+=7)
  {
    if (initializeColors == 0)
    {
      // randomly walk existing colors of every seventh LED
      // (neighboring LEDs to these will be dimmer versions of the same color)
      randomWalk(&leds[i].red, maxBrightness, changeAmount, dimOnly ? 1 : 3);
      randomWalk(&leds[i].green, maxBrightness, changeAmount, dimOnly ? 1 : 3);
      randomWalk(&leds[i].blue, maxBrightness, changeAmount, dimOnly ? 1 : 3);
    }
    else if (initializeColors == 1)
    {
      // initialize LEDs to alternating red and green
      if (i % 2)
      {
        leds[i] = CRGB(maxBrightness, 0, 0);
      }
      else
      {
        leds[i] = CRGB(0, maxBrightness, 0);
      }
    }
    else
    {
      // initialize LEDs to a string of random colors
      leds[i] = CRGB(random(maxBrightness), random(maxBrightness), random(maxBrightness));
    }
    
    // set neighboring LEDs to be progressively dimmer versions of the color we just set
    if (i >= 1)
    {
      leds[i-1] = CRGB(leds[i].red >> 2, leds[i].green >> 2, leds[i].blue >> 2);
    }
    if (i >= 2)
    {
      leds[i-2] = CRGB(leds[i].red >> 3, leds[i].green >> 3, leds[i].blue >> 3);
    }
    if (i + 1 < NUM_LEDS)
    {
      leds[i+1] = leds[i-1];
    }
    if (i + 2 < NUM_LEDS)
    {
      leds[i+2] = leds[i-2];
    }
  }
}

// ***** PATTERN TraditionalColors *****
// Credit: https://github.com/pololu/pololu-led-strip-arduino/blob/master/examples/LedStripXmas/LedStripXmas.ino
// This function creates a repeating patern of traditional Christmas
// light colors: red, green, orange, blue, magenta.
// Every fourth LED is colored, and the pattern slowly moves by fading
// out the current set of lit LEDs while gradually brightening a new
// set shifted over one LED.
void traditionalColors()
{
  // loop counts to leave strip initially dark
  const unsigned char initialDarkCycles = 10;
  // loop counts it takes to go from full off to fully bright
  const unsigned char brighteningCycles = 20;
  
  if (loopCount < initialDarkCycles) { // leave strip fully off for 20 cycles
    return;
  }

  // if NUM_LEDS is not an exact multiple of our repeating pattern size,
  // it will not wrap around properly, so we pick the closest LED count
  // that is an exact multiple of the pattern period (20) and is not smaller
  // than the actual LED count.
  unsigned int extendedLEDCount = (((NUM_LEDS-1)/20)+1)*20;

  for (int i = 0; i < extendedLEDCount; i++)
  {
    unsigned char brightness = (loopCount - initialDarkCycles)%brighteningCycles + 1;
    unsigned char cycle = (loopCount - initialDarkCycles)/brighteningCycles;

    // transform i into a moving idx space that translates one step per
    // brightening cycle and wraps around
    unsigned int idx = (i + cycle)%extendedLEDCount;
    if (idx < NUM_LEDS)  // if our transformed index exists
    {
      if (i % 4 == 0)
      {
        // if this is an LED that we are coloring, set the color based
        // on the LED and the brightness based on where we are in the
        // brightening cycle
        switch ((i/4)%5)
        {
           case 0:  // red
             leds[idx].red = 200 * brightness/brighteningCycles; 
             leds[idx].green = 10 * brightness/brighteningCycles; 
             leds[idx].blue = 10 * brightness/brighteningCycles;  
             break;
           case 1:  // green
             leds[idx].red = 10 * brightness/brighteningCycles; 
             leds[idx].green = 200 * brightness/brighteningCycles;  
             leds[idx].blue = 10 * brightness/brighteningCycles; 
             break;
           case 2:  // orange
             leds[idx].red = 200 * brightness/brighteningCycles;  
             leds[idx].green = 120 * brightness/brighteningCycles; 
             leds[idx].blue = 0 * brightness/brighteningCycles; 
             break;
           case 3:  // blue
             leds[idx].red = 10 * brightness/brighteningCycles; 
             leds[idx].green = 10 * brightness/brighteningCycles; 
             leds[idx].blue = 200 * brightness/brighteningCycles; 
             break;
           case 4:  // magenta
             leds[idx].red = 200 * brightness/brighteningCycles; 
             leds[idx].green = 64 * brightness/brighteningCycles;  
             leds[idx].blue = 145 * brightness/brighteningCycles;  
             break;
        }
      }
      else
      {
        // fade the 3/4 of LEDs that we are not currently brightening
        fade(&leds[idx].red, 3);
        fade(&leds[idx].green, 3);
        fade(&leds[idx].blue, 3);
      }
    }
  }
}

// ***** PATTERN Gradient *****
// Credit: https://github.com/pololu/pololu-led-strip-arduino/blob/master/examples/LedStripXmas/LedStripXmas.ino
// This function creates a scrolling color gradient that smoothly
// transforms from red to white to green back to white back to red.
// This pattern is overlaid with waves of brightness and dimness that
// scroll at twice the speed of the color gradient.
void gradient()
{
  unsigned int j = 0;
  
  // populate colors array with full-brightness gradient colors
  // (since the array indices are a function of loopCount, the gradient
  // colors scroll over time)
  while (j < NUM_LEDS)
  {
    // transition from red to green over 8 LEDs
    for (int i = 0; i < 8; i++)
    {
      if (j >= NUM_LEDS){ break; }
      leds[(loopCount/2 + j + NUM_LEDS)%NUM_LEDS] = CRGB(160 - 20*i, 20*i, (160 - 20*i)*20*i/160);
      j++;
    }
    // transition from green to red over 8 LEDs
    for (int i = 0; i < 8; i++)
    {
      if (j >= NUM_LEDS){ break; }
      leds[(loopCount/2 + j + NUM_LEDS)%NUM_LEDS] = CRGB(20*i, 160 - 20*i, (160 - 20*i)*20*i/160);
      j++;
    }
  }
  
  // modify the colors array to overlay the waves of dimness
  // (since the array indices are a function of loopCount, the waves
  // of dimness scroll over time)
  const unsigned char fullDarkLEDs = 10;  // number of LEDs to leave fully off
  const unsigned char fullBrightLEDs = 5;  // number of LEDs to leave fully bright
  const unsigned char cyclePeriod = 14 + fullDarkLEDs + fullBrightLEDs;
  
  // if NUM_LEDS is not an exact multiple of our repeating pattern size,
  // it will not wrap around properly, so we pick the closest LED count
  // that is an exact multiple of the pattern period (cyclePeriod) and is not
  // smaller than the actual LED count.
  unsigned int extendedLEDCount = (((NUM_LEDS-1)/cyclePeriod)+1)*cyclePeriod;

  j = 0;
  while (j < extendedLEDCount)
  {
    unsigned int idx;
    
    // progressively dim the LEDs
    for (int i = 1; i < 8; i++)
    {
      idx = (j + loopCount) % extendedLEDCount;
      if (j++ >= extendedLEDCount){ return; }
      if (idx >= NUM_LEDS){ continue; }
  
      leds[idx].red >>= i;
      leds[idx].green >>= i;
      leds[idx].blue >>= i;      
    }
    
    // turn off these LEDs
    for (int i = 0; i < fullDarkLEDs; i++)
    {
      idx = (j + loopCount) % extendedLEDCount;
      if (j++ >= extendedLEDCount){ return; }
      if (idx >= NUM_LEDS){ continue; }
  
      leds[idx].red = 0;
      leds[idx].green = 0;
      leds[idx].blue = 0;
    }
    
    // progressively bring these LEDs back
    for (int i = 0; i < 7; i++)
    {
      idx = (j + loopCount) % extendedLEDCount;
      if (j++ >= extendedLEDCount){ return; }
      if (idx >= NUM_LEDS){ continue; }
  
      leds[idx].red >>= (7 - i);
      leds[idx].green >>= (7 - i);
      leds[idx].blue >>= (7 - i);      
    }
    
    // skip over these LEDs to leave them at full brightness
    j += fullBrightLEDs;
  }
}

// ***** PATTERN BrightTwinkle *****
// Credit: https://github.com/pololu/pololu-led-strip-arduino/blob/master/examples/LedStripXmas/LedStripXmas.ino
// This function creates a sparkling/twinkling effect by randomly
// picking LEDs to brighten and then fade away.  Possible colors are:
//   white, red, green, blue, yellow, cyan, and magenta
// numColors is the number of colors to generate, and minColor
// indicates the starting point (white is 0, red is 1, ..., and
// magenta is 6), so colors generated are all of those from minColor
// to minColor+numColors-1.  For example, calling brightTwinkle(2, 2, 0)
// will produce green and blue twinkles only.
// When TRUE, the noNewBursts argument changes prevents the generation
// of new twinkles; this can be used for a fade-out effect.
// This function uses a very similar algorithm to the ColorExplosion
// pattern.  The main difference is that the random twinkling LEDs of
// this BrightTwinkle pattern do not propagate to neighboring LEDs.
void brightTwinkle(unsigned char minColor, unsigned char numColors, unsigned char noNewBursts)
{
  // Note: the colors themselves are used to encode additional state
  // information.  If the color is one less than a power of two
  // (but not 255), the color will get approximately twice as bright.
  // If the color is even, it will fade.  The sequence goes as follows:
  // * Randomly pick an LED.
  // * Set the color(s) you want to flash to 1.
  // * It will automatically grow through 3, 7, 15, 31, 63, 127, 255.
  // * When it reaches 255, it gets set to 254, which starts the fade
  //   (the fade process always keeps the color even).
  for (int i = 0; i < NUM_LEDS; i++)
  {
    brightTwinkleColorAdjust(&leds[i].red);
    brightTwinkleColorAdjust(&leds[i].green);
    brightTwinkleColorAdjust(&leds[i].blue);
  }
  
  if (!noNewBursts)
  {
    // if we are generating new twinkles, randomly pick four new LEDs
    // to light up
    for (int i = 0; i < 4; i++)
    {
      int j = random(NUM_LEDS);
      if (leds[j].red == 0 && leds[j].green == 0 && leds[j].blue == 0)
      {
        // if the LED we picked is not already lit, pick a random
        // color for it and seed it so that it will start getting
        // brighter in that color
        switch (random(numColors) + minColor)
        {
          case 0:
            leds[j] = CRGB(1, 1, 1);  // white
            break;
          case 1:
            leds[j] = CRGB(1, 0, 0);  // red
            break;
          case 2:
            leds[j] = CRGB(0, 1, 0);  // green
            break;
          case 3:
            leds[j] = CRGB(0, 0, 1);  // blue
            break;
          case 4:
            leds[j] = CRGB(1, 1, 0);  // yellow
            break;
          case 5:
            leds[j] = CRGB(0, 1, 1);  // cyan
            break;
          case 6:
            leds[j] = CRGB(1, 0, 1);  // magenta
            break;
          default:
            leds[j] = CRGB(1, 1, 1);  // white
        }
      }
    }
  }
}


// Helper function that blends one uint8_t toward another by a given amount
uint8_t nblendU8TowardU8( uint8_t& cur, const uint8_t target, uint8_t amount)
{
  if( cur == target) return  0;
  
  if( cur < target ) {
    uint8_t delta = target - cur;
    delta = scale8_video( delta, amount);
    cur += delta;
  } else {
    uint8_t delta = cur - target;
    delta = scale8_video( delta, amount);
    cur -= delta;
  }
  return  1;
}

// Blend one CRGB color toward another CRGB color by a given amount.
// Blending is linear, and done in the RGB color space.
// This function modifies 'cur' in place.
uint8_t fadeTowardColor( CRGB& cur, const CRGB& target, uint8_t amount)
{
  uint8_t done = 0;
  done += nblendU8TowardU8( cur.red,   target.red,   amount);
  done += nblendU8TowardU8( cur.green, target.green, amount);
  done += nblendU8TowardU8( cur.blue,  target.blue,  amount);
  return done;
}

// Fade an entire array of CRGBs toward a given background color by a given amount
// This function modifies the pixel array in place.
void fadeTowardColorAll(  uint16_t start, uint16_t end, const CRGB& bgColor, uint8_t fadeAmount)
{
  uint8_t done; 
  do{
    done = 0;

    for( uint16_t i = start; i < end; i++) {
      done += fadeTowardColor( leds[i], bgColor, fadeAmount);
    }
	if(stop || stopShuffle) {return;}
    showPixels();
    delay(TRANSITION_DELAY);  
  }while(done > 0);
}

//Same as fadeTowardColorAll but only for one pixel
void fadeTowardColorOne(uint16_t index, const CRGB& bgColor, uint8_t fadeAmount) {
	uint8_t done=0; 
	do{
      done = fadeTowardColor(leds[index], bgColor, fadeAmount);
	  if(stop || stopShuffle) {return;}
      showPixels();
      delay(TRANSITION_DELAY);  
    }while(done > 0);
}

uint8_t clamp(unsigned value, unsigned lowClamp, unsigned highClamp) {
	return ((value<lowClamp) ? lowClamp : (value>highClamp) ? highClamp : value);
}

/** Convert a given hex color value (e.g., 'FF') to integer (e.g., 255)*/
int hexToInt(char val) {
    int v = (val > '9')? (val &~ 0x20) - 'A' + 10: (val - '0');
    return v;
}

// Convert separate R,G,B into packed 32-bit RGB color.
// Packed format is always RGB, regardless of LED strand color order.
uint32_t rgb2Int(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}

// ************************************************************************************
// You must **NEVER** change any values below this line! Unless you know what you are doing
// ************************************************************************************

//Spark Cloud Mode
//Expect a string like this to change the mode Mode: M:ZONE,S:30,B:120,C1:002BFF,C2:FF00DB,C3:FF4600,C4:23FF00,
//Or simply this to just update speed or brightness:        S:30,B:120,
//Received command should have an ending comma, it makes this code easier
//Argument (command) length is limited to a max of 63 characters
//All colors are in hex format
//If the mode Mode is changing, return the enum value of the mode
//Else if only the speed or brightness is being updated return the following:
//returnValue = 1000 - command was received to update speed or brightness, but new values  are == to old values
//returnValue = 1001 - Brightness has been updated
//returnValue = 1002 - Speed has been updated
int SetMode(String command) {
	int beginIdx = 0;
	int returnValue = -1;
	int idx = command.indexOf(',');
    bool isNewMode = FALSE;
	bool isNewSpeed = FALSE;
	bool isNewBrightness = FALSE;
	bool isNewColor = FALSE;
    bool isNewSwitch = FALSE;
    
    // Trim extra spaces
    command.trim();
    
    //keep track or the last command received for the auto off feature
    lastCommandReceived = millis();
    unsigned long sysM = System.millis();
    //sprintf(debug2,"%s", command.c_str());

	while(idx != -1) {
		if(command.charAt(beginIdx) == 'M') {   //Mode name
			//set the new mode from modeStruct array index
			returnValue = setNewMode(getModeIndexFromName(command.substring(beginIdx+2, idx).c_str())); 
			//Handle Shuffle stuff here
			if(currentModeID == SHUFFLE) { stopShuffle = shuffleMode = TRUE; }
			else { stopShuffle = shuffleMode = FALSE;  }
			char tempBuf[20];
			sprintf(tempBuf,"SETAUXSWITCH:%i,%i;",SHFL,shuffleMode?1:0);
			FnRouter(tempBuf);  //update to reflect on or off states of shuffle
			isNewMode = TRUE;
			//EEPROM.PUT is in setNewMode
		}
		else if(command.charAt(beginIdx) == 'S') {  //Speed (Delay) value index
		    int receivedSpeedValue = command.substring(beginIdx+2, idx).toInt();
		    if(receivedSpeedValue > (int)(sizeof(speedPresets)/sizeof(int)))
		        receivedSpeedValue = (int)sizeof(speedPresets)/sizeof(int) - 1;
			//int newSpeed = speedPresets[receivedSpeedValue];
		    if (speedIndex != receivedSpeedValue) {
				isNewSpeed = TRUE;
			}
			//speedIndex = newSpeed;
			setSpeed(receivedSpeedValue);
			//hueAdvanceTimer.changePeriod((uint32_t)speed);
			EEPROM.put(SPEED_START_ADDR, speedIndex);
		}
		else if(command.charAt(beginIdx) == 'B') {  //Scaled Brightness value
		    int newBrightness = command.substring(beginIdx+2, idx).toInt() * 255 / 100;	//Scale 0-100 to 0-255
			if(brightness != newBrightness) {
				isNewBrightness = TRUE;
			}
			brightness = newBrightness;
			EEPROM.put(BRIGHT_START_ADDR, brightness);
		}
        else if(command.charAt(beginIdx) == 'C') {  //Color values are comming in
            char * p;
			isNewColor = TRUE;
			uint32_t C1Copy = colors.C1; //This is just in case our IFTTT case doesn't match a color
            switch(command.charAt(beginIdx+1)) {
                case '1':
                    colors.C1 = strtoul( command.substring(beginIdx+3, idx).c_str(), & p, 16 );  //Convert hex string to int
					EEPROM.put(COLOR1_START_ADDR, colors.C1);
                    break;
                case '2':
                    colors.C2 = strtoul( command.substring(beginIdx+3, idx).c_str(), & p, 16 );  //Convert hex string to int
					EEPROM.put(COLOR2_START_ADDR, colors.C2);
                    break;
                case '3':
                    colors.C3 = strtoul( command.substring(beginIdx+3, idx).c_str(), & p, 16 );  //Convert hex string to int
					EEPROM.put(COLOR3_START_ADDR, colors.C3);
                    break;
                case '4':
                    colors.C4 = strtoul( command.substring(beginIdx+3, idx).c_str(), & p, 16 );  //Convert hex string to int
					EEPROM.put(COLOR4_START_ADDR, colors.C4);
                    break;
                case '5':
                    colors.C5 = strtoul( command.substring(beginIdx+3, idx).c_str(), & p, 16 );  //Convert hex string to int
					EEPROM.put(COLOR5_START_ADDR, colors.C5);
                    break;
                case '6':
                    colors.C6 = strtoul( command.substring(beginIdx+3, idx).c_str(), & p, 16 );  //Convert hex string to int
					EEPROM.put(COLOR6_START_ADDR, colors.C6);
                    break;
            }
             if(command.substring(beginIdx+3,idx).equals("Red") || command.substring(beginIdx+3,idx).equals("red"))
                colors.C1 = 0xFF0000;   
            if(command.substring(beginIdx+3,idx).equals("Green") || command.substring(beginIdx+3,idx).equals("green"))
                colors.C1 = 0x00FF00;
            if(command.substring(beginIdx+3,idx).equals("Blue") || command.substring(beginIdx+3,idx).equals("blue"))
                colors.C1 = 0x0000FF;
            if(command.substring(beginIdx+3,idx).equals("Yellow") || command.substring(beginIdx+3,idx).equals("yellow"))
                colors.C1 = 0xFFFF00;
            if(command.substring(beginIdx+3,idx).equals("Magenta") || command.substring(beginIdx+3,idx).equals("magenta"))
                colors.C1 = 0xFF00FF;
            if(command.substring(beginIdx+3,idx).equals("Cyan") || command.substring(beginIdx+3,idx).equals("cyan"))
                colors.C1 = 0x00FFFF;
            if(command.substring(beginIdx+3,idx).equals("White") || command.substring(beginIdx+3,idx).equals("white"))
                colors.C1 = 0xFFFFFF;
            if(command.substring(beginIdx+3,idx).equals("Orange") || command.substring(beginIdx+3,idx).equals("orange"))
                colors.C1 = 0xFFA500;
            if(command.substring(beginIdx+3,idx).equals("Pink") || command.substring(beginIdx+3,idx).equals("pink"))
               colors.C1 = 0xffb6c1;
             if(command.substring(beginIdx+3,idx).equals("Purple") || command.substring(beginIdx+3,idx).equals("purple"))
               colors.C1 = 0x6A0DAD;
            if(command.substring(beginIdx+3,idx).equals("Chartreuse") || command.substring(beginIdx+3,idx).equals("chartreuse"))
                colors.C1 = 0x7FFF00;
            if(command.substring(beginIdx+3,idx).equals("Normal") || command.substring(beginIdx+3,idx).equals("normal"))
                colors.C1 = defaultColor;    

            if(colors.C1 == 0)
                colors.C1 = C1Copy;
                
		}
		// T for Toggle switch - expect 0 or 1 for FALSE or TRUE
		// S for Switch would have made more sense, but want to keep this backwards compatible and S is alreay Speed
		else if(command.charAt(beginIdx) == 'T') { 
			isNewSwitch = TRUE;
            switch(command.charAt(beginIdx+1)) {
                case '1':
                    switches.SW1 = command.substring(beginIdx+3, idx).toInt() & 1;
					EEPROM.put(SWITCH1_START_ADDR, switches.SW1);
                    break;
                case '2':
                    switches.SW2 = command.substring(beginIdx+3, idx).toInt() & 1;
					EEPROM.put(SWITCH2_START_ADDR, switches.SW2);
                    break;
                case '3':
                    switches.SW3 = command.substring(beginIdx+3, idx).toInt() & 1;
					EEPROM.put(SWITCH3_START_ADDR, switches.SW3);
                    break;
                case '4':
                    switches.SW4 = command.substring(beginIdx+3, idx).toInt() & 1;
					EEPROM.put(SWITCH4_START_ADDR, switches.SW4);
                    break;
            }
		}

		beginIdx = idx + 1;
		idx = command.indexOf(',', beginIdx);
	}
	
    if(isNewColor) {
		sprintf(colorValues,"c1=0x%X,c2=0x%X,c3=0x%X,c4=0x%X,c5=0x%X,c6=0x%X,",colors.C1,colors.C2,colors.C3,colors.C4,colors.C5,colors.C6);
	}
	
    //Set the flags if it's a new mode
	//Need this when just updating speed and brightness so a currently running mode doesn't start over
    if(isNewMode==TRUE) {
        run = TRUE;
	    stop = TRUE;    
    }
    else {
		//I guess we're updating only the speed or brightness, so let's update the Pixels
		showPixels();
		if(isNewBrightness==TRUE) {
			//Let the sender know we got the new brightness command
			returnValue = BRIGHTNESS_SET;
		}
		else if(isNewSpeed==TRUE) {
			//Let the sender know we got the new speed command
			returnValue = SPEED_SET;
		}
		else {
			//If we got here, it's possible that a command was set to update speed or brightness
			//But neither one was a new value. The new values, equal the current values
			returnValue = NO_CHANGE;
		}
    }

	// still here, so everything must be fine
	return returnValue;
}

/** 
 * SetText Cloud Function 
 * Display text on matrix configured LEDs
 **/
int SetText(String command) {

    sprintf(textInputString,"%s", command.c_str());

	return 1;
}

/** 
 * SetTimer Cloud Function 
 * Expect a string like this: 1,1700,2230,7C,M:COLORALL,B:120,C1:002BFF;
 * On and Off times are set as a hex text value
 **/
int SetTimer(String command) {
	int beginIdx = 0;
	int commaIdx = command.indexOf(',');
	char * p;
	
	// Trim extra spaces
    command.trim();
	
/* get Timer ID */
	uint8_t status_id = (uint16_t)strtoul( command.substring(beginIdx, commaIdx).c_str(), NULL, 16 );
	uint8_t id = status_id & 7;
	timerStruct[id].timerId = status_id;

/* get Timer Name */	
	beginIdx = commaIdx + 1;
	commaIdx = command.indexOf(',', beginIdx);
	sprintf(timerStruct[id].timerName, "%s", command.substring(beginIdx, commaIdx).c_str());
	
/* get On Time */
	beginIdx = commaIdx + 1;
	commaIdx = command.indexOf(',', beginIdx);
	timerStruct[id].turnOnTime = (uint16_t)strtoul( command.substring(beginIdx, commaIdx).c_str(), NULL, 16 );
	
/* get Off Time */
	beginIdx = commaIdx + 1;
	commaIdx = command.indexOf(',', beginIdx);
	timerStruct[id].turnOffTime = (uint16_t)strtoul( command.substring(beginIdx, commaIdx).c_str(), NULL, 16 );
	
/*get Days of Week */
	beginIdx = commaIdx + 1;
	commaIdx = command.indexOf(',', beginIdx);
	timerStruct[id].daysOfWeek = (byte)strtoul( command.substring(beginIdx, commaIdx).c_str(), NULL, 16 );
	
/*get Brightness */
	beginIdx = commaIdx + 1;
	commaIdx = command.indexOf(',', beginIdx);
	timerStruct[id].brightness = (byte)strtoul( command.substring(beginIdx, commaIdx).c_str(), NULL, 16 );
	timerStruct[id].brightness = timerStruct[id].brightness * 255 / 100;	//Scale 0-100 to 0-255
	
/* get Mode Info */
	beginIdx = commaIdx + 1;
	int semicolonIdx = command.indexOf(';', beginIdx);
	sprintf(timerStruct[id].modeInfo, "%s", command.substring(beginIdx, semicolonIdx).c_str());
	
	makeTimerList();
	storeTimers();
	return timerStruct[id].timerId;
}


/** 
 * Miscellaneous Cloud Function Router
 * Expect a string with a single function identifier 
 * followed by a colon followed by parameters
 **/
int FnRouter(String command) {
	// Trim extra spaces
    command.trim();
    // Convert it to upper-case for easier matching
    command.toUpperCase();
	
    int beginIdx = 0;
	int colonIdx = command.indexOf(':');
	//sprintf(debug,"%s", command.c_str());
	// Set time zone offset
    if(command.substring(beginIdx, colonIdx)=="SETTIMEZONE") {
		//Expect a string like this: SETTIMEZONE:-6
        timeZone = command.substring(colonIdx+1).toFloat();
        //sprintf(debug,"%s, tZone=%f", command.c_str(), timeZone);
        Time.zone(timeZone);
        EEPROM.put(TIMER_ZONE_ADDR, timeZone);
        return timeZone;
    }
	else if(command.substring(beginIdx, colonIdx)=="SETAUXSWITCH") {
		//Expect a string like this: SETAUXSWITCH:1,0;
		//That breaks down to: SwitchID,state;
		//State must be 0 or 1
		beginIdx = colonIdx+1;
		int commaIdx = command.indexOf(',');
		int semiColonIdx = command.indexOf(';');
		int id = 0;
		while(semiColonIdx != -1) {
			id = (int) command.substring(beginIdx, commaIdx).toInt();
			bool state = command.substring(commaIdx+1,semiColonIdx).equals("1") ? TRUE : FALSE;
			auxSwitchStruct[getAuxSwitchIndexFromID(id)].auxSwitchState = state;
			
			beginIdx = semiColonIdx + 1;
			commaIdx = command.indexOf(',', beginIdx);
			semiColonIdx = command.indexOf(';', commaIdx);
		}
		
		//Update the list
		makeAuxSwitchList();
		storeAuxSwitches();
		//Update Switch flags
		return updateAuxSwitches(id);
	}
	else if(command.substring(beginIdx, colonIdx)=="SETINTEGER") {
		//Expect a string like this: SETINTEGER:1,60;
		//That breaks down to: IntegerID,value;
		beginIdx = colonIdx+1;
		int commaIdx = command.indexOf(',');
		int semiColonIdx = command.indexOf(';');
		int id = 0;
		int value = 0;
		while(semiColonIdx != -1) {
			id = (int) command.substring(beginIdx, commaIdx).toInt();
			value = (int) command.substring(commaIdx+1,semiColonIdx).toInt();
			auxIntegerStruct[getAuxIntegerIndexFromID(id)].auxIntegerValue = value;
			
			beginIdx = semiColonIdx + 1;
			commaIdx = command.indexOf(',', beginIdx);
			semiColonIdx = command.indexOf(';', commaIdx);
		}
		makeAuxIntegerList();
		storeAuxIntegers();
		updateAuxInteger(id);
		return id;
	}
    else if(command.equals("GETTIME")) {
        //Time is expressed as number of minutes for the day
	    return (Time.hour()*60)+Time.minute();
	}
	else if(command.equals("REBOOT")) {
        resetFlag = TRUE;
        stop = TRUE;
        return 1;
    }
    else if(command.equals("CLEAREEPROM")) {
        EEPROM.clear();
        return 1;
    }
    
    return -1;  
 }
 
void setSpeed(int index) {
    speedIndex = index;
	speed = speedPresets[speedIndex];
}

//Change Mode based on the modeStruct array index
int setNewMode(int newModeIndex) {
    lastModeID = currentModeID;
    if(newModeIndex < 0) newModeIndex = 0;
    currentModeID = modeStruct[newModeIndex].modeId;
    
    sprintf(currentModeName,"%s", modeStruct[newModeIndex].modeName);
    resetVariables(modeStruct[newModeIndex].modeId);
    
    if(currentModeID != OFF ||
       currentModeID != NORMAL)
        EEPROM.put(LASTMODE_START_ADDR, currentModeID);
	return newModeIndex;
}

int getModeIndexFromName(String name) {
    for(uint16_t i=0;i<(int)(sizeof modeStruct / sizeof modeStruct[0]);i++) {
        if(name.equals(String(modeStruct[i].modeName))) {
            return i;
        }
    }
    return -1;
}

int getModeIndexFromID(int id) {
    for(uint16_t i=0;i<(int)(sizeof modeStruct / sizeof modeStruct[0]);i++) {
        if(id == modeStruct[i].modeId) {
            return i;
        }
    }
    return -1;
}

String getModeNameFromID(int id) {
    for(uint16_t i=0;i<(int)(sizeof modeStruct / sizeof modeStruct[0]);i++) {
        if(id == modeStruct[i].modeId) {
            return modeStruct[i].modeName;
        }
    }
    return NOT_FOUND;
}

int getAuxSwitchIndexFromID(int id) {
    for(uint16_t i=0;i<(int)(sizeof auxSwitchStruct / sizeof auxSwitchStruct[0]);i++) {
        if(id == auxSwitchStruct[i].auxSwitchId) {
            return i;
        }
    }
    return -1;
}

int getAuxIntegerIndexFromID(int id) {
    for(uint16_t i=0;i<(int)(sizeof auxIntegerStruct / sizeof auxIntegerStruct[0]);i++) {
        if(id == auxIntegerStruct[i].auxIntegerId) {
            return i;
        }
    }
    return -1;
}

