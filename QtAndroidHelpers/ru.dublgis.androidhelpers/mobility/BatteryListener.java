/*
    Offscreen Android Views library for Qt

    Authors:
    Evgeniy A. Samoylov <ghelius@gmail.com>

    Distrbuted under The BSD License

    Copyright (c) 2016, DoubleGIS, LLC.
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of the DoubleGIS, LLC nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
    BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
    THE POSSIBILITY OF SUCH DAMAGE.
*/

package ru.dublgis.androidhelpers.mobility;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.BatteryManager;
import android.util.Log;

public class BatteryListener extends BroadcastReceiver
{
    final private static String LOG_TAG = "Grym/BatteryListener";
    final private static boolean verbose_ = false;
    private long native_ptr_ = 0;
    private boolean started_ = false;

    public BatteryListener(long native_ptr)
    {
        native_ptr_ = native_ptr;
    }

    //! Called from C++ to notify us that the associated C++ object is being destroyed.
    public void cppDestroyed()
    {
        native_ptr_ = 0;
    }

    // start listening for battery info and report them
    public synchronized boolean start()
    {
        try
        {
            Log.d(LOG_TAG, "BatteryListener start");
            if (!started_) {
                getContext().registerReceiver(this, new IntentFilter(Intent.ACTION_BATTERY_CHANGED));
                started_ = true;
            } else {
                Log.d(LOG_TAG, "BatteryListener start: already started!");
            }
            return true;
        }
        catch (final Exception e)
        {
            Log.e(LOG_TAG, "Exception while starting BatteryListener: " + e);
            return false;
        }
    }

    public synchronized void stop()
    {
        Log.d(LOG_TAG, "BatteryListener stop");
        try
        {
            if (started_) {
                getContext().unregisterReceiver(this);
                started_ = false;
            } else {
               Log.d(LOG_TAG, "BatteryListener stop: was not started!");
            }
        }
        catch (final Exception e)
        {
            Log.e(LOG_TAG, "Exception while stopping: " + e);
        }
    }

    // on new battery info
    public synchronized void onReceive(Context c, Intent intent)
    {
        if (verbose_)
        {
            Log.d(LOG_TAG, "BatteryListener onReceive");
        }

        int level = intent.getIntExtra(BatteryManager.EXTRA_LEVEL, 0);
        boolean plugged = 0 != intent.getIntExtra(BatteryManager.EXTRA_PLUGGED, 0);
        batteryInfoUpdate(native_ptr_, plugged, level);
    }

    private native void batteryInfoUpdate(long native_ptr, boolean unplugged, int level);
    private native Context getContext();

} // class BatteryListener