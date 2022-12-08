INCLUDE(ExternalProject)
INCLUDE(ProcessorCount)
ProcessorCount(N)

set(C_FLAGS " -O2 -fPIC")
if (FORCE32)
  set(C_FLAGS "${C_FLAGS} -m32")
endif()

# Including target-syslibs only for standalone build of target-utils
if (STANDALONE)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/target-syslibs/include)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/bigcode/include/target_utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/cJSON)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/judy-1.0.5/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/klish/target_utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/tommyds/tommyds)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/xxHash/)


function(buildTommyds)
  include_directories(tommyds/tommyds/)
  add_library(tommyds_o OBJECT
    tommyds/tommyds/tommyalloc.c
    tommyds/tommyds/tommyarrayblk.c
    tommyds/tommyds/tommyarrayblkof.c
    tommyds/tommyds/tommyarray.c
    tommyds/tommyds/tommyarrayof.c
    tommyds/tommyds/tommyhash.c
    tommyds/tommyds/tommyhashdyn.c
    tommyds/tommyds/tommyhashlin.c
    tommyds/tommyds/tommyhashtbl.c
    tommyds/tommyds/tommylist.c
    tommyds/tommyds/tommytrie.c
    tommyds/tommyds/tommytrieinp.c)
  set_target_properties(tommyds_o PROPERTIES COMPILE_FLAGS "-Wno-pointer-arith -Wno-implicit-fallthrough -Wno-free-nonheap-object")
  add_library(tommyds SHARED EXCLUDE_FROM_ALL $<TARGET_OBJECTS:tommyds_o>)
endfunction()

function(build_cJSON)
  set(ENABLE_CJSON_TEST OFF CACHE BOOL "disable cjson test" FORCE)
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "build only static lib" FORCE)
  add_subdirectory(cJSON)
endfunction()

add_subdirectory(bigcode)
build_cJSON()
add_subdirectory(judy-1.0.5)
add_subdirectory(klish)
buildTommyds()
add_library(xxhash_o OBJECT xxHash/xxhash.c)
add_library(xxhash SHARED EXCLUDE_FROM_ALL $<TARGET_OBJECTS:xxhash_o>)


# required for standalone builds for other packages
file(COPY bigcode/include/target-utils DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
file(COPY cJSON/cJSON.h DESTINATION
  ${CMAKE_INSTALL_PREFIX}/include/target-utils/third-party/cJSON)
file(COPY judy-1.0.5/src/Judy.h DESTINATION
  ${CMAKE_INSTALL_PREFIX}/include/target-utils/third-party/judy-1.0.5/src/)
file(COPY klish/target-utils/ DESTINATION
  ${CMAKE_INSTALL_PREFIX}/include/target-utils)
file(COPY tommyds/tommyds/ DESTINATION
  ${CMAKE_INSTALL_PREFIX}/include/target-utils/third-party/tommyds/tommyds/
  FILES_MATCHING PATTERN *.h)
file(COPY xxHash DESTINATION
  ${CMAKE_INSTALL_PREFIX}/include/target-utils/third-party/xxHash/
  FILES_MATCHING PATTERN *.h)


if(CPYTHON)
  set(TDI_PYTHON_VER 3.10)
  set(LIBPYTHONNAME libpython3.10)
  set(LIBPYTHONNAMETAG v3.10.9)
  set(LIBPYTHONPIP pip3.10)
  EXTERNALPROJECT_ADD(
    ${LIBPYTHONNAME}
    PREFIX ${LIBPYTHONNAME}
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cpython
    CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/cpython/configure --prefix=${CMAKE_INSTALL_PREFIX} --enable-shared LDFLAGS=-Wl,-rpath=${CMAKE_INSTALL_PREFIX}/lib > /dev/null
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} -j${N} > /dev/null
    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} altinstall > /dev/null
    COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/pip_packages &&
    ${CMAKE_INSTALL_PREFIX}/bin/${LIBPYTHONPIP} install --no-index --find-links . -r cpython.requirements.txt
  )

  add_library(cpythonlib UNKNOWN IMPORTED)
  set_property(TARGET cpythonlib PROPERTY IMPORTED_LOCATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${LIBPYTHONNAME}.so)
  add_dependencies(cpythonlib ${LIBPYTHONNAME})
endif()

if(ASAN)
  set(C_FLAGS "${C_FLAGS} -fsanitize=address -fsanitize-recover=address -fsanitize=leak")
endif()
