cmake_minimum_required(VERSION 2.8)

# TODO: replace with toolchain file
set(CMAKE_SYSTEM_NAME "Generic")
set(CMAKE_C_COMPILER "arm-vita-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-vita-eabi-g++")

project(installer)

add_definitions(-Wl,-q -Wall -Werror -pedantic -Os)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")

add_subdirectory(framework)

set(INSTALLER_SOURCES
	"src/main.cpp"
	"src/installerview.cpp"
	"src/camera.cpp"
)
set(INSTALLER_SHADERS
	"shaders/clear.vert.cg"
	"shaders/clear.frag.cg"
)

set(INSTALLER_SHADER_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders)
file(MAKE_DIRECTORY ${INSTALLER_SHADER_OUTPUT_DIR})

function(CompileShaderTargets)
	foreach(shader ${ARGN})
		get_filename_component(name ${shader} NAME_WE)
		get_filename_component(ext ${shader} EXT)
		string(SUBSTRING ${ext} 1 -1 ext)
		get_filename_component(type ${ext} NAME_WE)
		set(resourceName ${name}.${type}.gxp)

		if (${type} STREQUAL "vert")
			set(type "vertex")
		elseif(${type} STREQUAL "frag")
			set(type "fragment")
		endif()

		add_custom_target(
			${resourceName}.c ALL
			COMMAND runshacc --${type} ${CMAKE_CURRENT_SOURCE_DIR}/${shader} ${resourceName}
			COMMAND xxd -i ${resourceName} ${CMAKE_CURRENT_BINARY_DIR}/${resourceName}.c
			COMMENT "Compiling ${type} shader ${resourceName}"
			WORKING_DIRECTORY ${INSTALLER_SHADER_OUTPUT_DIR}
		)

		add_dependencies(framework ${resourceName}.c)
	endforeach(shader)
endfunction()

function(GetShaderResourceFilenames outfilenames)
	set(out)

	foreach(shader ${ARGN})
		get_filename_component(name ${shader} NAME_WE)
		get_filename_component(ext ${shader} EXT)
		string(SUBSTRING ${ext} 1 -1 ext)
		get_filename_component(type ${ext} NAME_WE)
		set(resourceName ${name}.${type}.gxp.c)

		if (NOT ${type} STREQUAL "vert" AND NOT ${type} STREQUAL "frag")
			message(FATAL_ERROR "Unknown shader type '" ${type} "'. Expected 'vert' or 'frag'.")
		endif()

		set(out ${out} ${resourceName})
		set_source_files_properties(${resourceName} PROPERTIES GENERATED TRUE)
	endforeach(shader)

	set(${outfilenames} ${out} PARENT_SCOPE)
endfunction()

GetShaderResourceFilenames(INSTALLER_SHADER_SOURCES ${INSTALLER_SHADERS})

include_directories(framework/include)

add_executable(installer.elf ${INSTALLER_SOURCES} ${INSTALLER_SHADER_SOURCES})
CompileShaderTargets(${INSTALLER_SHADERS})

target_link_libraries(installer.elf -Wl,-q
	framework
	SceDisplay_stub
	SceCtrl_stub
	SceTouch_stub
	SceGxm_stub
	-Wl,--whole-archive pthread -Wl,--no-whole-archive
)

add_custom_target(installer.fself ALL
	COMMAND vita-elf-create installer.elf installer.velf
	COMMAND vita-make-fself installer.velf installer.fself
	DEPENDS installer.elf
)

add_dependencies(installer.elf framework)
