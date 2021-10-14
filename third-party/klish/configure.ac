#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
m4_define([MAJOR_VERSION], 2)
m4_define([MINOR_VERSION], 1)
m4_define([MICRO_VERSION], 3)

AC_PREREQ(2.59)
AC_INIT([klish],
        [MAJOR_VERSION.MINOR_VERSION.MICRO_VERSION],
        [serj.kalichev at gmail dot com])

AC_CONFIG_AUX_DIR(aux_scripts)
AC_CONFIG_MACRO_DIR([m4])
AM_SILENT_RULES([yes])

# Values for SONAME. See -version-info for details.
AC_SUBST(SONAME_CURRENT, 1)
AC_SUBST(SONAME_REVISION, 0)
AC_SUBST(SONAME_AGE, 0)

# Check for system extensions (_POSIX_THREAD_SEMANTICS for Solaris)
AC_USE_SYSTEM_EXTENSIONS

# Checks for programs.
AC_PROG_CC
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE(subdir-objects)
AM_PROG_CC_C_O

# Dir for libc replacements
AC_CONFIG_LIBOBJ_DIR([libc])

# needed to handle 64-bit architecture
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(size_t)

#########################################
# See if linker supports version scripts
#########################################
# Check if LD supports linker scripts,
# and define automake conditional HAVE_LD_VERSION_SCRIPT if so.
AC_ARG_ENABLE([ld-version-script],
              AS_HELP_STRING([--enable-ld-version-script],
                             [enable linker version script (default is enabled when possible)]),
                             [have_ld_version_script=$enableval], [])
if test -z "$have_ld_version_script"; then
    AC_MSG_CHECKING([if LD -Wl,--version-script works])
    save_LDFLAGS="$LDFLAGS"
    LDFLAGS="$LDFLAGS -Wl,--version-script=conftest.map"
    cat > conftest.map <<EOF
VERS_1 {
    global: sym;
};

VERS_2 {
    global: sym;
} VERS_1;
EOF
    AC_LINK_IFELSE([AC_LANG_SOURCE([int main() { return 0; }])],
                   [have_ld_version_script=yes], [have_ld_version_script=no])
    rm -f conftest.map
    LDFLAGS="$save_LDFLAGS"
    AC_MSG_RESULT($have_ld_version_script)
fi
AM_CONDITIONAL(HAVE_LD_VERSION_SCRIPT, test "$have_ld_version_script" = "yes")

################################
# Deal with debugging options
################################
AC_ARG_ENABLE(debug,
              [AS_HELP_STRING([--enable-debug],
                              [Turn on debugging including asserts [default=no]])],
              [],
              [enable_debug=no])
AM_CONDITIONAL(DEBUG,test x$enable_debug = xyes)

################################
# Check for Lua support
################################
AC_ARG_WITH(lua,
            [AS_HELP_STRING([--with-lua=DIR],
                            [Build Lua ACTION plugin [default=no]])],
            [use_lua=$withval],
            [use_lua=no])
AM_CONDITIONAL(WITH_LUA,test x$use_lua != xno)

if test x$use_lua != xno; then
    if test x$use_lua != xyes; then
        CPPFLAGS="${CPPFLAGS} -I$use_lua/include"
	LDFLAGS="${LDFLAGS} -L$use_lua/lib"
    fi
    LUA_VERSION="5.1"
    AX_LUA_HEADERS()
    AX_LUA_LIBS()
fi

################################
# select the default xml backend
################################
AC_DEFINE([HAVE_LIB_EXPAT],
          [],
          [libexpat-based XML backend])

################################
# Common XML related subst
################################

AC_SUBST(XML_LIBS)
AC_SUBST(XML_LDFLAGS)
AC_SUBST(XML_CFLAGS)

################################
# Search for network functions (like connect())
################################
AC_SEARCH_LIBS([socket], [socket])

################################
# Check for regex.h
################################
AC_CHECK_HEADERS(regex.h, [],
    AC_MSG_ERROR([regex.h not found: regular expressions are not supported]))

################################
# Internal getopt()
################################
AC_ARG_WITH(internal-getopt,
            [AS_HELP_STRING([--with-internal-getopt],
            [Use internal implementation of getopt [default=no]])],
            [],
            [with_internal_getopt=no])

if test x$with_internal_getopt != xno; then
    AC_DEFINE([WITH_INTERNAL_GETOPT], [1], [Use internal getopt() implementation])
    AC_LIBOBJ([getopt])
    AC_MSG_WARN([Use internal implementation of getopt() and getopt_long()])
else
    AC_CHECK_HEADERS(getopt.h, [found_getopt_h=yes],
        AC_MSG_WARN([getopt.h not found: only short parameters can be used on command line]))
fi

AC_MSG_CHECKING([for getopt_long()])
if test x$with_internal_getopt = xyes -o x$found_getopt_h = xyes; then
    AC_DEFINE([HAVE_GETOPT_LONG], [1], [getopt_long() function])
    AC_MSG_RESULT([yes])
else
    AC_MSG_RESULT([no])
fi

################################
# Check for locale.h
################################
AC_CHECK_HEADERS(locale.h, [],
    AC_MSG_WARN([locale.h not found: the locales is not supported]))

################################
# Check for CODESET within nl_langinfo
################################
AC_DEFUN([AM_LANGINFO_CODESET],
[
  AC_CACHE_CHECK([for nl_langinfo and CODESET], [am_cv_langinfo_codeset],
    [AC_TRY_LINK([#include <langinfo.h>],
      [char* cs = nl_langinfo(CODESET); return !cs;],
      [am_cv_langinfo_codeset=yes],
      [am_cv_langinfo_codeset=no])
    ])
  if test $am_cv_langinfo_codeset = yes; then
    AC_DEFINE([HAVE_LANGINFO_CODESET], [1],
      [Define if you have <langinfo.h> and nl_langinfo(CODESET).])
  fi
])
AM_LANGINFO_CODESET

################################
# Check for pwd.h and grp.h
################################
AC_CHECK_HEADERS(pwd.h, [],
    AC_MSG_WARN([pwd.h not found: the pwd operations is not supported]))
AC_CHECK_HEADERS(grp.h, [],
    AC_MSG_WARN([grp.h not found: the grp operations is not supported]))

################################
# Check for chroot
################################
AC_CHECK_FUNCS(chroot, [],
    AC_MSG_WARN([chroot() not found: the choot is not supported]))

################################
# Check for dlopen
################################
CLISH_PLUGIN_BUILTIN_LIST=
CLISH_PLUGIN_BUILTIN_DEFS=
CLISH_PLUGIN_BUILTIN_LIBS=

AC_DEFUN([AC_PLUGIN_BUILTIN],
[
  CLISH_PLUGIN_BUILTIN_LIBS="$CLISH_PLUGIN_BUILTIN_LIBS bfshell_plugin_$1.la"
  CLISH_PLUGIN_BUILTIN_DEFS="$CLISH_PLUGIN_BUILTIN_DEFS CLISH_PLUGIN_INIT($1);"
  CLISH_PLUGIN_BUILTIN_LIST="$CLISH_PLUGIN_BUILTIN_LIST { \"$1\", bfshell_plugin_$1_init },"
])

AC_CHECK_HEADERS(dlfcn.h, [
        AC_SEARCH_LIBS([dlopen], [dl dld], [], [
          AC_MSG_ERROR([unable to find the dlopen() function])
        ])
    ],[
          AC_MSG_WARN([dlfcn.h not found: the dl operations is not supported])
          AC_PLUGIN_BUILTIN([clish])
          if test x$use_lua != xno; then
            AC_PLUGIN_BUILTIN([lua])
          fi
    ])

AC_SUBST([CLISH_PLUGIN_BUILTIN_LIST])
AC_SUBST([CLISH_PLUGIN_BUILTIN_DEFS])
AC_SUBST([CLISH_PLUGIN_BUILTIN_LIBS])

AC_ARG_WITH([asan],
            [AS_HELP_STRING([--with-asan],
                            [Build with address sanitizer checking])],
            [with_asan=${withval}],
            [with_asan=no])
AS_IF([test "x$with_asan" = "xno"],
      [ASAN_FLAGS=],
      [test "x$with_asan" = "xyes"],
      [ASAN_FLAGS="-fsanitize=address -fsanitize-recover=address"],
      [ASAN_FLAGS="$with_asan"])

AC_SUBST([AM_CFLAGS], ["$ASAN_FLAGS"])
AC_SUBST([AM_CPPFLAGS], ["-I. \
                          -I\$(top_srcdir) \
                          -I\$(top_srcdir)/../expat \
                          -I$lt_sysroot/\$(prefix)/include"])
AC_CONFIG_FILES([clish/plugin_builtin.c])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
