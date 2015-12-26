  /**
 ******************************************************************************
 * @file     SparkPixels.ino:
 *		New mode: FLICKER
 *		New setting: Auto Shut Off enable/disable (through cloud function)
 *		New Functions: demo(), runMode(), setASO() 
 *      New Feature: Expanded modeStruct[] array to support up to 4 boolean switches
 *                  and a text input. Obviously text input it only good for Neopixel arrays. 
 *                  The switches can be used to turn mode options on and off.
 *                  Added switchTitleStruct[] array to define the switch titles in the app.
 *                  Added cloud controlled switch1, switch2, switch3, and switch4
 * @author   Kevin Carlborg
 * @version  V2.0.0
 * @date     18-November-2015 ~ 21-December-2015
 * 
 * @file     SparkPixels.ino
 *                  Added Demo Mode - Cycles through selected dynamic modes.
 *                  Restructured loop by adding runMode() function to handle
 *                  primary mode launches in the switch statement. This supports 
 *                  the Demo Mode.
 *                  Updated to Particle API calls instead of the depreciated 
 *                  Spark calls.
 *                  Removed the "&" from the STRING cloud variables - this fixes
 *                  the compiler Error: no matching function for call to 
 *                  ‘CloudClass::variable.
 *                  Other bew modes: CYCLECHASER, COLORCHASE, CHRISTMASTREE
 * @author   Kevin Carlborg
 * @version  V1.0.1
 * @date     06-August-2015 ~ 09-October-2015
 * 
 * @file     SparkPixels.ino
 * @authors  Kevin Carlborg
 * @version  V1.0.0
 * @date     14-July-2015
 * @brief    Neopixel strip Powered by the Spark Core
 *
 * @extended extra-examples.cpp from NEOPIXEL Library
 * @functions: colorAll,colorWipe,rainbow,rainbowCycle,theaterChase,Wheel 
 * @author   Phil Burgess
 * @version  V0.0.6
 * @date     NA
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
 
#include "neopixel/neopixel.h"
#include "math.h"

//Global Defines
#define MAX_PUBLISHED_STRING_SIZE   622     //defined by Particle Industries
#define GET_TEMP_ENABLED            FALSE   //Should we measure a temp sensor?
#define TIME_ZONE_OFFSET	        -6		//The offset to set your region's time correctly

//NEOPIXEL Defines
#define PIXEL_CNT 268
#define PIXEL_PIN D7
#define PIXEL_TYPE WS2812B

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_CNT, PIXEL_PIN, PIXEL_TYPE);

/* ======================= ADD NEW MODE ID HERE. ======================= */
// Mode ID Defines
const int OFF             = 0;
const int NORMAL          = 1;
const int COLORALL        = 2;
const int CHASER          = 3;
const int ZONE            = 4;
const int COLORWIPE       = 5;
const int CYCLEWIPE       = 6;
const int STROBE          = 7;
const int RAINBOW         = 8;
const int THEATERCHASE    = 9;
const int FROZEN          = 10;
const int COLLIDE         = 11;
const int DEMO            = 12;
const int COLORFADE       = 13;
const int WARMFADE        = 14;
const int CYCLECHASER     = 15;
const int COLORCHASE      = 16;
const int CHRISTMASTREE   = 17;
const int CHRISTMASWREATH = 18;
const int PLANTLIFE       = 19;
const int FLICKER         = 20;

const int EXAMPLE         = 99;

typedef struct modeParams
{
   int 	 modeId;
   char  modeName[20];      //Tell the android app what the mode title is
   int   numOfColors;       //Tell the android app home many colors to expect. Max number is 6
   int   numOfSwitches;     //Tell the android app home many switches to expect. Max number is 4
   bool  textInput;         //Tell the android app if a text input is needed
} modeParams;

const int MAX_NUM_COLORS = 6;
const int MAX_NUM_SWITCHES = 4;

/* ======================= ADD NEW MODE STRUCT HERE. ======================= */
//modeId and modeName should be the same name to prevent confusion
//Use this struct array to neatly organize and correlate Mode name with number of colors needed,
//  the number of switches needed, and whether a text input is needed or not
//The Android app uses these parameters to help populate the view 
//and to know how many colors and switches and text input to ask to update
modeParams modeStruct[] =
{ 
    /*     modeId           modeName 	   #Colors #Switches  textInput
     *     -------------    -------------  ------- ---------  ----- */
        {  OFF,             "OFF",             0,      0,      FALSE  },
        {  NORMAL,          "NORMAL",          0,      0,      FALSE  },
        {  COLORALL,        "COLORALL",        1,      0,      FALSE  },
        {  CHASER,          "CHASER",          1,      0,      FALSE  },
        {  ZONE,            "ZONE",            4,      0,      FALSE  },
        {  COLORWIPE,       "COLORWIPE",       1,      0,      FALSE  },
        {  CYCLEWIPE,       "CYCLEWIPE",       0,      0,      FALSE  },
        {  CYCLECHASER,     "CYCLECHASER",     0,      0,      FALSE  },
        {  COLORCHASE,      "COLORCHASE",      2,      0,      FALSE  },
        //{  STROBE, 	    "STROBE",          1,      0,      FALSE  }, //removed to prevent seizures
        {  RAINBOW,         "RAINBOW",         0,      0,      FALSE  },
        {  THEATERCHASE,    "THEATERCHASE",    0,      0,      FALSE  },
        {  FROZEN,          "FROZEN",          0,      0,      FALSE  },
	    {  COLLIDE,         "COLLIDE",         0,      0,      FALSE  },
	    {  COLORFADE,       "COLORFADE",       0,      0,      FALSE  },
        {  CHRISTMASTREE,   "CHRISTMASTREE",   0,      0,      FALSE  },
        {  CHRISTMASWREATH, "CHRISTMASWREATH", 0,      0,      FALSE  },
	    {  WARMFADE,        "WARMFADE",        0,      0,      FALSE  },
	    {  PLANTLIFE,       "PLANTLIFE",       0,      0,      FALSE  },
	    {  FLICKER,         "FLICKER",         0,      0,      FALSE  },
	   // {  EXAMPLE,         "EXAMPLE",         0,      0,      FALSE  },
	    {  DEMO,            "DEMO",            0,      0,      FALSE  }
};

typedef struct switchParams
{
   int 	 modeId;
   char  switch1Title[20];
   char  switch2Title[20];
   char  switch3Title[20];
   char  switch4Title[20];
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
	   // {  EXAMPLE,         "Random Colors",     "Matrix Mode",         "",                    ""  },  //Don't forget the comma here for more than one mode
	   // {  TEST,            "Switch Title 1",      "Switch Title 2",      "Switch Title 3",      "Switch Title 4"  }
};

//Preset speed constants
const int speedPresets[] = { 120, 100, 80, 70, 50, 30, 20, 10, 1};  //in mSec, slow to fast       

//Temp Sensor constants	
const double refVoltage = 3.3;    //Voltage used as reference for analogRead
const double MINFANTEMP = 100.0;  //Min Temp to turn fan on low speed in Deg Fahrenheit
const double MAXFANTEMP = 115.0;  //Max Temp to turn fan on max speed in Deg Fahrenheit
const double MAXTEMP = 140.0;     //Shut Off Temp in Deg Fahrenheit

//Time Interval constants           hh*mm*ss*ms    
unsigned long oneMinuteInterval =       1*60*1000;	//Read temp every minute
unsigned long oneHourInterval =      1*60*60*1000;  //auto off in 1 hr when night time
unsigned long oneDayInterval = 		24*60*60*1000;  //time sync interval - 24 hours

//Program Flags
int currentModeID;	//This is the ID of the current mode selected - used in the case statement
int run;    	//Use this for modes that don't need to loop. Set the color, then stop sending commands to the pixels
int stop;   	//Use this to break out of sequence loops when changing to a new mode

/*Should we shut off the lights at certain times? This is toggled from the app
  Configure the Auto Shut Off times in the loop() function */
bool autoShutOff;  

//Misc variables
int speed;	//not to be confused with speedIndex below, this is the local speed (delay) value
unsigned long previousMillis = 0;
unsigned long lastCommandReceived = 0;
unsigned long lastSync = 0;
uint32_t defaultColor;	//The NORMAL mode color
uint32_t color1;
uint32_t color2;
uint32_t color3;
uint32_t color4;
uint32_t color5;
uint32_t color6;
bool switch1;
bool switch2;
bool switch3;
bool switch4;
float redPrev;
float greenPrev;
float bluePrev;

//Spark Pin Defines
int FAN_PIN = A0;           //There is a fan in the project box in case it gets too hot in there
int TEMP_SENSOR_PIN = A7;   //TMP36 sensor on this pin.

//Spark Cloud Variables
int iwifi = 0;   //used for general info and setup
int tHour=0;    //used for general info and setup
int speedIndex;     				//Let the cloud know what speed preset we are using
int brightness;                     //How bright do we want these things anyway
double measuredTemperature = 0.0;   //Let's see how hot our project box is getting since is has the power supply in there
char modeNameList[MAX_PUBLISHED_STRING_SIZE] = "None";        //Holds all mode info comma delimited. Use this to populate the android app
char modeParamList[MAX_PUBLISHED_STRING_SIZE] = "None";
char currentModeName[64] = "None";  //Holds current selected mode
char textInputString[64];            //Holds the Text for any mode needing a test input - only useful for a Neopixel Matrix
char debug[200];                    //We might want some debug text for development
   
/* ======================= mode Specific Defines ======================= */
//ZONE mode Start and End Pixels
int zone1Start = 0;
int zone1End   = 59;
int zone2Start = 60;
int zone2End   = 141;
int zone3Start = 142;
int zone3End   = 219;
int zone4Start = 220;
int zone4End   = 267;

//CHASER mode specific Start and End Pixels, re-use some from ZONE mode
int ChaserZone3Section1End   = 177;
int chaserZone3Section2Start = 189;
#define CHASER_LENGTH   256

//Color Defines
#define RED   0xFF0000
#define GREEN 0x00FF00
#define PINK  0xFF00FF

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

//FROZEN mode defines
int randomFlakes[(int)(PIXEL_CNT*0.1)]; // holds the snowflake positions no more than10% of total number of pixels

/* ======================= Required Prototypes =============================== */
int setNewMode(int newMode);
int getModeIndexFromName(String name);
int getModeIndexFromID(int id);
int isValidMode(String newMode);
int getTemperature(void);
int showPixels(void);
void makeModeList(void);
uint32_t Wheel(byte WheelPos);

/* ======================= Spark Pixel Prototypes =============================== */
int colorAll(uint32_t c);
int colorZone(uint32_t c1, uint32_t c2, uint32_t c3, uint32_t c4);
int colorWipe(uint32_t c);
int colorChaser(uint32_t c);
int colorChase(uint32_t c1, uint32_t c2);
//void strobe(uint32_t color);
void rainbow(void);
void rainbowCycle(void) ;
void theaterChaseRainbow(void);
void cycleWipe(void);
void frozen(void);
void setRandomSnowFlakes(int numFlakes);
void findRandomSnowFlakesPositions(int numFlakes);
void collide(void);
void demo(void);
void arrayShuffle(int arrayToShuffle[], int arraySize);
void fade(void);
void cycleChaser(void);
void warmFade(void);
void christmasTree(void);
void christmasWreath(void);
void setZone1(uint32_t c);
void setZone2(uint32_t c);
void setZone3(uint32_t c);
void setZone4(uint32_t c);
uint8_t fadeSquare(float value);
uint8_t fadeSqRt(float value);
uint8_t fadeLinear(float value);
void flicker(void);
//int fadeIn(int delay);
//int fadeOut(int delay);

//Don't connect to the cloud first so we can turn on the lights right away
SYSTEM_MODE(SEMI_AUTOMATIC);   

void setup() {
    Particle.function("SetMode",       SetMode);            // Completely Necessary, Do Not Delete
    Particle.function("SetText",       SetText);            // Completely Necessary, Do Not Delete
    Particle.function("Function",      Function);           // Completely Necessary, Do Not Delete
    Particle.variable("hour",          tHour);              // Expendable   
    Particle.variable("wifi",          iwifi);              //// Expendable
    Particle.variable("speed",         speedIndex);         // Completely Necessary, Do Not Delete
    Particle.variable("brightness",    brightness);         // Completely Necessary, Do Not Delete
    Particle.variable("temp",          measuredTemperature);// Needed if "Show Control Box Temp" is selected in the app
	Particle.variable("modeList",      modeNameList);       // Completely Necessary, Do Not Delete
	Particle.variable("modeParmList",  modeParamList);      // Completely Necessary, Do Not Delete
	Particle.variable("mode",          currentModeName);    // Completely Necessary, Do Not Delete
	Particle.variable("textInput",     textInputString);    // Expendable
    Particle.variable("debug",         debug);              // Completely Necessary, Do Not Delete
    
    pinMode(TEMP_SENSOR_PIN,INPUT);
    pinMode(PIXEL_PIN, OUTPUT);
    pinMode(FAN_PIN, OUTPUT);
    
    analogWrite(FAN_PIN, 0); //Turn Fan off
  
	//Initialize
    color1 = color2 = color3 = color4 = color5 = color6 = 0;
    switch1 = switch2 = switch3 = switch4 = false;
    redPrev = greenPrev = bluePrev = 0;
    speedIndex = 5;
    brightness = 250;
    run = false;
    stop = false;
    autoShutOff = false;
	setNewMode(getModeIndexFromID(NORMAL));
	defaultColor = strip.Color(255,255,60);  // This seems close to incandescent color 
	lastSync = lastCommandReceived = previousMillis = millis();	//Take a time stamp
	
	strip.begin();     			//Start up the Neopixels     	
    colorAll(defaultColor);     //Turn on the NORMAL Mode
    Particle.connect();         //Now connect to the cloud
    Time.zone(TIME_ZONE_OFFSET);  //set time zone 
    
    //Clear the mode list variable
	sprintf(modeNameList,"");
	sprintf(modeParamList,"");
    //Assemble Spark Cloud available modes variable
    /*for(uint16_t i=0;i<sizeof modeStruct / sizeof modeStruct[0];i++) {
        char cBuff[20];
        sprintf(cBuff,"%s,%i,",modeStruct[i].modeName, modeStruct[i].numOfColors);
	    strcat(modeNameList,cBuff);
    } */
    makeModeList();

    
	getTemperature();
    tHour = Time.hour();	//used to check for correct time zone
    iwifi = WiFi.RSSI(); 
}

void makeModeList(void) {
    for(int i=0;i<sizeof modeStruct / sizeof modeStruct[0];i++) {
        char cNameBuff[20];
		char cParamBuff[60];
		//if((sizeof modeStruct[i].modeName)+1 <= modeNameCharCnt) {
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
    
    /*if(strlen(modeParamList) + textSize + 2 <= MAX_PUBLISHED_STRING_SIZE) {
        strcat(modeParamList,";");
    } else {
        char tempChar[MAX_PUBLISHED_STRING_SIZE];
        strcpy(tempChar,modeParamList);
        sprintf(modeParamList,"");
        strncat(modeParamList, tempChar, MAX_PUBLISHED_STRING_SIZE-1);
        strcat(modeParamList, ";");
    }*/
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

//delay (or speed) is passed 
void loop() {

    if(run)
    {
		stop = FALSE;
        if(currentModeID == DEMO) {
    	    demo();
        }
        else {
            runMode(currentModeID);
        }
    }
    
    unsigned long currentMillis = millis();
 
    if(currentMillis - previousMillis > oneMinuteInterval) {
        previousMillis = currentMillis;
        tHour = Time.hour();

        if(GET_TEMP_ENABLED) {
            getTemperature();
            if(measuredTemperature < MINFANTEMP) {
    			//Turn fan Off
                analogWrite(FAN_PIN, 0); 
            }
            else if(measuredTemperature > MINFANTEMP && measuredTemperature <  MAXFANTEMP) {
    			//Turn fan on at low speed
                analogWrite(FAN_PIN, 100); 
            }
            else if(measuredTemperature >  MAXFANTEMP) {
    			//Turn fan on at full speed
                analogWrite(FAN_PIN, 255); 
            }
            if(measuredTemperature > MAXTEMP) {
    			//Turn fan on at full speed and turn Off the lights
                analogWrite(FAN_PIN, 255); 
    			setNewMode(getModeIndexFromID(OFF));
            }  
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

int runMode(int modeID) {
    
    switch (modeID) {
        case OFF:
    	    colorAll(strip.Color(0,0,0));
    		break;
       case COLORALL:
    	    colorAll(color1);
    	    break;
        case CHASER:
    	    colorChaser(color1);
    		break;
    	case ZONE:
    	    colorZone(color1, color2, color3, color4); 
    	    break;
    	case COLORWIPE:
    	    colorWipe(color1);
    	    break;
    	case CYCLEWIPE:
    	    cycleWipe();
    	    break;    
    	case CYCLECHASER:    
    	    cycleChaser();
    	    break;
    	case COLORCHASE:    
    	    colorChase(color1, color2);
    	    break;
/*    	case STROBE:
    	    strobe(color1);
    	    break;*/    	
    	case RAINBOW:
    	    rainbowCycle();
    	    break;
    	case THEATERCHASE:
    	    theaterChaseRainbow();
    	    break;
    	case CHRISTMASTREE:
    	    christmasTree();
    	    break;
    	case FROZEN:
    	    frozen();
    	    break;
    	case COLLIDE:
    	    collide();
    	    break;
    	case COLORFADE:
    	    fade();
    	    break;
    	case WARMFADE:    
    	    warmFade();
    	    break;
    	case CHRISTMASWREATH:
    	    christmasWreath();
    	    break;
    	case PLANTLIFE: //There's plants under these lights
    	    colorAll(strip.Color(0,0,0));
    	    setZone3(PINK); 
    	    showPixels();
    	    break;
    	case FLICKER:
    	    flicker();
    	    break;
    	case NORMAL:
    	default:
		    colorAll(defaultColor);
    		break;
    }
    return 1;
}

void demo(void) {
    uint8_t mode;
    //Add the dynamic modes you want to cycle through in this array
    int modes2Cycle[] = {CYCLECHASER,COLORFADE,COLORWIPE,RAINBOW,CYCLEWIPE,THEATERCHASE,FROZEN,COLLIDE,COLORFADE,WARMFADE,CHRISTMASTREE,CHRISTMASWREATH};
    const unsigned long modeInterval =  2*60*1000; //Adjust the lifespan for each mode to perform
    unsigned long lastTime =  millis();
    arrayShuffle(modes2Cycle, sizeof modes2Cycle / sizeof modes2Cycle[0]);
    for(mode=0;mode<sizeof modes2Cycle / sizeof modes2Cycle[0];mode++) {
        color1 = Wheel(random(255));
        color2 = Wheel(random(255));
        color3 = Wheel(random(255));
        color4 = Wheel(random(255));
        //sprintf(debug,"DemoInsideLoop-ArraySize=%i-ModeIndex=%i-Mode=%i",sizeof modes2Cycle / sizeof modes2Cycle[0], mode,modes2Cycle[mode]);
        while(millis() - lastTime < modeInterval) {
            runMode(modes2Cycle[mode]);
            if(stop == TRUE) {return;}
        }
        lastTime = millis();
    }
    
    //sprintf(debug,"DemoOutOfLoop-Mode=%i", mode);
    run = true; //in case it's off, let's turn it back on
}

int getTemperature(void){
    double measuredVoltage = refVoltage * analogRead(TEMP_SENSOR_PIN) / 4095.0;
    measuredTemperature = (measuredVoltage - 0.5) * 100.0;          //Celcius Temp
    measuredTemperature = (measuredTemperature * 9.0 / 5.0) + 32.0; //Convert to Fahrenheit

    return 1;
}

void arrayShuffle(int arrayToShuffle[], int arraySize) {
    uint16_t i; 
    char cbuff[20];

    for(i=0;i<=arraySize;i++) {
        int r = random(0,arraySize);  // generate a random position
        int temp = arrayToShuffle[i]; 
        arrayToShuffle[i] = arrayToShuffle[r]; 
        arrayToShuffle[r] = temp;
        //sprintf(cbuff,"%i %i %s ", arraySize,r,modeStruct[getModeIndexFromID(arrayToShuffle[i])].modeName);
        //strcat(debug,cbuff);
    }
}

//Used in all modes to set the brightness, show the lights, process Spark events and delay
int showPixels(void) {
	strip.setBrightness(brightness);
    strip.show();
    Particle.process();    //process Spark events
	return 1;
}


// Set all pixels in the strip to a solid color
int colorAll(uint32_t c) {
    uint16_t i; 
    run = FALSE;
  
    for(i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, c);
    }
    showPixels();
    if(stop == TRUE) {return 0;}
 
    return 1;
}

//Turn off all pixels then run a single color down the strip 
//One pixel at a time
int colorChaser(uint32_t c) {
    uint16_t i;
    run = FALSE;
    
    //Turn Off all pixels
    for(i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, 0);
    }
    strip.show();

    //Forward
    for(i=zone1Start; i<=ChaserZone3Section1End; i++) {
        strip.setPixelColor(i, c);
        if(i > zone1Start) {
            strip.setPixelColor(i-1, 0);
        }
        showPixels();
        if(stop == TRUE) {return 0;}
		delay(speed);
    }
    strip.setPixelColor(ChaserZone3Section1End, 0);
    for(i=zone4Start; i<=zone4End; i++) {
        strip.setPixelColor(i, c);
        strip.setPixelColor(i-1, 0);
        showPixels();
        if(stop == TRUE) {return 0;}
		delay(speed);
    }
    strip.setPixelColor(zone4End, 0);
    for(i=chaserZone3Section2Start; i<=zone3End; i++) {
        strip.setPixelColor(i, c);
        strip.setPixelColor(i-1, 0);
        showPixels();
        if(stop == TRUE) {return 0;}
		delay(speed);
    }
	
    //Reverse
    strip.setPixelColor(zone3End, 0);
	for(i=zone3End-1; i>=chaserZone3Section2Start; i--) {
        strip.setPixelColor(i, c);
        strip.setPixelColor(i+1, 0);
        showPixels();
        if(stop == TRUE) {return 0;}
		delay(speed);
    }
    strip.setPixelColor(chaserZone3Section2Start, 0);
    for(i=zone4End; i>=zone4Start; i--) {
        strip.setPixelColor(i, c);
        strip.setPixelColor(i+1, 0);
        showPixels();
        if(stop == TRUE) {return 0;}
		delay(speed);
    }
    strip.setPixelColor(zone4Start, 0);
    for(i=ChaserZone3Section1End; i>zone1Start; i--) {
        strip.setPixelColor(i, c);
        strip.setPixelColor(i+1, 0);
        showPixels();
        if(stop == TRUE) {return 0;}
		delay(speed);
    }
    return 1;
}

//Similar to THEATERCHASE but with only 2 selected colors 
//that alternate back and forth
int colorChase(uint32_t c1, uint32_t c2) {
    run = true;
    /* The number of pixels to turn on of each color in a row
    * i.e. pixNum=3 will turn the first 3 pixels c1 and then
    * then next 3 pixels c2, and the next 3 pixels c1, etc.  */
    uint8_t pixNum = 3; 
    uint16_t i;
    int speedfactor = 3;    //increase the delay time
    for (i=0; i < strip.numPixels(); i++) { 
      if(i%(pixNum*2)>=0 && i%(pixNum*2)<=(pixNum-1)) {
        strip.setPixelColor(i, c1 );
      }
      if(i%(pixNum*2)>=pixNum && i%(pixNum*2)<=((pixNum*2)-1)) {
        strip.setPixelColor(i, c2 );
      }
    }
    showPixels();
    if(stop == TRUE) {return 0;}
	delay(speed*speedfactor);
    for (i=0; i < strip.numPixels(); i++) {
      if(i%(pixNum*2)>=0 && i%(pixNum*2)<=(pixNum-1)) {
        strip.setPixelColor(i, c2 );
      }
      if(i%(pixNum*2)>=pixNum && i%(pixNum*2)<=((pixNum*2)-1)) {
        strip.setPixelColor(i, c1 );
      }
    }
    showPixels();
    if(stop == TRUE) {return 0;}
	delay(speed*speedfactor);
	return 1;
}

//A colored Christmas light string that twinkles
void christmasTree(void) {
    uint16_t i, f;
    run = true;
    int speedfactor = 12;    //increase the delay time
    for (f=0; f < 6; f++) {
        for (i=0; i < strip.numPixels(); i++) {
          if(i%6==0) {
            strip.setPixelColor(i, 255,0,0 );
          }
          if(i%6==1) {
            strip.setPixelColor(i, 255,255,0 );
          }
          if(i%6==2) {
            strip.setPixelColor(i, 0,255,0 );
          }
          if(i%6==3) {
            strip.setPixelColor(i, 0,255,255 );
          }
          if(i%6==4) {
            strip.setPixelColor(i, 0,0,255 );
          }
          if(i%6==5) {
            strip.setPixelColor(i, 255,0,255 );
          }
        }
        showPixels();
        if(stop == TRUE) {return;}
        delay(speed*speedfactor);
        for (int i=0; i < strip.numPixels(); i++) {
            if(i%6==f) {
                strip.setPixelColor(i, 0,0,0 );
            }
        }
        showPixels();
        delay(speed*speedfactor);
        if(stop == TRUE) {return;}
    }
}

/* I have my neopixles installed under my kitchen cabinets.
*  I have 4 specific cabinets (zones) and each cabinet can be 
*  a different color. Feel free to expand the number of zones.
*  the Android app supports a max of 6 zones
*/
int colorZone(uint32_t c1, uint32_t c2, uint32_t c3, uint32_t c4) {
    run = FALSE;
    
	setZone1(c1);
	setZone2(c2);
	setZone3(c3);
	setZone4(c4);
    showPixels();
    return 1;
}

void setZone1(uint32_t c) {
    uint16_t i;
    run = FALSE;
    
    for(i=zone1Start; i<=zone1End; i++) {
        strip.setPixelColor(i, c);
	}
}

void setZone2(uint32_t c) {
    uint16_t i;
    run = FALSE;
    
    for(i=zone2Start; i<=zone2End; i++) {
		strip.setPixelColor(i, c);
	}
}

void setZone3(uint32_t c) {
    uint16_t i;
    run = FALSE;
    
    for(i=zone3Start; i<=zone3End; i++) {
		strip.setPixelColor(i, c);
	}
}

void setZone4(uint32_t c) {
    uint16_t i;
    run = FALSE;
    
    for(i=zone4Start; i<=zone4End; i++) {
        strip.setPixelColor(i, c);
	}
}


void christmasWreath(void) {
    double i;
    int speedFactor = 16;    //increase the delay time
    int currentBrightness = brightness;
    /*
    setZone1(RED); 
    setZone2(GREEN);
    setZone3(RED);
    setZone4(GREEN);
    if(fadeIn(speed*speedFactor) == 0) {return 0;}
    delay(speed*speedFactor);
    if(fadeOut(speed*speedFactor) == 0) {return 0;}
    setZone1(GREEN); 
    setZone2(RED);
    setZone3(GREEN);
    setZone4(RED);
    if(fadeIn(speed*speedFactor) == 0) {return 0;}
    delay(speed*speedFactor);
    if(fadeOut(speed*speedFactor) == 0) {return 0;}
*/    
    //fade in
    for (i=0; i < currentBrightness; i++) {
    //for (i=4.712; i < 10.995*currentBrightness/255; i+=0.001) {
        setZone1(RED); 
    	setZone2(GREEN);
    	setZone3(RED);
    	setZone4(GREEN);
    	brightness = i; // sin(i) * 127.5 + 127.5;
    	showPixels();
    	if(stop == TRUE) {return;}
    	delay(speedFactor*speed);
    }

    //fade out
	for (i=currentBrightness; i > 0; i-=2) {
        setZone1(RED); 
    	setZone2(GREEN);
    	setZone3(RED);
    	setZone4(GREEN);
    	brightness = i;
    	showPixels();
    	if(stop == TRUE) {return;}
    	delay(speedFactor*speed);
    }
    
    //alternator colors, fade in
    for (i=0; i < currentBrightness; i++) {
    //for (i=4.712; i < 10.995*currentBrightness/255; i+=0.001) {
        setZone1(GREEN); 
    	setZone2(RED);
    	setZone3(GREEN);
    	setZone4(RED);
    	brightness = i; // sin(i) * 127.5 + 127.5;
    	showPixels();
    	if(stop == TRUE) {return;}
    	delay(speedFactor*speed);
     }
    showPixels();
    
    //fade out
	for (i=currentBrightness; i > 0; i-=2) {
        setZone1(GREEN); 
    	setZone2(RED);
    	setZone3(GREEN);
    	setZone4(RED);
    	brightness = i;
    	showPixels();
    	if(stop == TRUE) {return;}
    	delay(speedFactor*speed);
    }

    brightness = currentBrightness;
	run = true;
}

void flicker(void) {
    uint16_t j;
    int flickerBrightness = random(127, 255);
    int flickerTime = random(1, 50);
    float flickerScale = (float)flickerBrightness / 255;
    float red = flickerScale * 255;
    float green = flickerScale * 80;
    float blue = flickerScale * 0;

   for (float i = 0; i < flickerTime; i++) {
        for (j=0; j < strip.numPixels(); j++) {
            strip.setPixelColor(j, (byte)(redPrev + (red-redPrev)/flickerTime*i), (byte)(greenPrev + (green-greenPrev)/flickerTime*i), (byte)(bluePrev + (blue-bluePrev)/flickerTime*i));
        }
        showPixels();
        if(stop == TRUE) {return;}
        delay(1);
    }
    redPrev = red;
    greenPrev = green;
    bluePrev = blue;
}

/*
int fadeIn(int fadeSpeed) {
    uint16_t i, j;
    run = true;
    int currentBrightness = brightness; //get a copy of the current brightness
    uint32_t initialPixelColor[strip.numPixels()];
    
    for (i=0; i < strip.numPixels(); i++) {
        initialPixelColor[i] = strip.getPixelColor(i);
    }
    
    for (j=0; j < currentBrightness; j+=5) {
        brightness = j;  //Use brightness variable to fade in
        showPixels();    //sets the brightness and then shows the pixels
    	if(stop == TRUE) {return 0;}
    	delay(fadeSpeed);
    	for (i=0; i < strip.numPixels(); i++) {
            strip.setPixelColor(i, initialPixelColor[i]);
    	}
    }
        
    brightness = currentBrightness;
    return 1;
}

int fadeOut(int fadeSpeed) {
    uint16_t i;
    run = true;
    int currentBrightness = brightness;
    
    for (i=currentBrightness; i > 0; i-=5) {
        brightness = i;
        showPixels();
    	if(stop == TRUE) {return 0;}
    	delay(fadeSpeed);
    }
    brightness = currentBrightness;
    return 1;
}
*/
//Fill the dots one after the other with a color, wait (ms) after each one
int colorWipe(uint32_t c) {
    uint16_t i;
    run = FALSE;
  
    for(i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, c);
        showPixels();
        if(stop == TRUE) {return 0;}
		delay(speed);
    }
    return 1;
}

//use colorWipe to cycle through all the colors 
void cycleWipe(void) {
    uint16_t j;
    int returnValue=1;

    for(j=0; j<256; j+=15) {
        if(j%2==0) {
            returnValue = colorWipe(Wheel(j & 0xFF));
        }
        else {
            returnValue = colorWipe(Wheel((j+80) & 0xFF));
        }
        if(stop == TRUE || returnValue == 0) {return;}
		delay(speed);
    }
    //colorWipe turns off the run loop, so let's turn it back on
    run = true; 
}

//Dance Party Techno??
/*
void strobe(uint32_t color) {
    int returnValue=1;

    returnValue = colorAll(color);				//Turn On all Pixels
    if(stop == TRUE || returnValue == 0) {return;}
	delay(speed);
    returnValue = colorAll(strip.Color(0,0,0));	//Turn Off all Pixels
    if(stop == TRUE || returnValue == 0) {return;}
	delay(speed);
    run = true;
}*/

void rainbow(void) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    showPixels();
    if(stop == TRUE) {return;}
	delay(speed);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout, then wait (ms)
void rainbowCycle(void) {
  uint16_t i, j;

  for(j=0; j<256; j++) { // 1 cycle of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    showPixels();
    if(stop == TRUE) {return;}
	delay(speed);
  }
}

//Theatre-style crawling lights with rai j, q, i;nbow effect
void theaterChaseRainbow(void) {
    uint16_t j, q, i;
  for (j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (q=0; q < 3; q++) {
        for (i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        showPixels();
        if(stop == TRUE) {return;}
		delay(speed);
        for (i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

//Fade from teal to blue to purple then back to blue then teal, repeat
//Random snowflakes twinkle white for random amounts of time 
//The snowflakes will hang around no faster than MIN_DELAY and no slower than MAX_DELAY
void frozen(void) {
  uint16_t i, j;
  uint16_t startColor = 140;    //Hue for teal
  uint16_t stopColor  = 210;    //Hue for purple
  const int MIN_DELAY = 400;	//in ms
  const int MAX_DELAY = 700;	//in ms
  const int MIN_FLAKES = strip.numPixels()*0.01;	//  1% of total number of pixels
  const int MAX_FLAKES = strip.numPixels()*0.1;	// 10% of total number of pixels
  int numSnowFlakes = random(MIN_FLAKES, MAX_FLAKES);	//How many flakes should we have at a time
  unsigned long previousMillis = millis();
  unsigned long flakeLifeSpan = random(MIN_DELAY, MAX_DELAY); //How long will a snowflake last

  //forwards
  for(j=startColor; j<stopColor; j++) { 	//cycle through the colors
    for(i=0; i< strip.numPixels(); i++) {	
        strip.setPixelColor(i, Wheel(j));	//first set all pixels to the same color
        if(millis() - previousMillis > flakeLifeSpan) {
			previousMillis = millis();						//reset time interval
			flakeLifeSpan = random(MIN_DELAY, MAX_DELAY);	//set new lifespan	
			numSnowFlakes = random(MIN_FLAKES, MAX_FLAKES); //set new number of flakes
            findRandomSnowFlakesPositions(numSnowFlakes);	//get the list of flake positions
        }
    }
	setRandomSnowFlakes(numSnowFlakes);	//now set the snowflake positions
	showPixels();
    if(stop == TRUE) {return;}
	delay(speed);
  }
  
  //backwards
  for(j=stopColor; j>startColor; j--) { 	//cycle through the colors
    for(i=0; i< strip.numPixels(); i++) {
        strip.setPixelColor(i, Wheel(j));	//first set all pixels to the same color
        if(millis() - previousMillis > flakeLifeSpan) {
			previousMillis = millis();						//reset time interval
			flakeLifeSpan = random(MIN_DELAY, MAX_DELAY);	//set new lifespan	
			numSnowFlakes = random(MIN_FLAKES, MAX_FLAKES); //set new number of flakes
            findRandomSnowFlakesPositions(numSnowFlakes);	//get the list of flake positions
        }
    }
	setRandomSnowFlakes(numSnowFlakes);	//now set the snowflake positions
    showPixels();
    if(stop == TRUE) {return;}
	delay(speed);
  }
}

void setRandomSnowFlakes(int numFlakes)
{
    for(uint16_t i=0;i<numFlakes;i++) {
        strip.setPixelColor(randomFlakes[i], 0xFFFFFF); //Set snowflake
    }
}

void findRandomSnowFlakesPositions(int numFlakes)
{
    for(uint16_t i=0;i<numFlakes;i++) {
        randomFlakes[i] = random(strip.numPixels());
        if(i > 0) {
            for(int j=0;j<i;j++) {
				//make sure we didn't already set that position
                if(randomFlakes[i] == randomFlakes[j]){
                    do {
                        randomFlakes[i] = random(strip.numPixels());
                    }while(randomFlakes[i] != randomFlakes[j]);
                }
            }
        }
    }
}

//Color Blender
//Red starts at 1 end and green at the other. They increment towards each other. 
//When they meet, they eplode into the combined color
void collide(void) {
	uint16_t i, j, color1, color2;
	//uint16_t startColor = 0;    
	
	for(color1=0; color1<=255; color1+=85) {
	//for(color1=0; color1<=255; color1+=170) {
	    for(color2=color1+42; color2<=color1+85; color2+=43) {
	    //for(color2=color1+85; color2<=color1+170; color2+=85) {
    		for(i=0; i<=strip.numPixels()/2; i++) { 
    			if(i*2 >= strip.numPixels()) {
    				//Explode the colors
    				for(j=0; j<strip.numPixels(); j++) {
    					strip.setPixelColor(j, Wheel((((color2-color1)/2)+color1)&0xFF));
    					if(j%5==0)
    					    showPixels();
    				}
    				showPixels();
    				if(stop == TRUE) {return;}
					delay(speed);
    				break;
    			}
    			else {
    				//Grow the two colors from either end of the strip
    				strip.setPixelColor(i, Wheel(color1));
    				strip.setPixelColor(strip.numPixels()-i, Wheel((color2)&0xFF));
    				showPixels();
    				if(stop == TRUE) {return;}
					delay(speed);
    			}
    		}
	    }
	}
}

/* Should probably break this into two functions, but here it is as is
*  Cycle through the major color hues and fade each color in and out.
*  Fade In = Start dim and get brighter
*  Fade Out = Start bright and get dimmer
*/
void fade(void) {
    uint16_t i, j;
    uint16_t brightnessStep = 5;
    int returnValue = 1;
    int originalBrightness = brightness;    //store the original brightness

    //pick the color
    for(j=0; j<256; j+=15) {
        //Fade In
        for(i=0; i<256; i+=brightnessStep) {
            brightness = i;
            if(j%2==0) {
                returnValue = colorAll(Wheel(j & 0xFF));
            }
            else {
                returnValue = colorAll(Wheel((j+80) & 0xFF));
            }
            if(stop == TRUE || returnValue == 0) {return;}
    		delay(speed);
        }
         //Fade Out
        for(i=255-brightnessStep; i>0; i-=brightnessStep) {
            brightness = i;
            if(j%2==0) {
                returnValue = colorAll(Wheel(j & 0xFF));
            }
            else {
                returnValue = colorAll(Wheel((j+80) & 0xFF));
            }
            if(stop == TRUE || returnValue == 0) {return;}
    		delay(speed);
        }
    }
    brightness = originalBrightness;
    run = true; //in case it's off, let's turn it back on
}

// Lets use the colorChaser mode and cycle through all of the major color hues
void cycleChaser(void) {
    uint16_t j;
    int returnValue = 1;

    //pick the color
    for(j=0; j<256; j+=15) {
        if(j%2==0) {
            returnValue = colorChaser(Wheel(j & 0xFF));
        }
        else {
            returnValue = colorChaser(Wheel((j+80) & 0xFF));
        }
        if(stop == TRUE || returnValue == 0) {return;}
		delay(speed);
    }
    run = true; //in case it's off, let's turn it back on
}


/* Inspired by my musically inclined work neighbor
*  This mode simulates halogen stage lights turning On and then Off
*  They start off redish-orange and warm up to bright white
*/
void warmFade(void) {
    float i; 
    int returnValue = 1;
    
    for(i=0; i<256; i++) {
        returnValue = colorAll(strip.Color(fadeSqRt(i),fadeLinear(i),fadeSquare(i)));
        if(stop == TRUE || returnValue == 0) {return;}
        delay(speed);
    }
    for(i=255; i>0; i--) {
        returnValue = colorAll(strip.Color(fadeSqRt(i),fadeLinear(i),fadeSquare(i)));
        if(stop == TRUE || returnValue == 0) {return;}
        delay(speed);
    }
   run = true; //in case it's off, let's turn it back on
}

// warmFade helper function
uint8_t fadeSquare(float value) {
    return (uint8_t)(255*pow(value/255,2));
}

// warmFade helper function
uint8_t fadeSqRt(float value) {
    return (uint8_t)(255*sqrt(value/255));
}

// warmFade helper function
uint8_t fadeLinear(float value) {
    return (uint8_t) value;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}


// ************************************************************************************
// You must **NEVER** change any values below this line!
// ************************************************************************************

//Spark Cloud Mode
//Expect a string like thisto change the mode Mode: M:ZONE,D:30,B:120,C1:002BFF,C2:FF00DB,C3:FF4600,C4:23FF00,
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
    
    // Trim extra spaces
    command.trim();
    // Convert it to upper-case for easier matching
    command.toUpperCase();
    
    //keep track or the last command received for the auto off feature
    lastCommandReceived = millis();
    
   // sprintf(debug,"%s", command.c_str());

	while(idx != -1) {
		if(command.charAt(beginIdx) == 'M') {   //Mode name
			//set the new mode from modeStruct array index
			returnValue = setNewMode(getModeIndexFromName(command.substring(beginIdx+2, idx).c_str())); 
			isNewMode = true;
			 sprintf(debug,"Mode Idx %i. Mode %s", returnValue, command.substring(beginIdx+2, idx).c_str());
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
            switch(command.charAt(beginIdx+1)) {
                case '1':
                    switch1 = command.substring(beginIdx+3, idx).toInt() & 1;
                    break;
                case '2':
                    switch2 = command.substring(beginIdx+3, idx).toInt() & 1;
                    break;
                case '3':
                    switch3 = command.substring(beginIdx+3, idx).toInt() & 1;
                    break;
                case '4':
                    switch4 = command.substring(beginIdx+3, idx).toInt() & 1;
                    break;
            }
		}

		beginIdx = idx + 1;
		idx = command.indexOf(',', beginIdx);
	}
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

int Function(String command) {
    int returnValue = 0;
    if(command == ASO_CMD_ON) {
        autoShutOff = true;
        returnValue = ASO_STATUS_ON;
    }
    else if(command == ASO_CMD_OFF) {
        autoShutOff = false;
        returnValue = ASO_STATUS_OFF;
    }else if(command == ASO_CMD_STATUS) {
        if(autoShutOff == false) {
            returnValue = ASO_STATUS_OFF;
        } else {
            returnValue = ASO_STATUS_ON;
        }
    }
    
    return returnValue;  
 }

//Change Mode based on the modeStruct array index
int setNewMode(int newModeIndex) {
    currentModeID = modeStruct[newModeIndex].modeId;
    sprintf(currentModeName,"%s", modeStruct[newModeIndex].modeName);
	return newModeIndex;
}

int getModeIndexFromName(String name)
{
    for(uint16_t i=0;i<(int)(sizeof modeStruct / sizeof modeStruct[0]);i++) {
        if(name.equals(String(modeStruct[i].modeName))) {
            return i;
        }
    }
    return -1;
}

int getModeIndexFromID(int id)
{
    for(uint16_t i=0;i<(int)(sizeof modeStruct / sizeof modeStruct[0]);i++) {
        if(id == modeStruct[i].modeId) {
            return i;
        }
    }
    return -1;
}

