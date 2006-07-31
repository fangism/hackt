/**
	\file "Object/module.cc"
	Method definitions for module class.  
	This file was renamed from "Object/art_object_module.cc".
 	$Id: module.cc,v 1.21 2006/07/31 22:22:26 fang Exp $
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
// class module::top_level_footprint_importer method definitions

/**
	Temporarily expands the footprint collection map
	by visiting all namespaces and collecting their
	top-level instance collections.  
 */
module::top_level_footprint_importer::top_level_footprint_importer(
		const module& m) : fp(const_cast<footprint&>(m._footprint)) {
	namespace_collection_type nsl;
	m.collect_namespaces(nsl);
	namespace_collection_type::const_iterator i(nsl.begin());
	const namespace_collection_type::const_iterator e(nsl.end());
	for ( ; i!=e; i++) {
		fp.import_hierarchical_scopespace(**i);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Restores the previous state of the footprint's instance
	collection map.  
	Is necessary for the top-level footprint because
	it temporarily violates an invariant with 
	object ownership.  
	See comment where this class is used.  
 */
module::top_level_footprint_importer::~top_level_footprint_importer() {
	fp.clear_instance_collection_map();
}

//=============================================================================
// class namespace_footprint_importer
/**
	Temporarily expands the footprint collection map
	by visiting only the global namespaces and collecting its
	instance collections.  
	This makes a temporary shallow copy of the global_namespace's
	instance collection pointers.  
 */
module::namespace_footprint_importer::namespace_footprint_importer(
		const module& m) : fp(const_cast<footprint&>(m._footprint)) {
	// shallow copy-ing pointers
	fp.import_scopespace_shallow(*m.global_namespace);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Restores the previous state of the footprint's instance
	collection map.  
	Is necessary for the top-level footprint because
	it temporarily violates an invariant with 
	object ownership.  
 */
module::namespace_footprint_importer::~namespace_footprint_importer() {
	fp.clear_instance_collection_map();
}

//=============================================================================
// class module method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
module::module() :
		persistent(), sequential_scope(),
		name(""), global_namespace(NULL),
		top_prs(), 
		_footprint(), allocated(false), global_state() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
module::module(const string& s) :
		persistent(), sequential_scope(),
		name(s), global_namespace(new name_space("")),
		top_prs(), 
		_footprint(), allocated(false), global_state() {
	NEVER_NULL(global_namespace);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
module::~module() {
	STACKTRACE_DTOR("~module()");
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
	o << "In module created from: " << name;
	if (is_unrolled())
		o << " (unrolled)";
	if (is_created())
		o << " (created)";
	o << endl;

	global_namespace->dump(o) << endl;
	if (!is_unrolled()) {
		o << "Sequential instance management (to unroll): " << endl;
		sequential_scope::dump(o,
			expr_dump_context::default_value);
		if (!top_prs.empty()) {
			o << auto_indent << "top-level prs:" << endl;
			INDENT_SECTION(o);
			const PRS::rule_dump_context rdc(*global_namespace);
			top_prs.dump(o, rdc) << endl;
		}
		return o;
	}
	if (is_created()) {
		o << "Created state:" << endl;
		_footprint.dump(o) << endl;
#if 0 && ENABLE_STACKTRACE
		_footprint.get_scope_alias_tracker().dump(o << "BUH: ");
#endif
	}
	if (is_allocated()) {
#if 0
		// only for debugging
		global_state.cache_process_parent_refs();
#endif
		o << "Globally allocated state:" << endl;
		global_state.dump(o, _footprint);
	}
	dump_top_level_unrolled_prs(o);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
module::dump_top_level_unrolled_prs(ostream& o) const {
	// footprint::dump doesn't contain unrolled prs
	const PRS::footprint& Pfp(_footprint.get_prs_footprint());
	Pfp.dump(o, _footprint);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't just call sequential_scope::unroll, this makes sure
	entire module is not already unrolled.  
 */
good_bool
module::unroll_module(void) {
	STACKTRACE("module::unroll_module()");
	if (!is_unrolled()) {
		STACKTRACE("not already unrolled, unrolling...");
		// start with blank context
		unroll_context c;
		if (!sequential_scope::unroll(c).good) {
			cerr << "Error encountered during module::unroll."
				<< endl;
			return good_bool(false);
		}
		
		_footprint.mark_unrolled();
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates placeholder footprints depedent types bottom up.
	This replays internal aliases from the leaf-most types up.  
	This routine is modeled after footprint::create_dependent_types.  
	Need to figure out why we can't just use that...
 */
good_bool
module::create_dependent_types(void) {
#if 0
	// enabling this requires changes in the end of ::create_unique
	// this doesn't quite work the way I expected it...
	const top_level_footprint_importer foo(*this);
	return _footprint.create_dependent_types();
#else
	typedef list<never_ptr<physical_instance_collection> >
			collection_list_type;
	typedef collection_list_type::const_iterator
			const_collection_iterator;
	typedef collection_list_type::iterator
			collection_iterator;
	STACKTRACE_VERBOSE;
	collection_list_type collections;
	collect(collections);
{
	const const_collection_iterator
		b(collections.begin()), e(collections.end());
	const_collection_iterator i(b);
	for ( ; i!=e; i++) {
		if (!(*i)->create_dependent_types().good) {
			// error message
			cerr << "Error during create_unique." << endl;
			return good_bool(false);
		}
	}
}
{
	// after replaying internal aliases, we can now assign instance_id's
	const collection_iterator
		b(collections.begin()), e(collections.end());
	collection_iterator i(b);
	for ( ; i!=e; i++) {
		if (!(*i)->allocate_local_instance_ids(_footprint).good) {
			// error message
			cerr << "Error during create_unique." << endl;
			return good_bool(false);
		}
	}
}
{
	const top_level_footprint_importer foo(*this);
	_footprint.evaluate_scope_aliases();
	_footprint.mark_created();
}
	return good_bool(true);
#endif
}

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
	{
		// using namespace_footprint_importer is only effective
		// with the global namespace and not deeper namespaces
		const namespace_footprint_importer foo(*this);
#if ENABLE_STACKTRACE
		_footprint.dump_with_collections(
			cerr << "module\'s footprint: ", 
			dump_flags::default_value,
			expr_dump_context::default_value) << endl;
#endif
		// will this automatically lookup global meta parameters?
		const unroll_context c(&_footprint);
		if (!top_prs.unroll(c, _footprint.get_pool<bool_tag>(),
				_footprint.get_prs_footprint()).good) {
			// already have error message
			return good_bool(false);
		}
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
	_footprint.import_scopespace(*pd);
#if ENABLE_STACKTRACE
	pd->dump(cerr << "process definition: ") << endl;
	_footprint.dump_with_collections(cerr << "module\'s footprint: ", 
		dump_flags::default_value,
		expr_dump_context::default_value) << endl;
#endif
	if (!pd->__create_complete_type(tp, _footprint).good) {
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
	_footprint.dump_with_collections(cerr << "module\'s footprint: ", 
		dump_flags::default_value,
		expr_dump_context::default_value) << endl;
#endif
	return __cflat_rules(o, cf) && __cflat_aliases_no_import(o, cf);
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
		const top_level_footprint_importer foo(*this);
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
	return __cflat_rules(o, cf) &&
		__cflat_aliases(o, cf);
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
	sequential_scope::collect_transient_info_base(m);
	top_prs.collect_transient_info_base(m);
	_footprint.collect_transient_info_base(m);
	global_state.collect_transient_info_base(m, _footprint);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
module::write_object(const persistent_object_manager& m, ostream& f) const {
	STACKTRACE_PERSISTENT("module::write_object()");
	write_string(f, name);
	m.write_pointer(f, global_namespace);
	sequential_scope::write_object_base(m, f);
	top_prs.write_object_base(m, f);
	_footprint.write_object_base(m, f);
	write_value(f, allocated);
	global_state.write_object_base(m, f, _footprint);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
module::load_object(const persistent_object_manager& m, istream& f) {
	STACKTRACE_PERSISTENT("module::load_object()");
	read_string(f, name);
	m.read_pointer(f, global_namespace);
//	global_namespace->load_object(m);	// not necessary
	sequential_scope::load_object_base(m, f);
	top_prs.load_object_base(m, f);
	_footprint.load_object_base(m, f);
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

