# "src/util/Makefile.inc"
#	$Id: Makefile.inc,v 1.49 2011/05/05 06:50:48 fang Exp $
#	vi: ft=automake

# TODO: consider moving guile support down to subdirectory, like lang/guile

# dirgen is completely bogus, workaround to coerce directory generation
UTILPROGRAMS = util/progs/readline_toy
LIBFANGUTIL = libfangutil.la
UTILLIBS = $(LIBFANGUTIL) libguileutil.la

util-programs: $(UTILPROGRAMS)
util-libs: $(UTILLIBS)

# SUBDIRS += util/test

# template definition header files
UTIL_TCC_FILES = \
	util/IO_utils.tcc \
	util/array.tcc \
	util/bitarray.tcc \
	util/bitset.tcc \
	util/const_assoc_query.tcc \
	util/discrete_interval_set.tcc \
	util/getopt_mapped.tcc \
	util/hash_qmap.tcc \
	util/list_vector.tcc \
	util/memory/allocator_adaptor.tcc \
	util/memory/chunk_map_pool.tcc \
	util/memory/destruction_policy.tcc \
	util/memory/fixed_pool_chunk.tcc \
	util/memory/index_pool.tcc \
	util/memory/list_vector_pool.tcc \
	util/memory/typeless_memory_chunk.tcc \
	util/memory/count_ptr.tcc \
	util/memory/count_array_ptr.tcc \
	util/memory/count_malloc_ptr.tcc \
	util/multidimensional_qmap.tcc \
	util/multidimensional_sparse_set.tcc \
	util/multikey.tcc \
	util/multikey_assoc.tcc \
	util/multikey_map.tcc \
	util/multikey_qmap.tcc \
	util/multikey_set.tcc \
	util/new_functor.tcc \
	util/numformat.tcc \
	util/optparse.tcc \
	util/packed_array.tcc \
	util/persistent_object_manager.tcc \
	util/persistent_functor.tcc \
	util/qmap.tcc \
	util/ring_node.tcc \
	util/string.tcc \
	util/sublist.tcc \
	util/tree_cache.tcc \
	util/unique_list.tcc \
	util/unique_stack.tcc \
	util/what.tcc

# POLICY: every .tcc MUST have a corresponding .h file, 
# but the _fwd.h file is optional.  
UTIL_HEADER_FILES = \
	util/attributes.h \
	util/c99.h \
	util/c_decl.h \
	util/ctype.h \
	util/cppcat.h \
	util/FILE_fwd.h \
	util/free.h \
	util/getopt_portable.h \
	util/inttypes.h \
	util/libc.h \
	util/libc_temp.h \
	util/likely.h \
	util/macros.h \
	util/NULL.h \
	util/numeric/constants.h \
	util/pthread.h \
	util/readline.h \
	util/signal.h \
	util/size_t.h \
	util/typeof.h \
	util/utypes.h \
	util/xdr.h

UTIL_CXX_HEADER_FILES = \
	$(UTIL_TCC_FILES:.tcc=.hh) \
	util/fig/xfig.hh \
	util/STL/algorithm_fwd.hh \
	util/STL/back_inserter.hh \
	util/STL/allocator_fwd.hh \
	util/STL/construct_fwd.hh \
	util/STL/container_iterator.hh \
	util/STL/deque_fwd.hh \
	util/STL/functional_fwd.hh \
	util/STL/functional.hh \
	util/STL/hash_map_fwd.hh \
	util/STL/hash_map.hh \
	util/STL/hash_map_utils.hh \
	util/STL/iterator_fwd.hh \
	util/STL/list_fwd.hh \
	util/STL/map_fwd.hh \
	util/STL/namespace.hh \
	util/STL/pair_fwd.hh \
	util/STL/queue_fwd.hh \
	util/STL/reverse_iterator.hh \
	util/STL/set_fwd.hh \
	util/STL/stack_fwd.hh \
	util/STL/valarray_fwd.hh \
	util/STL/valarray_iterator.hh \
	util/STL/vector_fwd.hh \
	util/IO_utils_fwd.hh \
	util/addressof.hh \
	util/algorithm.hh \
	util/array_indexer.hh \
	util/array_traits.hh \
	util/assoc_traits_fwd.hh \
	util/assoc_traits.hh \
	util/bitmanip.hh \
	util/bitwise_functional.hh \
	util/boolean_types.hh \
	util/caller.hh \
	util/call_traits.hh \
	util/copy_if.hh \
	util/counter.hh \
	util/convert.hh \
	util/dereference.hh \
	util/directory.hh \
	util/dirent.hh \
	util/discrete_interval_set_fwd.hh \
	util/finite_vector.hh \
	util/file_status.hh \
	util/for_all.hh \
	util/format_IO.hh \
	util/function_traits.hh \
	util/hash_qmap_fwd.hh \
	util/hash_specializations.hh \
	util/ifthenelse.hh \
	util/indent.hh \
	util/index_functor.hh \
	util/iomanip.hh \
	util/iterator_more.hh \
	util/list_vector_fwd.hh \
	util/maplikeset.hh \
	util/member_saver_fwd.hh \
	util/member_saver.hh \
	util/member_select.hh \
	util/memory/deallocation_policy_fwd.hh \
	util/memory/deallocation_policy.hh \
	util/memory/deep_copy.hh \
	util/memory/chunk_map_pool_fwd.hh \
	util/memory/list_vector_pool_fwd.hh \
	util/memory/pointer_classes_fwd.hh \
	util/memory/pointer_classes.hh \
	util/memory/pointer_traits.hh \
	util/memory/pointer_manipulator.hh \
	util/memory/unique_ptr.hh \
	util/memory/excl_ptr.hh \
	util/memory/excl_array_ptr.hh \
	util/memory/excl_malloc_ptr.hh \
	util/memory/free_list.hh \
	util/memory/ref_count_pool.hh \
	util/memory/ref_count_pool_anchor.hh \
	util/memory/thread_lock.hh \
	util/multidimensional_qmap_fwd.hh \
	util/multidimensional_sparse_set_fwd.hh \
	util/multikey_fwd.hh \
	util/multikey_assoc_fwd.hh \
	util/multikey_map_fwd.hh \
	util/multikey_qmap_fwd.hh \
	util/multikey_set_fwd.hh \
	util/named_ifstream_manager.hh \
	util/nested_iterator.hh \
	util/new_functor_fwd.hh \
	util/nullary_function_fwd.hh \
	util/nullary_function.hh \
	util/null_construct.hh \
	util/numeric/abs.hh \
	util/numeric/bigger_ints.hh \
	util/numeric/clz.hh \
	util/numeric/ctz.hh \
	util/numeric/div.hh \
	util/numeric/ffs.hh \
	util/numeric/functional.hh \
	util/numeric/integer_traits.hh \
	util/numeric/inttype_traits.hh \
	util/numeric/nibble_tables.hh \
	util/numeric/parity.hh \
	util/numeric/popcount.hh \
	util/numeric/random.hh \
	util/numeric/sign_traits.hh \
	util/numeric/trigonometry.hh \
	util/numeric/zero.hh \
	util/numeric/zerodiv_detect.hh \
	util/offset_array.hh \
	util/operators.hh \
	util/optparse_fwd.hh \
	util/optparse.hh \
	util/packed_array_fwd.hh \
	util/persistent_fwd.hh \
	util/persistent.hh \
	util/persistent_hash.hh \
	util/qmap_fwd.hh \
	util/readline_wrap.hh \
	util/rebinder.hh \
	util/reference_wrapper.hh \
	util/reserve.hh \
	util/ring_node_fwd.hh \
	util/sfinae_types.hh \
	util/sstream.hh \
	util/stack_saver.hh \
	util/stacktrace.hh \
	util/static_assert.hh \
	util/static_trace.hh \
	util/stream_ptr.hh \
	util/string_fwd.hh \
	util/swap_saver.hh \
	util/syntax_fwd.hh \
	util/syntax.hh \
	util/tagged_subtype.hh \
	util/template.hh \
	util/timer.hh \
	util/tokenize_fwd.hh \
	util/tokenize.hh \
	util/type_traits.hh \
	util/union_find.hh \
	util/uninitialized.hh \
	util/using_istream.hh \
	util/using_ostream.hh \
	util/value_saver_fwd.hh \
	util/value_saver.hh \
	util/vector_ops.hh \
	util/visitee.hh \
	util/visitor_functor.hh \
	util/what_fwd.hh \
	util/wtf.hh \
	util/binders.hh \
	util/compose.hh \
	util/conditional.hh \
	util/ptrs_functional.hh

UTIL_CC_FILES = \
	util/IO_utils.cc \
	util/bitset.cc \
	util/directory.cc \
	util/dirent.cc \
	util/fig/xfig.cc \
	util/format_IO.cc \
	util/indent.cc \
	util/getopt_mapped.cc \
	util/named_ifstream_manager.cc \
	util/numformat.cc \
	util/optparse.cc \
	util/persistent.cc \
	util/persistent_object_manager.cc \
	util/readline_wrap.cc \
	util/stacktrace.cc \
	util/static_trace.cc \
	util/string.cc \
	util/syntax.cc \
	util/timer.cc \
	util/tokenize.cc \
	util/stream_ptr.cc \
	util/uninitialized.cc \
	util/memory/deallocation_policy.cc \
	util/memory/thread_lock.cc \
	util/memory/ref_count_pool.cc \
	util/numeric/nibble_tables.cc \
	util/numeric/random.cc

UTIL_C_FILES = \
	util/libc.c

libfangutil_la_SOURCES = \
	$(UTIL_C_FILES) \
	$(UTIL_CC_FILES) \
	$(UTIL_CXX_HEADER_FILES) \
	$(UTIL_HEADER_FILES) \
	$(UTIL_TCC_FILES)
libfangutil_la_LDFLAGS = $(RL_LDPATH) $(NCURSES_LDPATH)
libfangutil_la_LIBADD = $(RL_LIB) $(NCURSES_LIB)
# TODO: readline often depends on ncurses
# extra libs should be detected by configure

# installed headers (keep to minimum)
nobase_pkginclude_HEADERS += \
	util/boolean_types.hh \
	util/cppcat.h \
	util/macros.h \
	util/memory/pointer_classes_fwd.hh \
	util/memory/pointer_manipulator.hh \
	util/memory/pointer_traits.hh \
	util/memory/count_ptr.hh \
	util/new_functor_fwd.hh \
	util/persistent_fwd.hh \
	util/persistent.hh

GUILE_UTIL_HH_FILES = \
	util/libguile.hh \
	util/guile_STL.hh

GUILE_UTIL_H_FILES = \
	util/guile_gh.h

UTIL_CXX_HEADER_FILES += $(GUILE_UTIL_HH_FILES)
UTIL_HEADER_FILES += $(GUILE_UTIL_H_FILES)

# exceptions to work-around guile header dependencies
GUILE_WARN_FLAGS = -Wno-long-long -Wno-error
# -Wno-old-style-cast -Wno-undef
LIBGUILE_HCHK_FLAGS = $(GUILE_CPPFLAGS) $(GUILE_WARN_FLAGS)


libguileutil_la_SOURCES = \
	util/libguile.cc \
	$(GUILE_UTIL_HH_FILES) \
	$(GUILE_UTIL_H_FILES)
libguileutil_la_CPPFLAGS = $(GUILE_CPPFLAGS)
libguileutil_la_CXXFLAGS = $(AM_CXXFLAGS) $(GUILE_WARN_FLAGS)
libguileutil_la_LDFLAGS = $(GUILE_LDFLAGS)
# libguileutil_la_LIBADD =

util_progs_readline_toy_SOURCES = util/progs/readline_toy.cc
util_progs_readline_toy_LDADD = $(LIBFANGUTIL)

ALL_HEADER_FILES += $(UTIL_HEADER_FILES)
ALL_CXX_HEADER_FILES += $(UTIL_CXX_HEADER_FILES)

ALL_TCC_FILES += $(UTIL_TCC_FILES)

# skip these, not standalone
util/using_istream.hhchk:
	@$(TOUCH) $@

util/using_ostream.hhchk:
	@$(TOUCH) $@

# extending default header check
LIBGUILE_HCHK_TEST_SCRIPT =
LIBGUILE_HHCHK_TEST_SCRIPT =
if HAVE_LIBGUILE
LIBGUILE_HCHK_TEST_SCRIPT += \
	$(ECHO) "Validating $< ... " ; \
	more_hchk_flags="$(LIBGUILE_HCHK_FLAGS)" ; \
	$(H_CHECK_TEST_SCRIPT)
LIBGUILE_HHCHK_TEST_SCRIPT += \
	$(ECHO) "Validating $< ... " ; \
	more_hchk_flags="$(LIBGUILE_HCHK_FLAGS)" ; \
	$(HH_CHECK_TEST_SCRIPT)
else
LIBGUILE_HCHK_TEST_SCRIPT += \
	$(ECHO) "Skipping $< ... " ; \
	$(TOUCH) $@
LIBGUILE_HHCHK_TEST_SCRIPT += \
	$(ECHO) "Skipping $< ... " ; \
	$(TOUCH) $@
endif

util/libguile.hhchk: util/libguile.hh
	@$(LIBGUILE_HHCHK_TEST_SCRIPT)

util/guile_gh.hchk: util/guile_gh.h
	@$(LIBGUILE_HCHK_TEST_SCRIPT)

util/guile_STL.hhchk: util/guile_STL.hh
	@$(LIBGUILE_HHCHK_TEST_SCRIPT)

LEXYACC_PREFIX_GENERATOR = util/lang/lexyacc/lexyacc-prefix-generator.awk

EXTRA_DIST += \
	util/ChangeLog \
	util/README \
	util/TODO \
	util/STL/README \
	util/progs/README \
	util/lang/README \
	$(LEXYACC_PREFIX_GENERATOR)

UTIL_DIRS = util util/numeric util/memory util/STL \
	util/lang util/lang/lexyacc util/lang/CX util/progs

CLEANSUBDIRS += $(UTIL_DIRS)

# oddly, this dir gets missed during distclean
DISTCLEANDIRS += util/progs/$(LTDL_OBJDIR)

util/.cvsignore: .cvsignore
	$(CP) $? $@
util/STL/.cvsignore: util/.cvsignore
	$(CP) $? $@
util/lang/.cvsignore: util/.cvsignore
	$(CP) $? $@
util/lang/lexyacc/.cvsignore: util/.cvsignore
	$(CP) $? $@
util/lang/CX/.cvsignore: util/.cvsignore
	$(CP) $? $@
util/memory/.cvsignore: util/.cvsignore
	$(CP) $? $@
util/numeric/.cvsignore: util/.cvsignore
	$(CP) $? $@
util/progs/.cvsignore: util/.cvsignore
	$(CP) $? $@

util/lang/CX/cx-parse.yy: util/lang/CX/cx-parse.yy.in
	cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@

# files to ignore in subdirectories
# can we generate this from suffix appending?
SUBDIR_CVSIGNORE += \
	util/.cvsignore util/STL/.cvsignore \
	util/lang/.cvsignore util/lang/lexyacc/.cvsignore \
	util/lang/CX/.cvsignore \
	util/memory/.cvsignore util/numeric/.cvsignore \
	util/progs/.cvsignore

