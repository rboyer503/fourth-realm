//
// Created by Rob on 7/11/2018.
//

#include "Wall.h"

#include <numeric>


WallObject::WallObject(Vector3 pos, eOrientation orientation, bool isOpen, Vector2 wallSize,
                       float frameWidth, float frameDepth) {
    if (orientation == WALL_TOP || orientation == WALL_BOTTOM)
        wallSize.y = wallSize.x;

    float innerTop = wallSize.y - frameWidth;
    float innerRight = wallSize.x - frameWidth;
    float deepZ = -(isOpen ? (frameDepth * 2) : (frameDepth * 0.95f));

    // Build vector of vertex positions.
    std::vector<Vector3> vertPosVec;
    vertPosVec = {{0.0f,       0.0f,       0.0f},
                  {0.0f,       wallSize.y, 0.0f},
                  {wallSize.x, wallSize.y, 0.0f},
                  {wallSize.x, 0.0f,       0.0f},
                  {frameWidth, frameWidth, 0.0f},
                  {frameWidth, innerTop,   0.0f},
                  {innerRight, innerTop,   0.0f},
                  {innerRight, frameWidth, 0.0f},
                  {frameWidth, frameWidth, deepZ},
                  {frameWidth, innerTop,   deepZ},
                  {innerRight, innerTop,   deepZ},
                  {innerRight, frameWidth, deepZ}};

    // Add context-dependent vertex positions.
    std::vector<Vector3> tempVertPosVec;
    if (isOpen)
    {
        tempVertPosVec = {{0.0f,       0.0f,       deepZ},
                          {0.0f,       wallSize.y, deepZ},
                          {wallSize.x, wallSize.y, deepZ},
                          {wallSize.x, 0.0f,       deepZ}};
    }
    else
    {
        tempVertPosVec = {{frameWidth, frameWidth, deepZ},
                          {frameWidth, innerTop,   deepZ},
                          {innerRight, innerTop,   deepZ},
                          {innerRight, frameWidth, deepZ}};
    }
    vertPosVec.insert(vertPosVec.end(), tempVertPosVec.begin(), tempVertPosVec.end());

    // Perform rotation/translation.
    float angle = 0.0f;
    Vector3 axis(0.0f, 1.0f, 0.0f);
    std::vector<Vector3> normals;
    switch (orientation)
    {
        case WALL_FRONT:
            normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
            normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
            normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
            normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
            normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
            normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
            break;

        case WALL_BACK:
            angle = 180.0f;
            normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
            normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
            normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
            normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
            normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
            normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
            break;

        case WALL_RIGHT:
            angle = 270.0f;
            normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
            normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
            normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
            normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
            normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
            normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
            break;

        case WALL_LEFT:
            angle = 90.0f;
            normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
            normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
            normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
            normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
            normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
            normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
            break;

        case WALL_TOP:
            angle = 90.0f;
            axis.set(1.0f, 0.0f, 0.0f);
            normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
            normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
            normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
            normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
            normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
            normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
            break;

        case WALL_BOTTOM:
            angle = 270.0f;
            axis.set(1.0f, 0.0f, 0.0f);
            normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
            normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
            normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
            normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
            normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
            normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
            break;

        default:
            break;
    }
    Matrix3 rotMatrix(angle, axis);
    for (auto &vertPos : vertPosVec)
        vertPos = (rotMatrix * vertPos) + pos;

    // Build vector of indices for each quad.
    std::vector<int> indices;
    indices = {0, 1, 4, 4, 1, 5,
               1, 2, 5, 5, 2, 6,
               2, 3, 6, 6, 3, 7,
               3, 0, 7, 7, 0, 4,
               4, 5, 8, 8, 5, 9,
               5, 6, 9, 9, 6, 10,
               6, 7, 10, 10, 7, 11,
               7, 4, 11, 11, 4, 8};

    // Add context-dependent indices.
    std::vector<int> tempIndices;
    if (isOpen)
    {
        tempIndices = {8, 9, 12, 12, 9, 13,
                       9, 10, 13, 13, 10, 14,
                       10, 11, 14, 14, 11, 15,
                       11, 8, 15, 15, 8, 12};
    }
    else
    {
        tempIndices = {13, 12, 14, 14, 12, 15};
    }
    indices.insert(indices.end(), tempIndices.begin(), tempIndices.end());

    // Build vector of UV positions.
    std::vector<Vector2> vertUVVec;
    float leftDiv = 0.125f;
    float rightDiv = 0.875f;
    vertUVVec = {{leftDiv,  1.0f},
                 {leftDiv,  0.0f},
                 {rightDiv, 1.0f},
                 {rightDiv, 1.0f},
                 {leftDiv,  0.0f},
                 {rightDiv, 0.0f}};

    std::vector<Vector2> vertUVVecBeamL;
    vertUVVecBeamL = {{0.0f,    1.0f},
                      {0.0f,    0.0f},
                      {leftDiv, 1.0f},
                      {leftDiv, 1.0f},
                      {0.0f,    0.0f},
                      {leftDiv, 0.0f}};

    std::vector<Vector2> vertUVVecBeamR;
    vertUVVecBeamR = {{rightDiv, 1.0f},
                      {rightDiv, 0.0f},
                      {1.0f,     1.0f},
                      {1.0f,     1.0f},
                      {rightDiv, 0.0f},
                      {1.0f,     0.0f}};

    std::vector<Vector2> vertUVVecBeamAngle;
    vertUVVecBeamAngle = {{0.0f,     1.0f},
                          {0.0f,     0.0f},
                          {leftDiv,  rightDiv},
                          {leftDiv,  rightDiv},
                          {0.0f,     0.0f},
                          {leftDiv,  leftDiv},
                          {rightDiv, rightDiv},
                          {rightDiv, leftDiv},
                          {1.0f,     1.0f},
                          {1.0f,     1.0f},
                          {rightDiv, leftDiv},
                          {1.0f,     0.0f}};


    // Calculate number of triangles/vertex attributes.
    unsigned long numVertAttr = indices.size() * 8;
    mVertAttrs.resize(numVertAttr);

    // Build front face (first 8 triangles).
    int i = 0;
    int offset = 0;
    for (; i < 24; ++i)
    {
        mVertAttrs[offset++] = vertPosVec[indices[i]].x;
        mVertAttrs[offset++] = vertPosVec[indices[i]].y;
        mVertAttrs[offset++] = vertPosVec[indices[i]].z;
        mVertAttrs[offset++] = normals[0].x;
        mVertAttrs[offset++] = normals[0].y;
        mVertAttrs[offset++] = normals[0].z;
        mVertAttrs[offset++] = vertUVVecBeamAngle[i % 12].x;
        mVertAttrs[offset++] = vertUVVecBeamAngle[i % 12].y;
    }

    // Build inner faces (next 8 triangles).
    for (; i < 30; ++i)
    {
        mVertAttrs[offset++] = vertPosVec[indices[i]].x;
        mVertAttrs[offset++] = vertPosVec[indices[i]].y;
        mVertAttrs[offset++] = vertPosVec[indices[i]].z;
        mVertAttrs[offset++] = normals[1].x;
        mVertAttrs[offset++] = normals[1].y;
        mVertAttrs[offset++] = normals[1].z;
        mVertAttrs[offset++] = vertUVVecBeamL[i % 6].x;
        mVertAttrs[offset++] = vertUVVecBeamL[i % 6].y;
    }

    for (; i < 36; ++i)
    {
        mVertAttrs[offset++] = vertPosVec[indices[i]].x;
        mVertAttrs[offset++] = vertPosVec[indices[i]].y;
        mVertAttrs[offset++] = vertPosVec[indices[i]].z;
        mVertAttrs[offset++] = normals[2].x;
        mVertAttrs[offset++] = normals[2].y;
        mVertAttrs[offset++] = normals[2].z;
        mVertAttrs[offset++] = vertUVVecBeamL[i % 6].x;
        mVertAttrs[offset++] = vertUVVecBeamL[i % 6].y;
    }

    for (; i < 42; ++i)
    {
        mVertAttrs[offset++] = vertPosVec[indices[i]].x;
        mVertAttrs[offset++] = vertPosVec[indices[i]].y;
        mVertAttrs[offset++] = vertPosVec[indices[i]].z;
        mVertAttrs[offset++] = normals[3].x;
        mVertAttrs[offset++] = normals[3].y;
        mVertAttrs[offset++] = normals[3].z;
        mVertAttrs[offset++] = vertUVVecBeamR[i % 6].x;
        mVertAttrs[offset++] = vertUVVecBeamR[i % 6].y;
    }

    for (; i < 48; ++i)
    {
        mVertAttrs[offset++] = vertPosVec[indices[i]].x;
        mVertAttrs[offset++] = vertPosVec[indices[i]].y;
        mVertAttrs[offset++] = vertPosVec[indices[i]].z;
        mVertAttrs[offset++] = normals[4].x;
        mVertAttrs[offset++] = normals[4].y;
        mVertAttrs[offset++] = normals[4].z;
        mVertAttrs[offset++] = vertUVVecBeamR[i % 6].x;
        mVertAttrs[offset++] = vertUVVecBeamR[i % 6].y;
    }

    if (isOpen)
    {
        // Build back face (next 8 triangles).
        for (; i < 72; ++i)
        {
            mVertAttrs[offset++] = vertPosVec[indices[i]].x;
            mVertAttrs[offset++] = vertPosVec[indices[i]].y;
            mVertAttrs[offset++] = vertPosVec[indices[i]].z;
            mVertAttrs[offset++] = normals[5].x;
            mVertAttrs[offset++] = normals[5].y;
            mVertAttrs[offset++] = normals[5].z;
            mVertAttrs[offset++] = vertUVVecBeamAngle[i % 12].x;
            mVertAttrs[offset++] = vertUVVecBeamAngle[i % 12].y;
        }
    }
    else
    {
        // Build center wall.
        for (; i < 54; ++i)
        {
            mVertAttrs[offset++] = vertPosVec[indices[i]].x;
            mVertAttrs[offset++] = vertPosVec[indices[i]].y;
            mVertAttrs[offset++] = vertPosVec[indices[i]].z;
            mVertAttrs[offset++] = normals[0].x;
            mVertAttrs[offset++] = normals[0].y;
            mVertAttrs[offset++] = normals[0].z;
            mVertAttrs[offset++] = vertUVVec[i % 6].x;
            mVertAttrs[offset++] = vertUVVec[i % 6].y;
        }
    }
}
