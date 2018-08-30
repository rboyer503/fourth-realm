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

#include <jni.h>
#include <memory>
#include <vector>

#include <android/asset_manager_jni.h>
#include <android/bitmap.h>

#include "Game.h"


extern "C" {

std::unique_ptr<Game> game;

JNIEXPORT void JNICALL
Java_com_derelictvesseldev_fourthrealm_MainActivity_native_1onCreate(JNIEnv *env, jobject instance,
                                                                     jobject jAssetManager) {
    //LOGD("ONCREATE");
    AAssetManager *assetManager = AAssetManager_fromJava(env, jAssetManager);
    game = std::make_unique<Game>(assetManager);
}

JNIEXPORT void JNICALL
Java_com_derelictvesseldev_fourthrealm_MainActivity_native_1onPause(JNIEnv *env, jobject instance) {
    //LOGD("ONPAUSE");
    (*game).onPause();
}

JNIEXPORT void JNICALL
Java_com_derelictvesseldev_fourthrealm_MainActivity_native_1onResume(JNIEnv *env,
                                                                     jobject instance) {
    //LOGD("ONRESUME");
    (*game).onResume();
}

JNIEXPORT void JNICALL
Java_com_derelictvesseldev_fourthrealm_RendererWrapper_native_1onSurfaceCreated(JNIEnv *env,
                                                                                jobject instance,
                                                                                jobjectArray bitmaps,
                                                                                jint count) {
    //LOGD("ONSURFACECREATED");
    AndroidBitmapInfo info = {0};
    void *pixels = nullptr;
    std::vector<DGLRawBitmap> rawBitmaps;
    std::vector<jobject> lockedBitmaps;

    for (int i = 0; i < count; ++i)
    {
        jobject bitmap = env->GetObjectArrayElement(bitmaps, i);

        pixels = nullptr;
        AndroidBitmap_getInfo(env, bitmap, &info);
        //LOGI("BITMAP: Width=%d, Height=%d, Stride=%d, Format=%d", info.width, info.height,
        //     info.stride, info.format);

        if ((info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) &&
            (info.format != ANDROID_BITMAP_FORMAT_A_8))
        {
            LOGE("Invalid bitmap format %d!", info.format);
        }
        else
        {
            AndroidBitmap_lockPixels(env, bitmap, &pixels);
        }

        if (!pixels)
        {
            LOGE("Cannot lock bitmap pixels!");
        }
        else
        {
            eDGLBitmapFormat format;
            switch (info.format)
            {
                case ANDROID_BITMAP_FORMAT_RGBA_8888:
                    format = BITMAP_FORMAT_RGBA_8888;
                    break;
                case ANDROID_BITMAP_FORMAT_A_8:
                    format = BITMAP_FORMAT_A_8;
                    break;
                default:
                    format = BITMAP_FORMAT_NONE;
                    break;
            }
            rawBitmaps.push_back({info.width, info.height, format, pixels});
            lockedBitmaps.push_back(bitmap);
        }
    }

    (*game).onSurfaceCreated(rawBitmaps);

    for (jobject bitmap : lockedBitmaps)
        AndroidBitmap_unlockPixels(env, bitmap);
}

JNIEXPORT void JNICALL
Java_com_derelictvesseldev_fourthrealm_RendererWrapper_native_1onSurfaceChanged(JNIEnv *env,
                                                                                jclass type,
                                                                                jint width,
                                                                                jint height) {
    //LOGD("ONSURFACECHANGED");
    (*game).onSurfaceChanged(width, height);
}

JNIEXPORT void JNICALL
Java_com_derelictvesseldev_fourthrealm_GameSurfaceView_native_1surfaceDestroyed__(JNIEnv *env,
                                                                                  jclass type) {
    //LOGD("ONSURFACEDESTROYED");
    (*game).onSurfaceDestroyed();
}

JNIEXPORT void JNICALL
Java_com_derelictvesseldev_fourthrealm_RendererWrapper_native_1onDrawFrame(JNIEnv *env,
                                                                           jclass type) {
    //LOGD("ONDRAWFRAME");
    (*game).onTick();
}

JNIEXPORT void JNICALL
Java_com_derelictvesseldev_fourthrealm_GameSurfaceView_native_1onTouchInput(JNIEnv *env,
                                                                            jclass type,
                                                                            jint event_type,
                                                                            jint pixel_x,
                                                                            jint pixel_y) {
    //LOGD("ONTOUCHINPUT");
    (*game).onTouchInput(static_cast<eInputEvent>(event_type), pixel_x, pixel_y);
}

JNIEXPORT void JNICALL
Java_com_derelictvesseldev_fourthrealm_MainActivity_native_1onRotationInput(JNIEnv *env,
                                                                            jobject instance,
                                                                            jdouble azimuth,
                                                                            jdouble pitch,
                                                                            jdouble roll) {
    //LOGD("ONROTATIONINPUT");
    (*game).onRotationInput(azimuth, pitch, roll);
}

}
