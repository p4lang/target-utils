if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -fno-strict-aliasing -Wno-sign-compare -Wno-array-bounds")
else()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -fno-strict-aliasing -fno-aggressive-loop-optimizations -Wno-sign-compare -Wno-shadow -Wno-array-bounds -Wno-stringop-overflow")
endif()

if (FORCE32)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -UJU_64BIT -m32")
else()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DJU_64BIT")
endif()

include(TestBigEndian)
test_big_endian(IS_BIGENDIAN)
if(NOT IS_BIGENDIAN)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DJU_LITTLE_ENDIAN")
endif()

include_directories(target_utils)
add_subdirectory(JudyCommon)
add_subdirectory(JudyL)
add_subdirectory(Judy1)
add_subdirectory(JudySL)
add_subdirectory(JudyHS)

add_library(Judy SHARED EXCLUDE_FROM_ALL
  $<TARGET_OBJECTS:JudyCommon>
  $<TARGET_OBJECTS:JudyL>
  $<TARGET_OBJECTS:Judy1>
  $<TARGET_OBJECTS:JudySL>
  $<TARGET_OBJECTS:JudyHS>
)
