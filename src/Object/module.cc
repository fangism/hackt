/**
	\file "Object/module.cc"
	Method definitions for module class.  
	This file was renamed from "Object/art_object_module.cc".
 	$Id: module.cc,v 1.3.4.1 2005/08/16 03:47:13 fang Exp $
 */

#ifndef	__OBJECT_MODULE_CC__
#define	__OBJECT_MODULE_CC__

// code debugging switches
#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_DESTRUCTORS		0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS		0 && ENABLE_STACKTRACE

#include <iostream>
#include "Object/module.h"
#include "Object/common/namespace.h"
#include "Object/unroll/unroll_context.h"
#include "Object/persistent_type_hash.h"
#if !USE_MODULE_FOOTPRINT
#include "Object/state_manager.h"
#endif
#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.h"

namespace util {
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::module, MODULE_TYPE_KEY, 0)
}	// end namespace util

namespace ART {
namespace entity {
#include "util/using_ostream.h"
using std::istream;
using util::write_value;
using util::read_value;
using util::write_string;
using util::read_string;
USING_STACKTRACE
using util::persistent_traits;

//=============================================================================
// class module method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
module::module() :
		persistent(), sequential_scope(),
		name(""), global_namespace(NULL),
#if USE_MODULE_FOOTPRINT
		_footprint()
#else
		unrolled(false), created(false)
#endif
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
module::module(const string& s) :
		persistent(), sequential_scope(),
		name(s), global_namespace(new name_space("")),
#if USE_MODULE_FOOTPRINT
		_footprint()
#else
		unrolled(false), created(false)
#endif
		{
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
		return sequential_scope::dump(o);
	}
	if (is_created()) {
		o << "Created state:" << endl;
#if USE_MODULE_FOOTPRINT
		_footprint.dump(o) << endl;
#else
		state_manager::dump_state(o) << endl;
#endif
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
#if USE_MODULE_FOOTPRINT
		_footprint.mark_unrolled();
#else
		unrolled = true;
#endif
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
	STACKTRACE("module::create_unique()");
	if (!unroll_module().good)
		return good_bool(false);
	if (!is_created()) {
		STACKTRACE("not already created, creating...");
		const unroll_context c;	// empty context
		if (!sequential_scope::create_unique(c).good) {
			cerr << "Error during create_unique." << endl;
			return good_bool(false);
		}
#if USE_MODULE_FOOTPRINT
		_footprint.mark_created();
#else
		created = true;
#endif
	}
	return good_bool(true);
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
#if USE_MODULE_FOOTPRINT
	_footprint.collect_transient_info_base(m);
#else
	state_manager::collect_state(m);
#endif
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
module::write_object(const persistent_object_manager& m, ostream& f) const {
	STACKTRACE_PERSISTENT("module::write_object()");
	write_string(f, name);
	m.write_pointer(f, global_namespace);
#if !USE_MODULE_FOOTPRINT
	write_value(f, unrolled);
	write_value(f, created);
#endif
	sequential_scope::write_object_base(m, f);
#if USE_MODULE_FOOTPRINT
	_footprint.write_object_base(m, f);
#else
	state_manager::write_state(m, f);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
module::load_object(const persistent_object_manager& m, istream& f) {
	STACKTRACE_PERSISTENT("module::load_object()");
	read_string(f, name);
	m.read_pointer(f, global_namespace);
#if !USE_MODULE_FOOTPRINT
	read_value(f, unrolled);
	read_value(f, created);
#endif
//	global_namespace->load_object(m);	// not necessary
	sequential_scope::load_object_base(m, f);
#if USE_MODULE_FOOTPRINT
	_footprint.load_object_base(m, f);
#else
	state_manager::load_state(m, f);
#endif
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

// clean up macros used in this module
#undef	ENABLE_STACKTRACE
#undef	STACKTRACE_DESTRUCTORS
#undef	STACKTRACE_DTOR
#undef	STACKTRACE_PERSISTENTS
#undef	STACKTRACE_PERSISTENT

#endif	// __OBJECT_MODULE_CC__

