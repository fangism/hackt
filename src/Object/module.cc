/**
	\file "Object/module.cc"
	Method definitions for module class.  
	This file was renamed from "Object/art_object_module.cc".
 	$Id: module.cc,v 1.26.4.1 2006/11/05 01:22:59 fang Exp $
 */

#ifndef	__HAC_OBJECT_MODULE_CC__
#define	__HAC_OBJECT_MODULE_CC__

// code debugging switches
#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_DESTRUCTORS		0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS		0 && ENABLE_STACKTRACE

#include "Object/module.tcc"
#include <iostream>
#include "Object/common/namespace.h"
#include "Object/unroll/unroll_context.h"
#include "Object/persistent_type_hash.h"
#include "Object/inst/physical_instance_collection.h"
#include "Object/lang/cflat_printer.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/type/process_type_reference.h"
#include "Object/type/canonical_type.h"
#include "Object/def/process_definition.h"

#if ENABLE_STACKTRACE
#include "Object/common/dump_flags.h"
#endif

#include "main/cflat_options.h"
#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.h"
#include "util/indent.h"
#include "common/TODO.h"

namespace util {
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::module, MODULE_TYPE_KEY, 0)
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
		allocated(false), global_state() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
module::module(const string& s) :
		process_definition(s), 
		global_namespace(new name_space("")),
		allocated(false), global_state() {
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
	return footprint_map.only();
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
module::dump(ostream& o) const {
	o << "In module created from: " << key;
	if (is_unrolled())
		o << " (unrolled)";
	if (is_created())
		o << " (created)";
	o << endl;

	global_namespace->dump(o) << endl;
	const footprint& _footprint(get_footprint());
	if (!is_unrolled()) {
		o << "Sequential instance management (to unroll): " << endl;
		sequential_scope::dump(o,
			expr_dump_context::default_value);
		if (!prs.empty()) {
			o << auto_indent << "top-level prs:" << endl;
			INDENT_SECTION(o);
			const PRS::rule_dump_context rdc(*global_namespace);
			prs.dump(o, rdc) << endl;
		}
		return o;
	} else {
		footprint_map.dump(o, expr_dump_context::default_value)
			<< endl;
	}
	if (is_allocated()) {
#if 0
		// only for debugging
		global_state.cache_process_parent_refs();
#endif
		o << "Globally allocated state:" << endl;
		global_state.dump(o, _footprint);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
module::is_created(void) const {
	return get_footprint().is_created();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
module::is_unrolled(void) const {
	return get_footprint().is_unrolled();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't just call sequential_scope::unroll, this makes sure
	entire module is not already unrolled.  
 */
good_bool
module::unroll_module(void) {
	STACKTRACE("module::unroll_module()");
	footprint& f(get_footprint());
	if (!parent_type::__unroll_complete_type(
			null_module_params, f, f).good) {
		cerr << "Error encountered during module::unroll." << endl;
		return good_bool(false);
	}
	return good_bool(true);
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
	footprint& f(get_footprint());
	if (!parent_type::__create_complete_type(
			null_module_params, f, f).good) {
		cerr << "Error during create_unique." << endl;
		return good_bool(false);
	}
	return good_bool(true);
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
	if (!unroll_module().good)
		return good_bool(false);
	if (!is_created()) {
		STACKTRACE("not already created, creating...");
		// this replays all internal aliases recursively
		// and assigns local instance IDs
		if (!create_dependent_types().good) {
			// alraedy have error mesage
			return good_bool(false);
		}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
#if 0
		// only for debugging
		global_state.cache_process_parent_refs();
		global_state.dump(cerr, _footprint) << endl;
#endif
		allocated = true;
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
module::allocate_unique(void) {
	if (!create_unique().good)
		return good_bool(false);
	else return __allocate_unique();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Takes process definition and unroll its contents into the top-level.  
 */
good_bool
module::allocate_unique_process_type(const process_type_reference& pt) {
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
	// need to import definition's local symbols (deep copy) into
	// this temporary module's global namespace and footprint.  
	footprint& _footprint(get_footprint());
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
		return __allocate_unique();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
module::cflat_process_type(const process_type_reference& pt, ostream& o, 
		const cflat_options& cf) {
	if (!allocate_unique_process_type(pt).good) {
		cerr << "ERROR in allocating global state.  Aborting." << endl;
		return good_bool(false);
	}
#if ENABLE_STACKTRACE
	get_footprint().dump_with_collections(cerr << "module\'s footprint: ", 
		dump_flags::default_value,
		expr_dump_context::default_value) << endl;
#endif
	return good_bool(__cflat_rules(o, cf).good &&
		__cflat_aliases_no_import(o, cf).good);
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
			global_state.accept(cfp);	// print!
			// support for top-level prs!
			// const top_level_footprint_importer foo(*this);
			const footprint& _footprint(get_footprint());
			_footprint.get_prs_footprint().accept(cfp);
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
		_footprint.cflat_aliases(o, global_state, cf);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This variation assumes that the top-level footprint is already
	loaded with instances to visit.  
 */
good_bool
module::__cflat_aliases_no_import(ostream& o, const cflat_options& cf) const {
	// TODO: instance_visitor
	if (cf.connect_style) {
		STACKTRACE("cflatting aliases.");
		const footprint& _footprint(get_footprint());
		_footprint.cflat_aliases(o, global_state, cf);
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
#if 0
	// can't depend on ordering (overloaded), my bad
	return __cflat_rules(o, cf) && __cflat_aliases(o, cf);
#else
	return good_bool(__cflat_rules(o, cf).good &&
			__cflat_aliases(o, cf).good);
#endif
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
	STACKTRACE_PERSISTENT("module::collect_transient_info()");
	global_namespace->collect_transient_info(m);
	// the list itself is a statically allocated member
	parent_type::collect_transient_info_base(m);
	const footprint& _footprint(get_footprint());
	global_state.collect_transient_info_base(m, _footprint);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
module::write_object(const persistent_object_manager& m, ostream& f) const {
	STACKTRACE_PERSISTENT("module::write_object()");
	m.write_pointer(f, global_namespace);
	parent_type::write_object_base(m, f);
	const footprint& _footprint(get_footprint());
	write_value(f, allocated);
	global_state.write_object_base(m, f, _footprint);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
module::load_object(const persistent_object_manager& m, istream& f) {
	STACKTRACE_PERSISTENT("module::load_object()");
	m.read_pointer(f, global_namespace);
//	global_namespace->load_object(m);	// not necessary
	parent_type::load_object_base(m, f);
	const footprint& _footprint(get_footprint());
	read_value(f, allocated);
	global_state.load_object_base(m, f, _footprint);
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

#endif	// __HAC_OBJECT_MODULE_CC__

