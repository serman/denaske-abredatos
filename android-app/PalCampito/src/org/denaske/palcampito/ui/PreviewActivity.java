package org.denaske.palcampito.ui;


import org.denaske.palcampito.R;
import org.denaske.palcampito.base.MapBaseActivity;

import android.content.Context;
import android.os.Bundle;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.webkit.WebSettings.ZoomDensity;

public class PreviewActivity extends MapBaseActivity { 

	static Context c;

	private static final String TAG = "Preview"; 
	
    private WebView mWebView;



	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		c = getApplicationContext(); 
		 
		//setContentView(R.layout.preview); 
		
        mWebView = (WebView) findViewById(R.id.webview); 
        
		mWebView = new WebView(this); 
		setContentView(mWebView); 
		//mWebView.requestFocus(View.FOCUS_DOWN); 
        
        mWebView.setWebViewClient(new WebViewClient() {  
        	   @Override  
        	   public boolean shouldOverrideUrlLoading(WebView view, String url)  
        	   {  
        	       view.loadUrl(url); 
        	       
    	    	   //Log.d("qq", "hola!!"); 

        	       
        	       return true;

        	    }  
        	   
        	   

        	 });  
        
        WebSettings webSettings = mWebView.getSettings(); 
        webSettings.setSavePassword(true); 
        webSettings.setSaveFormData(true); 
        webSettings.setJavaScriptEnabled(true); 
        webSettings.setSupportZoom(true); 
        webSettings.setBuiltInZoomControls(true); 
        webSettings.setDefaultZoom(ZoomDensity.FAR); 
        
        //addContentView(mWebView, null); 

        //mWebView.
        mWebView.loadUrl("http://denaske.metadrop.pro"); 
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

	@Override
	public void onNewPosition(double latitude, double longitude, double altitude) {
		// TODO Auto-generated method stub
		
	}

}