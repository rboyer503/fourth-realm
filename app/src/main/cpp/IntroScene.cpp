//
// Created by Rob on 8/1/2018.
//

#include "IntroScene.h"
#include "GameConstants.h"


IntroScene::IntroScene(DGLGame *game)
        : Scene(game) {
}

void IntroScene::loadScene() {
    buildGameObjects();
    initGameState();

    surfaceCreated();
    surfaceChanged(mGame->getScreenSizeX(), mGame->getScreenSizeY());
    updateViewMatrix();

    mGame->flushTicks();
}

void IntroScene::backPressed() {
    mGame->triggerTerminate();
}

void IntroScene::surfaceCreated() {
    loadDrawList();
    buildShaders();
}

void IntroScene::tick() {
    switch (mState)
    {
        case STATE_PRIME:
            if (--mTickCount == 0)
            {
                mState = STATE_FADEIN;
            }
            break;

        case STATE_FADEIN:
            mBackgroundColor.red -= 0.01f;
            mBackgroundColor.green -= 0.01f;
            mBackgroundColor.blue -= 0.01f;
            GraphicsIntf->SetClearColor(mBackgroundColor);
            if (mBackgroundColor.red <= 0.0f)
            {
                mBackgroundColor = BLACK;
                mState = STATE_WAIT;
            }
            break;

        case STATE_FADEOUT:
            mBackgroundColor.red += 0.01f;
            mBackgroundColor.green += 0.01f;
            mBackgroundColor.blue += 0.01f;
            GraphicsIntf->SetClearColor(mBackgroundColor);
            if (mBackgroundColor.red >= 1.0f)
            {
                mBackgroundColor = WHITE;
                BasicSingle32<uint32_t> *currLevel = dynamic_cast<BasicSingle32<uint32_t> *>(mGame->getGameState()[FR_GS_CURR_LEVEL].get());
                currLevel->setData(currLevel->getData() + 1);

                mState = STATE_PRIME;
                mGame->triggerSceneSwitch(1);
            }
            break;

        default:
            break;
    }
}

void IntroScene::postTicks() {
    if (mUpdateView)
        updateViewMatrix();
}

void IntroScene::render() {
    GraphicsIntf->SetBlend(BLEND_MODE_ONE_ONE);
    // Not using -> because Android Studio *still* hasn't fixed this fucking bug.
    (*mShaderProgram).useProgram();
    (*mShaderProgram).setUniforms(mViewProjMatrix, mGame->getTextureIds()[FR_IMAGE_ASSET_INTRO]);
    (*mDrawList).bindData(*mShaderProgram);
    (*mDrawList).draw();
    GraphicsIntf->SetBlend(BLEND_MODE_NONE);
}

void IntroScene::touchInput(eInputEvent event, Vector2 &pos) {
    if (mState != STATE_WAIT)
        return;

    switch (event)
    {
        case INPUT_EVENT_TOUCH_UP:
            mState = STATE_FADEOUT;
            break;
        default:
            break;
    }
}

void IntroScene::buildGameObjects() {
    mBackground = std::make_shared<SquareObject>(Vector3(0.0f, 0.0f, -1.0f), Vector2(2.0f, 1.0f));
}

void IntroScene::initGameState() {
    mState = STATE_PRIME;
    mTickCount = 50;
    mCurrPos.set(1.0f, 0.5f, 0.0f);
    mCurrAngle = 0.0f;
}

void IntroScene::updateViewMatrix() {
    mViewMatrix.identity();
    mViewMatrix.translate(-mCurrPos);
    mViewMatrix.rotate(-mCurrAngle, 0.0f, 1.0f, 0.0f);

    mITViewMatrix = mViewMatrix;
    mITViewMatrix.invert().transpose();

    mViewProjMatrix = mProjMatrix * mViewMatrix;

    mUpdateView = false;
}

void IntroScene::loadDrawList() {
    mDrawList.reset(new DrawList({ATTR_POSITION, ATTR_TEXTUREUV}, true));
    (*mDrawList).addGameObject(mBackground);
    (*mDrawList).buildBuffers();
}

void IntroScene::buildShaders() {
    mShaderProgram.reset(new BasicShaderProgram());
}
