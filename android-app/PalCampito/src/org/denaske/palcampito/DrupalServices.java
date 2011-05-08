package org.denaske.palcampito;

import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
import java.util.ArrayList;
import java.util.HashMap;

import org.denaske.palcampito.base.LocationNode;
import org.denaske.palcampito.utils.Base64;
import org.denaske.palcampito.utils.Utils;
import org.json.JSONArray;
import org.json.JSONObject;
import org.xmlrpc.android.XMLRPCClient;
import org.xmlrpc.android.XMLRPCException;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;

public class DrupalServices {

	final static String C_SERVER = "http://denaske.metadrop.pro/services/xmlrpc";
	final static String C_URL_BASE = "http://denaske.metadrop.pro/android_map/";
	final static String C_URL_BASE_RANDOM = "http://denaske.metadrop.pro/android_random/";
	final static String C_URL_JSON = "/json";
	final static String C_IMAGE_BASE_PATH = "http://denaske.metadrop.pro/";

	XMLRPCClient client;
	boolean debug = false;
	private int MAX_RADIUS = 5000; 

	public DrupalServices() {

	}

	public DrupalServices(boolean debug) {
		this.debug = debug;
	}

	public HashMap<String, Object> userLogin(String user, String password) {

		// conexion
		client = new XMLRPCClient(C_SERVER);
		HashMap<String, Object> userObj = null;

		String sessid = null;
		try {
			HashMap<String, Object> data = (HashMap<String, Object>) client.call("user.login",
					user, password);
			sessid = (String) data.get("sessid");
			userObj = (HashMap<String, Object>) data.get("user");
			userObj.get("uid");

			if (debug) {
				Log.d("data", "" + data);
				Log.d("sessid", "" + sessid);
			}

		} catch (XMLRPCException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}

		return userObj;
	}

	public HashMap<String, Object> getNode(String nid) {

		HashMap<String, Object> data = null;
		// devuelve contenido del nodo
		try {
			data = (HashMap<String, Object>) client.call("node.get", "18");

			if (debug) {
				Log.d("data", "" + data);
			}
		} catch (XMLRPCException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}

		return data;
	}

	public Object getNodeContent(String nid) {

		Object data = null;

		// devuelve contenido del nodo
		try {
			data = (HashMap<String, Object>) client.call("node.get", "18", new String[] { "vid",
					"status" });

			if (debug) {
				Log.d("data", "" + data);
			}
		} catch (XMLRPCException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}

		return data;
	}

	public ArrayList<LocationNode> downloadJSON(String url) {

		ArrayList<LocationNode> locationNodes = new ArrayList<LocationNode>();

		try {

			URLConnection connection = new URL(url).openConnection();
			BufferedReader reader = new BufferedReader(new InputStreamReader(connection
					.getInputStream()), 1024 * 16);
			StringBuffer builder = new StringBuffer();
			String line;

			while ((line = reader.readLine()) != null) {
				builder.append(line).append("\n");
			}

			JSONObject jsonObject = new JSONObject(builder.toString());

			// JSONArray jsonArray = new JSONArray(builder.toString());

			// damm json, getting the first array element I get the object
			// JSONObject jsonObject = new JSONObject(jsonArray.optString(0));

			JSONArray jsonArray = jsonObject.getJSONArray("nodes");
			int size = jsonArray.length();
			for (int i = 0; i < size; i++) {

				Log.d(MyApp.TAG, "" + size + " " + i);

				JSONObject joarray = jsonArray.getJSONObject(i);
				JSONObject jo = joarray.getJSONObject("node");

				LocationNode locationNode = new LocationNode();
				locationNode.titulo = jo.getString("Titulo");
				locationNode.latitude = jo.getDouble("Latitude");
				locationNode.longitude = jo.getDouble("Longitude");
				locationNode.nid = jo.getString("Nid");
				// locationNode.cuerpo = jo.getString("Cuerpo");
				locationNode.tipo = jo.getString("Tipo");
				// Log.d(MyApp.TAG, "" + locationNode);

				locationNodes.add(locationNode);
			}

			//

			// Toast.makeText(this, object.getString("Name"), 12000).show();

		} catch (Exception e) {
			// TODO: handle exception
			Log.d("qq", "no entra" + e.getMessage());
		}

		return locationNodes;

	}

	Bitmap downloadImage(String url) {

		Bitmap bmp = null;

		URL myFileUrl = null;
		try {
			myFileUrl = new URL(url);
		} catch (MalformedURLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		try {
			HttpURLConnection conn = (HttpURLConnection) myFileUrl.openConnection();
			conn.setDoInput(true);
			conn.connect();
			// int length = conn.getContentLength();
			// int[] bitmapData = new int[length];
			// byte[] bitmapData2 = new byte[length];
			InputStream is = conn.getInputStream();

			bmp = BitmapFactory.decodeStream(is);

		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		return bmp;

	}

	public Object getLocationPointsJSON(Double latitude, Double longitude, int radius) {

		ArrayList<String> args = new ArrayList();
		String qq = latitude + "," + longitude + "_" + radius;

		return downloadJSON(C_URL_BASE + qq + C_URL_JSON);
	}

	public Object getRandomPointJSON(Double latitude, Double longitude, int radius) {

		ArrayList<String> args = new ArrayList();
		String qq = latitude + "," + longitude + "_" + MAX_RADIUS; 

		String url = C_URL_BASE_RANDOM + qq + C_URL_JSON; 
		Log.d(MyApp.TAG, "" + url); 
		return downloadJSON(url);
	}

	public Object getLocationPoints(Double latitude, Double longitude, int radius) {

		String data = null;

		ArrayList<String> args = new ArrayList();
		String qq = latitude + "," + longitude + "_" + radius;
		Log.d(MyApp.TAG, "" + qq);
		args.add(qq);
		String qq2[] = new String[] { qq };

		// devuelve contenido del nodo
		try {
			data = (String) client.call("views.get", "android_map", "page_2", qq2, "0", "10",
					"false");

			if (debug) {
				Log.d("data", "" + data);
			}
		} catch (XMLRPCException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}

		return data;
	}

	public String[] getImage(Bitmap user, String password) {

		// cogemos imagen
		try {
			HashMap<String, Object> data = (HashMap<String, Object>) client.call("node.get", "18",
					new String[] { "field_espacio_fotos" });
			// for (int _i = 0; _i < data.size(); _i++) {
			Object[] q = (Object[]) data.get("field_espacio_fotos");

			for (int _i = 0; _i < q.length; _i++) {
				HashMap<String, Object> images = (HashMap<String, Object>) q[_i];

				if (debug) {
					Log.d("foto", "qq" + images.get("filepath"));
				}
			}

		} catch (XMLRPCException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		return null;
	}

	public HashMap<String, Object> uploadImage(String _path, String filename,
			HashMap<String, Object> userObj) {

		// salva imagen en el nodo
		// tiempo y geolocalizacion
		// 

		// String _path = "/sdcard/DCIM/Camera/1303651305348.jpg";
		Log.d("qq", "este path: " + _path);
		// subo el archivo

		BitmapFactory.Options options = new BitmapFactory.Options();
		options.inSampleSize = 4;

		Bitmap bitmap = BitmapFactory.decodeFile(_path, options);

		ByteArrayOutputStream baos = new ByteArrayOutputStream();
		bitmap.compress(Bitmap.CompressFormat.JPEG, 100, baos); // bm is the
		// bitmap object
		byte[] b = baos.toByteArray();

		String encodedImage = Base64.encodeBytes(b);

		HashMap<String, Object> img = new HashMap<String, Object>();
		img.put("file", encodedImage);
		img.put("filename", filename);
		img.put("filepath", "sites/default/files/" + filename);
		img.put("uid", userObj.get("uid"));
		img.put("filemime", "image/jpeg");
		img.put("filesize", b.length * 8);
		img.put("timestamp", Utils.getTimeStamp());
		// img.put("status", "1");

		// filedata
		HashMap<String, Object> imgdata = new HashMap<String, Object>();
		imgdata.put("alt", "alt");
		imgdata.put("title", "title");
		img.put("data", imgdata);
		img.put("list", "1");

		// img.put("timestamp", );

		try {
			String imgId = (String) client.call("file.save", img);
			Log.d("data", "" + imgId);

			if (imgId != null) {
				img.put("fid", imgId);
			}

		} catch (XMLRPCException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		return img;

	} 

	public void nodeSave(HashMap<String, Object> imgObj, HashMap<String, Object> userObj,
			String title, String comment, String item_nid, double longitude, double latitude) {

		// imgStruct.put("timestamp", "");

		// Object [] images = new Object() {imgStruct};
		ArrayList<HashMap<String, Object>> images = new ArrayList();
		images.add(imgObj);

		// location
		HashMap<String, Object> location = new HashMap<String, Object>();
		location.put("longitude", longitude);
		location.put("latitude", latitude); 

		ArrayList<HashMap<String, Object>> locations = new ArrayList();
		locations.add(location);

		HashMap<String, Object> node = new HashMap<String, Object>();
		node.put("uid", userObj.get("uid"));
		node.put("name", userObj.get("name"));
		node.put("type", "nota");
		node.put("title", title);
		node.put("body", comment);
		node.put("location", location);
		node.put("locations", locations);

		if (imgObj != null) {
			if (imgObj.get("fid") != null) {
				node.put("field_nota_fotos", images);
			}
		}

		// anyado el nid del padre
		HashMap<String, Object> nid = new HashMap<String, Object>();
		nid.put("nid", ""+item_nid); 
		ArrayList<HashMap<String, Object>> nidArray = new ArrayList();
		nidArray.add(nid);
		node.put("field_nota_node_nid", nidArray);

		// subo
		String q;
		try {
			q = (String) client.call("node.save", node);
		} catch (XMLRPCException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}

}
