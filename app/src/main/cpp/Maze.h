//
// Created by Rob on 7/16/2018.
//

#ifndef FOURTHREALM_MAZE_H
#define FOURTHREALM_MAZE_H

#include <cstdint>
#include <iostream>

#include "dgl.h"


enum eMazeDirection
{
    MAZE_DIR_NONE,
    MAZE_DIR_FRONT,
    MAZE_DIR_RIGHT,
    MAZE_DIR_TOP,
    MAZE_DIR_BACK,
    MAZE_DIR_LEFT,
    MAZE_DIR_BOTTOM
};

struct MazeConfig
{
    static constexpr uint32_t MIN_WIDTH = 3;
    static constexpr uint32_t MIN_DEPTH = 3;
    static constexpr uint32_t MIN_LEVELS = 1;
    static constexpr uint32_t MAX_WIDTH = 100;
    static constexpr uint32_t MAX_DEPTH = 100;
    static constexpr uint32_t MAX_LEVELS = 10;

    uint32_t width;
    uint32_t depth;
    uint32_t levels;

    MazeConfig() :
            width(0), depth(0), levels(0) {}

    MazeConfig(uint32_t width_, uint32_t depth_, uint32_t levels_) :
            width(width_), depth(depth_), levels(levels_) {}

    void print() const {
        std::cout << "Maze configuration: [" << "Width=" << width << ", Depth=" << depth
                  << ", Levels=" << levels << "]" << std::endl;
    }

    uint32_t getTotalRooms() const { return width * depth * levels; }
};


class Maze
{
    MazeConfig config_;
    matrix3d_u32 *maze_matrix_;
    std::vector<Vertex3DEx> rooms_;
    Vertex3DEx goal_;

public:
    static constexpr uint32_t MAZE_NONE = 0;
    static constexpr uint32_t MAZE_FRONT = 1 << 0;
    static constexpr uint32_t MAZE_RIGHT = 1 << 1;
    static constexpr uint32_t MAZE_TOP = 1 << 2;
    static constexpr uint32_t MAZE_BACK = 1 << 3;
    static constexpr uint32_t MAZE_LEFT = 1 << 4;
    static constexpr uint32_t MAZE_BOTTOM = 1 << 5;
    static constexpr uint32_t MAZE_DIMFRONT = 1 << 6;
    static constexpr uint32_t MAZE_DIMBACK = 1 << 7;
    static constexpr uint32_t MAZE_WALL_MASK = 0xFF;
    static constexpr uint32_t MAZE_LOADED = 1 << 8;
    static constexpr uint32_t MAZE_EXPLORED = 1 << 9;
    static constexpr uint32_t MAZE_DEADEND = 1 << 10;
    static constexpr uint32_t MAZE_DIR_OFFSET = 11;
    static constexpr uint32_t MAZE_DIR_MASK = 0x7 << MAZE_DIR_OFFSET;
    static constexpr uint32_t MAZE_GOAL = 1 << 14;
    static constexpr uint32_t MAZE_SOLVED_OFFSET = MAZE_DIR_OFFSET;
    static constexpr uint32_t MAZE_SOLVED_MASK = 0xF << MAZE_DIR_OFFSET;

    Maze();
    ~Maze();

    static size_t countBranches(uint32_t room) {
        size_t count = 0;
        for (size_t i = 1; i <= (1 << 5); i <<= 1)
            if (!(room & i))
                ++count;
        return count;
    }

    static bool hasStairwell(uint32_t room) {
        return ((room & (Maze::MAZE_BOTTOM | Maze::MAZE_TOP)) !=
                (Maze::MAZE_BOTTOM | Maze::MAZE_TOP));
    }

    MazeConfig &getMazeConfig() { return config_; }
    const MazeConfig &getMazeConfig() const { return config_; }
    const matrix3d_u32 *getMazeMatrix() const { return maze_matrix_; }
    const Vertex3DEx &getGoal() const { return goal_; }

    void buildMaze(const MazeConfig &config);

    static uint32_t getOppositeWall(const uint32_t dir);
    static uint32_t getRightWall(const uint32_t dir);
    static uint32_t getLeftWall(const uint32_t dir);

private:
    void processProspectiveRoom(const Vertex3DEx &prospect_vert, vertex3d_vec &explored_verts);
    void loadGoalDirection();

    // Non-copyable.
    Maze(const Maze &);
    void operator=(const Maze &);
};


#endif //FOURTHREALM_MAZE_H
