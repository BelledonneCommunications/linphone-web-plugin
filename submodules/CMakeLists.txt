cmake_minimum_required(VERSION 2.8)

if(WIN32)
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /SAFESEH:NO")
	set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL} /SAFESEH:NO")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SAFESEH:NO")
	set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /SAFESEH:NO")
	set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /SAFESEH:NO")
	set(CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL "${CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL} /SAFESEH:NO")
	set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /SAFESEH:NO")
	set(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO} /SAFESEH:NO")
	set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} /SAFESEH:NO")
	set(CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL "${CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL} /SAFESEH:NO")
	set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} /SAFESEH:NO")
	set(CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO} /SAFESEH:NO")
endif(WIN32)


set(MS2_ENABLE_GSM OFF CACHE BOOL "Build mediastreamer2 with the GSM codec.")


include(ExternalProject)

if(WIN32)
	set(LIB_INSTALL_DIR "bin")

	# add these flags to the build of all the external projects
	set(RECURSIVE_C_FLAGS "/D _WIN32_WINNT=0x0501 /D _ALLOW_KEYWORD_MACROS")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${RECURSIVE_C_FLAGS}")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${RECURSIVE_C_FLAGS}")
	set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} ${RECURSIVE_C_FLAGS}")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${RECURSIVE_C_FLAGS}")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} ${RECURSIVE_C_FLAGS}")
else(WIN32)
	set(LIB_INSTALL_DIR "lib")
endif(WIN32)

set(GENERAL_EP_VARS)

macro(_expand_external_project_vars)
  set(GENERAL_EP_ARGS "")
  set(GENERAL_EP_VARNAMES "")
  foreach(arg ${GENERAL_EP_VARS})
    string(REPLACE ":" ";" varname_and_vartype ${arg})
    set(target_info_list ${target_info_list})
    list(GET varname_and_vartype 0 _varname)
    list(GET varname_and_vartype 1 _vartype)
    list(APPEND GENERAL_EP_ARGS -D${_varname}:${_vartype}=${${_varname}})
    list(APPEND GENERAL_EP_VARNAMES ${_varname})
  endforeach()
endmacro()

list(APPEND GENERAL_EP_VARS
	CMAKE_SKIP_RPATH:BOOL
	CMAKE_MODULE_PATH:PATH
	CMAKE_BUILD_TYPE:STRING
	CMAKE_CXX_COMPILER:PATH
	CMAKE_CXX_FLAGS:STRING
	CMAKE_CXX_FLAGS_DEBUG:STRING
	CMAKE_CXX_FLAGS_MINSIZEREL:STRING
	CMAKE_CXX_FLAGS_RELEASE:STRING
	CMAKE_CXX_FLAGS_RELWITHDEBINFO:STRING
	CMAKE_C_COMPILER:PATH
	CMAKE_C_FLAGS:STRING
	CMAKE_C_FLAGS_DEBUG:STRING
	CMAKE_C_FLAGS_MINSIZEREL:STRING
	CMAKE_C_FLAGS_RELEASE:STRING
	CMAKE_C_FLAGS_RELWITHDEBINFO:STRING
	CMAKE_EXE_LINKER_FLAGS:STRING
	CMAKE_EXE_LINKER_FLAGS_DEBUG:STRING
	CMAKE_EXE_LINKER_FLAGS_MINSIZEREL:STRING
	CMAKE_EXE_LINKER_FLAGS_RELEASE:STRING
	CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO:STRING
	CMAKE_MODULE_LINKER_FLAGS:STRING
	CMAKE_MODULE_LINKER_FLAGS_DEBUG:STRING
	CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL:STRING
	CMAKE_MODULE_LINKER_FLAGS_RELEASE:STRING
	CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO:STRING
	CMAKE_SHARED_LINKER_FLAGS:STRING
	CMAKE_SHARED_LINKER_FLAGS_DEBUG:STRING
	CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL:STRING
	CMAKE_SHARED_LINKER_FLAGS_RELEASE:STRING
	CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO:STRING
	CMAKE_GENERATOR:STRING
	CMAKE_EXTRA_GENERATOR:STRING
	CMAKE_INSTALL_PREFIX:PATH
	CMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH
	CMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH
	CMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH
	CMAKE_BUNDLE_OUTPUT_DIRECTORY:PATH
	CMAKE_SHARED_LINKER_FLAGS:STRING
	CMAKE_EXE_LINKER_FLAGS:STRING
	CMAKE_MODULE_LINKER_FLAGS:STRING
	LIB_INSTALL_DIR:STRING
)
if(APPLE)
	list(APPEND GENERAL_EP_VARS
		CMAKE_OSX_DEPLOYMENT_TARGET:STRING
		CMAKE_OSX_ARCHITECTURES:STRING
	)
endif(APPLE)

_expand_external_project_vars()

ExternalProject_Add(EP_polarssl
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/polarssl
	CMAKE_ARGS ${GENERAL_EP_ARGS}
)

if(WIN32)
	ExternalProject_Add(EP_antlr3c
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/antlr3/runtime/C
		CMAKE_ARGS ${GENERAL_EP_ARGS}
	)

	ExternalProject_Add(EP_bellesip
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/belle-sip
		CMAKE_ARGS ${GENERAL_EP_ARGS} -DANTLR3_ROOT_DIR=${CMAKE_CURRENT_SOURCE_DIR}/antlr3
	)
	add_dependencies(EP_bellesip EP_antlr3c EP_polarssl)

	ExternalProject_Add(EP_xml2
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake/libxml2
		CMAKE_ARGS ${GENERAL_EP_ARGS}
	)

	ExternalProject_Add(EP_speex
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/speex
		CMAKE_ARGS ${GENERAL_EP_ARGS}
	)

	ExternalProject_Add(EP_ortp
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/linphone/oRTP
		CMAKE_ARGS ${GENERAL_EP_ARGS}
	)

	ExternalProject_Add(EP_ms2
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/linphone/mediastreamer2
		CMAKE_ARGS ${GENERAL_EP_ARGS} -DMS2_ENABLE_GSM=0
	)
	add_dependencies(EP_ms2 EP_ortp EP_speex)

	ExternalProject_Add(EP_linphone
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/linphone
		CMAKE_ARGS ${GENERAL_EP_ARGS}
	)
	add_dependencies(EP_linphone EP_ms2 EP_bellesip EP_xml2)
else(WIN32)
	if(APPLE)
		set(ARCH_CFLAGS "-O3 -mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET} -arch ${CMAKE_OSX_ARCHITECTURES}")
		set(ARCH_CPPFLAGS "${ARCH_CFLAGS}")
		set(ARCH_LDFLAGS "-mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET} -arch ${CMAKE_OSX_ARCHITECTURES}")
		set(ARCH_BUILD "i686-apple-darwin")
		set(ARCH_HOST "i686-apple-darwin")
	else(APPLE)
		set(ARCH_CFLAGS "-O3")
		set(ARCH_CPPFLAGS "")
		set(ARCH_LDFLAGS "")
		set(ARCH_BUILD "")
		set(ARCH_HOST "")
	endif(APPLE)

	ExternalProject_Add(EP_srtp
		GIT_REPOSITORY git://git.linphone.org/srtp.git
		GIT_TAG master
		CONFIGURE_COMMAND ./configure --prefix=${CMAKE_INSTALL_PREFIX} --build=${ARCH_BUILD} --host=${ARCH_HOST} "--enable-shared" "--enable-pic" "CFLAGS=${ARCH_CFLAGS}" "CPPFLAGS=${ARCH_CPPFLAGS}" "LDFLAGS=${ARCH_LDFLAGS}"
		BUILD_IN_SOURCE 1
	)

	set(EP_ffmpeg_ARCH "i386")
	if(APPLE)
		set(EP_ffmpeg_TARGET_OS "darwin")
		set(EP_ffmpeg_PATCH_COMMAND "patch" "-p1" "<" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ffmpeg/configure-osx.patch")
	else(APPLE)
		set(EP_ffmpeg_TARGET_OS "linux")
		set(EP_ffmpeg_PATCH_COMMAND "")
	endif(APPLE)
	set(EP_ffmpeg_CONFIGURE_OPTIONS "--enable-shared" "--disable-static" "--enable-pic" "--disable-bzlib" "--disable-mmx" "--disable-ffprobe" "--disable-ffserver" "--disable-avdevice" "--disable-avfilter" "--disable-network" "--disable-avformat" "--disable-everything" "--enable-decoder=mjpeg" "--enable-encoder=mjpeg")
	ExternalProject_Add(EP_ffmpeg
		URL http://ffmpeg.org/releases/ffmpeg-0.10.2.tar.gz
		PATCH_COMMAND ${EP_ffmpeg_PATCH_COMMAND}
		CONFIGURE_COMMAND ./configure --prefix=${CMAKE_INSTALL_PREFIX} --arch=${EP_ffmpeg_ARCH} --target-os=${EP_ffmpeg_TARGET_OS} "--extra-cflags=${ARCH_CFLAGS}" "--extra-ldflags=${ARCH_LDFLAGS}" ${EP_ffmpeg_CONFIGURE_OPTIONS}
		BUILD_IN_SOURCE 1
	)

	ExternalProject_Add(EP_opus
		URL http://downloads.xiph.org/releases/opus/opus-1.0.3.tar.gz
		CONFIGURE_COMMAND ./configure --prefix=${CMAKE_INSTALL_PREFIX} --build=${ARCH_BUILD} --host=${ARCH_HOST} "CFLAGS=${ARCH_CFLAGS}" "CPPFLAGS=${ARCH_CPPFLAGS}" "LDFLAGS=${ARCH_LDFLAGS}" "PKG_CONFIG_LIBDIR=${CMAKE_INSTALL_PREFIX}/lib/pkgconfig"
		BUILD_IN_SOURCE 1
	)

	if(APPLE)
		set(EP_vpx_TARGET "x86-darwin10-gcc")
	else(APPLE)
		set(EP_vpx_TARGET "generic-gnu")
	endif(APPLE)
	ExternalProject_Add(EP_vpx
		GIT_REPOSITORY https://chromium.googlesource.com/webm/libvpx
		GIT_TAG v1.3.0
		CONFIGURE_COMMAND ./configure --prefix=${CMAKE_INSTALL_PREFIX} --target=${EP_vpx_TARGET} --enable-static --disable-shared --enable-pic --enable-error-concealment --enable-realtime-only --enable-spatial-resampling --enable-vp8 --disable-vp9 --enable-libs --disable-install-docs --disable-debug-libs --disable-examples --disable-unit-tests "--extra-cflags=-O3"
		BUILD_IN_SOURCE 1
	)

	if(NOT WIN32 AND NOT APPLE)
		# Compile v4l on Linux platform
		ExternalProject_Add(EP_v4l
			URL http://linuxtv.org/downloads/v4l-utils/v4l-utils-1.0.0.tar.bz2
			CONFIGURE_COMMAND ./configure --prefix=${CMAKE_INSTALL_PREFIX} --build=${ARCH_BUILD} --host=${ARCH_HOST} --disable-v4l-utils --disable-libdvbv5 --with-udevdir=${CMAKE_INSTALL_PREFIX}/etc --without-jpeg "CFLAGS=${ARCH_CFLAGS}" "CPPFLAGS=${ARCH_CPPFLAGS}" "LDFLAGS=${ARCH_LDFLAGS}" "PKG_CONFIG_LIBDIR=${CMAKE_INSTALL_PREFIX}/lib/pkgconfig"
			BUILD_IN_SOURCE 1
		)
	endif(NOT WIN32 AND NOT APPLE)

	set(EP_antlr3c_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/antlr3/runtime/C)
	ExternalProject_Add(EP_antlr3c
		SOURCE_DIR ${EP_antlr3c_SOURCE_DIR}
		CONFIGURE_COMMAND ./autogen.sh && ./configure --prefix=${CMAKE_INSTALL_PREFIX} --build=${ARCH_BUILD} --host=${ARCH_HOST} "CFLAGS=${ARCH_CFLAGS}" "CPPFLAGS=${ARCH_CPPFLAGS}" "LDFLAGS=${ARCH_LDFLAGS}" "PKG_CONFIG_LIBDIR=${CMAKE_INSTALL_PREFIX}/lib/pkgconfig"
		BUILD_IN_SOURCE 1
	)

	set(EP_bellesip_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/belle-sip)
	ExternalProject_Add(EP_bellesip
		SOURCE_DIR ${EP_bellesip_SOURCE_DIR}
		CONFIGURE_COMMAND ./autogen.sh && ./configure --prefix=${CMAKE_INSTALL_PREFIX} --build=${ARCH_BUILD} --host=${ARCH_HOST} "CFLAGS=${ARCH_CFLAGS}" "CPPFLAGS=${ARCH_CPPFLAGS}" "LDFLAGS=${ARCH_LDFLAGS}" "PKG_CONFIG_LIBDIR=${CMAKE_INSTALL_PREFIX}/lib/pkgconfig"

		BUILD_IN_SOURCE 1
	)
	add_dependencies(EP_bellesip EP_antlr3c EP_polarssl)

	set(EP_xml2_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libxml2)
	ExternalProject_Add(EP_xml2
		SOURCE_DIR ${EP_xml2_SOURCE_DIR}
		CONFIGURE_COMMAND ${EP_xml2_SOURCE_DIR}/autogen.sh && ${EP_xml2_SOURCE_DIR}/configure --prefix=${CMAKE_INSTALL_PREFIX} --build=${ARCH_BUILD} --host=${ARCH_HOST} --without-legacy --with-catalog --with-docbook --without-python --with-c14n "CFLAGS=${ARCH_CFLAGS}" "CPPFLAGS=${ARCH_CPPFLAGS}" "LDFLAGS=${ARCH_LDFLAGS}" "PKG_CONFIG_LIBDIR=${CMAKE_INSTALL_PREFIX}/lib/pkgconfig"

	)

	set(EP_speex_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/speex)
	ExternalProject_Add(EP_speex
		SOURCE_DIR ${EP_speex_SOURCE_DIR}
		CONFIGURE_COMMAND ${EP_speex_SOURCE_DIR}/autogen.sh && ${EP_speex_SOURCE_DIR}/configure --prefix=${CMAKE_INSTALL_PREFIX} --build=${ARCH_BUILD} --host=${ARCH_HOST} --enable-fixed-point --disable-float-api --disable-vbr --disable-ogg --disable-oggtest "CFLAGS=${ARCH_CFLAGS}" "CPPFLAGS=${ARCH_CPPFLAGS}" "LDFLAGS=${ARCH_LDFLAGS}" "PKG_CONFIG_LIBDIR=${CMAKE_INSTALL_PREFIX}/lib/pkgconfig"

	)

	set(EP_linphone_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/linphone)
	ExternalProject_Add(EP_linphone
		SOURCE_DIR ${EP_linphone_SOURCE_DIR}
		CONFIGURE_COMMAND ./autogen.sh && ./configure --prefix=${CMAKE_INSTALL_PREFIX} --build=${ARCH_BUILD} --host=${ARCH_HOST} --disable-nls --enable-console_ui=no --enable-gtk_ui=no --enable-assistant=no --enable-debug=yes --enable-x11=no --disable-glx --disable-zrtp --enable-relativeprefix "--with-srtp=${CMAKE_INSTALL_PREFIX}" "CFLAGS=${ARCH_CFLAGS}" "CPPFLAGS=${ARCH_CPPFLAGS}" "LDFLAGS=${ARCH_LDFLAGS}" "PKG_CONFIG_LIBDIR=${CMAKE_INSTALL_PREFIX}/lib/pkgconfig"

		BUILD_IN_SOURCE 1
	)
	add_dependencies(EP_linphone EP_bellesip EP_xml2 EP_srtp EP_speex EP_opus EP_ffmpeg EP_vpx)
	if(NOT WIN32 AND NOT APPLE)
		add_dependencies(EP_linphone EP_v4l)
	endif(NOT WIN32 AND NOT APPLE)
endif(WIN32)

