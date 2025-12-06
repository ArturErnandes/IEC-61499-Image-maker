cmake_minimum_required(VERSION 3.15)
project(iec61499_bot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)

find_package(TgBot QUIET)
if(NOT TgBot_FOUND)
    pkg_check_modules(TGBOT REQUIRED tgbot)
endif()

pkg_check_modules(CAIRO REQUIRED cairo)
pkg_check_modules(RSVG REQUIRED librsvg-2.0)

find_package(Boost REQUIRED)

add_library(fbsvg STATIC
    src/fb_parser.cpp
    src/svg_renderer.cpp
    src/png_renderer.cpp
    pugixml/pugixml.cpp
)
target_include_directories(fbsvg
    PUBLIC
        ${CMAKE_SOURCE_DIR}/fbsvg
        ${CMAKE_SOURCE_DIR}/pugixml
        ${CAIRO_INCLUDE_DIRS}
        ${RSVG_INCLUDE_DIRS}
)
target_link_directories(fbsvg
    PUBLIC
        ${CAIRO_LIBRARY_DIRS}
        ${RSVG_LIBRARY_DIRS}
)
target_link_libraries(fbsvg
    PUBLIC
        ${CAIRO_LIBRARIES}
        ${RSVG_LIBRARIES}
)

add_executable(bot
    src/bot.cpp
)

target_include_directories(bot
    PRIVATE
        ${CMAKE_SOURCE_DIR}/fbsvg
        ${CMAKE_SOURCE_DIR}
        ${Boost_INCLUDE_DIRS}
)

if(TgBot_FOUND)
    target_link_libraries(bot PRIVATE TgBot::TgBot)
else()
    target_include_directories(bot PRIVATE ${TGBOT_INCLUDE_DIRS})
    target_link_libraries(bot PRIVATE ${TGBOT_LIBRARIES})
endif()

target_link_libraries(bot PRIVATE fbsvg)