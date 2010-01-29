/**
	\file "Object/module.cc"
	Method definitions for module class.  
	This file was renamed from "Object/art_object_module.cc".
 	$Id: module.cc,v 1.41.2.5 2010/01/29 02:39:42 fang Exp $
 */

#ifndef	__HAC_OBJECT_MODULE_CC__
#define	__HAC_OBJECT_MODULE_CC__

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
		global_namespace(NULL)
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		, allocated(false), global_state()
#endif
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
module::module(const string& s) :
		process_definition(s), 
		global_namespace(new name_space(""))
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		, allocated(false), global_state()
#endif
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
ostream&
module::dump_instance_map(ostream& o) const {
	if (is_allocated()) {
		o << "Globally allocated state:" << endl;
		const footprint& _footprint(get_footprint());
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
		// faked
		_footprint.dump_allocation_map(o);
#else
		global_state.dump(o, _footprint);
#endif
	}
	return o;
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
		dump_instance_map(o);
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
try {
	footprint& f(get_footprint());
	if (!parent_type::__create_complete_type(
			null_module_params, f, f).good) {
		return good_bool(false);
	}
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
	Will automatically unroll the object if it hasn't already 
	been unrolled.  
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
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
/**
	Allocates unique global entries for instance objects.  
 */
good_bool
module::__allocate_unique(void) {
	if (!is_allocated()) {
		STACKTRACE("not already allocated, allocating...");
		// we've established uniqueness among public ports
		// now go through footprint and recursively allocate
		// substructures in the footprint.  
		const footprint& _footprint(get_footprint());
		if (!_footprint.expand_unique_subinstances(global_state).good) {
			return good_bool(false);
		}
		global_state.optimize_pools();
#if 0
		// only for debugging
		global_state.cache_process_parent_refs();
		global_state.dump(cerr, _footprint) << endl;
#endif
		allocated = true;
	}
	return good_bool(true);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
module::allocate_unique(void) {
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	return create_unique();
#else
	if (!create_unique().good)
		return good_bool(false);
	else return __allocate_unique();
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Normally, the state_manager's top-level process, indexed at 0, 
	has an empty footprint_frame because each entry's value
	would be equal to its index.  
	This 'special' case creates a need for error-prone practice, 
	and unecessary code replication, so for consistency, 
	we provide a function to automatically load proces[0]'s frame.
	Consider moving this automatically into the allocate() methods...
 */
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
void
module::populate_top_footprint_frame(void) {
	global_entry<process_tag>&
		ptop(global_state.get_pool<process_tag>()[0]);
//	global_entry_dumper g(cerr, global_state, get_footprint());
//	ptop.dump_frame_only(cerr << "before:") << endl;
	ptop.initialize_top_frame(get_footprint());
//	ptop.dump_frame_only(cerr << "after:") << endl;
}
#endif

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
	// HIJACK!!! copy-overwrite module's global namespace
	AS_A(scopespace&, *global_namespace) = *pd;
#else
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
#if ENABLE_STACKTRACE
		_footprint.dump(cerr << "footprint:" << endl) << endl;
#endif
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
		return allocate_unique();
#else
		return __allocate_unique();
#endif
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
		const count_ptr<const process_type_reference>& pt) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(pt);
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
	pi->unroll(c);
	_footprint.create_dependent_types(_footprint);
	// force to reallocate new instance (fragile!)
	// since there are no new connections, no new aliases are formed
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	allocated = false;
#endif
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	return allocate_unique();
#else
	return __allocate_unique();
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This effectively wipes the work done by allocation, reverting back to
	a pre-allocated state.  
 */
void
module::reset(void) {
	STACKTRACE_VERBOSE;
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	global_state.clear();
	allocated = false;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
module::__cflat_rules(ostream& o, const cflat_options& cf) const {
	// our priting visitor functor
	PRS::cflat_prs_printer cfp(o, cf);
	const cflat_context::module_setter tmp(cfp, *this);
	if (cf.include_prs) {
		STACKTRACE("cflatting production rules.");
		if (cf.dsim_prs)	o << "dsim {" << endl;
		try {
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
			global_state.accept(cfp);	// print!
#endif
			// support for top-level prs!
			// const top_level_footprint_importer foo(*this);
			const footprint& _footprint(get_footprint());
			_footprint.get_prs_footprint().accept(cfp);
			_footprint.get_spec_footprint().accept(cfp);
			// no flat CHP yet (hopefully ever)
			// _footprint.get_chp_footprint().accept(cfp);
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
module::__cflat_aliases(ostream& o, const cflat_options& cf) const {
	// TODO: instance_visitor
	if (cf.connect_style) {
		STACKTRACE("cflatting aliases.");
		const footprint& _footprint(get_footprint());
		_footprint.cflat_aliases(o,
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
			global_state,
#endif
			cf);
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
module::__cflat(ostream& o, const cflat_options& cf) const {
	STACKTRACE_VERBOSE;
	// print the production rules first, using canonical names
	// print the name aliases in the manner requested in cflat_options
	return good_bool(__cflat_rules(o, cf).good &&
			__cflat_aliases(o, cf).good);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Since this variant is const, this expects the module to be
	already allocated, and will not perform any modifications.  
 */
good_bool
module::cflat(ostream& o, const cflat_options& cf) const {
	STACKTRACE_VERBOSE;
if (is_allocated()) {
	return __cflat(o, cf);
} else {
	cerr << "ERROR: Module is not globally allocated, "
		"as required by cflat." << endl;
	return good_bool(false);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This variant will automatically globally allocate unique 
	instances before cflat-ting, if needed.  
 */
good_bool
module::cflat(ostream& o, const cflat_options& cf) {
	STACKTRACE_VERBOSE;
if (allocate_unique().good) {
	return __cflat(o, cf);
} else {
	cerr << "ERROR: during global allocated of module" << endl;
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
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	const footprint& _footprint(get_footprint());
	global_state.collect_transient_info_base(m, _footprint);
#endif
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
module::write_object(const persistent_object_manager& m, ostream& f) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	m.write_pointer(f, global_namespace);
	parent_type::write_object_base(m, f);
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	const footprint& _footprint(get_footprint());
	write_value(f, allocated);
	global_state.write_object_base(m, f, _footprint);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
module::load_object(const persistent_object_manager& m, istream& f) {
	STACKTRACE_PERSISTENT_VERBOSE;
	m.read_pointer(f, global_namespace);
//	global_namespace->load_object(m);	// not necessary
	parent_type::load_object_base(m, f);
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	const footprint& _footprint(get_footprint());
	read_value(f, allocated);
	global_state.load_object_base(m, f, _footprint);
#endif
}

//=============================================================================
// explicit template instantiations

#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
template
void
module::match_aliases<bool_tag>(string_list&, const size_t) const;

template
void
module::match_aliases<int_tag>(string_list&, const size_t) const;

template
void
module::match_aliases<enum_tag>(string_list&, const size_t) const;

template
void
module::match_aliases<channel_tag>(string_list&, const size_t) const;

template
void
module::match_aliases<process_tag>(string_list&, const size_t) const;
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

// clean up macros used in this module
#undef	ENABLE_STACKTRACE
#undef	STACKTRACE_DESTRUCTORS
#undef	STACKTRACE_DTOR
#undef	STACKTRACE_PERSISTENTS
#undef	STACKTRACE_PERSISTENT

#endif	// __HAC_OBJECT_MODULE_CC__

