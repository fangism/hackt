/**
	\file "Object/inst/value_collection_pool_bundle.h"
	$Id: value_collection_pool_bundle.tcc,v 1.1.2.2 2006/11/03 05:22:34 fang Exp $
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
#include "Object/inst/collection_traits.h"
#include "Object/def/footprint.h"
#include "common/ICE.h"
#include "util/stacktrace.h"
#include "util/persistent_object_manager.tcc"
#include "util/persistent_functor.tcc"
#include "util/multikey.h"
#include "util/IO_utils.tcc"

#if	ENABLE_STACKTRACE
#define	STACKTRACE_THIS	STACKTRACE_INDENT_PRINT("this @ " << this << endl);
#else
#define	STACKTRACE_THIS
#endif

namespace HAC {
namespace entity {
using std::for_each;
using util::write_value;
using util::read_value;
#include "util/using_ostream.h"

//=============================================================================
// class value_collection_pool_wrapper method definitions

template <class T>
void
value_collection_pool_wrapper<T>::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	STACKTRACE_VERBOSE;
	STACKTRACE_THIS
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
	STACKTRACE_VERBOSE;
	STACKTRACE_THIS
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
never_ptr<typename value_collection_pool_bundle<Tag>::collection_base_type>
value_collection_pool_bundle<Tag>::lookup_collection(
		const unsigned char t, 
		const collection_index_entry::index_type i) const {

	typedef never_ptr<collection_base_type>    return_type;
	STACKTRACE_VERBOSE;
	STACKTRACE_THIS
	switch (t) {
#define	COLLECTION_TYPE_LOOKUP_CASE(v)					\
	case v: return return_type(&value_collection_pool_wrapper<	\
			typename value_collection_type_map<v>::		\
			template collection<Tag>::type>::pool[i]);
	COLLECTION_TYPE_LOOKUP_CASE(VALUE_COLLECTION_TYPE_SCALAR)
	COLLECTION_TYPE_LOOKUP_CASE(VALUE_COLLECTION_TYPE_1D)
	COLLECTION_TYPE_LOOKUP_CASE(VALUE_COLLECTION_TYPE_2D)
	COLLECTION_TYPE_LOOKUP_CASE(VALUE_COLLECTION_TYPE_3D)
	COLLECTION_TYPE_LOOKUP_CASE(VALUE_COLLECTION_TYPE_4D)
//	COLLECTION_TYPE_LOOKUP_CASE(VALUE_COLLECTION_TYPE_TEMPLATE_FORMAL)
#undef	COLLECTION_TYPE_LOOKUP_CASE
	default:
		ICE(cerr, cerr << "Unknown collection enum: " << t << endl;)
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Replacement for value_collection::make_port_formal_array.
 */
template <class Tag>
value_collection<Tag>*
value_collection_pool_bundle<Tag>::allocate_local_collection(
		footprint& f, 
		const never_ptr<const value_placeholder<Tag> > p) {
	STACKTRACE_VERBOSE;
	STACKTRACE_THIS
	NEVER_NULL(p);
	const size_t dim = p->get_dimensions();
	switch (dim) {
#define	CASE_ALLOCATE_SPARSE_ARRAY(D)					\
	case D: {							\
		typedef	value_array<Tag, D>	array_type;		\
		collection_pool<array_type>&				\
			_pool(this->get_collection_pool<array_type>());	\
		array_type* const ret = _pool.allocate();		\
		ret->~array_type();					\
		new (ret) array_type(p);				\
		f.register_collection_map_entry(			\
			ret->get_footprint_key(), 			\
			lookup_collection_pool_index_entry(_pool, *ret)); \
		return ret;						\
	}
	CASE_ALLOCATE_SPARSE_ARRAY(0)
	CASE_ALLOCATE_SPARSE_ARRAY(1)
	CASE_ALLOCATE_SPARSE_ARRAY(2)
	CASE_ALLOCATE_SPARSE_ARRAY(3)
	CASE_ALLOCATE_SPARSE_ARRAY(4)
#undef	CASE_ALLOCATE_SPARSE_ARRAY
	default:
		ICE(cerr, cerr << "FATAL: dimension limit is 4!" << endl;)
		return NULL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
value_collection_pool_bundle<Tag>::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	STACKTRACE_VERBOSE;
	STACKTRACE_THIS
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
	STACKTRACE_VERBOSE;
	STACKTRACE_THIS
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

#undef	STACKTRACE_THIS

#endif	// __HAC_OBJECT_INST_VALUE_COLLECTION_POOL_BUNDLE_TCC__

