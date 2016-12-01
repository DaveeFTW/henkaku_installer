
set(FRAMEWORK_HEADERS
	"include/framework/view.h"
#	"include/framework/viewmanager.h"
	"include/framework/elapsedtimer.h"
)

set(FRAMEWORK_SOURCES
	"src/view.cpp"
	"src/viewmanager.cpp"
	"src/elapsedtimer.cpp"
	"src/task.cpp"
	"src/taskscheduler.cpp"
	"src/guiapplication.cpp"
	"src/vitainput.cpp"
)

include_directories("include")

add_library(framework STATIC ${FRAMEWORK_SOURCES} ${FRAMEWORK_HEADERS})

install(FILES
	${FRAMEWORK_HEADERS}
	DESTINATION include/framework
)

install(TARGETS framework
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
