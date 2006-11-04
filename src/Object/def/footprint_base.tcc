/**
	\file "Object/def/footprint_base.tcc"
	Implementation of footprint class. 
	$Id: footprint_base.tcc,v 1.1.2.2 2006/11/04 09:23:10 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEF_FOOTPRINT_BASE_TCC__
#define	__HAC_OBJECT_DEF_FOOTPRINT_BASE_TCC__

#include <iostream>
#include <algorithm>
#include <iterator>
#include "Object/def/footprint.h"
#include "Object/traits/classification_tags.h"
#include "Object/inst/instance_collection_pool_bundle.h"
#include "Object/inst/value_collection_pool_bundle.h"

#include "util/stacktrace.h"
#include "util/persistent_object_manager.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"

//=============================================================================
// class footprint_base method definitions

/**
	Default constructor.  
	Sets the instance pool chunk size, but does not pre-allocate.  
 */
template <class Tag>
footprint_base<Tag>::footprint_base() :
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
	collection_pool_bundle(new collection_pool_bundle_type), 
#endif
	_instance_pool(new instance_pool_type(
		class_traits<Tag>::instance_pool_chunk_size >> 1))
	{
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
	NEVER_NULL(collection_pool_bundle);
#endif
	NEVER_NULL(_instance_pool);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default destructor.  
 */
template <class Tag>
footprint_base<Tag>::~footprint_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
footprint_base<Tag>::collect_transient_info_base(
		persistent_object_manager& m) const {
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
	NEVER_NULL(this->collection_pool_bundle);
	this->collection_pool_bundle->collect_transient_info_base(m);
#endif
	NEVER_NULL(this->_instance_pool);
	this->_instance_pool->collect_transient_info_base(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
footprint_base<Tag>::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
	NEVER_NULL(this->collection_pool_bundle);
	this->collection_pool_bundle->write_object_base(
		AS_A(const footprint&, *this), m, o);
	NEVER_NULL(this->_instance_pool);
	this->_instance_pool->write_object_base(
		*this->collection_pool_bundle, o);
#else
	NEVER_NULL(this->_instance_pool);
	this->_instance_pool->write_object_base(m, o);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
footprint_base<Tag>::load_object_base(
		const persistent_object_manager& m, istream& i) {
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
	NEVER_NULL(this->collection_pool_bundle);
	this->collection_pool_bundle->load_object_base(
		AS_A(footprint&, *this), m, i);
	NEVER_NULL(this->_instance_pool);
	this->_instance_pool->load_object_base(
		*this->collection_pool_bundle, i);
#else
	NEVER_NULL(this->_instance_pool);
	this->_instance_pool->load_object_base(m, i);
#endif
}

//=============================================================================
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
template <class Tag>
value_footprint_base<Tag>::value_footprint_base() :
		collection_pool_bundle(new collection_pool_bundle_type) {
	NEVER_NULL(collection_pool_bundle);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
value_footprint_base<Tag>::~value_footprint_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
value_footprint_base<Tag>::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	NEVER_NULL(this->collection_pool_bundle);
	this->collection_pool_bundle->write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
value_footprint_base<Tag>::load_object_base(
		const persistent_object_manager& m, istream& i) {
	NEVER_NULL(this->collection_pool_bundle);
	this->collection_pool_bundle->load_object_base(
		AS_A(footprint&, *this), m, i);
}
#endif	// POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_DEF_FOOTPRINT_BASE_TCC__

