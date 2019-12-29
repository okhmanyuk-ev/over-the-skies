package com.skymunge.hcg001;

import android.app.NativeActivity;

public class MainActivity extends NativeActivity {
    static {
        System.loadLibrary("fmod");
    }
}
