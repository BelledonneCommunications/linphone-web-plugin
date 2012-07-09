#Common cmake function

find_package(Java)

function (create_signed_xpi PROJNAME DIRECTORY OUT_FILE PEMFILE PASSFILE PROJDEP)
    set (WIX_SOURCES
            ${FB_ROOT}/cmake/dummy.cpp
        )
		
	if (NOT FB_XPI_SIGNED_SUFFIX)
		set (FB_XPI_SIGNED_SUFFIX _XPI_signed)
	endif()
	ADD_LIBRARY(${PROJNAME}${FB_XPI_SIGNED_SUFFIX} STATIC ${WIX_SOURCES})
	if (EXISTS ${PEMFILE})
		ADD_CUSTOM_COMMAND(TARGET ${PROJNAME}${FB_XPI_SIGNED_SUFFIX}
					POST_BUILD
					COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/Common/xpisign.py ${DIRECTORY} ${PEMFILE} ${PASSFILE} ${OUT_FILE}
		)
		message("-- Successfully added Sign XPI step")
	else()
		message("-- No signtool certificate found; assuming development machine (${PEMFILE})")
	endif()
	ADD_DEPENDENCIES(${PROJNAME}${FB_XPI_SIGNED_SUFFIX} ${PROJDEP})
endfunction(create_signed_xpi)


function (create_signed_crx PROJNAME DIRECTORY OUT_FILE PEMFILE PASSFILE PROJDEP)
    set (WIX_SOURCES
            ${FB_ROOT}/cmake/dummy.cpp
        )
		
	if (NOT FB_CRX_SIGNED_SUFFIX)
		set (FB_CRX_SIGNED_SUFFIX _CRX_signed)
	endif()
	ADD_LIBRARY(${PROJNAME}${FB_CRX_SIGNED_SUFFIX} STATIC ${WIX_SOURCES})
	if (EXISTS ${PEMFILE})
		ADD_CUSTOM_COMMAND(TARGET ${PROJNAME}${FB_CRX_SIGNED_SUFFIX}
					POST_BUILD
					COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/Common/crxmake.py ${DIRECTORY} ${PEMFILE} ${PASSFILE} ${OUT_FILE}
		)
		message("-- Successfully added Sign CRX step")
	else()
		message("-- No signtool certificate found; assuming development machine (${PEMFILE})")
	endif()
	ADD_DEPENDENCIES(${PROJNAME}${FB_CRX_SIGNED_SUFFIX} ${PROJDEP})
endfunction(create_signed_crx)

