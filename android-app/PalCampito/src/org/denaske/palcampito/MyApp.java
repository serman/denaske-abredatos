package org.denaske.palcampito;

import android.app.Application;



public class MyApp extends Application { 

	public static String TAG = ""; 
	public static String NAME = ""; 
	
	@Override
	public void onCreate() {
		super.onCreate();
		
		TAG = getString(R.string.app_name); 
		NAME = getString(R.string.app_name); 
	}

	public static String getName() {
		return NAME;
	}

}
