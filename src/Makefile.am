# "src/Makefile.am"
#	$Id: Makefile.am,v 1.161.2.3 2007/03/11 05:13:52 fang Exp $

# .yy for yacc (C++), .ll for lex (C++), 
# .hchk and .tccchk for standalone header checks
SUFFIXES = .yy .ll .ii .test .i .hii. .hhii .tccii

# standard definitions and macro initializers
# appends to SUFFIXES
include $(srcdir)/Make.hchk-head.am
# includes "Make.stddef"

AUTOMAKE_OPTIONS += subdir-objects

# for recursive make only, all other subdirectories (not listed)
# are managed by this Makefile (flat)
SUBDIRS = util/test .

# cumulative targets
BUILT_SOURCES =
PHONYSOURCES =
CLEANSUBDIRS =
SUBDIR_CVSIGNORE =

# for optional fun features
include $(srcdir)/misc/Makefile.inc
# for util subdirectory and library
include $(srcdir)/util/Makefile.inc

###################### Are we using yacc or bison? ############################
LEXYACC_PREFIX_SCRIPT = $(srcdir)/parser/lexyacc-prefix.awk

if HAVE_BISON
YACC_UNION_SCRIPT = $(srcdir)/parser/bison-union-type.awk
YACC_OUTPUT_H_SCRIPT = $(srcdir)/parser/bison-output-to-C.awk
endif
if HAVE_BYACC
YACC_UNION_SCRIPT = $(srcdir)/parser/yacc-union-type.awk
YACC_OUTPUT_H_SCRIPT = $(srcdir)/parser/yacc-output-to-C.awk
endif
if HAVE_YACC
YACC_UNION_SCRIPT = $(srcdir)/parser/yacc-union-type.awk
YACC_OUTPUT_H_SCRIPT = $(srcdir)/parser/yacc-output-to-C.awk
endif
# Automatically generated sources can just check for YYBISON definition.

########################### main dependencies #################################

bin_PROGRAMS = hackt haco haccreate hacalloc \
	hacobjdump hflat hacpp hacprsim hacchpsim
if USE_UNROLL_PHASE
bin_PROGRAMS += hacunroll
endif

CLEANFILES += $(bin_SCRIPTS)

# dirgen is completely bogus, workaround to coerce directory generation
noinst_PROGRAMS = dirgen getopt_test $(UTILPROGRAMS) $(FUNPROGRAMS)
noinst_SCRIPTS = hackt_persistent_table.sh

# convenience libraries
# want to install libhackt.la next... but causes undefined symbols...
noinst_LTLIBRARIES = libhackt.la \
	$(UTILLIBS) libhacktexec.la libhacktinstref.la \
	libhacktcommon.la $(FUNLIBS) \
	libbogus.la libbogus2.la libbogus3.la libbogus4.la

pkglib_LTLIBRARIES = libhacktlib.la libhacktsim.la \
	libhacktparsers.la libhacktast.la
# lib_LTLIBRARIES = libhacktlib.la

hackt_SOURCES = main/hackt.cc
hackt_LDADD = libhacktexec.la
#	libhacktexec.la libhacktsim.la libhacktparsers.la \
#	libhacktast.la libhackt.la libhacktinstref.la libhacktcommon.la \
#	$(LIBFANGUTIL)

# no need to use $(DESTDIR) (typically used for a staged install)
# installcheck is never run on the staged install, only on the final

haco_SOURCES = main/compile.cc
haco_CXXFLAGS = -DWITH_MAIN
haco_LDADD = libhacktlib.la
# goal: just libhacktparsers.la

# haco: $(top_builddir)/config.status
#	@{ $(ECHO) "#!$(SH_PATH)" ; \
#	$(ECHO) "exec $(bindir)/`$(INSTALL_TRANSFORM_SH) hackt` compile \"$$""@\"" ;} > $@ ; \
#	$(CHMOD) +x $@

hacobjdump_SOURCES = main/objdump.cc
hacobjdump_CXXFLAGS = -DWITH_MAIN
hacobjdump_LDADD = libhacktlib.la

hacpp_SOURCES = main/flatten.cc
hacpp_CXXFLAGS = -DWITH_MAIN
hacpp_LDADD = libhacktlib.la

if USE_UNROLL_PHASE
hacunroll_SOURCES = main/unroll.cc
hacunroll_CXXFLAGS = -DWITH_MAIN
hacunroll_LDADD = libhacktlib.la

# hacunroll: $(top_builddir)/config.status
#	@{ $(ECHO) "#!$(SH_PATH)" ; \
#	$(ECHO) "exec $(bindir)/`$(INSTALL_TRANSFORM_SH) hackt` unroll \"$$""@\"" ;} > $@ ; \
#	$(CHMOD) +x $@
endif

haccreate_SOURCES = main/create.cc
haccreate_CXXFLAGS = -DWITH_MAIN
haccreate_LDADD = libhacktlib.la

# haccreate: $(top_builddir)/config.status
#	@{ $(ECHO) "#!$(SH_PATH)" ; \
#	$(ECHO) "exec $(bindir)/`$(INSTALL_TRANSFORM_SH) hackt` create \"$$""@\"" ;} > $@ ; \
#	$(CHMOD) +x $@

hacalloc_SOURCES = main/alloc.cc
hacalloc_CXXFLAGS = -DWITH_MAIN
hacalloc_LDADD = libhacktlib.la

# hacalloc: $(top_builddir)/config.status
#	@{ $(ECHO) "#!$(SH_PATH)" ; \
#	$(ECHO) "exec $(bindir)/`$(INSTALL_TRANSFORM_SH) hackt` alloc \"$$""@\"" ;} > $@ ; \
#	$(CHMOD) +x $@

hflat_SOURCES = main/cflat.cc
hflat_CXXFLAGS = -DWITH_MAIN
hflat_LDADD = libhacktlib.la

# hflat: $(top_builddir)/config.status
#	@{ $(ECHO) "#!$(SH_PATH)" ; \
#	$(ECHO) "exec $(bindir)/`$(INSTALL_TRANSFORM_SH) hackt` cflat \"$$""@\"" ;} > $@ ; \
#	$(CHMOD) +x $@

hacprsim_SOURCES = main/prsim.cc
hacprsim_CXXFLAGS = -DWITH_MAIN
hacprsim_LDADD = libhacktsim.la
# goal: reduce to libhacktsim.la

# hacprsim: $(top_builddir)/config.status
#	@{ $(ECHO) "#!$(SH_PATH)" ; \
#	$(ECHO) "exec $(bindir)/`$(INSTALL_TRANSFORM_SH) hackt` prsim \"$$""@\"" ;} > $@ ; \
#	$(CHMOD) +x $@

hacchpsim_SOURCES = main/chpsim.cc
hacchpsim_CXXFLAGS = -DWITH_MAIN
hacchpsim_LDADD = libhacktsim.la
# goal: reduce to libhacktsim.la

# hacchpsim: $(top_builddir)/config.status
#	@{ $(ECHO) "#!$(SH_PATH)" ; \
#	$(ECHO) "exec $(bindir)/`$(INSTALL_TRANSFORM_SH) hackt` chpsim \"$$""@\"" ;} > $@ ; \
#	$(CHMOD) +x $@

# this name is needed for libtool objects
LIBHACKTPARSERS_LA = libhacktparsers_la
LIBBOGUS_LA = libbogus_la
LIBBOGUS2_LA = libbogus2_la
LIBBOGUS3_LA = libbogus3_la
LIBBOGUS4_LA = libbogus4_la

libhacktexec_la_LIBADD = libhacktsim.la

# pass in a C++ file to coerce --tag=CXX linkage
libhacktlib_la_SOURCES = main/main_funcs.cc
# want to split-off:  main/parse_funcs.cc
libhacktlib_la_LIBADD = libhacktparsers.la
# libhacktexec.la

libhacktsim_la_LIBADD = libhacktlib.la

libhacktparsers_la_LIBADD = libhacktast.la

libhacktast_la_LIBADD = \
	libhackt.la libhacktinstref.la libhacktcommon.la \
	$(LIBFANGUTIL)

# want to break, but have cyclic dependency through qualified_id_slice...
# libhackt_la_LIBADD = libhacktinstref.la libhacktcommon.la \
#	$(LIBFANGUTIL)

# need to list auto-generated header first to guarantee correct build
# order in parallel builds.  
libhacktlib.cc:
	$(TOUCH) $@
CLEANFILES += libhacktlib.cc

getopt_test_SOURCES = test/getopt_test.cc

# This says, "always build me first."
# trying NOT to abuse this, writing explicit dependencies below
BUILT_SOURCES += parser/hackt-parse.output.h \
	parser/instref-parse.output.h \
	parser/type-parse.output.h

# phony program to generate _build/*/.deps directories on distcheck
PHONYSOURCES += dummy.cc
dirgen_SOURCES = $(PHONYSOURCES) test/header_test.cc

# these two targets cause automake to create the corresponding _build
# subdirectories, that are needed for make distcheck, for check-headers
$(PHONYSOURCES):
	$(TOUCH) $@

cvstag.h: ChangeLog
	@{ $(ECHO) "#ifndef	CVSTAG" ; \
	tag=`$(HEAD) -n 10 $< | $(GREP) "^	[*].*:" | \
		$(HEAD) -n 1 | $(SED) 's/:.*$$//g' | $(CUT) -d\  -f 2` ; \
		$(ECHO) "#define	CVSTAG		\"$$tag\"" ; \
	$(ECHO) "#endif	/* CVSTAG */" ;} > $@

# we assume the config-params string is wrapped in quotes, and we strip them
config_params.h: $(top_builddir)/config.status
	@{ $(ECHO) "#ifndef	CONFIG_PARAMS" ; \
	value=`cd $(top_builddir) && $(SHELL) ./config.status --version | \
		$(GREP) options | \
		$(SED) -e 's/^[^"]*["]//g' -e 's/["]$$//g'` ; \
	$(ECHO) "#define	CONFIG_PARAMS	\"$$value\"" ; \
	$(ECHO) "#endif	/* CONFIG_PARAMS */" ;} > $@

# can't include libhacktexec.la because of circular dependence
# builddate.h:
# builddate.h: libhackt.la $(LIBFANGUTIL)
builddate.h: ChangeLog
	@{ $(ECHO) "#ifndef	BUILDDATE" ; \
	$(ECHO) "#define	BUILDDATE	\""`$(DATE)`"\"" ; \
	$(ECHO) "#endif	/* BUILDDATE */" ;} > $@

# most of these should really be generated at configure-time...

# not automatically generated by autoconf/automake
lexer/lex_version.h: $(top_builddir)/config.status
	$(RECONFIGURE)

parser/yacc_version.h: $(top_builddir)/config.status
	$(RECONFIGURE)


# should include make-generated headers only, no configure-generated headers
AUTOHEADERS = cvstag.h builddate.h am_cxxflags.h config_params.h
# parser/yacc_version.h
# lexer/lex_version.h
# cxx_version.h

# these headers are generated at configure time by config.status
VERSIONHEADERS = buildhost.h cxxflags.h cxx_version.h

# these headers are additional dependencies for configure-dependent parser files
CONFIGHEADERS = config.h __config__.h
IGNOREHEADERS = __config__.h __config__.h.in

HACFLAT_LEX_ROOT = hacflat-lex
HACKT_PARSE_ROOT = hackt-parse
HACKT_LEX_ROOT = hackt-lex
INSTREF_PARSE_ROOT = instref-parse
INSTREF_LEX_ROOT = instref-lex
TYPE_PARSE_ROOT = type-parse
TYPE_LEX_ROOT = type-lex

# libbogus.la is not actually used, hence the name.
# force use of lex/yacc suffix rules to generate the .c and .cc files
# the source files are post-processed via awk (below) into the real sources, 
# with symbol names with transformed prefixes.  
libbogus_la_SOURCES = \
	lexer/hackt-lex.ll \
	parser/hackt-parse.yy

libbogus2_la_SOURCES = \
	lexer/instref-lex.ll \
	parser/instref-parse.yy

libbogus3_la_SOURCES = \
	lexer/type-lex.ll \
	parser/type-parse.yy

libbogus4_la_SOURCES = \
	lexer/hacflat-lex.ll

# automake doesn't like macro in generating .Plo, a bug methinks
#	lexer/$(HACKT_LEX_ROOT).ll \
#	parser/$(HACKT_PARSE_ROOT).yy

AST_CC_FILES = \
	AST/node_position.cc \
	AST/expr.cc \
	AST/range.cc \
	AST/token.cc \
	AST/delim.cc \
	AST/definition.cc \
	AST/definition_item.cc \
	AST/instance.cc \
	AST/formal.cc \
	AST/CHP.cc \
	AST/HSE.cc \
	AST/PRS.cc \
	AST/SPEC.cc \
	AST/type.cc \
	AST/namespace.cc \
	AST/statement.cc \
	AST/import_root.cc \
	AST/parse_context.cc \
	AST/sizes-AST.cc

AST_TCC_FILES = \
	AST/node_list.tcc \
	AST/parse_context.tcc

AST_H_FILES = $(AST_CC_FILES:.cc=.h) \
	$(AST_TCC_FILES:.tcc=.h) \
	AST/AST_fwd.h \
	AST/CHP_fwd.h \
	AST/HSE_fwd.h \
	AST/PRS_fwd.h \
	AST/SPEC_fwd.h \
	AST/AST.h \
	AST/common.h \
	AST/expr_base.h \
	AST/expr_list.h \
	AST/identifier.h \
	AST/range_list.h \
	AST/reference.h \
	AST/root.h \
	AST/terminal.h \
	AST/token_char.h \
	AST/token_string.h \
	AST/type_base.h

# all of these .cc files have a corresponding .h file
OBJECT_INST_CC_FILES = \
	Object/inst/substructure_alias_base.cc \
	Object/inst/subinstance_manager.cc \
	Object/inst/connection_policy.cc \
	Object/inst/port_alias_tracker.cc \
	Object/inst/alias_actuals.cc \
	Object/inst/alias_matcher.cc \
	Object/inst/alias_printer.cc \
	Object/inst/instance_collection.cc \
	Object/inst/param_value_collection.cc \
	Object/inst/pbool_value_collection.cc \
	Object/inst/pint_value_collection.cc \
	Object/inst/preal_value_collection.cc \
	Object/inst/bool_instance_collection.cc \
	Object/inst/int_instance_collection.cc \
	Object/inst/enum_instance_collection.cc \
	Object/inst/struct_instance_collection.cc \
	Object/inst/process_instance_collection.cc \
	Object/inst/channel_instance_collection.cc

OBJECT_INST_TCC_FILES = \
	Object/inst/general_collection_type_manager.tcc \
	Object/inst/null_collection_type_manager.tcc \
	Object/inst/int_collection_type_manager.tcc \
	Object/inst/parameterless_collection_type_manager.tcc \
	Object/inst/channel_collection_type_manager.tcc \
	Object/inst/port_alias_tracker.tcc \
	Object/inst/subinstance_manager.tcc \
	Object/inst/alias_actuals.tcc \
	Object/inst/alias_empty.tcc \
	Object/inst/instance_alias.tcc \
	Object/inst/connection_policy.tcc \
	Object/inst/sparse_collection.tcc \
	Object/inst/collection_pool.tcc \
	Object/inst/instance_collection.tcc \
	Object/inst/port_formal_array.tcc \
	Object/inst/port_actual_collection.tcc \
	Object/inst/instance_collection_pool_bundle.tcc \
	Object/inst/instance_collection_pool_bundle_footprint.tcc \
	Object/inst/value_collection_pool_bundle.tcc \
	Object/inst/instance_placeholder.tcc \
	Object/inst/internal_aliases_policy.tcc \
	Object/inst/value_collection.tcc \
	Object/inst/value_placeholder.tcc \
	Object/inst/instance_pool.tcc \
	Object/inst/state_instance.tcc

OBJECT_INST_H_FILES = $(OBJECT_INST_CC_FILES:.cc=.h) \
	Object/inst/alias_visitee.h \
	Object/inst/alias_visitor.h \
	Object/inst/collection_fwd.h \
	Object/inst/connection_policy_fwd.h \
	Object/inst/instance_fwd.h \
	Object/inst/substructure_alias_fwd.h \
	Object/inst/general_collection_type_manager.h \
	Object/inst/null_collection_type_manager.h \
	Object/inst/int_collection_type_manager.h \
	Object/inst/parameterless_collection_type_manager.h \
	Object/inst/channel_collection_type_manager.h \
	Object/inst/instance_collection_base.h \
	Object/inst/physical_instance_collection.h \
	Object/inst/datatype_instance_collection.h \
	Object/inst/collection_index_entry.h \
	Object/inst/collection_traits.h \
	Object/inst/sparse_collection.h \
	Object/inst/collection_pool.h \
	Object/inst/collection_interface.h \
	Object/inst/instance_array.h \
	Object/inst/instance_scalar.h \
	Object/inst/port_formal_array.h \
	Object/inst/port_actual_collection.h \
	Object/inst/instance_collection_pool_bundle.h \
	Object/inst/value_collection_pool_bundle.h \
	Object/inst/instance_placeholder_base.h \
	Object/inst/instance_placeholder.h \
	Object/inst/value_placeholder.h \
	Object/inst/physical_instance_placeholder.h \
	Object/inst/datatype_instance_placeholder.h \
	Object/inst/param_value_placeholder.h \
	Object/inst/instance_alias_info.h \
	Object/inst/internal_aliases_policy_fwd.h \
	Object/inst/internal_aliases_policy.h \
	Object/inst/alias_empty.h \
	Object/inst/element_key_dumper.h \
	Object/inst/value_collection.h \
	Object/inst/value_array.h \
	Object/inst/value_scalar.h \
	Object/inst/instance_pool_fwd.h \
	Object/inst/instance_pool.h \
	Object/inst/state_instance.h \
	Object/inst/bool_instance.h \
	Object/inst/int_instance.h \
	Object/inst/enum_instance.h \
	Object/inst/struct_instance.h \
	Object/inst/process_instance.h \
	Object/inst/channel_instance.h \
	Object/inst/pbool_instance.h \
	Object/inst/pint_instance.h \
	Object/inst/preal_instance.h

OBJECT_REF_CC_FILES = \
	Object/ref/instance_reference.cc \
	Object/ref/instance_reference_datatype.cc \
	Object/expr/nonmeta_param_value_reference.cc \
	Object/ref/reference_set.cc

OBJECT_REF_TCC_FILES = \
	Object/ref/aggregate_reference_collection_base.tcc \
	Object/ref/aggregate_meta_instance_reference.tcc \
	Object/ref/aggregate_meta_value_reference.tcc \
	Object/ref/meta_instance_reference_subtypes.tcc \
	Object/ref/simple_meta_instance_reference.tcc \
	Object/ref/simple_nonmeta_instance_reference.tcc \
	Object/ref/member_meta_instance_reference.tcc \
	Object/ref/simple_meta_value_reference.tcc \
	Object/ref/simple_nonmeta_value_reference.tcc \
	Object/ref/nonmeta_ref_implementation.tcc

OBJECT_REF_H_FILES = \
	Object/ref/reference_enum.h \
	Object/ref/reference_set.h \
	Object/ref/references_fwd.h \
	Object/ref/meta_reference_union.h \
	Object/ref/aggregate_reference_collection_base.h \
	Object/ref/aggregate_meta_instance_reference_base.h \
	Object/ref/aggregate_meta_instance_reference.h \
	Object/ref/aggregate_meta_value_reference_base.h \
	Object/ref/aggregate_meta_value_reference.h \
	Object/ref/member_meta_instance_reference.h \
	Object/ref/meta_index_list_fwd.h \
	Object/ref/meta_instance_reference_base.h \
	Object/ref/meta_instance_reference_subtypes.h \
	Object/ref/meta_value_reference_base.h \
	Object/ref/meta_value_reference.h \
	Object/ref/nonmeta_instance_reference_base.h \
	Object/ref/nonmeta_instance_reference_subtypes.h \
	Object/ref/data_nonmeta_instance_reference.h \
	Object/ref/simple_meta_instance_reference.h \
	Object/ref/simple_meta_indexed_reference_base.h \
	Object/ref/simple_meta_value_reference.h \
	Object/ref/simple_nonmeta_instance_reference.h \
	Object/ref/simple_nonmeta_instance_reference_base.h \
	Object/ref/simple_nonmeta_value_reference.h \
	Object/ref/inst_ref_implementation_fwd.h \
	Object/ref/inst_ref_implementation.h \
	Object/ref/nonmeta_ref_implementation.h

# all these must have corresponding .h file
OBJECT_COMMON_CC_FILES = \
	Object/common/object_base.cc \
	Object/common/extern_templates.cc \
	Object/common/dump_flags.cc \
	common/ICE.cc \
	common/TODO.cc

OBJECT_COMMON_H_FILES = $(OBJECT_COMMON_CC_FILES:.cc=.h) \
	Object/common/alias_string_cache.h \
	Object/common/multikey_index.h \
	Object/common/util_types.h \
	common/devel_switches.h

OBJECT_CC_FILES = \
	Object/common/namespace.cc \
	Object/expr/exceptions.cc \
	Object/expr/expr_dump_context.cc \
	Object/expr/expr_visitor.cc \
	Object/expr/channel_probe.cc \
	Object/def/template_formals_manager.cc \
	Object/def/port_formals_manager.cc \
	Object/def/footprint.cc \
	Object/def/footprint_struct_base.cc \
	Object/def/footprint_basic_base.cc \
	Object/def/footprint_value_base.cc \
	Object/def/footprint_manager.cc \
	Object/traits/class_traits.cc \
	Object/unroll/conditional_scope.cc \
	Object/unroll/expression_assignment.cc \
	Object/unroll/instance_management_base.cc \
	Object/unroll/instantiation_statement.cc \
	Object/unroll/loop_scope.cc \
	Object/unroll/meta_instance_reference_connection.cc \
	Object/unroll/meta_conditional_base.cc \
	Object/unroll/meta_loop_base.cc \
	Object/unroll/unroll_context.cc \
	Object/module.cc \
	Object/global_entry.cc \
	Object/global_channel_entry.cc \
	Object/port_context.cc \
	Object/state_manager.cc \
	Object/nonmeta_state.cc \
	Object/nonmeta_variable.cc \
	Object/nonmeta_context.cc \
	Object/nonmeta_channel_manipulator.cc \
	Object/global_entry_context.cc \
	Object/sizes-entity.cc \
	Object/type/template_actuals.cc \
	Object/type/canonical_type_base.cc \
	Object/type/canonical_type.cc \
	Object/type/canonical_generic_chan_type.cc \
	Object/type/canonical_fundamental_chan_type.cc \
	Object/lang/cflat_visitor.cc \
	Object/lang/cflat_context_visitor.cc \
	Object/lang/cflat_printer.cc \
	Object/lang/bool_literal.cc \
	Object/lang/directive_base.cc \
	Object/lang/directive_definition.cc \
	Object/lang/directive_source.cc \
	Object/lang/PRS.cc \
	Object/lang/PRS_footprint.cc \
	Object/lang/PRS_attribute_common.cc \
	Object/lang/PRS_attribute_registry.cc \
	Object/lang/PRS_macro_common.cc \
	Object/lang/PRS_macro_registry.cc \
	Object/lang/CHP.cc \
	Object/lang/CHP_visitor.cc \
	Object/lang/SPEC.cc \
	Object/lang/SPEC_common.cc \
	Object/lang/SPEC_footprint.cc \
	Object/lang/SPEC_registry.cc

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
	Object/expr/operators.cc \
	Object/expr/data_expr.cc \
	Object/type/type_reference.cc

# these header files have source (.cc) files with different names
OBJECT_TCC_FILES = \
	Object/def/footprint.tcc \
	Object/expr/const_collection.tcc \
	Object/expr/convert_expr.tcc \
	Object/expr/loop_meta_expr.tcc \
	Object/expr/loop_nonmeta_expr.tcc \
	Object/def/footprint_base.tcc \
	Object/type/canonical_type.tcc \
	Object/unroll/alias_connection.tcc \
	Object/unroll/expression_assignment.tcc \
	Object/unroll/instantiation_statement.tcc \
	Object/unroll/port_connection.tcc \
	Object/unroll/unroll_context.tcc \
	Object/lang/PRS_macro_registry.tcc \
	Object/lang/SPEC_registry.tcc \
	Object/module.tcc \
	Object/global_entry.tcc \
	Object/global_entry_context.tcc \
	Object/nonmeta_context.tcc \
	Object/state_manager.tcc \
	Object/common/namespace.tcc \
	Object/lang/CHP.tcc

OBJECT_H_FILES = $(OBJECT_CC_FILES:.cc=.h) \
	Object/devel_switches.h \
	Object/object_fwd.h \
	Object/persistent_type_hash.h \
	Object/cflat_context.h \
	Object/entry_collection.h \
	Object/common/cflat_args.h \
	Object/common/scopespace.h \
	Object/traits/class_traits_fwd.h \
	Object/traits/classification_tags_fwd.h \
	Object/traits/classification_tags.h \
	Object/traits/chan_traits.h \
	Object/traits/data_traits.h \
	Object/traits/proc_traits.h \
	Object/traits/param_traits.h \
	Object/traits/pint_traits.h \
	Object/traits/pbool_traits.h \
	Object/traits/preal_traits.h \
	Object/traits/int_traits.h \
	Object/traits/bool_traits.h \
	Object/traits/enum_traits.h \
	Object/traits/struct_traits.h \
	Object/traits/instance_traits.h \
	Object/traits/type_tag_enum.h \
	Object/traits/value_traits.h \
	Object/def/definition_fwd.h \
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
	Object/def/footprint_base.h \
	Object/def/fundamental_channel_footprint.h \
	Object/expr/expr_fwd.h \
	Object/expr/exceptions_fwd.h \
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
	Object/expr/convert_expr.h \
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
	Object/expr/loop_meta_expr.h \
	Object/expr/loop_nonmeta_expr.h \
	Object/expr/real_expr.h \
	Object/expr/meta_index_expr.h \
	Object/expr/meta_index_list.h \
	Object/expr/meta_range_expr.h \
	Object/expr/meta_range_list.h \
	Object/expr/nonmeta_index_expr_base.h \
	Object/expr/nonmeta_index_list.h \
	Object/expr/nonmeta_range_expr_base.h \
	Object/expr/operator_info.h \
	Object/expr/operator_precedence.h \
	Object/expr/param_expr.h \
	Object/expr/param_expr_functor.h \
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
	Object/expr/preal_const.h \
	Object/expr/preal_expr.h \
	Object/expr/preal_arith_expr.h \
	Object/expr/preal_relational_expr.h \
	Object/expr/preal_unary_expr.h \
	Object/expr/struct_expr.h \
	Object/expr/types.h \
	Object/lang/cflat_visitee.h \
	Object/lang/CHP_footprint.h \
	Object/lang/CHP_footprint_expr.h \
	Object/lang/PRS_enum.h \
	Object/lang/PRS_footprint_expr.h \
	Object/lang/PRS_footprint_expr_pool_fwd.h \
	Object/lang/PRS_footprint_macro.h \
	Object/lang/PRS_footprint_rule.h \
	Object/type/type_fwd.h \
	Object/type/builtin_channel_type_reference.h \
	Object/type/channel_type_reference.h \
	Object/type/channel_type_reference_base.h \
	Object/type/data_type_reference.h \
	Object/type/fundamental_type_reference.h \
	Object/type/param_type_reference.h \
	Object/type/process_type_reference.h \
	Object/type/type_reference_base.h \
	Object/type/canonical_type_fwd.h \
	Object/type/canonical_generic_datatype.h \
	Object/unroll/unroll_fwd.h \
	Object/unroll/alias_connection.h \
	Object/unroll/aliases_connection_base.h \
	Object/unroll/channel_instantiation_statement.h \
	Object/unroll/data_alias_connection_base.h \
	Object/unroll/datatype_instantiation_statement.h \
	Object/unroll/empty_instantiation_statement_type_ref_base.h \
	Object/unroll/instance_management_base.h \
	Object/unroll/instantiation_statement_base.h \
	Object/unroll/instantiation_statement_type_ref_default.h \
	Object/unroll/channel_instantiation_type_ref_base.h \
	Object/unroll/null_parameter_type.h \
	Object/unroll/param_expression_assignment.h \
	Object/unroll/param_instantiation_statement.h \
	Object/unroll/port_connection_base.h \
	Object/unroll/port_connection.h \
	Object/unroll/process_instantiation_statement.h \
	Object/unroll/sequential_scope.h \
	Object/lang/PRS_fwd.h \
	Object/lang/PRS_base.h \
	Object/lang/PRS_literal_unroller.h \
	Object/lang/CHP_base.h \
	Object/lang/SPEC_fwd.h

PARSER_H_FILES = \
	parser/$(HACKT_PARSE_ROOT)-real.h \
	parser/hackt-union.h \
	parser/$(HACKT_PARSE_ROOT).output.h \
	parser/$(HACKT_PARSE_ROOT)-options.h \
	lexer/$(HACKT_LEX_ROOT)-options.h \
	parser/$(INSTREF_PARSE_ROOT)-real.h \
	parser/instref-union.h \
	parser/instref.h \
	parser/$(INSTREF_PARSE_ROOT).output.h \
	parser/$(INSTREF_PARSE_ROOT)-options.h \
	lexer/$(INSTREF_LEX_ROOT)-options.h \
	parser/$(TYPE_PARSE_ROOT)-real.h \
	parser/type-union.h \
	parser/type.h \
	parser/$(TYPE_PARSE_ROOT).output.h \
	parser/$(TYPE_PARSE_ROOT)-options.h \
	lexer/$(TYPE_LEX_ROOT)-options.h \
	lexer/$(HACFLAT_LEX_ROOT)-options.h \
	lexer/hacflat-lex-options.h \
	lexer/hacflat-yystype.h \
	lexer/file_manager.h \
	lexer/input_manager.h \
	lexer/yyin_manager.h \
	lexer/hac_lex.h

# can't use $(HACKT_PARSE_ROOT) to substitute :(
PARSER_CC_FILES = \
	parser/hackt-parse-real.cc \
	parser/hackt-union.cc \
	lexer/hackt-lex-real.cc \
	parser/instref-parse-real.cc \
	parser/instref-union.cc \
	parser/instref.cc \
	lexer/instref-lex-real.cc \
	parser/type-parse-real.cc \
	parser/type-union.cc \
	parser/type.cc \
	lexer/type-lex-real.cc \
	lexer/hacflat-lex-real.cc \
	lexer/file_manager.cc \
	lexer/yyin_manager.cc \
	lexer/hac_lex.cc


SIM_CC_FILES = \
	sim/ISE.cc \
	sim/time.cc \
	sim/command_base.cc \
	sim/command_builtin.cc \
	sim/command_registry.cc \
	sim/random_time.cc \
	sim/state_base.cc \
	sim/signal_handler.cc \
	sim/prsim/Command-prsim.cc \
	sim/prsim/Cause.cc \
	sim/prsim/Node.cc \
	sim/prsim/Expr.cc \
	sim/prsim/ExprAlloc.cc \
	sim/prsim/Event-prsim.cc \
	sim/prsim/State-prsim.cc \
	sim/chpsim/nonmeta_context.cc \
	sim/chpsim/Command.cc \
	sim/chpsim/Dependence.cc \
	sim/chpsim/DependenceCollector.cc \
	sim/chpsim/Event.cc \
	sim/chpsim/EventExecutor.cc \
	sim/chpsim/State.cc \
	sim/chpsim/StateConstructor.cc \
	sim/chpsim/Trace.cc

SIM_TCC_FILES = \
	sim/command_base.tcc \
	sim/command_builtin.tcc \
	sim/command_common.tcc \
	sim/command_category.tcc \
	sim/command_registry.tcc \
	sim/signal_handler.tcc \
	sim/event.tcc \
	sim/prsim/Rule.tcc

SIM_H_FILES = $(SIM_CC_FILES:.cc=.h) \
	$(SIM_TCC_FILES:.tcc=.h) \
	sim/common.h \
	sim/devel_switches.h \
	sim/prsim/devel_switches.h \
	sim/prsim/ExprAllocFlags.h \
	sim/chpsim/StateConstructorFlags.h \
	sim/chpsim/devel_switches.h \
	sim/chpsim/graph_options.h

# libhacktexec sources
MAIN_EXEC_CC_FILES = \
	main/force_load.cc \
	main/program_registry.cc \
	main/parse_test.cc \
	main/flatten.cc \
	main/compile.cc \
	main/unroll.cc \
	main/create.cc \
	main/alloc.cc \
	main/cflat.cc \
	main/prsobjdemo.cc \
	main/prsim.cc \
	main/dump_persistent_table.cc \
	main/objdump.cc \
	main/shell.cc \
	main/version.cc \
	main/chpsim.cc

MAIN_EXEC_TCC_FILES = \
	main/options_modifier.tcc \
	main/simple_options.tcc

MAIN_EXEC_H_FILES = $(MAIN_EXEC_CC_FILES:.cc=.h) \
	$(MAIN_EXEC_TCC_FILES:.tcc=.h) \
	main/hackt.h \
	main/hackt_fwd.h \
	main/cflat_options.h \
	main/compile_options.h \
	main/global_options.h \
	main/main_funcs.h

COMMON_H_FILES = \
	common/sizes-common.h

COMMON_FILES = $(COMMON_H_FILES) \
	common/sizes-common.cc

# automake doesn't like macro in generating .Plo, a bug methinks
#	parser/$(HACKT_PARSE_ROOT)-real.cc
#	lexer/$(HACKT_LEX_ROOT)-real.cc

# the generated lexer and parser sources aren't supposed to be distributed!
libhackt_la_SOURCES = \
	$(OBJECT_CC_FILES) $(OBJECT_TCC_FILES) $(OBJECT_H_FILES) \
	$(OBJECT_CC_NO_H_FILES) \
	$(COMMON_FILES) \
	config.h

libhacktcommon_la_SOURCES = \
	$(OBJECT_COMMON_CC_FILES) \
	$(OBJECT_COMMON_TCC_FILES) \
	$(OBJECT_COMMON_H_FILES)

# combined library of instances and references
libhacktinstref_la_SOURCES = \
	$(OBJECT_INST_CC_FILES) \
	$(OBJECT_INST_TCC_FILES) \
	$(OBJECT_INST_H_FILES) \
	$(OBJECT_REF_CC_FILES) \
	$(OBJECT_REF_TCC_FILES) \
	$(OBJECT_REF_H_FILES)

libhacktast_la_SOURCES = $(AST_CC_FILES) $(AST_TCC_FILES) $(AST_H_FILES)

libhacktexec_la_SOURCES = \
	$(MAIN_EXEC_CC_FILES) \
	$(MAIN_EXEC_H_FILES) \
	$(MAIN_EXEC_TCC_FILES)

echo-libhacktexec-sources:
	@$(ECHO) $(libhacktexec_la_SOURCES)

libhacktexec_la_LIBADD += $(LIBMISCFUN)

libhacktsim_la_SOURCES = $(SIM_CC_FILES) $(SIM_TCC_FILES) $(SIM_H_FILES)

# any other files that depend on automatically generated headers
# should have explicit dependencies.  
# unfornately, it doesn't detect these dependenvies until AFTER first compile
common/ICE.lo common/ICE.o: cvstag.h
main/main_funcs.lo main/main_funcs.o: parser/$(HACKT_PARSE_ROOT)-real.h
main/version.lo main/version.o: $(AUTOHEADERS) $(VERSIONHEADERS) \
	parser/yacc_version.h lexer/lex_version.h
lexer/$(LIBHACKTPARSERS_LA)-$(HACKT_LEX_ROOT)-real.lo \
	lexer/$(LIBHACKTPARSERS_LA)-$(HACKT_LEX_ROOT)-real.o: \
	parser/$(HACKT_PARSE_ROOT).h
parser/$(LIBHACKTPARSERS_LA)-hackt-union.lo \
	parser/$(LIBHACKTPARSERS_LA)-hackt-union.o: \
	parser/$(HACKT_PARSE_ROOT).output.h
lexer/$(LIBHACKTPARSERS_LA)-$(INSTREF_LEX_ROOT)-real.lo \
	lexer/$(LIBHACKTPARSERS_LA)-$(INSTREF_LEX_ROOT)-real.o: \
	parser/$(INSTREF_PARSE_ROOT).h
parser/$(LIBHACKTPARSERS_LA)-instref-union.lo \
	parser/$(LIBHACKTPARSERS_LA)-instref-union.o: \
	parser/$(INSTREF_PARSE_ROOT).output.h
lexer/$(LIBHACKTPARSERS_LA)-$(TYPE_LEX_ROOT)-real.lo \
	lexer/$(LIBHACKTPARSERS_LA)-$(TYPE_LEX_ROOT)-real.o: \
	parser/$(TYPE_PARSE_ROOT).h
parser/$(LIBHACKTPARSERS_LA)-type-union.lo \
	parser/$(LIBHACKTPARSERS_LA)-type-union.o: \
	parser/$(TYPE_PARSE_ROOT).output.h

# even more extra dependencies for bogus libraries
lexer/$(LIBBOGUS_LA)-$(HACKT_LEX_ROOT).lo \
	lexer/$(LIBBOGUS_LA)-$(HACKT_LEX_ROOT).o \
	parser/$(LIBBOGUS_LA)-$(HACKT_PARSE_ROOT).lo \
	parser/$(LIBBOGUS_LA)-$(HACKT_PARSE_ROOT).o: \
	parser/hackt-prefix.h
lexer/$(LIBBOGUS2_LA)-$(INSTREF_LEX_ROOT).lo \
	lexer/$(LIBBOGUS2_LA)-$(INSTREF_LEX_ROOT).o \
	parser/$(LIBBOGUS2_LA)-$(INSTREF_PARSE_ROOT).lo \
	parser/$(LIBBOGUS2_LA)-$(INSTREF_PARSE_ROOT).o: \
	parser/instref-prefix.h
lexer/$(LIBBOGUS3_LA)-$(TYPE_LEX_ROOT).lo \
	lexer/$(LIBBOGUS3_LA)-$(TYPE_LEX_ROOT).o \
	parser/$(LIBBOGUS3_LA)-$(TYPE_PARSE_ROOT).lo \
	parser/$(LIBBOGUS3_LA)-$(TYPE_PARSE_ROOT).o: \
	parser/type-prefix.h

# the following targets need to be prefixed with the library name, libbogus, 
# but automake complains that it overrides the automatic definition...
# parser/$(HACKT_PARSE_ROOT).lo: parser/$(HACKT_PARSE_ROOT).output.h
# parser/$(HACKT_PARSE_ROOT).$(OBJEXT): parser/$(HACKT_PARSE_ROOT).output.h
# this creates cyclic dependencies (bad)
# parser/$(HACKT_PARSE_ROOT).cc: parser/$(HACKT_PARSE_ROOT).output.h

# compile lexer and parser into separate library to for convenient way
# of giving them exceptional compiler flags, see libhacktparsers_la_CXXFLAGS
libhacktparsers_la_SOURCES = \
	$(PARSER_CC_FILES) $(PARSER_H_FILES)

ALL_HEADER_FILES += \
	$(CONFIG_HEADERS) \
	$(COMMON_H_FILES) \
	$(OBJECT_H_FILES) \
	$(OBJECT_COMMON_H_FILES) \
	$(OBJECT_INST_H_FILES) \
	$(OBJECT_REF_H_FILES) \
	$(AST_H_FILES) \
	$(PARSER_H_FILES) \
	$(SIM_H_FILES) \
	$(MAIN_EXEC_H_FILES)

ALL_TCC_FILES += \
	$(OBJECT_TCC_FILES) \
	$(OBJECT_INST_TCC_FILES) \
	$(OBJECT_REF_TCC_FILES) \
	$(MAIN_EXEC_TCC_FILES) \
	$(SIM_TCC_FILES) \
	$(AST_TCC_FILES)

# don't actually need -l[f]l
# libhackt_la_LDADD = @LEXLIB@

# why, oh why, yy?
HACKT_YACC_PREFIX = hackt_
INSTREF_YACC_PREFIX = instref_
TYPE_YACC_PREFIX = type_

# regenerate the parser input file using config.status
parser/$(HACKT_PARSE_ROOT).yy: parser/$(HACKT_PARSE_ROOT).yy.in
	$(RECONFIGURE)
parser/$(INSTREF_PARSE_ROOT).yy: parser/$(INSTREF_PARSE_ROOT).yy.in
	$(RECONFIGURE)
parser/$(TYPE_PARSE_ROOT).yy: parser/$(TYPE_PARSE_ROOT).yy.in
	$(RECONFIGURE)

# these should be distcleaned, because they are configure-generated
CONFIG_PARSERS = parser/$(HACKT_PARSE_ROOT).yy \
	parser/$(INSTREF_PARSE_ROOT).yy \
	parser/$(TYPE_PARSE_ROOT).yy

# because I can't figure out how to produce a literal '&' in awk subst regex.  
SED_CXX_REFERENCE = $(SED) 's/REFERENCE/\&/g'
# because yacc has no mechanism for overriding the yyparse prototype
SED_YACC_YYPARSE = $(SED) '/^yyparse()/s/()/(void*, YYSTYPE\& yylval, FILE* infile)/g'

# some old yacc skeletons assign string literals to char*, which is deprecated
# e.g apple ships yacc skeleton 1.14
SED_CONST_CHAR_P = $(AWK) -f $(srcdir)/parser/yacc_const_char.awk

# Oddly, these targets get mistakenly placed in the srcdir
parser/hackt-prefix.h: $(LEXYACC_PREFIX_GENERATOR)
	$(SED_srcdir_strip_setup) ; \
	$(AWK) -f $< -v PREFIX=$(HACKT_YACC_PREFIX) < /dev/null > \
		`$(ECHO) $@ | $(SED_srcdir_strip_fast)`

parser/instref-prefix.h: $(LEXYACC_PREFIX_GENERATOR)
	$(SED_srcdir_strip_setup) ; \
	$(AWK) -f $< -v PREFIX=$(INSTREF_YACC_PREFIX) < /dev/null > \
		`$(ECHO) $@ | $(SED_srcdir_strip_fast)`

parser/type-prefix.h: $(LEXYACC_PREFIX_GENERATOR)
	$(SED_srcdir_strip_setup) ; \
	$(AWK) -f $< -v PREFIX=$(TYPE_YACC_PREFIX) < /dev/null > \
		`$(ECHO) $@ | $(SED_srcdir_strip_fast)`

LEXYACC_PREFIX_H_FILES = parser/hackt-prefix.h \
	parser/instref-prefix.h parser/type-prefix.h

ALL_HEADER_FILES += $(LEXYACC_PREFIX_H_FILES)
CLEANFILES += $(LEXYACC_PREFIX_H_FILES)

PARSER_HACK_PIPE =
if HAVE_BISON
PARSER_HACK_PIPE += $(AWK) -f $(srcdir)/parser/try_lex_catch.awk | \
	$(AWK) -f $(srcdir)/parser/nls_bison_hack.awk | \
	$(AWK) -f $(srcdir)/parser/yyerror_bison_hack.awk | \
	$(AWK) -f $(srcdir)/parser/purify_flex.awk | \
	$(SED_CXX_REFERENCE) | \
	$(GREP) -v "^YYSTYPE yylval;"
endif
if HAVE_BYACC
PARSER_HACK_PIPE += $(AWK) -f $(srcdir)/parser/try_lex_catch.awk | \
	$(AWK) -f $(srcdir)/parser/yyerror_bison_hack.awk | \
	$(AWK) -f $(srcdir)/parser/purify_yacc.awk | \
	$(AWK) -f $(srcdir)/parser/purify_flex.awk | \
	$(AWK) -f $(srcdir)/parser/yygrowstack_yacc_hack.awk | \
	$(SED_CXX_REFERENCE) | \
	$(SED_CONST_CHAR_P) | \
	$(SED_YACC_YYPARSE) | \
	$(AWK) -f $(srcdir)/parser/patch_byacc.awk
endif
if HAVE_YACC
PARSER_HACK_PIPE += $(AWK) -f $(srcdir)/parser/try_lex_catch.awk | \
	$(AWK) -f $(srcdir)/parser/yyerror_bison_hack.awk | \
	$(AWK) -f $(srcdir)/parser/purify_yacc.awk | \
	$(AWK) -f $(srcdir)/parser/purify_flex.awk | \
	$(AWK) -f $(srcdir)/parser/yygrowstack_yacc_hack.awk | \
	$(SED_CXX_REFERENCE) | \
	$(SED_CONST_CHAR_P) | \
	$(SED_YACC_YYPARSE)
endif


# rules for the prefix-transformed versions of generated lexer/parser sources
# make these conditional on yacc vs. bison
# important: the LEXYACC_PREFIX_SCRIPT must be applied LAST!
# the other preceeding scripts shouldn't matter in their order
parser/$(HACKT_PARSE_ROOT)-real.cc: parser/$(HACKT_PARSE_ROOT).cc \
		parser/$(HACKT_PARSE_ROOT).output.h
	$(CAT) parser/$(HACKT_PARSE_ROOT).cc | $(PARSER_HACK_PIPE) > $@

parser/$(INSTREF_PARSE_ROOT)-real.cc: parser/$(INSTREF_PARSE_ROOT).cc \
		parser/$(INSTREF_PARSE_ROOT).output.h
	$(CAT) parser/$(INSTREF_PARSE_ROOT).cc | $(PARSER_HACK_PIPE) > $@

parser/$(TYPE_PARSE_ROOT)-real.cc: parser/$(TYPE_PARSE_ROOT).cc \
		parser/$(TYPE_PARSE_ROOT).output.h
	$(CAT) parser/$(TYPE_PARSE_ROOT).cc | $(PARSER_HACK_PIPE) > $@


# NOTE: oddly, this sometimes builds in the srcdir, when it shouldn't
parser/$(HACKT_PARSE_ROOT)-real.h: parser/$(HACKT_PARSE_ROOT).h \
		parser/hackt-prefix.h
	$(SED_srcdir_strip_setup) ; \
	{ $(ECHO) "#include \"parser/hackt-prefix.h\"" ; \
	$(ECHO) "#include \"parser/hackt-union.h\"" ; \
	$(CAT) parser/$(HACKT_PARSE_ROOT).h ;} > \
		`$(ECHO) $@ | $(SED_srcdir_strip_fast)`

parser/$(INSTREF_PARSE_ROOT)-real.h: parser/$(INSTREF_PARSE_ROOT).h \
		parser/instref-prefix.h
	$(SED_srcdir_strip_setup) ; \
	{ $(ECHO) "#include \"parser/instref-prefix.h\"" ; \
	$(ECHO) "#include \"parser/instref-union.h\"" ; \
	$(ECHO) "#include \"parser/instref.h\"" ; \
	$(CAT) parser/$(INSTREF_PARSE_ROOT).h ;} > \
		`$(ECHO) $@ | $(SED_srcdir_strip_fast)`

parser/$(TYPE_PARSE_ROOT)-real.h: parser/$(TYPE_PARSE_ROOT).h \
		parser/type-prefix.h
	$(SED_srcdir_strip_setup) ; \
	{ $(ECHO) "#include \"parser/type-prefix.h\"" ; \
	$(ECHO) "#include \"parser/type-union.h\"" ; \
	$(CAT) parser/$(TYPE_PARSE_ROOT).h ;} > \
		`$(ECHO) $@ | $(SED_srcdir_strip_fast)`

PURIFY_LEXER_PIPE = $(AWK) -f $(srcdir)/lexer/purify_flex.awk | \
	$(SED) 's/REFERENCE/\&/g'

# need to include prefix header because the lexer-generated code
# doesn't include it early enough from the .ll source :(
# if HAVE_FLEX ...
lexer/$(HACKT_LEX_ROOT)-real.cc: lexer/$(HACKT_LEX_ROOT).cc \
		parser/$(HACKT_PARSE_ROOT)-real.h
# need forward declaration for flex 2.5.31
# need early declaration of flex::lexer_state needed for flex 2.5.{31,33}
	{ $(ECHO) "#include \"parser/hackt-prefix.h\"" ; \
	$(ECHO) "#include \"lexer/flex_lexer_state.h\"" ; \
	$(CAT) lexer/$(HACKT_LEX_ROOT).cc | $(PURIFY_LEXER_PIPE) ;} > $@
# flex scanner purification eliminates need to rename most variables
#	$(AWK) -f $(LEXYACC_PREFIX_SCRIPT) \
#		-v PREFIX=$(HACKT_YACC_PREFIX) > $@

lexer/$(INSTREF_LEX_ROOT)-real.cc: lexer/$(INSTREF_LEX_ROOT).cc \
		parser/$(INSTREF_PARSE_ROOT)-real.h
	{ $(ECHO) "#include \"parser/instref-prefix.h\"" ; \
	$(ECHO) "#include \"lexer/flex_lexer_state.h\"" ; \
	$(CAT) lexer/$(INSTREF_LEX_ROOT).cc | $(PURIFY_LEXER_PIPE) ;} > $@

lexer/$(TYPE_LEX_ROOT)-real.cc: lexer/$(TYPE_LEX_ROOT).cc \
		parser/$(TYPE_PARSE_ROOT)-real.h
	{ $(ECHO) "#include \"parser/type-prefix.h\"" ; \
	$(ECHO) "#include \"lexer/flex_lexer_state.h\"" ; \
	$(CAT) lexer/$(TYPE_LEX_ROOT).cc | $(PURIFY_LEXER_PIPE) ;} > $@

lexer/$(HACFLAT_LEX_ROOT)-real.cc: lexer/$(HACFLAT_LEX_ROOT).cc
	{ $(ECHO) "#include \"lexer/flex_lexer_state.h\"" ; \
	$(CAT) lexer/$(HACFLAT_LEX_ROOT).cc | $(PURIFY_LEXER_PIPE) ;} > $@

$(srcdir)/lexer/instref.cc: parser/$(INSTREF_PARSE_ROOT)-real.h

$(srcdir)/lexer/type.cc: parser/$(TYPE_PARSE_ROOT)-real.h

# can't depend on the .output alone, not a known dependence with .y[y]
HACKT_PARSE_OUTPUT = parser/$(HACKT_PARSE_ROOT).output
$(HACKT_PARSE_OUTPUT): parser/$(HACKT_PARSE_ROOT).h

PARSE_OUTPUT_TO_HEADER_PIPE = $(AWK) -f $(YACC_OUTPUT_H_SCRIPT) \
	-f $(srcdir)/parser/common-output-to-C.awk

# Oddly, the targets sometimes get mistakenly generated in the srcdir...
PARSE_OUTPUT_HEADER_SCRIPT = \
	if $(GREP) "conflict" $< ; then exit 1; fi ; \
	$(SED_srcdir_strip_setup) ; \
	$(CAT) $< | $(PARSE_OUTPUT_TO_HEADER_PIPE) > \
		`$(ECHO) $@ | $(SED_srcdir_strip_fast)`

$(HACKT_PARSE_OUTPUT).h: $(HACKT_PARSE_OUTPUT)
	$(PARSE_OUTPUT_HEADER_SCRIPT)
#	@if $(GREP) "conflict" $<; then exit 1; fi
#	$(CAT) $< | $(PARSE_OUTPUT_TO_HEADER_PIPE) > $@

INSTREF_PARSE_OUTPUT = parser/$(INSTREF_PARSE_ROOT).output
$(INSTREF_PARSE_OUTPUT): parser/$(INSTREF_PARSE_ROOT).h

$(INSTREF_PARSE_OUTPUT).h: $(INSTREF_PARSE_OUTPUT)
	$(PARSE_OUTPUT_HEADER_SCRIPT)
#	@if $(GREP) "conflict" $<; then exit 1; fi
#	$(CAT) $< | $(PARSE_OUTPUT_TO_HEADER_PIPE) > $@

TYPE_PARSE_OUTPUT = parser/$(TYPE_PARSE_ROOT).output
$(TYPE_PARSE_OUTPUT): parser/$(TYPE_PARSE_ROOT).h

$(TYPE_PARSE_OUTPUT).h: $(TYPE_PARSE_OUTPUT)
	$(PARSE_OUTPUT_HEADER_SCRIPT)
#	@if $(GREP) "conflict" $<; then exit 1; fi
#	$(CAT) $< | $(PARSE_OUTPUT_TO_HEADER_PIPE) > $@

# These all depend on parser/$(HACKT_PARSE_ROOT).h, 
# which is made non-atomically, by automake's yacc rules.
# Using the NOTPARALLEL directive kills parallel-make for this whole Makefile!
# .NOTPARALLEL: parser/$(HACKT_PARSE_ROOT)-real.h $(HACKT_PARSE_OUTPUT) \
#	lexer/$(HACKT_LEX_ROOT).cc
# What we could do is enforce some artificial ordering, making
# $(HACKT_PARSE_OUTPUT) the canonical target of parser/$(HACKT_PARSE_ROOT).h

# extra deps to make sure parser is regenerated when configure changes
# in the case when switching between yacc and bison
lexer/$(HACKT_LEX_ROOT).cc: $(CONFIGHEADERS) lexer/lex_version.h
parser/$(HACKT_PARSE_ROOT).cc: $(CONFIGHEADERS) parser/yacc_version.h
$(HACKT_PARSE_OUTPUT): $(CONFIGHEADERS) parser/yacc_version.h

lexer/$(INSTREF_LEX_ROOT).cc: $(CONFIGHEADERS) lexer/lex_version.h
parser/$(INSTREF_PARSE_ROOT).cc: $(CONFIGHEADERS) parser/yacc_version.h
$(INSTREF_PARSE_OUTPUT): $(CONFIGHEADERS) parser/yacc_version.h

lexer/$(TYPE_LEX_ROOT).cc: $(CONFIGHEADERS) lexer/lex_version.h
parser/$(TYPE_PARSE_ROOT).cc: $(CONFIGHEADERS) parser/yacc_version.h
$(TYPE_PARSE_OUTPUT): $(CONFIGHEADERS) parser/yacc_version.h

lexer/$(HACFLAT_LEX_ROOT).cc: $(CONFIGHEADERS) lexer/lex_version.h

# why didn't this work?
# fixed: need to escape the '#' character
PARSER_TYPES_PIPE = $(GREP) -v "\#include" | $(CPP_TRADITIONAL) -P | \
	$(GREP) -v pragma | \
		$(SED) -e '/^%start/,$$d' -e '/%{/,/%}/d' -e '/^[ \t]*$$/d'

parser/hackt.types: parser/$(HACKT_PARSE_ROOT).yy
	$(CAT) $< | $(PARSER_TYPES_PIPE) > $@
#	$(CAT) $< | $(GREP) -v "#include" | $(CPP_TRADITIONAL) -P | \
#		$(GREP) -v pragma | \
#		$(SED) -e "/^%start/,$$$$d" -e "/%{/,/%}/d" | \
#		$(SED) '/^[ \t]*$$/d' > $@

parser/instref.types: parser/$(INSTREF_PARSE_ROOT).yy
	$(CAT) $< | $(PARSER_TYPES_PIPE) > $@
#	$(CAT) $< | $(GREP) -v "#include" | $(CPP_TRADITIONAL) -P | \
#		$(GREP) -v pragma | \
#		$(SED) -e "/^%start/,$$$$d" -e "/%{/,/%}/d" | \
#		$(SED) '/^[ \t]*$$/d' > $@

parser/type.types: parser/$(TYPE_PARSE_ROOT).yy
	$(CAT) $< | $(PARSER_TYPES_PIPE) > $@
#	$(CAT) $< | $(GREP) -v "#include" | $(CPP_TRADITIONAL) -P | \
#		$(GREP) -v pragma | \
#		$(SED) -e "/^%start/,$$$$d" -e "/%{/,/%}/d" | \
#		$(SED) '/^[ \t]*$$/d' > $@

# extract num: first matched line, 3rd space-delimited field (#define foo bar)
# "cut" is not a good idea, if # define is separated... :(
# the last awk pipe eliminate extraneous whitespace not filtered by sed
GET_START_TOKEN_ENUM = \
	$(GREP) "define.*MINIMUM_BOGOSITY" parser/$(HACKT_PARSE_ROOT).h | \
		$(HEAD) -n1 | $(SED) 's/^.*MINIMUM_BOGOSITY[ \t]*//g' | \
		$(AWK) '/.*/{printf($$1);}'
# TODO: use configure-time yacc test to determine this

# so far, yacc token enums start at 257 consistently, but we take no chances.
HACKT_YACC_UNION_SCRIPT_ARGS = -v yaccfile=parser/hackt.types \
		-v include="AST/AST.h parser/$(HACKT_PARSE_ROOT)-real.h lexer/hac_lex.h" \
		-v namespace="HAC::parser HAC::lexer" \
		-v token_enum=`$(GET_START_TOKEN_ENUM)`

INSTREF_YACC_UNION_SCRIPT_ARGS = -v yaccfile=parser/instref.types \
		-v include="AST/AST.h parser/$(INSTREF_PARSE_ROOT)-real.h lexer/hac_lex.h" \
		-v namespace="HAC::parser HAC::lexer" \
		-v token_enum=`$(GET_START_TOKEN_ENUM)`

TYPE_YACC_UNION_SCRIPT_ARGS = -v yaccfile=parser/type.types \
		-v include="AST/AST.h parser/$(TYPE_PARSE_ROOT)-real.h lexer/hac_lex.h" \
		-v namespace="HAC::parser HAC::lexer" \
		-v token_enum=`$(GET_START_TOKEN_ENUM)`

parser/hackt-union.cc: $(HACKT_PARSE_OUTPUT) parser/hackt.types \
		parser/$(HACKT_PARSE_ROOT)-real.h
	@if test ! -s parser/hackt.types ; then \
		$(ECHO) "parser/hackt.types is empty!" ; \
		exit 1 ; \
	fi
	$(AWK) -f $(srcdir)/parser/common-union-type.awk \
		-f $(YACC_UNION_SCRIPT) $(HACKT_YACC_UNION_SCRIPT_ARGS) \
		$(HACKT_PARSE_OUTPUT) > $@

parser/instref-union.cc: $(INSTREF_PARSE_OUTPUT) parser/instref.types \
		parser/$(INSTREF_PARSE_ROOT)-real.h
	@if test ! -s parser/instref.types ; then \
		$(ECHO) "parser/instref.types is empty!" ; \
		exit 1 ; \
	fi
	$(AWK) -f $(srcdir)/parser/common-union-type.awk \
		-f $(YACC_UNION_SCRIPT) $(INSTREF_YACC_UNION_SCRIPT_ARGS) \
		$(INSTREF_PARSE_OUTPUT) > $@

parser/type-union.cc: $(TYPE_PARSE_OUTPUT) parser/type.types \
		parser/$(TYPE_PARSE_ROOT)-real.h
	@if test ! -s parser/type.types ; then \
		$(ECHO) "parser/type.types is empty!" ; \
		exit 1 ; \
	fi
	$(AWK) -f $(srcdir)/parser/common-union-type.awk \
		-f $(YACC_UNION_SCRIPT) $(TYPE_YACC_UNION_SCRIPT_ARGS) \
		$(TYPE_PARSE_OUTPUT) > $@


# configure accumulated AUTO_CPPFLAGS for dependent package includes
AM_CPPFLAGS = -I$(srcdir) @AUTO_CPPFLAGS@
# will need _build directories for "make distcheck" with auto-generated headers
# the last -Iparser is for the relative path in the build directory


# extremely anal-retentive
if HAVE_GXX
# our good friend, g++
GENERAL_WARN_FLAGS = @FANG_WARN_FLAGS@
WARN_CFLAGS = @FANG_WARN_CFLAGS@
WARN_CXXFLAGS = @FANG_WARN_CXXFLAGS@
# -Wsign-promo
DIALECT_FLAGS = @FANG_DIALECT_FLAGS@
PIPE_FLAG = -pipe
else
# non-GNU compiler: your guess is as good as mine
# pass in whatever happened to succeed from configure tests
GENERAL_WARN_FLAGS = @FANG_WARN_FLAGS@
WARN_CFLAGS = @FANG_WARN_CFLAGS@
WARN_CXXFLAGS = @FANG_WARN_CXXFLAGS@
DIALECT_FLAGS = @FANG_DIALECT_FLAGS@
PIPE_FLAG =
endif
# TODO: would like to have these flags available
# at configure time during compile of conftest's.
# KNOWN ISSUE: for example -ansi -pedantic will disable long long
# but unless these flags are given at configure time (say, by the user)
# configure will test things with the wrong flags, producing wrong results. 

MORE_CFLAGS = $(PIPE_FLAG) $(DIALECT_FLAGS) $(WARN_CFLAGS) $(GENERAL_WARN_FLAGS)
MORE_CXXFLAGS = $(PIPE_FLAG) $(DIALECT_FLAGS) $(WARN_CXXFLAGS) $(GENERAL_WARN_FLAGS)
# These flags are used to cancel out previous warning flags, 
# such exceptions are needed for the automatic lexers and parsers
if HAVE_GXX
PARSER_NOWARN_FLAGS = -Wno-unused -Wno-old-style-cast -Wno-shadow
else
# -w inhibits all warning messages, at least for gcc
PARSER_NOWARN_FLAGS = -w
endif

AM_CFLAGS = $(MORE_CFLAGS)
AM_CXXFLAGS = $(MORE_CXXFLAGS)

# configure accumulated AUTO_LDFLAGS for linking LIBS of dependent packages
AM_LDFLAGS = @AUTO_LDFLAGS@

# now extended to include other important flags
am_cxxflags.h: $(srcdir)/Makefile.am
	@{ $(ECHO) "#ifndef	AM_CPPFLAGS" ; \
	$(ECHO) "#define	AM_CPPFLAGS \"$(AM_CPPFLAGS)\"" ; \
	$(ECHO) "#endif	/* AM_CPPFLAGS */" ; \
	$(ECHO) "#ifndef	AM_CXXFLAGS" ; \
	$(ECHO) "#define	AM_CXXFLAGS \"$(AM_CXXFLAGS)\"" ; \
	$(ECHO) "#endif	/* AM_CXXFLAGS */" ; \
	$(ECHO) "#ifndef	AM_LDFLAGS" ; \
	$(ECHO) "#define	AM_LDFLAGS \"$(AM_LDFLAGS)\"" ; \
	$(ECHO) "#endif	/* AM_LDFLAGS */" ;} > $@

# flags needed to produce .output from yacc/bison
AM_YFLAGS = -d -t -v
AM_LFLAGS = 

# for occasional debugging
# SAVETEMP = $(CXXCOMPILE) -v -save-temps
.cc.ii:
	$(CXXPREPROCESS) $< > $@

.c.i:
	$(CPREPROCESS) $< > $@

# yes, even use C++ for .h files because we've (perhaps mistakenly)
# named all C++ header files .h instead of .hh
.h.hii:
	$(CXXPREPROCESS) $< > $@

.hh.hhii:
	$(CXXPREPROCESS) $< > $@

.tcc.tccii:
	$(CXXPREPROCESS) $< > $@

# doesn't work
# YACC_PREFIX = -DYACC_PREFIX=hackt_yy
# libhackt_la_CPPFLAGS = $(YACC_PREFIX) $(AM_CPPFLAGS)

MORE_BOGUS_CPPFLAGS =
MORE_BOGUS_CXXFLAGS =
if HAVE_BYACC
# we don't care about libbogus, whatever it takes to make it compile
MORE_BOGUS_CPPFLAGS += -DYYDEBUG=0
MORE_BOGUS_CXXFLAGS += -Wno-error
endif
# as a workaround for non-standard-conforming yacc skeletons
if HAVE_YACC
MORE_BOGUS_CXXFLAGS += -Wno-error
endif
BOGUS_CPPFLAGS = -DLIBBOGUS $(AM_CPPFLAGS) $(MORE_BOGUS_CPPFLAGS)
BOGUS_CXXFLAGS = $(AM_CXXFLAGS) $(PARSER_NOWARN_FLAGS) $(MORE_BOGUS_CXXFLAGS)

# Conditional definition of __STDC_VERSION__ needed for flex 2.5.33?
INCLUDE_C99 = -include $(srcdir)/util/c99.h

# *COUGH* *HACK*:
# LIBBOGUS tells lexer/hackt-lex.cc to use un-transformed symbol names

# the -include is necessary because flex doesn't respectfully
# include user header files early enough to rename declarations ^@#!%&!@^&%
# really, this isn't necessary since we've prevented yy-name conflicts
# by compiling each lexer/parser pair into a separate bogus convenience library
libbogus_la_CPPFLAGS = $(BOGUS_CPPFLAGS) \
	-include parser/hackt-prefix.h
libbogus_la_CXXFLAGS = $(BOGUS_CXXFLAGS)
libbogus2_la_CPPFLAGS = $(BOGUS_CPPFLAGS) \
	-include parser/instref-prefix.h
libbogus2_la_CXXFLAGS = $(BOGUS_CXXFLAGS)
libbogus3_la_CPPFLAGS = $(BOGUS_CPPFLAGS) \
	-include parser/type-prefix.h
libbogus3_la_CXXFLAGS = $(BOGUS_CXXFLAGS)
libbogus4_la_CXXFLAGS = $(BOGUS_CXXFLAGS)
# here we give the exceptional flags to the lexer and parser, 
# which are generated automatically by yacc/bison/[f]lex for 
# traditional C compilers.  (Not the cleanest code ever generated.)
# libhacktparsers_la_CPPFLAGS = $(AM_CPPFLAGS)
libhacktparsers_la_CXXFLAGS = \
	$(AM_CXXFLAGS) $(PARSER_NOWARN_FLAGS)

# is required to be set for Make.hchk-tail.am
EMPTY_SOURCE = $(srcdir)/test/header_test.cc

# there are a few headers we want exempt from stand-alone testing
# what about their dependencies?  
# automake produces empty placeholders (never updated) for them initially, 
# so make will not die.

common/devel_switches.hchk: common/devel_switches.h
	@$(TOUCH) $@

Object/devel_switches.hchk: Object/devel_switches.h
	@$(TOUCH) $@

sim/devel_switches.hchk: sim/devel_switches.h
	@$(TOUCH) $@

sim/prsim/devel_switches.hchk: sim/prsim/devel_switches.h
	@$(TOUCH) $@

__config__.hchk: __config__.h
	@$(TOUCH) $@

EXTRA_DIST += local.cvsignore \
	parser/local.cvsignore \
	parser/lexyacc-prefix.awk \
	parser/bison-output-to-C.awk \
	parser/common-output-to-C.awk \
	parser/yacc-output-to-C.awk \
	parser/common-union-type.awk \
	parser/yacc-union-type.awk \
	parser/bison-union-type.awk \
	parser/yyerror_bison_hack.awk \
	parser/nls_bison_hack.awk \
	parser/yygrowstack_yacc_hack.awk \
	parser/patch_byacc.awk \
	parser/purify_flex.awk \
	parser/purify_yacc.awk \
	parser/try_lex_catch.awk \
	parser/yacc_const_char.awk \
	parser/NOTES \
	parser/hackt-parse.yy.in \
	lexer/local.cvsignore \
	lexer/flex_lexer_state.h \
	lexer/purify_flex.awk \
	test/local.cvsignore \
	test/header_test.cc \
	test/getopt_test.sh \
	test/getopt_test.stderr \
	common/README \
	scripts/filter_namespaces.sed \
	scripts/deps_to_dot.awk.in \
	AST/README \
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
	BUGS \
	TODO \
	wc-l \
	sim/README \
	sim/prsim/README \
	sim/prsim/TODO \
	sim/chpsim/README \
	sim/chpsim/TODO

TESTS += hackt_persistent_table.sh test/getopt_test.test

installcheck-local:
	@list='$(bin_PROGRAMS)' ; \
	for p in $$list ; do \
		p1=`echo $$p|$(SED) 's/$(EXEEXT)$$//'`; \
		if test -f $$p || test -f $$p1 ; then \
			f=`echo "$$p1" | $(SED) 's,^.*/,,;$(transform);s/$$/$(EXEEXT)/'`; \
			run=$(bindir)/$$f ; \
			if test ! -f $$run ; then \
				$(ECHO) "*** DID YOU FORGET TO make install FIRST? ***" ; \
			fi ; \
			$(ECHO) "======== running: $$run ================" ; \
			$$run || : ; \
			$(ECHO) "======== running: $$run version ========" ; \
			$$run version || : ; \
		else : ; \
		fi; \
	done
	@list='$(bin_SCRIPTS)' ; \
	for p1 in $$list ; do \
		f=`echo "$$p1" | $(SED) 's|^.*/||;$(transform)'`; \
		run=$(bindir)/$$f ; \
		if test ! -f $$run ; then \
			$(ECHO) "*** DID YOU FORGET TO make install FIRST? ***" ; \
		fi ; \
		$(ECHO) "======== running: $$run ==============" ; \
		$$run || : ; \
	done
	@$(ECHO) "======== end of installcheck ========"

# test/getopt_test.test
CLEANFILES += $(AUTOHEADERS) config.log
DISTCLEANFILES += hackt_persistent_table.sh $(VERSIONHEADERS)
AUTO_IGNORE += $(TESTS) $(PROGRAMS) $(LTLIBRARIES) $(CONFIG_PARSERS) \
	$(IGNOREHEADERS) config.log

test/getopt_test.test: getopt_test test/getopt_test.sh
	@{ $(ECHO) "#!$(SHELL)" ; \
	$(ECHO) "$(srcdir)/$*.sh > $*.output 2>&1" ; \
	$(ECHO) "$(DIFF) -u $(srcdir)/$*.stderr $*.output > $*.diff" ; \
	$(ECHO) "if [ -s $*.diff ]" ; \
	$(ECHO) then ; \
	  $(ECHO) "$(ECHO) \"libc getopt behavior differs from expectation!\"" ; \
	  $(ECHO) "$(CAT) $*.diff" ; \
	  $(ECHO) "exit 1" ; \
	$(ECHO) fi ;} > $@ ; \
	$(CHMOD) +x $@

DISTCLEANFILES += test/getopt_test.test \
	test/getopt_test.diff \
	test/getopt_test.output

hackt_persistent_table.sh: hackt
	@{ $(ECHO) "#!/bin/sh" ; \
	$(ECHO) "./hackt dump_persistent_table" ;} > $@ ; \
	$(CHMOD) +x $@

# choosing not to distribute lex/yacc-generated sources for now
CLEANPARSER = \
	parser/$(HACKT_PARSE_ROOT).cc \
	parser/$(HACKT_PARSE_ROOT).h \
	parser/$(HACKT_PARSE_ROOT)-real.cc \
	parser/$(HACKT_PARSE_ROOT)-real.h \
	parser/$(HACKT_PARSE_ROOT).output \
	parser/$(HACKT_PARSE_ROOT).output.h \
	parser/hackt-union.cc \
	parser/hackt.types \
	parser/$(INSTREF_PARSE_ROOT).cc \
	parser/$(INSTREF_PARSE_ROOT).h \
	parser/$(INSTREF_PARSE_ROOT)-real.cc \
	parser/$(INSTREF_PARSE_ROOT)-real.h \
	parser/$(INSTREF_PARSE_ROOT).output \
	parser/$(INSTREF_PARSE_ROOT).output.h \
	parser/instref-union.cc \
	parser/instref.types \
	parser/$(TYPE_PARSE_ROOT).cc \
	parser/$(TYPE_PARSE_ROOT).h \
	parser/$(TYPE_PARSE_ROOT)-real.cc \
	parser/$(TYPE_PARSE_ROOT)-real.h \
	parser/$(TYPE_PARSE_ROOT).output \
	parser/$(TYPE_PARSE_ROOT).output.h \
	parser/type-union.cc \
	parser/type.types

CLEANLEXER = \
	lexer/$(HACKT_LEX_ROOT)-real.cc \
	lexer/$(HACKT_LEX_ROOT).cc \
	lexer/$(HACFLAT_LEX_ROOT)-real.cc \
	lexer/$(HACFLAT_LEX_ROOT).cc \
	lexer/$(INSTREF_LEX_ROOT)-real.cc \
	lexer/$(INSTREF_LEX_ROOT).cc \
	lexer/$(TYPE_LEX_ROOT)-real.cc \
	lexer/$(TYPE_LEX_ROOT).cc

libs: $(LTLIBRARIES)

AUTO_IGNORE += $(CLEANPARSER) $(CLEANLEXER)
DISTCLEANFILES += $(CLEANPARSER) $(CLEANLEXER)

OBJECT_DIRS = Object Object/common Object/expr Object/def Object/inst \
	Object/lang Object/ref Object/traits Object/type Object/unroll

SIM_DIRS = sim sim/prsim sim/chpsim

CLEANSUBDIRS += lexer parser AST $(OBJECT_DIRS) $(SIM_DIRS)

clean-parser:
	-$(RM) $(CLEANPARSER)
	-$(RM) parser/*.$(OBJEXT) parser/*.lo

clean-lexer:
	-$(RM) $(CLEANLEXER)
	-$(RM) lexer/*.$(OBJEXT) lexer/*.lo

clean-AST:
	-$(RM) AST/*.$(OBJEXT)
	-$(RM) AST/*.lo

# could just rm `find ... `
clean-Object:
	for d in $(OBJECT_DIRS) ; do \
		$(RM) $$d/*.$(OBJEXT) ; \
		$(RM) $$d/*.lo ; \
	done

clean-sim:
	for d in $(SIM_DIRS) ; do \
		$(RM) $$d/*.$(OBJEXT) ; \
		$(RM) $$d/*.lo ; \
	done

# implies CLEANFILES
clean-local: clean-AST clean-Object clean-lexer clean-parser clean-sim \
		clean-header-tests
	for d in $(CLEANSUBDIRS); do \
		$(RM) $$d/*.loT ; \
	done

# cvsignore targets
if AUTO_CVSIGNORE
$(srcdir)/lexer/.cvsignore: $(srcdir)/.cvsignore lexer/local.cvsignore
	$(CAT) $(srcdir)/.cvsignore $(srcdir)/lexer/local.cvsignore > $@
$(srcdir)/parser/.cvsignore: $(srcdir)/.cvsignore parser/local.cvsignore
	$(CAT) $(srcdir)/.cvsignore $(srcdir)/parser/local.cvsignore > $@
$(srcdir)/common/.cvsignore: $(srcdir)/.cvsignore
	$(CP) $< $@
$(srcdir)/Object/.cvsignore: $(srcdir)/.cvsignore
	$(CP) $< $@
$(srcdir)/Object/common/.cvsignore: $(srcdir)/.cvsignore
	$(CP) $< $@
$(srcdir)/Object/def/.cvsignore: $(srcdir)/.cvsignore
	$(CP) $< $@
$(srcdir)/Object/expr/.cvsignore: $(srcdir)/.cvsignore
	$(CP) $< $@
$(srcdir)/Object/inst/.cvsignore: $(srcdir)/.cvsignore
	$(CP) $< $@
$(srcdir)/Object/lang/.cvsignore: $(srcdir)/.cvsignore
	$(CP) $< $@
$(srcdir)/Object/ref/.cvsignore: $(srcdir)/.cvsignore
	$(CP) $< $@
$(srcdir)/Object/traits/.cvsignore: $(srcdir)/.cvsignore
	$(CP) $< $@
$(srcdir)/Object/type/.cvsignore: $(srcdir)/.cvsignore
	$(CP) $< $@
$(srcdir)/Object/unroll/.cvsignore: $(srcdir)/.cvsignore
	$(CP) $< $@
$(srcdir)/AST/.cvsignore: $(srcdir)/.cvsignore
	$(CP) $< $@
$(srcdir)/main/.cvsignore: $(srcdir)/.cvsignore
	$(CP) $< $@
$(srcdir)/sim/.cvsignore: $(srcdir)/.cvsignore
	$(CP) $< $@
$(srcdir)/sim/prsim/.cvsignore: $(srcdir)/.cvsignore
	$(CP) $< $@
$(srcdir)/sim/chpsim/.cvsignore: $(srcdir)/.cvsignore
	$(CP) $< $@
$(srcdir)/test/.cvsignore: $(srcdir)/.cvsignore test/local.cvsignore
	@$(CAT) $(srcdir)/.cvsignore $(srcdir)/test/local.cvsignore > $@

# files to ignore in subdirectories
# can we generate this from suffix appending?
SUBDIR_CVSIGNORE += \
	$(srcdir)/lexer/.cvsignore \
	$(srcdir)/parser/.cvsignore \
	$(srcdir)/common/.cvsignore \
	$(srcdir)/Object/.cvsignore \
	$(srcdir)/Object/common/.cvsignore \
	$(srcdir)/Object/def/.cvsignore \
	$(srcdir)/Object/expr/.cvsignore \
	$(srcdir)/Object/inst/.cvsignore \
	$(srcdir)/Object/lang/.cvsignore \
	$(srcdir)/Object/ref/.cvsignore \
	$(srcdir)/Object/traits/.cvsignore \
	$(srcdir)/Object/type/.cvsignore \
	$(srcdir)/Object/unroll/.cvsignore \
	$(srcdir)/AST/.cvsignore \
	$(srcdir)/main/.cvsignore \
	$(srcdir)/sim/.cvsignore \
	$(srcdir)/sim/prsim/.cvsignore \
	$(srcdir)/sim/chpsim/.cvsignore \
	$(srcdir)/test/.cvsignore

cvsignore: $(SUBDIR_CVSIGNORE)

clean-cvsignore: clean-cvsignore-subdirs

clean-cvsignore-subdirs:
	-$(RM) $(SUBDIR_CVSIGNORE)
endif
# Make.global already has:
# all: cvsignore

clean-libs:
	-$(RM) $(LTLIBRARIES)

clean-scripts:
	-$(RM) $(SCRIPTS)

DISTCLEANFILES += $(SUBDIR_CVSIGNORE) $(CONFIG_PARSERS)
DISTCLEANDIRS += $(DEPDIR)

distclean-local: clean-parser clean-lexer distclean-deps

distclean-deps:
	-$(RM) -r `$(FIND) . -name .deps -type d`

dist-hook:
	for f in $(DISTCLEANFILES) $(CLEANFILES); do \
		$(RM) $(distdir)/$$f ; \
	done

# useful for rapidly turning off debugging
show-stacktrace-sources: force
	-$(GREP) ENABLE_STACKTRACE */{,*}/*.{tcc,cc} | $(GREP) 1 | \
		$(GREP) -v "util/test" | $(GREP) -v "util/stacktrace"

help-local:
	-@$(CALLMAKE) help-check-headers 2> /dev/null
	@$(ECHO) "Local make targets:" ; \
	$(ECHO) "    hackt: the main executable" ; \
	$(ECHO) "    libs: all (libtool) libraries" ; \
	$(ECHO) "    libhackt.la: core library" ; \
	$(ECHO) "    libhacktparsers.la: lexers/parsers library" ; \
	$(ECHO) "    libhacktexec.la: subprogram library" ; \
	$(ECHO) "    libfangutil.la: common utilities library" ; \
	$(ECHO) "    clean-AST: cleans AST subdir's objects" ; \
	$(ECHO) "    clean-Object: cleans Object subdir's objects" ; \
	$(ECHO) "    clean-lexer: cleans lexer subdir's objects" ; \
	$(ECHO) "    clean-parser: cleans parser subdir's objects" ; \
	$(ECHO) "    clean-libs: cleans all libraries and archives" ; \
	$(ECHO) "    clean-scripts: cleans built bin scripts" ; \
	$(ECHO) "    show-stacktrace-sources: grep for debug-enabled sources" ; \
	$(ECHO) "Local echo-* targets:" ; \
	$(ECHO) "    libhacktexec-sources"

.PHONY: distclean-deps distclean-local dist-hook

# standard targets
include $(srcdir)/Make.hchk-tail.am
# includes "Make.global"

