package org.denaske.palcampito.base;

import org.denaske.palcampito.R;
import org.denaske.palcampito.ui.ManualActivity;

import android.app.Activity;
import android.content.Intent;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.WindowManager;

public class BaseActivity extends Activity { 
	private static final String TAG = "MediaPlayerDemo";
	private MediaPlayer mMediaPlayer;

	//private String path = "http://91.121.134.23:8001/cityfireflies.mp3";
	


	@Override
	public void onCreate(Bundle icicle) {
		super.onCreate(icicle);
	


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
			//startActivity(new Intent(getApplicationContext(), WActivity.class)); 
			
			return true; 
			
		case R.id.menu_about: 
			
			startActivity(new Intent(getApplicationContext(), ManualActivity.class)); 
			//Intent intent1 = new Intent("net.sweetmonster.android.app.cityfireflies.ui.VideoViewActivity"); 
			//intent.setData(Uri.parse("android.resource://" + getPackageName() + "/raw/cityfireflies.mov")); 
						
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
	
}
