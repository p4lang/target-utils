AM_CPPFLAGS += -I$(srcdir)/../JudyCommon/ -I$(srcdir) -I$(srcdir)/..
AM_CFLAGS += -DJUDYL

noinst_LTLIBRARIES = libJudyL.la libnext.la libprev.la libcount.la libinline.la

libJudyL_la_SOURCES = JudyLCascade.c JudyLTables.c JudyLCount.c JudyLCreateBranch.c JudyLDecascade.c JudyLDel.c JudyLFirst.c JudyLFreeArray.c JudyLGet.c JudyLInsArray.c JudyLIns.c JudyLInsertBranch.c JudyLMallocIF.c JudyLMemActive.c JudyLMemUsed.c 
libJudyL_la_CFLAGS = $(AM_CFLAGS) -I$(srcdir)/../JudyCommon/

libnext_la_SOURCES = JudyLNext.c JudyLNextEmpty.c
libnext_la_CFLAGS = $(libJudyL_la_CFLAGS) -DJUDYNEXT

libprev_la_SOURCES = JudyLPrev.c JudyLPrevEmpty.c
libprev_la_CFLAGS = $(libJudyL_la_CFLAGS) -DJUDYPREV

libcount_la_SOURCES = JudyLByCount.c
libcount_la_CFLAGS = $(libJudyL_la_CFLAGS) -DNOSMARTJBB -DNOSMARTJBU -DNOSMARTJLB

libinline_la_SOURCES = j__udyLGet.c
libinline_la_CFLAGS = $(libJudyL_la_CFLAGS) -DJUDYGETINLINE

JudyLByCount.c:   ../JudyCommon/JudyByCount.c
	cp -f $(srcdir)/../JudyCommon/JudyByCount.c      		JudyLByCount.c   

JudyLCascade.c:../JudyCommon/JudyCascade.c
	cp -f $(srcdir)/../JudyCommon/JudyCascade.c       	JudyLCascade.c

JudyLCount.c:../JudyCommon/JudyCount.c
	cp -f $(srcdir)/../JudyCommon/JudyCount.c        		JudyLCount.c

JudyLCreateBranch.c:../JudyCommon/JudyCreateBranch.c 
	cp -f $(srcdir)/../JudyCommon/JudyCreateBranch.c JudyLCreateBranch.c

JudyLDecascade.c:../JudyCommon/JudyDecascade.c    
	cp -f $(srcdir)/../JudyCommon/JudyDecascade.c    JudyLDecascade.c

JudyLDel.c:../JudyCommon/JudyDel.c       
	cp -f $(srcdir)/../JudyCommon/JudyDel.c          		JudyLDel.c

JudyLFirst.c:../JudyCommon/JudyFirst.c
	cp -f $(srcdir)/../JudyCommon/JudyFirst.c        		JudyLFirst.c

JudyLFreeArray.c:../JudyCommon/JudyFreeArray.c    
	cp -f $(srcdir)/../JudyCommon/JudyFreeArray.c    		JudyLFreeArray.c

JudyLGet.c:../JudyCommon/JudyGet.c       
	cp -f $(srcdir)/../JudyCommon/JudyGet.c          		JudyLGet.c

j__udyLGet.c:../JudyCommon/JudyGet.c 
	cp -f $(srcdir)/../JudyCommon/JudyGet.c          		j__udyLGet.c

JudyLInsArray.c:../JudyCommon/JudyInsArray.c     
	cp -f $(srcdir)/../JudyCommon/JudyInsArray.c     		JudyLInsArray.c

JudyLIns.c:../JudyCommon/JudyIns.c
	cp -f $(srcdir)/../JudyCommon/JudyIns.c          JudyLIns.c

JudyLInsertBranch.c:../JudyCommon/JudyInsertBranch.c
	cp -f $(srcdir)/../JudyCommon/JudyInsertBranch.c JudyLInsertBranch.c

JudyLMallocIF.c:../JudyCommon/JudyMallocIF.c
	cp -f $(srcdir)/../JudyCommon/JudyMallocIF.c   	 JudyLMallocIF.c

JudyLMemActive.c:../JudyCommon/JudyMemActive.c
	cp -f $(srcdir)/../JudyCommon/JudyMemActive.c    		JudyLMemActive.c

JudyLMemUsed.c:../JudyCommon/JudyMemUsed.c   
	cp -f $(srcdir)/../JudyCommon/JudyMemUsed.c      		JudyLMemUsed.c

JudyLNext.c:../JudyCommon/JudyPrevNext.c
	cp -f $(srcdir)/../JudyCommon/JudyPrevNext.c     		JudyLNext.c

JudyLPrev.c:../JudyCommon/JudyPrevNext.c
	cp -f $(srcdir)/../JudyCommon/JudyPrevNext.c     	JudyLPrev.c

JudyLNextEmpty.c:../JudyCommon/JudyPrevNextEmpty.c
	cp -f $(srcdir)/../JudyCommon/JudyPrevNextEmpty.c	JudyLNextEmpty.c

JudyLPrevEmpty.c:../JudyCommon/JudyPrevNextEmpty.c
	cp -f $(srcdir)/../JudyCommon/JudyPrevNextEmpty.c	JudyLPrevEmpty.c

DISTCLEANFILES = .deps Makefile 

CLEANFILES = JudyLByCount.c  \
	     JudyLCascade.c \
	     JudyLCount.c \
	     JudyLCreateBranch.c \
	     JudyLDecascade.c \
	     JudyLDel.c \
	     JudyLFirst.c \
	     JudyLFreeArray.c \
	     JudyLGet.c \
	     j__udyLGet.c \
	     JudyLInsArray.c \
	     JudyLIns.c \
	     JudyLInsertBranch.c \
	     JudyLMallocIF.c \
	     JudyLMemActive.c \
	     JudyLMemUsed.c \
	     JudyLNext.c \
	     JudyLPrev.c \
	     JudyLNextEmpty.c \
	     JudyLPrevEmpty.c \
	     .libs \
	     *.o \
	     *.lo \
	     *.la 
