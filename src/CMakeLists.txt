project(libtarget_utils VERSION 0.1 LANGUAGES C)

add_library(target_sysutil_o OBJECT
  target_utils.c
  hashtbl/hashtbl.c
  bitset/bitset.c
  fbitset/fbitset.c
  id/id.c
  map/map.c
  rbt/rbt.c
  power2_allocator/power2_allocator.c
)
