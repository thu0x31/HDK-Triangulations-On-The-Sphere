cmake_minimum_required( VERSION 3.6 )

project( HDK_CGAL_SOP_Triangulations_Sphere )

if (MSVC)
    set(VCPKG_BUILD_TYPE release)
endif (MSVC)

list( APPEND CMAKE_PREFIX_PATH "$ENV{HFS}/toolkit/cmake" )

find_package( Houdini REQUIRED )
find_package( CGAL REQUIRED )

set( library_name SOP_Triangulations_Sphere )

houdini_generate_proto_headers( FILES  SOP_Triangulations_Sphere.cpp )

add_library( ${library_name} SHARED
    SOP_Triangulations_Sphere.cpp
    SOP_Triangulations_Sphere.hpp
)

target_link_libraries( ${library_name} Houdini CGAL)

target_include_directories( ${library_name} PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
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
