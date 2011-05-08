package org.denaske.palcampito;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;

public class ServerService extends Service {


	public class LocalBinder extends Binder {
		public ServerService getService() {
			// Return this instance of LocalService so clients can call public
			// methods
			return ServerService.this;
		}

	}


	private ServerListener listener = null;



	private final IBinder mBinder = (IBinder) new LocalBinder();

	@Override
	public IBinder onBind(Intent arg0) {
		// TODO Auto-generated method stub
		return mBinder; 
	}

	@Override
	public void onCreate() {
		super.onCreate();



	}

	@Override
	public void onDestroy() {
		super.onDestroy();


	} 
	
	@Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.i("LocalService", "Received start id " + startId + ": " + intent);
        // We want this service to continue running until it is explicitly
        // stopped, so return sticky.
        return START_STICKY;
    }


	public void startServer(int port) {
	
	}

	public void setListener(Object listener) {
		this.listener = (ServerListener) listener;

	}

}
