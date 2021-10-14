set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -DJUDY1")
include_directories(.)
include_directories(..)
include_directories(../JudyCommon)
include_directories(../target_utils)

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/Judy1Next.c PROPERTIES COMPILE_FLAGS -DJUDYNEXT)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Judy1Next.c
  COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/../JudyCommon/JudyPrevNext.c ${CMAKE_CURRENT_BINARY_DIR}/Judy1Next.c
)

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/Judy1NextEmpty.c PROPERTIES COMPILE_FLAGS -DJUDYNEXT)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Judy1NextEmpty.c
  COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/../JudyCommon/JudyPrevNextEmpty.c ${CMAKE_CURRENT_BINARY_DIR}/Judy1NextEmpty.c
)

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/Judy1Prev.c PROPERTIES COMPILE_FLAGS -DJUDYPREV)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Judy1Prev.c
  COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/../JudyCommon/JudyPrevNext.c ${CMAKE_CURRENT_BINARY_DIR}/Judy1Prev.c
)

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/Judy1PrevEmpty.c PROPERTIES COMPILE_FLAGS -DJUDYPREV)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Judy1PrevEmpty.c
  COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/../JudyCommon/JudyPrevNextEmpty.c ${CMAKE_CURRENT_BINARY_DIR}/Judy1PrevEmpty.c
)


add_library(Judy1 OBJECT
../JudyCommon/JudyCascade.c
Judy1Tables.c
../JudyCommon/JudyCascade.c
../JudyCommon/JudyCount.c
../JudyCommon/JudyCreateBranch.c
../JudyCommon/JudyDecascade.c
../JudyCommon/JudyDel.c
../JudyCommon/JudyFirst.c
../JudyCommon/JudyFreeArray.c
../JudyCommon/JudyGet.c
../JudyCommon/JudyInsArray.c
../JudyCommon/JudyIns.c
../JudyCommon/JudyInsertBranch.c
../JudyCommon/JudyMallocIF.c
../JudyCommon/JudyMemActive.c
../JudyCommon/JudyMemUsed.c
${CMAKE_CURRENT_BINARY_DIR}/Judy1Next.c
${CMAKE_CURRENT_BINARY_DIR}/Judy1NextEmpty.c
${CMAKE_CURRENT_BINARY_DIR}/Judy1Prev.c
${CMAKE_CURRENT_BINARY_DIR}/Judy1PrevEmpty.c
)
