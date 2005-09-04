# "scripts/Makefile.am"
#	$Id: Makefile.am,v 1.1 2005/09/04 22:13:48 fang Exp $

include $(top_srcdir)/Make.stddef

# empty
local.cvsignore:
	$(TOUCH) $@

CLEANFILES +=
DISTCLEANFILES += local.cvsignore
AUTO_IGNORE += $(DISTCLEANFILES)
EXTRA_DIST += configure-gcc4.sh \
	readable_gcc_errors.sed \
	balance_indent.awk \
	tally_tests.awk


include $(top_srcdir)/Make.global

