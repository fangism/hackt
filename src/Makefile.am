# "src/Makefile.am"
#	$Id: Makefile.am,v 1.73.2.4.2.1 2005/08/13 17:31:51 fang Exp $

# TODO: substitute ARTXX -> HACKT

# .yy for yacc (C++), .ll for lex (C++), 
# .hchk and .tccchk for standalone header checks
SUFFIXES = .yy .ll .ii .test

# standard definitions and macro initializers
# appends to SUFFIXES
include $(srcdir)/Make.hchk-head.am
# includes "Make.stddef"

AUTOMAKE_OPTIONS += subdir-objects

# for recursive make only, all other subdirectories (not listed)
# are managed by this Makefile (flat)
SUBDIRS = util/test .


###################### Are we using yacc or bison? ############################
LEXYACC_PREFIX_SCRIPT = $(srcdir)/parser/lexyacc-prefix.awk

if HAVE_BISON
YACC_UNION_SCRIPT = $(srcdir)/parser/bison-union-type.awk
YACC_OUTPUT_H_SCRIPT = $(srcdir)/parser/bison-output-to-C.awk
else
YACC_UNION_SCRIPT = $(srcdir)/parser/yacc-union-type.awk
YACC_OUTPUT_H_SCRIPT = $(srcdir)/parser/yacc-output-to-C.awk
endif
# Automatically generated sources can just check for YYBISON definition.

########################### main dependencies #################################

bin_PROGRAMS = hackt

# dirgen is completely bogus, workaround to corce directory generation
noinst_PROGRAMS = dirgen getopt_test
# art_persistent_table
noinst_SCRIPTS = art_persistent_table.sh

# convenience libraries
noinst_LTLIBRARIES = libhackt.la libhacktparsers.la libbogus.la libfangutil.la \
	libhacktexec.la

# need to list auto-generated header first to guarantee correct build
# order in parallel builds.  

hackt_SOURCES = main/hackt.cc
hackt_LDADD = libhacktexec.la libhackt.la libfangutil.la

getopt_test_SOURCES = test/getopt_test.cc

# This says, "always build me first."
# trying NOT to abuse this, writing explicit dependencies below
BUILT_SOURCES = parser/art++-parse.output.h

# phony program to generate _build/*/.deps directories on distcheck
PHONYSOURCES = util/STL/dummy.cc util/numeric/dummy.cc util/obsolete/dummy.cc \
	dummy.cc
dirgen_SOURCES = $(PHONYSOURCES) test/header_test.cc

# these two targets cause automake to create the corresponding _build
# subdirectories, that are needed for make distcheck, for check-headers
dummy.cc:
	$(TOUCH) $@

util/STL/dummy.cc:
	$(TOUCH) $@

util/numeric/dummy.cc:
	$(TOUCH) $@

util/obsolete/dummy.cc:
	$(TOUCH) $@

ARTXX_PARSE_ROOT = art++-parse
ARTXX_LEX_ROOT = art++-lex

# libbogus.la is not actually used, hence the name.
# force use of lex/yacc suffix rules to generate the .c and .cc files
# the source files are post-processed via awk (below) into the real sources, 
# with symbol names with transformed prefixes.  
libbogus_la_SOURCES = \
	lexer/art++-lex.ll \
	parser/art++-parse.yy

# automake doesn't like macro in generating .Plo, a bug methinks
#	lexer/$(ARTXX_LEX_ROOT).ll \
#	parser/$(ARTXX_PARSE_ROOT).yy

AST_CC_FILES = \
	AST/art_parser_base.cc \
	AST/art_parser_node_position.cc \
	AST/art_parser_expr.cc \
	AST/art_parser_range.cc \
	AST/art_parser_token.cc \
	AST/art_parser_definition.cc \
	AST/art_parser_instance.cc \
	AST/art_parser_formal.cc \
	AST/art_parser_chp.cc \
	AST/art_parser_hse.cc \
	AST/art_parser_prs.cc \
	AST/parse_context.cc

AST_TCC_FILES = \
	AST/art_parser_node_list.tcc \
	AST/parse_context.tcc

AST_H_FILES = $(AST_CC_FILES:.cc=.h) \
	$(AST_TCC_FILES:.tcc=.h) \
	AST/art_parser_fwd.h \
	AST/art_parser_chp_fwd.h \
	AST/art_parser_hse_fwd.h \
	AST/art_parser_prs_fwd.h \
	AST/art_parser_delim.h \
	AST/art_parser.h \
	AST/art_parser_node_list.h \
	AST/art_parser_definition_item.h \
	AST/art_parser_expr_base.h \
	AST/art_parser_expr_list.h \
	AST/art_parser_identifier.h \
	AST/art_parser_range_list.h \
	AST/art_parser_root.h \
	AST/art_parser_root_item.h \
	AST/art_parser_statement.h \
	AST/art_parser_terminal.h \
	AST/art_parser_token_char.h \
	AST/art_parser_token_string.h \
	AST/art_parser_type.h \
	AST/art_parser_type_base.h

# all of these .cc files have a corresponding .h file
OBJECT_CC_FILES = \
	Object/common/object_base.cc \
	Object/common/namespace.cc \
	Object/common/extern_templates.cc \
	Object/traits/class_traits.cc \
	Object/inst/substructure_alias_base.cc \
	Object/inst/subinstance_manager.cc \
	Object/inst/alias_actuals.cc \
	Object/inst/instance_collection.cc \
	Object/inst/param_value_collection.cc \
	Object/inst/pbool_value_collection.cc \
	Object/inst/pint_value_collection.cc \
	Object/inst/bool_instance_collection.cc \
	Object/inst/int_instance_collection.cc \
	Object/inst/enum_instance_collection.cc \
	Object/inst/struct_instance_collection.cc \
	Object/inst/process_instance_collection.cc \
	Object/inst/channel_instance_collection.cc \
	Object/unroll/conditional_scope.cc \
	Object/unroll/expression_assignment.cc \
	Object/unroll/instance_management_base.cc \
	Object/unroll/instantiation_statement.cc \
	Object/unroll/loop_scope.cc \
	Object/unroll/meta_instance_reference_connection.cc \
	Object/unroll/unroll_context.cc \
	Object/module.cc \
	Object/state_manager.cc \
	Object/type/template_actuals.cc \
	Object/type/canonical_type_base.cc \
	Object/type/canonical_type.cc \
	Object/def/template_formals_manager.cc \
	Object/def/port_formals_manager.cc \
	Object/def/footprint.cc \
	Object/def/footprint_manager.cc \
	Object/lang/PRS.cc \
	Object/lang/CHP.cc

# these source files have no .h file (use other name)
OBJECT_CC_NO_H_FILES = \
	Object/traits/class_traits_types.cc \
	Object/def/definition.cc \
	Object/expr/basic_param.cc \
	Object/expr/param_const_collection.cc \
	Object/expr/meta_index_expr_list.cc \
	Object/expr/meta_param_expr_list.cc \
	Object/expr/meta_range.cc \
	Object/expr/meta_range_list.cc \
	Object/expr/meta_param_value_reference.cc \
	Object/expr/nonmeta_param_value_reference.cc \
	Object/expr/operators.cc \
	Object/expr/data_expr.cc \
	Object/type/type_reference.cc \
	Object/ref/instance_reference.cc \
	Object/ref/instance_reference_datatype.cc

# these header files have source (.cc) files with different names
OBJECT_TCC_FILES = \
	Object/expr/const_collection.tcc \
	Object/inst/general_collection_type_manager.tcc \
	Object/inst/null_collection_type_manager.tcc \
	Object/inst/int_collection_type_manager.tcc \
	Object/inst/parameterless_collection_type_manager.tcc \
	Object/inst/subinstance_manager.tcc \
	Object/inst/alias_actuals.tcc \
	Object/inst/instance_collection.tcc \
	Object/inst/value_collection.tcc \
	Object/inst/instance_pool.tcc \
	Object/inst/state_instance.tcc \
	Object/ref/simple_meta_instance_reference.tcc \
	Object/ref/simple_nonmeta_instance_reference.tcc \
	Object/ref/member_meta_instance_reference.tcc \
	Object/ref/simple_meta_value_reference.tcc \
	Object/ref/simple_nonmeta_value_reference.tcc \
	Object/type/canonical_type.tcc \
	Object/unroll/alias_connection.tcc \
	Object/unroll/expression_assignment.tcc \
	Object/unroll/instantiation_statement.tcc \
	Object/unroll/param_instantiation_statement.tcc \
	Object/unroll/unroll_context.tcc \
	Object/module.tcc \
	Object/common/namespace.tcc \
	Object/lang/CHP.tcc

OBJECT_H_FILES = $(OBJECT_CC_FILES:.cc=.h) \
	Object/art_object_fwd.h \
	Object/persistent_type_hash.h \
	Object/common/multikey_index.h \
	Object/common/util_types.h \
	Object/common/scopespace.h \
	Object/traits/class_traits_fwd.h \
	Object/traits/classification_tags.h \
	Object/traits/chan_traits.h \
	Object/traits/data_traits.h \
	Object/traits/proc_traits.h \
	Object/traits/param_traits.h \
	Object/traits/pint_traits.h \
	Object/traits/pbool_traits.h \
	Object/traits/int_traits.h \
	Object/traits/bool_traits.h \
	Object/traits/enum_traits.h \
	Object/traits/struct_traits.h \
	Object/def/built_in_datatype_def.h \
	Object/def/channel_definition_alias.h \
	Object/def/channel_definition_base.h \
	Object/def/datatype_definition_alias.h \
	Object/def/datatype_definition_base.h \
	Object/def/definition_base.h \
	Object/def/enum_datatype_def.h \
	Object/def/param_definition.h \
	Object/def/process_definition.h \
	Object/def/process_definition_alias.h \
	Object/def/process_definition_base.h \
	Object/def/typedef_base.h \
	Object/def/user_def_chan.h \
	Object/def/user_def_datatype.h \
	Object/expr/bool_expr.h \
	Object/expr/bool_logical_expr.h \
	Object/expr/bool_negation_expr.h \
	Object/expr/const_collection.h \
	Object/expr/const_index.h \
	Object/expr/const_index_list.h \
	Object/expr/const_param.h \
	Object/expr/const_param_expr_list.h \
	Object/expr/const_range.h \
	Object/expr/const_range_list.h \
	Object/expr/data_expr.h \
	Object/expr/dynamic_meta_index_list.h \
	Object/expr/dynamic_meta_range_list.h \
	Object/expr/dynamic_param_expr_list.h \
	Object/expr/enum_expr.h \
	Object/expr/int_arith_expr.h \
	Object/expr/int_expr.h \
	Object/expr/int_negation_expr.h \
	Object/expr/int_range_expr.h \
	Object/expr/int_range_list.h \
	Object/expr/int_relational_expr.h \
	Object/expr/meta_index_expr.h \
	Object/expr/meta_index_list.h \
	Object/expr/meta_range_expr.h \
	Object/expr/meta_range_list.h \
	Object/expr/nonmeta_index_expr_base.h \
	Object/expr/nonmeta_index_list.h \
	Object/expr/nonmeta_range_expr_base.h \
	Object/expr/param_expr.h \
	Object/expr/param_expr_list.h \
	Object/expr/pbool_const.h \
	Object/expr/pbool_expr.h \
	Object/expr/pbool_logical_expr.h \
	Object/expr/pbool_unary_expr.h \
	Object/expr/pint_arith_expr.h \
	Object/expr/pint_const.h \
	Object/expr/pint_expr.h \
	Object/expr/pint_range.h \
	Object/expr/pint_relational_expr.h \
	Object/expr/pint_unary_expr.h \
	Object/expr/struct_expr.h \
	Object/expr/types.h \
	Object/inst/substructure_alias_fwd.h \
	Object/inst/general_collection_type_manager.h \
	Object/inst/null_collection_type_manager.h \
	Object/inst/int_collection_type_manager.h \
	Object/inst/parameterless_collection_type_manager.h \
	Object/inst/instance_collection_base.h \
	Object/inst/physical_instance_collection.h \
	Object/inst/datatype_instance_collection.h \
	Object/inst/instance_alias_info.h \
	Object/inst/instance_alias.h \
	Object/inst/alias_empty.h \
	Object/inst/value_collection.h \
	Object/inst/instance_pool_fwd.h \
	Object/inst/instance_pool.h \
	Object/inst/state_instance.h \
	Object/inst/bool_instance.h \
	Object/inst/int_instance.h \
	Object/inst/enum_instance.h \
	Object/inst/struct_instance.h \
	Object/inst/process_instance.h \
	Object/inst/channel_instance.h \
	Object/ref/member_meta_instance_reference.h \
	Object/ref/meta_instance_reference_base.h \
	Object/ref/meta_instance_reference_subtypes.h \
	Object/ref/nonmeta_instance_reference_base.h \
	Object/ref/nonmeta_instance_reference_subtypes.h \
	Object/ref/simple_datatype_meta_instance_reference_base.h \
	Object/ref/simple_datatype_nonmeta_value_reference.h \
	Object/ref/simple_meta_instance_reference.h \
	Object/ref/simple_meta_instance_reference_base.h \
	Object/ref/simple_meta_value_reference.h \
	Object/ref/simple_nonmeta_instance_reference.h \
	Object/ref/simple_nonmeta_instance_reference_base.h \
	Object/ref/simple_nonmeta_value_reference.h \
	Object/ref/simple_param_meta_value_reference.h \
	Object/ref/inst_ref_implementation_fwd.h \
	Object/ref/inst_ref_implementation.h \
	Object/type/builtin_channel_type_reference.h \
	Object/type/channel_type_reference.h \
	Object/type/channel_type_reference_base.h \
	Object/type/data_type_reference.h \
	Object/type/fundamental_type_reference.h \
	Object/type/param_type_reference.h \
	Object/type/process_type_reference.h \
	Object/type/type_reference_base.h \
	Object/type/canonical_type_fwd.h \
	Object/unroll/alias_connection.h \
	Object/unroll/aliases_connection_base.h \
	Object/unroll/channel_instantiation_statement.h \
	Object/unroll/data_alias_connection_base.h \
	Object/unroll/datatype_instantiation_statement.h \
	Object/unroll/instance_management_base.h \
	Object/unroll/instantiation_statement_base.h \
	Object/unroll/instantiation_statement_type_ref_default.h \
	Object/unroll/param_expression_assignment.h \
	Object/unroll/param_instantiation_statement.h \
	Object/unroll/param_instantiation_statement_base.h \
	Object/unroll/port_connection.h \
	Object/unroll/process_instantiation_statement.h \
	Object/unroll/sequential_scope.h \
	Object/lang/PRS_base.h \
	Object/lang/CHP_base.h

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
	util/memory/list_vector_pool.tcc \
	util/memory/count_ptr.tcc \
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
	util/boolean_types.h \
	util/const_traits.h \
	util/cppcat.h \
	util/dereference.h \
	util/discrete_interval_set_fwd.h \
	util/getopt_portable.h \
	util/hash_qmap_fwd.h \
	util/hash_specializations.h \
	util/ifthenelse.h \
	util/indent.h \
	util/inttypes.h \
	util/iterator_more.h \
	util/likely.h \
	util/list_vector_fwd.h \
	util/maplikeset.h \
	util/memory/chunk_map_pool_fwd.h \
	util/memory/list_vector_pool_fwd.h \
	util/memory/pointer_classes_fwd.h \
	util/memory/pointer_classes.h \
	util/memory/pointer_traits.h \
	util/memory/pointer_manipulator.h \
	util/memory/excl_ptr.h \
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
	util/nullary_function_fwd.h \
	util/nullary_function.h \
	util/null_construct.h \
	util/numeric/bigger_ints.h \
	util/numeric/integer_traits.h \
	util/numeric/inttype_traits.h \
	util/numeric/nibble_tables.h \
	util/operators.h \
	util/packed_array_fwd.h \
	util/persistent_fwd.h \
	util/persistent.h \
	util/qmap_fwd.h \
	util/reserve.h \
	util/ring_node_fwd.h \
	util/size_t.h \
	util/sstream.h \
	util/stacktrace.h \
	util/static_assert.h \
	util/static_trace.h \
	util/string_fwd.h \
	util/type_traits.h \
	util/using_istream.h \
	util/using_ostream.h \
	util/what_fwd.h \
	util/wtf.h \
	util/binders.h \
	util/compose.h \
	util/conditional.h \
	util/ptrs_functional.h \
	util/obsolete/ptrs.h \
	util/obsolete/ptrs_fwd.h \
	util/obsolete/count_ptr.h \
	util/obsolete/count_ptr_fwd.h

UTIL_CC_FILES = \
	util/IO_utils.cc \
	util/bitset.cc \
	util/indent.cc \
	util/persistent.cc \
	util/persistent_object_manager.cc \
	util/stacktrace.cc \
	util/static_trace.cc \
	util/memory/thread_lock.cc \
	util/memory/ref_count_pool.cc

PARSER_H_FILES = \
	parser/$(ARTXX_PARSE_ROOT)-prefix.h \
	parser/art++-union.h \
	parser/$(ARTXX_PARSE_ROOT).output.h \
	parser/$(ARTXX_PARSE_ROOT)-options.h \
	lexer/$(ARTXX_LEX_ROOT)-options.h \
	lexer/art_lex.h

PARSER_CC_FILES = \
	parser/art++-parse-prefix.cc \
	parser/art++-union.cc \
	lexer/art++-lex-prefix.cc \
	lexer/art_lex.cc

# libhacktexec sources
MAIN_EXEC_CC_FILES = \
	main/force_load.cc \
	main/program_registry.cc \
	main/main_funcs.cc \
	main/parse_test.cc \
	main/compile.cc \
	main/unroll.cc \
	main/create.cc \
	main/prsobjdemo.cc \
	main/dump_persistent_table.cc \
	main/objdump.cc

MAIN_EXEC_H_FILES = $(MAIN_EXEC_CC_FILES:.cc=.h) \
	main/hackt.h \
	main/hackt_fwd.h

COMMON_FILES = \
	common/ICE.cc \
	common/ICE.h \
	common/TODO.h

# automake doesn't like macro in generating .Plo, a bug methinks
#	parser/$(ARTXX_PARSE_ROOT)-prefix.cc
#	lexer/$(ARTXX_LEX_ROOT)-prefix.cc

# the generated lexer and parser sources aren't supposed to be distributed!
libhackt_la_SOURCES = \
	$(AST_CC_FILES) $(AST_TCC_FILES) $(AST_H_FILES) \
	$(OBJECT_CC_FILES) $(OBJECT_TCC_FILES) $(OBJECT_H_FILES) \
	$(OBJECT_CC_NO_H_FILES) \
	$(COMMON_FILES) \
	config.h

libhacktexec_la_SOURCES = $(MAIN_EXEC_CC_FILES) $(MAIN_EXEC_H_FILES)

# any other files that depend on automatically generated headers
# should have explicit dependencies.  
main/main_funcs.cc: parser/$(ARTXX_PARSE_ROOT)-prefix.h
lexer/$(ARTXX_LEX_ROOT).cc: parser/$(ARTXX_PARSE_ROOT).h
parser/art++-union.lo: parser/$(ARTXX_PARSE_ROOT).output.h
parser/art++-union.$(OBJEXT): parser/$(ARTXX_PARSE_ROOT).output.h

# the following targets need to be prefixed with the library name, libbogus, 
# but automake complains that it overrides the automatic definition...
# parser/$(ARTXX_PARSE_ROOT).lo: parser/$(ARTXX_PARSE_ROOT).output.h
# parser/$(ARTXX_PARSE_ROOT).$(OBJEXT): parser/$(ARTXX_PARSE_ROOT).output.h
# this creates cyclic dependencies (bad)
# parser/$(ARTXX_PARSE_ROOT).cc: parser/$(ARTXX_PARSE_ROOT).output.h

# build up main library from pieces
# Here, libhackt will include all of libhacktparsers
libhackt_la_LIBADD = libhacktparsers.la

# compile lexer and parser into separate library to for convenient way
# of giving them exceptional compiler flags, see libhacktparsers_la_CXXFLAGS
libhacktparsers_la_SOURCES = \
	$(PARSER_CC_FILES) $(PARSER_H_FILES)

libfangutil_la_SOURCES = $(UTIL_CC_FILES) $(UTIL_HEADER_FILES) $(UTIL_TCC_FILES)

ALL_HEADER_FILES += \
	$(UTIL_HEADER_FILES) \
	$(OBJECT_H_FILES) \
	$(AST_H_FILES) \
	$(PARSER_H_FILES) \
	config.h \
	main/main_funcs.h

ALL_TCC_FILES += \
	$(UTIL_TCC_FILES) \
	$(OBJECT_TCC_FILES) \
	$(AST_TCC_FILES)

# note: Object/art_object_IO.cc only contains explicit template instantiations

# don't actually need -l[f]l
# libhackt_la_LDADD = @LEXLIB@

ARTXX_YACC_PREFIX = artxx_

# rules for the prefix-transformed versions of generated lexer/parser sources
# make these conditional on yacc vs. bison
if HAVE_BISON
parser/$(ARTXX_PARSE_ROOT)-prefix.cc: parser/$(ARTXX_PARSE_ROOT).cc \
		parser/$(ARTXX_PARSE_ROOT).output.h
	$(AWK) -f $(srcdir)/parser/try_lex_catch.awk \
		parser/$(ARTXX_PARSE_ROOT).cc | \
	$(AWK) -f $(srcdir)/parser/yyerror_bison_hack.awk | \
	$(AWK) -f $(LEXYACC_PREFIX_SCRIPT) \
		-v PREFIX=$(ARTXX_YACC_PREFIX) > $@
else
parser/$(ARTXX_PARSE_ROOT)-prefix.cc: parser/$(ARTXX_PARSE_ROOT).cc \
		parser/$(ARTXX_PARSE_ROOT).output.h
	$(AWK) -f $(srcdir)/parser/try_lex_catch.awk \
		parser/$(ARTXX_PARSE_ROOT).cc | \
	$(AWK) -f $(LEXYACC_PREFIX_SCRIPT) \
		-v PREFIX=$(ARTXX_YACC_PREFIX) > $@
endif

parser/$(ARTXX_PARSE_ROOT)-prefix.h: parser/$(ARTXX_PARSE_ROOT).h
	$(ECHO) "#include \"parser/art++-union.h\"" > $@
	$(AWK) -f $(LEXYACC_PREFIX_SCRIPT) \
		-v PREFIX=$(ARTXX_YACC_PREFIX) $? >> $@

lexer/$(ARTXX_LEX_ROOT)-prefix.cc: lexer/$(ARTXX_LEX_ROOT).cc \
		parser/$(ARTXX_PARSE_ROOT)-prefix.h
	$(AWK) -f $(LEXYACC_PREFIX_SCRIPT) \
		-v PREFIX=$(ARTXX_YACC_PREFIX) \
			lexer/$(ARTXX_LEX_ROOT).cc > $@

# can't depend on the .output alone, not a known dependence with .y[y]
ARTXX_PARSE_OUTPUT = parser/$(ARTXX_PARSE_ROOT).output
$(ARTXX_PARSE_OUTPUT): parser/$(ARTXX_PARSE_ROOT).h

$(ARTXX_PARSE_OUTPUT).h: $(ARTXX_PARSE_OUTPUT)
	@if $(GREP) "conflict" $?; then exit 1; fi
	$(AWK) -f $(YACC_OUTPUT_H_SCRIPT) \
		-f $(srcdir)/parser/common-output-to-C.awk $? | \
	$(AWK) -f $(LEXYACC_PREFIX_SCRIPT) \
		-v PREFIX=$(ARTXX_YACC_PREFIX) > $@

# NOTE: CPP here should be cpp, not gcc -E, as set by autoconf
# gcc -E ignores file that require no preprocessing

CPP_TRADITIONAL = @CPP_TRADITIONAL@

parser/art++.types: parser/$(ARTXX_PARSE_ROOT).yy
	$(CAT) $? | $(GREP) -v "#include" | $(CPP_TRADITIONAL) -P | \
		$(GREP) -v pragma | \
		$(SED) -e "/^%start/,$$$$d" -e "/%{/,/%}/d" | \
		$(SED) '/^[ \t]*$$/d' > $@

# extract num: first matched line, 3rd space-delimited field (#define foo bar)
# "cut" is not a good idea, if # define is separated... :(
# the last awk pipe eliminate extraneous whitespace not filtered by sed
GET_START_TOKEN_ENUM = \
	$(GREP) "define.*MINIMUM_BOGOSITY" parser/$(ARTXX_PARSE_ROOT).h | \
		$(HEAD) -n1 | $(SED) 's/^.*MINIMUM_BOGOSITY[ \t]*//g' | \
		$(AWK) '/.*/{printf($$1);}'


YACC_UNION_SCRIPT_ARGS = -v yaccfile=parser/art++.types \
		-v include="AST/art_parser.h parser/$(ARTXX_PARSE_ROOT)-prefix.h" \
		-v namespace=ART::parser \
		-v token_enum=`$(GET_START_TOKEN_ENUM)`
# so far, yacc token enums start at 257 consistently, but we take no chances.

parser/art++-union.cc: $(ARTXX_PARSE_OUTPUT) parser/art++.types parser/$(ARTXX_PARSE_ROOT)-prefix.h
	@if ! [ -s parser/art++.types ] ; then \
		$(ECHO) "parser/art++.types is empty!" ; \
		exit 1 ; \
	fi
	$(AWK) -f $(srcdir)/parser/common-union-type.awk \
		-f $(YACC_UNION_SCRIPT) $(YACC_UNION_SCRIPT_ARGS) \
		$(ARTXX_PARSE_OUTPUT) > $@


# AM_CPPFLAGS = -I$(srcdir)/parser -I$(srcdir)/util -I$(srcdir)/AST \
#	-I$(srcdir)/lexer -I$(srcdir)/Object -Iparser
AM_CPPFLAGS = -I$(srcdir)
# will need _build directories for "make distcheck" with auto-generated headers
# the last -Iparser is for the relative path in the build directory

# extremely anal-retentive
GENERAL_WARN_FLAGS = -W -Wall -Wundef -Wshadow -Wno-unused -Wpointer-arith \
	-Wcast-qual -Wcast-align -Wconversion -Werror
WARN_CFLAGS = -Wmissing-prototypes -Wstrict-prototypes \
	-Wbad-function-cast -Wnested-externs
WARN_CXXFLAGS = -Wold-style-cast -Woverloaded-virtual
# -Wsign-promo
DIALECT_FLAGS = -ansi -pedantic-errors
# -pedantic		# kills some pointer class conversions, fix later...
MORE_CFLAGS = -pipe $(DIALECT_FLAGS) $(WARN_CFLAGS) $(GENERAL_WARN_FLAGS)
MORE_CXXFLAGS = -pipe $(DIALECT_FLAGS) $(WARN_CXXFLAGS) $(GENERAL_WARN_FLAGS)
# These flags are used to cancel out previous warning flags, 
# such exceptions are needed for the automatic lexers and parsers
PARSER_NOWARN_FLAGS = -Wno-old-style-cast -Wno-shadow

AM_CFLAGS = $(MORE_CFLAGS)
AM_CXXFLAGS = $(MORE_CXXFLAGS)

AM_YFLAGS = -d -t -v
AM_LFLAGS = 

# for occasional debugging
SAVETEMP = $(CXXCOMPILE) -v -save-temps
.cc.ii:
	$(SAVETEMP) -c $< -o $*.o

# doesn't work
# YACC_PREFIX = -DYACC_PREFIX=artxx_yy
# libhackt_la_CPPFLAGS = $(YACC_PREFIX) $(AM_CPPFLAGS)

# *COUGH* *HACK*:
# NO_FANCY_YYERROR tells lexer/art++-lex.cc to use un-transfomed symbol names
libbogus_la_CPPFLAGS = -DNO_FANCY_YYERROR $(AM_CPPFLAGS)
libbogus_la_CXXFLAGS = $(AM_CXXFLAGS) $(PARSER_NOWARN_FLAGS)
# here we give the exceptional flags to the lexer and parser, 
# which are generated automatically by yacc/bison/[f]lex for 
# traditional C compilers.  (Not the cleanest code ever generated.)
libhacktparsers_la_CXXFLAGS = $(AM_CXXFLAGS) $(PARSER_NOWARN_FLAGS)

# artc_CPPFLAGS = $(AM_CPPFLAGS)
# art__2obj_CPPFLAGS = $(AM_CPPFLAGS)
# artobjdump_CPPFLAGS = $(AM_CPPFLAGS)

# is required to be set for Make.hchk-tail.am
EMPTY_SOURCE = $(srcdir)/test/header_test.cc

# there are a few headers we want exempt from stand-alone testing
# what about their dependencies?  
# automake produces empty placeholders (never updated) for them initially, 
# so make will not die.

util/using_istream.hchk:
	@$(TOUCH) $@

util/using_ostream.hchk:
	@$(TOUCH) $@

../config.hchk:
	@$(TOUCH) $@

EXTRA_DIST += local.cvsignore \
	parser/local.cvsignore \
	parser/lexyacc-prefix.awk \
	parser/common-output-to-C.awk \
	parser/yacc-output-to-C.awk \
	parser/bison-output-to-C.awk \
	parser/common-union-type.awk \
	parser/yacc-union-type.awk \
	parser/bison-union-type.awk \
	parser/yyerror_bison_hack.awk \
	parser/try_lex_catch.awk \
	parser/NOTES \
	lexer/local.cvsignore \
	test/local.cvsignore \
	test/header_test.cc \
	test/getopt_test.sh \
	test/getopt_test.stderr \
	common/README \
	util/ChangeLog \
	util/README \
	util/TODO \
	util/STL/README \
	util/obsolete/README \
	scripts/filter_namespaces.sed \
	Object/README \
	Object/common/README \
	Object/def/README \
	Object/expr/README \
	Object/inst/README \
	Object/lang/README \
	Object/traits/README \
	Object/type/README \
	Object/ref/README \
	Object/unroll/README \
	ChangeLog \
	README \
	NOTES \
	wc-l

TESTS = art_persistent_table.sh test/getopt_test.test
# test/getopt_test.test
DISTCLEANFILES += art_persistent_table.sh
AUTO_IGNORE += $(TESTS) $(PROGRAMS) $(LTLIBRARIES)

test/getopt_test.test: getopt_test test/getopt_test.sh
	@$(ECHO) "#!$(SHELL)" > $@
	@$(ECHO) "$(srcdir)/$*.sh 2>&1 | cat > $*.output" >> $@
	@$(ECHO) "$(DIFF) -u $(srcdir)/$*.stderr $*.output > $*.diff" >> $@
	@$(ECHO) "if [ -s $*.diff ]" >> $@
	@$(ECHO) then >> $@
		@$(ECHO) "$(ECHO) \"libc getopt behavior differs from expectation!\"" >> $@
		@$(ECHO) "$(CAT) $*.diff" >> $@
		@$(ECHO) "exit 1" >> $@
	@$(ECHO) fi >> $@
	@$(CHMOD) +x $@

DISTCLEANFILES += test/getopt_test.test \
	test/getopt_test.diff \
	test/getopt_test.output

art_persistent_table.sh: hackt
	@$(ECHO) "#!/bin/sh" > $@
	@$(ECHO) "hackt dump_persistent_table" >> $@
	@$(CHMOD) +x $@

# choosing not to distribute lex/yacc-generated sources for now
CLEANPARSER = \
	parser/$(ARTXX_PARSE_ROOT).cc \
	parser/$(ARTXX_PARSE_ROOT).h \
	parser/$(ARTXX_PARSE_ROOT)-prefix.cc \
	parser/$(ARTXX_PARSE_ROOT)-prefix.h \
	parser/$(ARTXX_PARSE_ROOT).output \
	parser/$(ARTXX_PARSE_ROOT).output.h \
	parser/art++-union.cc \
	parser/art++.types

CLEANLEXER = \
	lexer/$(ARTXX_LEX_ROOT)-prefix.cc \
	lexer/$(ARTXX_LEX_ROOT).cc


DISTCLEANFILES += $(CLEANPARSER) $(CLEANLEXER)

OBJECT_DIRS = Object Object/common Object/expr Object/def Object/inst \
	Object/lang Object/ref Object/traits Object/type Object/unroll

CLEANSUBDIRS = lexer parser AST $(OBJECT_DIRS) \
	util util/numeric util/memory util/obsolete util/STL

clean-parser:
	-$(RM) $(CLEANPARSER)
	-$(RM) parser/*.$(OBJEXT) parser/*.lo

clean-lexer:
	-$(RM) $(CLEANLEXER)
	-$(RM) lexer/*.$(OBJEXT) lexer/*.lo

clean-AST:
	-$(RM) AST/*.$(OBJEXT)
	-$(RM) AST/*.lo

clean-Object:
	for d in $(OBJECT_DIRS) ; do \
		$(RM) $$d/*.$(OBJEXT) ; \
		$(RM) $$d/*.lo ; \
	done

# implies CLEANFILES
clean-local: clean-AST clean-Object clean-lexer clean-parser clean-header-tests
	for d in $(CLEANSUBDIRS); do \
		$(RM) $$d/*.loT ; \
	done

# cvsignore targets
lexer/.cvsignore: .cvsignore lexer/local.cvsignore
	$(CAT) .cvsignore $(srcdir)/lexer/local.cvsignore > $@
parser/.cvsignore: .cvsignore parser/local.cvsignore
	$(CAT) .cvsignore $(srcdir)/parser/local.cvsignore > $@
common/.cvsignore: .cvsignore
	$(CP) $? $@
Object/.cvsignore: .cvsignore
	$(CP) $? $@
Object/common/.cvsignore: .cvsignore
	$(CP) $? $@
Object/def/.cvsignore: .cvsignore
	$(CP) $? $@
Object/expr/.cvsignore: .cvsignore
	$(CP) $? $@
Object/inst/.cvsignore: .cvsignore
	$(CP) $? $@
Object/lang/.cvsignore: .cvsignore
	$(CP) $? $@
Object/ref/.cvsignore: .cvsignore
	$(CP) $? $@
Object/traits/.cvsignore: .cvsignore
	$(CP) $? $@
Object/type/.cvsignore: .cvsignore
	$(CP) $? $@
Object/unroll/.cvsignore: .cvsignore
	$(CP) $? $@
AST/.cvsignore: .cvsignore
	$(CP) $? $@
main/.cvsignore: .cvsignore
	$(CP) $? $@
util/.cvsignore: .cvsignore
	$(CP) $? $@
util/STL/.cvsignore: .cvsignore
	$(CP) $? $@
util/memory/.cvsignore: .cvsignore
	$(CP) $? $@
util/numeric/.cvsignore: .cvsignore
	$(CP) $? $@
util/obsolete/.cvsignore: .cvsignore
	$(CP) $? $@
test/.cvsignore: .cvsignore test/local.cvsignore
	@$(CAT) .cvsignore $(srcdir)/test/local.cvsignore > $@

SUBDIR_CVSIGNORE = lexer/.cvsignore parser/.cvsignore \
	common/.cvsignore \
	Object/.cvsignore Object/common/.cvsignore \
	Object/def/.cvsignore Object/expr/.cvsignore \
	Object/inst/.cvsignore Object/lang/.cvsignore Object/ref/.cvsignore \
	Object/traits/.cvsignore Object/type/.cvsignore \
	Object/unroll/.cvsignore \
	AST/.cvsignore main/.cvsignore \
	util/.cvsignore util/STL/.cvsignore \
	util/memory/.cvsignore util/numeric/.cvsignore \
	util/obsolete/.cvsignore \
	test/.cvsignore

all: cvsignore

cvsignore: $(SUBDIR_CVSIGNORE)

clean-cvsignore: clean-cvsignore-subdirs

clean-cvsignore-subdirs:
	-$(RM) $(SUBDIR_CVSIGNORE)

DISTCLEANFILES += $(SUBDIR_CVSIGNORE)
DISTCLEANDIRS += $(DEPDIR)

dist-hook:
	for f in $(DISTCLEANFILES) $(CLEANFILES); do \
		$(RM) $(distdir)/$$f ; \
	done

# standard targets
include $(srcdir)/Make.hchk-tail.am
# includes "Make.global"

