package org.denaske.palcampito.ui;


import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.MotionEvent;

public class BootScreenActivity extends Activity {

	protected int _splashTime = 525;
	protected Handler _exitHandler = null;
	protected Runnable _exitRunnable = null;

	/** Called when the activity is first created. */

	@Override
	public void onCreate(Bundle savedInstanceState) {

		super.onCreate(savedInstanceState);
		//setContentView(R.layout.bootscreen);
		// Runnable exiting the splash screen and launching the menu

		_exitRunnable = new Runnable() {
			public void run() {
				exitSplash();
			}
		};

		// Run the exitRunnable in in _splashTime ms
		_exitHandler = new Handler();
		_exitHandler.postDelayed(_exitRunnable, _splashTime);
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		if (event.getAction() == MotionEvent.ACTION_DOWN) {
			// Remove the exitRunnable callback from the handler queue
			_exitHandler.removeCallbacks(_exitRunnable);
			// Run the exit code manually
			exitSplash();
		}
		return true;
	}
	
	private void exitSplash() { 
		finish(); 
		startActivity(new Intent("org.denaske.palcampito.ui.MainActivity")); 
	}
}
