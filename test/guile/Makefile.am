# "test/chpsim/Makefile.am"
#	$Id: Makefile.am,v 1.3 2009/02/23 09:11:38 fang Exp $
# Test auto-Makefile for chpsim.  

include $(top_srcdir)/test/Make.test-head
include $(top_srcdir)/installdirs.mk

SUBDIRS =

HACO_FLAGS +=

HACKT_PARSE_TEST_PASSES = empty

# in this directory, no parse-fail tests
HACKT_CREATE_TEST_PASSES = $(HACKT_PARSE_TEST_PASSES)

HACKT_ALLOC_TEST_PASSES = $(HACKT_CREATE_TEST_PASSES)

# temporary for now:
HACKT_CHPSIM_ALLOC_TEST_SUBJECTS = $(HACKT_ALLOC_TEST_PASSES)

# TODO: eventually generate object test cases for all of these:
HACKT_CHPSIMRC_TEST_SUBJECTS =

# consider making .chpsim-event-dot/ps for visualizations

HACKT_CHPSIMRC_TEST_SCRIPTS_PASSES =

HACKT_CHPSIMRC_TEST_SCRIPTS_FAILURES =

# not yet, three tests report checkpoint diffs due to queue-reorderings
HACKT_CHPSIM_CKPT_TEST_SCRIPTS_BASES = $(HACKT_CHPSIMRC_TEST_SCRIPTS_PASSES)

HACKT_GUILE_TEST_SCRIPTS_PASSES = \
	rb-tree-01 rb-tree-02 rb-tree-03

HACKT_GUILE_TEST_SCRIPTS_FAILURES =

# additional sources not already mentioned
# srcdir_EXTRA_DIST +=

if HAVE_LIBGUILE
if INSTALL_TESTS
# check for basic usability of installed guile modules
installcheck-local: installcheck-hacguile installcheck-guile

# test 1: make sure guile works with load paths set, no output checking
installcheck-guile:
	PATH=$(INSTALL_PATH_OVERRIDE) && \
	export PATH && \
	. `which init_hackt.sh` && \
	$(mkdir_p) installtest && \
	pushd installtest && \
	$(ECHO) "(display \"Hello, guile!\") (newline)" | \
	$(GUILE) -l $(scmdir)/hacguile.scm -- -b && \
	popd

# test 2: compare a built test to an equivalent installed test
HACGUILE_INSTALLCHECK_TEST_FILE = rb-tree-03.hacscm
HACGUILE_INSTALLCHECK_COPY_FILES = \
	$(srcdir)/$(HACGUILE_INSTALLCHECK_TEST_FILE) \
	$(HACGUILE_INSTALLCHECK_TEST_FILE)-out \
	empty.haco-a

installcheck-hacguile: $(HACGUILE_INSTALLCHECK_COPY_FILES)
	PATH=$(INSTALL_PATH_OVERRIDE) && \
	export PATH && \
	$(mkdir_p) installtest && \
	cp $(HACGUILE_INSTALLCHECK_COPY_FILES) installtest && \
	pushd installtest && \
	$(INSTALLED_HACGUILE) -b empty.haco-a \
		< $(HACGUILE_INSTALLCHECK_TEST_FILE) \
		> $(HACGUILE_INSTALLCHECK_TEST_FILE)-out-inst && \
	$(DIFF) -u $(HACGUILE_INSTALLCHECK_TEST_FILE)-out \
		$(HACGUILE_INSTALLCHECK_TEST_FILE)-out-inst && \
	$(ECHO) "Installed test for $(HACGUILE_INSTALLCHECK_TEST_FILE) produces consistent results." && \
	popd
DISTCLEANDIRS += installtest
endif
endif

include $(top_srcdir)/test/Make.test-tail

