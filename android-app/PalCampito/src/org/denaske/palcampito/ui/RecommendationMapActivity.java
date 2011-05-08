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
import org.denaske.palcampito.utils.QuickAction;
import org.denaske.palcampito.utils.Utils;

import android.app.Activity;
import android.content.Context;
import android.graphics.drawable.Drawable;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Vibrator;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Toast;

import com.google.android.maps.GeoPoint;
import com.google.android.maps.MapController;
import com.google.android.maps.MapView;
import com.google.android.maps.Overlay;
import com.google.android.maps.OverlayItem;
import com.google.android.maps.Projection;

public class RecommendationMapActivity extends MapBaseActivity implements ILocation,
		SensorEventListener {

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

	// agitar
	protected long[] mShakePattern;
	protected Vibrator mVibrator;
	protected SensorManager sensormanager; 
	Sensor accelerometer; 

	protected static final int SHAKE_FINISH = 100;
	protected static final int SHAKE_THREESHOLD = 10;
	protected static final int SHAKE_MAX = 45;

	protected float lastAccX;
	protected float lastAccY;
	protected float lastAccZ;

	protected float totalMovement = 0;

	public void onCreate(Bundle bundle) {
		super.onCreate(bundle);
		setContentView(R.layout.recommendation_map); // bind the layout to the
		// activity

		mActivity = RecommendationMapActivity.this;
		// create a map view
		mapView = (MapView) findViewById(R.id.mapview);
		mapView.setBuiltInZoomControls(true);
		mapView.setStreetView(false);
		mapView.setSatellite(true);
		

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

		mVibrator = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
		mVibrator.vibrate(100);

		// register
		sensormanager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
		accelerometer = sensormanager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
		sensormanager.registerListener(this, accelerometer, SensorManager.SENSOR_DELAY_NORMAL);

	} 
	
	@Override
	protected void onResume() {
		super.onResume(); 
		sensormanager.registerListener(this, accelerometer, SensorManager.SENSOR_DELAY_NORMAL);

	} 
	
	@Override
	protected void onPause() {
		super.onPause(); 
		sensormanager.unregisterListener(this, accelerometer);

	}

	public void getNodes(final double latitude, final double longitude, final int radius) {

		Runnable r = new Runnable() {
			public void run() {
				ds.userLogin(AppConfig.username, AppConfig.password);
				ArrayList<LocationNode> locationNodes = (ArrayList<LocationNode>) ds
						.getRandomPointJSON(latitude, longitude, radius);
			
				Log.d("recomendaciones", "hola"); 
			
				if (locationNodes != null) {
					Log.d("recomendaciones", "hola2"); 
					for (int i = 0; i < locationNodes.size(); i++) {
						Log.d("recomendaciones", "" + i); 
						LocationNode l = locationNodes.get(i); 
						moveTo(l.latitude, l.longitude); 

						GeoPoint point = new GeoPoint((int) (l.latitude * 1E6),
								(int) (l.longitude * 1E6));
						MyOverlayItem overlayitem = new MyOverlayItem(point, l.titulo, "");

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

		mLatitude = latitude;
		mLongitude = longitude;

	}


	/**
	 * @param accelX
	 *            the accelX to set
	 */
	public void setAccel(float accelX, float accelY, float accelZ) {

		// The accelerometer values indicate the acceleration in units of 1 = 1g
		// = 9.8 m/s2 along the X, Y, and Z
		// a stationary device will have an acceleration value of (0, 0, -1)
		if (Math.abs(lastAccX - accelX) > 9.8f || Math.abs(lastAccY - accelY) > 9.8f
				|| Math.abs(lastAccZ - accelZ) > 9.8f) {

			lastAccX = accelX;
			lastAccY = accelY;
			lastAccZ = accelZ; 
			
			// Real total amount of movement
			// (float)Math.sqrt(accelX*accelX + accelY*accelY + accelZ*accelZ);
			float totalAcceleration = Math.abs(accelX) + Math.abs(accelY) + Math.abs(accelZ);

			float normalizedGlobalAcceleration = Utils.norm(totalAcceleration, SHAKE_THREESHOLD,
					SHAKE_MAX);

			// si va bien
			if (totalAcceleration > SHAKE_THREESHOLD) {
				totalMovement = totalMovement + normalizedGlobalAcceleration * 5; 
				Log.d("qq", "" + "qq2");
				getNodes(mLatitude, mLongitude, mRadius); 
				
				mVibrator.vibrate(500); 

				// updateState(normalizedGlobalAcceleration);
			}
		}
	}

	// ------------------------------------------------------
	// Sensors 
	
	@Override
	public void onAccuracyChanged(Sensor sensor, int accuracy) {
	}

	@Override
	public void onSensorChanged(SensorEvent event) {

		// Get the sensor info and data.
		if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
			// Proccess it
			setAccel(event.values[0], event.values[1], event.values[2]); 
		}
	}

}
