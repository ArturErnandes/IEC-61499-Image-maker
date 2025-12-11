cmake_minimum_required(VERSION 3.15)
project(iec61499_bot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
find_package(PkgConfig REQUIRED)

pkg_check_modules(CAIRO REQUIRED cairo)
pkg_check_modules(RSVG REQUIRED librsvg-2.0)

set(PUGIXML_BUILD_TESTS OFF CACHE INTERNAL "")
set(PUGIXML_BUILD_EXAMPLES OFF CACHE INTERNAL "")
FetchContent_Declare(
    pugixml
    GIT_REPOSITORY https://github.com/zeux/pugixml.git
    GIT_TAG v1.14
)
FetchContent_MakeAvailable(pugixml)

set(BUILD_TESTS OFF CACHE INTERNAL "")
set(BUILD_EXAMPLES OFF CACHE INTERNAL "")
FetchContent_Declare(
    tgbot
    GIT_REPOSITORY https://github.com/reo7sp/tgbot-cpp.git
    GIT_TAG v1.4.3
)
FetchContent_MakeAvailable(tgbot)

set(FBSVG_SOURCES
    src/fb_parser.cpp
    src/png_renderer.cpp
    src/svg_renderer.cpp
)

add_library(fbsvg STATIC ${FBSVG_SOURCES})
target_include_directories(fbsvg
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
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
        pugixml
        ${CAIRO_LIBRARIES}
        ${RSVG_LIBRARIES}
)

add_executable(fb2svg src/fb2svg.cpp)
target_link_libraries(fb2svg PRIVATE fbsvg)

add_executable(fb2png src/fb2png.cpp)
target_link_libraries(fb2png PRIVATE fbsvg)

add_executable(bot src/bot.cpp)
target_include_directories(bot PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(TARGET TgBot::TgBot)
    set(TGBOT_TARGET TgBot::TgBot)
elseif(TARGET TgBot)
    set(TGBOT_TARGET TgBot)
else()
    message(FATAL_ERROR "TgBot target not found after FetchContent")
endif()
target_link_libraries(bot PRIVATE fbsvg ${TGBOT_TARGET})
