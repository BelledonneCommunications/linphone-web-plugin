cmake_minimum_required(VERSION 2.6)
project(LIBXML2 C)

set(LIBXML2_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../libxml2)

include_directories(
	${LIBXML2_SRC_DIR}/include/
	${LIBXML2_SRC_DIR}/win32/VC10/
	${CMAKE_CURRENT_SOURCE_DIR}
)

set(src
	${LIBXML2_SRC_DIR}/buf.c
    ${LIBXML2_SRC_DIR}/c14n.c
    ${LIBXML2_SRC_DIR}/catalog.c
    ${LIBXML2_SRC_DIR}/chvalid.c
    ${LIBXML2_SRC_DIR}/debugXML.c
    ${LIBXML2_SRC_DIR}/dict.c
    ${LIBXML2_SRC_DIR}/DOCBparser.c
    ${LIBXML2_SRC_DIR}/encoding.c
    ${LIBXML2_SRC_DIR}/entities.c
    ${LIBXML2_SRC_DIR}/error.c
    ${LIBXML2_SRC_DIR}/globals.c
    ${LIBXML2_SRC_DIR}/hash.c
    ${LIBXML2_SRC_DIR}/HTMLparser.c
    ${LIBXML2_SRC_DIR}/HTMLtree.c
    ${LIBXML2_SRC_DIR}/legacy.c
    ${LIBXML2_SRC_DIR}/list.c
    ${LIBXML2_SRC_DIR}/nanoftp.c
    ${LIBXML2_SRC_DIR}/nanohttp.c
    ${LIBXML2_SRC_DIR}/parser.c
    ${LIBXML2_SRC_DIR}/parserInternals.c
    ${LIBXML2_SRC_DIR}/pattern.c
    ${LIBXML2_SRC_DIR}/relaxng.c
    ${LIBXML2_SRC_DIR}/SAX.c
    ${LIBXML2_SRC_DIR}/SAX2.c
    ${LIBXML2_SRC_DIR}/schematron.c
    ${LIBXML2_SRC_DIR}/threads.c
    ${LIBXML2_SRC_DIR}/tree.c
    ${LIBXML2_SRC_DIR}/uri.c
    ${LIBXML2_SRC_DIR}/valid.c
    ${LIBXML2_SRC_DIR}/xinclude.c
    ${LIBXML2_SRC_DIR}/xlink.c
    ${LIBXML2_SRC_DIR}/xmlcatalog.c
    ${LIBXML2_SRC_DIR}/xmlIO.c
    ${LIBXML2_SRC_DIR}/xmlmemory.c
    ${LIBXML2_SRC_DIR}/xmlmodule.c
    ${LIBXML2_SRC_DIR}/xmlreader.c
    ${LIBXML2_SRC_DIR}/xmlregexp.c
    ${LIBXML2_SRC_DIR}/xmlsave.c
    ${LIBXML2_SRC_DIR}/xmlschemas.c
    ${LIBXML2_SRC_DIR}/xmlschemastypes.c
    ${LIBXML2_SRC_DIR}/xmlstring.c
    ${LIBXML2_SRC_DIR}/xmlunicode.c
    ${LIBXML2_SRC_DIR}/xmlwriter.c
    ${LIBXML2_SRC_DIR}/xpath.c
    ${LIBXML2_SRC_DIR}/xpointer.c
	${LIBXML2_SRC_DIR}/elfgcchack.h
    ${LIBXML2_SRC_DIR}/libxml.h
    ${LIBXML2_SRC_DIR}/win32/VC10/config.h
)

if(WIN32)
add_definitions(-DHAVE_WIN32_THREADS)

set(libs ws2_32)
endif(WIN32)

add_library(xml2 SHARED ${src})
set_target_properties(xml2 PROPERTIES VERSION 2.9.1 SOVERSION 1)

target_link_libraries(xml2 ${libs})

install(TARGETS xml2
	DESTINATION ${LIB_INSTALL_DIR}
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)
if(WIN32)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/xml2.pdb
	DESTINATION ${LIB_INSTALL_DIR}
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)
endif()
endif(WIN32)


file(GLOB HEADER_FILES
	"${LIBXML2_SRC_DIR}/include/libxml/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/libxml/*.h"
)

install(FILES ${HEADER_FILES}
	DESTINATION include/libxml2/libxml
	PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
