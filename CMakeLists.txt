cmake_minimum_required(VERSION 2.8)

project(installer)

add_definitions(-Wl,-q -Wall -Werror -pedantic -Os)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")

add_subdirectory(framework)
add_subdirectory(shaders)

set(INSTALLER_SOURCES
	"src/main.cpp"
	"src/installerview.cpp"
	"src/camera.cpp"
	"src/animatedbackground.cpp"
	"src/sysmodule.cpp"
	"src/resource.cpp"
)

include_directories(framework/include)
include_directories(${CMAKE_BINARY_DIR}/auto)
include_directories(${ZLIB_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIR})

add_executable(installer.elf ${INSTALLER_SOURCES} auto/generatedresources.cpp)

target_link_libraries(installer.elf -Wl,-q
	framework
	SceDisplay_stub
	SceCtrl_stub
	SceTouch_stub
	SceGxm_stub
	SceSysmodule_stub
	-Wl,--whole-archive pthread -Wl,--no-whole-archive
	${ZLIB_LIBRARIES}
	${FREETYPE_LIBRARIES}
)

add_dependencies(installer.elf libpng)
add_dependencies(installer.elf freetype)

add_custom_command(
	OUTPUT auto/generatedresources.cpp
	DEPENDS ${CMAKE_SOURCE_DIR}/tools/resource2cpp.py shaders ${CMAKE_BINARY_DIR}/shaders/shaders.rsc
	COMMAND python ${CMAKE_SOURCE_DIR}/tools/resource2cpp.py auto/generatedresources.cpp auto/generatedresources.h shaders/shaders.rsc
)

add_custom_target(installer.fself ALL
	COMMAND vita-elf-create installer.elf installer.velf
	COMMAND vita-make-fself installer.velf installer.fself
	DEPENDS installer.elf
)

