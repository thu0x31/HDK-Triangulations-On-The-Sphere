cmake_minimum_required( VERSION 3.6 )

project( HDK_SOP_Triangulations_On_The_Sphere )

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if (MSVC)
        set(VCPKG_BUILD_TYPE release)
    endif (MSVC)
endif()

find_package( Houdini REQUIRED )
find_package( CGAL REQUIRED )

set( library_name SOP_Triangulations_On_The_Sphere )

houdini_generate_proto_headers( FILES  src/SOP_Triangulations_On_The_Sphere.cpp )

add_library( ${library_name} SHARED
    src/SOP_Triangulations_On_The_Sphere.cpp
    src/SOP_Triangulations_On_The_Sphere.hpp
)

target_link_libraries( ${library_name} Houdini CGAL)

target_include_directories( ${library_name} PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/src
)

houdini_configure_target( ${library_name} )

if (MSVC)
    set(GMP_DLL_TARGET $<TARGET_FILE_DIR:${library_name}>/gmp-10.dll)
    set(GMP_DLL_OUTPUT_DIR $<TARGET_FILE_DIR:${library_name}>/../bin)
    set(GMP_DLL_OUTPUT ${GMP_DLL_OUTPUT_DIR}/gmp-10.dll)

    add_custom_command(
        TARGET ${library_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${GMP_DLL_TARGET}
            ${GMP_DLL_OUTPUT}
        COMMAND ${CMAKE_COMMAND} -E remove
            ${GMP_DLL_TARGET}
        COMMENT "moved gmp-10.dll"
    )
endif (MSVC)

# TODO: icon