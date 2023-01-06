cmake_minimum_required(VERSION 3.10)

project(HardWare_Server)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)


set(INCLUDE_DIR  "${CMAKE_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(CJSON_INC "${CMAKE_SOURCE_DIR}/thirdparty/cJSON")

add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/cJSON)

include_directories(
    ${INCLUDE_DIR}/camera
    ${INCLUDE_DIR}/udpserver
    ${INCLUDE_DIR}/light
	${CJSON_INC}
) 

file(GLOB_RECURSE SRC_FILE
    "${SOURCE_DIR}/light/*.cpp"
	"${SOURCE_DIR}/camera/*.cpp"
	"${SOURCE_DIR}/*cpp"
)

file (GLOB_RECURSE INC_FILE
    "${INCLUDE_DIR}/light/*.h"
    "${INCLUDE_DIR}/camera/*.h"
    "${INCLUDE_DIR}/udpserver/*.h"
)


if(UNIX)
    list(APPEND EXTRA_LIBS
    )
else(WIN32)
    list(APPEND EXTRA_LIBS
    )
endif()

# Add the executable
if (WIN32) 
    add_executable(${PROJECT_NAME} ${SRC_FILE})
else(UNIX)
    add_executable(${PROJECT_NAME} ${SRC_FILE})
endif()

# Add the target includes for MY_PROJECT 
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${SOURCE_DIR})

#===================== LINKING LIBRARIES =======================#
target_link_libraries(${PROJECT_NAME} cjson ${EXTRA_LIBS})
