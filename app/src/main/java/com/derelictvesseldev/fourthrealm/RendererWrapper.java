/*
 * Copyright 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Modified June 22, 2018 by Derelict Vessel Dev for "Fourth Realm".
 * Original from Oboe RhythmGame sample.
 */

package com.derelictvesseldev.fourthrealm;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLSurfaceView;
import androidx.appcompat.app.AppCompatActivity;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class RendererWrapper implements GLSurfaceView.Renderer {
    private static final int NUM_BITMAPS = 6;
    private final Context context;
    private final Context activityContext;

    public RendererWrapper(Context context, Context activityContext) {
        this.context = context;
        this.activityContext = activityContext;
    }

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        final BitmapFactory.Options options = new BitmapFactory.Options();
        options.inScaled = false;

        Bitmap[] bitmaps = new Bitmap[NUM_BITMAPS];
        bitmaps[0] = BitmapFactory.decodeResource(context.getResources(), R.drawable.font, options);
        bitmaps[1] = BitmapFactory.decodeResource(context.getResources(), R.drawable.buttons, options);
        bitmaps[2] = BitmapFactory.decodeResource(context.getResources(), R.drawable.hud, options);
        bitmaps[3] = BitmapFactory.decodeResource(context.getResources(), R.drawable.intro, options);
        bitmaps[4] = BitmapFactory.decodeResource(context.getResources(), R.drawable.stone1, options);
        bitmaps[5] = BitmapFactory.decodeResource(context.getResources(), R.drawable.circle1, options);

        native_onSurfaceCreated(bitmaps, NUM_BITMAPS);
    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int width, int height) {
        native_onSurfaceChanged(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl10) {
        if (native_onDrawFrame())
        {
            AppCompatActivity activity = (AppCompatActivity)activityContext;
            activity.finishAndRemoveTask();
        }
    }

    private static native void native_onSurfaceCreated(Bitmap[] bitmaps, int count);

    private static native void native_onSurfaceChanged(int widthInPixels, int heightInPixels);

    private static native boolean native_onDrawFrame();
}
