/**
	\file "Object/inst/instance_collection_pool_bundle.h"
	$Id: instance_collection_pool_bundle.tcc,v 1.1.2.1 2006/10/31 00:28:23 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_COLLECTION_POOL_BUNDLE_TCC__
#define	__HAC_OBJECT_INST_INSTANCE_COLLECTION_POOL_BUNDLE_TCC__

#include <iostream>
#include <algorithm>
#include "Object/inst/instance_collection_pool_bundle.h"
#include "Object/inst/collection_pool.tcc"
#include "Object/inst/instance_scalar.h"
#include "Object/inst/instance_array.h"
#include "Object/inst/port_formal_array.h"
#include "Object/inst/port_actual_collection.h"
#include "Object/inst/instance_alias_info.h"
#include "util/persistent_object_manager.tcc"
#include "util/persistent_functor.tcc"
#include "util/IO_utils.tcc"

namespace HAC {
namespace entity {
using std::for_each;
using util::write_value;
using util::read_value;

//=============================================================================
// class collection_pool_wrapper method definitions

template <class T>
void
collection_pool_wrapper<T>::collect_transient_info_base(
		persistent_object_manager& m) const {
	for_each(this->pool.begin(), this->pool.end(), 
		util::persistent_collector_ref(m));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
void
collection_pool_wrapper<T>::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	const size_t s = this->pool.size();
	write_value(o, s);
	for_each(this->pool.begin(), this->pool.end(), 
		util::persistent_writer_ref(m, o));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
void
collection_pool_wrapper<T>::load_object_base(
		const persistent_object_manager& m, istream& i) {
	size_t s;
	read_value(i, s);
	this->pool.allocate(s);		// will be contiguous! (first chunk)
	INVARIANT(this->pool.size() == s);
	for_each(this->pool.begin(), this->pool.end(), 
		util::persistent_loader_ref(m, i));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
void
collection_pool_wrapper<T>::write_connections(
		const persistent_object_manager& m, ostream& o) const {
	const size_t s = this->pool.size();
	write_value(o, s);
	for_each(this->pool.begin(), this->pool.end(), 
		util::persistent_writer<T>(&T::write_connections, m, o));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
void
collection_pool_wrapper<T>::load_connections(
		const persistent_object_manager& m, istream& i) {
	size_t s;
	read_value(i, s);
	this->pool.allocate(s);		// will be contiguous! (first chunk)
	INVARIANT(this->pool.size() == s);
	for_each(this->pool.begin(), this->pool.end(), 
		util::persistent_loader<T>(&T::load_connections, m, i));
}

//=============================================================================
// class instance_collection_pool_bundle method definitions

template <class Tag>
instance_collection_pool_bundle<Tag>::instance_collection_pool_bundle() :
		collection_pool_wrapper<instance_array<Tag, 0> >(), 
		collection_pool_wrapper<instance_array<Tag, 1> >(), 
		collection_pool_wrapper<instance_array<Tag, 2> >(), 
		collection_pool_wrapper<instance_array<Tag, 3> >(), 
		collection_pool_wrapper<instance_array<Tag, 4> >(), 
		collection_pool_wrapper<port_formal_array<Tag> >(), 
		collection_pool_wrapper<port_actual_collection<Tag> >() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
instance_collection_pool_bundle<Tag>::~instance_collection_pool_bundle() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
instance_collection_pool_bundle<Tag>::collect_transient_info_base(
		persistent_object_manager& m) const {
	collection_pool_wrapper<instance_array<Tag, 0> >
		::collect_transient_info_base(m);
	collection_pool_wrapper<instance_array<Tag, 1> >
		::collect_transient_info_base(m);
	collection_pool_wrapper<instance_array<Tag, 2> >
		::collect_transient_info_base(m);
	collection_pool_wrapper<instance_array<Tag, 3> >
		::collect_transient_info_base(m);
	collection_pool_wrapper<instance_array<Tag, 4> >
		::collect_transient_info_base(m);
	collection_pool_wrapper<port_formal_array<Tag> >
		::collect_transient_info_base(m);
	collection_pool_wrapper<port_actual_collection<Tag> >
		::collect_transient_info_base(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: want to re-establish alias connections AFTER all
	collections in a footprint frame (of the same meta-type) 
	have been reconstructed.  
	caveat: port_actual_collections will depend on their referenced
		formal_collection being sufficiently constructed
		for proper resizing.  How do we keep track of inter-footprint
		on-demand once-only reconstruction?
		Might need footprints to be heap-allocated...
		or at least use some auxiliary locking.  
		(Depends on acyclic footprint dependence, naturally)
 */
template <class Tag>
void
instance_collection_pool_bundle<Tag>::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	collection_pool_wrapper<instance_array<Tag, 0> >
		::write_object_base(m, o);
	collection_pool_wrapper<instance_array<Tag, 1> >
		::write_object_base(m, o);
	collection_pool_wrapper<instance_array<Tag, 2> >
		::write_object_base(m, o);
	collection_pool_wrapper<instance_array<Tag, 3> >
		::write_object_base(m, o);
	collection_pool_wrapper<instance_array<Tag, 4> >
		::write_object_base(m, o);
	collection_pool_wrapper<port_formal_array<Tag> >
		::write_object_base(m, o);
	collection_pool_wrapper<port_actual_collection<Tag> >
		::write_object_base(m, o);

	collection_pool_wrapper<instance_array<Tag, 0> >
		::write_connections(m, o);
	collection_pool_wrapper<instance_array<Tag, 1> >
		::write_connections(m, o);
	collection_pool_wrapper<instance_array<Tag, 2> >
		::write_connections(m, o);
	collection_pool_wrapper<instance_array<Tag, 3> >
		::write_connections(m, o);
	collection_pool_wrapper<instance_array<Tag, 4> >
		::write_connections(m, o);
	collection_pool_wrapper<port_formal_array<Tag> >
		::write_connections(m, o);
	collection_pool_wrapper<port_actual_collection<Tag> >
		::write_connections(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
instance_collection_pool_bundle<Tag>::load_object_base(
		const persistent_object_manager& m, istream& i) {
	collection_pool_wrapper<instance_array<Tag, 0> >
		::load_object_base(m, i);
	collection_pool_wrapper<instance_array<Tag, 1> >
		::load_object_base(m, i);
	collection_pool_wrapper<instance_array<Tag, 2> >
		::load_object_base(m, i);
	collection_pool_wrapper<instance_array<Tag, 3> >
		::load_object_base(m, i);
	collection_pool_wrapper<instance_array<Tag, 4> >
		::load_object_base(m, i);
	collection_pool_wrapper<port_formal_array<Tag> >
		::load_object_base(m, i);
	collection_pool_wrapper<port_actual_collection<Tag> >
		::load_object_base(m, i);

	collection_pool_wrapper<instance_array<Tag, 0> >
		::load_connections(m, i);
	collection_pool_wrapper<instance_array<Tag, 1> >
		::load_connections(m, i);
	collection_pool_wrapper<instance_array<Tag, 2> >
		::load_connections(m, i);
	collection_pool_wrapper<instance_array<Tag, 3> >
		::load_connections(m, i);
	collection_pool_wrapper<instance_array<Tag, 4> >
		::load_connections(m, i);
	collection_pool_wrapper<port_formal_array<Tag> >
		::load_connections(m, i);
	collection_pool_wrapper<port_actual_collection<Tag> >
		::load_connections(m, i);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_COLLECTION_POOL_BUNDLE_TCC__

