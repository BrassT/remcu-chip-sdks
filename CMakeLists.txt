cmake_minimum_required(VERSION 3.5)

set(REMCU_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/output)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
	message("**************************************************\n\n\n")
	message("not defined toolchain file")
	message("set -DCMAKE_TOOLCHAIN_FILE argument")
	message("for example: cmake . -DCMAKE_TOOLCHAIN_FILE=REMCU/platform/windows_x64.cmake")
	message(FATAL_ERROR "\n\n\n**************************************************")
endif()


include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set(CMAKE_MODULE_PATH ${REMCU_VM_PATH}/cmake)
include(MultiBuild)

ExternalProject_Add(_TEST_LIB_BUILD_ 
      DOWNLOAD_COMMAND ""
      SOURCE_DIR "${REMCU_VM_PATH}/test/"
      CMAKE_ARGS ${CMAKE_ARGS}
      )
