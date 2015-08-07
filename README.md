# Spark Pixels
<img align="left" src="Pics/ic_launcher-web.png" width="22%" height="22%" hspace="15" style="float: left">Another Neopixel project controlled by the Spark Core from [Particle devices](http://docs.particle.io/) (formerly Spark). This repository contains the source code for the Spark Pixels Android App and Spark Core firmware. You will need an Android device, a Spark Core, and a strip of Neopixels. The app and Core firmware have been designed so that if you want to add a new LED sequence (aka mode) for your Neopixels, you only have to re-flash the Core. The Android app reads in the list of available modes from the Core every time the app is launched. The Android app code should never need to be updated.


## Usage
1. Load the sparkPixel firmware on your Core
2. Install the Android app on your phone from [here](https://play.google.com/store/apps/details?id=kc.spark.pixels.android) or by importing the source code into Eclipse (see Android App Building below).
3. Launch the app and login to your Particle.io account
4. You should see a list of your Cores, select your SparkPixel Core.
4. A welcome screen should appear. 
5. Tapping anywhere on the screen should bring up the settings page.
6. ***Important*** select **Spark Core Neopixel Driver** from the settings page to select your SparkPixel Core. The app won't be able to communicate with your Core if you skip this step.
7. Enjoy controlling your Neopixels from your Android device.


## Flashing the Firmware
1. Go to the Particle web IDE (https://build.particle.io/build/) 
2. Click on **CREATE NEW APP** and name it what you wish. I call mine *UNDERCABINETPIXELS*.
3. Add the **NEOPIXEL** library, Click on the libraries icon and select **NEOPIXEL** (it should be at the top of the list). Then click on **INCLUDE IN APP**. Select the app you just created from the list. Then click on **ADD TO THIS APP**.
4. Copy and paste the *SparkPixel.ino* code into the app, replacing any existing text.
5. Update the ***PIXEL_CNT*** and ***PIXEL_PIN*** (near the top of the code) for your setup.
6. Flash your Core and enjoy!

**Note**: If you are trying to flash a Photon device and get compile errors, you may need to comment out *Spark.syncTime()* on *line 309*.


## Android App
The app is based off of the Spark Core App Thermostat: SCAT (https://github.com/RolfHut/ThermosApp) which is heavily based on the official Spark app for android. The source code for that app can be found at https://github.com/spark/android-app. The whole view has been revamped to control the Neopixel brightness, speed and mode. Upon loading the app view, the app aquires these Spark Variables from the Core: brightness, speed and a comma delimited String called modeList. Once aquired, the brightness and speed slider bars are updated per the current Spark Core values. The modeList String is parsed out to populate a list view to display all of the available modes programmed into the Core. The modeList String also holds the number of colors each mode needs to run. Some modes need 1 color to operate, some modes need more, some modes need none. The user will select the desired color from a color picker dialog in the app. 

I have a temperature sensor inside my project box that houses the Spark Core and the +5V power supply. The temp reading can be displayed in the app view. The temperature reading view can be turned on or off in the settings found in the app menu.

The delay or speed of the Neopixels is controlled by a slider bar in the app. The speed is setup as preset settings that index the speedPresets array in the Firmware. The value sent to the Core is a numerical value from 0-8. The speed presets have default names, but can be changed to your liking in the settings page.

The list view of the modes will not populate unless it gets the modeList cloud variable from the Core and you have to set the Spark Core Neopixel Driver in the settings page of the app. If you have more than one Core (or Photon) registered, the app will always default to selecting this device from your list of Cores when the app launches.


## Adding a new Neopixel Mode to the Firmware
1. Add your new mode function to the firmware code.
2. Create a name for your new mode and add it to the list under "Mode ID Defines". It's actually of a *const int* type. Try to keep the number of characters to a minimum. See Limitations below for explanation.
3. Add that same name to the **modeStruct[]** array. The previously defined name must be used as the modeID parameter. I also use the same name as the modeName string. 
4. Then decide how many colors you want to pass to your new mode. (The Android app will force you to select this many different colors when selecting this mode). i.e The COLORALL mode takes one color. When the user selects this mode from the android app, the app will popup a color picker dialog to let the user pick the desired color for to pass to the selected mode.
5. Add the mode name to the case statement in the main loop() and add the function call to it.

## Firmware
All the mode information is defined in the **modeStruct[]** array. The setup routine takes this info and assembles the mode string info and the number of required colors into the Spark Cloud String Variable **modeList**. The info is assembled comma delimited. i.e. the modeList String would start out like this: *OFF,0,NORMAL,0,COLORALL,1,CHASER,1,ZONE,4, etc*
Feel free to remove any modes you don't care to have. You can simply comment out the line for the mode in the **modeStruct[]** array.

I have my Neopixels installed under my cabinets. So, I wanted to be able to set each cabinet to a different color. I call this mode ZONE. If you care to use the ZONE mode, you may need to add or subtract the number of colors (1 for each zone) you have setup. The max allowed is 6. You will also need to edit the start and end pixel defines under the *ZONE mode Start and End Pixels* section.

FYI on the CHASER mode, I wanted my chaser path to be a little bit different than the actual wired path of the LEDs. So I added a CHASER_LENGTH define and some extra Start and End defines under the *CHASER mode specific Start and End Pixels* section.

There are 9 preset speeds define by the **speedPresets** int array. The Android app passes an index to this array. Feel free to change these to your liking.


## Limitations
Spark String Variables have a max length of 622 bytes. This will limit the number of modes you can create since the modes get populated into the modeList String variable. Based off the average number of characters I currently have, I estimated a max number of modes to be 69. I really can't imagine I'll ever have 40 or 50 modes. So, I don't see this being an issue. I do try to keep the number of characters in my mode names shorter just to be safe.


## Android App Building
1. In Eclipse, go to File --> Import, and under the Android "folder", click "Existing Android Code into workspace", then click Next
2. Click Browse, select the dir where you cloned the repo, and click OK
3. You should now see two projects under the "Projects to Import" header: "SparkPixels" and "Fontify".  Click on Finish.
4. In the SparkPixels app, create the file ```res/values/local_build.xml``` with the following contents:

```
    <?xml version="1.0" encoding="utf-8"?>
    <resources>
        <string name="spark_token_creation_credentials">spark:spark</string>
    </resources>
```
_(You could actually put any valid HTTP Basic Auth string where it says ```spark:spark```; these values aren't currently used, but they must be present.)_

After this, you might also need to do a refresh and/or clean on the SparkPixels project, because Eclipse. ;-)


## Android App Requires TI SmartConfig Library
You must add smartconfiglib.jar to the `SparkPixels/libs` directory.

To get the SmartConfig library, go to the
[CC3000 Wi-Fi Downloads](http://processors.wiki.ti.com/index.php/CC3000_Wi-Fi_Downloads)
page. Search the page for the Android SmartConfig Application.
Download and unpack the app, which will require Windows. :-/
You can find smartconfiglib.jar in the libs directory of TI's app.


## Key Classes
If you want to know where the action is in the app, look at:
* PixelFragment: handles the main view and Spark Pixel control
* SimpleSparkApiService: an IntentService which performs the actual HTTP calls to talk to the Spark Cloud
* ApiFacade: A simple interface for making requests and handling responses from the Spark API. If you want to work with the Spark API from Android, this is the place to start. See examples below like nameCore(), digitalWrite(), etc, for templates to work from.
* SparkCoreApp: There are a number of classes which rely on an initialization step during app startup.  All of this happens in SparkCoreApp.onCreate().


## Hardware Implemenation
Firmware was developed on the Spark Core. This should work with the Particle Photon as well, but it hasn't been tested. 
The Android app was developed on a Samsung Galaxy S4 phone with Android version 5.0.1. I have no guarentees on how the list view will populate with another phone or tablet.


## Open Source Licenses
Original code in this repository is licensed by Spark Labs, Inc. under the Apache License, Version 2.0.
See LICENSE for more information.

This app uses several Open Source libraries. See SparkCore/libs/licenses for more information.
