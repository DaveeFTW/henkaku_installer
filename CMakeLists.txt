cmake_minimum_required(VERSION 2.8)

project(installer)

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
)

include_directories(3rdparty/include)
include_directories(framework/include)
include_directories(${CMAKE_BINARY_DIR}/auto)

add_executable(installer.elf ${INSTALLER_SOURCES} auto/generatedresources.cpp)

target_link_libraries(installer.elf -Wl,-q
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

add_custom_target(installer.fself ALL
	COMMAND vita-elf-create installer.elf installer.velf
	COMMAND vita-make-fself installer.velf installer.fself
	DEPENDS installer.elf
)

