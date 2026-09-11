package org.leke.networkdefaults;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.UserHandle;
import android.provider.Settings;
import android.util.Log;

/** One-time migration for clean installs and existing OTA userdata. */
public final class BootReceiver extends BroadcastReceiver {
    @Override public void onReceive(Context context, Intent intent) {
        if (UserHandle.myUserId() != 0) return;
        SharedPreferences prefs = context.getSharedPreferences("migration", 0);
        if (prefs.getInt("version", 0) >= 1) return;
        try {
            put(context, "captive_portal_http_url", "http://connect.rom.miui.com/generate_204");
            put(context, "captive_portal_https_url", "https://connect.rom.miui.com/generate_204");
            put(context, "captive_portal_fallback_url", "http://connectivitycheck.platform.hicloud.com/generate_204");
            put(context, "captive_portal_other_fallback_urls", "http://connect.rom.miui.com/generate_204");
            put(context, "captive_portal_use_https", "1");
            put(context, "captive_portal_mode", "1");
            put(context, "captive_portal_detection_enabled", "1");
            put(context, "ntp_server", "ntp.aliyun.com");
            // Do not change automatic time/timezone preferences.
            if (!prefs.edit().putInt("version", 1).commit())
                throw new IllegalStateException("Cannot persist migration version");
            Log.i("LekeNetworkDefaults", "Applied domestic connectivity and NTP endpoints v1");
        } catch (RuntimeException e) {
            Log.e("LekeNetworkDefaults", "Migration failed; retry at next boot", e);
        }
    }
    private static void put(Context context, String key, String value) {
        if (!Settings.Global.putString(context.getContentResolver(), key, value))
            throw new IllegalStateException("Cannot write " + key);
    }
}
