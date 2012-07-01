function(create_cpack_config filename)
	SET(CPACK_OUTPUT_CONFIG_FILE "${filename}")
	INCLUDE(CPack)
endfunction(create_cpack_config)

function (create_signed_xpi PROJNAME CPACK_FILE PEMFILE PASSFILE)
    set (WIX_SOURCES
            ${FB_ROOT}/cmake/dummy.cpp
        )
		
	if (NOT FB_XPI_SUFFIX)
		set (FB_XPI_SUFFIX _Xpi)
	endif()
	
	ADD_LIBRARY(${PROJNAME}${FB_XPI_SUFFIX} STATIC ${WIX_SOURCES})
	ADD_CUSTOM_COMMAND(TARGET ${PROJNAME}${FB_XPI_SUFFIX}
                 POST_BUILD
                 COMMAND cpack --config ${CPACK_FILE}
                 COMMAND ${CMAKE_COMMAND} -E rename ${PROJNAME}-${FBSTRING_PLUGIN_VERSION}-win32.zip ${FB_OUT_DIR}/${PROJNAME}-${FBSTRING_PLUGIN_VERSION}-win32-unsigned.xpi
	)
    if (EXISTS ${PEMFILE})
		ADD_CUSTOM_COMMAND(TARGET ${PROJNAME}${FB_XPI_SUFFIX}
					POST_BUILD
					COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/Common/xpisign.py -f -k ${PEMFILE} -a ${PASSFILE} ${FB_OUT_DIR}/${PROJNAME}-${FBSTRING_PLUGIN_VERSION}-win32-unsigned.xpi ${FB_OUT_DIR}/${PROJNAME}-${FBSTRING_PLUGIN_VERSION}-win32.xpi
		)
	else()
		message("-- No signtool certificate found; assuming development machine (${PEMFILE})")
	endif()
	ADD_DEPENDENCIES(${PROJNAME}${FB_XPI_SUFFIX} ${PROJNAME})
	message("-- Successfully added XPI step")
endfunction(create_signed_xpi)