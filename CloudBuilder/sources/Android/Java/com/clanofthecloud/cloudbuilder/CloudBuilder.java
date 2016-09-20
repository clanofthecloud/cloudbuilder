package com.clanofthecloud.cloudbuilder;

import android.app.Activity;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.provider.Settings.Secure;
import android.util.Log;

import com.clanofthecloud.cloudbuilder.iab.IabHelper;
import com.clanofthecloud.cloudbuilder.pushnotifications.Controller;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;

public abstract class CloudBuilder {

	private static final String TAG = "Cloudbuilder";

    private static native int RegisterDevice(String aToken);

    public static native int Suspended();

    public static native int Resumed();

	// Works for a CInternalResultHandler handler only!
	public static native void InvokeHandler(long handlerId, String resultStr);

    public static boolean sGCMAvailable = false;

	static Activity sActivity = null;
	static Controller sController = null;
	public static boolean sVerboseLog = true;

	/**
	 * Call this upon initialization.
	 * @param activity your main activity; used for various purchases such as a parent window for
	 *                 in-app billing operations.
	 */
	public static void Init (Activity activity) {
		// TODO make this call mandatory
		sActivity = activity;
		sController = new Controller(activity);

		Log.v(TAG, "Launching CloudBuilder Java v3.1.0");

		// Check whether the GCM is available (config in the manifest)
		sGCMAvailable = false;
		try {
			ApplicationInfo ai = activity.getPackageManager().getApplicationInfo(activity.getPackageName(), PackageManager.GET_META_DATA);
			sGCMAvailable = ai != null && ai.metaData != null && ai.metaData.get("cotc.GcmSenderId") != null;
		} catch (PackageManager.NameNotFoundException nnfe) {}

		Log.d(TAG, sGCMAvailable ?
			"GCM support (push notifications) is enabled." :
			"cotc.GcmSenderId not configured in manifest, GCM push notifications disabled.");
	}

	/**
	 * Call this method to enable or disable verbose logging.
	 * @param verboseLog whether to extensively log android-related features
	 */
	public static void setVerboseLog(boolean verboseLog) {
		CloudBuilder.sVerboseLog = verboseLog;
	}

	public static Context GetContext() {
		if (sActivity == null) {
			Log.e(TAG, "!!!!!! Failed to retrieve context. Check that you called CloudBuilder.Init. You may need a custom activity. App will crash. !!!!!!");
			throw new RuntimeException("Failed to retrieve context, please check logs");
		}
		return sActivity;
	}

	// Used in CAndroidFilesystemHandlerImpl.cpp
	public static String GetDataDirectory() {
		return GetContext().getFilesDir().getAbsolutePath();
	}

	public static boolean CreateDirectory(String fullName) {
		return new File(fullName).mkdirs();
	}

	public static boolean DeleteFile(String fullName) {
		return new File(fullName).delete();
	}
	// End Used

	// Used in CloudBuilderJNI.cpp
	// Handler is called with {"token": <the token: string>}
	public static void QueryRegisterDevice() {
		if (sVerboseLog) Log.v(TAG, "QueryRegisterDevice, get ID with GCMRegistrar");

		if (sGCMAvailable) {
			sController.startRegistration(new IabHelper.CloudResultListener() {
				@Override
				public void onDone(EErrorCode code, JSONObject result, String message) {
					try {
						RegisterDevice(result.getString("token"));
					} catch (JSONException e) {
						Log.e(TAG, "Could not retrieve token in QueryRegisterDevice");
						e.printStackTrace();
					}
				}
			});
		}
	}

	public static String CollectDeviceInformation()
	{
		JSONObject json = new JSONObject();
		try {
			json.put("id", Secure.getString(GetContext().getContentResolver(), Secure.ANDROID_ID));
			json.put("osname", "Android");
			json.put("osversion", Integer.toString(android.os.Build.VERSION.SDK_INT));
			json.put("model", android.os.Build.MODEL);
			json.put("version", "1");
		} catch(JSONException e) {
			e.printStackTrace();
		}

        return json.toString();
	}

	static void UnregisterDevice() {
		if (sVerboseLog) Log.v(TAG, "Unregistering device");
		if (sGCMAvailable) {
			sController.startUnregistration(new IabHelper.CloudResultListener() {
				@Override
				public void onDone(EErrorCode code, JSONObject result, String message) {
					if (sVerboseLog) Log.v(TAG, "Finished unregistration");
				}
			});
		}
	}
	// End used

	// Use these functions to forward a call to a native CInternalResultHandler from the java code
	public static void InvokeHandler(long handlerId, EErrorCode code, JSONObject result, String message) {
		try {
			result = result != null ? result : new JSONObject();
			result.put("_error", code.getCode());
			if (message != null) { result.put("_description", message); }
			InvokeHandler(handlerId, result.toString());
		} catch (JSONException e) {
			// Thrown if the key is null, which we know isn't bound to happen…
			e.printStackTrace();
		}
	}

	// Works for a CInternalResultHandler handler only!
	public static void InvokeHandler(long handlerId, EErrorCode code, String message) { InvokeHandler(handlerId, code, null, message); }
	public static void InvokeHandler(long handlerId, EErrorCode code, JSONObject result) { InvokeHandler(handlerId, code, result, null); }
}
