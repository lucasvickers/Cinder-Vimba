# ListCameras
cmake_minimum_required( VERSION 2.8 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE on )

get_filename_component( CINDER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../.." ABSOLUTE )
include( ${CINDER_DIR}/linux/cmake/Cinder.cmake )

project( ListCameras )

get_filename_component( SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src" ABSOLUTE )
get_filename_component( BLOCK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE )
get_filename_component( BLOCK_INC_DIR "${BLOCK_ROOT}/include/civimba" ABSOLUTE )
get_filename_component( VIMBA_INC_DIR "${BLOCK_ROOT}/include" ABSOLUTE )
#does not follow the Vimba/path format of other includes
get_filename_component( VIMBA_TRANSFORM_INC_DIR "${BLOCK_ROOT}/include/VimbaImageTransform" ABSOLUTE )

get_filename_component( BLOCK_SRC_DIR "${BLOCK_ROOT}/src" ABSOLUTE )

# best if these are in a global .so search path, but for good measure
# TODO figure out the RPATH.  cmake rpath wiki
get_filename_component( VIMBA_LIB_DIR "${BLOCK_ROOT}/libs/linux/x64/" ABSOLUTE )

if( NOT TARGET cinder${CINDER_LIB_SUFFIX} )
    find_package( cinder REQUIRED
        PATHS ${CINDER_DIR}/linux/${CMAKE_BUILD_TYPE}/${CINDER_OUT_DIR_PREFIX}
        $ENV{Cinder_DIR}/linux/${CMAKE_BUILD_TYPE}/${CINDER_OUT_DIR_PREFIX}
    )
endif()

# Use PROJECT_NAME since CMAKE_PROJET_NAME returns the top-level project name.
set( EXE_NAME ${PROJECT_NAME} )

set( SRC_FILES
    ${SRC_DIR}/ListCameras.cpp
)

set( BLOCK_SRC_FILES
    ${BLOCK_SRC_DIR}/ApiController.cpp
    ${BLOCK_SRC_DIR}/CameraController.cpp
    ${BLOCK_SRC_DIR}/FrameObserver.cpp
)

add_executable( "${EXE_NAME}" ${BLOCK_SRC_FILES} ${SRC_FILES} )

# TODO figure out which one of these are not needed
include_directories(
    ${INC_DIR}
    ${BLOCK_INC_DIR}
    ${VIMBA_INC_DIR}
    ${VIMBA_TRANSFORM_INC_DIR}
)

target_include_directories(
    "${EXE_NAME}"
    PUBLIC ${INC_DIR}
    PUBLIC ${BLOCK_INC_DIR}
    PUBLIC ${VIMBA_INC_DIR}
    PUBLIC ${VIMBA_TRANSFORM_INC_DIR}
)

get_property( inc_dirs DIRECTORY PROPERTY INCLUDE_DIRECTORIES )
message( "inc_dir = ${inc_dirs}" )

target_link_libraries( "${EXE_NAME}" cinder${CINDER_LIB_SUFFIX} )
