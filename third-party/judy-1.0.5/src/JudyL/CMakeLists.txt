set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -DJUDYL")
#  -DJUDYGETINLINE -DJUDYNEXT -DJUDYPREV  -DNOSMARTJBB -DNOSMARTJBU -DNOSMARTJLB

include_directories(.)
include_directories(..)
include_directories(../JudyCommon)
include_directories(../target_utils)

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/JudyLNext.c PROPERTIES COMPILE_FLAGS -DJUDYNEXT)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/JudyLNext.c
  COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/../JudyCommon/JudyPrevNext.c ${CMAKE_CURRENT_BINARY_DIR}/JudyLNext.c
)

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/JudyLNextEmpty.c PROPERTIES COMPILE_FLAGS -DJUDYNEXT)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/JudyLNextEmpty.c
  COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/../JudyCommon/JudyPrevNextEmpty.c ${CMAKE_CURRENT_BINARY_DIR}/JudyLNextEmpty.c
)

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/JudyLPrev.c PROPERTIES COMPILE_FLAGS -DJUDYPREV)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/JudyLPrev.c
  COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/../JudyCommon/JudyPrevNext.c ${CMAKE_CURRENT_BINARY_DIR}/JudyLPrev.c
)

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/JudyLPrevEmpty.c PROPERTIES COMPILE_FLAGS -DJUDYPREV)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/JudyLPrevEmpty.c
  COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/../JudyCommon/JudyPrevNextEmpty.c ${CMAKE_CURRENT_BINARY_DIR}/JudyLPrevEmpty.c
)

add_library(JudyL OBJECT
../JudyCommon/JudyCascade.c
JudyLTables.c
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
../JudyCommon/JudyByCount.c
${CMAKE_CURRENT_BINARY_DIR}/JudyLNext.c
${CMAKE_CURRENT_BINARY_DIR}/JudyLNextEmpty.c
${CMAKE_CURRENT_BINARY_DIR}/JudyLPrev.c
${CMAKE_CURRENT_BINARY_DIR}/JudyLPrevEmpty.c
)
