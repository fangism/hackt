/**
	\file "Object/inst/value_collection_pool_bundle.h"
	$Id: value_collection_pool_bundle.tcc,v 1.1.2.1 2006/10/31 05:23:57 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_VALUE_COLLECTION_POOL_BUNDLE_TCC__
#define	__HAC_OBJECT_INST_VALUE_COLLECTION_POOL_BUNDLE_TCC__

#include <iostream>
#include <algorithm>
#include "Object/inst/value_collection_pool_bundle.h"
#include "Object/inst/param_value_collection.h"
#include "Object/inst/collection_pool.tcc"
#include "Object/inst/value_scalar.h"
#include "Object/inst/value_array.h"
#include "Object/expr/const_collection.h"
#include "Object/expr/const_param.h"
#include "util/persistent_object_manager.tcc"
#include "util/persistent_functor.tcc"
#include "util/multikey.h"
#include "util/IO_utils.tcc"

namespace HAC {
namespace entity {
using std::for_each;
using util::write_value;
using util::read_value;

//=============================================================================
// class value_collection_pool_wrapper method definitions

template <class T>
void
value_collection_pool_wrapper<T>::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	const size_t s = this->pool.size();
	write_value(o, s);
	for_each(this->pool.begin(), this->pool.end(), 
		util::persistent_writer_ref(m, o));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
void
value_collection_pool_wrapper<T>::load_object_base(
		const persistent_object_manager& m, istream& i) {
	size_t s;
	read_value(i, s);
	this->pool.allocate(s);		// will be contiguous! (first chunk)
	INVARIANT(this->pool.size() == s);
	for_each(this->pool.begin(), this->pool.end(), 
		util::persistent_loader_ref(m, i));
}

//=============================================================================
// class value_collection_pool_bundle method definitions

template <class Tag>
value_collection_pool_bundle<Tag>::value_collection_pool_bundle() :
		value_collection_pool_wrapper<value_array<Tag, 0> >(), 
		value_collection_pool_wrapper<value_array<Tag, 1> >(), 
		value_collection_pool_wrapper<value_array<Tag, 2> >(), 
		value_collection_pool_wrapper<value_array<Tag, 3> >(), 
		value_collection_pool_wrapper<value_array<Tag, 4> >() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
value_collection_pool_bundle<Tag>::~value_collection_pool_bundle() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
value_collection_pool_bundle<Tag>::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	value_collection_pool_wrapper<value_array<Tag, 0> >
		::write_object_base(m, o);
	value_collection_pool_wrapper<value_array<Tag, 1> >
		::write_object_base(m, o);
	value_collection_pool_wrapper<value_array<Tag, 2> >
		::write_object_base(m, o);
	value_collection_pool_wrapper<value_array<Tag, 3> >
		::write_object_base(m, o);
	value_collection_pool_wrapper<value_array<Tag, 4> >
		::write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
value_collection_pool_bundle<Tag>::load_object_base(
		const persistent_object_manager& m, istream& i) {
	value_collection_pool_wrapper<value_array<Tag, 0> >
		::load_object_base(m, i);
	value_collection_pool_wrapper<value_array<Tag, 1> >
		::load_object_base(m, i);
	value_collection_pool_wrapper<value_array<Tag, 2> >
		::load_object_base(m, i);
	value_collection_pool_wrapper<value_array<Tag, 3> >
		::load_object_base(m, i);
	value_collection_pool_wrapper<value_array<Tag, 4> >
		::load_object_base(m, i);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_VALUE_COLLECTION_POOL_BUNDLE_TCC__

