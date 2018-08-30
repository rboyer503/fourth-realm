/*
 * Copyright 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
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

#include "Game.h"
#include "IntroScene.h"
#include "MainScene.h"
#include "GameConstants.h"


Game::Game(AAssetManager *assetManager)
        : DGLGame(assetManager, FR_IMAGE_ASSET_FONT,
                  SpriteFrameMapInfo({Vector2(0.125f, 0.125f), 8, 8})) {
    (*mGameState).addGameData(
            std::make_shared<BasicSingle32<uint32_t> >(BasicSingle32<uint32_t>(0)));
    (*mGameState).addGameData(
            std::make_shared<BasicSingle32<uint32_t> >(BasicSingle32<uint32_t>(FRGC_INIT_SPARKS)));

    (*mSceneManager).addScene(std::make_shared<IntroScene>(this));
    (*mSceneManager).addScene(std::make_shared<MainScene>(this));
    triggerSceneSwitch(0);
}

Game::~Game() {
}
