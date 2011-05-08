package org.denaske.palcampito.ui;

import java.io.File;
import java.util.HashMap;

import org.denaske.palcampito.DrupalServices;
import org.denaske.palcampito.MyApp;
import org.denaske.palcampito.R;
import org.denaske.palcampito.base.AppConfig;
import org.denaske.palcampito.base.MapBaseActivity;
import org.denaske.palcampito.utils.Utils;

import android.app.ProgressDialog;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.provider.MediaStore;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.ViewFlipper;
import android.widget.RadioGroup.OnCheckedChangeListener;

public class PhotoCaptureActivity extends MapBaseActivity {

	static Context c;

	private static final String TAG = "Photo";

	static ViewFlipper flipper;
	private Button _btnTakePicture;
	private Button _btnUpload;
	private ImageView _image;
	private TextView _field;
	TextView msgCabecera;
	private RadioGroup mRadioGroup;
	private String _rootPath;
	private String _path;
	private String _fileName;

	private boolean _taken;
	private static final String PHOTO_TAKEN = "photo_taken";

	DrupalServices ds;
	Handler mHandler;
	ProgressDialog mPdialog;

	private String nid = null;
	protected int mRating = 1;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		c = getApplicationContext();

		Bundle bundle = this.getIntent().getExtras();
		if (bundle != null) {
			nid = bundle.getString("param1");
			Log.d(MyApp.TAG + TAG, "" + nid);
		}

		setContentView(R.layout.camera);

		_image = (ImageView) findViewById(R.id.image);
		_field = (TextView) findViewById(R.id.editTextComment);

		_btnTakePicture = (Button) findViewById(R.id.buttonP);
		_btnTakePicture.setOnClickListener(new TakePhoto());

		_btnUpload = (Button) findViewById(R.id.buttonN);
		_btnUpload.setOnClickListener(new Upload());

		mRadioGroup = (RadioGroup) findViewById(R.id.radioGroup1);
		mRadioGroup.setOnCheckedChangeListener(new OnCheckedChangeListener() {

			@Override
			public void onCheckedChanged(RadioGroup group, int checkedId) {
				String q = (String) findViewById(checkedId).getTag();

				if (q.equals("megusta")) {
					mRating = 100;
				} else {
					mRating = 1;

				}

			}
		});

		msgCabecera = (TextView) findViewById(R.id.textCabecera);

		_rootPath = Environment.getExternalStorageDirectory() + "/" + MyApp.getName() + "/images/";
		ds = new DrupalServices(true);

		mHandler = new Handler() {
			@Override
			public void handleMessage(Message msg) {
				super.handleMessage(msg);

				mPdialog.dismiss();
				resetActivity();

			}

		};

		// flipper = (ViewFlipper) findViewById(R.id.flipper);

		// MyNotification.show(getApplicationContext(), MainActivity.class,
		// R.drawable.umbrella,
		// "hola", "hola2");
		//
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

	public class TakePhoto implements View.OnClickListener {
		public void onClick(View view) {
			Log.d(MyApp.TAG + TAG, "TakePhoto.onClick()");

			startCameraActivity();

			// _field.setVisibility(View.GONE);
			_btnUpload.setEnabled(true);

		}
	}

	public class Upload implements View.OnClickListener {
		public void onClick(View view) {
			Log.d(MyApp.TAG + TAG, "Next.onClick()");
			mPdialog = ProgressDialog.show(PhotoCaptureActivity.this, "", getResources().getString(
					R.string.upload_pic_wait), true, false);
			upload();

			// flipper.setAnimation(AnimationUtils.loadAnimation(view.getContext(),
			// R.anim.push_left_out));
			// flipper.showNext();
			// _btnTakePicture.setVisibility(View.GONE);
			// _buttonB.setVisibility(View.VISIBLE);
			// msgCabecera.setText("completa los datos");

		}
	}

	protected void startCameraActivity() {
		Log.d(MyApp.TAG + TAG, "startCameraActivity()");
		_fileName = Utils.getCurrentTime() + ".jpg";
		Log.d(MyApp.TAG + TAG, " " + Utils.getCurrentTime());

		_path = _rootPath + _fileName;

		new File(_rootPath).mkdirs();
		File file = new File(_path);
		Uri outputFileUri = Uri.fromFile(file);

		Intent intent = new Intent(android.provider.MediaStore.ACTION_IMAGE_CAPTURE);
		intent.putExtra(MediaStore.EXTRA_OUTPUT, outputFileUri);

		startActivityForResult(intent, 0);

	}

	//
	public void upload() {
		// dialog.setCancelable(true);

		Runnable r = new Runnable() {
			public void run() {
				HashMap<String, Object> userObj = ds.userLogin(AppConfig.username,
						AppConfig.password);
				HashMap<String, Object> imgObj = null;

				if (_path != null) {
					imgObj = ds.uploadImage(_path, _fileName, userObj);
				}

				ds.nodeSave(imgObj, userObj, _field.getText().toString(), _field.getText()
						.toString(), nid, 0, 0);

				mHandler.sendEmptyMessage(0);
			}
			// handler.post(this);

		};
		// r.run();
		mHandler.post(r);

	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		Log.d(MyApp.TAG + TAG, "resultCode: " + resultCode);
		switch (resultCode) {
		case 0:
			Log.d(MyApp.TAG + TAG, "User cancelled");
			_fileName = null;
			_path = null;
			break;

		case -1:
			onPhotoTaken();
			break;
		}
	}

	protected void onPhotoTaken() {
		Log.d(MyApp.TAG + TAG, "onPhotoTaken");

		_taken = true;

		BitmapFactory.Options options = new BitmapFactory.Options();
		options.inSampleSize = 4;

		// tiempo y geolocalizacion
		//
		Log.d(MyApp.TAG + TAG, "este path: " + _path);
		// subo el archivo
		Bitmap bitmap = BitmapFactory.decodeFile(_path, options);
		_btnTakePicture.setText(R.string.takepic_again);
		// HttpUploadAsync q = new HttpUploadAsync(this);
		// q.execute(_path);

		_image.setImageBitmap(bitmap);

	}

	@Override
	protected void onRestoreInstanceState(Bundle savedInstanceState) {
		Log.d(MyApp.TAG + TAG, "onRestoreInstanceState()");
		if (savedInstanceState.getBoolean(PhotoCaptureActivity.PHOTO_TAKEN)) {
			onPhotoTaken();
		}
	}

	@Override
	protected void onSaveInstanceState(Bundle outState) {
		outState.putBoolean(PhotoCaptureActivity.PHOTO_TAKEN, _taken);
	}

	private void resetActivity() {
		_btnUpload.setText(R.string.takepic);
		_field.setText("");
		_image.setImageResource(R.drawable.nofoto);
		_path = null;
		_fileName = null;

	}

	@Override
	public void onNewPosition(double latitude, double longitude, double altitude) {
		// TODO Auto-generated method stub

	}
}