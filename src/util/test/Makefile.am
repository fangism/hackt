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
	count_ptr_test \
	sublist_test \
	deque_iterator_test \
	hash_qmap_test \
	discrete_interval_set_test \
	multidimensional_sparse_set_test \
	multidimensional_qmap_test \
	multikey_qmap_test \
	multikey_generator_test \
	multikey_qmap_slice_test

# not yet
#	list_vector_pool_test

ptrs_test_SOURCES = ptrs_test.cc var.h func.h
count_ptr_test_SOURCES = count_ptr_test.cc var.h
sublist_test_SOURCES = sublist_test.cc
deque_iterator_test_SOURCES = deque_iterator_test.cc
hash_qmap_test_SOURCES = hash_qmap_test.cc
# list_vector_pool_test_SOURCES = list_vector_pool_test.cc
discrete_interval_set_test_SOURCES = discrete_interval_set_test.cc
multidimensional_sparse_set_test_SOURCES = multidimensional_sparse_set_test.cc
multidimensional_qmap_test_SOURCES = multidimensional_qmap_test.cc
multikey_qmap_test_SOURCES = multikey_qmap_test.cc
multikey_generator_test_SOURCES = multikey_generator_test.cc
multikey_qmap_slice_test_SOURCES = multikey_qmap_slice_test.cc cube_slice.h

# the util/ source directory
AM_CPPFLAGS = -I$(srcdir)/..

MORE_CFLAGS = -pipe -W -Wall -Wno-unused -Werror

AM_CFLAGS = $(MORE_CFLAGS)
AM_CXXFLAGS = $(MORE_CFLAGS)

EXTRA_DIST = test-expect.sh $(noinst_PROGRAMS:=.stderr)

CLEANFILES = \
	$(noinst_PROGRAMS:=.test) \
	$(noinst_PROGRAMS:=.diff) \
	$(noinst_PROGRAMS:=.out)

TESTS = $(noinst_PROGRAMS:=.test)

clean-local:
	-$(RM) *.core core.*
	-$(RM) *.diff *.test *.out

