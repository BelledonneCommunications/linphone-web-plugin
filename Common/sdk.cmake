##
# Linphone Web - Web plugin of Linphone an audio/video SIP phone
# Copyright (C) 2012-2014 Belledonne Communications
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
##

find_package(PythonInterp REQUIRED)
find_program(ZIP_PROGRAM zip REQUIRED)
find_program(JSDOC_PROGRAM jsdoc REQUIRED)

file(GLOB DOCUMENTATION
	${SDK_STAGE_DIR}/share/doc/linphone-[^.]*.[^.]*.[^.]*/xml/[^.]*.xml
)

file(GLOB TUTORIALS
	${SDK_PROJECT_SOURCE_DIR}/Doc/tutorials/README
	${SDK_PROJECT_SOURCE_DIR}/Doc/tutorials/[^.]*.html
	${SDK_PROJECT_SOURCE_DIR}/Doc/tutorials/[^.]*.js
)

set(SDK_SOURCES
	${SDK_PROJECT_SOURCE_DIR}/README.md
	${SDK_PROJECT_SOURCE_DIR}/LICENSE.md
	${SDK_PROJECT_SOURCE_DIR}/GETTING_STARTED.md
	${SDK_PROJECT_SOURCE_DIR}/Doc/plugin_specifics.js
	${DOCUMENTATION}
	${TUTORIALS}
)

set(SDK_WORK_DIR ${CMAKE_CURRENT_BINARY_DIR}/Sdk)
set(JSWRAPPER_DIR ${SDK_WORK_DIR}/jswrapper)
set(SDK_DIR ${SDK_WORK_DIR}/${SDK_PROJECT_NAME}-${SDK_PROJECT_VERSION}-sdk)

execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory ${SDK_WORK_DIR})
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${SDK_WORK_DIR})
execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory ${JSWRAPPER_DIR})
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${JSWRAPPER_DIR})
execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory ${SDK_DIR})
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${SDK_DIR})
execute_process(COMMAND ${PYTHON_EXECUTABLE} ${SDK_PROJECT_SOURCE_DIR}/Common/concat_files.py ${SDK_WORK_DIR}/MAINPAGE.md ${SDK_PROJECT_SOURCE_DIR}/GETTING_STARTED.md ${SDK_PROJECT_SOURCE_DIR}/README.md ${SDK_PROJECT_SOURCE_DIR}/LICENSE.md)
execute_process(COMMAND ${SDK_STAGE_DIR}/bin/lp-gen-wrappers --output javascript --project linphone ${DOCUMENTATION}
	WORKING_DIRECTORY ${JSWRAPPER_DIR})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${SDK_PROJECT_SOURCE_DIR}/Doc/plugin_specifics.js ${JSWRAPPER_DIR}/linphone/)
file(GLOB JSWRAPPER_SOURCES	${JSWRAPPER_DIR}/linphone/[^.]*.js)
execute_process(COMMAND ${PYTHON_EXECUTABLE} ${SDK_PROJECT_SOURCE_DIR}/Common/concat_files.py ${SDK_DIR}/linphone.js ${JSWRAPPER_SOURCES})
execute_process(COMMAND ${PYTHON_EXECUTABLE} ${SDK_PROJECT_SOURCE_DIR}/Common/clean_js.py ${SDK_DIR}/linphone.js)
execute_process(COMMAND ${JSDOC_PROGRAM} --recurse --destination ${SDK_PROJECT_NAME}-${SDK_PROJECT_VERSION}-doc ${JSWRAPPER_DIR} ${SDK_WORK_DIR}/MAINPAGE.md
	WORKING_DIRECTORY ${SDK_DIR})
execute_process(COMMAND ${PYTHON_EXECUTABLE} ${SDK_PROJECT_SOURCE_DIR}/Common/copy.py ${SDK_PROJECT_SOURCE_DIR}/Doc/tutorials ${SDK_DIR}/tutorials)
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${SDK_DIR}/linphone.js ${SDK_DIR}/tutorials/)
execute_process(COMMAND ${ZIP_PROGRAM} -r ${SDK_FILENAME} ${SDK_PROJECT_NAME}-${SDK_PROJECT_VERSION}-sdk
	WORKING_DIRECTORY ${SDK_WORK_DIR})
