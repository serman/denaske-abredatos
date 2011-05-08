package org.denaske.palcampito.ui;

import java.util.ArrayList;
import java.util.List;

import org.denaske.palcampito.DrupalServices;
import org.denaske.palcampito.MeItemizedOverlay;
import org.denaske.palcampito.R;
import org.denaske.palcampito.SitiosItemizedOverlay;
import org.denaske.palcampito.base.AppConfig;
import org.denaske.palcampito.base.ILocation;
import org.denaske.palcampito.base.LocationNode;
import org.denaske.palcampito.base.MapBaseActivity;
import org.denaske.palcampito.base.MyOverlayItem;
import org.denaske.palcampito.utils.ActionItem;

import android.app.Activity;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ImageButton;
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

public class MyMapActivity extends MapBaseActivity implements ILocation {

	private MapController mapController;
	private MapView mapView; 
	public Activity mActivity; 

	static ActionItem chart;  
	static ActionItem production;  

	List<Overlay> mapOverlays;
	MeItemizedOverlay itemizedoverlay_me;
	SitiosItemizedOverlay itemizedoverlay_park;

	Drawable icon_park;
	Drawable icon_me;

	DrupalServices ds; 

	Handler handler = new Handler(); 
	private int mRadius = 50; 
	private boolean mFirstTime = true;
	private double mLongitude = -3.6934661865234375;
	private double mLatitude = 40.41065325368961;

	public void onCreate(Bundle bundle) {
		super.onCreate(bundle);
		setContentView(R.layout.map); // bind the layout to the activity

		mActivity = MyMapActivity.this; 
		// create a map view
		mapView = (MapView) findViewById(R.id.mapview);
		mapView.setBuiltInZoomControls(true);
		mapView.setStreetView(false);
		mapView.setSatellite(true);
		mapView.setOnTouchListener(new View.OnTouchListener() {

			@Override
			public boolean onTouch(View v, MotionEvent event) {
				Log.d("qq", "qq");
				return false;
			}
		});

		ImageButton btnToCenter = (ImageButton) findViewById(R.id.btnCenter);
		btnToCenter.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				centerGPSInfo(mLatitude, mLongitude);

			}
		}); 
		
		final TextView lblProgress = (TextView) findViewById(R.id.lblDistance); 
		lblProgress.setText("" + mRadius + " km"); 

		SeekBar bar = (SeekBar) findViewById(R.id.barDistance); 
		bar.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
		
				mRadius = seekBar.getProgress(); 
				reloadMapData(); 
				Log.d("qq", "cambio el radio a " + mRadius); 
				
			}
			
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {
				
			}
			
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) { 
				lblProgress.setText("" + progress + " km"); 
			} 
		}); 
	

		chart = new ActionItem();
		
		chart.setTitle("Anyadir foto");
		chart.setIcon(getResources().getDrawable(R.drawable.ic_menu_camera));
		chart.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				Toast.makeText(mActivity, "Chart selected" , Toast.LENGTH_SHORT).show();
			}
		});
	

			
		mapController = mapView.getController();
		mapController.setZoom(8); // Zoon 1 is world view

		mapOverlays = mapView.getOverlays();

		icon_park = getResources().getDrawable(R.drawable.map_icon_park);
		icon_me = getResources().getDrawable(R.drawable.map_icon_hiking);
	
		itemizedoverlay_me = new MeItemizedOverlay(this, icon_me);
		itemizedoverlay_park = new SitiosItemizedOverlay(this, icon_park);

		double latitude = mLatitude;
		double longitude = mLongitude;

		moveTo(latitude, longitude);

		ds = new DrupalServices(true); 

		getNodes(40.446947, -3.427734, mRadius);

	}
	
	public void getNodes(final double latitude, final double longitude, final int radius) {

		Runnable r = new Runnable() {
			public void run() {
				ds.userLogin(AppConfig.username, AppConfig.password); 
				ArrayList<LocationNode> locationNodes = (ArrayList<LocationNode>) ds
						.getLocationPointsJSON(latitude, longitude, radius);

				if (locationNodes != null) {
					for (int i = 0; i < locationNodes.size(); i++) {

						LocationNode l = locationNodes.get(i);

						GeoPoint point = new GeoPoint((int) (l.latitude * 1E6),
								(int) (l.longitude * 1E6));
						MyOverlayItem overlayitem = new MyOverlayItem(point, l.titulo, ""); 
						
						overlayitem.addNid(l.nid); 

						itemizedoverlay_park.addOverlay(overlayitem);
						mapOverlays.add(itemizedoverlay_park);

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

	void moveTo(double latitude, double longitude) {

		itemizedoverlay_me = new MeItemizedOverlay(this, icon_me);

		GeoPoint point = new GeoPoint((int) (latitude * 1E6), (int) (longitude * 1E6));
		OverlayItem overlayitem = new OverlayItem(point, "Medialab Prado",
				"Plaza de las letras, Madrid");
		

		mapController.animateTo(point);

		itemizedoverlay_me.addOverlay(overlayitem);
		mapOverlays.add(itemizedoverlay_me);
	}

	@Override
	public boolean dispatchTouchEvent(MotionEvent event) {
		boolean result = super.dispatchTouchEvent(event);
		if (event.getAction() == MotionEvent.ACTION_UP) {
			reloadMapData(); // / call the first block of code here

		}

		return result;
	}

	@Override
	protected boolean isRouteDisplayed() {
		// TODO Auto-generated method stub
		return false;
	}

	public void clearMap() { 
		itemizedoverlay_me.clear();
		itemizedoverlay_park.clear();
		mapOverlays.clear();
	}

	public void reloadMapData() {
		Projection projection = mapView.getProjection();
		int y = mapView.getHeight() / 2;
		int x = mapView.getWidth() / 2;

		GeoPoint geoPoint = projection.fromPixels(x, y);
		double latitude = (double) geoPoint.getLatitudeE6() / (double) 1E6;
		double longitude = (double) geoPoint.getLongitudeE6() / (double) 1E6;

		clearMap();
		getNodes(latitude, longitude, mRadius);
	}

	@Override
	public void onNewPosition(double latitude, double longitude, double altitude) {
		clearMap();

		mLatitude = latitude;
		mLongitude = longitude;

		if (mFirstTime) {
			centerGPSInfo(latitude, longitude);
			mFirstTime = false;
		}
	} 
	
	public void centerGPSInfo(double latitude, double longitude) {
		moveTo(latitude, longitude);
		Toast.makeText(mActivity, R.string.gpscenteringinfo, Toast.LENGTH_LONG);
	}
}
