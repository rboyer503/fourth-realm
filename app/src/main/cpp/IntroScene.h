//
// Created by Rob on 8/1/2018.
//

#ifndef FOURTHREALM_INTROSCENE_H
#define FOURTHREALM_INTROSCENE_H

#include "dgl.h"


class IntroScene : public Scene
{
    enum eState
    {
        STATE_PRIME,
        STATE_FADEIN,
        STATE_WAIT,
        STATE_FADEOUT,
        STATE_MAX
    };

    DGLColor mBackgroundColor = WHITE;
    uint32_t mTickCount = 100;

    game_object_ptr mBackground;
    std::unique_ptr<DrawList> mDrawList;
    std::unique_ptr<BasicShaderProgram> mShaderProgram;
    bool mUpdateView = true;

    Vector3 mCurrPos;
    float mCurrAngle;

    // Input state.
    Vector2 mTouchStartPos;
    Vector2 mTouchCurrPos;
    bool mTouchActive = false;
    bool mTouchEvent = false;
    bool mGestureEvent = false;

public:
    IntroScene(DGLGame *game);
    ~IntroScene();

    // Inherited from Scene
    void loadScene() override;
    void pause() override;
    void resume() override;
    void surfaceCreated() override;
    void surfaceDestroyed() override;
    void preTicks() override;
    void tick() override;
    void postTicks() override;
    void render() override;
    void touchInput(eInputEvent event, Vector2 &pos) override;
    void rotationInput(double azimuth, double pitch, double roll) override;

private:
    void buildGameObjects();
    void initGameState();
    void updateViewMatrix();
    void loadDrawList();
    void buildShaders();

};


#endif //FOURTHREALM_INTROSCENE_H
