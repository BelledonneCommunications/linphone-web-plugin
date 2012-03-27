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

# pkg-config
find_package(PkgConfig)

# include liblinphone
pkg_check_modules(LINPHONE REQUIRED linphone>=2.5)
link_directories(${LINPHONE_STATIC_LINK_DIRS})
include_directories(${LINPHONE_STATIC_INCLUDE_DIRS})

SOURCE_GROUP(X11 FILES ${PLATFORM})

set (SOURCES
    ${SOURCES}
    ${PLATFORM}
    )

add_x11_plugin(${PROJECT_NAME} SOURCES)

# add library dependencies here; leave ${PLUGIN_INTERNAL_DEPS} there unless you know what you're doing!
target_link_libraries(${PROJECT_NAME} ${PLUGIN_INTERNAL_DEPS})

#link liblinphone
target_link_libraries(${PROJECT_NAME} ${LINPHONE_STATIC_LIBRARIES})
