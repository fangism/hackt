# "src/util/test/Makefile.am"
#	$Id: Makefile.am,v 1.50 2006/10/24 07:27:47 fang Exp $
# NOTE: none of the tests in this directory should depend on ANY
# external package libraries or headers, so we coercively set
# LIBS and LDFLAGS to empty, which is normally bad policy for the
# real project being built.  We forgive this directory, for it is exceptional.
# We do, however, keep CXXFLAGS as specified by the user.  

include $(top_srcdir)/Make.stddef

AUTOMAKE_OPTIONS += subdir-objects

SUFFIXES = .test .ii .i .hii .hhii .tccii
TEST_EXPECT_SH = test-expect.sh

all-local: $(TEST_EXPECT_SH)

check_SCRIPTS = $(TEST_EXPECT_SH)

# auto-generated test scripts
.cc.test:
	@$(ECHO) "#!$(SHELL)" > $@
	@$(ECHO) "# \"$@\"" >> $@
	@$(ECHO) ./$(TEST_EXPECT_SH) ./$* $(srcdir) >> $@
	@$(CHMOD) +x $@

# these are all test programs, not to be installed
noinst_PROGRAMS = \
	exit_test \
	throw_test \
	what_test \
	pretty_function_test \
	stacktrace_test \
	friend_function_formal_bug \
	link_test \
	pointer_classes_test \
	excl_ptr_variant_test \
	count_ptr_test \
	count_ptr_variant_test \
	sublist_test \
	deque_iterator_test \
	hash_map_iterator_O2_bug_test \
	hash_qmap_test \
	discrete_interval_set_test \
	EBCO_test \
	integer_traits_test \
	new_functor_test \
	list_vector_test \
	list_vector_pool_test \
	chunk_map_pool_test \
	static_count_ptr_test \
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
	sparse_collection_test


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

BUILT_SOURCES = stacktrace.cc static_trace.cc thread_lock.cc ref_count_pool.cc \
	$(EMPTY_STDERR_FILES) $(AUTO_STDERR_FILES)

pointer_classes_test_SOURCES = pointer_classes_test.cc var.h func.h
excl_ptr_variant_test_SOURCES = excl_ptr_variant_test.cc
count_ptr_variant_test_SOURCES = count_ptr_variant_test.cc
count_ptr_variant_test_LDADD = libfangutil.la
count_ptr_test_SOURCES = count_ptr_test.cc var.h
count_ptr_test_LDADD = libfangutil.la
sublist_test_SOURCES = sublist_test.cc
deque_iterator_test_SOURCES = deque_iterator_test.cc
deque_iterator_test_LDADD = libfangutil.la
hash_map_iterator_O2_bug_test_SOURCES = hash_map_iterator_O2_bug_test.cc
hash_qmap_test_SOURCES = hash_qmap_test.cc
friend_function_formal_bug_SOURCES = friend_function_formal_bug.cc
link_test_SOURCES = link_test.cc
link_test_LDADD = liblinktest.la libbogusutil.la
list_vector_test_SOURCES = list_vector_test.cc
list_vector_pool_test_SOURCES = list_vector_pool_test.cc
list_vector_pool_test_LDADD = libfangutil.la
chunk_map_pool_test_SOURCES = chunk_map_pool_test.cc
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

libfangutil_la_SOURCES = stacktrace.cc static_trace.cc thread_lock.cc \
	ref_count_pool.cc
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
	$(LN_S) $? $@

static_trace.cc: ../static_trace.cc
	$(LN_S) $? $@

thread_lock.cc: ../memory/thread_lock.cc
	$(LN_S) $? $@

ref_count_pool.cc: ../memory/ref_count_pool.cc
	$(LN_S) $? $@

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

# some tests require more than the default filtering
MOREFILTER_TESTS = \
	local_static_store_test \
	multi_pool_module_test

PREFILTER_FILES = \
	$(MOREFILTER_TESTS:=.stderr.prefilter) \
	$(MOREFILTER_TESTS:=.out.prefilter)

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

EXTRA_DIST += README test-expect.sh.in local.cvsignore \
	$(noinst_PROGRAMS:=.stderr) \
	$(MOREFILTER_TESTS:=.morefilter.sh)

# test-expect.sh: $(top_builddir)/config.status test-expect.sh.in
#	cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@

CLEANFILES += \
	$(noinst_PROGRAMS:=.test) \
	$(noinst_PROGRAMS:=.diff) \
	$(noinst_PROGRAMS:=.filter) \
	$(noinst_PROGRAMS:=.stderr.filter) \
	$(noinst_PROGRAMS:=.out) \
	$(noinst_PROGRAMS:=.out.filter) \
	$(PREFILTER_FILES) \
	$(EMPTY_STDERR_FILES)

DISTCLEANFILES += $(TEST_EXPECT_SH) $(libfangutil_la_SOURCES)

TESTS += $(noinst_PROGRAMS:=.test)

AUTO_IGNORE += test-expect.sh $(noinst_PROGRAMS) $(noinst_LTLIBRARIES) \
	$(BUILT_SOURCES) $(PREFILTER_FILES) $(TESTS)

all-local: $(TESTS)

clean-local:
	-$(RM) $(BUILT_SOURCES)
	-$(RM) *.core core.*
	-$(RM) *.diff *.test *.out *.filter

include $(top_srcdir)/Make.global

