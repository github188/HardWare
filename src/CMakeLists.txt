# option (use_cxx11
# "Use tutorial provided math implementation" ON)

INCLUDE_DIRECTORIES(include 
                    ${PROJECT_SOURCE_DIR}/thirdparty/ACE_wrappers/
                    RTDBCommon 
                    )

if(CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set(ARCH_NAME x64)
else(CMAKE_SIZEOF_VOID_P EQUAL 4 )
    set(ARCH_NAME x86)
endif (CMAKE_SIZEOF_VOID_P EQUAL 8 )

message(STATUS "ARCH_NAME is ${ARCH_NAME}")

SET(CMAKE_C_COMPILER g++)
set(CMAKE_BUILD_TYPE "Debug")
# set (CMAKE_CXX_STANDARD 11)

if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wno-unused-variable")
else( CMAKE_BUILD_TYPE STREQUAL "Release" )
    set(CMAKE_BUILD_TYPE "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2  -fopenmp")
endif( CMAKE_BUILD_TYPE STREQUAL "Debug" )

# macro(use_cxx11)
# if (CMAKE_VERSION VERSION_LESS "3.1")
#   if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
#     set (CMAKE_CXX_FLAGS "-std=gnu++11 ${CMAKE_CXX_FLAGS}")
#   endif ()
# else ()
#   set (CMAKE_CXX_STANDARD 11)
# endif ()
# endmacro(use_cxx11)

#set(CMAKE_LINK_PATH "3rd_party/lib/${CMAKE_SYSTEM_NAME}/${ARCH_NAME}" ${CMAKE_LINK_PATH})
#set(CMAKE_LINK_PATH "/lib" ${CMAKE_LINK_PATH})

message(STATUS "link_dir is ${CMAKE_LINK_PATH}")
message(STATUS "PROJECT_SOURCE_DIR  is ${PROJECT_SOURCE_DIR}" )
message(STATUS "PROJECT_BINARY_DIR  is ${PROJECT_BINARY_DIR}")

message(STATUS "CMAKE_SYSTEM   is ${CMAKE_SYSTEM}")

SUBDIRS(RTDBCommon
        RTDBBaseClasses
        RTDBPlatformClass
        RTDBDriverFrame
        RTDBDrvSheduleMgr
        RTDBNodeManager
		IOHelpCfgManager
		IODrvBoard
		IONetCltBoard
		IOSerialBoard
        IOServer
        Drivers)
