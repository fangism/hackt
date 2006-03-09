/**
	\file "Object/module.cc"
	Method definitions for module class.  
	This file was renamed from "Object/art_object_module.cc".
 	$Id: module.cc,v 1.14.12.1 2006/03/09 05:50:27 fang Exp $
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
#include "main/cflat_options.h"
#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.h"
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

//=============================================================================
// class module::top_level_footprint_importer definition

/**
	Helper class for a hack that temporarily loads namespaces'
	instance collections into the module's top-level footprint.  
	TODO: redesign module to be a definition to eliminate this hackery.  
 */
class module::top_level_footprint_importer {
private:
	footprint&			fp;
public:
	/**
		Temporarily expands the footprint collection map
		by visiting all namespaces and collecting their
		top-level instance collections.  
	 */
	explicit
	top_level_footprint_importer(const module& m) :
			fp(const_cast<footprint&>(m._footprint)) {
		namespace_collection_type nsl;
		m.collect_namespaces(nsl);
		namespace_collection_type::const_iterator i(nsl.begin());
		const namespace_collection_type::const_iterator e(nsl.end());
		for ( ; i!=e; i++) {
			fp.import_hierarchical_scopespace(**i);
		}
	}

	/**
		Restores the previous state of the footprint's instance
		collection map.  
		Is necessary for the top-level footprint because
		it temporarily violates an invariant with 
		object ownership.  
		See comment where this class is used.  
	 */
	~top_level_footprint_importer() {
		fp.clear_instance_collection_map();
	}

} __ATTRIBUTE_UNUSED__;	// end class module::top_level_footprint_importer

//=============================================================================
// class module method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
module::module() :
		persistent(), sequential_scope(),
		name(""), global_namespace(NULL),
		_footprint(), allocated(false), global_state() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
module::module(const string& s) :
		persistent(), sequential_scope(),
		name(s), global_namespace(new name_space("")),
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
		return sequential_scope::dump(o,
			expr_dump_context::default_value);
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
	return o;
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
#if 1
		if (!sequential_scope::unroll(c).good) {
			cerr << "Error encountered during module::unroll."
				<< endl;
			return good_bool(false);
		}
#else
		// three-phase unrolling
		if (!sequential_scope::unroll_meta_evaluate(c).good) {
			cerr << "Error during unroll_meta_evaluate." << endl;
			return good_bool(false);
		}
		if (!sequential_scope::unroll_meta_instantiate(c).good) {
			cerr << "Error during unroll_meta_instantiate." << endl;
			return good_bool(false);
		}
		// this would be a good point to finalize sparse and dense
		// array collections into index maps
		// Top-level finalization would bind indexed instances
		// to fixed offsets.  
		if (!sequential_scope::unroll_meta_connect(c).good) {
			cerr << "Error during unroll_meta_connect." << endl;
			return good_bool(false);
		}
#endif
		_footprint.mark_unrolled();
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates placeholder footprints depedent types bottom up.
 */
good_bool
module::create_dependent_types(void) {
	typedef list<never_ptr<physical_instance_collection> >
			collection_list_type;
	typedef collection_list_type::const_iterator
			const_collection_iterator;
	STACKTRACE_VERBOSE;
	collection_list_type collections;
	collect(collections);
	const_collection_iterator i(collections.begin());
	const const_collection_iterator e(collections.end());
	for ( ; i!=e; i++) {
		if (!(*i)->create_dependent_types().good) {
			// error message
			cerr << "Error during create_unique." << endl;
			return good_bool(false);
		}
	}
	return good_bool(true);
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
		if (!create_dependent_types().good) {
			// alraedy have error mesage
			return good_bool(false);
		}
#if SEPARATE_ALLOCATE_SUBPASS
		FINISH_ME(Fang);
#else
		const unroll_context c;	// empty top-level context
		if (!sequential_scope::create_unique(c, _footprint).good) {
			cerr << "Error during create_unique." << endl;
			return good_bool(false);
		}
#endif
		// this is needed for evaluating scope_aliases, 
		// but cannot be maintained persistently because
		// of memory pointer hack (see implementation of 
		// footprint::import_hierarchical_scopespace.
		// Plan B: destroy after evaluating aliases!
		// we call clear_instance_collection_map after we're done.
		// This is now taken care of by the helper class:
		const top_level_footprint_importer foo(*this);
		_footprint.evaluate_scope_aliases();
		_footprint.mark_created();
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Allocates unique global entries for instance objects.  
 */
good_bool
module::allocate_unique(void) {
	if (!create_unique().good)
		return good_bool(false);
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
/**
	The actual cflat procedure.  
	Always prints all rules before aliases.  
	TODO: if necessary, make the order an option
	TODO: implement instance visitor pattern, including the footprint.  
 */
good_bool
module::__cflat(ostream& o, const cflat_options& cf) const {
	// print the production rules first, using canonical names
#if 0
	if (!global_state.cflat_prs(o, _footprint, cf).good) {
		cerr << "Unexpected error during cflat." << endl;
		return good_bool(false);
	}
#else
{
	// our priting visitor functor
	PRS::cflat_prs_printer cfp(o, cf);
	const cflat_context::module_setter tmp(cfp, *this);
	if (cf.dsim_prs)	o << "dsim {" << endl;
	global_state.accept(cfp);	// print!
	if (cf.dsim_prs)	o << "}" << endl;
}
#endif
	// print the name aliases in the manner requested in cflat_options
	// TODO: instance_visitor
	if (cf.connect_style) {
		STACKTRACE("cflatting aliases.");
		// top-level footprint is actually empty
		// so we need to load it first...
		// we promise to clean it up after we're done.
		// This is now handled by the helper class:
		const top_level_footprint_importer foo(*this);
		footprint& _fp(const_cast<footprint&>(_footprint));
		_fp.cflat_aliases(o, global_state, cf);
	}
	return good_bool(true);
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

