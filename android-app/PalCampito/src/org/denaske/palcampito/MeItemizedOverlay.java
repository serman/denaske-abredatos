package org.denaske.palcampito;

import java.util.ArrayList;

import org.denaske.palcampito.base.AppConfig;
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

import com.google.android.maps.GeoPoint;
import com.google.android.maps.ItemizedOverlay;
import com.google.android.maps.MapView;
import com.google.android.maps.OverlayItem;

public class MeItemizedOverlay extends ItemizedOverlay {

	Context mContext;
	private ArrayList<OverlayItem> mOverlays = new ArrayList<OverlayItem>();

	public MeItemizedOverlay(Context c, Drawable defaultMarker) {
		// super(defaultMarker);
		super(boundCenterBottom(defaultMarker));

		mContext = c;

	}

	public void addOverlay(OverlayItem overlay) {
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
	protected OverlayItem createItem(int i) {
		return mOverlays.get(i);
	}

	@Override
	public int size() {
		return mOverlays.size();
	}

	// tappint on to an icon
	@Override
	protected boolean onTap(int index) {
		OverlayItem item = mOverlays.get(index);

		GeoPoint g = item.getPoint();

		AlertDialog.Builder dialog = new AlertDialog.Builder(mContext); 
		dialog.setIcon(mContext.getResources().getDrawable(R.drawable.ic_menu_view)); 
	
		dialog.setNegativeButton(R.string.volver, new OnClickListener() {

			@Override
			public void onClick(DialogInterface dialog, int which) {

			}
		}); 
		
//		dialog.setPositiveButton(R.string.chekin, new OnClickListener() {
//
//			@Override
//			public void onClick(DialogInterface dialog, int which) {
//				mContext.startActivity(new Intent(mContext, NodeDetailActivity.class)); 
//			}
//		}); 

		dialog.setCancelable(true);

		dialog.setTitle(R.string.chekin);
		dialog.setMessage(R.string.chekin_explanation);
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
