cmake_minimum_required(VERSION 3.1)

# TODO: replace with toolchain file
set(CMAKE_SYSTEM_NAME "Generic")
set(CMAKE_C_COMPILER "arm-vita-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-vita-eabi-g++")

project(installer)

add_definitions(-Wl,-q -Wall -Werror -pedantic -Os)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions")

add_subdirectory(framework)

set(INSTALLER_SOURCES
	"src/main.cpp"
)

include_directories(framework/include)

add_executable(installer.elf ${INSTALLER_SOURCES})

target_link_libraries(installer.elf -Wl,-q
	framework
	pthread
	SceDisplay_stub
)

add_custom_target(installer.fself ALL
	COMMAND vita-elf-create installer.elf installer.velf
	COMMAND vita-make-fself installer.velf installer.fself
)

add_dependencies(installer.fself installer.elf)
add_dependencies(installer.elf framework)
