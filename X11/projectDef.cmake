#/**********************************************************\ 
# Auto-generated X11 project definition file for the
# linphone project
#\**********************************************************/

# X11 template platform definition CMake file
# Included from ../CMakeLists.txt

# remember that the current source dir is the project root; this file is in X11/
file (GLOB PLATFORM RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    X11/[^.]*.cpp
    X11/[^.]*.h
    X11/[^.]*.cmake
    )

# GCC options
add_definitions(-std=c++0x)

include_directories(Rootfs/include/linphone)
include_directories(Rootfs/include)

SOURCE_GROUP(X11 FILES ${PLATFORM})

set (SOURCES
    ${SOURCES}
    ${PLATFORM}
    )

add_x11_plugin(${PROJECT_NAME} SOURCES)

# Add library dependencies here; leave ${PLUGIN_INTERNAL_DEPS} there unless you know what you're doing!
target_link_libraries(${PROJECT_NAME} 
	${PLUGIN_INTERNAL_DEPS}
	"${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/liblinphone.so.5"
	"${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libmediastreamer.so.1"
)

# Copy dll dependencies
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} 
		  PRE_BUILD
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libavcodec.so.53 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libavutil.so.51 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libeXosip2.so.7 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libjpeg.so.8 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/liblinphone.so.5 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libmediastreamer.so.1 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/  
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libogg.so.0 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libortp.so.8 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libosip2.so.7 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libosipparser2.so.7 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libspeex.so.1 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libspeexdsp.so.1 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libswscale.so.2 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libtheora.so.0 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libv4l1.so.0 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libv4l2.so.0 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libv4lconvert.so.0 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libz.so.1 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
)

# Use default chrpath if not defined
IF(NOT DEFINED CMAKE_CHRPATH)
	SET(CMAKE_CHRPATH chrpath)
ENDIF(NOT DEFINED CMAKE_CHRPATH)

# Change rpath
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} 
                 PRE_BUILD
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libavcodec.so.53
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libavutil.so.51
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libjpeg.so.8
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libeXosip2.so.7
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/liblinphone.so.5
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libmediastreamer.so.1           
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libogg.so.0
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libortp.so.8
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libosip2.so.7
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libosipparser2.so.7
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libspeex.so.1
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libspeexdsp.so.1
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libswscale.so.2
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libtheora.so.0
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libv4l1.so.0
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libv4l2.so.0
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libv4lconvert.so.0
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libz.so.1
)

# Add rpath to generated library
SET (CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath,'\$ORIGIN/linphonejs_libs'" )

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/X11/XPI/install.rdf DESTINATION .)
install(TARGETS ${PROJECT_NAME} DESTINATION plugins)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libavcodec.so.53 DESTINATION plugins/linphonejs_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libavutil.so.51 DESTINATION plugins/linphonejs_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libjpeg.so.8 DESTINATION plugins/linphonejs_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libeXosip2.so.7 DESTINATION plugins/linphonejs_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/liblinphone.so.5 DESTINATION plugins/linphonejs_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libmediastreamer.so.1 DESTINATION plugins/linphonejs_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libogg.so.0 DESTINATION plugins/linphonejs_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libortp.so.8 DESTINATION plugins/linphonejs_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libosip2.so.7 DESTINATION plugins/linphonejs_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libosipparser2.so.7 DESTINATION plugins/linphonejs_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libspeex.so.1 DESTINATION plugins/linphonejs_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libspeexdsp.so.1 DESTINATION plugins/linphonejs_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libswscale.so.2 DESTINATION plugins/linphonejs_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libtheora.so.0 DESTINATION plugins/linphonejs_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libv4l1.so.0 DESTINATION plugins/linphonejs_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libv4l2.so.0 DESTINATION plugins/linphonejs_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libv4lconvert.so.0 DESTINATION plugins/linphonejs_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libz.so.1 DESTINATION plugins/linphonejs_libs)

SET(CPACK_GENERATOR "ZIP")
SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY "0")
INCLUDE(CPack)
