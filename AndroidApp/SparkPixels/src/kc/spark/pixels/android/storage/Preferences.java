package kc.spark.pixels.android.storage;


import java.util.ArrayList;
import java.util.List;

import kc.get.pixel.list.android.R;
import kc.spark.pixels.android.app.DeviceState;
import kc.spark.pixels.android.cloud.api.Device;
import android.content.SharedPreferences;
import android.content.SharedPreferences.OnSharedPreferenceChangeListener;
import android.os.Bundle;
import android.preference.EditTextPreference;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceFragment;
import android.preference.PreferenceGroup;
 
public class Preferences extends PreferenceActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getFragmentManager().beginTransaction().replace(android.R.id.content, new MyPreferenceFragment()).commit();
    }

    public static class MyPreferenceFragment extends PreferenceFragment implements OnSharedPreferenceChangeListener
    {
        @Override
        public void onCreate(final Bundle savedInstanceState)
        {
            super.onCreate(savedInstanceState);
            addPreferencesFromResource(R.xml.preferences);
            
            final ListPreference listPreference = (ListPreference) findPreference("pixelCore");
            setListPreferenceData(listPreference);
            
            // show the current value in the settings screen
            for (int i = 0; i < getPreferenceScreen().getPreferenceCount(); i++) {
            	initSummary(getPreferenceScreen().getPreference(i));
            }
        }
        
        @Override
		public void onResume() {
        	super.onResume();
        	getPreferenceScreen().getSharedPreferences().registerOnSharedPreferenceChangeListener(this);
        }

        @Override
		public void onPause() {
        	super.onPause();
         	getPreferenceScreen().getSharedPreferences().unregisterOnSharedPreferenceChangeListener(this);
         }

        @Override
        public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
        	updatePreferences(findPreference(key));
        }

        private void initSummary(Preference p) {
        	if (p instanceof PreferenceGroup) {
        		PreferenceGroup pGrp = (PreferenceGroup) p;
        		for (int i = 0; i < pGrp.getPreferenceCount(); i++) {
        			initSummary(pGrp.getPreference(i));
        		}
        	} 
        	else {
       			updatePreferences(p);
        	}
        }
        
        private void updatePreferences(Preference p) {
        	if (p instanceof ListPreference) {
                ListPreference listPref = (ListPreference) p;
                p.setSummary(listPref.getEntry());
            }
        	if (p instanceof EditTextPreference) {
        		EditTextPreference editTextPref = (EditTextPreference) p;
        		p.setSummary(editTextPref.getText());
        	}
        }
        
        protected static void setListPreferenceData(ListPreference lp) {
        	if(!DeviceState.getKnownDevices().isEmpty()){
        		int i;
        		List<String> entries = new ArrayList<String>();
        	    List<String> entryValues = new ArrayList<String>();
        		
	        	for(i=0;i<DeviceState.getKnownDevices().size();i++) {
	        		Device device = DeviceState.getKnownDevices().get(i);
	        		entries.add(device.name);
	        		entryValues.add(Integer.toString(i+1));
	        	}
	        	
	        	final CharSequence[] entryCharSeq = entries.toArray(new CharSequence[entries.size()]);
	        	final CharSequence[] entryValsChar = entries.toArray(new CharSequence[entries.size()]);
	            lp.setEntries(entryCharSeq);
	            lp.setEntryValues(entryValsChar);
        	}
        }
    }
}

