/**
	\file "Object/def/footprint_base.tcc"
	Implementation of footprint class. 
	$Id: footprint_base.tcc,v 1.1.2.4 2006/11/07 00:47:40 fang Exp $
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
	collection_pool_bundle(new collection_pool_bundle_type), 
	_instance_pool(new instance_pool_type(
		class_traits<Tag>::instance_pool_chunk_size >> 1)) {
	NEVER_NULL(collection_pool_bundle);
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
	NEVER_NULL(this->collection_pool_bundle);
	this->collection_pool_bundle->collect_transient_info_base(m);
	NEVER_NULL(this->_instance_pool);
	this->_instance_pool->collect_transient_info_base(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
footprint_base<Tag>::write_reserve_sizes(ostream& o) const {
	NEVER_NULL(this->collection_pool_bundle);
	this->collection_pool_bundle->write_reserve_sizes(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
footprint_base<Tag>::load_reserve_sizes(istream& i) {
	NEVER_NULL(this->collection_pool_bundle);
	this->collection_pool_bundle->load_reserve_sizes(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
footprint_base<Tag>::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	NEVER_NULL(this->collection_pool_bundle);
	this->collection_pool_bundle->write_object_base(
		AS_A(const footprint&, *this), m, o);
	NEVER_NULL(this->_instance_pool);
	this->_instance_pool->write_object_base(
		*this->collection_pool_bundle, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
footprint_base<Tag>::load_object_base(
		const persistent_object_manager& m, istream& i) {
	NEVER_NULL(this->collection_pool_bundle);
	this->collection_pool_bundle->load_object_base(
		AS_A(footprint&, *this), m, i);
	NEVER_NULL(this->_instance_pool);
	this->_instance_pool->load_object_base(
		*this->collection_pool_bundle, i);
}

//=============================================================================
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

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_DEF_FOOTPRINT_BASE_TCC__

