package org.denaske.palcampito;

import org.denaske.palcampito.base.AppConfig;

import android.app.Application;
import android.content.Context;
import android.content.SharedPreferences;
import android.util.Log;



public class MyApp extends Application { 

	public static String TAG = ""; 
	public static String NAME = ""; 
	
	@Override
	public void onCreate() {
		super.onCreate();
		
		TAG = getString(R.string.app_name); 
		NAME = getString(R.string.app_name); 
		
		loadCustomPreferences(getApplicationContext()); 

	}

	public static String getName() {
		return NAME;
	}

	public static void loadCustomPreferences(Context c) {

		SharedPreferences settings = c.getSharedPreferences("mypreferences", c.MODE_PRIVATE);
		AppConfig.times = settings.getInt(AppConfig.TIMES, 1); 
		AppConfig.username = settings.getString(AppConfig.USER, "services"); 
		AppConfig.password = settings.getString(AppConfig.PASSWORD, "services"); 
		AppConfig.logged = settings.getInt(AppConfig.LOGGED, 0); 
		Log.d("qq", "times " + AppConfig.username);
		Log.d("qq", "times " + AppConfig.password);
		SharedPreferences.Editor editor = settings.edit();
		AppConfig.times += 1; 
		// Log.d(MyApp.TAG + TAG, "times " + Preferences.times);
		editor.putInt(AppConfig.TIMES, AppConfig.times);
		// editor.rem
		editor.commit();
//
	} 
	
	public static void saveCustomPreferences(Context c) {
		
		SharedPreferences settings = c.getSharedPreferences("mypreferences", c.MODE_PRIVATE); 
		SharedPreferences.Editor editor = settings.edit();
		editor.putString(AppConfig.USER, AppConfig.username); 
		editor.putString(AppConfig.PASSWORD, AppConfig.password); 
		editor.putInt(AppConfig.LOGGED, AppConfig.logged); 
		editor.commit();
		
	} 
	
	
	public static void borrarUsuarioPreferences(Context c) {
		
		SharedPreferences settings = c.getSharedPreferences("mypreferences", c.MODE_PRIVATE); 
		SharedPreferences.Editor editor = settings.edit();
		AppConfig.logged = 0; 
		editor.putInt(AppConfig.LOGGED, AppConfig.logged); 
		editor.commit();
		
	} 
	
	
}
