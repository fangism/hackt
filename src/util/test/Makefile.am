# "src/util/test/Makefile.am"

AUTOMAKE_OPTIONS = 1.9 subdir-objects

SUFFIXES = .test
TEST_EXPECT_SH = $(srcdir)/test-expect.sh

include $(top_srcdir)/Make.stddef

.cc.test:
	@$(ECHO) "#!$(SHELL)" > $@
	@$(ECHO) "# \"$@\"" >> $@
	@$(ECHO) $(TEST_EXPECT_SH) $* $(srcdir) >> $@
	@$(CHMOD) +x $@

noinst_PROGRAMS = \
	ptrs_test \
	pointer_classes_test \
	count_ptr_test \
	sublist_test \
	deque_iterator_test \
	hash_qmap_test \
	discrete_interval_set_test \
	list_vector_test \
	list_vector_pool_test \
	multidimensional_sparse_set_test \
	multidimensional_qmap_test \
	multikey_qmap_test \
	multikey_generator_test \
	multikey_qmap_slice_test \
	packed_array_test \
	exit_test \
	throw_test \
	stacktrace_test \
	what_test \
	pretty_function_test

ptrs_test_SOURCES = ptrs_test.cc var.h func.h
pointer_classes_test_SOURCES = pointer_classes_test.cc var.h func.h
count_ptr_test_SOURCES = count_ptr_test.cc var.h
sublist_test_SOURCES = sublist_test.cc
deque_iterator_test_SOURCES = deque_iterator_test.cc
hash_qmap_test_SOURCES = hash_qmap_test.cc
list_vector_test_SOURCES = list_vector_test.cc
list_vector_pool_test_SOURCES = list_vector_pool_test.cc
discrete_interval_set_test_SOURCES = discrete_interval_set_test.cc
multidimensional_sparse_set_test_SOURCES = multidimensional_sparse_set_test.cc
multidimensional_qmap_test_SOURCES = multidimensional_qmap_test.cc
multikey_qmap_test_SOURCES = multikey_qmap_test.cc
multikey_generator_test_SOURCES = multikey_generator_test.cc
multikey_qmap_slice_test_SOURCES = multikey_qmap_slice_test.cc cube_slice.h
packed_array_test_SOURCES = packed_array_test.cc
exit_test_SOURCES = exit_test.cc pooled_thing.cc pooled_thing.h
throw_test_SOURCES = throw_test.cc pooled_thing.cc pooled_thing.h
stacktrace_test_SOURCES = stacktrace_test.cc stacktrace.cc
what_test_SOURCES = what_test.cc
pretty_function_test_SOURCES = pretty_function_test.cc

# do not explicitly reference files in parent directory, because automake
# will clobber the .deps dependencies in the parent directory, 
# possibly confusing the parent directories when they expect .deps to
# exist, especially on make distclean.  
stacktrace.cc: ../stacktrace.cc
	$(LN_S) $< $@

# the util/ source directory
AM_CPPFLAGS = -I$(srcdir)/..

WARN_CFLAGS = -W -Wall -Wno-unused -Werror
MORE_CFLAGS = -ansi -pipe $(WARN_CFLAGS)

AM_CFLAGS = $(MORE_CFLAGS)
AM_CXXFLAGS = $(MORE_CFLAGS)

EXTRA_DIST = test-expect.sh test.cvsignore \
	$(noinst_PROGRAMS:=.stderr)

CLEANFILES = \
	$(noinst_PROGRAMS:=.test) \
	$(noinst_PROGRAMS:=.diff) \
	$(noinst_PROGRAMS:=.filter) \
	$(noinst_PROGRAMS:=.out)

DISTCLEANFILES = .cvsignore

TESTS = $(noinst_PROGRAMS:=.test)

SPACES_TO_NEWLINE = $(AWK) -f $(top_srcdir)/test/spacestonewline.awk

# resorting to intermediate file because distcheck sets file permissions
# to forbid writing/appending to existing files... (read-only)
.cvsignore: test.cvsignore
	$(ECHO) $(noinst_PROGRAMS) | $(SPACES_TO_NEWLINE) > $@.temp
	$(CAT) $< $@.temp > $@
	-$(RM) $@.temp

all-local: .cvsignore

clean-local:
	-$(RM) *.core core.*
	-$(RM) *.diff *.test *.out *.filter

