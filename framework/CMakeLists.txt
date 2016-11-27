
set(FRAMEWORK_HEADERS
#	"include/framework/view.h"
)

set(FRAMEWORK_SOURCES
	"src/viewmanager.cpp"
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
