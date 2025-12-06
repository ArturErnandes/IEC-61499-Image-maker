cmake_minimum_required(VERSION 3.15)
project(bot_preview LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(bot_preview bot_preview.cpp)

# Try the CMake package first; fall back to pkg-config if it is not available.
find_package(TgBot QUIET)
if(TgBot_FOUND)
    target_link_libraries(bot_preview PRIVATE TgBot::TgBot)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(TGBOT REQUIRED tgbot)
    target_include_directories(bot_preview PRIVATE ${TGBOT_INCLUDE_DIRS})
    target_link_libraries(bot_preview PRIVATE ${TGBOT_LIBRARIES})
endif()

# TgBot headers depend on Boost.PropertyTree; make sure Boost include dirs are visible.
find_package(Boost REQUIRED)
target_include_directories(bot_preview PRIVATE ${Boost_INCLUDE_DIRS})
