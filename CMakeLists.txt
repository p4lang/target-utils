cmake_minimum_required(VERSION 3.5)

project(libtarget_utils VERSION 0.1 LANGUAGES C)


set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_BUILD_TYPE "RelWithDebInfo")

include(ConfigureChecks)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})


# Including target-syslibs only for standalone build of target-utils
if (STANDALONE)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/target-syslibs/include)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/bigcode/include/target_utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/cJSON)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/klish/target_utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/judy-1.0.5/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/tommyds/tommyds)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/xxHash)

add_subdirectory(src)
add_subdirectory(third-party)

file(COPY include/target_utils DESTINATION ${CMAKE_INSTALL_PREFIX}/include
  PATTERN "*.doxy" EXCLUDE
  PATTERN "*.am" EXCLUDE)

