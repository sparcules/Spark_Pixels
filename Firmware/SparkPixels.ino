/**
 ******************************************************************************
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

//NEOPIXEL Defines
#define PIXEL_CNT 268
#define PIXEL_PIN D7
#define PIXEL_TYPE WS2812B

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_CNT, PIXEL_PIN, PIXEL_TYPE);

/* ======================= ADD NEW MODE ID HERE. ======================= */
// Mode ID Defines
const int OFF 			= 0;
const int NORMAL 		= 1;
const int COLORALL 		= 2;
const int CHASER		= 3;
const int ZONE			= 4;
const int COLORWIPE 	= 5;
const int CYCLEWIPE		= 6;
const int STROBE		= 7;
const int RAINBOW		= 8;
const int THEATERCHASE	= 9;
const int FROZEN		= 10;
const int COLLIDE		= 11;
 
typedef struct modeParams
{
   int 	 modeId;
   char  modeName[64];
   int   numOfColors;       //Tell the android app home many colors to expect. Max number is 6
} modeParams;

/* ======================= ADD NEW MODE STRUCT HERE. ======================= */
//modeId and modeName should be the same name to prevent confusion
//Use this struct array to neatly organize and correlate Mode name with number of colors needed
//The Android app uses numOfColors to help populate the view 
//and to know how many colors to ask to update
modeParams modeStruct[] =
{
    /*     modeId       	modeName 		numOfColors
     *     ------------- 	-------------	---- */
        {  OFF,             "OFF",          0   },
        {  NORMAL,          "NORMAL",       0   },
        {  COLORALL,        "COLORALL",     1   },
        {  CHASER,          "CHASER",       1   },
        {  ZONE,            "ZONE",         4   },
        {  COLORWIPE,       "COLORWIPE",    1   },
        {  CYCLEWIPE,       "CYCLEWIPE",    0   },
        {  STROBE,          "STROBE",       1   },
        {  RAINBOW,         "RAINBOW",      0   },
        {  THEATERCHASE,    "THEATERCHASE", 0   },
        {  FROZEN,          "FROZEN",       0   },
        {  COLLIDE,         "COLLIDE",      0   }
};

//Preset speed constants
const int speedPresets[] = { 120, 100, 80, 70, 50, 30, 20, 10, 1};  //in mSec, slow to fast       

//Temp Sensor constants	
double refVoltage = 3.3;    //Voltage used as reference for analogRead
double MINFANTEMP = 100.0;  //Min Temp to turn fan on low speed in Deg Fahrenheit
double MAXFANTEMP = 115.0;  //Max Temp to turn fan on max speed in Deg Fahrenheit
double MAXTEMP = 140.0;     //Shut Off Temp in Deg Fahrenheit

//Time Interval constants               hh*mm*ss*ms    
unsigned long oneMinuteInterval =     1*60*1000;	//Read temp every minute
unsigned long oneHourInterval =      1*60*60*1000;  //auto off in 1 hr when night time
unsigned long oneDayInterval = 		24*60*60*1000;  //time sync interval - 24 hours

//Program Flags
int currentModeID;	//This is the ID of the current mode selected - used in the case statement
int run;    	//Use this for modes that don't need to loop. Set the color, then stop sending commands to the pixels
int stop;   	//Use this to break out of sequence loops when changing to a new mode

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

//Spark Pin Defines
int FAN_PIN = A0;           //There is a fan in the project box in case it gets too hot in thee=re
int TEMP_SENSOR_PIN = A7;   //TMP36 sensor on this pin.

//Spark Cloud Variables
int wifi = 0;   //used for general info and setup
int tHour=0;    //used for general info and setup
int speedIndex;     				//Let the cloud know what speed preset we are using
int brightness;                     //How bright do we want these things anyway
double measuredTemperature = 0.0;   //Let's see how hot our project box is getting since is has the power supply in there
char modeList[622] = "None";	//Holds all mode info comma delimited. Use this to populate the android app
char currentModeName[64] = "None";  //Holds current selected mode
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

//FROZEN mode defines
int randomFlakes[(int)(PIXEL_CNT*0.1)]; // holds the snowflake positions no more than10% of total number of pixels

/* ======================= Required Prototypes =============================== */
int setNewMode(int newMode);
int getModeIndexFromName(String name);
int getModeIndexFromID(int id);
int isValidMode(String newMode);
int getTemperature(void);
int showPixels(void);
uint32_t Wheel(byte WheelPos);

/* ======================= Spark Pixel Prototypes =============================== */
int colorAll(uint32_t c);
int colorZone(uint32_t c1, uint32_t c2, uint32_t c3, uint32_t c4);
int colorWipe(uint32_t c);
void colorChaser(uint32_t c);
void strobe(uint32_t color);
void rainbow(void);
void rainbowCycle(void) ;
void theaterChaseRainbow(void);
void cycleWipe(void);
void frozen(void);
void setRandomSnowFlakes(int numFlakes);
void findRandomSnowFlakesPositions(int numFlakes);
void collide(void);

//Don't connect to the cloud first so we can turn on the lights right away
SYSTEM_MODE(SEMI_AUTOMATIC);   

void setup() {
    Spark.function("SetMode", SetMode);
    Spark.variable("wifi",          &wifi,                  INT);
    Spark.variable("tHour",         &tHour,                 INT);
    Spark.variable("speed",         &speedIndex,            INT);
    Spark.variable("brightness",    &brightness,            INT);
    Spark.variable("temp",          &measuredTemperature,   DOUBLE);
	Spark.variable("modeList",      &modeList,              STRING);
	Spark.variable("mode",          &currentModeName,       STRING);
    Spark.variable("debug",         &debug,                 STRING);
    
    pinMode(TEMP_SENSOR_PIN,INPUT);
    pinMode(PIXEL_PIN, OUTPUT);
    pinMode(FAN_PIN, OUTPUT);
    
    analogWrite(FAN_PIN, 0); //Turn Fan off
  
	//Initialize
    color1 = 0;
    color2 = 0;
    color3 = 0;
    color4 = 0;
    speedIndex = 5;
    brightness = 250;
    run = FALSE;
    stop = FALSE;
	setNewMode(getModeIndexFromID(NORMAL));
	defaultColor = strip.Color(255,255,60);  // This seems close to incandescent color 
	lastSync = lastCommandReceived = previousMillis = millis();	//Take a time stamp
	
	strip.begin();     			//Start up the Neopixels     	
    colorAll(defaultColor);     //Turn on the NORMAL Mode
    Spark.connect();            //Now connect to the cloud
    Time.zone(-5);              //set time zone 
    
    //Clear the mode list variable
	sprintf(modeList,"");
    //Assemble Spark Cloud available modes variable
    for(uint16_t i=0;i<sizeof modeStruct / sizeof modeStruct[0];i++) {
        char cBuff[20];
        sprintf(cBuff,"%s,%i,",modeStruct[i].modeName, modeStruct[i].numOfColors);
	    strcat(modeList,cBuff);
    } 

	getTemperature();
    tHour = Time.hour();	//used to check for correct time zone
}

//delay (or speed) is passed 
void loop() {

    if(run)
    {
		stop = FALSE;
        switch (currentModeID) {
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
    		case STROBE:
    		    strobe(color1);
    		    break;    	
    		case RAINBOW:
    		    rainbowCycle();
    		    break;
    		case THEATERCHASE:
    		    theaterChaseRainbow();
    		    break;
    		case FROZEN:
    		    frozen();
    		    break;
    		case COLLIDE:
    		    collide();
    		    break;
    		case NORMAL:
    		default:
		        colorAll(defaultColor);
    			break;
        }
    }
    
    unsigned long currentMillis = millis();
 
    if(currentMillis - previousMillis > oneMinuteInterval) {
        previousMillis = currentMillis;
        getTemperature();
        tHour = Time.hour();

        if(measuredTemperature < MINFANTEMP) {
			//Turn fan Off
            analogWrite(FAN_PIN, 0); 
            //Spark.publish("Fan", "OFF", 36000, PRIVATE);
            //Spark.publish("Fan", "Off");
            //Spark.publish("Fan Off", NULL, 36000, PRIVATE);
        }
        else if(measuredTemperature > MINFANTEMP && measuredTemperature <  MAXFANTEMP) {
			//Turn fan on at low speed
            analogWrite(FAN_PIN, 100); 
            //Spark.publish("Fan", "Low", 36000, PRIVATE);
            //Spark.publish("Fan", "Low");
            //Spark.publish("Fan Low", NULL, 36000, PRIVATE);
        }
        else if(measuredTemperature >  MAXFANTEMP) {
			//Turn fan on at full speed
            analogWrite(FAN_PIN, 255); 
            //Spark.publish("Fan", "High", 36000, PRIVATE);
            //Spark.publish("Fan", "High");
           // Spark.publish("Fan High", NULL, 36000, PRIVATE);
        }
        if(measuredTemperature > MAXTEMP) {
			//Turn fan on at full speed and turn Off the lights
            analogWrite(FAN_PIN, 255); 
			setNewMode(getModeIndexFromID(OFF));
        }  
    //Put other timing stuff in here to speed up main loop
        if (currentMillis - lastSync > oneDayInterval) {
            // Request time time synchronization from the Spark Cloud
            sprintf(debug,"Last sync time = %i,", (int)(currentMillis - lastSync));
			Spark.syncTime();
            lastSync = currentMillis;
        }
        if (currentMillis - lastCommandReceived > oneHourInterval) {
            //Auto Off Criteria
            //If it's Monday through Friday between 8am and 4pm or between 10pm and 5am any day, turn Off the lights
            if(((Time.weekday() >= 2 && Time.weekday() <=6) && (Time.hour() >= 8 && Time.hour() <= 16)) || (Time.hour() >= 22) || (Time.hour() <= 5)) {
                //No one is home or everyone is sleeping. So shut it down
				sprintf(debug,"Last auto Off time = %i,", (int)(currentMillis - lastCommandReceived));
				lastCommandReceived = currentMillis;
				wifi = WiFi.RSSI();
				setNewMode(getModeIndexFromID(OFF));
                run = TRUE;
            }
        }
    }
}

int getTemperature(void){
    double measuredVoltage = refVoltage * analogRead(TEMP_SENSOR_PIN) / 4095.0;
    measuredTemperature = (measuredVoltage - 0.5) * 100.0;          //Celcius Temp
    measuredTemperature = (measuredTemperature * 9.0 / 5.0) + 32.0; //Convert to Fahrenheit

    return 1;
}

//Used in all modes to set the brightness, show the lights, process Spark events and delay
int showPixels(void) {
	strip.setBrightness(brightness);
    strip.show();
    Spark.process();    //process Spark events
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

void colorChaser(uint32_t c) {
    uint16_t i;
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
        if(stop == TRUE) {return;}
		delay(speed);
    }
    strip.setPixelColor(ChaserZone3Section1End, 0);
    for(i=zone4Start; i<=zone4End; i++) {
        strip.setPixelColor(i, c);
        strip.setPixelColor(i-1, 0);
        showPixels();
        if(stop == TRUE) {return;}
		delay(speed);
    }
    strip.setPixelColor(zone4End, 0);
    for(i=chaserZone3Section2Start; i<=zone3End; i++) {
        strip.setPixelColor(i, c);
        strip.setPixelColor(i-1, 0);
        showPixels();
        if(stop == TRUE) {return;}
		delay(speed);
    }
	
    //Reverse
    strip.setPixelColor(zone3End, 0);
	for(i=zone3End-1; i>=chaserZone3Section2Start; i--) {
        strip.setPixelColor(i, c);
        strip.setPixelColor(i+1, 0);
        showPixels();
        if(stop == TRUE) {return;}
		delay(speed);
    }
    strip.setPixelColor(chaserZone3Section2Start, 0);
    for(i=zone4End; i>=zone4Start; i--) {
        strip.setPixelColor(i, c);
        strip.setPixelColor(i+1, 0);
        showPixels();
        if(stop == TRUE) {return;}
		delay(speed);
    }
    strip.setPixelColor(zone4Start, 0);
    for(i=ChaserZone3Section1End; i>zone1Start; i--) {
        strip.setPixelColor(i, c);
        strip.setPixelColor(i+1, 0);
        showPixels();
        if(stop == TRUE) {return;}
		delay(speed);
    }
}

int colorZone(uint32_t c1, uint32_t c2, uint32_t c3, uint32_t c4) {
    uint16_t i;
    run = FALSE;
    
    for(i=0; i<strip.numPixels(); i++) {
        if(i>=zone1Start && i<=zone1End) 
	        strip.setPixelColor(i, c1);
		else if(i>=zone2Start && i<=zone2End)
			strip.setPixelColor(i, c2);
		else if(i>=zone3Start && i<=zone3End)
			strip.setPixelColor(i, c3);
		else if(i>=zone4Start && i<=zone4End)
		    strip.setPixelColor(i, c4);
	}
    showPixels();
    return 1;
}

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

void strobe(uint32_t color) {
    int returnValue=1;

    returnValue = colorAll(color);				//Turn On all Pixels
    if(stop == TRUE || returnValue == 0) {return;}
	delay(speed);
    returnValue = colorAll(strip.Color(0,0,0));	//Turn Off all Pixels
    if(stop == TRUE || returnValue == 0) {return;}
	delay(speed);
    run = true;
}

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

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(void) {
  for (uint16_t j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (uint16_t q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        showPixels();
        if(stop == TRUE) {return;}
		delay(speed);
        for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
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
    for(int i=0;i<numFlakes;i++) {
        strip.setPixelColor(randomFlakes[i], 0xFFFFFF); //Set snowflake
    }
}

void findRandomSnowFlakesPositions(int numFlakes)
{
    for(int i=0;i<numFlakes;i++) {
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

//Spark Cloud Mode
//Expect a string like thisto change the mode Mode: M:ZONE,W:30,B:120,C1:002BFF,C2:FF00DB,C3:FF4600,C4:23FF00,
//Or simply this to just update speed or brightness:        W:30,B:120,
//Received command should have an ending comma, it makes this code easier
//All colors are in hex format
//If the mode Mode is changing, return the enum value of the mode
//Else if only the speed or brightness is being updated return the following:
//returnValue = 1000 - command was recieved to update speed or brightness, but new values  are == to old values
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
    
    //sprintf(debug,"%s", command.c_str());

	while(idx != -1) {
		if(command.charAt(beginIdx) == 'M') {
			//set the new mode from modeStruct array index
			returnValue = setNewMode(getModeIndexFromName(command.substring(beginIdx+2, idx).c_str())); 
			isNewMode = true;
		}
		else if(command.charAt(beginIdx) == 'S') {
		    int receivedSpeedValue = command.substring(beginIdx+2, idx).toInt();
		    if(receivedSpeedValue > (int)(sizeof(speedPresets)/sizeof(int)))
		        receivedSpeedValue = sizeof(speedPresets)/sizeof(int) - 1;
		    if (speedIndex != receivedSpeedValue) {
				isNewSpeed = true;
			}
			speedIndex = receivedSpeedValue;
			speed = speedPresets[speedIndex];
		}
		else if(command.charAt(beginIdx) == 'B') {
		    int newBrightness = command.substring(beginIdx+2, idx).toInt() * 255 / 100;	//Scale 0-100 to 0-255
			if(brightness != newBrightness) {
				isNewBrightness = true;
			}
			brightness = newBrightness;
		}
        else if(command.charAt(beginIdx) == 'C') {
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
			returnValue = 1001;
		}
		else if(isNewSpeed==true) {
			//Let the sender know we got the new speed command
			returnValue = 1002;
		}
		else {
			//If we got here, it's possible that a command was set to update speed or brightness
			//But neither one was a new value. The new values, equal the current values
			returnValue = 1000;
		}
    }

	// still here, so everything must be fine
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
    for(int i=0;i<(int)(sizeof modeStruct / sizeof modeStruct[0]);i++) {
        if(name.equals(String(modeStruct[i].modeName))) {
            return i;
        }
    }
    return -1;
}

int getModeIndexFromID(int id)
{
    for(int i=0;i<(int)(sizeof modeStruct / sizeof modeStruct[0]);i++) {
        if(id == modeStruct[i].modeId) {
            return i;
        }
    }
    return -1;
}

