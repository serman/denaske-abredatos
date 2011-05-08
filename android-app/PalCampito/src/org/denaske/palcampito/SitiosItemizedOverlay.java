package org.denaske.palcampito;

import java.util.ArrayList;

import org.denaske.palcampito.base.AppConfig;
import org.denaske.palcampito.base.MyOverlayItem;
import org.denaske.palcampito.ui.MyCustomDialog;
import org.denaske.palcampito.ui.NodeDetailActivity;
import org.denaske.palcampito.ui.PhotoCaptureActivity;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.DialogInterface.OnClickListener;
import android.graphics.Point;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.util.Log;

import com.google.android.maps.GeoPoint;
import com.google.android.maps.ItemizedOverlay;
import com.google.android.maps.MapView;

public class SitiosItemizedOverlay extends ItemizedOverlay {

	Context mContext;
	MyCustomDialog myCustomDialog;

	private ArrayList<MyOverlayItem> mOverlays = new ArrayList<MyOverlayItem>();

	public SitiosItemizedOverlay(Context c, Drawable defaultMarker) {
		// super(defaultMarker);
		super(boundCenterBottom(defaultMarker));

		mContext = c;
		myCustomDialog = new MyCustomDialog(mContext);
	}

	public void addOverlay(MyOverlayItem overlay) {
		mOverlays.add(overlay);
		populate(); // calls create item
	}

	public void clear() {
		mOverlays.clear();
	}

	public void removePoint(int i) {
		mOverlays.remove(i);
	}

	@Override
	protected MyOverlayItem createItem(int i) {
		return mOverlays.get(i);
	}

	@Override
	public int size() {
		return mOverlays.size();
	}

	// tappint on to an icon
	@Override
	protected boolean onTap(int index) {
		MyOverlayItem item = mOverlays.get(index);
		final String nid = item.getNid();

		Log.d(MyApp.TAG, "nid" + nid);

		GeoPoint g = item.getPoint();

		AlertDialog.Builder dialog = new AlertDialog.Builder(mContext);
		dialog.setIcon(mContext.getResources().getDrawable(R.drawable.ic_menu_view));

		dialog.setNegativeButton(R.string.volver, new OnClickListener() {

			@Override
			public void onClick(DialogInterface dialog, int which) {

			}
		});

		// dialog.setPositiveButton(R.string.vermas, new OnClickListener() {
		//
		// @Override
		// public void onClick(DialogInterface dialog, int which) {
		// mContext.startActivity(new Intent(mContext,
		// NodeDetailActivity.class));
		// }
		// });

		dialog.setNeutralButton(R.string.anyadir, new OnClickListener() {

			@Override
			public void onClick(DialogInterface dialog, int which) {
				// mContext.startActivity(new Intent(mContext,
				// PhotoCaptureActivity.class));

				if (AppConfig.logged == 1) {

					Bundle bundle = new Bundle();
					bundle.putString("param1", nid);

					Intent newIntent = new Intent(mContext, PhotoCaptureActivity.class);
					newIntent.putExtras(bundle);
					mContext.startActivity(newIntent);
				} else { 
					myCustomDialog.show(); 
				}

			}

		});

		dialog.setCancelable(true);

		dialog.setTitle(item.getTitle());
		dialog.setMessage(item.getSnippet());
		dialog.show();

		return true;
	}

	@Override
	public boolean onTap(GeoPoint p, MapView mapView) {
		// MyMapActivity.showDialog(mapView);
		// mContext.startActivity(new Intent(mContext,
		// NodeDetailActivity.class));
		// mapView.

		return super.onTap(p, mapView);
	}

	@Override
	public boolean onSnapToItem(int x, int y, Point snapPoint, MapView mapView) {
		return super.onSnapToItem(x, y, snapPoint, mapView);
	}
}
