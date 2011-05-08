/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.denaske.palcampito.ui;

import org.denaske.palcampito.MyApp;
import org.denaske.palcampito.R;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.OnSharedPreferenceChangeListener;
import android.os.Bundle;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.Preference.OnPreferenceClickListener;

public class PreferencesActivity extends PreferenceActivity implements
		OnSharedPreferenceChangeListener {

	final static String TAG = "Preferences"; 
	MyApp ap;
	Context c;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		// Load the preferences from an XML resource
		addPreferencesFromResource(R.xml.preferences);
		c = getApplicationContext();


		Preference aboutPreference = (Preference) findPreference("cBorrar");
		aboutPreference.setOnPreferenceClickListener(new OnPreferenceClickListener() {
			public boolean onPreferenceClick(Preference preference) {

				MyApp.borrarUsuarioPreferences(c); 
				
				return true;
			}
		});

	}

	@Override
	public void onSharedPreferenceChanged(SharedPreferences arg0, String arg1) {
		// TODO Auto-generated method stub
		
	}
}
