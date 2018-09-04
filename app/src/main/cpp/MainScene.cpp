//
// Created by Rob on 7/30/2018.
//

#include "MainScene.h"
#include "objects/Wall.h"
#include "GameConstants.h"


constexpr float DEG2RAD = 3.141593f / 180.0f;
constexpr int FRONT = 0;
constexpr int BACK = 1;
constexpr int RIGHT = 2;
constexpr int LEFT = 3;
constexpr int TOP = 4;
constexpr int BOTTOM = 5;
constexpr int MAX_DIR = 6;


MainScene::MainScene(DGLGame *game)
        : Scene(game),
          mHUDFrameMapInfo({Vector2(0.125f, 0.125f), 8, 8}),
          mButtonsFrameMapInfo({Vector2(0.5f, 0.125f), 2, 8}) {
    initSounds();
    initTextObjects();
}

void MainScene::loadScene() {
    mGame->loadSounds(mSoundMetadataVec);
    mGame->loadTextObjects(mTextObjects);

    mGlobalTone.alpha = 1.0f;
    mGame->updateGlobalTone(mGlobalTone);

    mLevelColor = {0.25f, 0.25f, 1.0f, 1.0f};
    mTextObjects[1]->updateColor(mLevelColor);

    buildGameObjects();
    initGameState();

    surfaceCreated();
    surfaceChanged(mGame->getScreenSizeX(), mGame->getScreenSizeY());
    updateViewMatrix();

    mGame->flushTicks();
}

void MainScene::unloadScene() {
    surfaceDestroyed();

    mPhysicsEngine->removeAllPhysicals();
    destroyGameObjects();

    mGame->unloadTextObjects();
    mGame->unloadAllSounds();
}

void MainScene::pause() {
}

void MainScene::resume() {
}

void MainScene::backPressed() {
    for (auto &buttonObject : mButtonObjects)
        (*mButtonDrawList).addGameObject(buttonObject);

    addTouchable(mButtonObjects[0]);
    addTouchable(mButtonObjects[1]);

    mGame->onPause(true);
}

void MainScene::surfaceCreated() {
    loadDrawList();
    buildShaders();
}

void MainScene::preTicks() {
    if (mCancelExit)
    {
        mCancelExit = false;
        mGame->backPressHandled();
        mGame->onResume(true);

        removeTouchable(mButtonObjects[0]);
        removeTouchable(mButtonObjects[1]);
        (*mButtonDrawList).removeAllGameObjects();
    }

    if (mGame->isPaused())
        return;

    // Input management.
    if (mGestureEvent)
    {
        mGestureEvent = false;
        if (mActiveMotion == MOTION_EVENT_NONE)
        {
            bool processed = false;
            Vector2 touchDiff = mTouchCurrPos - mTouchStartPos;
            if (abs(touchDiff.x) > abs(touchDiff.y))
            {
                if (touchDiff.x > FRGC_SWIPE_THRES)
                {
                    processed = true;
                    mActiveMotion = MOTION_EVENT_LEFT;
                    mMotionTarget = mCurrAngle + 90.0f;
                    if (mMotionTarget >= 361.0f)
                        mMotionTarget = 90.0f;

                    // TEMP
                    /*
                    mVolume += 0.25f;
                    if (mVolume > 1.0f)
                        mVolume = 1.0f;
                    for (int i = 0; i < mSoundMetadataVec.size(); ++i)
                        mGame->setSoundVolume(i, mVolume);

                    mGlobalTone.alpha += 0.05;
                    if (mGlobalTone.alpha >= 1.0f)
                        mGlobalTone.alpha = 1.0f;
                    mGame->updateGlobalTone(mGlobalTone);
                     */
                }
                else if (touchDiff.x < -FRGC_SWIPE_THRES)
                {
                    processed = true;
                    mActiveMotion = MOTION_EVENT_RIGHT;
                    mMotionTarget = mCurrAngle - 90.0f;
                    if (mMotionTarget < -1.0f)
                    {
                        mMotionTarget = 270.0f;
                        mCurrAngle = 360.0f;
                    }

                    // TEMP
                    /*
                    mVolume -= 0.25f;
                    if (mVolume < 0.0f)
                        mVolume = 0.0f;
                    for (int i = 0; i < mSoundMetadataVec.size(); ++i)
                        mGame->setSoundVolume(i, mVolume);

                    mGlobalTone.alpha -= 0.05;
                    if (mGlobalTone.alpha < 0.0f)
                        mGlobalTone.alpha = 0.0f;
                    mGame->updateGlobalTone(mGlobalTone);
                     */
                }
            }
            else
            {
                if (touchDiff.y < -FRGC_SWIPE_THRES)
                {
                    processed = true;
                    if (!((*mMaze).getMazeMatrix()->at(mCurrRoom) & Maze::MAZE_BOTTOM))
                    {
                        mActiveMotion = MOTION_EVENT_DOWN;
                        mMotionTarget = mCurrPos.y - mRoomOffset.y;
                    }
                }
                else if (touchDiff.y > FRGC_SWIPE_THRES)
                {
                    processed = true;
                    if (!((*mMaze).getMazeMatrix()->at(mCurrRoom) & Maze::MAZE_TOP))
                    {
                        mActiveMotion = MOTION_EVENT_UP;
                        mMotionTarget = mCurrPos.y + mRoomOffset.y;
                    }
                }
            }
            if (!processed)
            {
                uint32_t dir;
                //if ((mTouchStartPos.y > 0.7f) && (mTouchStartPos.x < -0.75f))
                if (mTouchStartPos.y > 0.5f)
                {
                    mActiveMotion = MOTION_EVENT_BACKWARD;
                    dir = Maze::getOppositeWall(mCurrFacing);
                }
                else
                {
                    mActiveMotion = MOTION_EVENT_FORWARD;
                    dir = mCurrFacing;
                }

                if ((*mMaze).getMazeMatrix()->at(mCurrRoom) & dir)
                {
                    mActiveMotion = MOTION_EVENT_NONE;
                }
                else
                {
                    switch (dir)
                    {
                        case Maze::MAZE_FRONT:
                            mMotionTarget = mCurrPos.z - mRoomOffset.x;
                            break;

                        case Maze::MAZE_BACK:
                            mMotionTarget = mCurrPos.z + mRoomOffset.x;
                            break;

                        case Maze::MAZE_RIGHT:
                            mMotionTarget = mCurrPos.x + mRoomOffset.x;
                            break;

                        case Maze::MAZE_LEFT:
                            mMotionTarget = mCurrPos.x - mRoomOffset.x;
                            break;

                        default:
                            break;
                    }
                }
            }
        }
    }
}

void MainScene::tick() {
    if (mGame->isPaused())
        return;

    switch (mState)
    {
        case STATE_FADEIN:
            mGlobalTone.alpha -= 0.02f;
            if (mGlobalTone.alpha <= 0.0f)
            {
                mGlobalTone.alpha = 0.0f;
                mState = STATE_SHOWLEVEL;
            }
            mGame->updateGlobalTone(mGlobalTone);
            return;

        case STATE_SHOWLEVEL:
            mLevelColor.red *= 0.98f;
            mLevelColor.green *= 0.98f;
            mLevelColor.blue *= 0.98f;
            if (mLevelColor.blue <= 0.01f)
            {
                mLevelColor = BLACK;
                mState = STATE_PLAY;
            }
            mTextObjects[1]->updateColor(mLevelColor);
            break;

        case STATE_FADEOUT:
            mGlobalTone.alpha += 0.02f;
            if (mGlobalTone.alpha >= 1.0f)
            {
                mGlobalTone.alpha = 1.0f;
                mGame->triggerSceneSwitch(1);
            }
            mGame->updateGlobalTone(mGlobalTone);
            return;

        default:
            break;
    }

    updatePlaylist();
    updateForces();

    if (mParticles[HELPER_SPARK]->isActive())
    {
        float intensity = mParticles[HELPER_SPARK]->getIntensity();
        intensity *= 0.999f;
        mParticles[HELPER_SPARK]->setIntensity(intensity);

        if (intensity <= 0.001f)
        {
            mParticles[HELPER_SPARK]->setActive(false);
            intensity = 0.0f;
        }

        mPointLightColor[0] = mBaselineColors[HELPER_SPARK].red * intensity;
        mPointLightColor[1] = mBaselineColors[HELPER_SPARK].green * intensity;
        mPointLightColor[2] = mBaselineColors[HELPER_SPARK].blue * intensity;

        float helperDist = (mParticles[HELPER_SPARK]->getPosition().distance(mCurrPos));
        if (helperDist == 0.0f)
            helperDist = 0.0001f;
        mVolume = (1.0f / (helperDist * helperDist));
        if (mVolume >= 1.0f)
            mVolume = 1.0f;
        mVolume *= (intensity + 1.0f) / 2.0f;
        mGame->setSoundVolume(9, mVolume);
    }

    if (mActiveMotion != MOTION_EVENT_NONE)
    {
        switch (mActiveMotion)
        {
            case MOTION_EVENT_FORWARD:
                mCurrPos += mCurrDir * 0.08f;
                switch (mCurrFacing)
                {
                    case Maze::MAZE_FRONT:
                        if (mCurrPos.z <= mMotionTarget)
                        {
                            mCurrPos.z = mMotionTarget;
                            mCurrRoom.z++;
                            mActiveMotion = MOTION_EVENT_NONE;
                        }
                        break;
                    case Maze::MAZE_RIGHT:
                        if (mCurrPos.x >= mMotionTarget)
                        {
                            mCurrPos.x = mMotionTarget;
                            mCurrRoom.x++;
                            mActiveMotion = MOTION_EVENT_NONE;
                        }
                        break;
                    case Maze::MAZE_BACK:
                        if (mCurrPos.z >= mMotionTarget)
                        {
                            mCurrPos.z = mMotionTarget;
                            mCurrRoom.z--;
                            mActiveMotion = MOTION_EVENT_NONE;
                        }
                        break;
                    case Maze::MAZE_LEFT:
                        if (mCurrPos.x <= mMotionTarget)
                        {
                            mCurrPos.x = mMotionTarget;
                            mCurrRoom.x--;
                            mActiveMotion = MOTION_EVENT_NONE;
                        }
                        break;
                    default:
                        break;
                }
                break;

            case MOTION_EVENT_BACKWARD:
                mCurrPos -= mCurrDir * 0.08f;
                switch (mCurrFacing)
                {
                    case Maze::MAZE_FRONT:
                        if (mCurrPos.z >= mMotionTarget)
                        {
                            mCurrPos.z = mMotionTarget;
                            mCurrRoom.z--;
                            mActiveMotion = MOTION_EVENT_NONE;
                        }
                        break;
                    case Maze::MAZE_RIGHT:
                        if (mCurrPos.x <= mMotionTarget)
                        {
                            mCurrPos.x = mMotionTarget;
                            mCurrRoom.x--;
                            mActiveMotion = MOTION_EVENT_NONE;
                        }
                        break;
                    case Maze::MAZE_BACK:
                        if (mCurrPos.z <= mMotionTarget)
                        {
                            mCurrPos.z = mMotionTarget;
                            mCurrRoom.z++;
                            mActiveMotion = MOTION_EVENT_NONE;
                        }
                        break;
                    case Maze::MAZE_LEFT:
                        if (mCurrPos.x >= mMotionTarget)
                        {
                            mCurrPos.x = mMotionTarget;
                            mCurrRoom.x++;
                            mActiveMotion = MOTION_EVENT_NONE;
                        }
                        break;
                    default:
                        break;
                }
                break;

            case MOTION_EVENT_RIGHT:
                mCurrAngle -= 1.5f;
                if (mCurrAngle <= mMotionTarget)
                {
                    mCurrAngle = mMotionTarget;
                    mCurrDir.x = -sinf(mCurrAngle * DEG2RAD);
                    mCurrDir.z = -cosf(mCurrAngle * DEG2RAD);
                    mActiveMotion = MOTION_EVENT_NONE;
                    mCurrFacing = Maze::getRightWall(mCurrFacing);
                }
                break;

            case MOTION_EVENT_LEFT:
                mCurrAngle += 1.5f;
                if (mCurrAngle >= mMotionTarget)
                {
                    mCurrAngle = mMotionTarget;
                    if (mCurrAngle >= 360.0f)
                        mCurrAngle = 0.0f;
                    mCurrDir.x = -sinf(mCurrAngle * DEG2RAD);
                    mCurrDir.z = -cosf(mCurrAngle * DEG2RAD);
                    mActiveMotion = MOTION_EVENT_NONE;
                    mCurrFacing = Maze::getLeftWall(mCurrFacing);
                }
                break;

            case MOTION_EVENT_UP:
                mCurrPos.y += 0.1f;
                if (mCurrPos.y >= mMotionTarget)
                {
                    mCurrPos.y = mMotionTarget;
                    mCurrRoom.y++;
                    mActiveMotion = MOTION_EVENT_NONE;
                }
                break;

            case MOTION_EVENT_DOWN:
                mCurrPos.y -= 0.1f;
                if (mCurrPos.y <= mMotionTarget)
                {
                    mCurrPos.y = mMotionTarget;
                    mCurrRoom.y--;
                    mActiveMotion = MOTION_EVENT_NONE;
                }
                break;

            default:
                break;
        }

        mUpdateView = true;

        if (mActiveMotion == MOTION_EVENT_NONE)
        {
            if (mCurrRoom == (*mMaze).getGoal())
            {
                // Next level.
                BasicSingle32<uint32_t> *currLevel = dynamic_cast<BasicSingle32<uint32_t> *>(mGame->getGameState()[FR_GS_CURR_LEVEL].get());
                currLevel->setData(currLevel->getData() + 1);
                BasicSingle32<uint32_t> *numSparks = dynamic_cast<BasicSingle32<uint32_t> *>(mGame->getGameState()[FR_GS_NUM_SPARKS].get());
                numSparks->setData(numSparks->getData() + 1);
                mState = STATE_FADEOUT;
            }
        }
    }
}

void MainScene::postTicks() {
    if (mUpdateView)
        updateViewMatrix();
}

void MainScene::render() {
    mPointLightPosArray[LOCAL_LIGHTSOURCE].set(mCurrPos, 1.0f);
    mPointLightPosArray[HELPER_SPARK].set(mParticles[HELPER_SPARK]->getPosition(), 1.0f);
    mPointLightPos.clear();
    for (auto &pointLightPos : mPointLightPosArray)
    {
        auto pointLightPosEye = mViewMatrix * pointLightPos;
        mPointLightPos.push_back(pointLightPosEye.x);
        mPointLightPos.push_back(pointLightPosEye.y);
        mPointLightPos.push_back(pointLightPosEye.z);
        mPointLightPos.push_back(pointLightPosEye.w);
    }

    (*mWallShaderProgram).useProgram();
    (*mWallShaderProgram).setUniforms(mViewMatrix, mITViewMatrix, mViewProjMatrix, mPointLightPos,
                                      mPointLightColor, 3,
                                      mGame->getTextureIds()[FR_IMAGE_ASSET_STONE1]);
    (*mDrawList).bindData(*mWallShaderProgram);
    (*mDrawList).draw();

    (*mParticleDrawList).updateBuffers();
    GraphicsIntf->SetBlend(BLEND_MODE_ONE_ONE);
    (*mParticleShaderProgram).useProgram();
    (*mParticleShaderProgram).setUniforms(mViewMatrix, mViewProjMatrix,
                                          mGame->getTextureIds()[FR_IMAGE_ASSET_CIRCLE1]);
    (*mParticleDrawList).bindData(*mParticleShaderProgram);
    (*mParticleDrawList).draw();
    GraphicsIntf->SetBlend(BLEND_MODE_NONE);

    (*mHUDBackDrawList).updateBuffers();
    if (!(*mHUDBackDrawList).isEmpty())
    {
        (*mHUDShaderProgram).useProgram();
        (*mHUDShaderProgram).setUniforms(mGame->getTextureIds()[FR_IMAGE_ASSET_HUD]);
        (*mHUDBackDrawList).bindData(*mHUDShaderProgram);
        (*mHUDBackDrawList).draw();
    }

    GraphicsIntf->SetBlend(BLEND_MODE_ONE_ONE);
    (*mHUDDrawList).updateBuffers();
    if (!(*mHUDDrawList).isEmpty())
    {
        (*mHUDShaderProgram).useProgram();
        (*mHUDShaderProgram).setUniforms(mGame->getTextureIds()[FR_IMAGE_ASSET_HUD]);
        (*mHUDDrawList).bindData(*mHUDShaderProgram);
        (*mHUDDrawList).draw();
    }
    GraphicsIntf->SetBlend(BLEND_MODE_NONE);

    (*mButtonDrawList).updateBuffers();
    if (!(*mButtonDrawList).isEmpty())
    {
        (*mHUDShaderProgram).useProgram();
        (*mHUDShaderProgram).setUniforms(mGame->getTextureIds()[FR_IMAGE_ASSET_BUTTONS]);
        (*mButtonDrawList).bindData(*mHUDShaderProgram);
        (*mButtonDrawList).draw();
    }
}

void MainScene::rotationInput(double azimuth, double pitch, double roll) {
    // Avoid gimbal lock.
    if (abs(pitch) >= 89.0f)
        return;

    if (!mRotationCalibrated)
    {
        mInitRotation[0] = azimuth;
        mInitRotation[1] = 10.0f; // Adjust for slight down-tilt typical of normal user posture.
        mInitRotation[2] = roll;
        mRotationCalibrated = true;
    }
    else
    {
        mCurrRotation[0] = azimuth - mInitRotation[0];
        mCurrRotation[1] = pitch - mInitRotation[1];
        mCurrRotation[2] = roll - mInitRotation[2];
        mUpdateView = true;
    }
}

void MainScene::initSounds() {
    mSoundMetadataVec = {
            {"AboveSea.raw",      SOUND_CLASS_BACKTRACK_LONG,  0, 1.0f},
            {"Amorphous.raw",     SOUND_CLASS_BACKTRACK_LONG,  0, 1.0f},
            {"Bardok.raw",        SOUND_CLASS_BACKTRACK_LONG,  0, 1.0f},
            {"Confusion.raw",     SOUND_CLASS_BACKTRACK_LONG,  0, 1.0f},
            {"Dimension.raw",     SOUND_CLASS_BACKTRACK_LONG,  0, 1.0f},
            {"DreamingAwake.raw", SOUND_CLASS_BACKTRACK_LONG,  0, 1.0f},
            {"Drums1.raw",        SOUND_CLASS_BACKTRACK_SHORT, 0, 1.0f},
            {"Drums2.raw",        SOUND_CLASS_BACKTRACK_SHORT, 0, 1.0f},
            {"Drums3.raw",        SOUND_CLASS_BACKTRACK_SHORT, 0, 1.0f},
            {"Bells5.raw",        SOUND_CLASS_NORMAL,          0, 1.0f},
    };
}

bool MainScene::handleSparkButton() {
    if (mGame->isPaused())
        return true;

    BasicSingle32<uint32_t> *numSparks = dynamic_cast<BasicSingle32<uint32_t> *>(mGame->getGameState()[FR_GS_NUM_SPARKS].get());
    uint32_t numSparksData = numSparks->getData();
    if (numSparksData)
    {
        --numSparksData;
        numSparks->setData(numSparksData);
        mParticles[HELPER_SPARK]->setPosition(mCurrPos);
        mParticles[HELPER_SPARK]->setIntensity(1.0f);
        mParticles[HELPER_SPARK]->setActive(true);

        mPointLightColor[0] = mBaselineColors[HELPER_SPARK].red;
        mPointLightColor[1] = mBaselineColors[HELPER_SPARK].green;
        mPointLightColor[2] = mBaselineColors[HELPER_SPARK].blue;

        mTextObjects[0]->updateText(std::to_string(numSparksData));
    }

    return true;
}

bool MainScene::handleCancelButton() {
    mCancelExit = true;
    return true;
}

bool MainScene::handleQuitButton() {
    mGame->triggerTerminate();
    return true;
}

void MainScene::initTextObjects() {
    Vector2 fontSize(0.06f, 0.075f);
    float currPosX = -0.9f;
    float currPosY = -0.875f;

    BasicSingle32<uint32_t> *numSparks = dynamic_cast<BasicSingle32<uint32_t> *>(mGame->getGameState()[FR_GS_NUM_SPARKS].get());
    std::string text = std::to_string(numSparks->getData());
    mTextObjects.push_back(
            std::move(
                    mGame->buildTextObject(text, 2, JUSTIFY_MODE_LEFT,
                                           Vector3(-0.89f, -0.915f, FRGC_HUD_LAYER_FRONT),
                                           fontSize, {0.25f, 0.25f, 1.0f, 1.0f}, true)));

    BasicSingle32<uint32_t> *currLevel = dynamic_cast<BasicSingle32<uint32_t> *>(mGame->getGameState()[FR_GS_CURR_LEVEL].get());
    text = "LEVEL " + std::to_string(currLevel->getData());
    LOGD("TEXT=%s", text.c_str());
    mTextObjects.push_back(
            std::move(
                    mGame->buildTextObject(text, 8, JUSTIFY_MODE_LEFT,
                                           Vector3(-0.285f, -0.1f, FRGC_HUD_LAYER_FRONT),
                                           Vector2(0.1f, 0.125f), mLevelColor,
                                           true)));

    /*
    mTextObjects.push_back(
            std::move(mGame->buildTextObject("WIDTH:", 6, JUSTIFY_MODE_LEFT,
                                             Vector2(currPosX, currPosY),
                                             fontSize, true)));
    currPosX += fontSize.x * 7.0f * 0.75f;

    mTextObjects.push_back(
            std::move(
                    mGame->buildTextObject("0", 2, JUSTIFY_MODE_RIGHT, Vector2(currPosX, currPosY),
                                           fontSize, true)));
    currPosX += fontSize.x * 3.0f * 0.75f;

    mTextObjects.push_back(
            std::move(mGame->buildTextObject("DEPTH:", 6, JUSTIFY_MODE_LEFT,
                                             Vector2(currPosX, currPosY),
                                             fontSize, true)));
    currPosX += fontSize.x * 7.0f * 0.75f;

    mTextObjects.push_back(
            std::move(
                    mGame->buildTextObject("0", 2, JUSTIFY_MODE_RIGHT, Vector2(currPosX, currPosY),
                                           fontSize, true)));
    currPosX += fontSize.x * 3.0f * 0.75f;

    mTextObjects.push_back(
            std::move(mGame->buildTextObject("FLOORS:", 7, JUSTIFY_MODE_LEFT,
                                             Vector2(currPosX, currPosY),
                                             fontSize, true)));
    currPosX += fontSize.x * 8.0f * 0.75f;

    mTextObjects.push_back(
            std::move(
                    mGame->buildTextObject("0", 2, JUSTIFY_MODE_RIGHT, Vector2(currPosX, currPosY),
                                           fontSize, true)));

    */
}

void MainScene::buildGameObjects() {
    MazeConfig mazeConfig;
    BasicSingle32<uint32_t> *currLevel = dynamic_cast<BasicSingle32<uint32_t> *>(mGame->getGameState()[FR_GS_CURR_LEVEL].get());
    if (currLevel->getData() == 0)
    {
        mazeConfig.width = static_cast<uint32_t>((rand() % 10) + 3);
        mazeConfig.depth = static_cast<uint32_t>((rand() % 10) + 3);
        mazeConfig.levels = static_cast<uint32_t>((rand() % 10) + 3);
    }
    else
    {
        mazeConfig.width = mazeConfig.depth = mazeConfig.levels = currLevel->getData() + 2;
    }

    /*
    mTextObjects[1]->updateText(std::to_string(mazeConfig.width));
    mTextObjects[3]->updateText(std::to_string(mazeConfig.depth));
    mTextObjects[5]->updateText(std::to_string(mazeConfig.levels));
    */

    mMaze.reset(new Maze());
    (*mMaze).buildMaze(mazeConfig);

    float frameWidth = 0.3f;
    float frameDepth = 0.3f;
    float roomSeparation = frameDepth * 2;
    Vector3 originPos(0.0f, 0.0f, 0.0f);

    mRoomOffset = mRoomSize;
    mRoomOffset.x += roomSeparation;
    mRoomOffset.y += roomSeparation;

    const matrix3d_u32 *mazeMatrix = (*mMaze).getMazeMatrix();
    for (uint32_t i = 0; i < mazeMatrix->getWidth(); ++i)
    {
        for (uint32_t j = 0; j < mazeMatrix->getDepth(); ++j)
        {
            for (uint32_t k = 0; k < mazeMatrix->getHeight(); ++k)
            {
                Vector3 roomPos = originPos;
                roomPos.x += mRoomOffset.x * i;
                roomPos.y += mRoomOffset.y * k;
                roomPos.z -= mRoomOffset.x * j;

                const uint32_t &room = mazeMatrix->at(i, j, k);
                buildRoom(roomPos, static_cast<uint8_t>(room), mRoomSize, frameWidth, frameDepth);
            }
        }
    }

    Vector3 particlePos = originPos + Vector3(2.5f, 2.0f, -2.5f);
    mParticles.push_back(std::make_shared<ParticleObject>(
            ParticleObject(particlePos, DGLColor({1.0f, 0.86f, 0.5f, 1.0f}), 128.0f, 1.0f,
                           Vector3(0.03f, 0.03f, 0.03f), 100.0f, 0.99f, false)));
    mPhysicsEngine->addPhysical(mParticles[HELPER_SPARK]);
    mParticles[HELPER_SPARK]->addForce(Vector3(0.0f, 0.0f, 0.0f));
    mParticles[HELPER_SPARK]->addForce(Vector3(0.0f, 0.0f, 0.0f));
    mParticles[HELPER_SPARK]->addForce(Vector3(0.0f, 0.0f, 0.0f));
    mParticles[HELPER_SPARK]->addForce(Vector3(0.0f, 0.0f, 0.0f));
    mParticles[HELPER_SPARK]->addForce(Vector3(0.0f, 0.0f, 0.0f));

    particlePos.x += (mRoomOffset.x * (mazeMatrix->getWidth() - 1));
    particlePos.y += (mRoomOffset.y * (mazeMatrix->getHeight() - 1));
    particlePos.z -= (mRoomOffset.x * (mazeMatrix->getDepth() - 1));
    mParticles.push_back(std::make_shared<ParticleObject>(
            ParticleObject(particlePos, DGLColor({0.5f, 0.86f, 1.0f, 1.0f}), 512.0f, 1.0f,
                           Vector3(0.01f, 0.01f, 0.01f), 100.0f, 0.999f, true)));
    mPhysicsEngine->addPhysical(mParticles[GOAL_SPARK]);

    mPointLightPosArray[HELPER_SPARK].set(mParticles[HELPER_SPARK]->getPosition(), 1.0f);
    mPointLightPosArray[GOAL_SPARK].set(mParticles[GOAL_SPARK]->getPosition(), 1.0f);

    mPointLightColor = {0.0f, 0.0f, 0.0f,
                        mBaselineColors[GOAL_SPARK].red, mBaselineColors[GOAL_SPARK].green,
                        mBaselineColors[GOAL_SPARK].blue,
                        mBaselineColors[LOCAL_LIGHTSOURCE].red,
                        mBaselineColors[LOCAL_LIGHTSOURCE].green,
                        mBaselineColors[LOCAL_LIGHTSOURCE].blue};

    mHUDObjects.push_back(
            std::make_shared<SpriteObject>(&mHUDFrameMapInfo,
                                           Vector3(-0.95f, -0.9f, FRGC_HUD_LAYER_MID),
                                           Vector2(0.05f, 0.05f), WHITE));

    mHUDBackObjects.push_back(
            std::make_shared<SpriteObject>(&mHUDFrameMapInfo,
                                           Vector3(-0.98f, -0.925f, FRGC_HUD_LAYER_REAR),
                                           Vector2(0.2f, 0.1f), WHITE,
                                           [&] { return handleSparkButton(); }));
    addTouchable(mHUDBackObjects[0]);
    mHUDBackObjects[0]->setFrame(1);

    mButtonObjects.push_back(
            std::make_shared<SpriteObject>(&mButtonsFrameMapInfo,
                                           Vector3(-0.3f, -0.05f, FRGC_HUD_LAYER_FRONT),
                                           Vector2(0.25f, 0.15f), WHITE,
                                           [&] { return handleCancelButton(); }));
    mButtonObjects[0]->setFrame(0);

    mButtonObjects.push_back(
            std::make_shared<SpriteObject>(&mButtonsFrameMapInfo,
                                           Vector3(0.05f, -0.05f, FRGC_HUD_LAYER_FRONT),
                                           Vector2(0.25f, 0.15f), WHITE,
                                           [&] { return handleQuitButton(); }));
    mButtonObjects[1]->setFrame(1);
}

void MainScene::destroyGameObjects() {
    mWalls.clear();
    mParticles.clear();
    mHUDObjects.clear();
    mHUDBackObjects.clear();
    mButtonObjects.clear();
    mForceDelta = Vector3();
}

void
MainScene::buildRoom(Vector3 pos, uint8_t walls, Vector2 size, float frameWidth, float frameDepth) {
    mWalls.push_back(std::make_shared<GameObject>(
            WallObject(Vector3(pos.x, pos.y, pos.z - size.x), WallObject::WALL_FRONT,
                       !(walls & WallObject::WALL_FRONT),
                       size, frameWidth, frameDepth)));
    mWalls.push_back(std::make_shared<GameObject>(
            WallObject(Vector3(pos.x + size.x, pos.y, pos.z - size.x), WallObject::WALL_RIGHT,
                       !(walls & WallObject::WALL_RIGHT),
                       size, frameWidth, frameDepth)));
    mWalls.push_back(std::make_shared<GameObject>(
            WallObject(Vector3(pos.x, pos.y + size.y, pos.z - size.x), WallObject::WALL_TOP,
                       !(walls & WallObject::WALL_TOP),
                       size, frameWidth, frameDepth)));

    if (walls & WallObject::WALL_BACK)
    {
        mWalls.push_back(std::make_shared<GameObject>(
                WallObject(Vector3(pos.x + size.x, pos.y, pos.z), WallObject::WALL_BACK, false,
                           size, frameWidth, frameDepth)));
    }

    if (walls & WallObject::WALL_LEFT)
    {
        mWalls.push_back(std::make_shared<GameObject>(
                WallObject(Vector3(pos.x, pos.y, pos.z), WallObject::WALL_LEFT, false,
                           size, frameWidth, frameDepth)));
    }

    if (walls & WallObject::WALL_BOTTOM)
    {
        mWalls.push_back(std::make_shared<GameObject>(
                WallObject(Vector3(pos.x, pos.y, pos.z), WallObject::WALL_BOTTOM, false,
                           size, frameWidth, frameDepth)));
    }
}

void MainScene::initGameState() {
    mState = STATE_FADEIN;

    mCurrPos.set(2.5f, 2.0f, -2.5f);
    mCurrAngle = 0.0f;
    mCurrDir.x = -sinf(mCurrAngle * DEG2RAD);
    mCurrDir.z = -cosf(mCurrAngle * DEG2RAD);
    mCurrFacing = Maze::MAZE_FRONT;
    mCurrRoom = Vertex3DEx();
    mActiveMotion = MOTION_EVENT_NONE;
    mMotionTarget = 0.0f;

    mRotationCalibrated = false;

    mPointLightPosArray[LOCAL_LIGHTSOURCE].set(mCurrPos, 1.0f);

    BasicSingle32<uint32_t> *numSparks = dynamic_cast<BasicSingle32<uint32_t> *>(mGame->getGameState()[FR_GS_NUM_SPARKS].get());
    mTextObjects[0]->updateText(std::to_string(numSparks->getData()));

    BasicSingle32<uint32_t> *currLevel = dynamic_cast<BasicSingle32<uint32_t> *>(mGame->getGameState()[FR_GS_CURR_LEVEL].get());
    mTextObjects[1]->updateText("LEVEL " + std::to_string(currLevel->getData()));

    int soundIndex = (rand() % mGame->getNumSounds(SOUND_CLASS_BACKTRACK_LONG)) +
                     mGame->getSoundClassOffset(SOUND_CLASS_BACKTRACK_LONG);
    mGame->scheduleSound(soundIndex, 100);

    mVolume = 0.0f;
    mGame->setSoundVolume(9, 0.0f);
    mGame->playSound(9, true);
}

void MainScene::buildShaders() {
    mWallShaderProgram.reset(new WallShaderProgram());
    mParticleShaderProgram.reset(new ParticleShaderProgram());
    mHUDShaderProgram.reset(new SpriteShaderProgram());
}

void MainScene::loadDrawList() {
    mDrawList.reset(new DrawList({ATTR_POSITION, ATTR_NORMAL, ATTR_TEXTUREUV}, false));
    for (auto &wall : mWalls)
        (*mDrawList).addGameObject(wall);
    (*mDrawList).buildBuffers();

    mParticleDrawList.reset(new ParticleDrawList({ATTR_POSITION, ATTR_COLOR, ATTR_POINTSIZE}));
    for (auto &particle : mParticles)
        (*mParticleDrawList).addGameObject(particle);
    (*mParticleDrawList).buildBuffers();

    mHUDDrawList.reset(new DrawList({ATTR_POSITION, ATTR_COLOR, ATTR_TEXTUREUV}, true));
    for (auto &hudObject : mHUDObjects)
        (*mHUDDrawList).addGameObject(hudObject);
    (*mHUDDrawList).buildBuffers();

    mHUDBackDrawList.reset(new DrawList({ATTR_POSITION, ATTR_COLOR, ATTR_TEXTUREUV}, true));
    for (auto &hudObject : mHUDBackObjects)
        (*mHUDBackDrawList).addGameObject(hudObject);
    (*mHUDBackDrawList).buildBuffers();

    mButtonDrawList.reset(new DrawList({ATTR_POSITION, ATTR_COLOR, ATTR_TEXTUREUV}, true));

    if (mGame->isPaused())
        for (auto &buttonObject : mButtonObjects)
            (*mButtonDrawList).addGameObject(buttonObject);
    //(*mButtonDrawList).buildBuffers();
}

void MainScene::updateViewMatrix() {
    if (mActiveMotion == MOTION_EVENT_NONE)
    {
        float angle = static_cast<float>(mCurrRotation[0]);
        while (angle < 0.0f)
            angle += 360.0f;

        if ((angle >= 45.0f) && (angle < 315.0f))
        {
            // Azimuth has turned 45 degrees left or right.
            if (angle < 180.0f)
            {
                mCurrAngle -= 90.0f;
                if (mCurrAngle < 0.0f)
                    mCurrAngle += 360.0f;

                mInitRotation[0] += 90.0;
                if (mInitRotation[0] >= 360.0f)
                    mInitRotation[0] -= 360.0f;

                mCurrRotation[0] -= 90.0;
                if (mCurrRotation[0] < 0.0f)
                    mCurrRotation[0] += 360.0f;

                mCurrFacing = Maze::getRightWall(mCurrFacing);
            }
            else
            {
                mCurrAngle += 90.0f;
                if (mCurrAngle >= 360.0f)
                    mCurrAngle -= 360.0f;

                mInitRotation[0] -= 90.0;
                if (mInitRotation[0] < 0.0f)
                    mInitRotation[0] += 360.0f;

                mCurrRotation[0] += 90.0;
                if (mCurrRotation[0] >= 360.0f)
                    mCurrRotation[0] -= 360.0f;

                mCurrFacing = Maze::getLeftWall(mCurrFacing);
            }

            mCurrDir.x = -sinf(mCurrAngle * DEG2RAD);
            mCurrDir.z = -cosf(mCurrAngle * DEG2RAD);
        }
    }

    mViewMatrix.identity();
    mViewMatrix.translate(-mCurrPos);
    mViewMatrix.rotate(-mCurrAngle + static_cast<float>(mCurrRotation[0]), 0.0f, 1.0f, 0.0f);
    mViewMatrix.rotate(static_cast<float>(mCurrRotation[1]), 1.0f, 0.0f, 0.0f);

    mITViewMatrix = mViewMatrix;
    mITViewMatrix.invert().transpose();

    mViewProjMatrix = mProjMatrix * mViewMatrix;

    mUpdateView = false;
}

void MainScene::updateForces() {
    Vertex3DEx roomCoord = getRoomFromPos(mParticles[HELPER_SPARK]->getPosition());
    uint32_t room = (*mMaze).getMazeMatrix()->at(roomCoord);

    Vector3 currRoomPos = Vector3(mRoomOffset.x * roomCoord.x, mRoomOffset.y * roomCoord.y,
                                  -mRoomOffset.x * roomCoord.z);
    Vector3 distanceFromWalls = mParticles[HELPER_SPARK]->getPosition() - currRoomPos;

    const float resistanceFactor = 0.25f;
    const float resistanceRange = 1.6f;
    float resistance[MAX_DIR] = {
            resistanceRange - (mRoomSize.x + distanceFromWalls.z), // FRONT
            resistanceRange + distanceFromWalls.z, //BACK
            resistanceRange - (mRoomSize.x - distanceFromWalls.x), // RIGHT
            resistanceRange - distanceFromWalls.x, // LEFT
            resistanceRange - (mRoomSize.y - distanceFromWalls.y), // TOP
            resistanceRange - distanceFromWalls.y // BOTTOM
    };
    bool hasResistance[MAX_DIR];
    for (int i = 0; i < MAX_DIR; ++i)
        hasResistance[i] = resistance[i] > 0.0f;

    bool forceUpdated = false;
    if (hasResistance[FRONT])
    {
        if ((room & Maze::MAZE_FRONT) || hasResistance[RIGHT] || hasResistance[LEFT] ||
            hasResistance[TOP] || hasResistance[BOTTOM])
        {
            mParticles[HELPER_SPARK]->updateForce(3, {0.0f, 0.0f,
                                                      resistance[FRONT] * resistanceFactor});
            forceUpdated = true;
        }
    }
    else if (hasResistance[BACK])
    {
        if ((room & Maze::MAZE_BACK) || hasResistance[RIGHT] || hasResistance[LEFT] ||
            hasResistance[TOP] || hasResistance[BOTTOM])
        {
            mParticles[HELPER_SPARK]->updateForce(3, {0.0f, 0.0f,
                                                      -resistance[BACK] * resistanceFactor});
            forceUpdated = true;
        }
    }
    if (!forceUpdated)
        mParticles[HELPER_SPARK]->updateForce(3, {0.0f, 0.0f, 0.0f});
    else
        forceUpdated = false;

    if (hasResistance[RIGHT])
    {
        if ((room & Maze::MAZE_RIGHT) || hasResistance[FRONT] || hasResistance[BACK] ||
            hasResistance[TOP] || hasResistance[BOTTOM])
        {
            mParticles[HELPER_SPARK]->updateForce(1, {-resistance[RIGHT] * resistanceFactor, 0.0f,
                                                      0.0f});
            forceUpdated = true;
        }
    }
    else if (hasResistance[LEFT])
    {
        if ((room & Maze::MAZE_LEFT) || hasResistance[FRONT] || hasResistance[BACK] ||
            hasResistance[TOP] || hasResistance[BOTTOM])
        {
            mParticles[HELPER_SPARK]->updateForce(1, {resistance[LEFT] * resistanceFactor, 0.0f,
                                                      0.0f});
            forceUpdated = true;
        }
    }
    if (!forceUpdated)
        mParticles[HELPER_SPARK]->updateForce(1, {0.0f, 0.0f, 0.0f});
    else
        forceUpdated = false;

    if (hasResistance[TOP])
    {
        if ((room & Maze::MAZE_TOP) || hasResistance[FRONT] || hasResistance[BACK] ||
            hasResistance[RIGHT] || hasResistance[LEFT])
        {
            mParticles[HELPER_SPARK]->updateForce(2, {0.0f, -resistance[TOP] * resistanceFactor,
                                                      0.0f});
            forceUpdated = true;
        }
    }
    else if (hasResistance[BOTTOM])
    {
        if ((room & Maze::MAZE_BOTTOM) || hasResistance[FRONT] || hasResistance[BACK] ||
            hasResistance[RIGHT] || hasResistance[LEFT])
        {
            mParticles[HELPER_SPARK]->updateForce(2, {0.0f, resistance[BOTTOM] * resistanceFactor,
                                                      0.0f});
            forceUpdated = true;
        }
    }
    if (!forceUpdated)
        mParticles[HELPER_SPARK]->updateForce(2, {0.0f, 0.0f, 0.0f});

    mForceDelta.x += randomFloat(-0.002f, 0.002f);
    mForceDelta.y += randomFloat(-0.002f, 0.002f);
    mForceDelta.z += randomFloat(-0.002f, 0.002f);
    mForceDelta.limit(Vector3(0.005f, 0.005f, 0.005f));
    Vector3 force = mParticles[HELPER_SPARK]->getForces()[0];
    force += mForceDelta;
    force.limit(Vector3(0.1f, 0.1f, 0.1f));
    mParticles[HELPER_SPARK]->updateForce(0, force);

    eMazeDirection goalDir = static_cast<eMazeDirection>((room & Maze::MAZE_DIR_MASK)
            >> Maze::MAZE_DIR_OFFSET);
    if (goalDir == MAZE_DIR_FRONT)
    {
        mParticles[HELPER_SPARK]->updateForce(4, {0.0f, 0.0f, -0.4f});
    }
    else if (goalDir == MAZE_DIR_BACK)
    {
        mParticles[HELPER_SPARK]->updateForce(4, {0.0f, 0.0f, 0.4f});
    }
    else if (goalDir == MAZE_DIR_RIGHT)
    {
        mParticles[HELPER_SPARK]->updateForce(4, {0.4f, 0.0f, 0.0f});
    }
    else if (goalDir == MAZE_DIR_LEFT)
    {
        mParticles[HELPER_SPARK]->updateForce(4, {-0.4f, 0.0f, 0.0f});
    }
    else if (goalDir == MAZE_DIR_TOP)
    {
        mParticles[HELPER_SPARK]->updateForce(4, {0.0f, 0.4f, 0.0f});
    }
    else if (goalDir == MAZE_DIR_BOTTOM)
    {
        mParticles[HELPER_SPARK]->updateForce(4, {0.0f, -0.4f, 0.0f});
    }
    else
    {
        mParticles[HELPER_SPARK]->updateForce(4, {0.0f, 0.0f, 0.0f});
    }
}

void MainScene::updatePlaylist() {
    int soundIndex;
    if (mGame->getNumSoundsPlaying(SOUND_CLASS_BACKTRACK_LONG) +
        mGame->getNumSoundsQueued(SOUND_CLASS_BACKTRACK_LONG) < MAX_BACKTRACK_LONG)
    {
        soundIndex = (rand() % mGame->getNumSounds(SOUND_CLASS_BACKTRACK_LONG)) +
                     mGame->getSoundClassOffset(SOUND_CLASS_BACKTRACK_LONG);
        mGame->scheduleSound(soundIndex, static_cast<uint32_t>(rand() % 2000));
    }

    if (mGame->getNumSoundsPlaying(SOUND_CLASS_BACKTRACK_SHORT) +
        mGame->getNumSoundsQueued(SOUND_CLASS_BACKTRACK_SHORT) < MAX_BACKTRACK_SHORT)
    {
        soundIndex = (rand() % mGame->getNumSounds(SOUND_CLASS_BACKTRACK_SHORT)) +
                     mGame->getSoundClassOffset(SOUND_CLASS_BACKTRACK_SHORT);
        mGame->scheduleSound(soundIndex, static_cast<uint32_t>(rand() % 4000));
    }
}

Vertex3DEx MainScene::getRoomFromPos(Vector3 pos) {
    return Vertex3DEx(static_cast<uint32_t>(pos.x / mRoomOffset.x),
                      static_cast<uint32_t>(pos.y / mRoomOffset.y),
                      static_cast<uint32_t>(-pos.z / mRoomOffset.x));
}
