package org.denaske.palcampito.base;

import java.util.Iterator;

import org.denaske.palcampito.R;
import org.denaske.palcampito.ui.ManualActivity;
import org.denaske.palcampito.ui.PreferencesActivity;
import org.denaske.palcampito.ui.PreviewActivity;

import android.content.Context;
import android.content.Intent;
import android.location.Criteria;
import android.location.GpsSatellite;
import android.location.GpsStatus;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.WindowManager;
import android.widget.Toast;

import com.google.android.maps.MapActivity;
import com.google.android.maps.MapView;

public abstract class MapBaseActivity extends MapActivity {
	private static final String TAG = "MAP";
	// private MediaPlayer mMediaPlayer;

	// private String path = "http://91.121.134.23:8001/cityfireflies.mp3";

	// location
	LocationManager locationManager;
	String bestProvider;
	LocationListener locationListener;
	public String locationInfo = "";
	public String speedInfo = "";

	@Override
	public void onCreate(Bundle icicle) {
		super.onCreate(icicle);

		// entrar();

		initGPS();
	}

	@Override
	protected void onResume() {
		super.onResume();
		startListening();
	}

	@Override
	protected void onPause() {
		super.onPause();
		stopListening();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.menu, menu);
		// menu.getItem(R.id.stop).setVisible(false);

		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle item selection
		switch (item.getItemId()) {

		case R.id.menu_preferencias:
			startActivity(new Intent(getApplicationContext(), PreferencesActivity.class));

			return true;

		case R.id.menu_about:

			startActivity(new Intent(getApplicationContext(), ManualActivity.class));
			// Intent intent1 = new
			// Intent("net.sweetmonster.android.app.cityfireflies.ui.VideoViewActivity");
			// intent.setData(Uri.parse("android.resource://" + getPackageName()
			// + "/raw/cityfireflies.mov"));

			return true;

		case R.id.menu_iraweb:

			startActivity(new Intent(getApplicationContext(), PreviewActivity.class));
			// Intent intent1 = new
			// Intent("net.sweetmonster.android.app.cityfireflies.ui.VideoViewActivity");
			// intent.setData(Uri.parse("android.resource://" + getPackageName()
			// + "/raw/cityfireflies.mov"));

			return true;

		default:
			return super.onOptionsItemSelected(item);
		}

	}

	@Override
	public boolean onPrepareOptionsMenu(Menu menu) {
		Log.d("qq", "" + menu);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FORCE_NOT_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);

		return true;
	}

	@Override
	public boolean onMenuOpened(int featureId, Menu menu) {

		return super.onMenuOpened(featureId, menu);
	}

	@Override
	public void onOptionsMenuClosed(Menu menu) {
		super.onOptionsMenuClosed(menu);

		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);

	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
	}

	@Override
	protected boolean isRouteDisplayed() {
		// TODO Auto-generated method stub
		return false;
	}

	public void initGPS() {

		// find GPS service and start listening
		Criteria criteria = new Criteria();
		criteria.setAccuracy(Criteria.ACCURACY_FINE);
		criteria.setAltitudeRequired(false);
		criteria.setBearingRequired(false);
		criteria.setSpeedRequired(false);
		criteria.setCostAllowed(true);
		criteria.setPowerRequirement(Criteria.POWER_MEDIUM);

		locationManager = (LocationManager) getSystemService(Context.LOCATION_SERVICE);
		bestProvider = locationManager.getBestProvider(criteria, true);

		// this reports on the status of the GPS engine, but does not enable
		// additional controls
		locationListener = new LocationListener() {

			public void onStatusChanged(String provider, int status, Bundle extras) {

				Log.d(TAG, "" + "hola");
				GpsStatus gpsStatus = locationManager.getGpsStatus(null);
				// gpsStatus.getSatellites();

				switch (status) {
				case GpsStatus.GPS_EVENT_STARTED:
					Log.d(TAG, "onGpsStatusChanged(): GPS started");
					break;

				case GpsStatus.GPS_EVENT_FIRST_FIX:
					Log.d(TAG, "onGpsStatusChanged(): time to first fix in ms = "
							+ gpsStatus.getTimeToFirstFix());
					break;

				case GpsStatus.GPS_EVENT_SATELLITE_STATUS:
					int maxSatellites = gpsStatus.getMaxSatellites(); // appears
					// fixed at 255 if (H.DEBUG)
					// Log.d(TAG,"onGpsStatusChanged(): max sats = " +
					// maxSatellites);
					Log.d(TAG, "onGpsStatusChanged(): ##,used,s/n,az,el");
					Iterable<GpsSatellite> satellites = gpsStatus.getSatellites();
					Iterator<GpsSatellite> satI = satellites.iterator();

					while (satI.hasNext()) {
						GpsSatellite satellite = satI.next();
						Log.d(TAG, "onGpsStatusChanged(): " + satellite.getPrn() + ","
								+ satellite.usedInFix() + "," + satellite.getSnr() + ","
								+ satellite.getAzimuth() + "," + satellite.getElevation());
						// http://en.wikipedia.org/wiki/Global_Positioning_System:
						// the almanac consists of coarse orbit and status
						// information for each satellite
						// http://en.wikipedia.org/wiki/Ephemeris: the positions
						// of
						// astronomical objects in the sky at a given time
						// + "," + satellite.hasAlmanac() + "," +
						// satellite.hasEphemeris());
					}
					break;

				case GpsStatus.GPS_EVENT_STOPPED:
					Log.i(TAG, "onGpsStatusChanged(): GPS stopped");
					break;
				}

			}

			public void onProviderEnabled(String provider) {
				Log.d(TAG, "enable");
			}

			public void onProviderDisabled(String provider) {
				Log.d(TAG, "enable");

			}

			public void onLocationChanged(Location location) {
				if (location != null) {
					Toast.makeText(
							getApplicationContext(),
							"New location latitude [" + location.getLatitude() + "] longitude ["
									+ location.getLongitude() + "]", Toast.LENGTH_SHORT).show();

					locationInfo = location.getLatitude() + " " + location.getLongitude() + " "
							+ location.getAltitude() + " " + location.getTime();

					onNewPosition(location.getLatitude(), location.getLongitude(), location
							.getAltitude());

				}

				if (location.hasSpeed()) {
					speedInfo = location.getSpeed() + " " + location.getTime();
				}
			}
		};

	}

	abstract public void onNewPosition(double latitude, double longitude, double altitude);

	public void startListening() {
		locationManager.requestLocationUpdates(bestProvider, 0, 0,
				(LocationListener) locationListener);

	}

	public void stopListening() {
		locationManager.removeUpdates(locationListener);
	}

	public boolean onTouchEvent(MotionEvent event, MapView mapView) {
		Log.d("qq", "qq");
		return false;
	}
}
