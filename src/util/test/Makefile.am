# "src/util/test/Makefile.am"
#	$Id: Makefile.am,v 1.65 2010/04/02 22:19:25 fang Exp $
# NOTE: none of the tests in this directory should depend on ANY
# external package libraries or headers, so we coercively set
# LIBS and LDFLAGS to empty, which is normally bad policy for the
# real project being built.  We forgive this directory, for it is exceptional.
# We do, however, keep CXXFLAGS as specified by the user.  

include $(top_srcdir)/Make.stddef

AUTOMAKE_OPTIONS += subdir-objects

SUFFIXES = .test .ii .i .hii .hhii .tccii \
	.out .out-filter .stderr .stderr-filter .diff

CPREPROCESS = $(COMPILE) -E
CXXPREPROCESS = $(CXXCOMPILE) -E

# TEST_EXPECT_SH = test-expect.sh
ADDRESS_FILTER = $(SED) -f $(top_srcdir)/test/address_filter.sed

# all-local: $(TEST_EXPECT_SH)

# check_SCRIPTS = $(TEST_EXPECT_SH)

# auto-generated test scripts
# .cc.test:
#	@{ $(ECHO) "#!$(SHELL)" ; \
#	$(ECHO) "# \"$@\"" ; \
#	$(ECHO) ./$(TEST_EXPECT_SH) ./$* $(srcdir) ;} > $@ ; \
#	$(CHMOD) +x $@

DEFAULT_DIFF_TEST_SCRIPT = \
	$(ECHO) "Summarizing $@ ..." ; \
	{ $(ECHO) $(SHELL_INTERPRETER) ; \
	$(ECHO) "\# \"$@\"" ; \
	if test -s $< ; then \
	  $(ECHO) "$(ECHO) $< is non-empty!" ; \
	  $(ECHO) "exit 1" ; \
	fi ;} > $@ ; \
	$(CHMOD) +x $@

.diff.test:
	@$(DEFAULT_DIFF_TEST_SCRIPT)

.out.out-filter:
	@$(ADDRESS_FILTER) $< > $@

.stderr.stderr-filter:
	@$(ADDRESS_FILTER) $< > $@

TEST_DIFF = $(DIFF) -u

.stderr-filter.diff:
	@out=`$(ECHO) $@ | $(SED) 's/\.diff$$/.out-filter/g'` ; \
	expect=`$(ECHO) $@ | $(SED) 's/\.diff$$/.stderr-filter/g'` ; \
	morefilter=$(srcdir)/`$(ECHO) $@ | $(SED) 's/\.diff$$/.morefilter.sh/g'` ; \
	if test -f $$morefilter ; then \
	  $(MV) $$out $$out-orig ; \
	  $(MV) $$expect $$expect-orig ; \
	  $$morefilter $$out-orig $$out ; \
	  $$morefilter $$expect-orig $$expect ; \
	  $(TEST_DIFF) $$expect-orig $$out-orig > $@-orig ; \
	  if test -s $@-orig ; then \
	    $(ECHO) "$@-orig is actually non-empty.  (OK)" ; \
	  fi ; \
	fi
	-$(TEST_DIFF) $^ > $@
	@$(TOUCH) $@ ; \
	if test -s $@ ; then \
	  $(ECHO) "Differences found in $@!" ; \
	fi

AUTO_DEPENDS += diff.autodepend
diff.autodepend: Makefile
	@$(ECHO) "Generating dependencies $@" ; \
	list='$(noinst_PROGRAMS)' ; \
	test -z "$$list" || \
	for e in $$list ; do \
	  $(ECHO) "$$e.diff: $$e.out-filter" ; \
	  $(ECHO) "" ; \
	done > $@ ; \
	$(TOUCH) $@

-include diff.autodepend

# these are all test programs, not to be installed
noinst_PROGRAMS = \
	exit_test \
	pipe_fork_test \
	popen_test \
	throw_test \
	what_test \
	pretty_function_test \
	stacktrace_test \
	friend_function_formal_bug \
	link_test \
	bitmanip_test \
	string_test \
	priority_queue_test \
	pointer_classes_test \
	excl_ptr_variant_test \
	gxx-pr29164-operator-delete \
	count_ptr_test \
	count_ptr_variant_test \
	sublist_test \
	deque_iterator_test \
	hash_map_iterator_O2_bug_test \
	hash_qmap_test \
	free_list_test \
	discrete_interval_set_test \
	EBCO_test \
	integer_traits_test \
	new_functor_test \
	list_vector_test \
	list_vector_pool_test \
	chunk_map_pool_test \
	static_count_ptr_test \
	chunk_pool_ref_count_test \
	local_static_store_test \
	multi_pool_module_test \
	multidimensional_sparse_set_test \
	multidimensional_qmap_test \
	maplikeset_test \
	multikey_qmap_test \
	multikey_generator_test \
	multikey_qmap_slice_test \
	multikey_set_test \
	multikey_set_derived_test \
	packed_array_test \
	ring_node_test \
	union_find_test \
	sparse_collection_test \
	collection_pool_test

AUTO_DEPENDS += exec.autodepend
# rules for creating .out from executables
exec.autodepend: Makefile
	@$(ECHO) "Generating dependencies $@" ; \
	list='$(noinst_PROGRAMS)' ; \
	test -z "$$list" || \
	for e in $$list ; do \
	  $(ECHO) "$$e.out: $$e" ; \
	  $(ECHO) "	-./$$""< > $$""@ 2>&1 ; $(TOUCH) $$""@" ; \
	  $(ECHO) "" ; \
	done > $@ ; \
	$(TOUCH) $@

-include exec.autodepend

noinst_LTLIBRARIES = libfangutil.la libpooled.la \
	liblinktest.la libbogusutil.la

noinst_HEADERS = multikey_qmap_type_helper.h

# No external libraries! see comment at top of this Makefile.am
LIBS = 

# but is included in distribution, EXTRA_DIST? don't bother cleaning?
EMPTY_STDERR_FILES = friend_function_formal_bug.stderr \
	link_test.stderr excl_ptr_variant_test.stderr \
	count_ptr_variant_test.stderr \
	hash_map_iterator_O2_bug_test.stderr

AUTO_STDERR_FILES = multikey_set_derived_test.stderr

BUILT_SOURCES = stacktrace.cc static_trace.cc thread_lock.cc \
	ref_count_pool.cc bitset.cc nibble_tables.cc \
	$(EMPTY_STDERR_FILES) $(AUTO_STDERR_FILES)

pointer_classes_test_SOURCES = pointer_classes_test.cc var.h func.h
excl_ptr_variant_test_SOURCES = excl_ptr_variant_test.cc
count_ptr_variant_test_SOURCES = count_ptr_variant_test.cc
count_ptr_variant_test_LDADD = libfangutil.la
count_ptr_test_SOURCES = count_ptr_test.cc var.h
count_ptr_test_LDADD = libfangutil.la
gxx_pr29164_operator_delete_SOURCES = gxx-pr29164-operator-delete.cc \
	one_array_only.cc one_array_only.h
sublist_test_SOURCES = sublist_test.cc
deque_iterator_test_SOURCES = deque_iterator_test.cc
deque_iterator_test_LDADD = libfangutil.la
hash_map_iterator_O2_bug_test_SOURCES = hash_map_iterator_O2_bug_test.cc
hash_qmap_test_SOURCES = hash_qmap_test.cc
free_list_test_SOURCES = free_list_test.cc
free_list_test_LDADD = libfangutil.la
friend_function_formal_bug_SOURCES = friend_function_formal_bug.cc
priority_queue_test_SOURCES = priority_queue_test.cc
bitmanip_test_SOURCES = bitmanip_test.cc
bitmanip_test_LDADD = libfangutil.la
string_test_SOURCES = string_test.cc
string_test_LDADD = libfangutil.la
link_test_SOURCES = link_test.cc
link_test_LDADD = liblinktest.la libbogusutil.la
list_vector_test_SOURCES = list_vector_test.cc
list_vector_pool_test_SOURCES = list_vector_pool_test.cc
list_vector_pool_test_LDADD = libfangutil.la
chunk_map_pool_test_SOURCES = chunk_map_pool_test.cc
chunk_map_pool_test_LDADD = libfangutil.la
chunk_pool_ref_count_test_SOURCES = chunk_pool_ref_count_test.cc
chunk_pool_ref_count_test_LDADD = libfangutil.la
discrete_interval_set_test_SOURCES = discrete_interval_set_test.cc
EBCO_test_SOURCES = EBCO_test.cc
multidimensional_sparse_set_test_SOURCES = multidimensional_sparse_set_test.cc
multidimensional_sparse_set_test_LDADD = libfangutil.la
multidimensional_qmap_test_SOURCES = multidimensional_qmap_test.cc
maplikeset_test_SOURCES = maplikeset_test.cc
multikey_qmap_test_SOURCES = multikey_qmap_test.cc
multikey_generator_test_SOURCES = multikey_generator_test.cc
multikey_qmap_slice_test_SOURCES = multikey_qmap_slice_test.cc cube_slice.h plane_slice.h
multikey_set_test_SOURCES = multikey_set_test.cc
multikey_set_derived_test_SOURCES = multikey_set_derived_test.cc
integer_traits_test_SOURCES = integer_traits_test.cc
new_functor_test_SOURCES = new_functor_test.cc
packed_array_test_SOURCES = packed_array_test.cc
exit_test_SOURCES = exit_test.cc pooled_thing.cc pooled_thing.h
exit_test_LDADD = libfangutil.la
pipe_fork_test_SOURCES = pipe_fork_test.cc
# pipe_fork_test_LDADD = libfangutil.la
popen_test_SOURCES = popen_test.cc
# popen_test_LDADD = libfangutil.la
throw_test_SOURCES = throw_test.cc pooled_thing.cc pooled_thing.h
throw_test_LDADD = libfangutil.la
stacktrace_test_SOURCES = stacktrace_test.cc
stacktrace_test_LDADD = libfangutil.la
what_test_SOURCES = what_test.cc
pretty_function_test_SOURCES = pretty_function_test.cc
local_static_store_test_SOURCES = local_static_store_test.cc \
	named_pooled_thing.cc named_pooled_thing.h
local_static_store_test_LDADD = libfangutil.la
multi_pool_module_test_SOURCES = multi_pool_module_test.cc
multi_pool_module_test_LDADD = libpooled.la libfangutil.la
# note link ordering!
static_count_ptr_test_SOURCES = static_count_ptr_test.cc named_pooled_thing.cc
static_count_ptr_test_LDADD = libfangutil.la
ring_node_test_SOURCES = ring_node_test.cc
union_find_test_SOURCES = union_find_test.cc
sparse_collection_test_SOURCES = sparse_collection_test.cc
sparse_collection_test_LDADD = libfangutil.la
collection_pool_test_SOURCES = collection_pool_test.cc
collection_pool_test_LDADD = libfangutil.la

libfangutil_la_SOURCES = stacktrace.cc static_trace.cc thread_lock.cc \
	ref_count_pool.cc bitset.cc nibble_tables.cc string.cc
libbogusutil_la_SOURCES = bogus_stacktrace.cc bogus_stacktrace.h static_trace.cc
libpooled_la_SOURCES = \
	pool_module_a.cc pool_module_b.cc pool_module_a.h pool_module_b.h
liblinktest_la_SOURCES = \
	link_mod_a.cc link_mod_b.cc link_mod_a.h link_mod_b.h

# liblinktest_la_LDFLAGS = -multiply_defined

# for occasional compiler debugging
# SAVETEMP = $(CXXCOMPILE) -v -save-temps
.c.i:
	$(CPREPROCESS) $< > $@

.cc.ii:
	$(CXXPREPROCESS) $< > $@

.h.hii:
	$(CXXPREPROCESS) $< > $@

.hh.hhii:
	$(CXXPREPROCESS) $< > $@

.tcc.tccii:
	$(CXXPREPROCESS) $< > $@

# do not explicitly reference files in parent directory, because automake
# will clobber the .deps dependencies in the parent directory, 
# possibly confusing the parent directories when they expect .deps to
# exist, especially on make distclean.  
stacktrace.cc: ../stacktrace.cc
	$(LN_SF) $? $@

static_trace.cc: ../static_trace.cc
	$(LN_SF) $? $@

thread_lock.cc: ../memory/thread_lock.cc
	$(LN_SF) $? $@

ref_count_pool.cc: ../memory/ref_count_pool.cc
	$(LN_SF) $? $@

bitset.cc: ../bitset.cc
	$(LN_SF) $? $@

string.cc: ../string.cc
	$(LN_SF) $? $@

nibble_tables.cc: ../numeric/nibble_tables.cc
	$(LN_SF) $? $@

friend_function_formal_bug.stderr:
	$(TOUCH) $@

link_test.stderr:
	$(TOUCH) $@

excl_ptr_variant_test.stderr:
	$(TOUCH) $@

count_ptr_variant_test.stderr:
	$(TOUCH) $@

hash_map_iterator_O2_bug_test.stderr:
	$(TOUCH) $@

# special case: for static_count_ptr_test
if ABI_ILP32
# use default suffix rule
else
# override suffix rule, mimic ILP32 result
static_count_ptr_test.out-filter: static_count_ptr_test.out
	$(CAT) $< | $(SED) '/chunk/s/16\*[0-9]/16*4/g' | $(ADDRESS_FILTER) > $@
endif

# don't filter out hex
bitmanip_test.diff: bitmanip_test.out bitmanip_test.stderr
	-@out=`$(ECHO) $@ | $(SED) 's/\.diff$$/.out/g'` ; \
	expect=$(srcdir)/`$(ECHO) $@ | $(SED) 's/\.diff$$/.stderr/g'` ; \
	$(ECHO) "$(DIFF) -u $$expect $$out > $@" && \
	$(DIFF) -u $$expect $$out > $@ 2>&1 ; \
	$(TOUCH) $@

# some tests require more than the default filtering
MOREFILTER_TESTS = \
	local_static_store_test \
	multi_pool_module_test

PREFILTER_FILES = \
	$(MOREFILTER_TESTS:=.stderr-orig) \
	$(MOREFILTER_TESTS:=.out-orig)

multikey_set_derived_test.stderr: multikey_set_test.stderr
	$(CP) $? $@

# the util/ source directory
AM_CPPFLAGS = -I$(srcdir)/../..

if HAVE_GXX
# some of these flags are deadly...
GENERAL_WARN_FLAGS = @FANG_WARN_FLAGS@
# C specific warnings
WARN_CFLAGS = @FANG_WARN_CFLAGS@
# C++ specific warnings
WARN_CXXFLAGS = @FANG_WARN_CXXFLAGS@
# -Wsign-promo	# kills some code
DIALECT_FLAGS = @FANG_DIALECT_FLAGS@
# PIPE_FLAG = -pipe
PIPE_FLAG =
else
GENERAL_WARN_FLAGS = @FANG_WARN_FLAGS@
WARN_CFLAGS = @FANG_WARN_CFLAGS@
WARN_CXXFLAGS = @FANG_WARN_CXXFLAGS@
DIALECT_FLAGS = @FANG_DIALECT_FLAGS@
PIPE_FLAG =
endif

MORE_CFLAGS = $(PIPE_FLAG) $(DIALECT_FLAGS) $(WARN_CFLAGS) $(GENERAL_WARN_FLAGS)
MORE_CXXFLAGS = $(PIPE_FLAG) $(DIALECT_FLAGS) $(WARN_CXXFLAGS) $(GENERAL_WARN_FLAGS)

AM_CFLAGS = $(MORE_CFLAGS)
AM_CXXFLAGS = $(MORE_CXXFLAGS)

EXTRA_DIST += README local.cvsignore \
	$(noinst_PROGRAMS:=.stderr) \
	$(MOREFILTER_TESTS:=.morefilter.sh)

CLEANFILES += \
	$(PREFILTER_FILES) \
	$(EMPTY_STDERR_FILES)

DISTCLEANFILES += $(TEST_EXPECT_SH) $(libfangutil_la_SOURCES)

if CHECK_TESTS
TESTS += $(noinst_PROGRAMS:=.test)
endif

AUTO_IGNORE += test-expect.sh $(noinst_PROGRAMS) $(noinst_LTLIBRARIES) \
	$(BUILT_SOURCES) $(PREFILTER_FILES) $(TESTS)

# all-local: $(TESTS)

build-check-local: \
	$(TESTS) \
	$(noinst_PROGRAMS:=.out) \
	$(noinst_PROGRAMS:=.out-filter) \
	$(noinst_PROGRAMS:=.stderr-filter) \
	$(noinst_PROGRAMS:=.diff)

clean-local:
	-$(RM) $(BUILT_SOURCES)
	-$(RM) *.core core.*
	-$(RM) *.diff *.test *.out
	-$(RM) *.filter *-filter *-orig

include $(top_srcdir)/Make.global

