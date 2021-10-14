# fPIC is needed if the library eventually goes into a shared-library.
noinst_LTLIBRARIES = libbigcode.la

libbigcode_includedir = $(includedir)/bfutils/AIM
libbigcode_include_HEADERS = include/bfutils/AIM/aim.h \
							 include/bfutils/AIM/aim_bitmap.h \
						   	 include/bfutils/AIM/aim_config.h \
							 include/bfutils/AIM/aim_daemon.h \
							 include/bfutils/AIM/aim_datatypes.h \
							 include/bfutils/AIM/aim_error.h \
							 include/bfutils/AIM/aim_log.h \
							 include/bfutils/AIM/aim_map.h \
							 include/bfutils/AIM/aim_memory.h \
							 include/bfutils/AIM/aim_object.h \
							 include/bfutils/AIM/aim_porting.h \
							 include/bfutils/AIM/aim_printf.h \
							 include/bfutils/AIM/aim_pvs.h \
							 include/bfutils/AIM/aim_pvs_buffer.h \
							 include/bfutils/AIM/aim_pvs_file.h \
							 include/bfutils/AIM/aim_rl.h \
							 include/bfutils/AIM/aim_sparse.h \
							 include/bfutils/AIM/aim_string.h \
							 include/bfutils/AIM/aim_utils.h \
							 include/bfutils/AIM/aim_valgrind.h \
							 include/bfutils/AIM/aim_valist.h

libucli_includedir = $(includedir)/bfutils/uCli
libucli_include_HEADERS = include/bfutils/uCli/ucli.h \
						  include/bfutils/uCli/ucli_argparse.h \
						  include/bfutils/uCli/ucli_common.h \
						  include/bfutils/uCli/ucli_config.h \
						  include/bfutils/uCli/ucli_errors.h \
						  include/bfutils/uCli/ucli_handler_macros.h \
						  include/bfutils/uCli/ucli_module.h \
						  include/bfutils/uCli/ucli_node.h \
						  include/bfutils/uCli/ucli_porting.h

libbiglist_includedir = $(includedir)/bfutils/BigList
libbiglist_include_HEADERS = include/bfutils/BigList/biglist.h \
							 include/bfutils/BigList/biglist_config.h \
							 include/bfutils/BigList/biglist_locked.h \
							 include/bfutils/BigList/biglist_porting.h

libbigcode_la_CFLAGS = $(AM_CFLAGS) -DUCLI_CONFIG_INCLUDE_ELS_LOOP=1 -DUCLI_CONFIG_INCLUDE_MODULE_NODES=0

# AIM module from infra/modules/AIM.
aim_sources = \
AIM/module/src/aim_pvs.c \
AIM/module/src/aim_object.c \
AIM/module/src/aim_sparse.c \
AIM/module/src/aim_memory.c \
AIM/module/src/aim_pvs_file.c \
AIM/module/src/aim_pvs_syslog.c \
AIM/module/src/aim_rl.c \
AIM/module/src/aim_map.c \
AIM/module/src/aim_bitmap.c \
AIM/module/src/aim_log.h \
AIM/module/src/aim_printf.c \
AIM/module/src/aim_string.c \
AIM/module/src/aim_pvs_buffer.c \
AIM/module/src/aim_error.c \
AIM/module/src/aim_int.h \
AIM/module/src/aim_enums.c \
AIM/module/src/aim_valgrind.c \
AIM/module/src/aim_util.h \
AIM/module/src/aim_daemon.c \
AIM/module/src/aim_datatypes.c \
AIM/module/src/aim_modules_init.c \
AIM/module/src/aim_config.c \
AIM/module/src/aim_log.c \
AIM/module/src/aim_module.c \
include/bfutils/AIM/aim_pvs_file.h \
include/bfutils/AIM/aim_daemon.h \
include/bfutils/AIM/aim_map.h \
include/bfutils/AIM/aim.x \
include/bfutils/AIM/aim_config.h \
include/bfutils/AIM/aim_datatypes.h \
include/bfutils/AIM/aim_string.h \
include/bfutils/AIM/aim_sparse.h \
include/bfutils/AIM/aim_log.h \
include/bfutils/AIM/aim.h \
include/bfutils/AIM/aim_valist.h \
include/bfutils/AIM/aim_memory.h \
include/bfutils/AIM/aim_dox.h \
include/bfutils/AIM/aim_pvs_buffer.h \
include/bfutils/AIM/aim_object.h \
include/bfutils/AIM/aim_valgrind.h \
include/bfutils/AIM/aim_utils.h \
include/bfutils/AIM/aim_pvs_syslog.h \
include/bfutils/AIM/aim_list.h \
include/bfutils/AIM/aim_printf.h \
include/bfutils/AIM/aim_bitmap.h \
include/bfutils/AIM/aim_error.h \
include/bfutils/AIM/aim_pvs.h \
include/bfutils/AIM/aim_porting.h \
include/bfutils/AIM/aim_rl.h

# BigList module from bigcode/modules/BigData/BigList.
biglist_sources = \
BigData/BigList/module/src/biglist_locked_remove_link.c \
BigData/BigList/module/src/biglist_unlock.c \
BigData/BigList/module/src/biglist_locked_remove_link_free.c \
BigData/BigList/module/src/biglist_int.h \
BigData/BigList/module/src/biglist_find.c \
BigData/BigList/module/src/biglist_log.h \
BigData/BigList/module/src/biglist_foreach.c \
BigData/BigList/module/src/biglist_reverse.c \
BigData/BigList/module/src/biglist_last.c \
BigData/BigList/module/src/biglist_locked_free_all.c \
BigData/BigList/module/src/biglist_dump.c \
BigData/BigList/module/src/biglist_locked_prepend.c \
BigData/BigList/module/src/biglist_log.c \
BigData/BigList/module/src/biglist_remove.c \
BigData/BigList/module/src/biglist_append_list.c \
BigData/BigList/module/src/biglist_find_custom.c \
BigData/BigList/module/src/biglist_locked_remove.c \
BigData/BigList/module/src/biglist_to_array.c \
BigData/BigList/module/src/biglist_locked_find.c \
BigData/BigList/module/src/biglist_sort.c \
BigData/BigList/module/src/biglist_from_array.c \
BigData/BigList/module/src/biglist_remove_link_free.c \
BigData/BigList/module/src/biglist_from_data_array.c \
BigData/BigList/module/src/biglist_copy.c \
BigData/BigList/module/src/biglist_next.c \
BigData/BigList/module/src/biglist_lock.c \
BigData/BigList/module/src/biglist_remove_link.c \
BigData/BigList/module/src/biglist_append.c \
BigData/BigList/module/src/biglist_length.c \
BigData/BigList/module/src/biglist_locked_length.c \
BigData/BigList/module/src/biglist_prepend_list.c \
BigData/BigList/module/src/biglist.c \
BigData/BigList/module/src/biglist_alloc.c \
BigData/BigList/module/src/biglist_prev.c \
BigData/BigList/module/src/biglist_locked_create.c \
BigData/BigList/module/src/biglist_to_data_array.c \
BigData/BigList/module/src/biglist_locked_append.c \
BigData/BigList/module/src/biglist_locked_free.c \
BigData/BigList/module/src/biglist_free.c \
BigData/BigList/module/src/biglist_insert_sorted.c \
BigData/BigList/module/src/biglist_prepend.c \
BigData/BigList/module/src/biglist_free_all.c \
BigData/BigList/module/src/biglist_config.c \
include/bfutils/BigList/biglist.h \
include/bfutils/BigList/biglist_locked.h \
include/bfutils/BigList/biglist_config.h \
include/bfutils/BigList/biglist_porting.h \
include/bfutils/BigList/biglist_dox.h

# ELS module from bigcode/modules/ELS.
els_sources = \
ELS/module/src/els_int.h \
ELS/module/src/els_config.c \
ELS/module/src/els_log.c \
ELS/module/src/els_enums.c \
ELS/module/src/els_ucli.c \
ELS/module/src/els_module.c \
ELS/module/src/els_log.h \
ELS/module/src/els.c \
include/bfutils/ELS/els_dox.h \
include/bfutils/ELS/els.x \
include/bfutils/ELS/els_porting.h \
include/bfutils/ELS/els_config.h \
include/bfutils/ELS/els.h

# IOF module from bigcode/modules/IOF.
iof_sources = \
IOF/module/src/iof_log.c \
IOF/module/src/iof_log.h \
IOF/module/src/iof_module.c \
IOF/module/src/iof.c \
IOF/module/src/iof_config.c \
include/bfutils/IOF/iof.h \
include/bfutils/IOF/iof_porting.h \
include/bfutils/IOF/iof_dox.h \
include/bfutils/IOF/iof_config.h

# OS modules from bigcode/modules/OS.
os_sources = \
OS/module/src/os_ucli.c \
OS/module/src/os_config.c \
OS/module/src/os_int.h \
OS/module/src/os_module.c \
OS/module/src/os_time_posix.c \
OS/module/src/os_log.c \
OS/module/src/os_log.h \
OS/module/src/os_sem_posix.c \
OS/module/src/os_enums.c \
include/bfutils/OS/os_sleep.h \
include/bfutils/OS/os_config.h \
include/bfutils/OS/os_sem.h \
include/bfutils/OS/os_time.h \
include/bfutils/OS/os_porting.h \
include/bfutils/OS/os.h \
include/bfutils/OS/os.x

# uCli modules from bigcode/modules/uCli.
ucli_sources = \
uCli/module/src/ucli_util.c \
uCli/module/src/ucli.c \
uCli/module/src/ucli_handlers.c \
uCli/module/src/ucli_enums.c \
uCli/module/src/ucli_util.h \
uCli/module/src/ucli_log.h \
uCli/module/src/ucli_int.h \
uCli/module/src/ucli_config.c \
uCli/module/src/ucli_argparse.c \
uCli/module/src/ucli_log.c \
uCli/module/src/ucli_error.c \
uCli/module/src/ucli_printf.c \
uCli/module/src/ucli_module.c \
uCli/module/src/ucli_node.c \
include/bfutils/uCli/ucli_config.h \
include/bfutils/uCli/ucli.h \
include/bfutils/uCli/ucli_module.h \
include/bfutils/uCli/ucli_node.h \
include/bfutils/uCli/ucli_argparse.h \
include/bfutils/uCli/ucli_dox.h \
include/bfutils/uCli/ucli.x \
include/bfutils/uCli/ucli_errors.h \
include/bfutils/uCli/ucli_common.h \
include/bfutils/uCli/ucli_handler_macros.h \
include/bfutils/uCli/ucli_porting.h

libbigcode_la_SOURCES = $(biglist_sources) \
$(els_sources) $(iof_sources) $(os_sources) $(aim_sources) $(ucli_sources)
