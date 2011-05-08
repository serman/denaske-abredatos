package org.denaske.palcampito.ui;

import org.denaske.palcampito.MyApp;
import org.denaske.palcampito.R;
import org.denaske.palcampito.base.AppConfig;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;


public class MyCustomDialog extends Dialog {

	Context mContext; 
	EditText user;
	EditText password;

	public MyCustomDialog(Context context) {
		super(context); 
		this.mContext = context; 

	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.login);
		setTitle("Enter your Name ");
		user = (EditText) findViewById(R.id.login_usuario);
		password = (EditText) findViewById(R.id.login_contrasenya);
		Button buttonOK = (Button) findViewById(R.id.aceptar);
		buttonOK.setOnClickListener(new OKListener());
	}

	private class OKListener implements android.view.View.OnClickListener {
		@Override
        public void onClick(View v) { 
        	AppConfig.username = user.getText().toString(); 
        	AppConfig.password = password.getText().toString(); 
        	AppConfig.logged = 1; 
        	MyApp.saveCustomPreferences(mContext); 
			dismiss(); 

        }
	}

	private class CancelListener implements android.view.View.OnClickListener {
		@Override
		public void onClick(View v) {
			dismiss(); 
		}
	}

}

