//
// Created by Rob on 7/16/2018.
//

#include "Maze.h"

#include <deque>
#include <random>


Maze::Maze()
        : maze_matrix_(nullptr) {
}

Maze::~Maze() {
    delete maze_matrix_;
}

void Maze::buildMaze(const MazeConfig &config) {
    config_ = config;

    // Build 3D matrix with all "rooms" having all 6 walls and flagged as not explored.
    uint32_t init_value = MAZE_LEFT | MAZE_RIGHT | MAZE_FRONT | MAZE_BACK | MAZE_BOTTOM | MAZE_TOP;
    maze_matrix_ = new matrix3d_u32(config_.width, config_.depth, config_.levels, init_value);

    // Load initial room and initiate modified Prim's algorithm.
    rooms_.push_back(Vertex3DEx(config_.width / 2, config_.levels / 2, config_.depth / 2));

    while (rooms_.size() > 0)
    {
        // Randomly select next room and mark it explored.
        size_t index = (rand() % rooms_.size());
        Vertex3DEx curr_room = rooms_[index];
        maze_matrix_->at(curr_room) |= MAZE_EXPLORED;

        // Load adjacent, unexplored rooms for future processing.
        // Also locally track adjacent, explored rooms.
        vertex3d_vec explored_verts;
        if (curr_room.x > 0)
        {
            processProspectiveRoom(Vertex3DEx(curr_room.x - 1, curr_room.y, curr_room.z, MAZE_LEFT),
                                   explored_verts);
        }
        if (curr_room.x < (config_.width - 1))
        {
            processProspectiveRoom(
                    Vertex3DEx(curr_room.x + 1, curr_room.y, curr_room.z, MAZE_RIGHT),
                    explored_verts);
        }
        if (curr_room.y > 0)
        {
            processProspectiveRoom(
                    Vertex3DEx(curr_room.x, curr_room.y - 1, curr_room.z, MAZE_BOTTOM),
                    explored_verts);
        }
        if (curr_room.y < (config_.levels - 1))
        {
            processProspectiveRoom(Vertex3DEx(curr_room.x, curr_room.y + 1, curr_room.z, MAZE_TOP),
                                   explored_verts);
        }
        if (curr_room.z > 0)
        {
            processProspectiveRoom(Vertex3DEx(curr_room.x, curr_room.y, curr_room.z - 1, MAZE_BACK),
                                   explored_verts);
        }
        if (curr_room.z < (config_.depth - 1))
        {
            processProspectiveRoom(
                    Vertex3DEx(curr_room.x, curr_room.y, curr_room.z + 1, MAZE_FRONT),
                    explored_verts);
        }

        if (explored_verts.size() > 0)
        {
            // Randomly select adjacent, explored room to connect.
            size_t adj_index = (rand() % explored_verts.size());
            Vertex3DEx &curr_adj = explored_verts[adj_index];

            // Extract direction from extra field and remove the appropriate walls from both rooms.
            uint32_t extra = static_cast<uint32_t>(curr_adj.extra);
            uint32_t opposite_wall = getOppositeWall(extra);
            maze_matrix_->at(curr_room) &= ~extra;
            maze_matrix_->at(curr_adj) &= ~opposite_wall;
        }

        // Remove the room we just processed.
        rooms_.erase(rooms_.begin() + index);
    }

    loadGoalDirection();
}

uint32_t Maze::getOppositeWall(const uint32_t dir) {
    switch (dir & MAZE_WALL_MASK)
    {
        case MAZE_NONE:
            return MAZE_NONE;
        case MAZE_LEFT:
            return MAZE_RIGHT;
        case MAZE_RIGHT:
            return MAZE_LEFT;
        case MAZE_FRONT:
            return MAZE_BACK;
        case MAZE_BACK:
            return MAZE_FRONT;
        case MAZE_BOTTOM:
            return MAZE_TOP;
        case MAZE_TOP:
            return MAZE_BOTTOM;
        default:
            throw std::runtime_error("Maze::getOppositeWall: [Unexpected input]");
    }
}

uint32_t Maze::getRightWall(const uint32_t dir) {
    switch (dir & MAZE_WALL_MASK)
    {
        case MAZE_FRONT:
            return MAZE_RIGHT;
        case MAZE_RIGHT:
            return MAZE_BACK;
        case MAZE_BACK:
            return MAZE_LEFT;
        case MAZE_LEFT:
            return MAZE_FRONT;
        default:
            throw std::runtime_error("Maze::getRightWall: [Unexpected input]");
    }
}

uint32_t Maze::getLeftWall(const uint32_t dir) {
    switch (dir & MAZE_WALL_MASK)
    {
        case MAZE_FRONT:
            return MAZE_LEFT;
        case MAZE_RIGHT:
            return MAZE_FRONT;
        case MAZE_BACK:
            return MAZE_RIGHT;
        case MAZE_LEFT:
            return MAZE_BACK;
        default:
            throw std::runtime_error("Maze::getLeftWall: [Unexpected input]");
    }
}

void Maze::processProspectiveRoom(const Vertex3DEx &prospect_vert, vertex3d_vec &explored_verts) {
    uint32_t &room = maze_matrix_->at(prospect_vert);
    if (room & MAZE_EXPLORED)
    {
        explored_verts.push_back(prospect_vert);
    }
    else if ((room & MAZE_LOADED) == 0)
    {
        room |= MAZE_LOADED;
        rooms_.push_back(prospect_vert);
    }
}

void Maze::loadGoalDirection() {
    std::deque<Vertex3DEx> rooms;

    Vertex3DEx currCoord(config_.width - 1, config_.levels - 1, config_.depth - 1);
    rooms.push_back(currCoord);
    uint32_t *currRoom = maze_matrix_->ptr(currCoord);
    *currRoom |= MAZE_GOAL;
    goal_ = currCoord;

    Vertex3DEx nextCoord;
    uint32_t *nextRoom;

    while (!rooms.empty())
    {
        currCoord = rooms.front();
        rooms.pop_front();

        currRoom = maze_matrix_->ptr(currCoord);
        nextCoord = currCoord;

        if (!(*currRoom & MAZE_FRONT))
        {
            nextCoord.z++;
            nextRoom = maze_matrix_->ptr(nextCoord);
            if (!(*nextRoom & MAZE_SOLVED_MASK))
            {
                *nextRoom |= MAZE_DIR_BACK << MAZE_DIR_OFFSET;
                rooms.push_back(nextCoord);
            }
            nextCoord.z--;
        }

        if (!(*currRoom & MAZE_BACK))
        {
            nextCoord.z--;
            nextRoom = maze_matrix_->ptr(nextCoord);
            if (!(*nextRoom & MAZE_SOLVED_MASK))
            {
                *nextRoom |= MAZE_DIR_FRONT << MAZE_DIR_OFFSET;
                rooms.push_back(nextCoord);
            }
            nextCoord.z++;
        }

        if (!(*currRoom & MAZE_RIGHT))
        {
            nextCoord.x++;
            nextRoom = maze_matrix_->ptr(nextCoord);
            if (!(*nextRoom & MAZE_SOLVED_MASK))
            {
                *nextRoom |= MAZE_DIR_LEFT << MAZE_DIR_OFFSET;
                rooms.push_back(nextCoord);
            }
            nextCoord.x--;
        }

        if (!(*currRoom & MAZE_LEFT))
        {
            nextCoord.x--;
            nextRoom = maze_matrix_->ptr(nextCoord);
            if (!(*nextRoom & MAZE_SOLVED_MASK))
            {
                *nextRoom |= MAZE_DIR_RIGHT << MAZE_DIR_OFFSET;
                rooms.push_back(nextCoord);
            }
            nextCoord.x++;
        }

        if (!(*currRoom & MAZE_TOP))
        {
            nextCoord.y++;
            nextRoom = maze_matrix_->ptr(nextCoord);
            if (!(*nextRoom & MAZE_SOLVED_MASK))
            {
                *nextRoom |= MAZE_DIR_BOTTOM << MAZE_DIR_OFFSET;
                rooms.push_back(nextCoord);
            }
            nextCoord.y--;
        }

        if (!(*currRoom & MAZE_BOTTOM))
        {
            nextCoord.y--;
            nextRoom = maze_matrix_->ptr(nextCoord);
            if (!(*nextRoom & MAZE_SOLVED_MASK))
            {
                *nextRoom |= MAZE_DIR_TOP << MAZE_DIR_OFFSET;
                rooms.push_back(nextCoord);
            }
            nextCoord.y++;
        }
    }
}
