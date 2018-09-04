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
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.SurfaceHolder;

public class GameSurfaceView extends GLSurfaceView {
    private final RendererWrapper mRenderer;

    public GameSurfaceView(Context context, Context activityContext) {
        super(context);
        setEGLContextClientVersion(2);
        mRenderer = new RendererWrapper(context, activityContext);
        setRenderer(mRenderer);
    }

    public GameSurfaceView(Context context, Context activityContext, AttributeSet attrs) {
        super(context, attrs);
        setEGLContextClientVersion(2);
        mRenderer = new RendererWrapper(context, activityContext);
        setRenderer(mRenderer);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        native_surfaceDestroyed();
        super.surfaceDestroyed(holder);
    }

    @Override
    public boolean onTouchEvent(MotionEvent e) {
        switch (e.getAction()) {
            case MotionEvent.ACTION_DOWN:
                native_onTouchInput(0, (int) e.getX(), (int) e.getY());
                break;

            case MotionEvent.ACTION_UP:
                native_onTouchInput(1, (int) e.getX(), (int) e.getY());
                break;

            case MotionEvent.ACTION_MOVE:
                native_onTouchInput(2, (int) e.getX(), (int) e.getY());
                break;
        }
        return true;
    }

    private static native void native_onTouchInput(int eventType, int pixel_x, int pixel_y);

    private static native void native_surfaceDestroyed();
}
