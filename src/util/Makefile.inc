# "src/Makefile.am"
#	$Id: Makefile.inc,v 1.1.2.2 2006/01/15 22:25:41 fang Exp $

# dirgen is completely bogus, workaround to coerce directory generation
UTILPROGRAMS =
LIBFANGUTIL = libfangutil.la
UTILLIBS = $(LIBFANGUTIL)

# SUBDIRS += util/test

# phony program to generate _build/.../.deps directories on distcheck
PHONYSOURCES += util/STL/dummy.cc util/numeric/dummy.cc util/obsolete/dummy.cc

# template definition header files
UTIL_TCC_FILES = \
	util/STL/list.tcc \
	util/STL/map.tcc \
	util/IO_utils.tcc \
	util/bitset.tcc \
	util/const_assoc_query.tcc \
	util/discrete_interval_set.tcc \
	util/hash_qmap.tcc \
	util/list_vector.tcc \
	util/memory/chunk_map_pool.tcc \
	util/memory/destruction_policy.tcc \
	util/memory/index_pool.tcc \
	util/memory/list_vector_pool.tcc \
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
	util/qmap.tcc \
	util/ring_node.tcc \
	util/sublist.tcc \
	util/unique_list.tcc \
	util/unique_stack.tcc \
	util/what.tcc

# POLICY: every .tcc MUST have a corresponding .h file, 
# but the _fwd.h file is optional.  
UTIL_HEADER_FILES = \
	$(UTIL_TCC_FILES:.tcc=.h) \
	util/macros.h \
	util/STL/allocator_fwd.h \
	util/STL/construct_fwd.h \
	util/STL/deque_fwd.h \
	util/STL/functional_fwd.h \
	util/STL/hash_map_fwd.h \
	util/STL/hash_map.h \
	util/STL/list_fwd.h \
	util/STL/map_fwd.h \
	util/STL/namespace.h \
	util/STL/pair_fwd.h \
	util/STL/reverse_iterator.h \
	util/STL/set_fwd.h \
	util/STL/stack_fwd.h \
	util/STL/valarray_fwd.h \
	util/STL/vector_fwd.h \
	util/IO_utils_fwd.h \
	util/array_traits.h \
	util/assoc_traits_fwd.h \
	util/assoc_traits.h \
	util/attributes.h \
	util/boolean_types.h \
	util/const_traits.h \
	util/cppcat.h \
	util/counter.h \
	util/ctype.h \
	util/dereference.h \
	util/dirent.h \
	util/discrete_interval_set_fwd.h \
	util/finite_vector.h \
	util/FILE_fwd.h \
	util/file_status.h \
	util/getopt_portable.h \
	util/hash_qmap_fwd.h \
	util/hash_specializations.h \
	util/ifthenelse.h \
	util/indent.h \
	util/inttypes.h \
	util/iterator_more.h \
	util/libc.h \
	util/likely.h \
	util/list_vector_fwd.h \
	util/maplikeset.h \
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
	util/nested_iterator.h \
	util/new_functor_fwd.h \
	util/NULL.h \
	util/nullary_function_fwd.h \
	util/nullary_function.h \
	util/null_construct.h \
	util/numeric/bigger_ints.h \
	util/numeric/integer_traits.h \
	util/numeric/inttype_traits.h \
	util/numeric/nibble_tables.h \
	util/offset_array.h \
	util/operators.h \
	util/packed_array_fwd.h \
	util/persistent_fwd.h \
	util/persistent.h \
	util/pthread.h \
	util/qmap_fwd.h \
	util/readline.h \
	util/readline_wrap.h \
	util/reserve.h \
	util/ring_node_fwd.h \
	util/size_t.h \
	util/sstream.h \
	util/stacktrace.h \
	util/static_assert.h \
	util/static_trace.h \
	util/string.h \
	util/string_fwd.h \
	util/template.h \
	util/tokenize_fwd.h \
	util/tokenize.h \
	util/type_traits.h \
	util/using_istream.h \
	util/using_ostream.h \
	util/visitee.h \
	util/what_fwd.h \
	util/wtf.h \
	util/binders.h \
	util/compose.h \
	util/conditional.h \
	util/ptrs_functional.h

UTIL_CC_FILES = \
	util/IO_utils.cc \
	util/bitset.cc \
	util/dirent.cc \
	util/indent.cc \
	util/libc.c \
	util/persistent.cc \
	util/persistent_object_manager.cc \
	util/readline_wrap.cc \
	util/stacktrace.cc \
	util/static_trace.cc \
	util/tokenize.cc \
	util/memory/thread_lock.cc \
	util/memory/ref_count_pool.cc

libfangutil_la_SOURCES = $(UTIL_CC_FILES) $(UTIL_HEADER_FILES) $(UTIL_TCC_FILES)

ALL_HEADER_FILES += $(UTIL_HEADER_FILES)

ALL_TCC_FILES += $(UTIL_TCC_FILES)

util/using_istream.hchk:
	@$(TOUCH) $@

util/using_ostream.hchk:
	@$(TOUCH) $@

LEXYACC_PREFIX_GENERATOR = util/lang/lexyacc/lexyacc-prefix-generator.awk

EXTRA_DIST += \
	util/ChangeLog \
	util/README \
	util/TODO \
	util/STL/README \
	util/lang/README \
	$(LEXYACC_PREFIX_GENERATOR) \
	util/obsolete/README

UTIL_DIRS = util util/numeric util/memory util/obsolete util/STL

CLEANSUBDIRS += $(UTIL_DIRS)

util/.cvsignore: .cvsignore
	$(CP) $? $@
util/STL/.cvsignore: util/.cvsignore
	$(CP) $? $@
util/memory/.cvsignore: util/.cvsignore
	$(CP) $? $@
util/numeric/.cvsignore: util/.cvsignore
	$(CP) $? $@
util/obsolete/.cvsignore: util/.cvsignore
	$(CP) $? $@

util/lang/CX/cx-parse.yy: util/lang/CX/cx-parse.yy.in
	cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@

# files to ignore in subdirectories
# can we generate this from suffix appending?
SUBDIR_CVSIGNORE += \
	util/.cvsignore util/STL/.cvsignore \
	util/memory/.cvsignore util/numeric/.cvsignore \
	util/obsolete/.cvsignore

