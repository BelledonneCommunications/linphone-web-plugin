#Common cmake function

find_package(Java)

if (NOT FB_XPI_SIGNED_SUFFIX)
	set (FB_XPI_SIGNED_SUFFIX _XPI_signed)
endif()
function (create_signed_xpi PROJNAME DIRECTORY OUT_FILE PEMFILE PASSFILE PROJDEP)
	set (XPI_SOURCES
		${FB_OUT_DIR}/XPI.updated
		${PEMFILE}
		${PASSFILE}
	)
	
	ADD_CUSTOM_TARGET(${PROJNAME}${FB_XPI_SIGNED_SUFFIX} ALL DEPENDS ${OUT_FILE})
	if (EXISTS ${PEMFILE})
		ADD_CUSTOM_COMMAND(OUTPUT ${OUT_FILE}
					DEPENDS ${XPI_SOURCES}
					COMMAND ${CMAKE_COMMAND} -E remove ${OUT_FILE}
					COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/Common/xpisign.py ${DIRECTORY} ${OUT_FILE} ${PEMFILE} ${PASSFILE}
		)
		message("-- Successfully added Sign XPI step")
	else()
		message("-- No signtool certificate found; assuming development machine (${PEMFILE})")
	endif()
	ADD_DEPENDENCIES(${PROJNAME}${FB_XPI_SIGNED_SUFFIX} ${PROJDEP})
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
	

	
	ADD_CUSTOM_TARGET(${PROJNAME}${FB_CRX_SIGNED_SUFFIX} ALL DEPENDS ${OUT_FILE})
	if (EXISTS ${PEMFILE})
		ADD_CUSTOM_COMMAND(OUTPUT ${OUT_FILE}
					DEPENDS ${CRX_SOURCES}
					COMMAND ${CMAKE_COMMAND} -E remove ${OUT_FILE}
					COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/Common/crxmake.py ${DIRECTORY} ${OUT_FILE} ${PEMFILE} ${PASSFILE}
		)
		message("-- Successfully added Sign CRX step")
	else()
		message("-- No signtool certificate found; assuming development machine (${PEMFILE})")
	endif()
	ADD_DEPENDENCIES(${PROJNAME}${FB_CRX_SIGNED_SUFFIX} ${PROJDEP})
endfunction(create_signed_crx)

