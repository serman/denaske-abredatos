package org.denaske.palcampito.ui;

import org.denaske.palcampito.R;

import android.app.AlertDialog;
import android.app.TabActivity;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.location.LocationManager;
import android.os.Bundle;
import android.provider.Settings;
import android.widget.TabHost;
import android.widget.TabHost.TabSpec;

public class MainActivity extends TabActivity {

	static private TabHost mTabHost;
	private Resources mResources;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		mTabHost = getTabHost(); 
		//mTabHost = (TabHost) findViewById(android.R.id.tabhost); 

		mResources = getResources();
		addTab(getResources().getString(R.string.mialrededor), "Texto Tab2",
				android.R.drawable.ic_menu_mapmode, MyMapActivity.class); 
		//addTab(getResources().getString(R.string.denunciar), "Texto Tab1",
		//		android.R.drawable.ic_menu_agenda, PhotoCaptureActivity.class);
		addTab(getResources().getString(R.string.explora), "Texto Tab2",
				android.R.drawable.ic_menu_edit, RecommendationMapActivity.class);

		// final Intent myIntent = new
		// Intent(android.content.Intent.ACTION_VIEW,
		// Uri.parse("geo:0,0?q=http://casastristes.org/es/kml"));
		// startActivity(myIntent);
		mTabHost.setCurrentTab(0);

		final LocationManager manager = (LocationManager) getSystemService(Context.LOCATION_SERVICE);

		if (!manager.isProviderEnabled(LocationManager.GPS_PROVIDER)) {
			buildAlertMessageNoGps();
		}

	}

	private void buildAlertMessageNoGps() {
		final AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setTitle("Parece que tu GPS esta desactivado");
		builder.setMessage("Si quieres que todo funcione mejor activalo! :)").setCancelable(false)
				.setPositiveButton("Si", new DialogInterface.OnClickListener() {
					public void onClick(@SuppressWarnings("unused") final DialogInterface dialog,
							@SuppressWarnings("unused") final int id) {
						Intent intent = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
						startActivity(intent);
					}
				}).setNegativeButton("No", new DialogInterface.OnClickListener() {
					public void onClick(final DialogInterface dialog,
							@SuppressWarnings("unused") final int id) {
						dialog.cancel();
					}
				});
		final AlertDialog alert = builder.create();
		alert.show();
	}

	private void addTab(String tabName, String tabSpec, int drawableID, Class<?> cls) {
		Intent intent = new Intent(this, cls);
		TabSpec spec = mTabHost.newTabSpec(tabSpec);
		spec.setIndicator(tabName, mResources.getDrawable(drawableID));
		spec.setContent(intent);
		mTabHost.addTab(spec);

	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);

		if (newConfig.orientation == Configuration.ORIENTATION_LANDSCAPE) {
			// this.loadURLData = false;
		}

		if (newConfig.orientation == Configuration.ORIENTATION_PORTRAIT) {
			// this.loadURLData = true;
		}

	}

}
