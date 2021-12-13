/*@file     SparkPixels_FastLED.ino
 * @authors  Kevin Carlborg
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

// This #include statement was automatically added by the Particle IDE.
#include <FastLED.h>
#include "math.h"
FASTLED_USING_NAMESPACE;

//Global Defines
#define BUILD_FILE_NAME             "FastLED Pixels"
#define BUILD_REVISION              "0.2"
#define GET_TEMP_ENABLED            FALSE   //Should we measure a temp sensor?
#define TIME_ZONE_OFFSET	        -5		//The offset to set your region's time correctly  -6
#define DEFAULT_BRIGHTNESS			100		//Set how bright you want your pixels on startup

//NEOPIXEL Defines
#define NUM_LEDS   	512
#define PIXEL_PIN  	D0
#define PIXEL_TYPE 	NEOPIXEL  //WS2812B
CRGB leds[NUM_LEDS];

//Particle Defines
#define MAX_PUBLISHED_STRING_SIZE   622    //defined by Particle Industries
#define TEXT_LENGTH					64	//Max character length passed to a Cloud Function

#ifndef PI
#define PI  3.1415926535
#endif

#define CLAMP_255(v) (v > 255 ? 255 : (v < 0 ? 0 : v))

//Timer Defines
Timer demoTimer(5*60*1000, advanceDemo);	// 5 minutes
Timer hueAdvanceTimer(20, advanceHue);		

/* ======================= ADD NEW MODE ID HERE. ======================= */
// Mode ID Defines
#define OFF                  0
#define NORMAL               1      //credit: Kevin Carlborg
#define COLORALL             2      //credit: Kevin Carlborg
#define SHUFFLE				 3		//credit: Kevin Carlborg
#define IFTTTWEATHER         4      //credit: Kevin Carlborg
#define DIGI            	 5		//credit: Kevin Carlborg
#define CHEERLIGHTS          6		//credit: Alex Hornstein, Werner Moecke (stability fixes), Kevin Carlborg(extra transition effects)
#define RIPPLE               7		//credit: Andrew Tuline - https://pastebin.com/LfBsPLRn
#define RAINBOW				 8		//credit: Mark Kriegsman - DemoReel100 example from fastled library
#define CONFETTI			 9		//credit: Mark Kriegsman - DemoReel100 example from fastled library
#define SINELON				10		//credit: Mark Kriegsman - DemoReel100 example from fastled library
#define JUGGLE				11		//credit: Mark Kriegsman - DemoReel100 example from fastled library
#define BPM					12		//credit: Mark Kriegsman - DemoReel100 example from fastled library
#define WIPE				13		//credit: Kevin Carlborg
#define CRASH               14 		//credit: Kevin Carlborg - previously called COLLIDE
#define COLORCYCLE          15		//credit: Kevin Carlborg
#define BEATWAVE            16		//credit: Andrew Tuline - https://github.com/atuline/FastLED-Demos/blob/master/beatwave/beatwave.ino
#define BLENDWAVE           17		//credit: Andrew Tuline - https://github.com/atuline/FastLED-Demos/blob/master/blendwave/blendwave.ino
#define LIGHTENING			18		//credit: Andrew Tuline - https://github.com/atuline/FastLED-Demos/blob/master/lightnings/lightnings.ino
#define NOISE16_1			19		//credit: Andrew Tuline - https://github.com/atuline/FastLED-Demos/blob/master/noise16_1/noise16_1.ino
#define INDEX               20      //credit: Kevin Carlborg - Use for setup to see where sections start/stop. Only send integer values in the text box of the app

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
    /*     modeId             modeName 	   #Colors #Switches  textInput
     *     -------------      -------------  ------- ---------  ----- */
        {  OFF,               "Off",               0,      0,      FALSE  },
        {  NORMAL,            "Normal",            0,      0,      FALSE  },  //credit: Kevin Carlborg
        {  SHUFFLE,           "Shuffle",           0,      0,      FALSE  },  //credit: Kevin Carlborg
		{  BEATWAVE,          "BeatWave",          0,      0,      FALSE  },  //credit: Andrew Tuline
		{  BLENDWAVE,         "BlendWave",         0,      0,      FALSE  },  //credit: Andrew Tuline
        {  BPM,               "BPM",               0,      0,      FALSE  },  //credit: Mark Kriegsman from DemoReel100 FastLED library
        {  CHEERLIGHTS,       "CheerLights",       0,      0,      FALSE  },  //credit: Alex Hornstein, Werner Moecke (stability fixes), Kevin Carlborg(extra transition effects)
		{  COLORALL,          "ColorAll",          1,      0,      FALSE  },  //credit: Kevin Carlborg
		{  COLORCYCLE,        "ColorCycle",        0,      0,      FALSE  },  //credit: Kevin Carlborg
        {  CONFETTI,          "Confetti",          0,      0,      FALSE  },  //credit: Mark Kriegsman from DemoReel100 FastLED library
		{  CRASH,             "Crash",             2,      2,      FALSE  },  //credit: Kevin Carlborg
		{  DIGI,              "DIGI",              1,      3,      FALSE  },  //credit: Kevin Carlborg
		{  INDEX,             "Index",             1,      0,      TRUE   },  //credit: Kevin Carlborg
        {  JUGGLE,            "Juggle",            0,      0,      FALSE  },  //credit: Mark Kriegsman from DemoReel100 FastLED library
		{  LIGHTENING,        "Lightening",        0,      0,      FALSE  },  //credit: Andrew Tuline
		{  NOISE16_1,         "Noise",             0,      0,      FALSE  },  //credit: Andrew Tuline
        {  RAINBOW,           "Rainbow",           0,      1,      FALSE  },  //credit: Andrew Tuline - https://pastebin.com/LfBsPLRn
		{  RIPPLE,            "Ripple",            0,      0,      FALSE  },  //credit: Mark Kriegsman from DemoReel100 FastLED library
		{  SINELON,           "Sinelon",           0,      0,      FALSE  },  //credit: Mark Kriegsman from DemoReel100 FastLED library
		{  WIPE,              "Wipe",              1,      4,      FALSE  },  //credit: Kevin Carlborg

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
		  {  RAINBOW,       "Add Glitter",         "",                    "",                    ""                  },  
		  {  WIPE,          "Cycle",               "Color Sweep",         "Random Colors",       "Fade Out"          },
	  //  {  TEST,          "Switch Title 1",      "Switch Title 2",      "Switch Title 3",      "Switch Title 4"    },
};


/* ======================= ADD NEW AUX SWITCH ID HERE. ======================= */
// AUX SWITCH ID Defines
#define ASO           	0
#define LIGHTSENSOR   	1
#define SHFL			2
#define RLM  			3

typedef struct auxSwitchParams {
    uint8_t  auxSwitchId;
    bool     auxSwitchState;
    char     auxSwitchTitle[20];
    char     auxSwitchOnName[20];
    char     auxSwitchOffName[20];
} auxSwitchParams;

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
		{  ASO,             TRUE,          "Auto Shut Off",       "ON",                  "OFF"                }, 
		//{  RLM,             FALSE,		   "On Startup",          "Run Last Mode",       "Run Default Mode"	  }
	    //{  LIGHTSENSOR,     FALSE,         "Brightness Control",  "Light Sensor",        "App Controlled"     },
};

/* ========================= Local Aux Switch Flags =========================== */

bool autoShutOff;   //Should we shut off the lights at certain times? This is toggled from the app
                    //Configure the Auto Shut Off times in the loop() function 
bool rememberLastMode;   //Should we remember the last mode ran? This is toggled from the app and kept in EEPROM
bool brightnessControl;


//Preset speed constants
const int speedPresets[] = { 120, 100, 80, 70, 50, 30, 20, 10, 1};  //in mSec, slow to fast       

//Temp Sensor constants	
const double refVoltage = 3.3;    //Voltage used as reference for analogRead
const double MINFANTEMP = 100.0;  //Min Temp to turn fan on low speed in Deg Fahrenheit
const double MAXFANTEMP = 115.0;  //Max Temp to turn fan on max speed in Deg Fahrenheit
const double MAXTEMP = 140.0;     //Shut Off Temp in Deg Fahrenheit


//Time Interval constants          hh*mm*ss*ms    
unsigned long oneMinuteInterval =      1*60*1000;	//Read temp every minute
unsigned long oneHourInterval =     1*60*60*1000;  //auto off in 1 hr when night time
unsigned long oneDayInterval = 	   24*60*60*1000;  //time sync interval - 24 hours=
unsigned long iftttWeatherInterval =  8*60*1000;  //revert back to last mode for IFTTT Weather

//Program Flags
int currentModeID;	//This is the ID of the current mode selected - used in the case statement
int lastModeID;
int run;    	//Use this for modes that don't need to loop. Set the color, then stop sending commands to the pixels
int stop;   	//Use this to break out of sequence loops when changing to a new mode
bool firstLap;
bool resetFlag;
bool shuffleMode;
volatile bool stopDemo;		//Set to TRUE when the Interrupt Timer demoTimer gets triggered

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
int speed;	//not to be confused with speedIndex below, this is the local speed (delay) value
int timeZone = TIME_ZONE_OFFSET;
unsigned long previousMillis = 0;
unsigned long lastCommandReceived = 0;
unsigned long lastSync = 0;
uint8_t colorWheel;
uint8_t colorWheel2;
uint32_t defaultColor = 0xFFFF3C;	//The NORMAL mode color
uint32_t color1;
uint32_t color2;
uint32_t color3;
uint32_t color4;
uint32_t color5;
uint32_t color6;
//uint32_t c1, c2;
uint32_t lastColors[6];
int lastBrightness;
bool switch1, switch2, switch3, switch4;
bool lastSwitchState[4];

//Spark Pin Defines
const int TEMP_SENSOR_PIN = A7;   //TMP36 sensor on this pin.
const int LIGHT_SENSOR_PIN = A6;  //Photo Resistor

//Spark Cloud Variables
int wifi = 0;   //used for general info and setup
int tHour=0;    //used for general info and setup
int speedIndex;     				//Let the cloud know what speed preset we are using
int brightness;                     //How bright do we want these things anyway
double temp = 0.0;   
double pressure = 0.0;
double humidity = 0.0;
char modeNameList[MAX_PUBLISHED_STRING_SIZE] = "None";        //Holds all mode info comma delimited. Use this to populate the android app
char modeParamList[MAX_PUBLISHED_STRING_SIZE] = "None";
char auxSwitchList[MAX_PUBLISHED_STRING_SIZE] = "None";
char deviceInfo[MAX_PUBLISHED_STRING_SIZE] = "";
char currentModeName[64] = "None";  //Holds current selected mode
char textInputString[64];           //Holds the Text for any mode needing a test input - only useful for a Neopixel Matrix
char debug[MAX_PUBLISHED_STRING_SIZE];                    //We might want some debug text for development


/* ============ SetMode return  Defines  ====================== */
#define NO_CHANGE           1000
#define BRIGHTNESS_SET      1001
#define SPEED_SET           1002

/* ============ AUTO SHUT OFF (ASO) Defines for Cloud Function: Function ====================== */
#define ASO_CMD_ON        "ASO_ON"
#define ASO_CMD_OFF       "ASO_OFF"
#define ASO_CMD_STATUS    "ASO_STATUS"
#define ASO_STATUS_OFF          2000
#define ASO_STATUS_ON           2001

/* ====================== EEPROM Addressing Definitions ====================== */
/************************
 *      constants       *
 ************************/
#define MAX_EEPROM_SIZE			2047	// the maximum available space in EEPROM storage (Photon)
#define TEXT_START_ADDR			0													// offset for the text store in TEXT mode
#define SWITCHES_START_ADDR		TEXT_START_ADDR + TEXT_LENGTH + 1					// offset for the lastSwitchState store
#define COLORS_START_ADDR		SWITCHES_START_ADDR + sizeof(lastSwitchState) + 1	// offset for the lastColors store
#define LASTMODE_START_ADDR		COLORS_START_ADDR + sizeof(lastColors) + 1			// offset for the currentModeID store
#define SPEED_START_ADDR		LASTMODE_START_ADDR + sizeof(int) + 1               // offset for the speedIndex store
#define BRIGHT_START_ADDR		SPEED_START_ADDR + sizeof(int) + 1                  // offset for the brightness store
#define AUXSW_START_ADDR		BRIGHT_START_ADDR + sizeof(int) + 1                 // offset for the auxSwitchStruct switch store


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


/* ========================= BeatWave function defines ======================== */
CRGBPalette16 currentPalette;
CRGBPalette16 targetPalette;
TBlendType    currentBlending;


/* ========================= BlendWave function defines ======================== */
CRGB clr1;
CRGB clr2;
uint8_t blendWaveSpeed;
uint8_t loc1;
uint8_t loc2;
uint8_t ran1;
uint8_t ran2;


/* ========================= Lightening function defines ======================== */
uint8_t flashes = 8;                                          //the upper limit of flashes per strike
unsigned int dimmer = 1;
uint8_t ledstart;                                             // Starting location of a flash
uint8_t ledlen;                                               // Length of a flash


/* ========================= Noise16_1 function defines ======================== */
uint8_t maxChanges;      // Number of tweens between palettes.


/* ========================= Transition Definitions ========================= */
#define LINEAR		0
#define POLAR       1
#define RED         0
#define GREEN       1
#define BLUE        2
uint8_t clamp(unsigned value, unsigned lowClamp, unsigned highClamp);
void transitionAll(CRGB endColor, uint16_t method);
void transitionOne(CRGB endColor, uint16_t index, uint16_t method);
void transitionHelper(CRGB startColor, CRGB endColor, uint16_t index, uint16_t method, int16_t numSteps, int16_t step);
int16_t getTransitionStep(CRGB startColor, CRGB endColor, uint16_t method, int16_t numSteps, int16_t step, uint8_t whichColor);

/* ======================= Required Mode Prototypes =============================== */
void arrayShuffle(int arrayToShuffle[], int arraySize);
int getModeIndexFromID(int id);
int getModeIndexFromName(String name);
int getSwitchTitleStructIndex(int modeId);
int getWeather(void);
int hexToInt(char val);
int isValidMode(String newMode);
bool is_number(const std::string& s);
void makeModeList(void);
int randomPixelFill(CRGB c);
void resetShuffleMode(void);
void resetVariables(int modeIndex);
uint32_t rgb2Int(uint8_t r, uint8_t g, uint8_t b);
int setNewMode(int newMode);
void setRandomMode(void);
int showPixels(void);
int updateAuxSwitches(int id);


/* ======================= Spark Pixel Mode Prototypes =============================== */
void addGlitter( fract8 chanceOfGlitter);
void beatWave();
void blendWave();
void bpm();
int colorAll(uint32_t c);
void colorCycle();
void confetti();
void crash(CRGB c1, CRGB c2);
void digi(uint32_t c);
void iftttWeather(uint32_t c);
void index(CRGB c);
void juggle() ;
void lightening();
void noise16_1();
void one_color_allHSV(int ahue, int abright);
void rainbow();
void ripple();
void sinelon();
int wrap(int step);
void wipe(CRGB c);


//Don't connect to the cloud first so we can turn on the lights right away
//SYSTEM_MODE(SEMI_AUTOMATIC);   

void setup() {
    //Declare Cloud Functions - Up to 15
    Particle.function("SetMode",       SetMode);            // Completely Necessary, Do Not Delete
    Particle.function("SetText",       SetText);            // Completely Necessary, Do Not Delete
    Particle.function("Function",      FnRouter);           // Completely Necessary, Do Not Delete

    //Delcare Cloud Variables - Up to 20
    Particle.variable("debug",         debug);              // Completely Necessary, Do Not Delete
    Particle.variable("speed",         speedIndex);         // Completely Necessary, Do Not Delete
    Particle.variable("brightness",    brightness);         // Completely Necessary, Do Not Delete
    Particle.variable("modeList",      modeNameList);       // Completely Necessary, Do Not Delete
	Particle.variable("modeParmList",  modeParamList);      // Completely Necessary, Do Not Delete
	Particle.variable("mode",          currentModeName);    // Completely Necessary, Do Not Delete
	Particle.variable("auxSwtchList",  auxSwitchList);      // Completely Necessary, Do Not Delete
	Particle.variable("deviceInfo",    deviceInfo);         // Completely Necessary, Do Not Delete
	Particle.variable("temp",          temp);				// Optional
	Particle.variable("pressure",      pressure);			// Optional
	Particle.variable("humidity",      humidity);			// Optional
    Particle.variable("wifi",          wifi);               // Optional
    Particle.variable("hour",          tHour);              // Optional   
    
    pinMode(TEMP_SENSOR_PIN,INPUT);
    pinMode(LIGHT_SENSOR_PIN,INPUT);
    
    //Serial.begin(9600);
    
    FastLED.addLeds<PIXEL_TYPE, PIXEL_PIN>(leds, NUM_LEDS);

	//Initialize
    color1 = color2 = color3 = color4 = color5 = color6 = 0;
    switch1 = switch2 = switch3 = switch4 = false;
    speedIndex = 5;
    speed = speedPresets[speedIndex];
    brightness = DEFAULT_BRIGHTNESS;
    stop = autoShutOff = shuffleMode = resetFlag = rememberLastMode = false;
	lastSync = lastCommandReceived = previousMillis = millis();	//Take a time stamp

    //colorAll(defaultColor);     //Turn on the NORMAL Mode
    transitionAll(defaultColor,POLAR);
	setNewMode(getModeIndexFromID(NORMAL));
	
    //Particle.connect();     //Now connect to the cloud
    Time.zone(timeZone);  	//set time zone 
    
    //Clear the cloud list variables
	sprintf(modeNameList,"");
	sprintf(modeParamList,"");
    sprintf(auxSwitchList,"");

    
    //initEEPROM();		 //Work in progress, not ready for release yet
	makeModeList();		 //Assemble Spark Cloud available modes variable
    makeAuxSwitchList();
    makeDeviceInfo();
    
	if(GET_TEMP_ENABLED)
		getTemperature();
		
    tHour = Time.hour();	//used to check for correct time zone
    wifi = WiFi.RSSI();    //Sometime I want to see how good the wifi signal is
    
    run = true;
	
	//populate the shuffle order array
	for(int i=0;i<(int)(sizeof modeStruct / sizeof modeStruct[0]);i++)
		modeShuffleOrder[i] = i;

}


void loop() {

    if(run || shuffleMode) {
		stop = FALSE;
		runMode(currentModeID);
    }
    
    if(resetFlag) {
        resetFlag = false;
        delay(200); //Need this here otherwise the Cloud Function returned response is null
        System.reset();
    }
	
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis > 10000) {
        previousMillis = currentMillis;
        tHour = Time.hour();
        wifi = WiFi.RSSI(); 
        makeDeviceInfo();  	//Keep up to date with my Device Info

        if(GET_TEMP_ENABLED) {
            getTemperature();
        }
		
		//Put other timing stuff in here to speed up main loop
        if (currentMillis - lastSync > oneDayInterval) {
            // Request time time synchronization from the Spark Cloud
            // sprintf(debug,"Last sync time = %i,", (int)(currentMillis - lastSync));
			Particle.syncTime();
            lastSync = currentMillis;
        }
        
       if(autoShutOff && (currentMillis - lastCommandReceived > oneHourInterval)) {
            //Auto Off Criteria
            //If it's Monday through Friday between 8am and 4pm or between 10pm and 5am any day, turn Off the lights
            if(((Time.weekday() >= 2 && Time.weekday() <=6) && (Time.hour() >= 8 && Time.hour() <= 16)) || (Time.hour() >= 23) || (Time.hour() <= 5)) {
                //No one is home or everyone is sleeping. So shut it down
   				lastCommandReceived = currentMillis;
   				setNewMode(getModeIndexFromID(OFF));
                run = TRUE;
            }
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
    
    sprintf(cBuff,"Free Memory (bytes):\"%i\",",System.freeMemory());
    strcat(deviceInfo,cBuff);
    
    sprintf(cBuff,"Current Time On Device:\"%i:%i:%i %s %s %i %i\",",Time.hour(),Time.minute(),Time.second(),getWeekDay(),getMonth(),Time.day(),Time.year());
    //sprintf(cBuff,"Current Time On Device:\"%s\",",Time.timeStr().c_str());
    strcat(deviceInfo,cBuff);
}

char* getWeekDay(void) {
    int weekDay = Time.weekday();   
    
    switch(weekDay) {
        case 1: return "Sun";
        case 2: return "Mon";
        case 3: return "Tue";
        case 4: return "Wed";
        case 5: return "Thu";
        case 6: return "Fri";
        case 7: return "Sat";
    }
    return "Not Found";
}

char* getMonth(void) {
    int month = Time.month();
    
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
    return "Not Found";
}

void makeModeList(void) {
    for(int i=0;i<sizeof modeStruct / sizeof modeStruct[0];i++) {
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
        return true;
    }
    int idx = strlen(modeParamList)-1;
	while(modeParamList[idx] != ',' && modeParamList[idx] != ';') {
		idx--;
	}
	modeParamList[idx] = ';';

    sprintf(debug,"Error: modeParamList has reached max size limit");
    
    return false;
}

int getSwitchTitleStructIndex(int modeId) {
    uint16_t i;
    for(i=0;i<sizeof switchTitleStruct / sizeof switchTitleStruct[0];i++) {
        if(switchTitleStruct[i].modeId == modeId)
            return i;
    }
    sprintf(debug,"Error: Missing Switch Titles for mode %s", modeStruct[getModeIndexFromID(modeId)].modeName);
    return -1;
}

// Uses the auxSwitchStruct to assemble the cloud attainable auxSwtchList variable
// Switch param order: "id,title,onName,offName,switchState;"
void makeAuxSwitchList(void) {
    sprintf(auxSwitchList,"");
    for(uint16_t i=0;i<sizeof auxSwitchStruct / sizeof auxSwitchStruct[0];i++) {
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
			if(shuffleMode) { stopDemo = TRUE; resetShuffleMode(); }
			else if( demoTimer.isActive()) { demoTimer.stop(); }
			return shuffleMode;
    }
    return -1;
}

/***************************************************************
 * Author: Werner Moecke
 * NOTE ON MY USE OF THE 'inline' QUALIFIER IN BELOW FUNCTIONS:
 * Using this provides a slight improvement in speed, with the
 * cost of adding an extra overhead, because it replaces each 
 * call with the whole function code; doing so because I know
 * both functions below will be called ONLY ONCE in the entire
 * run - clearEEPROM() is only called in DEBUG situations and
 * initEEPROM() is only called once within setup().
 ***************************************************************/
/* Function clearEEPROM() implemented due to the missing clear()
 * function in the EEPROM class (<= v0.4.9 firmware)
 */
inline void clearEEPROM(void) {
    for(int i = 0; i < MAX_EEPROM_SIZE; i++)
        EEPROM.write(i, 0xFF);
}

/* Check EEPROM area and initialize globals (if values were previoulsy set);
 * otherwise, initializes the respective EEPROM storage area with defaults.
 * Variables covered: speed, brightness, currentModeID, color1/2/3/4, 
 * switch1/2/3/4, drawingBuffer[]
 */
inline void initEEPROM(void) {
	//Initialize local flags
    bool colorsStored, switchesStored, clearBuffer;
    colorsStored = switchesStored = FALSE;
    clearBuffer = TRUE;
	
	// Initialize textInputString variable
	SetText("");
	//if(rememberLastMode) 
	{
		// Initialize speed variable
		speedIndex = EEPROM.read(SPEED_START_ADDR);
		if(speedIndex == 0xFF) {
			speedIndex = 5; 
			// Initialize EEPROM storage area
			EEPROM.write(SPEED_START_ADDR, speedIndex);
		}
		speed = speedPresets[speedIndex];
		
		// Initialize brightness variable
		brightness = EEPROM.read(BRIGHT_START_ADDR);
		if(brightness == 0xFF) {
			brightness = 20 * (255 * .01);	// Scale 0-100 to 0-255;
			// Initialize EEPROM storage area
			EEPROM.write(BRIGHT_START_ADDR, brightness);
		}
		lastBrightness = brightness;
		
		// Initialize currentModeID variable
		currentModeID = EEPROM.read(LASTMODE_START_ADDR);
		if(currentModeID == 0xFF) {
			currentModeID = getModeIndexFromID(NORMAL);
			// Initialize EEPROM storage area
			EEPROM.write(LASTMODE_START_ADDR, currentModeID);
		}
		setNewMode(getModeIndexFromID(currentModeID));
		lastModeID = currentModeID;
		run = true;
		
		// Initialize colors
		EEPROM.get(COLORS_START_ADDR, lastColors);
		for(int i=0; i<sizeof(lastColors); i++) {
			if(lastColors[i] != 0xFFFFFFFF) {
				colorsStored = TRUE;
				break;
			}
		}
		if(colorsStored) {
			color1 = lastColors[0];
			color2 = lastColors[1];
			color3 = lastColors[2];
			color4 = lastColors[3];
			color5 = lastColors[4];
			color6 = lastColors[5];
		}
		else {
			color1 = lastColors[0] = 0;
			color2 = lastColors[1] = 0;
			color3 = lastColors[2] = 0;
			color4 = lastColors[3] = 0;
			color5 = lastColors[4] = 0;
			color6 = lastColors[5] = 0;
			// Initialize EEPROM storage area
			EEPROM.put(COLORS_START_ADDR, lastColors);
		}

		// Initialize switches
	EEPROM.get(SWITCHES_START_ADDR, lastSwitchState);
	for(int i=0; i<sizeof(lastSwitchState); i++) {
		if(lastSwitchState[i] != 0xFF) {
			switchesStored = TRUE;
			break;
		}
	}
	if(switchesStored) {
        switch1 = lastSwitchState[0];
        switch2 = lastSwitchState[1];
        switch3 = lastSwitchState[2];
        switch4 = lastSwitchState[3];
	}
	else {
        switch1 = lastSwitchState[0] = FALSE;
        switch2 = lastSwitchState[1] = FALSE;
        switch3 = lastSwitchState[2] = FALSE;
        switch4 = lastSwitchState[3] = FALSE;
        // Initialize EEPROM storage area
        EEPROM.put(SWITCHES_START_ADDR, lastSwitchState);
	}
    }
}


int runMode(int modeID) {
    
	if(shuffleMode) { 
		if(stopDemo) {
			stopDemo = FALSE;
			setRandomMode(); 
			demoTimer.start(); 
		}
	}
	
    switch (modeID) {
        case OFF:
    	    transitionAll(CRGB::Black,LINEAR);
    	    //colorAll(CRGB::Black);
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
    	    colorAll(color1);
    	    transitionAll(CRGB(color1),POLAR);
    	    break;
		case COLORCYCLE:
			colorCycle();
			break;
    	case CONFETTI:
			confetti();
			break;
		case CRASH:
			crash(CRGB(color1), CRGB(color2));
			break;
		 case DIGI:
		    digi(color1);
		    break;
    	case IFTTTWEATHER:
		    iftttWeather(color6);
            break;
        case INDEX:
            index(CRGB(color1));
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
		case RAINBOW:
			rainbow();
			break;
		case RIPPLE:
			ripple();
			break;
		case SINELON:
			sinelon();
			break;
		case WIPE:
			wipe(CRGB(color1));
			break;
    	case NORMAL:
    	    //colorAll(defaultColor);
			transitionAll(CRGB(defaultColor),POLAR);
    	    break;
    	default:
		    //colorAll(defaultColor);
		    //transitionAll(CRGB(defaultColor),LINEAR);
    		break;
    }
    return 1;
}


void resetVariables(int modeIndex) {
	if(!shuffleMode && demoTimer.isActive()) {
		demoTimer.stop();
	}
	firstLap = true;
	
    switch (modeIndex) {
		case BEATWAVE:
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
			switch1 = switch2 = switch3 = switch4 = false;
		    break;
		case CRASH:
		case DIGI:
		case WIPE:
			colorWheel = random(256);
			colorWheel2=colorWheel+42;
			transitionAll(CRGB::Black, LINEAR);
			break;   
	    case IFTTTWEATHER:
	    case INDEX:
	    case LIGHTENING:
	        transitionAll(CRGB::Black, LINEAR);
	        break;
		case NOISE16_1:
			currentPalette = PartyColors_p;
			currentBlending = LINEARBLEND;
			maxChanges = 24;
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
		case SHUFFLE:
			resetShuffleMode();
			transitionAll(CRGB::Black, LINEAR);
			break;
		default:
			break;
    }    
	run = true;
}

int getTemperature(void){
    double measuredVoltage = refVoltage * analogRead(TEMP_SENSOR_PIN) / 4095.0;
    temp = (measuredVoltage - 0.5) * 100.0;          //Celcius Temp
    temp = (temp * 9.0 / 5.0) + 32.0; //Convert to Fahrenheit

    return 1;
}


void arrayShuffle(int arrayToShuffle[], int arraySize) {
    uint16_t i; 
    char cbuff[20];

    for(i=0;i<arraySize;i++) {
        int r = random(0,arraySize);  // generate a random position
        int temp = arrayToShuffle[i]; 
        arrayToShuffle[i] = arrayToShuffle[r]; 
        arrayToShuffle[r] = temp;
        //sprintf(cbuff,"%i %i %s ", arraySize,r,modeStruct[getModeIndexFromID(arrayToShuffle[i])].modeName);
        //strcat(debug,cbuff);
    }
}

void setRandomMode(void) {
	color1 = rgb2Int(random(256), random(256), random(256));
    color2 = rgb2Int(random(256), random(256), random(256));
    color3 = rgb2Int(random(256), random(256), random(256));
    color4 = rgb2Int(random(256), random(256), random(256));
	switch1 = random(2);
	switch2 = random(2);
	switch3 = random(2);
	switch4 = random(2);
	
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
	    switch1 = true;
	setNewMode(getModeIndexFromID(currentModeID));
}

//Shuffle the deck
void resetShuffleMode(void) {
	shuffleIdx = 0;
	arrayShuffle(modeShuffleOrder,(int)(sizeof modeShuffleOrder / sizeof modeShuffleOrder[0]));
}

void advanceDemo() {
    stopDemo = true;
	run = true;
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
	    //sprintf(debug, "pollinterval:%i, polltime:%i",POLLING_INTERVAL,millis()-pollTime);
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
            //sprintf(debug, "connected");
        }
        else {
            //sprintf(debug, "not connected");
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
						transitionAll(color,POLAR);
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
			firstLap = false;
			if(brightness > 10) {
				transitionAll(CRGB(c),POLAR);
			}
		} 
		else
			colorAll(c);
        brightness = oldBrightness;
    }
    else {
		transitionAll(CRGB::Black,LINEAR);
        currentModeID = lastModeID;
        setNewMode(getModeIndexFromID(currentModeID));
    }
    run = true;
}


/**
 * digi() helper function
 * Randomly fills the whole strip with a selected color or a random color
 * @param c: Next Color to populate
 * @switch1 = Random Color Fill: Ignore the passed color and choose random colors for each pixel
 */
int randomPixelFill(CRGB c) {
    uint16_t i; 
    int pixelFillOrder[NUM_LEDS];
    
    for(i=0; i<NUM_LEDS; i++) {
        pixelFillOrder[i]=i;
    }
    
    arrayShuffle(pixelFillOrder, sizeof pixelFillOrder / sizeof pixelFillOrder[0]);
    
    for(i=0; i<NUM_LEDS; i++) {
        if(stop || stopDemo) {return 0;}
        if(switch2 && c != CRGB(0)) { c = CHSV( random(256), 255, 255); }
		
        if(switch3) {
            transitionOne(c,pixelFillOrder[i],POLAR);
        }
        else {
            leds[pixelFillOrder[i]] =  c;
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
 * @switch1 = Color Sweep through the wheel
 * @switch2 = Random Color Fill: Used in randomPixelFill() to fill the strip with random colors
 * @switch3 = Slow Transition: 
 */
void digi(uint32_t col) {
    uint16_t i; 
    CRGB nextCol;

    nextCol = switch1? CHSV( colorWheel+=16, 255, 255) : CRGB(col);
    
    if(0 == randomPixelFill(nextCol)) { return; }	//Populate the cube
	delay(400);
    if(0 == randomPixelFill(0x0)) { return; }		//Kill off the voxels
	
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
  if(switch1)
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
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
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
	//If Color Sweep, pre-load the color from the colorWheel Hue
	if(switch2)	{ c = CHSV( colorWheel+=16, 255, 255); }
    else if(switch3) { c = CHSV( random(256), 255, 255); }
	//Forward
	for( int16_t i = 0; i < NUM_LEDS; i+=headLength) {
		for( int16_t headIncrement = 0; headIncrement < headLength; headIncrement++) {
			//For longer strings, light up more pixels each time
			if(i+headIncrement < NUM_LEDS)
			    leds[i+headIncrement] = c;
		}
		if(stop || stopDemo) {return;}
		showPixels();
		delay(speed);
	}
	
	// Cycle or Color Sweep
	if(switch1 || switch2 || switch3) 
	{
		delay(300);
		if(switch4) {	//Fade All out at the same time - No Sweep to next state
		    int tempSpeed = speed;
		    speed = speed < 70 ? 70 : speed;
			transitionAll(CRGB::Black, LINEAR);
			speed = tempSpeed;
		} else {		//Sweep Backwards to black 
			for( int16_t i = NUM_LEDS-1; i >= 0; i-=headLength) {
				for( int16_t headIncrement = 0; headIncrement < headLength; headIncrement++) {
				    if(i-headIncrement >= 0)
					    leds[i-headIncrement] = CRGB::Black;
				}
				if(stop || stopDemo) {return;}
				showPixels();
				delay(speed);
			}
		
		}
		run = true;
	} else {
	    run = false;
	}
}


//LEDs grow from each end of the strip, when they meet in the middle, they blow up into white, then fade out to the combined color
void crash(CRGB c1, CRGB c2) {
	int16_t stripLength = NUM_LEDS;	//Have to cast NUM_LEDS this way to calculate headLength properly - compiler issue? Weird!
	int16_t headLength = stripLength / 100 + 1;
	
	//Select the colors
	if(switch1 || switch2) {
    	if(switch1) {
    	    colorWheel = (uint8_t)(colorWheel+=85)%256;
    		colorWheel2 = (uint8_t)(colorWheel2+=43)%256;
    	} else if(switch2) {
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
		if(stop || stopDemo) {return;}
		showPixels();
		delay(speed);
	}

    //Explode the colors
    int tempSpeed = speed;
	speed = 5;
	transitionAll(CRGB::White, LINEAR);
	delay(100);
	transitionAll(c1 + c2, POLAR);
	delay(100);
	speed = 70;
	transitionAll(CRGB::Black,LINEAR);
	speed = tempSpeed;
    if(stop || stopDemo) {return;}
	showPixels();
	delay(speed);

	run = true;
}

//Cycle through colors over all LEDs
void colorCycle() { //OK //-FADE ALL LEDS THROUGH HSV RAINBOW
   run = TRUE;

   colorWheel = (colorWheel+1) % 256;

   for(int idex=0; idex<NUM_LEDS; idex++ ) {
      leds[idex] = CHSV(colorWheel, 255, 255);
	  if(stop || stopDemo) {return;}
   }
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
		if(stop || stopDemo) {return;}
	}
	
    showPixels();
    delay(speed);
}


void blendWave()	{
	blendWaveSpeed = beatsin8(6,0,255);

	clr1 = blend(CHSV(beatsin8(3,0,255),255,255), CHSV(beatsin8(4,0,255),255,255), blendWaveSpeed);
	clr2 = blend(CHSV(beatsin8(4,0,255),255,255), CHSV(beatsin8(3,0,255),255,255), blendWaveSpeed);

	loc1 = beatsin8(10,0,NUM_LEDS-1);
  
	fill_gradient_RGB(leds, 0, clr2, loc1, clr1);
	fill_gradient_RGB(leds, loc1, clr2, NUM_LEDS-1, clr1);
	
	showPixels();
	delay(speed);
}


void lightening() {
  ledstart = random8(NUM_LEDS);                               // Determine starting location of flash
  ledlen = random8(NUM_LEDS*0.6, NUM_LEDS-ledstart);                        // Determine length of flash (not to go beyond NUM_LEDS-1)
  
  for (int flashCounter = 0; flashCounter < random8(3,flashes); flashCounter++) {
    if(flashCounter == 0) dimmer = 5;                         // the brightness of the leader is scaled down by a factor of 5
    else dimmer = random8(1,3);                               // return strokes are brighter than the leader
    
    fill_solid(leds+ledstart,ledlen,CHSV(255, 0, 255/dimmer));
    FastLED.show();                       // Show a section of LED's
    delay(random8(4,10));                                     // each flash only lasts 4-10 milliseconds
    fill_solid(leds+ledstart,ledlen,CHSV(255,0,0));           // Clear the section of LED's
    FastLED.show();
    
    if (flashCounter == 0) delay (150);                       // longer delay until next flash after the leader
    
	if(stop || stopDemo) {return;}
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
	if(stop || stopDemo) {return;}
  }
  showPixels();
  delay(speed);
}


void index(CRGB c) {
    int pixelIndex = 0;
    if(is_number(textInputString)) {
        pixelIndex = atoi( textInputString );
        leds[pixelIndex] = c;
        showPixels();
    }
}

bool is_number(const std::string& s) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

/** 
 * Used to make linear transitions between colors by drawing a linear line from
 * the current color to the end color in the HSV model.
 * Assumes every pixel is at a different color value.
 * If the current color is blue and the next color is yellow, it will pass 
 * through the white spectrum to get there.
 * If the current color is red and the next color is blue, it will pass 
 * through pink/purple to get there.
 */
void transitionAll(CRGB endColor, uint16_t method) {
    int numSteps = 8;
    CRGB startColor[NUM_LEDS];

    //Save the start color for each pixel - yeah, I know this is using a lot of memory, but I'm not smart enough to do it a better way
    for(int index = 0; index < NUM_LEDS; index++) {
        startColor[index] = leds[index];
    }

    for(int i=1; i<=numSteps; i++) {
        for(int index = 0; index < NUM_LEDS; index++) {
            transitionHelper(startColor[index], endColor, index, method, numSteps, i); 
            if(stop || stopDemo) {return;}
        }
        showPixels();
        delay(speed);
    }
    run = false;
}

//Same as transitionAll but only for one pixel
void transitionOne(CRGB endColor, uint16_t index, uint16_t method) {
    int numSteps = 8;
    CRGB startColor = leds[index];

    for(int i=1; i<=numSteps; i++) {
        transitionHelper(startColor, endColor, index, method, numSteps, i);
        if(stop || stopDemo) {return;}
        showPixels();
        delay(speed);
    }
}

//Used to set the next color step for transitionAll and transitionOne
void transitionHelper(CRGB startColor, CRGB endColor, uint16_t index, uint16_t method, int16_t numSteps, int16_t step) {
    CRGB col2;

    //Find the step
    int16_t redStep   = getTransitionStep(startColor, endColor, method, numSteps, step, RED);
    int16_t greenStep = getTransitionStep(startColor, endColor, method, numSteps, step, GREEN);
    int16_t blueStep  = getTransitionStep(startColor, endColor, method, numSteps, step, BLUE);

    //Add the increment to get the next color segments
    //If new color is a higher value, set the high clamp to the new color
    //If new color is a smaller value, set the low clamp to the new color
    if(endColor.red   > startColor.red)   col2.red   = clamp(startColor.red   + redStep,  0,             endColor.red);
	else                                  col2.red   = clamp(startColor.red   + redStep,  endColor.red,  0xFF);
	if(endColor.green > startColor.green) col2.green = clamp(startColor.green + greenStep,0,             endColor.green);
	else                                  col2.green = clamp(startColor.green + greenStep,endColor.green,0xFF);
	if(endColor.blue  > startColor.blue)  col2.blue  = clamp(startColor.blue  + blueStep, 0,             endColor.blue);
	else                                  col2.blue  = clamp(startColor.blue  + blueStep, endColor.blue, 0xFF);
	
    //Let's make sure we hit the target
    if(step == numSteps) {
        col2.red   = endColor.red;
        col2.green = endColor.green;
        col2.blue  = endColor.blue;
    }
    
    //leds[index] = CRGB(col2.red, col2.green, col2.blue);
	leds[index] = col2;
}

//Used to get the next color step for transitionHelper()
int16_t getTransitionStep(CRGB startColor, CRGB endColor, uint16_t method, int16_t numSteps, int16_t step, uint8_t whichColor) {
	int16_t increment=0;

    if(method == LINEAR) {
        if(whichColor == RED)        increment = (step * (endColor.red-startColor.red))     / numSteps;
        else if(whichColor == GREEN) increment = (step * (endColor.green-startColor.green)) / numSteps;
        else if(whichColor == BLUE)  increment = (step * (endColor.blue-startColor.blue))   / numSteps;
    }
	else { // Not Quite POLAR     
	    if(whichColor == RED) {
	        if(endColor.red < startColor.red)
	            increment = sqrt(float(step)/numSteps) * float(endColor.red-startColor.red);
	        else
	            increment = (float(step)/numSteps) * (float(step)/numSteps) * float(endColor.red-startColor.red);
	    }
	    else if(whichColor == GREEN) {
	        if(endColor.green < startColor.green)
	            increment = sqrt(float(step)/numSteps) * float(endColor.green-startColor.green);
	        else
	            increment = (float(step)/numSteps) * (float(step)/numSteps) * float(endColor.green-startColor.green);
	    }
	    else if(whichColor == BLUE) {
	        if(endColor.blue < startColor.blue)
	            increment = sqrt(float(step)/numSteps) * float(endColor.blue-startColor.blue);
	        else 
	            increment = (float(step)/numSteps) * (float(step)/numSteps) * float(endColor.blue-startColor.blue);
	    }
	}

    return increment;
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
//Expect a string like this to change the mode Mode: M:ZONE,D:30,B:120,C1:002BFF,C2:FF00DB,C3:FF4600,C4:23FF00,
//Or simply this to just update speed or brightness:        D:30,B:120,
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
    bool isNewMode = false;
	bool isNewSpeed = false;
	bool isNewBrightness = false;
	bool isNewColor = FALSE;
    bool isNewSwitch = FALSE;
    
    // Trim extra spaces
    command.trim();
    // Convert it to upper-case for easier matching
   // command.toUpperCase();
    
    //keep track or the last command received for the auto off feature
    lastCommandReceived = millis();
    
   // sprintf(debug,"%s", command.c_str());

	while(idx != -1) {
		if(command.charAt(beginIdx) == 'M') {   //Mode name
			//set the new mode from modeStruct array index
			returnValue = setNewMode(getModeIndexFromName(command.substring(beginIdx+2, idx).c_str())); 
			//Handle Shuffle stuff here
			if(currentModeID == SHUFFLE) { stopDemo = shuffleMode = TRUE; }
			else { stopDemo = shuffleMode = FALSE;  }
			char tempBuf[20];
			sprintf(tempBuf,"SETAUXSWITCH:%i,%i;",SHFL,shuffleMode?1:0);
			FnRouter(tempBuf);  //update to reflect on or off states of shuffle
			isNewMode = true;
		}
		else if(command.charAt(beginIdx) == 'S') {  //Speed (Delay) value index
		    int receivedSpeedValue = command.substring(beginIdx+2, idx).toInt();
		    if(receivedSpeedValue > (int)(sizeof(speedPresets)/sizeof(int)))
		        receivedSpeedValue = sizeof(speedPresets)/sizeof(int) - 1;
			//int newSpeed = speedPresets[receivedSpeedValue];
		    if (speedIndex != receivedSpeedValue) {
				isNewSpeed = true;
			}
			//speedIndex = newSpeed;
			speedIndex = receivedSpeedValue;
			speed = speedPresets[speedIndex];
			hueAdvanceTimer.changePeriod((uint32_t)speed);
		}
		else if(command.charAt(beginIdx) == 'B') {  //Scaled Brightness value
		    int newBrightness = command.substring(beginIdx+2, idx).toInt() * 255 / 100;	//Scale 0-100 to 0-255
			if(brightness != newBrightness) {
				isNewBrightness = true;
			}
			brightness = newBrightness;
		}
        else if(command.charAt(beginIdx) == 'C') {  //Color values are comming in
            char * p;
			isNewColor = TRUE;
            switch(command.charAt(beginIdx+1)) {
                case '1':
                    color1 = strtoul( command.substring(beginIdx+3, idx).c_str(), & p, 16 );  //Convert hex string to int
                    break;
                case '2':
                    color2 = strtoul( command.substring(beginIdx+3, idx).c_str(), & p, 16 );  //Convert hex string to int
                    break;
                case '3':
                    color3 = strtoul( command.substring(beginIdx+3, idx).c_str(), & p, 16 );  //Convert hex string to int
                    break;
                case '4':
                    color4 = strtoul( command.substring(beginIdx+3, idx).c_str(), & p, 16 );  //Convert hex string to int
                    break;
                case '5':
                    color5 = strtoul( command.substring(beginIdx+3, idx).c_str(), & p, 16 );  //Convert hex string to int
                    break;
                case '6':
                    color6 = strtoul( command.substring(beginIdx+3, idx).c_str(), & p, 16 );  //Convert hex string to int
                    break;
            }
		}
		// T for Toggle switch - expect 0 or 1 for false or true
		// S for Switch would have made more sense, but want to keep this backwards compatible and S is alreay Speed
		else if(command.charAt(beginIdx) == 'T') { 
			isNewSwitch = TRUE;
            switch(command.charAt(beginIdx+1)) {
                case '1':
                    switch1 = command.substring(beginIdx+3, idx).toInt() & 1;
					lastSwitchState[0] = switch1;
                    break;
                case '2':
                    switch2 = command.substring(beginIdx+3, idx).toInt() & 1;
					lastSwitchState[1] = switch2;
                    break;
                case '3':
                    switch3 = command.substring(beginIdx+3, idx).toInt() & 1;
					lastSwitchState[2] = switch3;
                    break;
                case '4':
                    switch4 = command.substring(beginIdx+3, idx).toInt() & 1;
					lastSwitchState[3] = switch4;
                    break;
            }
		}

		beginIdx = idx + 1;
		idx = command.indexOf(',', beginIdx);
	}
	
	// Update the EEPROM storage area; EEPROM.put() only updates as necessary
    //if(isNewColor) {EEPROM.put(COLORS_START_ADDR, lastColors);}
    //if(isNewSwitch) {EEPROM.put(SWITCHES_START_ADDR, lastSwitchState);}
	
    //Set the flags if it's a new mode
	//Need this when just updating speed and brightness so a currently running mode doesn't start over
    if(isNewMode==true) {
        run = TRUE;
	    stop = TRUE;    
    }
    else {
		//I guess we're updating only the speed or brightness, so let's update the Pixels
		showPixels();
		if(isNewBrightness==true) {
			//Let the sender know we got the new brightness command
			returnValue = BRIGHTNESS_SET;
		}
		else if(isNewSpeed==true) {
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

int SetText(String command) {

    sprintf(textInputString,"%s", command.c_str());

	return 1;
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
	
	// Set time zone offset
    if(command.substring(beginIdx, colonIdx)=="SETTIMEZONE") {
		//Expect a string like this: SETTIMEZONE:-6
        timeZone = command.substring(colonIdx+1).toInt();
        Time.zone(timeZone);
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
			bool state = command.substring(commaIdx+1,semiColonIdx).equals("1") ? true : false;
			auxSwitchStruct[getAuxSwitchIndexFromID(id)].auxSwitchState = state;
			
			beginIdx = semiColonIdx + 1;
			commaIdx = command.indexOf(',', beginIdx);
			semiColonIdx = command.indexOf(';', commaIdx);
		}
		
		//Update the list
		makeAuxSwitchList();
		
		//Update Switch flags
		return updateAuxSwitches(id);
	}
	else if(command.equals("REBOOT")) {
        resetFlag = true;
        stop = TRUE;
        return 1;
    }
    
    return -1;  
 }
 

//Change Mode based on the modeStruct array index
int setNewMode(int newModeIndex) {
    lastModeID = currentModeID;
    if(newModeIndex < 0) newModeIndex = 0;
    currentModeID = modeStruct[newModeIndex].modeId;

	// Update the EEPROM storage area
    //if(currentModeID != IFTTTWEATHER && currentModeID != OFF) 
    //    EEPROM.write(LASTMODE_START_ADDR, currentModeID);
		
    sprintf(currentModeName,"%s", modeStruct[newModeIndex].modeName);
    resetVariables(modeStruct[newModeIndex].modeId);
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

int getAuxSwitchIndexFromID(int id) {
    for(uint16_t i=0;i<(int)(sizeof auxSwitchStruct / sizeof auxSwitchStruct[0]);i++) {
        if(id == auxSwitchStruct[i].auxSwitchId) {
            return i;
        }
    }
    return -1;
}
