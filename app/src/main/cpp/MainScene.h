//
// Created by Rob on 7/30/2018.
//

#ifndef FOURTHREALM_MAINSCENE_H
#define FOURTHREALM_MAINSCENE_H

#include "dgl.h"
#include "Maze.h"


enum eMotionEvent
{
    MOTION_EVENT_NONE = 0,
    MOTION_EVENT_FORWARD = 1,
    MOTION_EVENT_BACKWARD = 2,
    MOTION_EVENT_RIGHT = 3,
    MOTION_EVENT_LEFT = 4,
    MOTION_EVENT_UP = 5,
    MOTION_EVENT_DOWN = 6
};


class MainScene : public Scene
{
    static constexpr int MAX_BACKTRACK_LONG = 2;
    static constexpr int MAX_BACKTRACK_SHORT = 1;
    static constexpr int HELPER_SPARK = 0;
    static constexpr int GOAL_SPARK = 1;
    static constexpr int LOCAL_LIGHTSOURCE = 2;

    enum eState
    {
        STATE_FADEIN,
        STATE_SHOWLEVEL,
        STATE_PLAY,
        STATE_FADEOUT,
        STATE_MAX
    };

    // Game objects/graphics.
    std::unique_ptr<Maze> mMaze;
    game_object_list mWalls;
    particle_object_vec mParticles;
    sprite_vec mHUDObjects;
    sprite_vec mHUDBackObjects;
    sprite_vec mButtonObjects;
    Vector3 mForceDelta;
    std::unique_ptr<DrawList> mDrawList;
    std::unique_ptr<ParticleDrawList> mParticleDrawList;
    std::unique_ptr<DrawList> mHUDDrawList;
    std::unique_ptr<DrawList> mHUDBackDrawList;
    std::unique_ptr<DrawList> mButtonDrawList;
    std::unique_ptr<WallShaderProgram> mWallShaderProgram;
    std::unique_ptr<ParticleShaderProgram> mParticleShaderProgram;
    std::unique_ptr<SpriteShaderProgram> mHUDShaderProgram;
    std::vector<float> mPointLightPos;
    std::vector<float> mPointLightColor;
    Vector4 mPointLightPosArray[3];
    bool mUpdateView = true;
    text_object_vec mTextObjects;
    const SpriteFrameMapInfo mHUDFrameMapInfo;
    const SpriteFrameMapInfo mButtonsFrameMapInfo;
    DGLColor mGlobalTone = {1.0f, 1.0f, 1.0f, 0.0f};
    DGLColor mLevelColor = {0.25f, 0.25f, 1.0f, 1.0f};
    const DGLColor mBaselineColors[3] = {
            {0.5f, 0.43f, 0.25f, 1.0f},
            {0.5f, 0.86f, 1.0f,  1.0f},
            {1.0f, 0.75f, 0.75f, 1.0f}
    };

    // Game state.
    Vector3 mCurrPos;
    Vector3 mCurrDir;
    uint32_t mCurrFacing = Maze::MAZE_FRONT;
    Vertex3DEx mCurrRoom{0, 0, 0};
    float mCurrAngle;
    eMotionEvent mActiveMotion = MOTION_EVENT_NONE;
    float mMotionTarget = 0.0f;
    Vector2 mRoomOffset;
    Vector2 mRoomSize{5.0f, 4.0f};
    float mVolume = 1.0f;
    bool mCancelExit = false;

    // Input state.
    bool mRotationCalibrated = false;
    double mInitRotation[3];
    double mCurrRotation[3];

public:
    MainScene(DGLGame *game);

    // Inherited from Scene
    void loadScene() override;
    void unloadScene() override;
    void pause() override;
    void resume() override;
    void backPressed() override;
    void surfaceCreated() override;
    void preTicks() override;
    void tick() override;
    void postTicks() override;
    void render() override;
    void rotationInput(double azimuth, double pitch, double roll) override;

    void initSounds() override;

    bool handleSparkButton();
    bool handleCancelButton();
    bool handleQuitButton();

private:
    void initTextObjects();
    void buildGameObjects();
    void destroyGameObjects();
    void buildRoom(Vector3 pos, uint8_t walls, Vector2 size, float frameWidth, float frameDepth);
    void initGameState();
    void buildShaders();
    void loadDrawList();
    void updateViewMatrix();
    void updateForces();
    void updatePlaylist();
    Vertex3DEx getRoomFromPos(Vector3 pos);
};


#endif //FOURTHREALM_MAINSCENE_H
