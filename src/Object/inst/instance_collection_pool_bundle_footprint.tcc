/**
	\file "Object/inst/instance_collection_pool_bundle_footprint.tcc"
	This contains select methods to export to Object/def/footprint.cc
	$Id: instance_collection_pool_bundle_footprint.tcc,v 1.3 2006/11/27 08:29:10 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_COLLECTION_POOL_BUNDLE_FOOTPRINT_TCC__
#define	__HAC_OBJECT_INST_INSTANCE_COLLECTION_POOL_BUNDLE_FOOTPRINT_TCC__

#include "Object/inst/instance_collection_pool_bundle.h"
#include "Object/inst/instance_placeholder.h"
#include "Object/inst/datatype_instance_placeholder.h"
#include "Object/inst/instance_array.h"
#include "Object/inst/instance_scalar.h"
#include "Object/inst/port_formal_array.h"
#include "Object/inst/port_actual_collection.h"
#include "Object/port_context.h"
#include "util/stacktrace.h"

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
#if !COPY_IF_PORT_ALIASES
/**
	Functor for collecting port_aliases, conditional upon collection
	being a port-formal, of course.  
 */
template <class T>
struct instance_collection_pool_wrapper<T>::port_alias_collector {
	port_alias_tracker&		pt;

	explicit
	port_alias_collector(port_alias_tracker& p) : pt(p) { }

	void
	operator () (const T& t) {
		if (t.get_placeholder()->is_port_formal()) {
			t.collect_port_aliases(pt);
		}
	}
};
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor for assigning footprint frame IDs.  
 */
template <class T>
struct instance_collection_pool_wrapper<T>::footprint_frame_assigner {
	footprint_frame&		ff;
	const port_member_context&	pmc;

	explicit
	footprint_frame_assigner(footprint_frame& f, 
		const port_member_context& p) : ff(f), pmc(p) { }

	void
	operator () (const T& t) {
		// recall: port-formal is 1-based indexed
		const size_t pfp = t.get_placeholder()->is_port_formal();
		if (pfp) {
			t.assign_footprint_frame(ff, pmc.member_array[pfp -1]);
		}
	}
};

//=============================================================================
// selected class instance_collection_pool_wrapper method definitions

template <class T>
good_bool
instance_collection_pool_wrapper<T>::create_dependent_types(
		const footprint& f) {
	STACKTRACE_VERBOSE;
	const iterator b(this->pool.begin()), e(this->pool.end());
	return for_each(b, e, dependent_creator(f)).g;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
good_bool
instance_collection_pool_wrapper<T>::allocate_local_instance_ids(footprint& f) {
	STACKTRACE_VERBOSE;
	const iterator b(this->pool.begin()), e(this->pool.end());
	return for_each(b, e, index_allocator(f)).g;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
void
instance_collection_pool_wrapper<T>::collect_scope_aliases(
		port_alias_tracker& pt) const {
	STACKTRACE_VERBOSE;
	const const_iterator b(this->pool.begin()), e(this->pool.end());
	for_each(b, e, scope_alias_collector(pt));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !COPY_IF_PORT_ALIASES
template <class T>
void
instance_collection_pool_wrapper<T>::collect_port_aliases(
		port_alias_tracker& pt) const {
	STACKTRACE_VERBOSE;
	const const_iterator b(this->pool.begin()), e(this->pool.end());
	for_each(b, e, port_alias_collector(pt));
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
void
instance_collection_pool_wrapper<T>::assign_footprint_frame(
		footprint_frame& ff, const port_member_context& pmc) const {
	STACKTRACE_VERBOSE;
	const const_iterator b(this->pool.begin()), e(this->pool.end());
	for_each(b, e, footprint_frame_assigner(ff, pmc));
}

//=============================================================================
// selected class instance_collection_pool_bundle method definitions

template <class Tag>
good_bool
instance_collection_pool_bundle<Tag>::create_dependent_types(
		const footprint& f) {
	STACKTRACE_VERBOSE;
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
	STACKTRACE_VERBOSE;
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
#if COPY_IF_PORT_ALIASES
/**
	Visits all collections and evaluate aliases.  
	This covers local private aliases and ports.  
	Ports can be extracted later with filtering.  
 */
template <class Tag>
void
instance_collection_pool_bundle<Tag>::collect_scope_aliases(
		port_alias_tracker& spt) const {
	STACKTRACE_VERBOSE;
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
	instance_collection_pool_wrapper<port_actual_collection<Tag> >
		::collect_scope_aliases(spt);
#endif
}

#else	// COPY_IF_PORT_ALIASES
/**
	Only visit port-formals for port aliases.  
	\param spt scope_alias set (entire footprint)
	\param ppt port_alias set (footprint's ports only)
 */
template <class Tag>
void
instance_collection_pool_bundle<Tag>::collect_scope_and_port_aliases(
		port_alias_tracker& spt, port_alias_tracker& ppt) const {
	STACKTRACE_VERBOSE;
	instance_collection_pool_wrapper<instance_array<Tag, 0> >
		::collect_scope_aliases(spt);
	instance_collection_pool_wrapper<instance_array<Tag, 0> >
		::collect_port_aliases(ppt);

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
	instance_collection_pool_wrapper<port_formal_array<Tag> >
		::collect_port_aliases(ppt);

#if !RECURSE_COLLECT_ALIASES
	// redundant, as they will be covered recursively by the above
	instance_collection_pool_wrapper<port_actual_collection<Tag> >
		::collect_scope_aliases(spt);
#if 0
	// will not work: wrong port formal indices during lookups
	instance_collection_pool_wrapper<port_actual_collection<Tag> >
		::collect_port_aliases(ppt);
#endif
#endif
}
#endif	// COPY_IF_PORT_ALIASES

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only visit port-formals.  
 */
template <class Tag>
void
instance_collection_pool_bundle<Tag>::assign_footprint_frame(
		footprint_frame& ff, const port_member_context& pmc) const {
	STACKTRACE_VERBOSE;
	instance_collection_pool_wrapper<instance_array<Tag, 0> >
		::assign_footprint_frame(ff, pmc);
#if 0
	instance_collection_pool_wrapper<instance_array<Tag, 1> >
		::assign_footprint_frame(ff, pmc);
	instance_collection_pool_wrapper<instance_array<Tag, 2> >
		::assign_footprint_frame(ff, pmc);
	instance_collection_pool_wrapper<instance_array<Tag, 3> >
		::assign_footprint_frame(ff, pmc);
	instance_collection_pool_wrapper<instance_array<Tag, 4> >
		::assign_footprint_frame(ff, pmc);
#endif
	instance_collection_pool_wrapper<port_formal_array<Tag> >
		::assign_footprint_frame(ff, pmc);
#if 0
	instance_collection_pool_wrapper<port_actual_collection<Tag> >
		::assign_footprint_frame(ff, pmc);
#endif
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_COLLECTION_POOL_BUNDLE_FOOTPRINT_TCC__

