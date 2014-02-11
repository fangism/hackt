/**
	\file "Object/inst/instance_collection_pool_bundle_footprint.tcc"
	This contains select methods to export to Object/def/footprint.cc
	$Id: instance_collection_pool_bundle_footprint.tcc,v 1.9 2010/04/07 00:12:41 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_COLLECTION_POOL_BUNDLE_FOOTPRINT_TCC__
#define	__HAC_OBJECT_INST_INSTANCE_COLLECTION_POOL_BUNDLE_FOOTPRINT_TCC__

#include "Object/inst/instance_collection_pool_bundle.hh"
#include "Object/inst/instance_placeholder.hh"
#include "Object/inst/datatype_instance_placeholder.hh"
#include "Object/inst/instance_array.hh"
#include "Object/inst/instance_scalar.hh"
#include "Object/inst/port_formal_array.hh"
#include "Object/inst/port_actual_collection.hh"
#include "Object/devel_switches.hh"
#include "util/stacktrace.hh"

namespace HAC {
namespace entity {

//=============================================================================
// class instance_collection_pool_wrapper helper structs

/**
	Functor stops calling on first error, for now.  
 */
template <class T>
struct instance_collection_pool_wrapper<T>::dependent_creator {
	const footprint&		fp;
	// result type
	good_bool			g;

	explicit
	dependent_creator(const footprint& f) : fp(f), g(true) { }

	void
	operator () (T& t) {
		if (g.good) {
			g = t.create_dependent_types(fp);
		}
	}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor stops calling on first error, for now.  
 */
template <class T>
struct instance_collection_pool_wrapper<T>::index_allocator {
	footprint&			fp;
	// result type
	good_bool			g;

	explicit
	index_allocator(footprint& f) : fp(f), g(true) { }

	void
	operator () (T& t) {
		if (g.good) {
			g = t.allocate_local_instance_ids(fp);
		}
	}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor for collecting scope_aliases.
	Unconditionally collects aliases.  
 */
template <class T>
struct instance_collection_pool_wrapper<T>::scope_alias_collector {
	port_alias_tracker&		pt;

	explicit
	scope_alias_collector(port_alias_tracker& p) : pt(p) { }

	void
	operator () (const T& t) {
		t.collect_port_aliases(pt);
	}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
struct instance_collection_pool_wrapper<T>::substructure_finalizer {
	const unroll_context&	context;

	explicit
	substructure_finalizer(const unroll_context& c) : context(c) { }

	void
	operator () (T& t) {
		t.finalize_substructure_aliases(context);
		// throws exception on error
	}
};

//=============================================================================
// selected class instance_collection_pool_wrapper method definitions

template <class T>
good_bool
instance_collection_pool_wrapper<T>::create_dependent_types(
		const footprint& f) {
//	STACKTRACE_VERBOSE;
	const iterator b(this->pool.begin()), e(this->pool.end());
	return for_each(b, e, dependent_creator(f)).g;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
good_bool
instance_collection_pool_wrapper<T>::allocate_local_instance_ids(footprint& f) {
//	STACKTRACE_VERBOSE;
	const iterator b(this->pool.begin()), e(this->pool.end());
	return for_each(b, e, index_allocator(f)).g;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
void
instance_collection_pool_wrapper<T>::collect_scope_aliases(
		port_alias_tracker& pt) const {
//	STACKTRACE_VERBOSE;
	const const_iterator b(this->pool.begin()), e(this->pool.end());
	for_each(b, e, scope_alias_collector(pt));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
good_bool
instance_collection_pool_wrapper<T>::finalize_substructure_aliases(
		const unroll_context& c) {
	try {
		const iterator b(this->pool.begin()), e(this->pool.end());
		for_each(b, e, substructure_finalizer(c));
	} catch (...) {
		return good_bool(false);
	}
	return good_bool(true);
}

//=============================================================================
// selected struct instance_collection_pool_bundle method definitions

template <class Tag>
good_bool
instance_collection_pool_bundle<Tag>::create_dependent_types(
		const footprint& f) {
//	STACKTRACE_VERBOSE;
	return good_bool(
		instance_collection_pool_wrapper<instance_array<Tag, 0> >
			::create_dependent_types(f).good &&
		instance_collection_pool_wrapper<instance_array<Tag, 1> >
			::create_dependent_types(f).good &&
		instance_collection_pool_wrapper<instance_array<Tag, 2> >
			::create_dependent_types(f).good &&
		instance_collection_pool_wrapper<instance_array<Tag, 3> >
			::create_dependent_types(f).good &&
		instance_collection_pool_wrapper<instance_array<Tag, 4> >
			::create_dependent_types(f).good &&
		instance_collection_pool_wrapper<port_formal_array<Tag> >
			::create_dependent_types(f).good &&
		instance_collection_pool_wrapper<port_actual_collection<Tag> >
			::create_dependent_types(f).good
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
good_bool
instance_collection_pool_bundle<Tag>::allocate_local_instance_ids(
		footprint& f) {
//	STACKTRACE_VERBOSE;
	return good_bool(
		instance_collection_pool_wrapper<instance_array<Tag, 0> >
			::allocate_local_instance_ids(f).good &&
		instance_collection_pool_wrapper<instance_array<Tag, 1> >
			::allocate_local_instance_ids(f).good &&
		instance_collection_pool_wrapper<instance_array<Tag, 2> >
			::allocate_local_instance_ids(f).good &&
		instance_collection_pool_wrapper<instance_array<Tag, 3> >
			::allocate_local_instance_ids(f).good &&
		instance_collection_pool_wrapper<instance_array<Tag, 4> >
			::allocate_local_instance_ids(f).good &&
		instance_collection_pool_wrapper<port_formal_array<Tag> >
			::allocate_local_instance_ids(f).good &&
		instance_collection_pool_wrapper<port_actual_collection<Tag> >
			::allocate_local_instance_ids(f).good
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visits all collections and evaluate aliases.  
	This covers local private aliases and ports.  
	Ports can be extracted later with filtering.  
 */
template <class Tag>
void
instance_collection_pool_bundle<Tag>::collect_scope_aliases(
		port_alias_tracker& spt) const {
//	STACKTRACE_VERBOSE;
	instance_collection_pool_wrapper<instance_array<Tag, 0> >
		::collect_scope_aliases(spt);
	instance_collection_pool_wrapper<instance_array<Tag, 1> >
		::collect_scope_aliases(spt);
	instance_collection_pool_wrapper<instance_array<Tag, 2> >
		::collect_scope_aliases(spt);
	instance_collection_pool_wrapper<instance_array<Tag, 3> >
		::collect_scope_aliases(spt);
	instance_collection_pool_wrapper<instance_array<Tag, 4> >
		::collect_scope_aliases(spt);
	instance_collection_pool_wrapper<port_formal_array<Tag> >
		::collect_scope_aliases(spt);
#if !RECURSE_COLLECT_ALIASES
	// redundant, as they will be covered recursively by the above
	// these only contain subinstances, public ports
	instance_collection_pool_wrapper<port_actual_collection<Tag> >
		::collect_scope_aliases(spt);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
good_bool
instance_collection_pool_bundle<Tag>::finalize_substructure_aliases(
		const unroll_context& c) {
	return good_bool(
	instance_collection_pool_wrapper<instance_array<Tag, 0> >
		::finalize_substructure_aliases(c).good &&
	instance_collection_pool_wrapper<instance_array<Tag, 1> >
		::finalize_substructure_aliases(c).good &&
	instance_collection_pool_wrapper<instance_array<Tag, 2> >
		::finalize_substructure_aliases(c).good &&
	instance_collection_pool_wrapper<instance_array<Tag, 3> >
		::finalize_substructure_aliases(c).good &&
	instance_collection_pool_wrapper<instance_array<Tag, 4> >
		::finalize_substructure_aliases(c).good &&
	instance_collection_pool_wrapper<port_formal_array<Tag> >
		::finalize_substructure_aliases(c).good
//	instance_collection_pool_wrapper<port_actual_collection<Tag> >
//		::finalize_substructure_aliases(c);
	);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_COLLECTION_POOL_BUNDLE_FOOTPRINT_TCC__

