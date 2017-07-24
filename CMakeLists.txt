cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{VITASDK})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
    else()
        message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
    endif()
endif()

set(SHORT_NAME installer)
project(${SHORT_NAME})
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_APP_NAME "Installer")
set(VITA_TITLEID  "MLCL00004")

find_package(Freetype REQUIRED)
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})

option(USE_FILESYSTEM_TEXTURES "toggle whether to store textures within the executable or source from file system" ON)

add_definitions(-Wl,-q -Wall -Werror -pedantic -Os)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")

if (${USE_FILESYSTEM_TEXTURES})
    add_definitions(-DTEXTURE_SOURCE_PREFIX="app0:/")
else()
    add_definitions(-DTEXTURE_SOURCE_PREFIX="rsc:/")
endif(${USE_FILESYSTEM_TEXTURES})

add_subdirectory(framework)
add_subdirectory(shaders)
add_subdirectory(assets)

set(INSTALLER_SOURCES
    "src/main.cpp"
    "src/installerview.cpp"
    "src/camera.cpp"
    "src/animatedbackground.cpp"
    "src/sysmodule.cpp"
    "src/resource.cpp"
    "src/textureatlas.cpp"
    "src/characteratlas.cpp"
    "src/font.cpp"
    "src/textrenderer.cpp"
    "src/geometryrenderer.cpp"
    "src/text.cpp"
    "src/fpscounter.cpp"
    "src/numberanimation.cpp"
    "src/buttoneventfilter.cpp"
    "src/welcomepage.cpp"
    "src/installoptionpage.cpp"
    "src/resetpage.cpp"
    "src/configpage.cpp"
    "src/offlinepage.cpp"
    "src/confirmpage.cpp"
    "src/installpage.cpp"
    "src/successpage.cpp"
    "src/failurepage.cpp"
    "src/menu.cpp"
    "src/checkbox.cpp"
    "src/checkboxmenu.cpp"
)

include_directories(3rdparty/include)
include_directories(framework/include)
include_directories(${CMAKE_BINARY_DIR}/auto)

add_executable(${SHORT_NAME} ${INSTALLER_SOURCES} auto/generatedresources.cpp)

target_link_libraries(${SHORT_NAME} -Wl,-q
    framework
    SceDisplay_stub
    SceCtrl_stub
    SceTouch_stub
    SceGxm_stub
    SceSysmodule_stub
    -Wl,--whole-archive pthread -Wl,--no-whole-archive
    ${FREETYPE_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${PNG_LIBRARIES}
)

add_custom_command(
    OUTPUT auto/generatedresources.cpp
    DEPENDS ${CMAKE_SOURCE_DIR}/tools/resource2cpp.py shaders ${CMAKE_BINARY_DIR}/shaders/shaders.rsc assets ${CMAKE_BINARY_DIR}/assets/assets.rsc
    COMMAND python ${CMAKE_SOURCE_DIR}/tools/resource2cpp.py auto/generatedresources.cpp auto/generatedresources.h shaders/shaders.rsc assets/assets.rsc
)

vita_create_self(${SHORT_NAME}.self ${SHORT_NAME}
)

vita_create_vpk(${SHORT_NAME}.vpk ${VITA_TITLEID} ${SHORT_NAME}.self
    VERSION ${VITA_VERSION}
    NAME ${VITA_APP_NAME}
)

