
package com.th000.keycontainer;

import android.app.Application;
import android.app.Activity;
import android.os.Bundle;
import android.view.WindowManager;
import android.content.Context;

public class SelfApplication extends org.qtproject.qt5.android.bindings.QtApplication {
    private  Context context;
    public void onCreate() {
        super.onCreate();
        context = getApplicationContext();
        setupActivityListener();
    }

    private void setupActivityListener() {
        registerActivityLifecycleCallbacks(new ActivityLifecycleCallbacks() {
            @Override
            public void onActivityCreated(Activity activity, Bundle savedInstanceState) {
                activity.getWindow().setFlags(WindowManager.LayoutParams.FLAG_SECURE, WindowManager.LayoutParams.FLAG_SECURE);
            }
            @Override
            public void onActivityStarted(Activity activity) {
            }
            @Override
            public void onActivityResumed(Activity activity) {

            }
            @Override
            public void onActivityPaused(Activity activity) {

            }
            @Override
            public void onActivityStopped(Activity activity) {
            }
            @Override
            public void onActivitySaveInstanceState(Activity activity, Bundle outState) {
            }
            @Override
            public void onActivityDestroyed(Activity activity) {
            }
        });
    }

}
