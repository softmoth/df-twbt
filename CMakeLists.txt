cmake_minimum_required(VERSION 3.22)
SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(DFHACK ../dfhack)
SET(DFHACK_BUILD ${DFHACK}/build)

IF(EXISTS "${DFHACK_BUILD}/CMakeCache.txt")
    LOAD_CACHE("${DFHACK_BUILD}" READ_WITH_PREFIX DEFAULT_ CMAKE_INSTALL_PREFIX)
    SET(CMAKE_INSTALL_PREFIX ${DEFAULT_CMAKE_INSTALL_PREFIX})
ENDIF()

#set(CMAKE_CXX_STANDARD 11)
PROJECT( twbt )

include(${DFHACK}/CMake/Common.cmake)
string(REPLACE "." "" DFVERNUM ${DF_VERSION})
SET( TWBT_VER "6.xx" )
#SET( DFVERNUM 04705 )
#SET( DFHACKVER 0.47.05-r4 )
#SET( DF /data/develop/projects/dwarf-fortress/install )
#SET( DFHACK /data/develop/projects/dwarf-fortress/dfhack )
set( CMAKE_SHARED_LIBRARY_PREFIX_CXX "" )
SET( PLUGIN_TARGET twbt.plug)
#SET( CMAKE_CXX_FLAGS "-m64 -DLINUX_BUILD -O3 -D_GLIBCXX_USE_CXX11_ABI=0 -DDFHACK_VERSION=\"${DFHACKVER}\" -DDF_${DFVERNUM} -DTWBT_VER=\"\\\"${TWBT_VER}\\\"\"" )
#SET( CMAKE_CXX_FLAGS "-DDFHACK_VERSION=\"${DFHACKVER}\" -DDF_${DFVERNUM} -DTWBT_VER=\"\\\"${TWBT_VER}\\\"\"" )

# source files
SET( SRC twbt.cpp )
SET( DEP renderer.hpp config.hpp dungeonmode.hpp dwarfmode.hpp renderer_twbt.h commands.hpp plugin.hpp tileupdate_text.hpp tileupdate_map.hpp patches.hpp zoomfix.hpp buildings.hpp items.hpp units.hpp Makefile legacy/renderer_legacy.hpp legacy/twbt_legacy.hpp)

# include directories
include_directories(${DFHACK}/library/include)
include_directories(${DFHACK}/library/proto)
include_directories(${DFHACK}/depends/protobuf)
include_directories(${DFHACK}/depends/lua/include)

find_package(PNG)
if(NOT PNG_FOUND)
    # XXX Probably this is wrong. It appears df-twbt/libpng has precompiled
    # libs for Windows? Probably it should be a submodule that has the
    # actual libpng source (which builds with CMake).
    include_directories(libpng)
endif()
add_definitions(${PNG_DEFINITIONS})
#target_link_libraries(${PLUGIN_TARGET} ${PNG_LIBRARIES})

# plugin target
# This is a string, not a number!
add_compile_definitions(TWBT_VER=\"${TWBT_VER}\")
add_compile_definitions(DFHACK_VERSION=${DFHACK_VERSION})
add_compile_definitions(DF_${DFVERNUM})
add_compile_options("-fpermissive")
add_compile_options("-Wno-error")
#add_link_options("-L${DFHACK_BUILD}/library -ldfhack -ldfhack-version")
link_directories("${DFHACK_BUILD}/library")
add_library(${PLUGIN_TARGET} SHARED ${SRC} ${DEP})
target_link_libraries(${PLUGIN_TARGET} PUBLIC dfhack dfhack-version)
install(TARGETS ${PLUGIN_TARGET}
    LIBRARY DESTINATION ${DFHACK_PLUGIN_DESTINATION})
