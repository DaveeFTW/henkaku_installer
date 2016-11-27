
set(FRAMEWORK_HEADERS
#	"include/framework/view.h"
)

set(FRAMEWORK_SOURCES
	"src/viewmanager.cpp"
)

include_directories("include")

add_library(framework_objects OBJECT ${FRAMEWORK_SOURCES} ${FRAMEWORK_HEADERS})
add_library(framework $<TARGET_OBJECTS:framework_objects>)

add_executable(framework.elf
	$<TARGET_OBJECTS:framework_objects>
	"src/plugin.cpp"
)

target_link_libraries(framework.elf -Wl,-q -nostdlib -nodefaultlibs)

add_custom_target(framework.suprx ALL
	COMMAND vita-elf-create -n -e ${CMAKE_CURRENT_SOURCE_DIR}/exports.yml framework.elf framework.vuprx
	COMMAND vita-make-fself framework.vuprx framework.suprx
	COMMAND vita-elf-export u framework.elf ${CMAKE_CURRENT_SOURCE_DIR}/exports.yml framework.json
)

add_dependencies(framework.suprx framework.elf)

install(FILES
	${FRAMEWORK_HEADERS}
	DESTINATION include/framework
)

install(TARGETS framework
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)