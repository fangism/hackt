# "dox/lang/Makefile.am"

include $(top_srcdir)/Make.stddef

DVIPS = dvips
PSNUP = psnup
LATEX = latex
PDFLATEX = pdflatex
BIBTEX = bibtex
MAKEINDEX = makeindex

SUFFIXES = .ps .dvi .tex .pdf

PDFS = artxx.pdf
PSS = $(PDFS:.pdf=.ps)

# all-local: pdf-local ps-local

pdf-local: $(PDFS)

ps-local: $(PSS)

EXTRA_DIST = artxx.bib artxx.tex chapters/intro.tex

.tex.dvi:
	@$(ECHO) "Generating $@..."
	@$(LATEX) $< > /dev/null < /dev/null
	@if $(GREP) "TeX capacity" $*.log; \
		then $(ECHO) SHIT ; \
	fi
	@$(BIBTEX) $*
	@$(MAKEINDEX) $*.idx
	@if ($(GREP) 'LaTeX Warning:' $*.log | $(GREP) -v floats); \
	then \
		$(ECHO); $(ECHO) "Detected warnings, doing second LaTeX pass..." ; \
		$(LATEX) $< > /dev/null < /dev/null; \
		$(MAKEINDEX) $*.idx; \
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
	@$(BIBTEX) $*
	@$(MAKEINDEX) $*.idx
	@if ($(GREP) 'LaTeX Warning:' $*.log | $(GREP) -v floats); \
	then \
		$(ECHO); $(ECHO) "Detected warnings, doing second LaTeX pass..." ; \
		$(PDFLATEX) $< > /dev/null < /dev/null; \
		$(MAKEINDEX) $*.idx; \
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

clean-local:
	-$(RM) *.aux *.log *.bbl *.blg *.idx *.ind *.ilg *.out *.toc
	-$(RM) *.ps *.dvi *.pdf

