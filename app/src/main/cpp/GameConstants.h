//
// Created by Rob on 8/8/2018.
//

#ifndef FOURTHREALM_GAMECONSTANTS_H
#define FOURTHREALM_GAMECONSTANTS_H

#define FRGC_INIT_SPARKS 3
#define FRGC_SWIPE_THRES 0.05f

#define FRGC_HUD_LAYER_REAR 0.02f
#define FRGC_HUD_LAYER_MID 0.01f
#define FRGC_HUD_LAYER_FRONT 0.0f

enum eImageAsset
{
    FR_IMAGE_ASSET_FONT,
    FR_IMAGE_ASSET_HUD,
    FR_IMAGE_ASSET_INTRO,
    FR_IMAGE_ASSET_STONE1,
    FR_IMAGE_ASSET_CIRCLE1,
    FR_IMAGE_ASSET_MAX
};

enum eGameStateVars
{
    FR_GS_CURR_LEVEL,
    FR_GS_NUM_SPARKS,
    FR_GS_MAX
};

#endif //FOURTHREALM_GAMECONSTANTS_H