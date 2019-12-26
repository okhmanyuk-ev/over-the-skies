package com.skymunge.hcg001;

import android.app.NativeActivity;
import android.os.Bundle;
import android.content.Intent;

import com.android.billingclient.api.BillingClient;

import org.fmod.FMOD;

public class DummyActivity extends NativeActivity {
    static {
        System.loadLibrary("fmod");
    }
    private BillingClient billingClient;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Intent intent = new Intent(this, NativeActivity.class);
        startActivity(intent);

        //FMOD.init(this);
    }
}
