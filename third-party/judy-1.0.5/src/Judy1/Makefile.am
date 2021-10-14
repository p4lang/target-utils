AM_CPPFLAGS +=  -I.. -I$(srcdir)/../JudyCommon/ -I$(srcdir) -I$(srcdir)/..
AM_CFLAGS += -DJUDY1

noinst_LTLIBRARIES = libJudy1.la libnext.la libprev.la libcount.la libinline.la

libJudy1_la_SOURCES = Judy1Test.c Judy1Tables.c Judy1Set.c Judy1SetArray.c Judy1Unset.c Judy1Cascade.c Judy1Count.c Judy1CreateBranch.c Judy1Decascade.c Judy1First.c Judy1FreeArray.c Judy1InsertBranch.c Judy1MallocIF.c Judy1MemActive.c Judy1MemUsed.c 
libJudy1_la_CFLAGS = $(AM_CFLAGS) -w -I$(srcdir)/../JudyCommon/

libnext_la_SOURCES = Judy1Next.c Judy1NextEmpty.c 
libnext_la_CFLAGS = $(AM_CFLAGS) -DJUDYNEXT

libprev_la_SOURCES = Judy1Prev.c Judy1PrevEmpty.c 
libprev_la_CFLAGS = $(AM_CFLAGS) -DJUDYPREV

libcount_la_SOURCES = Judy1ByCount.c
libcount_la_CFLAGS = $(AM_CFLAGS) -DNOSMARTJBB -DNOSMARTJBU -DNOSMARTJLB

libinline_la_SOURCES = j__udy1Test.c
libinline_la_CFLAGS = $(AM_CFLAGS) -DJUDYGETINLINE

Judy1ByCount.c:../JudyCommon/JudyByCount.c
	cp -f $(srcdir)/../JudyCommon/JudyByCount.c      		Judy1ByCount.c   

Judy1Cascade.c:../JudyCommon/JudyCascade.c
	cp -f $(srcdir)/../JudyCommon/JudyCascade.c      		Judy1Cascade.c

Judy1Count.c:../JudyCommon/JudyCount.c
	cp -f $(srcdir)/../JudyCommon/JudyCount.c        		Judy1Count.c

Judy1CreateBranch.c:../JudyCommon/JudyCreateBranch.c
	cp -f $(srcdir)/../JudyCommon/JudyCreateBranch.c 		Judy1CreateBranch.c

Judy1Decascade.c:../JudyCommon/JudyDecascade.c
	cp -f $(srcdir)/../JudyCommon/JudyDecascade.c    		Judy1Decascade.c

Judy1Unset.c:../JudyCommon/JudyDel.c
	cp -f $(srcdir)/../JudyCommon/JudyDel.c          		Judy1Unset.c

Judy1First.c:../JudyCommon/JudyFirst.c
	cp -f $(srcdir)/../JudyCommon/JudyFirst.c        		Judy1First.c

Judy1FreeArray.c:../JudyCommon/JudyFreeArray.c
	cp -f $(srcdir)/../JudyCommon/JudyFreeArray.c    		Judy1FreeArray.c

Judy1Test.c:../JudyCommon/JudyGet.c
	cp -f $(srcdir)/../JudyCommon/JudyGet.c          		Judy1Test.c

j__udy1Test.c:../JudyCommon/JudyGet.c
	cp -f $(srcdir)/../JudyCommon/JudyGet.c          		j__udy1Test.c

Judy1SetArray.c:../JudyCommon/JudyInsArray.c
	cp -f $(srcdir)/../JudyCommon/JudyInsArray.c     		Judy1SetArray.c

Judy1Set.c:../JudyCommon/JudyIns.c
	cp -f $(srcdir)/../JudyCommon/JudyIns.c          		Judy1Set.c

Judy1InsertBranch.c:../JudyCommon/JudyInsertBranch.c
	cp -f $(srcdir)/../JudyCommon/JudyInsertBranch.c 		Judy1InsertBranch.c

Judy1MallocIF.c:../JudyCommon/JudyMallocIF.c
	cp -f $(srcdir)/../JudyCommon/JudyMallocIF.c     		Judy1MallocIF.c

Judy1MemActive.c:../JudyCommon/JudyMemActive.c
	cp -f $(srcdir)/../JudyCommon/JudyMemActive.c    		Judy1MemActive.c

Judy1MemUsed.c:../JudyCommon/JudyMemUsed.c
	cp -f $(srcdir)/../JudyCommon/JudyMemUsed.c      		Judy1MemUsed.c

Judy1Next.c:../JudyCommon/JudyPrevNext.c
	cp -f $(srcdir)/../JudyCommon/JudyPrevNext.c     		Judy1Next.c

Judy1Prev.c:../JudyCommon/JudyPrevNext.c
	cp -f $(srcdir)/../JudyCommon/JudyPrevNext.c     		Judy1Prev.c

Judy1NextEmpty.c:../JudyCommon/JudyPrevNextEmpty.c
	cp -f $(srcdir)/../JudyCommon/JudyPrevNextEmpty.c		Judy1NextEmpty.c

Judy1PrevEmpty.c:../JudyCommon/JudyPrevNextEmpty.c
	cp -f $(srcdir)/../JudyCommon/JudyPrevNextEmpty.c		Judy1PrevEmpty.c

DISTCLEANFILES = .deps Makefile 

CLEANFILES = Judy1ByCount.c \
	     Judy1Cascade.c \
	     Judy1Count.c \
	     Judy1CreateBranch.c \
	     Judy1Decascade.c \
	     Judy1Unset.c \
	     Judy1First.c \
	     Judy1FreeArray.c \
	     Judy1Test.c \
	     j__udy1Test.c \
	     Judy1SetArray.c \
	     Judy1Set.c \
	     Judy1InsertBranch.c \
	     Judy1MallocIF.c \
	     Judy1MemActive.c \
	     Judy1MemUsed.c \
	     Judy1Next.c \
	     Judy1Prev.c \
	     Judy1NextEmpty.c \
	     Judy1PrevEmpty.c \
	     .libs \
	     *.o \
	     *.lo \
	     *.la
