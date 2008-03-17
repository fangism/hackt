# "src/Makefile.am"
#	$Id: Makefile.inc,v 1.37 2008/03/17 23:03:08 fang Exp $

# TODO: consider moving guile support down to subdirectory, like lang/guile
#	vi: ft=automake

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
	util/bitset.tcc \
	util/const_assoc_query.tcc \
	util/discrete_interval_set.tcc \
	util/getopt_mapped.tcc \
	util/hash_qmap.tcc \
	util/list_vector.tcc \
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
	util/packed_array.tcc \
	util/persistent_object_manager.tcc \
	util/persistent_functor.tcc \
	util/qmap.tcc \
	util/ring_node.tcc \
	util/string.tcc \
	util/sublist.tcc \
	util/unique_list.tcc \
	util/unique_stack.tcc \
	util/what.tcc

# POLICY: every .tcc MUST have a corresponding .h file, 
# but the _fwd.h file is optional.  
UTIL_HEADER_FILES = \
	$(UTIL_TCC_FILES:.tcc=.h) \
	util/STL/algorithm_fwd.h \
	util/STL/back_inserter.h \
	util/STL/allocator_fwd.h \
	util/STL/construct_fwd.h \
	util/STL/container_iterator.h \
	util/STL/deque_fwd.h \
	util/STL/functional_fwd.h \
	util/STL/functional.h \
	util/STL/hash_map_fwd.h \
	util/STL/hash_map.h \
	util/STL/hash_map_utils.h \
	util/STL/iterator_fwd.h \
	util/STL/list_fwd.h \
	util/STL/map_fwd.h \
	util/STL/namespace.h \
	util/STL/pair_fwd.h \
	util/STL/queue_fwd.h \
	util/STL/reverse_iterator.h \
	util/STL/set_fwd.h \
	util/STL/stack_fwd.h \
	util/STL/valarray_fwd.h \
	util/STL/valarray_iterator.h \
	util/STL/vector_fwd.h \
	util/IO_utils_fwd.h \
	util/addressof.h \
	util/algorithm.h \
	util/array_traits.h \
	util/assoc_traits_fwd.h \
	util/assoc_traits.h \
	util/attributes.h \
	util/bitmanip.h \
	util/boolean_types.h \
	util/c99.h \
	util/c_decl.h \
	util/caller.h \
	util/call_traits.h \
	util/copy_if.h \
	util/cppcat.h \
	util/counter.h \
	util/convert.h \
	util/ctype.h \
	util/dereference.h \
	util/dirent.h \
	util/discrete_interval_set_fwd.h \
	util/finite_vector.h \
	util/FILE_fwd.h \
	util/file_status.h \
	util/for_all.h \
	util/format_IO.h \
	util/free.h \
	util/getopt_portable.h \
	util/hash_qmap_fwd.h \
	util/hash_specializations.h \
	util/ifthenelse.h \
	util/indent.h \
	util/inttypes.h \
	util/iterator_more.h \
	util/libc.h \
	util/libc_temp.h \
	util/likely.h \
	util/list_vector_fwd.h \
	util/macros.h \
	util/maplikeset.h \
	util/member_saver_fwd.h \
	util/member_saver.h \
	util/member_select.h \
	util/memory/deallocation_policy_fwd.h \
	util/memory/deallocation_policy.h \
	util/memory/deep_copy.h \
	util/memory/chunk_map_pool_fwd.h \
	util/memory/list_vector_pool_fwd.h \
	util/memory/pointer_classes_fwd.h \
	util/memory/pointer_classes.h \
	util/memory/pointer_traits.h \
	util/memory/pointer_manipulator.h \
	util/memory/unique_ptr.h \
	util/memory/excl_ptr.h \
	util/memory/excl_array_ptr.h \
	util/memory/excl_malloc_ptr.h \
	util/memory/free_list.h \
	util/memory/ref_count_pool.h \
	util/memory/ref_count_pool_anchor.h \
	util/memory/thread_lock.h \
	util/multidimensional_qmap_fwd.h \
	util/multidimensional_sparse_set_fwd.h \
	util/multikey_fwd.h \
	util/multikey_assoc_fwd.h \
	util/multikey_map_fwd.h \
	util/multikey_qmap_fwd.h \
	util/multikey_set_fwd.h \
	util/named_ifstream_manager.h \
	util/nested_iterator.h \
	util/new_functor_fwd.h \
	util/NULL.h \
	util/nullary_function_fwd.h \
	util/nullary_function.h \
	util/null_construct.h \
	util/numeric/bigger_ints.h \
	util/numeric/clz.h \
	util/numeric/ctz.h \
	util/numeric/div.h \
	util/numeric/ffs.h \
	util/numeric/functional.h \
	util/numeric/integer_traits.h \
	util/numeric/inttype_traits.h \
	util/numeric/nibble_tables.h \
	util/numeric/parity.h \
	util/numeric/popcount.h \
	util/numeric/random.h \
	util/numeric/sign_traits.h \
	util/numeric/zerodiv_detect.h \
	util/offset_array.h \
	util/operators.h \
	util/packed_array_fwd.h \
	util/persistent_fwd.h \
	util/persistent.h \
	util/persistent_hash.h \
	util/pthread.h \
	util/qmap_fwd.h \
	util/readline.h \
	util/readline_wrap.h \
	util/rebinder.h \
	util/reference_wrapper.h \
	util/reserve.h \
	util/ring_node_fwd.h \
	util/signal.h \
	util/sfinae_types.h \
	util/size_t.h \
	util/sstream.h \
	util/stacktrace.h \
	util/static_assert.h \
	util/static_trace.h \
	util/stream_ptr.h \
	util/string_fwd.h \
	util/tagged_subtype.h \
	util/template.h \
	util/tokenize_fwd.h \
	util/tokenize.h \
	util/type_traits.h \
	util/union_find.h \
	util/using_istream.h \
	util/using_ostream.h \
	util/utypes.h \
	util/value_saver_fwd.h \
	util/value_saver.h \
	util/visitee.h \
	util/visitor_functor.h \
	util/what_fwd.h \
	util/wtf.h \
	util/xdr.h \
	util/binders.h \
	util/compose.h \
	util/conditional.h \
	util/ptrs_functional.h

UTIL_CC_FILES = \
	util/IO_utils.cc \
	util/bitset.cc \
	util/dirent.cc \
	util/format_IO.cc \
	util/indent.cc \
	util/getopt_mapped.cc \
	util/libc.c \
	util/named_ifstream_manager.cc \
	util/persistent.cc \
	util/persistent_object_manager.cc \
	util/readline_wrap.cc \
	util/stacktrace.cc \
	util/static_trace.cc \
	util/tokenize.cc \
	util/stream_ptr.cc \
	util/memory/thread_lock.cc \
	util/memory/ref_count_pool.cc \
	util/numeric/nibble_tables.cc \
	util/numeric/random.cc

libfangutil_la_SOURCES = \
	$(UTIL_CC_FILES) \
	$(UTIL_HEADER_FILES) \
	$(UTIL_TCC_FILES)

# installed headers (keep to minimum)
nobase_pkginclude_HEADERS += \
	util/boolean_types.h \
	util/cppcat.h \
	util/macros.h \
	util/memory/pointer_classes_fwd.h \
	util/memory/pointer_manipulator.h \
	util/memory/pointer_traits.h \
	util/memory/count_ptr.h \
	util/new_functor_fwd.h \
	util/persistent_fwd.h \
	util/persistent.h

GUILE_UTIL_H_FILES = \
	util/libguile.h \
	util/guile_gh.h \
	util/guile_STL.h

UTIL_HEADER_FILES += $(GUILE_UTIL_H_FILES)

# exceptions to work-around guile header dependencies
GUILE_WARN_FLAGS = -Wno-long-long -Wno-error
# -Wno-old-style-cast -Wno-undef
LIBGUILE_HCHK_FLAGS = $(GUILE_CPPFLAGS) $(GUILE_WARN_FLAGS)


libguileutil_la_SOURCES = \
	util/libguile.cc \
	$(GUILE_UTIL_H_FILES)
libguileutil_la_CPPFLAGS = $(GUILE_CPPFLAGS)
libguileutil_la_CXXFLAGS = $(AM_CXXFLAGS) $(GUILE_WARN_FLAGS)
libguileutil_la_LDFLAGS = $(GUILE_LDFLAGS)
# libguileutil_la_LIBADD =

util_progs_readline_toy_SOURCES = util/progs/readline_toy.cc
util_progs_readline_toy_LDADD = $(LIBFANGUTIL)

ALL_HEADER_FILES += $(UTIL_HEADER_FILES)

ALL_TCC_FILES += $(UTIL_TCC_FILES)

util/using_istream.hchk:
	@$(TOUCH) $@

util/using_ostream.hchk:
	@$(TOUCH) $@

# extending default header check
LIBGUILE_HCHK_TEST_SCRIPT =
if HAVE_LIBGUILE
LIBGUILE_HCHK_TEST_SCRIPT += \
	$(ECHO) "Validating $< ... " ; \
	more_hchk_flags="$(LIBGUILE_HCHK_FLAGS)" ; \
	$(H_CHECK_TEST_SCRIPT)
else
LIBGUILE_HCHK_TEST_SCRIPT += \
	$(ECHO) "Skipping $< ... " ; \
	$(TOUCH) $@
endif

util/libguile.hchk: util/libguile.h
	@$(LIBGUILE_HCHK_TEST_SCRIPT)

util/guile_gh.hchk: util/guile_gh.h
	@$(LIBGUILE_HCHK_TEST_SCRIPT)

util/guile_STL.hchk: util/guile_STL.h
	@$(LIBGUILE_HCHK_TEST_SCRIPT)

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

