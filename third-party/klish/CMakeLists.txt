include_directories(. target_utils)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DHAVE_CONFIG_H=1 -DHAVE_LIB_EXPAT -DCLISH_PLUGIN_BUILTIN_LIBS=\"$CLISH_PLUGIN_BUILTIN_LIBS bfshell_plugin_$1.la\" -DCLISH_PLUGIN_BUILTIN_DEFS=\"$CLISH_PLUGIN_BUILTIN_DEFS CLISH_PLUGIN_INIT($1    );\" -DCLISH_PLUGIN_BUILTIN_LIST=\"$CLISH_PLUGIN_BUILTIN_LIST { \"$1\", bfshell_plugin_$1_init },\"") 
add_definitions(-D_GNU_SOURCE)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-pedantic -Wno-error")

add_library(clish_o OBJECT
  clish/plugin_builtin.c
	clish/command/command.c
	clish/command/command_dump.c
	clish/param/param.c
	clish/param/param_dump.c
	clish/pargv/pargv.c
	clish/pargv/pargv_dump.c
	clish/ptype/ptype.c
	clish/ptype/ptype_dump.c
	clish/shell/shell_view.c
	clish/shell/shell_ptype.c
	clish/shell/shell_var.c
	clish/shell/shell_command.c
	clish/shell/shell_dump.c
	clish/shell/shell_execute.c
	clish/shell/shell_help.c
	clish/shell/shell_new.c
	clish/shell/shell_parse.c
	clish/shell/shell_file.c
	clish/shell/shell_loop.c
	clish/shell/shell_startup.c
	clish/shell/shell_wdog.c
	clish/shell/shell_pwd.c
	clish/shell/shell_tinyrl.c
	clish/shell/shell_plugin.c
	clish/shell/shell_xml.c
	clish/shell/shell_roxml.c
	clish/shell/shell_libxml2.c
	clish/shell/shell_expat.c
	clish/shell/shell_udata.c
	clish/shell/shell_misc.c
	clish/shell/shell_thread.c
	clish/shell/shell_strmap.c
	clish/shell/context.c
	clish/view/view.c
	clish/view/view_dump.c
	clish/nspace/nspace.c
	clish/nspace/nspace_dump.c
	clish/var/var.c
	clish/var/var_dump.c
	clish/action/action.c
	clish/action/action_dump.c
	clish/config/config.c
	clish/config/config_dump.c
	clish/hotkey/hotkey.c
	clish/hotkey/hotkey_dump.c
	clish/plugin/plugin.c
	clish/plugin/plugin_dump.c
	clish/udata/udata.c
	lub/argv/argv.c
  lub/bintree/bintree_dump.c
  lub/bintree/bintree_find.c
  lub/bintree/bintree_findfirst.c
  lub/bintree/bintree_findlast.c
  lub/bintree/bintree_findnext.c
  lub/bintree/bintree_findprevious.c
  lub/bintree/bintree_init.c
  lub/bintree/bintree_insert.c
  lub/bintree/bintree_iterator_init.c
  lub/bintree/bintree_iterator_next.c
  lub/bintree/bintree_iterator_previous.c
  lub/bintree/bintree_node_init.c
  lub/bintree/bintree_remove.c
  lub/bintree/bintree_splay.c
	lub/list/list.c
  lub/ctype/ctype.c
  lub/dump/dump.c
	lub/string/string.c
  lub/system/system_test.c
	lub/system/system_file.c
	lub/system/test.c
  lub/db/db.c
	lub/ini/pair.c
	lub/ini/ini.c
  lub/log/log.c
	lub/conv/conv.c
	tinyrl/tinyrl.c
  tinyrl/history/history.c
  tinyrl/history/history_entry.c
	tinyrl/vt100/vt100.c
	konf/tree/tree.c
	konf/tree/tree_dump.c
	konf/query/query.c
	konf/query/query_dump.c
	konf/buf/buf.c
	konf/net/net.c
  #plugins/lua/plugin_init.c
  #plugins/lua/plugin_fini.c
  #plugins/lua/lua_init.c
  #plugins/lua/lua_action.c
  #plugins/lua/lua_print_error.c
	libc/getopt.c
)
add_library(clish  SHARED
  $<TARGET_OBJECTS:clish_o>
  $<TARGET_OBJECTS:bigcode_o>
  #$<TARGET_OBJECTS:expat_o>
)
# expat is static lib
target_compile_options(clish_o PRIVATE "-w")
find_package(Libedit)
find_library(EXPAT_LIB expat)
target_link_libraries(clish PUBLIC ${LIBEDIT_LIBRARIES} ${EXPAT_LIB})



project(bfshell)
add_executable(bfshell bin/bfshell.c)
install(TARGETS bfshell DESTINATION bin)

add_library(bfshell_plugin_clish_o OBJECT
	plugins/clish/builtin_init.c
	plugins/clish/hook_access.c
	plugins/clish/hook_config.c
	plugins/clish/hook_log.c
	plugins/clish/sym_misc.c
	plugins/clish/sym_script.c
)
target_compile_options(bfshell_plugin_clish_o PRIVATE -Wno-unused-parameter)
if (NOT STATIC-LINK-LIB)
  add_library(bfshell_plugin_clish SHARED $<TARGET_OBJECTS:bfshell_plugin_clish_o>)
  target_link_libraries(bfshell_plugin_clish clish)
  SET_TARGET_PROPERTIES(bfshell_plugin_clish PROPERTIES PREFIX "")
endif()
install(FILES target-utils/xml/startup.xml target-utils/xml/types.xml DESTINATION share/cli/xml)
