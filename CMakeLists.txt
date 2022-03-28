cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 11)
PROJECT( twbt )

SET( TWBT_VER "6.xx" )
SET( DFVERNUM 04705 )
SET( DFHACKVER 0.47.05-r4 )
SET( DF /data/develop/projects/dwarf-fortress/install )
SET( DFHACK /data/develop/projects/dwarf-fortress/dfhack )
SET( DFHACK_BUILD ${DFHACK}/build )
set( CMAKE_SHARED_LIBRARY_PREFIX_CXX "" )
SET( PLUGIN_TARGET twbt.plug)
SET( CMAKE_CXX_FLAGS "-m64 -DLINUX_BUILD -O3 -D_GLIBCXX_USE_CXX11_ABI=0 -DDFHACK_VERSION=\"${DFHACKVER}\" -DDF_${DFVERNUM} -DTWBT_VER=\"\\\"${TWBT_VER}\\\"\"" )
#SET( CMAKE_CXX_FLAGS "-DDFHACK_VERSION=\"${DFHACKVER}\" -DDF_${DFVERNUM} -DTWBT_VER=\"\\\"${TWBT_VER}\\\"\"" )

# source files
SET( SRC twbt.cpp )
SET( DEP renderer.hpp config.hpp dungeonmode.hpp dwarfmode.hpp renderer_twbt.h commands.hpp plugin.hpp tileupdate_text.hpp tileupdate_map.hpp patches.hpp zoomfix.hpp buildings.hpp items.hpp units.hpp Makefile legacy/renderer_legacy.hpp legacy/twbt_legacy.hpp)

# include directories
include_directories(${DFHACK}/library/include)
include_directories(${DFHACK}/library/proto)
include_directories(${DFHACK}/depends/protobuf)
include_directories(${DFHACK}/depends/lua/include)

# plugin target
add_compile_options("-fpermissive")
#add_link_options("-L${DFHACK_BUILD}/library -ldfhack -ldfhack-version")
link_directories("${DFHACK_BUILD}/library")
add_library(${PLUGIN_TARGET} SHARED ${SRC} ${DEP})
target_link_libraries(${PLUGIN_TARGET} PUBLIC dfhack dfhack-version)
install(TARGETS ${PLUGIN_TARGET}
        LIBRARY DESTINATION ${DF}/hack/plugins)