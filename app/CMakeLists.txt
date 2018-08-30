cmake_minimum_required(VERSION 3.4.1)

include_directories(src/main/cpp/)

add_library( native-lib
             SHARED

             # main game files
             src/main/cpp/native-lib.cpp
             src/main/cpp/Game.cpp
             src/main/cpp/IntroScene.cpp
             src/main/cpp/MainScene.cpp
             src/main/cpp/Maze.cpp

             # game objects
             src/main/cpp/objects/Wall.cpp

             )

target_link_libraries( native-lib
                       log
                       android
                       dgl
                       oboe
                       )

set (OBOE_DIR C:/Users/Rob/AndroidStudioProjects/oboe/oboe-master)
add_subdirectory (${OBOE_DIR} ./oboe-bin)
include_directories (${OBOE_DIR}/include)

set (DGL_DIR C:/Users/Rob/AndroidStudioProjects/DerelictGL)
add_subdirectory (${DGL_DIR} ./dgl-bin)
include_directories (${DGL_DIR}/include/dgl)
