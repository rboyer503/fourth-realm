//
// Created by Rob on 7/11/2018.
//

#ifndef FOURTHREALM_WALL_H
#define FOURTHREALM_WALL_H

#include "dgl.h"


class WallObject : public GameObject
{
public:
    enum eOrientation
    {
        WALL_FRONT = 0x1,
        WALL_RIGHT = 0x2,
        WALL_TOP = 0x4,
        WALL_BACK = 0x8,
        WALL_LEFT = 0x10,
        WALL_BOTTOM = 0x20
    };

    WallObject(Vector3 pos, eOrientation orientation, bool isOpen, Vector2 wallSize,
               float frameWidth, float frameDepth);

};


#endif //FOURTHREALM_WALL_H
