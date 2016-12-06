
set(FRAMEWORK_HEADERS
	"include/framework/view.h"
#	"include/framework/viewmanager.h"
	"include/framework/elapsedtimer.h"
)

set(FRAMEWORK_SOURCES
	"src/view.cpp"
	"src/elapsedtimer.cpp"
	"src/task.cpp"
	"src/taskscheduler.cpp"
	"src/guiapplication.cpp"
	"src/vitainput.cpp"
	"src/vitascreen.cpp"
	"src/gxmshaderpatcher.cpp"
)

set(FRAMEWORK_SHADERS
	"shaders/clear.vert.cg"
	"shaders/clear.frag.cg"
)

set(FRAMEWORK_SHADER_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders)
file(MAKE_DIRECTORY ${FRAMEWORK_SHADER_OUTPUT_DIR})

include_directories("include")

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
			WORKING_DIRECTORY ${FRAMEWORK_SHADER_OUTPUT_DIR}
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

	set(outfilenames ${out} PARENT_SCOPE)
endfunction()

GetShaderResourceFilenames(FRAMEWORK_SHADER_SOURCES ${FRAMEWORK_SHADERS})
add_library(framework STATIC ${FRAMEWORK_SOURCES} ${FRAMEWORK_HEADERS} ${FRAMEWORK_SHADER_SOURCES})

CompileShaderTargets(${FRAMEWORK_SHADERS})

install(FILES
	${FRAMEWORK_HEADERS}
	DESTINATION include/framework
)

install(TARGETS framework
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
