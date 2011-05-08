package org.denaske.palcampito.ui;

import java.util.ArrayList;
import java.util.List;

import org.denaske.palcampito.DrupalServices;
import org.denaske.palcampito.SitiosItemizedOverlay;
import org.denaske.palcampito.R;
import org.denaske.palcampito.base.ILocation;
import org.denaske.palcampito.base.LocationNode;
import org.denaske.palcampito.base.MapBaseActivity;

import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.SeekBar.OnSeekBarChangeListener;

import com.google.android.maps.GeoPoint;
import com.google.android.maps.MapController;
import com.google.android.maps.MapView;
import com.google.android.maps.Overlay;
import com.google.android.maps.OverlayItem;
import com.google.android.maps.Projection;

public class MyListActivity extends MapBaseActivity implements ILocation {

	DrupalServices ds;

	Handler handler = new Handler();
	private int mRadius = 50;
	private boolean mFirstTime = true;
	private double mLongitude = -3.6934661865234375;
	private double mLatitude = 40.41065325368961;

	public void onCreate(Bundle bundle) {
		super.onCreate(bundle);
		setContentView(R.layout.map); // bind the layout to the activity


		
		final TextView lblProgress = (TextView) findViewById(R.id.lblDistance); 

		SeekBar bar = (SeekBar) findViewById(R.id.barDistance); 
		bar.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
		
				mRadius = seekBar.getProgress(); 
				getNodes(mLatitude, mLongitude, mRadius); 				
			}
			
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {
				
			}
			
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) { 
				lblProgress.setText("" + mRadius); 
			}
		}); 
		
		

		ds = new DrupalServices(true);

		getNodes(40.446947, -3.427734, mRadius);

	}

	public void getNodes(final double latitude, final double longitude, final int radius) {

		Runnable r = new Runnable() {
			public void run() {
				ds.userLogin("admin", "admin");
				ArrayList<LocationNode> locationNodes = (ArrayList<LocationNode>) ds
						.getLocationPointsJSON(latitude, longitude, radius);

				if (locationNodes != null) {
					for (int i = 0; i < locationNodes.size(); i++) {

						LocationNode l = locationNodes.get(i);

					}
				} else {
					// TODO mostrar mensaje de error
				}
				// handler.post(this);
			}
		};
		// r.run();
		handler.post(r);

	} 

	
	@Override
	public void onNewPosition(double latitude, double longitude, double altitude) {

		mLatitude = latitude;
		mLongitude = longitude;

		if (mFirstTime) {
			centerGPSInfo(latitude, longitude);
			mFirstTime = false;
		}
	} 

	public void centerGPSInfo(double latitude, double longitude) {
		Toast.makeText(this, R.string.gpscenteringinfo, Toast.LENGTH_LONG);
	}
}
