# CameraFeatures
cmake_minimum_required( VERSION 2.8 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE on )

get_filename_component( CINDER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../.." ABSOLUTE )
include( ${CINDER_DIR}/linux/cmake/Cinder.cmake )

project( CameraFeatures )

# various needed directories
get_filename_component( SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src" ABSOLUTE )
get_filename_component( LOCAL_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../include" ABSOLUTE )

get_filename_component( BLOCK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE )
get_filename_component( BLOCK_INC_DIR "${BLOCK_ROOT}/include/civimba" ABSOLUTE )
get_filename_component( VIMBA_INC_DIR "${BLOCK_ROOT}/include" ABSOLUTE )
#does not follow the Vimba/path format of other includes
get_filename_component( VIMBA_TRANSFORM_INC_DIR "${BLOCK_ROOT}/include/VimbaImageTransform" ABSOLUTE )

get_filename_component( BLOCK_SRC_DIR "${BLOCK_ROOT}/src" ABSOLUTE )

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

# project source files
set( SRC_FILES
    ${SRC_DIR}/CameraFeatures.cpp
)

set( BLOCK_SRC_FILES
    ${BLOCK_SRC_DIR}/ApiController.cpp
    ${BLOCK_SRC_DIR}/CameraController.cpp
    ${BLOCK_SRC_DIR}/FrameObserver.cpp
    ${BLOCK_SRC_DIR}/TransformImage.cpp
    ${BLOCK_SRC_DIR}/FeatureContainer.cpp
)

add_executable( "${EXE_NAME}" ${BLOCK_SRC_FILES} ${SRC_FILES} )

find_library( VIMBACPP_LIB NAMES libVimbaCPP.so PATHS ${VIMBA_LIB_DIR} )
find_library( VIMBA_TRANS_LIB NAMES libVimbaC.so PATHS ${VIMBA_LIB_DIR} )
find_library( VIMBAC_LIB NAMES libVimbaImageTransform.so PATHS ${VIMBA_LIB_DIR} )

list( APPEND VIMBA_LIBS ${VIMBACPP_LIB} )
list( APPEND VIMBA_LIBS ${VIMBA_TRANS_LIB} )
list( APPEND VIMBA_LIBS ${VIMBAC_LIB} )

target_link_libraries( "${EXE_NAME}" ${VIMBA_LIBS} )

# TODO figure out which one of these are not needed
#include_directories(
#    ${INC_DIR}
#    ${BLOCK_INC_DIR}
#    ${VIMBA_INC_DIR}
#    ${VIMBA_TRANSFORM_INC_DIR}
#)

target_include_directories(
    "${EXE_NAME}"
    PUBLIC ${LOCAL_INC_DIR}
    PUBLIC ${INC_DIR}
    PUBLIC ${BLOCK_INC_DIR}
    PUBLIC ${VIMBA_INC_DIR}
    PUBLIC ${VIMBA_TRANSFORM_INC_DIR}
)

target_link_libraries( "${EXE_NAME}" cinder${CINDER_LIB_SUFFIX} )
