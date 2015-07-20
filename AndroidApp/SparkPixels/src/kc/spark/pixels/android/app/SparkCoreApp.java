package kc.spark.pixels.android.app;

import kc.spark.pixels.android.cloud.WebHelpers;
import kc.spark.pixels.android.storage.Prefs;
import kc.spark.pixels.android.storage.TinkerPrefs;
import android.app.Application;


public class SparkCoreApp extends Application {

	@Override
	public void onCreate() {
		super.onCreate();

		AppConfig.initialize(this);
		Prefs.initialize(this);
		TinkerPrefs.initialize(this);
		WebHelpers.initialize(this);
		DeviceState.initialize(this);
	}

}
