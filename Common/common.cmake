#Common cmake function

find_package(Java)

###############################################################################
# Get Core Rootfs tarball
if (NOT FB_CORE_ROOTFS_SUFFIX)
	SET(FB_CORE_ROOTFS_SUFFIX _CoreRootFS)
endif()

function (get_core_rootfs PROJNAME)
	SET(CORE_ROOTFS_TARBALL ${FB_OUT_DIR}/linphone-web-rootfs.tar.gz)
	if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs)
		FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs)
	endif()
	if (NOT EXISTS ${CORE_ROOTFS_TARBALL})
		message("-- Downloading core rootfs")
		FILE(DOWNLOAD ${CORE_ROOTFS_URL} ${CORE_ROOTFS_TARBALL} SHOW_PROGRESS STATUS CORE_ROOTFS_DL_STATUS)
		list(GET CORE_ROOTFS_DL_STATUS 0 CORE_ROOTFS_DL_STATUS_CODE)
		if (${CORE_ROOTFS_DL_STATUS_CODE} EQUAL 0)
			message("     Successful")
		else()
			list(GET CORE_ROOTFS_DL_STATUS 1 CORE_ROOTFS_DL_STATUS_MSG)
			message(FATAL_ERROR "     Failed: ${CORE_ROOTFS_DL_STATUS_CODE} ${CORE_ROOTFS_DL_STATUS_MSG}")
		endif()
	endif()

	ADD_CUSTOM_COMMAND(OUTPUT ${FB_OUT_DIR}/Rootfs.VERSION
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs
		DEPENDS ${CORE_ROOTFS_TARBALL}
		COMMAND ${CMAKE_COMMAND} -E remove -f *
		COMMAND ${CMAKE_COMMAND} -E tar xvzf ${CORE_ROOTFS_TARBALL}
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/VERSION ${FB_OUT_DIR}/Rootfs.VERSION
	)

	ADD_CUSTOM_TARGET(${PROJNAME}${FB_CORE_ROOTFS_SUFFIX} ALL DEPENDS ${FB_OUT_DIR}/Rootfs.VERSION)
	SET_TARGET_PROPERTIES(${PROJNAME}${FB_CORE_ROOTFS_SUFFIX} PROPERTIES FOLDER ${FBSTRING_ProductName})
	ADD_DEPENDENCIES(${PROJNAME} ${PROJNAME}${FB_CORE_ROOTFS_SUFFIX})
	MESSAGE("-- Successfully added Core Rootfs extration step")
endfunction(get_core_rootfs)
###############################################################################

if (NOT FB_XPI_SIGNED_SUFFIX)
	set (FB_XPI_SIGNED_SUFFIX _XPI_signed)
endif()
function (create_signed_xpi PROJNAME DIRECTORY OUT_FILE PEMFILE PASSFILE PROJDEP)
	set (XPI_SOURCES
		${FB_OUT_DIR}/XPI.updated
		${PEMFILE}
		${PASSFILE}
	)
	
	if (EXISTS ${PEMFILE})
		ADD_CUSTOM_TARGET(${PROJNAME}${FB_XPI_SIGNED_SUFFIX} ALL DEPENDS ${OUT_FILE})
		ADD_DEPENDENCIES(${PROJNAME}${FB_XPI_SIGNED_SUFFIX} ${PROJDEP})
		ADD_CUSTOM_COMMAND(OUTPUT ${OUT_FILE}
					DEPENDS ${XPI_SOURCES}
					COMMAND ${CMAKE_COMMAND} -E remove ${OUT_FILE}
					COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/Common/xpisign.py ${DIRECTORY} ${OUT_FILE} ${PEMFILE} ${PASSFILE}
		)
		SET_TARGET_PROPERTIES(${PLUGIN_NAME}${FB_XPI_SIGNED_SUFFIX} PROPERTIES FOLDER ${FBSTRING_ProductName})
		message("-- Successfully added Sign XPI step")
	else()
		message("-- No signtool certificate found; assuming development machine (${PEMFILE})")
	endif()
endfunction(create_signed_xpi)

if (NOT FB_CRX_SIGNED_SUFFIX)
	set (FB_CRX_SIGNED_SUFFIX _CRX_signed)
endif()
function (create_signed_crx PROJNAME DIRECTORY OUT_FILE PEMFILE PASSFILE PROJDEP)
	set (CRX_SOURCES
		${FB_OUT_DIR}/CRX.updated
		${PEMFILE}
		${PASSFILE}
	)
	
	if (EXISTS ${PEMFILE})
		ADD_CUSTOM_TARGET(${PROJNAME}${FB_CRX_SIGNED_SUFFIX} ALL DEPENDS ${OUT_FILE})
		ADD_DEPENDENCIES(${PROJNAME}${FB_CRX_SIGNED_SUFFIX} ${PROJDEP})
		ADD_CUSTOM_COMMAND(OUTPUT ${OUT_FILE}
					DEPENDS ${CRX_SOURCES}
					COMMAND ${CMAKE_COMMAND} -E remove ${OUT_FILE}
					COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/Common/crxmake.py ${DIRECTORY} ${OUT_FILE} ${PEMFILE} ${PASSFILE}
		)
		SET_TARGET_PROPERTIES(${PLUGIN_NAME}${FB_CRX_SIGNED_SUFFIX} PROPERTIES FOLDER ${FBSTRING_ProductName})
		message("-- Successfully added Sign CRX step")
	else()
		message("-- No signtool certificate found; assuming development machine (${PEMFILE})")
	endif()
endfunction(create_signed_crx)

function(configure_file_ext SRC DEST)
	configure_file("${SRC}" "${DEST}" COPYONLY)
	get_cmake_property(_variableNames VARIABLES)
	foreach(VAR ${_variableNames})
		if(VAR MATCHES "^LW_.*$")
			set(VALUE "${${VAR}}")
			execute_process(COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/Common/regex.py" "${DEST}" "${DEST}" "${VAR}" "${VALUE}")
		endif()
	endforeach()
	
	configure_file("${DEST}" "${DEST}")
endfunction(configure_file_ext)

macro(my_sign_file PROJNAME _FILENAME PFXFILE PASSFILE TIMESTAMP_URL)
	if (WIN32)
		if (EXISTS ${PFXFILE})
			message("-- ${_FILENAME} will be signed with ${PFXFILE}")
			GET_FILENAME_COMPONENT(WINSDK_DIR "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows;CurrentInstallFolder]" REALPATH CACHE)
			find_program(SIGNTOOL signtool
				PATHS
				${WINSDK_DIR}/bin
				)
			if (SIGNTOOL)
				set(_STCMD signtool sign /f "${PFXFILE}")
				if (NOT "${PASSFILE}" STREQUAL "")
					set(_STCMD ${_STCMD} /p ${PASSFILE})
				endif()
				if (NOT "${TIMESTAMP_URL}" STREQUAL "")
					set(_STCMD ${_STCMD} /t "${TIMESTAMP_URL}")
				endif()
				set(_STCMD ${_STCMD} "${_FILENAME}")
				ADD_CUSTOM_COMMAND(
					TARGET ${PROJNAME}
					POST_BUILD
					COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/Common/signtool.py ${_STCMD}
					)
				message(STATUS "Successfully added signtool step to sign ${_FILENAME}")
			else()
				message("!! Could not find signtool! Code signing disabled ${SIGNTOOL}")
			endif()
			set(PASSPHRASE "")
		else()
			message(STATUS "No signtool certificate found; assuming development machine (${PFXFILE})")
		endif()
	
	endif()
endmacro(my_sign_file)
