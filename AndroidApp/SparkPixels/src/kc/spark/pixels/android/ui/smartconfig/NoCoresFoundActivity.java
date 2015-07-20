package kc.spark.pixels.android.ui.smartconfig;

import kc.get.pixel.list.android.R;
import kc.spark.pixels.android.ui.BaseActivity;
import kc.spark.pixels.android.ui.util.Ui;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.app.NavUtils;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;


public class NoCoresFoundActivity extends BaseActivity {


	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_smart_config);
		View border = Ui.findView(this, R.id.header_border);
		border.setBackgroundColor(getResources().getColor(R.color.failure_red));

		TextView finePrint = Ui.findView(this, R.id.fine_print);
		finePrint.setText("");

		getFragmentManager()
				.beginTransaction()
				.add(R.id.smart_config_frag, new NoCoresFoundFragment())
				.commit();
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
			case android.R.id.home:
				NavUtils.navigateUpTo(this, new Intent(this, SmartConfigActivity.class));
				return true;
		}
		return super.onOptionsItemSelected(item);
	}

}
