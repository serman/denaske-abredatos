package org.denaske.palcampito.ui;


import org.denaske.palcampito.MyApp;
import org.denaske.palcampito.R;
import org.denaske.palcampito.R.id;
import org.denaske.palcampito.R.layout;
import org.denaske.palcampito.base.BaseActivity;
import org.denaske.palcampito.utils.Intents;

import android.content.Context;
import android.os.Bundle;
import android.webkit.JsResult;
import android.webkit.WebChromeClient;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;

public class ManualActivity extends BaseActivity {

	static Context c;

	private WebView mWebView;
	private String uiPath;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		c = getApplicationContext();

		Context context = getApplicationContext();

		MyApp ap = (MyApp) getApplication();

		setContentView(R.layout.webviewmanual);
		mWebView = (WebView) findViewById(R.id.webview);

		// mWebView = new WebView(this);

		mWebView.setInitialScale(1);

		mWebView.setWebViewClient(new WebViewClient() { 
			@Override
			public boolean shouldOverrideUrlLoading(WebView view, String url) {

				if (url.contains("manual")) {
					view.loadUrl(url); 
					
					return true;
				} else { 
					
					Intents.openWeb(getApplicationContext(), url); 
					
					return false;
				}

			} 

		});
		
		WebSettings webSettings = mWebView.getSettings(); 
		//webSettings.setSavePassword(false);
		//webSettings.setSaveFormData(false);
		//webSettings.setJavaScriptEnabled(true);
		//webSettings.setSupportZoom(false); 
		//webSettings.setDefaultZoom(ZoomDensity.CLOSE); 
		webSettings.setDefaultFontSize(35); 

		//mWebView.setFocusable(true);
		//mWebView.setFocusableInTouchMode(true); 
		mWebView.loadUrl("file:///android_asset/manual.html");

	}

	@Override
	protected void onRestart() {
		super.onRestart();
	}

	@Override
	protected void onPause() {
		super.onPause();
	}

	@Override
	protected void onResume() {
		super.onResume();

		mWebView.loadUrl("file:///android_asset/manual.html");

	}

	@Override
	public void onDestroy() {
		super.onDestroy();

	}

	@Override
	protected void onRestoreInstanceState(Bundle savedInstanceState) {

	}

	@Override
	protected void onSaveInstanceState(Bundle outState) {
	}

	/**
	 * Provides a hook for calling "alert" from javascript. Useful for debugging
	 * your javascript.
	 */
	final class MyWebChromeClient extends WebChromeClient {
		@Override
		public boolean onJsAlert(WebView view, String url, String message, JsResult result) { 
			result.confirm();
			return true;
		}
	}

}