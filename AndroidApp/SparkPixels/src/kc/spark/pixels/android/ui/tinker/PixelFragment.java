package kc.spark.pixels.android.ui.tinker;

import static org.solemnsilence.util.Py.list;

import java.util.ArrayList;

import kc.get.pixel.list.android.R;

import org.apache.http.HttpStatus;
import org.solemnsilence.util.TLog;

import kc.spark.pixels.android.app.DeviceState;
import kc.spark.pixels.android.cloud.ApiFacade;
import kc.spark.pixels.android.cloud.api.Device;
import kc.spark.pixels.android.cloud.api.FunctionResponse;
import kc.spark.pixels.android.storage.Preferences;
import kc.spark.pixels.android.storage.TinkerPrefs;
import kc.spark.pixels.android.ui.BaseActivity;
import kc.spark.pixels.android.ui.BaseFragment;
import kc.spark.pixels.android.ui.ErrorsDelegate;
import kc.spark.pixels.android.ui.corelist.CoreListActivity;
import kc.spark.pixels.android.ui.tinker.ColorPickerDialog;
import kc.spark.pixels.android.ui.tinker.Item;
import kc.spark.pixels.android.ui.tinker.ItemAdapter;
import kc.spark.pixels.android.ui.tinker.ColorPickerDialog.OnColorSelectedListener;
import kc.spark.pixels.android.ui.util.NamingHelper;
import kc.spark.pixels.android.ui.util.Ui;


import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.app.AlertDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ListView;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;

//https://github.com/codepath/android_guides/wiki/Using-an-ArrayAdapter-with-ListView
//http://stackoverflow.com/questions/14188685/android-onclick-event-on-custom-listview-adapter-not-working
//https://www.bignerdranch.com/blog/customizing-android-listview-rows-subclassing/

/**
 * A fragment representing a single Core detail screen. This fragment is either
 * contained in a {@link CoreListActivity} in two-pane mode (on tablets) or a
 * {@link CoreDetailActivity} on handsets.
 */
public class PixelFragment extends BaseFragment implements OnClickListener, OnSeekBarChangeListener {

	private static final TLog log = new TLog(PixelFragment.class);

	/**
	 * The fragment argument representing the item ID that this fragment
	 * represents.
	 */
	public static final String ARG_DEVICE_ID = "ARG_DEVICE_ID";
	private static final int UPDATE_VIEW 	= 1;
	private static final int Is_First_Visit = 2;
	
	AlertDialog selectDialog;

	int defaultColor;
	int i_speed = 5;
	int i_brightness = 100; 
	int currentModeNum = 0; 
    String pixelCoreName = null;
    String initialMode;
    String s_userMode = "";
	final String modeHeader       	= "M:"; 
	final String brightnessHeader  	= "B:";	
	final String speedHeader       	= "S:"; 
	final String color1Header     	= "C1:"; 
	final String color2Header     	= "C2:";
	final String color3Header     	= "C3:";
	final String color4Header     	= "C4:";
	final String color5Header     	= "C5:";
	final String color6Header     	= "C6:";
	
	Item thisItem;
	Context globalContext;
	
	SharedPreferences prefs;
	
	private SeekBar sbPixleBrightness; 
	private SeekBar sbPixleSpeed; 
	private TextView tvBoxTempTitle;
	private TextView tvBoxTempValue;
	private TextView tvModeTitle;
	private TextView tvModeValue;
	private TextView tvColumnTitleMode;
	private TextView tvColumnTitleColor;
	private ListView listView;
	
	ArrayList<Item> mode = new ArrayList<Item>();
	String[] speed_names = new String[9];
	
	private Device device;
	private FunctionReceiver functionReceiver;
	private NamingCompleteReceiver namingCompleteReceiver;
	private NamingFailedReceiver namingFailedReceiver;
	private NamingStartedReceiver namingStartedReceiver;

	public static PixelFragment newInstance(String deviceId) {
		Bundle arguments = new Bundle();
		arguments.putString(PixelFragment.ARG_DEVICE_ID, deviceId);
		PixelFragment fragment = new PixelFragment();
		fragment.setArguments(arguments);
		return fragment;
	}

	/**
	 * Mandatory empty constructor for the fragment manager to instantiate the
	 * fragment (e.g. upon screen orientation changes).
	 */
	public PixelFragment() {
	}


	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setHasOptionsMenu(true);
		globalContext = this.getActivity();
		defaultColor = getResources().getColor(R.color.tinker_pin_bg);
		if (getArguments().containsKey(ARG_DEVICE_ID)) {
			device = DeviceState.getDeviceById(getArguments().getString(ARG_DEVICE_ID));
		}
		functionReceiver = new FunctionReceiver();
		namingCompleteReceiver = new NamingCompleteReceiver();
		namingFailedReceiver = new NamingFailedReceiver();
		namingStartedReceiver = new NamingStartedReceiver();
		
		prefs = PreferenceManager.getDefaultSharedPreferences(globalContext);
		//get the pixel driver core name preference
		pixelCoreName = prefs.getString("pixelCore", "NA");
	}

	@Override
	public void onViewCreated(View view, Bundle savedInstanceState) {
		super.onViewCreated(view, savedInstanceState);
		
		boolean isThisTheFirstVisit = false;
		pixelCoreName = prefs.getString("pixelCore", "NA");	//update the pixel Core 
		tvBoxTempTitle  	  = (TextView)Ui.findView(this, R.id.tv_temp_title);
		tvBoxTempValue  	  = (TextView)Ui.findView(this, R.id.tv_temp_value);
		tvModeTitle 	  	  = (TextView)Ui.findView(this, R.id.tv_mode_title);
		tvModeValue 	  	  = (TextView)Ui.findView(this, R.id.tv_mode_value);
		tvColumnTitleMode 	  = (TextView)Ui.findView(this, R.id.header_mode_name);
		tvColumnTitleColor 	  = (TextView)Ui.findView(this, R.id.header_colors);
		
		if (TinkerPrefs.getInstance().isFirstVisit()) {
			isThisTheFirstVisit = true;
			showInstructions();
		}
		
		if(device.name.equals(pixelCoreName)) {
			showProgress(R.id.progress_indicator, true);
			//first get the current selected mode from the Core, don't ask me why I do this first
			getVariable("mode", "SVgetInitialMode");
		}
		else if(!isThisTheFirstVisit) { 
			setWrongCoreView();
		}
	}
	
	@Override
	public void onStart() {
		super.onStart();
		broadcastMgr.registerReceiver(functionReceiver, functionReceiver.getFilter());
		broadcastMgr.registerReceiver(namingCompleteReceiver, namingCompleteReceiver.getFilter());
		broadcastMgr.registerReceiver(namingFailedReceiver, namingFailedReceiver.getFilter());
		broadcastMgr.registerReceiver(namingStartedReceiver, namingStartedReceiver.getFilter());
	}

	@Override
	public void onStop() {
		broadcastMgr.unregisterReceiver(functionReceiver);
		broadcastMgr.unregisterReceiver(namingCompleteReceiver);
		broadcastMgr.unregisterReceiver(namingFailedReceiver);
		broadcastMgr.unregisterReceiver(namingStartedReceiver);
		super.onStop();
	}

	@Override
	public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
		super.onCreateOptionsMenu(menu, inflater);
		inflater.inflate(R.menu.tinker, menu);
		inflater.inflate(R.menu.core_row_overflow, menu);
		inflater.inflate(R.menu.pref_menu, menu);
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
			case R.id.action_rename_core:
				new NamingHelper(getActivity(), api).showRenameDialog(device);
				return true;
			case R.id.action_settings:
				//Show preferences screen
				//startActivity(new Intent(getActivity(), Preferences.class));
				startActivityForResult(new Intent(getActivity(), Preferences.class), UPDATE_VIEW);
				return true; 
		}
		return super.onOptionsItemSelected(item);
	}

	@Override
	public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
 
        switch (requestCode) {
        case UPDATE_VIEW:
        	//Check to see if the pixel Core changed
        	if(pixelCoreName.equals(prefs.getString("pixelCore", "NA"))) {
        		if(device.name.equals(pixelCoreName)) {
        			//update view from changed preferences
            		setSpeedValues();
            		showBoxTemp(prefs.getBoolean("showBoxTemp", false));
            	}
    		}
        	else {
        		//Pixel Core Preference changed, so lets load the whole view
        		listView = (ListView) Ui.findView(this, R.id.mode_list);
        		pixelCoreName = prefs.getString("pixelCore", "NA");	//update the pixel Core
        		if(device.name.equals(pixelCoreName)) {
        			showProgress(R.id.progress_indicator, true);
        			//first get the current selected mode from the Core, don't ask me why I do this first
        			getVariable("mode", "SVgetInitialMode");
        			if(listView != null) {
        				listView.setVisibility(View.VISIBLE);
        			}
        		}
        		else { 
        			setWrongCoreView();
        			if(listView != null) {
        				listView.setVisibility(View.INVISIBLE);
        			}
        		}
        	}
            break;
        case Is_First_Visit:
        	showProgress(R.id.progress_indicator, true);
        	pixelCoreName = prefs.getString("pixelCore", "NA");
        	if(device.name.equals(pixelCoreName)) {
        		//first get the current selected mode from the Core, don't ask me why I do this first
        		getVariable("mode", "SVgetInitialMode");
        	}
        	else {
        		setWrongCoreView();
        	}
			break;
        }
    }
	
	private void setWrongCoreView() {
		showProgress(R.id.progress_indicator, false);
		showBoxTemp(false);
		showHeaderTitles(false);
		tvModeTitle.setText("Wrong Core Selected");
		tvModeValue.setText("");
	}
	
	private void completeOnViewCreated() {
		loadViews();
		setupListeners();
		showProgress(R.id.progress_indicator, false);
		
		//Step 5: get initial temp and mode
		if(prefs.getBoolean("showBoxTemp", false)) {
			getVariable("temp", "SVshowMeasuredTemperature");
		}
		//refreshSparkReadings();
	}
	
	private void loadViews() {
		log.d("REACHED THIS POINT");

		//Set brightness Slider
		sbPixleBrightness = (SeekBar)Ui.findView(this, R.id.pixel_brightness); // make seekbar object
		sbPixleBrightness.setProgress(i_brightness);
		Ui.setText(this, R.id.tv_brightness_value, Integer.toString(i_brightness));	//set brightness value text

		sbPixleSpeed = (SeekBar)Ui.findView(this, R.id.pixel_speed); // make seekbar object
		sbPixleSpeed.setProgress(i_speed);
		setSpeedValues();
	
		showBoxTemp(prefs.getBoolean("showBoxTemp", false));
	}

	private void showBoxTemp(boolean doIShowItOrNot) {
		if(doIShowItOrNot) {
			tvBoxTempTitle.setVisibility(View.VISIBLE);
			tvBoxTempValue.setVisibility(View.VISIBLE);
		}
		else {
			tvBoxTempTitle.setVisibility(View.GONE);
			tvBoxTempValue.setVisibility(View.GONE);
		}
	}

	private void showHeaderTitles(boolean doIShowItOrNot) {
		if(doIShowItOrNot) {
			tvColumnTitleMode.setVisibility(View.VISIBLE);
			tvColumnTitleColor.setVisibility(View.VISIBLE);
		}
		else {
			tvColumnTitleMode.setVisibility(View.GONE);
			tvColumnTitleColor.setVisibility(View.GONE);
		}
	}
	
	private void setupListeners() {
		Ui.findView(this, R.id.tinker_main).setOnClickListener(this);
		
		//Brightness Listener
		sbPixleBrightness.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            	callSimpleFunction("SetMode", "SFsetSpeedOrBrightness");
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                // TODO Auto-generated method stub
            }
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            	i_brightness = progress;		
            	Ui.setText(getActivity(), R.id.tv_brightness_value, Integer.toString(i_brightness));
            	//Ui.setText(getActivity(), R.id.tv_brightness_value, String.format("%3d",ibrightness));
            }
        });
		
		//Speed Listener
		sbPixleSpeed.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            	callSimpleFunction("SetMode", "SFsetSpeedOrBrightness");
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                // TODO Auto-generated method stub
            }
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            	i_speed = progress;
            	//Ui.setText(getActivity(), R.id.tv_speed_value,  Integer.toString(ispeed));
            	Ui.setText(getActivity(), R.id.tv_speed_value, speed_names[i_speed]);
            }
        });

		//Control Box Temperature Listener
		//Touch the text view of the temp reading to make a Core request to get the current temp  
		tvBoxTempValue.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				//check preferences to see if we should get the temp reading from the Core
				//We really should never get here if the preferences is set to false
				if(prefs.getBoolean("showBoxTemp", false)) {
					getVariable("temp", "SVshowMeasuredTemperature");
				}
			}
		});
		
		//Current Mode Listener
		//Touch the text view of the current mode to make a Core request to get the current mode
		Ui.findView(this, R.id.tv_mode_value).setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				getVariable("mode", "SVshowCurrentMode");
			}
		});
		
		//Item in ListView Listener
		listView.setOnItemClickListener(new OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
	             // ListView Clicked item value
	             final Item item = (Item)listView.getItemAtPosition(position);
	             thisItem = item;
           
	             //display color picker dialog for modes that need colors
	             if(item.getNumColors() > 0) {
	            	 //Have to setup views in reverse order so that they pop up on the phone in the ascending order
	            	 for(int i=item.getNumColors();i>0;i--) {
	            		 pickColor(item, view, i, item.getNumColors());
	            	 }
	             }
	             else {
	            	 callFunction("SetMode", item, "SFsetMode");
	            	 getVariable("mode", "SVshowCurrentMode");
	             }
			 }
		}); 
		
	}

	private void setSpeedValues() {
		int i;
		String s_buff;
		for(i=0;i<9;i++) {
			s_buff = "speedTag" + (i+1);
			speed_names[i] = prefs.getString(s_buff, "NA");
		}
		Ui.setText(this, R.id.tv_speed_value, speed_names[i_speed]);	//set speed value text
	}
	
	public int pickColor(final Item item, final View view, final int colorNum, final int numColors) {
		String dialogTitle = "Pick Color " + (colorNum);
	    new ColorPickerDialog(
	    		globalContext, 
	    		item.getColor(colorNum), 
	    		dialogTitle, 
	    		i_brightness, 
	    		new OnColorSelectedListener() {
	    			@Override
	    			public void onColorSelected(int color, int brightnessValue) {
				        item.setColor(colorNum, color);
				        i_brightness = brightnessValue;
				        sbPixleBrightness.setProgress(i_brightness);
				        //Update color in view
				        ItemView itemView = (ItemView)view;
					    itemView.setItem(item);
					    if(numColors == colorNum) {
					    	callFunction("SetMode", item, "SFsetMode");
					    	getVariable("mode", "SVshowCurrentMode");
					    }
	    			}
					@Override
					public void onCancelSelected() {
						// TODO Auto-generated method stub
						return;
					}

        }).show();
	    
	    return 1;
	 }
	 
	private Bundle bundleSimpleArgs() {
		Bundle args = new Bundle();
		StringBuilder stringBuilder = new StringBuilder();

		stringBuilder.append(speedHeader+i_speed+","+brightnessHeader+i_brightness+",");
		args.putString("params",stringBuilder.toString() );
		return args;
	}
	
	private Bundle bundleFunctionArgs(Item item) {
		int i;
		Bundle args = new Bundle();
		StringBuilder stringBuilder = new StringBuilder();

		stringBuilder.append(modeHeader+item.getTitle()+","+speedHeader+i_speed+","+brightnessHeader+i_brightness+",");
		
		for(i=1;i<=item.getNumColors();i++) {
			switch(i){
				case 1:
					stringBuilder.append(color1Header+String.format("%06X",0xFFFFFF &item.getColor1())+",");
					//args.putString("params",color1Header+String.format("%06X",0xFFFFFF &color1)+",");
					break;
				case 2:
					stringBuilder.append(color2Header+String.format("%06X",0xFFFFFF &item.getColor2())+",");
					//args.putString("params",color2Header+String.format("%06X",0xFFFFFF &color2)+",");
					break;
				case 3:
					stringBuilder.append(color3Header+String.format("%06X",0xFFFFFF &item.getColor3())+",");
					//args.putString("params",color3Header+String.format("%06X",0xFFFFFF &color3)+",");
					break;
				case 4:
					stringBuilder.append(color4Header+String.format("%06X",0xFFFFFF &item.getColor4())+",");
					//args.putString("params",color4Header+String.format("%06X",0xFFFFFF &color4)+",");
					break;
			}
		}
		args.putString("params",stringBuilder.toString() );
		return args;
	}
	
	private void getVariable(String spark_variable, String responseType) {
		if(device.name.equals(pixelCoreName)) {
			api.getSparkVariable(device.id, spark_variable, responseType);
		}
	}
	
	private void callFunction(String spark_function, Item item, String responseType) {
		if(device.name.equals(pixelCoreName)) {
			api.callSparkFunction(device.id, spark_function, bundleFunctionArgs(item), responseType);
		}
	}
	
	private void callSimpleFunction(String spark_function, String responseType) {
		if(device.name.equals(pixelCoreName)) {
			api.callSparkFunction(device.id, spark_function, bundleSimpleArgs(), responseType);
		}
	}
	
	private void refreshSparkReadings() {
		new Thread(new Runnable() {
			public void run(){
				while (true) {
					//getVariable("temp", "SVshowMeasuredTemperature");
					//Sleep(30000);
				}
			}
		}).start();
	}
	
	private void Sleep(long time) {
		try {
			Thread.sleep(time);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	private void showInstructions() {
		View instructions = Ui.findView(this, R.id.tinker_instructions);

		// set visible and then set it to disappear when we're done. and then
		// never show up again.
		instructions.setVisibility(View.VISIBLE);
		instructions.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				v.setVisibility(View.GONE);
				TinkerPrefs.getInstance().setVisited(true);
				startActivityForResult(new Intent(getActivity(), Preferences.class), Is_First_Visit);
			}
		});
	}

	private void toggleViewVisibilityWithFade(int viewId, final boolean show) {
		final View view = Ui.findView(this, viewId);
		int shortAnimTime = 150; // ms
		view.setVisibility(View.VISIBLE);
		view.animate()
				.setDuration(shortAnimTime)
				.alpha(show ? 1 : 0)
				.setListener(new AnimatorListenerAdapter() {

					@Override
					public void onAnimationEnd(Animator animation) {
						view.setVisibility(show ? View.VISIBLE : View.GONE);
					}
				});
	}

	@Override
	public int getContentViewLayoutId() {
		return R.layout.fragment_tinker;
	}

	private void onFunctionResponse(final FunctionResponse response) {
		log.d("Function response received: " + response);
		
		if (!device.id.equals(response.coreId)) {
			log.i("Function resposne did not match core ID");
			return;
		}
		
		if (response.errorMakingRequest) {
			ErrorsDelegate errorsDelegate = ((BaseActivity) getActivity()).getErrorsDelegate();
			errorsDelegate.showTinkerError();
		}
		log.d("response value: " + response.responseValue);
		log.d("CoreId value: " + response.coreId);
		log.d("response Type: " + response.responseType);
		log.d("response to String: " + response.toString());
		
		//Response Type starting with SV = Spark Variable
		//Response Type starting with SF = Spark Function
		switch (response.responseType) {
			case "SVshowMeasuredTemperature" :  
				if(response.responseValue != null) {
				//Set the received temperature on screen
					tvBoxTempValue.setText(String.format("%3.1f * F", Double.parseDouble(response.responseValue)));
				}
				else {
					showReadingError("No response from Core");
		        }
				break;
			case "SVshowCurrentMode":
				if(response.responseValue != null) {
					if(!(response.responseValue.equalsIgnoreCase("None"))) {
						final Handler handler = new Handler();
						handler.postDelayed(new Runnable() {
							@Override
							public void run() {
								//Set the received current mode title
								tvModeTitle.setText("Current Mode:");
								tvModeValue.setText(response.responseValue);
						  }
						}, 1);	//Delay updating view for 1 millisec
								//Increase this to your hearts desire, but I think it slows the app down too much
					}
					else {
						showReadingError("Error reading from Core");
					}
				}
				else {
					showReadingError("No response from Core");
		        }
				break;
			case "SVgetInitialMode":
				if(response.responseValue != null) {
					if(!(response.responseValue.equalsIgnoreCase("None"))) {
						//Store initial mode, then get number of available modes from the Core
						initialMode = response.responseValue;
						tvModeValue.setText(initialMode);
						//Step 2: Now get the current brightness value from the Core
						api.getSparkVariable(device.id, "brightness", "SVgetInitialBrightnessValue");
					}
					else {
						showReadingError("Error reading from Core");
					}
				}
				else {
					showReadingError("No response from Core");
		        }
				break;
			case "SVgetInitialBrightnessValue":
				if(response.responseValue != null) {
					i_brightness = (int)((Integer.parseInt(response.responseValue)+0.5) * 100 / 255);	//Scale from 0-255 to 0-100
					//Step 3: Now get the current Speed (delay) value from the Core
					api.getSparkVariable(device.id, "speed", "SVgetInitialSpeedValue");
				}
				break;
			case "SVgetInitialSpeedValue":
				if(response.responseValue != null) {
					i_speed = (int)(Integer.parseInt(response.responseValue));	
					//Step 4: Get the list of modes from the Core
					api.getSparkVariable(device.id, "modeList", "SVsetModeView");
				}
				break;
			case "SVsetModeView":
				if(response.responseValue != null) {
					if(!(response.responseValue.equalsIgnoreCase("None"))) {
						tvModeTitle.setText("Getting modes from Core");
						tvModeValue.setText("");
						int beginIdx = 0;													//parsing index helper
						int modeIdx = response.responseValue.indexOf(',');					//parsing index helper
						int colorNumIdx = response.responseValue.indexOf(',', modeIdx+1);	//parsing index helper
						int maxNumColors=0;		//use this to help wipe out the unused color views
						//Expect a string like this: "OFF,0,NORMAL,0,COLORALL,1,ZONE,4,etc..."
					    //Sequence is: "FUNCTION NAME 1,Number of Colors for Mode 1,FUNCTION NAME 2,Number of Colors for Mode 2,etc,"
					    //Sending command should have an ending comma, it makes this code easier
						while(modeIdx != -1) {
							String returnedMode = response.responseValue.substring(beginIdx, modeIdx);
							int numOfColors = Integer.parseInt(response.responseValue.substring(modeIdx+1,colorNumIdx));
							Item item = new Item(returnedMode, numOfColors, defaultColor, defaultColor, defaultColor, defaultColor, defaultColor, defaultColor);
							mode.add(item);
							if(numOfColors > maxNumColors) {
								maxNumColors = numOfColors; 
							}
							if(initialMode.equals(returnedMode)) {
								thisItem = item;	//set the current core selected mode by item
							}
											
							beginIdx = colorNumIdx+1;
							modeIdx = response.responseValue.indexOf(',', beginIdx);
							colorNumIdx = response.responseValue.indexOf(',', modeIdx+1);
						}
						ItemAdapter adapter = new ItemAdapter(globalContext, mode, maxNumColors);
						// Attach the adapter to a ListView
						listView = (ListView) Ui.findView(this, R.id.mode_list);
						listView.setAdapter(adapter);
						tvModeTitle.setText("Current Mode:");
						tvModeValue.setText(initialMode);
						showHeaderTitles(true);
						//Step 5: Finish populating the view using the gathered data from the Core
						completeOnViewCreated();
					}
					else {
						showReadingError("Error reading from Core");
					}
				}
				else {
					showReadingError("No response from Core");
		        }
				break;
			case "SFsetMode":
				if(response.responseValue != null) {
					if(Integer.parseInt(response.responseValue) != -1) {
						tvModeTitle.setText("Mode sent successfully");
						tvModeValue.setText("");
					}
				}
				break;
			case "SFsetSpeedOrBrightness":
				if(response.responseValue != null) {
					if(Integer.parseInt(response.responseValue) == 1000) {
						tvModeTitle.setText("No new values were sent");
						tvModeValue.setText("");
					}
					else if(Integer.parseInt(response.responseValue) == 1001) {
						tvModeTitle.setText("Brightness Updated");
						tvModeValue.setText("");
					}
					else if(Integer.parseInt(response.responseValue) == 1002) {
						tvModeTitle.setText("Speed Updated");
						tvModeValue.setText("");
					}
					else {
						tvModeTitle.setText("Not sure how I got here");
						tvModeValue.setText("");
					}
					getVariable("mode", "SVshowCurrentMode");
				}
				break;
			default :
				log.d("no known responseType: " + response.responseType);
				showReadingError("no known responseType: " + response.responseType);
				break;
		}
	}
	
	private void showReadingError(String errorMsg) {
		Log.e("Error", errorMsg);
        tvModeTitle.setText(errorMsg);
        tvModeValue.setText("");
        showProgress(R.id.progress_indicator, false);
        showBoxTemp(false);
		showHeaderTitles(false);
	}
	
	private class FunctionReceiver extends BroadcastReceiver {

		IntentFilter getFilter() {
			return new IntentFilter(ApiFacade.BROADCAST_FUNCTION_RESPONSE_RECEIVED);
		}

		@Override
		public void onReceive(Context context, Intent intent) {
			FunctionResponse response = intent.getParcelableExtra(ApiFacade.EXTRA_FUNCTION_RESPONSE);
			onFunctionResponse(response);
		}

	}


	private class NamingFailedReceiver extends BroadcastReceiver {

		IntentFilter getFilter() {
			return new IntentFilter(ApiFacade.BROADCAST_CORE_NAMING_REQUEST_COMPLETE);
		}

		@Override
		public void onReceive(Context context, Intent intent) {
			if ((ApiFacade.getResultCode(intent) != HttpStatus.SC_OK)) {
				BaseActivity activity = (BaseActivity) getActivity();
				activity.setCustomActionBarTitle(device.name);
				DeviceState.updateSingleDevice(device, true);
			}
		}

	}

	private class NamingStartedReceiver extends BroadcastReceiver {

		IntentFilter getFilter() {
			return new IntentFilter(NamingHelper.BROADCAST_NEW_NAME_FOUND);
		}

		@Override
		public void onReceive(Context context, Intent intent) {
			String newName = intent.getStringExtra(NamingHelper.EXTRA_NEW_NAME);
			if (newName != null) {
				BaseActivity activity = (BaseActivity) getActivity();
				activity.setCustomActionBarTitle(newName);
			}
		}

	}

	private class NamingCompleteReceiver extends BroadcastReceiver {

		IntentFilter getFilter() {
			return new IntentFilter(ApiFacade.BROADCAST_DEVICES_UPDATED);
		}

		@Override
		public void onReceive(Context context, Intent intent) {
			Device newDevice = DeviceState.getDeviceById(device.id);
			if (newDevice == null) {
				return;
			}
			// store previous name before switching out class level var
			String previousName = (device.name == null)
					? getString(R.string._unnamed_core_)
					: device.name;
			device = newDevice;

			if (!previousName.equals(device.name) && device.name != null) {
				BaseActivity activity = (BaseActivity) getActivity();
				activity.setCustomActionBarTitle(device.name);
			}
		}
	}

	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
	}
	
	@Override
	public void onProgressChanged(SeekBar seekBar, int progress,
			boolean fromUser) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onStartTrackingTouch(SeekBar seekBar) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onStopTrackingTouch(SeekBar seekBar) {
		// TODO Auto-generated method stub
		
	}

}

