
include(ExternalProject)

# Location to save the downloaded tarballs. Can be overriden from command line.
set(DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads CACHE PATH "Default download path for tarballs")
set(INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/prefix)

ExternalProject_Add(zlib
	URL http://zlib.net/zlib-1.2.8.tar.gz
	URL_HASH SHA256=36658cb768a54c1d4dec43c3116c27ed893e88b02ecfcb44f2166f9c0b7f2a0d
	DOWNLOAD_DIR ${DOWNLOAD_DIR}
	INSTALL_DIR ${INSTALL_DIR}
	PATCH_COMMAND ${CMAKE_COMMAND} -E remove <SOURCE_DIR>/zconf.h
	BUILD_IN_SOURCE 1
	PATCH_COMMAND ""
	CONFIGURE_COMMAND CHOST=arm-vita-eabi <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
)

ExternalProject_Add(libpng
	DEPENDS zlib
	URL http://sourceforge.net/projects/libpng/files/libpng16/1.6.26/libpng-1.6.26.tar.xz
	URL_HASH SHA256=266743A326986C3DBCEE9D89B640595F6B16A293FD02B37D8C91348D317B73F9
	DOWNLOAD_DIR ${DOWNLOAD_DIR}
	INSTALL_DIR ${INSTALL_DIR}
	BUILD_IN_SOURCE 1
	CONFIGURE_COMMAND CPPFLAGS=-I<INSTALL_DIR>/include LDFLAGS=-L<INSTALL_DIR>/lib <SOURCE_DIR>/configure --host=arm-vita-eabi --enable-arm-neon --disable-shared --enable-static --prefix=<INSTALL_DIR>
)

ExternalProject_Add(freetype
	DEPENDS zlib libpng
	URL http://download.savannah.gnu.org/releases/freetype/freetype-2.7.tar.bz2
	URL_HASH SHA256=D6A451F5B754857D2AA3964FD4473F8BC5C64E879B24516D780FB26BEC7F7D48
	DOWNLOAD_DIR ${DOWNLOAD_DIR}
	INSTALL_DIR ${INSTALL_DIR}
	BUILD_IN_SOURCE 1
	CONFIGURE_COMMAND CPPFLAGS=-I<INSTALL_DIR>/include LDFLAGS=-L<INSTALL_DIR>/lib <SOURCE_DIR>/configure --host=arm-vita-eabi --disable-shared --enable-static --without-harfbuzz --without-bzip2 --prefix=<INSTALL_DIR>
)

set(ZLIB_INCLUDE_DIR ${INSTALL_DIR}/include)
set(ZLIB_LIBRARIES ${INSTALL_DIR}/lib/libz.a)

set(PNG_INCLUDE_DIRS ${INSTALL_DIR}/include)
set(PNG_LIBRARIES ${INSTALL_DIR}/lib/libpng.a)

set(FREETYPE_INCLUDE_DIRS ${INSTALL_DIR}/include)
set(FREETYPE_LIBRARIES ${INSTALL_DIR}/lib/libfreetype.a)
