# "dox/lang/Makefile.am"

include $(top_srcdir)/Make.stddef

DVIPS = dvips
PSNUP = psnup
LATEX = latex
PDFLATEX = pdflatex
BIBTEX = bibtex
MAKEINDEX = makeindex
MAKETEXDEPEND = $(AWK) -f $(srcdir)/maketexdepend.awk \
	-v readable=1 -v extensions="pdf dvi depend" -v touch=0

SUFFIXES = .ps .dvi .tex .pdf .depend

PDFS = artxx.pdf
PSS = $(PDFS:.pdf=.ps)

# all-local: pdf-local ps-local

pdf-local: $(PDFS)

ps-local: $(PSS)

# have to add these by hand? :S
EXTRA_DIST = maketexdepend.awk maketexdepend.tex \
	artxx.bib artxx.tex \
	chapters/intro.tex \
	chapters/types.tex \
	chapters/expressions.tex \
	chapters/arrays.tex \
	chapters/processes.tex \
	chapters/channels.tex \
	chapters/datatypes.tex \
	chapters/namespaces.tex \
	chapters/templates.tex \
	chapters/typedefs.tex \
	chapters/connections.tex \
	chapters/linkage.tex

# automatically generated dependencies in this (built) dir
-include artxx.depend

.tex.depend:
	@$(ECHO) "Generating auto-dependencies $@..."
	@if [ -f $@ ] ; then \
		$(MAKETEXDEPEND) $< > $@.new ; \
		if $(DIFF) -q $@ $@.new ; then \
			mv -f $@.new $@ ; \
		else 	$(RM) $@.new ; \
		fi > /dev/null ; \
	else	$(MAKETEXDEPEND) $< > $@ ; \
	fi


.tex.dvi:
	@$(ECHO) "Generating $@..."
	@$(LATEX) $< > /dev/null < /dev/null
	@if $(GREP) "TeX capacity" $*.log; \
		then $(ECHO) SHIT ; \
	fi
	@if $(GREP) -q "\\citation" $*.aux ; then $(BIBTEX) $* ; fi
	@if [ -f $*.idx ] ; then $(MAKEINDEX) $*.idx ; fi 
	@if ($(GREP) 'LaTeX Warning:' $*.log | $(GREP) -v floats); then \
		$(ECHO); $(ECHO) "Detected warnings, doing second LaTeX pass..." ; \
		$(LATEX) $< > /dev/null < /dev/null; \
		if [ -f $*.idx ] ; then $(MAKEINDEX) $*.idx ; fi ; \
		if $(GREP) "TeX capacity" $*.log; \
			then $(ECHO) SHIT; \
		elif $(GREP) 'LaTeX Warning: Citation' $*.log; then \
			$(ECHO); $(ECHO) "Citation warnings, third LaTeX pass..."; \
			$(LATEX) $< > /dev/null < /dev/null; \
		fi; \
		($(GREP) 'LaTeX Warn' $*.log || ($(ECHO) > /dev/null)); \
	fi
	@$(ECHO) "$@ completed."

.tex.pdf:
	@$(ECHO) "Generating $@..."
	@$(PDFLATEX) $< > /dev/null < /dev/null
	@if $(GREP) "TeX capacity" $*.log; \
		then $(ECHO) SHIT ; \
	fi
	@if $(GREP) -q "\\citation" $*.aux ; then $(BIBTEX) $* ; fi
	@if [ -f $*.idx ] ; then $(MAKEINDEX) $*.idx ; fi 
	@if ($(GREP) 'LaTeX Warning:' $*.log | $(GREP) -v floats); then \
		$(ECHO); $(ECHO) "Detected warnings, doing second LaTeX pass..." ; \
		$(PDFLATEX) $< > /dev/null < /dev/null; \
		if [ -f $*.idx ] ; then $(MAKEINDEX) $*.idx ; fi ; \
		if $(GREP) "TeX capacity" $*.log; \
			then $(ECHO) SHIT; \
		elif $(GREP) 'LaTeX Warning: Citation' $*.log; then \
			$(ECHO); $(ECHO) "Citation warnings, third LaTeX pass..."; \
			$(PDFLATEX) $< > /dev/null < /dev/null; \
		fi; \
		($(GREP) 'LaTeX Warn' $*.log || ($(ECHO) > /dev/null)); \
	fi
	@$(ECHO) "$@ completed."

.dvi.ps:
	$(DVIPS) $< -o $@

clean-bib:
	-$(RM) *.bbl *.blg

clean-index:
	-$(RM) *.idx *.ind *.ilg *.out

clean-local: clean-bib clean-index
	-$(RM) *.aux *.log *.toc *.depend
	-$(RM) *.ps *.dvi *.pdf

