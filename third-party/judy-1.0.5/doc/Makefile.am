man3_MANS = man/man3/Judy \
	    man/man3/Judy1 \
	    man/man3/Judy1_funcs \
	    man/man3/JudyL_funcs \
	    man/man3/JudySL \
	    man/man3/JudySL_funcs \
	    man/man3/JudyHS \
	    man/man3/JudyHS_funcs \
	    man/man3/J1T \
	    man/man3/J1S \
	    man/man3/J1U \
	    man/man3/J1F \
	    man/man3/J1N \
	    man/man3/J1L \
	    man/man3/J1P \
	    man/man3/J1FE \
	    man/man3/J1NE \
	    man/man3/J1LE \
	    man/man3/J1PE \
	    man/man3/J1C \
	    man/man3/J1BC \
	    man/man3/J1FA \
	    man/man3/J1MU \
	    man/man3/Judy1Test \
	    man/man3/Judy1Set \
	    man/man3/Judy1Unset \
	    man/man3/Judy1First \
	    man/man3/Judy1Next \
	    man/man3/Judy1Last \
	    man/man3/Judy1Prev \
	    man/man3/Judy1FirstEmpty \
	    man/man3/Judy1NextEmpty \
	    man/man3/Judy1LastEmpty \
	    man/man3/Judy1PrevEmpty \
	    man/man3/Judy1Count \
	    man/man3/Judy1ByCount \
	    man/man3/Judy1FreeArray \
	    man/man3/Judy1MemUsed \
	    man/man3/JudyL \
	    man/man3/JLG \
	    man/man3/JLI \
	    man/man3/JLD \
	    man/man3/JLF \
	    man/man3/JLN \
	    man/man3/JLL \
	    man/man3/JLP \
	    man/man3/JLFE \
	    man/man3/JLNE \
	    man/man3/JLLE \
	    man/man3/JLPE \
	    man/man3/JLC \
	    man/man3/JLBC \
	    man/man3/JLFA \
	    man/man3/JLMU \
	    man/man3/JudyLGet \
	    man/man3/JudyLIns \
	    man/man3/JudyLDel \
	    man/man3/JudyLFirst \
	    man/man3/JudyLNext \
	    man/man3/JudyLLast \
	    man/man3/JudyLPrev \
	    man/man3/JudyLFirstEmpty \
	    man/man3/JudyLNextEmpty \
	    man/man3/JudyLLastEmpty \
	    man/man3/JudyLPrevEmpty \
	    man/man3/JudyLCount \
	    man/man3/JudyLByCount \
	    man/man3/JudyLFreeArray \
	    man/man3/JudyLMemUsed \
	    man/man3/JSLG \
	    man/man3/JSLI \
	    man/man3/JSLD \
	    man/man3/JSLF \
	    man/man3/JSLN \
	    man/man3/JSLL \
	    man/man3/JSLP \
	    man/man3/JSLFA \
	    man/man3/JudySLGet \
	    man/man3/JudySLIns \
	    man/man3/JudySLDel \
	    man/man3/JudySLFirst \
	    man/man3/JudySLNext \
	    man/man3/JudySLLast \
	    man/man3/JudySLPrev \
	    man/man3/JudySLFreeArray \
	    man/man3/JHSG \
	    man/man3/JHSI \
	    man/man3/JHSD \
	    man/man3/JHSFA \
	    man/man3/JudyHSGet \
	    man/man3/JudyHSIns \
	    man/man3/JudyHSDel \
	    man/man3/JudyHSFreeArray



man/man3/Judy:
	mkdir -p $(builddir)/man/man3
	$(builddir)/../tool/jhton $(srcdir)/ext/Judy_3.htm | grep -v '^[   ]*$$' | sed -e 's/\.C//' >  man/man3/Judy

@builddir@/man/man3/Judy1:
	$(builddir)/../tool/jhton $(srcdir)/ext/Judy1_3.htm | grep -v '^[   ]*$$' | sed -e 's/\.C//' > @builddir@/man/man3/Judy1
@builddir@/man/man3/J1T  \
@builddir@/man/man3/J1S  \
@builddir@/man/man3/J1U  \
@builddir@/man/man3/J1F  \
@builddir@/man/man3/J1N  \
@builddir@/man/man3/J1L  \
@builddir@/man/man3/J1P  \
@builddir@/man/man3/J1FE \
@builddir@/man/man3/J1NE \
@builddir@/man/man3/J1LE \
@builddir@/man/man3/J1PE \
@builddir@/man/man3/J1C  \
@builddir@/man/man3/J1BC \
@builddir@/man/man3/J1FA \
@builddir@/man/man3/J1MU : @builddir@/man/man3/Judy1
	ln -s -f $$( readlink -f $< ) $@

@builddir@/man/man3/Judy1_funcs:
	$(builddir)/../tool/jhton $(srcdir)/ext/Judy1_funcs_3.htm | grep -v '^[   ]*$$' | sed -e 's/\.C//' > @builddir@/man/man3/Judy1_funcs
@builddir@/man/man3/Judy1Test       \
@builddir@/man/man3/Judy1Set        \
@builddir@/man/man3/Judy1Unset      \
@builddir@/man/man3/Judy1First      \
@builddir@/man/man3/Judy1Next       \
@builddir@/man/man3/Judy1Last       \
@builddir@/man/man3/Judy1Prev       \
@builddir@/man/man3/Judy1FirstEmpty \
@builddir@/man/man3/Judy1NextEmpty  \
@builddir@/man/man3/Judy1LastEmpty  \
@builddir@/man/man3/Judy1PrevEmpty  \
@builddir@/man/man3/Judy1Count      \
@builddir@/man/man3/Judy1ByCount    \
@builddir@/man/man3/Judy1FreeArray  \
@builddir@/man/man3/Judy1MemUsed : @builddir@/man/man3/Judy1_funcs
	ln -s -f $$( readlink -f $< ) $@

@builddir@/man/man3/JudyL:
	$(builddir)/../tool/jhton $(srcdir)/ext/JudyL_3.htm | grep -v '^[   ]*$$' | sed -e 's/\.C//' > @builddir@/man/man3/JudyL
@builddir@/man/man3/JLG  \
@builddir@/man/man3/JLI  \
@builddir@/man/man3/JLD  \
@builddir@/man/man3/JLF  \
@builddir@/man/man3/JLN  \
@builddir@/man/man3/JLL  \
@builddir@/man/man3/JLP  \
@builddir@/man/man3/JLFE \
@builddir@/man/man3/JLNE \
@builddir@/man/man3/JLLE \
@builddir@/man/man3/JLPE \
@builddir@/man/man3/JLC  \
@builddir@/man/man3/JLBC \
@builddir@/man/man3/JLFA \
@builddir@/man/man3/JLMU: @builddir@/man/man3/JudyL
	ln -s -f $$( readlink -f $< ) $@

@builddir@/man/man3/JudyL_funcs:
	$(builddir)/../tool/jhton $(srcdir)/ext/JudyL_funcs_3.htm | grep -v '^[   ]*$$' | sed -e 's/\.C//' > @builddir@/man/man3/JudyL_funcs
@builddir@/man/man3/JudyLGet        \
@builddir@/man/man3/JudyLIns        \
@builddir@/man/man3/JudyLDel        \
@builddir@/man/man3/JudyLFirst      \
@builddir@/man/man3/JudyLNext       \
@builddir@/man/man3/JudyLLast       \
@builddir@/man/man3/JudyLPrev       \
@builddir@/man/man3/JudyLFirstEmpty \
@builddir@/man/man3/JudyLNextEmpty  \
@builddir@/man/man3/JudyLLastEmpty  \
@builddir@/man/man3/JudyLPrevEmpty  \
@builddir@/man/man3/JudyLCount      \
@builddir@/man/man3/JudyLByCount    \
@builddir@/man/man3/JudyLFreeArray  \
@builddir@/man/man3/JudyLMemUsed : @builddir@/man/man3/JudyL_funcs
	ln -s -f $$( readlink -f $< ) $@

@builddir@/man/man3/JudySL:
	$(builddir)/../tool/jhton $(srcdir)/ext/JudySL_3.htm | grep -v '^[   ]*$$' | sed -e 's/\.C//' > @builddir@/man/man3/JudySL
@builddir@/man/man3/JSLG  \
@builddir@/man/man3/JSLI  \
@builddir@/man/man3/JSLD  \
@builddir@/man/man3/JSLF  \
@builddir@/man/man3/JSLN  \
@builddir@/man/man3/JSLL  \
@builddir@/man/man3/JSLP  \
@builddir@/man/man3/JSLFA : @builddir@/man/man3/JudySL
	ln -s -f $$( readlink -f $< ) $@

@builddir@/man/man3/JudySL_funcs:
	$(builddir)/../tool/jhton $(srcdir)/ext/JudySL_funcs_3.htm | grep -v '^[   ]*$$' | sed -e 's/\.C//' > @builddir@/man/man3/JudySL_funcs
@builddir@/man/man3/JudySLGet   \
@builddir@/man/man3/JudySLIns   \
@builddir@/man/man3/JudySLDel   \
@builddir@/man/man3/JudySLFirst \
@builddir@/man/man3/JudySLNext  \
@builddir@/man/man3/JudySLLast  \
@builddir@/man/man3/JudySLPrev  \
@builddir@/man/man3/JudySLFreeArray: @builddir@/man/man3/JudySL_funcs
	ln -s -f $$( readlink -f $< ) $@

@builddir@/man/man3/JudyHS:
	$(builddir)/../tool/jhton $(srcdir)/ext/JudyHS_3.htm | grep -v '^[   ]*$$' | sed -e 's/\.C//' > @builddir@/man/man3/JudyHS
@builddir@/man/man3/JHSG \
@builddir@/man/man3/JHSI \
@builddir@/man/man3/JHSD \
@builddir@/man/man3/JHSFA : @builddir@/man/man3/JudyHS
	ln -s -f $$( readlink -f $< ) $@

@builddir@/man/man3/JudyHS_funcs:
	$(builddir)/../tool/jhton $(srcdir)/ext/JudyHS_funcs_3.htm | grep -v '^[   ]*$$' | sed -e 's/\.C//' > @builddir@/man/man3/JudyHS_funcs
@builddir@/man/man3/JudyHSGet \
@builddir@/man/man3/JudyHSIns \
@builddir@/man/man3/JudyHSDel \
@builddir@/man/man3/JudyHSFreeArray : @builddir@/man/man3/JudyHS_funcs
	ln -s -f $$( readlink -f $< ) $@

CLEANFILES = man/man3/*
.NOTPARALLEL:
