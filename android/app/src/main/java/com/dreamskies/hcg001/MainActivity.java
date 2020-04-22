package com.dreamskies.hcg001;

import android.os.Bundle;
import com.dreamskies.sky.SkyActivity;

public class MainActivity extends SkyActivity {    
	static {
        System.loadLibrary("hcg001");
    }
    @Override
    public void onCreate(Bundle savedInstance) {
        super.onCreate(savedInstance);

        // nothing
    }
}
