/**
	\file "Object/module.cc"
	Method definitions for module class.  
	This file was renamed from "Object/art_object_module.cc".
 	$Id: module.cc,v 1.48 2011/04/02 01:45:53 fang Exp $
 */

// code debugging switches
#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_DESTRUCTORS		(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_PERSISTENTS		(0 && ENABLE_STACKTRACE)

#include "Object/module.tcc"
#include <iostream>
#include "Object/common/namespace.h"
#include "Object/unroll/unroll_context.h"
#include "Object/unroll/expression_assignment.h"
#include "Object/unroll/instantiation_statement.h"
#include "Object/unroll/param_instantiation_statement.h"
// #include "Object/traits/value_traits.h"
#include "Object/persistent_type_hash.h"
#include "Object/inst/physical_instance_collection.h"
#include "Object/lang/cflat_printer.h"
#include "Object/lang/PRS_footprint.h"
#include "Object/lang/SPEC_footprint.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/meta_range_list.h"
#include "Object/type/process_type_reference.h"
#include "Object/type/canonical_type.h"
#include "Object/def/process_definition.h"
#include "Object/global_entry.h"
#include "Object/traits/proc_traits.h"
#include "AST/token_string.h"		// blech, cyclic library dep?

#if ENABLE_STACKTRACE
#include "Object/common/dump_flags.h"
#endif

#include "main/cflat_options.h"
#include "util/persistent_object_manager.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/stacktrace.h"
#include "util/indent.h"
#include "util/value_saver.h"
#include "common/TODO.h"

namespace util {
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::module, MODULE_TYPE_KEY, 0)
namespace memory {
using HAC::entity::module;
template class count_ptr<module>;
template class count_ptr<const module>;
}	// end namespace memory
}	// end namespace util

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using std::istream;
using util::write_value;
using util::read_value;
using util::write_string;
using util::read_string;
using util::persistent_traits;
using util::auto_indent;
using util::string_list;
using util::value_saver;

//=============================================================================
/**
	Just a convenience definition.  
 */
static 
const count_ptr<const const_param_expr_list> null_module_params(NULL);

//=============================================================================
// class module method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
module::module() :
		process_definition(), 
		global_namespace(NULL), 
		compile_opts()
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
module::module(const string& s) :
		process_definition(s), 
		global_namespace(new name_space(""))
		{
	NEVER_NULL(global_namespace);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
module::~module() {
	STACKTRACE_DTOR("~module()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const footprint&
module::get_footprint(void) const {
	// return parent_type::get_footprint(null_module_params);
	return footprint_map.only();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint&
module::get_footprint(void) {
	// return parent_type::get_footprint(null_module_params);
	return footprint_map.only(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return pointer to the global namespace.  
 */
never_ptr<name_space>
module::get_global_namespace(void) const {
	return global_namespace;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sets the global namespace for this module.
	\param n owned (and transferred) pointer to new namespace.  
 */
void
module::set_global_namespace(excl_ptr<name_space>& n) {
	// automatically memory-managed
	global_namespace = n;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param l the list in which to accumulate collection of 
		pointers to namespaces.  
 */
void
module::collect_namespaces(namespace_collection_type& l) const {
	INVARIANT(global_namespace);
	l.push_back(global_namespace);
	global_namespace->collect_namespaces(l);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
module::what(ostream& o) const {
	return o << "module";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Copy-modified from process_definition::dump().
 */
ostream&
module::dump_definitions(ostream& o) const {
	o << "In module created from: " << key;
	if (is_created())
		o << " (unrolled) (created)";
	o << endl;

	global_namespace->dump(o) << endl;
	const expr_dump_context& dc(expr_dump_context::default_value);
	if (!is_created()) {
		o << "Sequential instance management (to unroll): " << endl;
		sequential_scope::dump(o, dc);
	}
	// PRS
	if (!prs.empty()) {
		o << auto_indent << "top-level prs:" << endl;
		INDENT_SECTION(o);
		const PRS::rule_dump_context rdc(*global_namespace);
		prs.dump(o, rdc) << endl;
	}
	// CHP
	if (!chp.empty()) {
		o << auto_indent << "top-level chp:" << endl;
		INDENT_SECTION(o);
		chp.dump(o << auto_indent, dc) << endl;
	}
	// SPEC
	if (!spec.empty()) {
		o << auto_indent << "top-level spec:" << endl;
		INDENT_SECTION(o);
		const PRS::rule_dump_context rdc(*this);
		spec.dump(o, rdc);	// << endl;
	}
	if (is_created()) {
		footprint_map.dump(o, expr_dump_context::default_value) << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
module::dump(ostream& o) const {
	dump_definitions(o);
	if (is_created()) {
		o << "Globally allocated state:" << endl;
		get_footprint().dump_allocation_map(o);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
module::is_created(void) const {
	return get_footprint().is_created();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates placeholder footprints depedent types bottom up.
	This replays internal aliases from the leaf-most types up.  
	This routine is modeled after footprint::create_dependent_types.  
	Need to figure out why we can't just use that...
	TODO: reuse process_definition::create_dependent_type.
 */
good_bool
module::create_dependent_types(void) {
	// pass options globally
	const value_saver<create_options>
		_copt_(global_create_options, compile_opts.create_opts);
//	global_create_options.dump(cerr);
try {
	footprint& f(get_footprint());
	if (!parent_type::__create_complete_type(
			null_module_params, f, f).good) {
		return good_bool(false);
	}
	// this is actually not necessary because the top-level footprint
	// doesn't register !Vdd and !GND as ports.  
	f.zero_top_level_ports();
	return good_bool(true);
} catch (...) {
	// generic error message
	return good_bool(false);
}
}	// end method create_dependent_types

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Replays all instantiation statements and allocates unique
	space to each alias recursively.  
	\return 'good' if successful.  
 */
good_bool
module::create_unique(void) {
	STACKTRACE_VERBOSE;
	if (!is_created()) {
		STACKTRACE("not already created, creating...");
		// this replays all internal aliases recursively
		// and assigns local instance IDs
		if (!create_dependent_types().good) {
			static const char err[] =
				"Error encountered during module::create.";
			cerr << err << endl;
			return good_bool(false);
		}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
module::allocate_unique(void) {
	return create_unique();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Kludge: detect instance-management statements that deal 
	only with parameter values, not physical instances, 
	nor other language bodies.  
	Implementation here is hideous...
	FIXME: will this miss conditionals and loops?
		perhaps a recursive visitor-idiom would be more appropriate?
 */
static
bool
instance_management_of_values(const instance_management_base* m) {
	if (
		IS_A(const instantiation_statement<pbool_tag>*, m) ||
		IS_A(const instantiation_statement<pint_tag>*, m) ||
		IS_A(const instantiation_statement<preal_tag>*, m) ||
		IS_A(const expression_assignment<pbool_tag>*, m) ||
		IS_A(const expression_assignment<pint_tag>*, m) ||
		IS_A(const expression_assignment<preal_tag>*, m)
		) {
		return true;
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Takes process definition and unroll its contents into the top-level.  
	\param top_module the true top-level module is used to 
		unroll parameter values and their assignments, for the 
		purposes of global parameter lookup.  
 */
good_bool
module::allocate_unique_process_type(const process_type_reference& pt, 
		const module& top_module) {
	STACKTRACE_VERBOSE;
	const canonical_process_type cpt(pt.make_canonical_type());
	if (!cpt) {
		cerr << "Error resolving canonical type." << endl;
		return good_bool(false);
	}
	const never_ptr<const process_definition>
		pd(cpt.get_base_def());
	const count_ptr<const const_param_expr_list>&
		tp(cpt.get_raw_template_params());
	// only want to clobber physical instances, leave top-level parameters
	footprint& _footprint(get_footprint());
	// need to import definition's local symbols (deep copy) into
	// this temporary module's global namespace and footprint.  
#if 0
	// need to unroll all global value parameters first
	// what to do about overshadowed parameters?
	const unroll_context c(&_footprint, &_footprint);
	if (!top_module.unroll_if(c, &instance_management_of_values).good) {
		cerr << "Error unrolling top-level parameter values." << endl;
		return good_bool(false);
	}
	global_namespace->import_physical_instances(*pd);
	// need to remove shadowed value collection, since they won't be
	// visible to the process-type within its scope anyways...
	_footprint.remove_shadowed_collections(*pd);
	// non-shadowed values remain intact for lookup during next create phase
#else
	if (!__import_global_parameters(top_module, *pd).good) {
		return good_bool(false);
	}
#endif
#if ENABLE_STACKTRACE
	pd->dump(cerr << "process definition: ") << endl;
	_footprint.dump_with_collections(cerr << "module\'s footprint: ", 
		dump_flags::default_value,
		expr_dump_context::default_value) << endl;
#endif
	if (!pd->__create_complete_type(tp, _footprint, _footprint).good) {
		cerr << "Error creating complete process type." << endl;
		return good_bool(false);
	} else {
		_footprint.zero_top_level_ports();
#if ENABLE_STACKTRACE
		_footprint.dump(cerr << "footprint:" << endl) << endl;
#endif
		return allocate_unique();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Takes process type and instantiate one at the top-level, 
		erasing all other physical instances.  
		Top-instance cannot possible alias to anything else, 
		so it *should* be safe to allocate-assign it.  
		Should we assert that nothing else was instantiated
		at the top-level? or at least warn?
		This just appends an instance...
	\param top_module the true top-level module is used to 
		unroll parameter values and their assignments, for the 
		purposes of global parameter lookup.  
 */
good_bool
module::allocate_single_process(
		const count_ptr<const process_type_reference>& pt,
		const module& top_module) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(pt);
	// grab global parameters
	const never_ptr<const process_definition>
		pdef(pt->get_base_def().is_a<const process_definition>());
	NEVER_NULL(pdef);
	if (!__import_global_parameters(top_module, *pdef).good) {
		return good_bool(false);
	}
	const count_ptr<instantiation_statement_base>
		pi(fundamental_type_reference::make_instantiation_statement(
			pt, index_collection_item_ptr_type(NULL)));
	// follows parser::parse_context::add_instance
	if (!pi) {
		cerr << "Error instantiating process type ";
		pt->dump(cerr) << endl;
		return good_bool(false);
	}
	const never_ptr<const instance_placeholder_base>
		pp(scopespace::add_instance(pi, 
			token_identifier("***RUMPELSTILTSKIN***"), false));
	NEVER_NULL(pp);
	footprint& _footprint(get_footprint());
	// _footprint.clear_nonglobal_physical_collections();	// ?
	// keep !Vdd and !GND
	// need to unroll all global value parameters first
	// what to do about overshadowed parameters?
	const unroll_context c(&_footprint, &_footprint);
	if (pi->unroll(c).good) {
		_footprint.create_dependent_types(c);
		// force to reallocate new instance (fragile!)
		// since there are no new connections, no new aliases are formed
		// do NOT want to call process_definition::unroll_lang.
		return allocate_unique();
	}
	else return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This effectively wipes the work done by allocation, reverting back to
	a pre-allocated state.  
 */
void
module::reset(void) {
	STACKTRACE_VERBOSE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
module::__import_global_parameters(const module& m,
		const process_definition& pd) {
	footprint& _footprint(get_footprint());
#if 0
	_footprint.import_global_parameters(m.get_footprint());
#else
	// need to unroll all global value parameters first
	// what to do about overshadowed parameters?
	const unroll_context c(&_footprint, &_footprint);
	if (!m.unroll_if(c, &instance_management_of_values).good) {
		cerr << "Error unrolling top-level parameter values." << endl;
		return good_bool(false);
	}
	global_namespace->import_physical_instances(pd);
	// need to remove shadowed value collection, since they won't be
	// visible to the process-type within its scope anyways...
	_footprint.remove_shadowed_collections(pd);
	// non-shadowed values remain intact for lookup during next create phase
#endif
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
module::__cflat_rules(const footprint& _footprint, 
		ostream& o, const cflat_options& cf) {
	STACKTRACE_VERBOSE;
	// our printing visitor functor
	const footprint_frame ff(_footprint);	// empty ports
	global_offset g;	// 0s
	PRS::cflat_prs_printer cfp(ff, g, o, cf);
	if (cf.include_prs) {
		STACKTRACE("cflatting production rules.");
		if (cf.dsim_prs)	o << "dsim {" << endl;
		try {
			_footprint.accept(cfp);
		} catch (...) {
			cerr << "Caught exception during cflat PRS." << endl;
			return good_bool(false);
		}
		if (cf.dsim_prs)	o << "}" << endl;
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Top-level footprint is actually empty so we need to load it first...
	We promise to clean it up after we're done, upon destruction
		of the top_level_footprint_importer helper class.
 */
good_bool
module::__cflat_aliases(const footprint& _footprint, 
		ostream& o, const cflat_options& cf) {
	STACKTRACE_VERBOSE;
	if (cf.connect_style) {
		_footprint.cflat_aliases(o, cf);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The actual cflat procedure.  
	Always prints all rules before aliases.  
	TODO: if necessary, make the order an option
	TODO: implement instance visitor pattern, including the footprint.  
 */
good_bool
module::__cflat(const footprint& _f, 
		ostream& o, const cflat_options& cf) {
	STACKTRACE_VERBOSE;
	// print the production rules first, using canonical names
	// print the name aliases in the manner requested in cflat_options
	return good_bool(__cflat_rules(_f, o, cf).good &&
			__cflat_aliases(_f, o, cf).good);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Since this variant is const, this expects the module to be
	already allocated, and will not perform any modifications.  
 */
good_bool
module::cflat(const footprint& _f,
		ostream& o, const cflat_options& cf) {
	STACKTRACE_VERBOSE;
if (_f.is_created()) {
	return __cflat(_f, o, cf);
} else {
	cerr << "ERROR: Module is not globally allocated, "
		"as required by cflat." << endl;
	return good_bool(false);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sweeps entire module structure for persistently managed objects.  
 */
void
module::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	STACKTRACE_PERSISTENT_VERBOSE;
	global_namespace->collect_transient_info(m);
	// the list itself is a statically allocated member
	parent_type::collect_transient_info_base(m);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
module::write_object(const persistent_object_manager& m, ostream& f) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	m.write_pointer(f, global_namespace);
	compile_opts.write_object(f);		// record compile options
	// need options *before* body because reconstruction depends on it
	parent_type::write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Side effect: sets global_create_options
 */
void
module::load_object(const persistent_object_manager& m, istream& f) {
	STACKTRACE_PERSISTENT_VERBOSE;
	m.read_pointer(f, global_namespace);
//	global_namespace->load_object(m);	// not necessary
	compile_opts.load_object(f);
	// footprint reconstruction depends on global_create_options
	// can't use value_saver because footprints are loaded
	// outside of this scope by the persistent_object_manager
	// thus, we need to permanently modify it as a side-effect
	// The module is always the first (top-level) object, 
	// so this is guaranteed to occur before any other definitions'
	// footprints are processed.
	global_create_options = compile_opts.create_opts;
	parent_type::load_object_base(m, f);
	// since instance pools are reconstructed,
	// we need to override the top-level module to have no ports
	get_footprint().zero_top_level_ports();
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

// clean up macros used in this module
#undef	ENABLE_STACKTRACE
#undef	STACKTRACE_DESTRUCTORS
#undef	STACKTRACE_DTOR
#undef	STACKTRACE_PERSISTENTS
#undef	STACKTRACE_PERSISTENT

