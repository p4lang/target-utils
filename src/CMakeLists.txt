project(libtarget_utils VERSION 0.1 LANGUAGES C)

add_library(target_sysutil_o OBJECT
  target_utils.c
  hashtbl/hashtbl.c
  bitset/bitset.c
  fbitset/fbitset.c
  id/id.c
  map/map.c
  power2_allocator/power2_allocator.c
)
add_library(target_utils SHARED 
  $<TARGET_OBJECTS:target_sysutil_o>
  $<TARGET_OBJECTS:xxhash_o>
  $<TARGET_OBJECTS:tommyds_o>
  $<TARGET_OBJECTS:JudyCommon>
  $<TARGET_OBJECTS:JudyL>
  $<TARGET_OBJECTS:Judy1>
  $<TARGET_OBJECTS:JudySL>
  $<TARGET_OBJECTS:JudyHS>
)

target_link_libraries(target_utils PUBLIC cjson)
