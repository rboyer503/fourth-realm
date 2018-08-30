package com.derelictvesseldev.fourthrealm;

import android.content.res.AssetManager;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.WindowManager;

public class MainActivity extends AppCompatActivity implements SensorEventListener {
    private static GameSurfaceView mView = null;
    private static SensorManager mSensorManager = null;
    private Sensor mRotationVectorSensor = null;
    private final float[] mRotationMatrix = new float[16];
    private final float[] mRotationMatrixMapped = new float[16];
    private final float[] mOrientationValues = new float[3];

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Get rotation vector sensor.
        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        mRotationVectorSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR);

        // Create basic surface for game display and handling touch events.
        mView = new GameSurfaceView(getApplication());
        setContentView(mView);

        // Make sure device doesn't go to sleep on us.
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        // Notify native code and supply asset manager.
        native_onCreate(getAssets());
    }

    @Override
    protected void onPause() {
        super.onPause();

        mView.onPause();
        mSensorManager.unregisterListener(this);

        native_onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();

        mView.onResume();
        mSensorManager.registerListener(this, mRotationVectorSensor, 10000);

        native_onResume();
    }

    public void onSensorChanged(SensorEvent event) {
        if (event.sensor.getType() == Sensor.TYPE_ROTATION_VECTOR) {
            SensorManager.getRotationMatrixFromVector(mRotationMatrix, event.values);

            SensorManager.remapCoordinateSystem(mRotationMatrix,
                    SensorManager.AXIS_X, SensorManager.AXIS_Z, mRotationMatrixMapped);

            SensorManager.getOrientation(mRotationMatrixMapped, mOrientationValues);
            double azimuth = Math.toDegrees(mOrientationValues[0]);
            double pitch = Math.toDegrees(mOrientationValues[1]);
            double roll = Math.toDegrees(mOrientationValues[2]);

            native_onRotationInput(azimuth, pitch, roll);
        }
    }

    public void onAccuracyChanged(Sensor sensor, int accuracy) {
    }

    private native void native_onCreate(AssetManager assetManager);

    private native void native_onPause();

    private native void native_onResume();

    private native void native_onRotationInput(double azimuth, double pitch, double roll);
}
