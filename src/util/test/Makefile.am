# "src/util/test/Makefile.am"

AUTOMAKE_OPTIONS = 1.9 subdir-objects

SUFFIXES = .test
TEST_EXPECT_SH = $(srcdir)/test-expect.sh

include $(top_srcdir)/Make.stddef

# auto-generated test scripts
.cc.test:
	@$(ECHO) "#!$(SHELL)" > $@
	@$(ECHO) "# \"$@\"" >> $@
	@$(ECHO) $(TEST_EXPECT_SH) $* $(srcdir) >> $@
	@$(CHMOD) +x $@

noinst_PROGRAMS = \
	exit_test \
	throw_test \
	what_test \
	pretty_function_test \
	stacktrace_test \
	friend_function_formal_bug \
	link_test \
	ptrs_test \
	pointer_classes_test \
	count_ptr_test \
	sublist_test \
	deque_iterator_test \
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
	ring_node_test


noinst_LTLIBRARIES = libfangutil.la libpooled.la \
	liblinktest.la libbogusutil.la

BUILT_SOURCES = stacktrace.cc static_trace.cc thread_lock.cc

ptrs_test_SOURCES = ptrs_test.cc var.h func.h
pointer_classes_test_SOURCES = pointer_classes_test.cc var.h func.h
count_ptr_test_SOURCES = count_ptr_test.cc var.h
sublist_test_SOURCES = sublist_test.cc
deque_iterator_test_SOURCES = deque_iterator_test.cc
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
multidimensional_qmap_test_SOURCES = multidimensional_qmap_test.cc
maplikeset_test_SOURCES = maplikeset_test.cc
multikey_qmap_test_SOURCES = multikey_qmap_test.cc
multikey_generator_test_SOURCES = multikey_generator_test.cc
multikey_qmap_slice_test_SOURCES = multikey_qmap_slice_test.cc cube_slice.h
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

libfangutil_la_SOURCES = stacktrace.cc static_trace.cc thread_lock.cc
libbogusutil_la_SOURCES = bogus_stacktrace.cc bogus_stacktrace.h static_trace.cc
libpooled_la_SOURCES = \
	pool_module_a.cc pool_module_b.cc pool_module_a.h pool_module_b.h
liblinktest_la_SOURCES = \
	link_mod_a.cc link_mod_b.cc link_mod_a.h link_mod_b.h

# liblinktest_la_LDFLAGS = -multiply_defined

# do not explicitly reference files in parent directory, because automake
# will clobber the .deps dependencies in the parent directory, 
# possibly confusing the parent directories when they expect .deps to
# exist, especially on make distclean.  
stacktrace.cc: ../stacktrace.cc
	$(LN_S) $< $@

static_trace.cc: ../static_trace.cc
	$(LN_S) $< $@

thread_lock.cc: ../memory/thread_lock.cc
	$(LN_S) $< $@

friend_function_formal_bug.stderr:
	$(TOUCH) $@

link_test.stderr:
	$(TOUCH) $@

# some tests require more than the default filtering
MOREFILTER_TESTS = \
	local_static_store_test \
	multi_pool_module_test

PREFILTER_FILES = \
	$(MOREFILTER_TESTS:=.stderr.prefilter) \
	$(MOREFILTER_TESTS:=.out.prefilter)

# but is included in distribution, EXTRA_DIST? don't bother cleaning?
EMPTY_STDERR_FILES = friend_function_formal_bug.stderr \
	link_test.stderr

multikey_set_derived_test.stderr: multikey_set_test.stderr
	$(CP) $< $@

AUTO_STDERR_FILES = multikey_set_derived_test.stderr

# the util/ source directory
AM_CPPFLAGS = -I$(srcdir)/..

# some of these flags are deadly...
GENERAL_WARN_FLAGS = -W -Wall -Wno-unused -Werror
WARN_CFLAGS = -Wmissing-prototypes -Wstrict-prototypes
WARN_CXXFLAGS = -Wold-style-cast -Woverloaded-virtual
# -Wsign-promo	# kills some code
DIALECT_FLAGS = -ansi
# -pedantic	# kills some code
MORE_CFLAGS = -pipe $(DIALECT_FLAGS) $(WARN_CFLAGS) $(GENERAL_WARN_FLAGS)
MORE_CXXFLAGS = -pipe $(DIALECT_FLAGS) $(WARN_CXXFLAGS) $(GENERAL_WARN_FLAGS)

AM_CFLAGS = $(MORE_CFLAGS)
AM_CXXFLAGS = $(MORE_CXXFLAGS)

EXTRA_DIST = test-expect.sh test.cvsignore \
	$(noinst_PROGRAMS:=.stderr) \
	$(MOREFILTER_TESTS:=.morefilter.sh)

CLEANFILES = \
	$(noinst_PROGRAMS:=.test) \
	$(noinst_PROGRAMS:=.diff) \
	$(noinst_PROGRAMS:=.filter) \
	$(noinst_PROGRAMS:=.out) \
	$(PREFILTER_FILES) \
	$(EMPTY_STDERR_FILES)

DISTCLEANFILES = .cvsignore $(libfangutil_la_SOURCES)

TESTS = $(noinst_PROGRAMS:=.test)

SPACES_TO_NEWLINE = $(AWK) -f $(top_srcdir)/test/spacestonewline.awk

AUTO_IGNORE = $(noinst_PROGRAMS) $(noinst_LTLIBRARIES) \
	$(EMPTY_STDERR_FILES) $(AUTO_STDERR_FILES) $(PREFILTER_FILES)

# resorting to intermediate file because distcheck sets file permissions
# to forbid writing/appending to existing files... (read-only)
.cvsignore: test.cvsignore
	@$(ECHO) $(AUTO_IGNORE) | $(SPACES_TO_NEWLINE) > $@.temp
	$(CAT) $< $@.temp > $@
	-$(RM) $@.temp

.cvsignore: Makefile.am Makefile.in

all-local: .cvsignore $(TESTS) $(EMPTY_STDERR_FILES) $(AUTO_STDERR_FILES)

clean-local:
	-$(RM) $(BUILT_SOURCES)
	-$(RM) *.core core.*
	-$(RM) *.diff *.test *.out *.filter

