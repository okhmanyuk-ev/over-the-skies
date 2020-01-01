package com.skymunge.hcg001;

import android.app.NativeActivity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;

import com.android.billingclient.api.BillingClient;
import com.android.billingclient.api.BillingClientStateListener;
import com.android.billingclient.api.BillingResult;
import com.android.billingclient.api.Purchase;
import com.android.billingclient.api.PurchasesUpdatedListener;

import androidx.annotation.Nullable;

import java.util.List;

public class MainActivity extends NativeActivity {
    static {
        System.loadLibrary("fmod");
    }

    private BillingClient billingClient;

    @Override
    public void onCreate(Bundle savedInstance) {
        super.onCreate(savedInstance);

      /*  Dialog dialog = new AlertDialog.Builder(this)
                .setMessage("awdawd")
                .setPositiveButton("ok", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {

                    }
                })
                .setNegativeButton("cancel", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        // User cancelled the dialog
                    }
                }).create();

        dialog.show();*/







      /*  billingClient = BillingClient.newBuilder(this).setListener(new PurchasesUpdatedListener() {
            @Override
            public void onPurchasesUpdated(BillingResult result, @Nullable List<Purchase> purchases){
                if (result.getResponseCode() == BillingClient.BillingResponseCode.OK && purchases != null) {
                    //сюда мы попадем когда будет осуществлена покупк
                }
            }
            //public void onPurchasesUpdated(int responseCode, @Nullable List<Purchase> purchases) {

            //}
        }).build();
*/




/*
        billingClient.startConnection(new BillingClientStateListener() {
            @Override
            public void onBillingSetupFinished(BillingResult result) {
                if (result.getResponseCode() == BillingClient.BillingResponseCode.OK) {
                    // The BillingClient is ready. You can query purchases here.
                }
            }
            @Override
            public void onBillingServiceDisconnected() {
                // Try to restart the connection on the next request to
                // Google Play by calling the startConnection() method.
            }
        });*/
    }
}
