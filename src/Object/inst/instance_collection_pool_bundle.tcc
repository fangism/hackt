/**
	\file "Object/inst/instance_collection_pool_bundle.h"
	$Id: instance_collection_pool_bundle.tcc,v 1.1.2.6 2006/11/04 21:59:22 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_COLLECTION_POOL_BUNDLE_TCC__
#define	__HAC_OBJECT_INST_INSTANCE_COLLECTION_POOL_BUNDLE_TCC__

#include <iostream>
#include <algorithm>
#include "Object/inst/instance_collection_pool_bundle.h"
#include "Object/inst/collection_pool.tcc"
#include "Object/inst/instance_placeholder.h"
#include "Object/inst/instance_scalar.h"
#include "Object/inst/instance_array.h"
#include "Object/inst/port_formal_array.h"
#include "Object/inst/port_actual_collection.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/collection_traits.h"
#include "Object/def/footprint.h"
#include "common/ICE.h"
#include "util/persistent_object_manager.tcc"
#include "util/persistent_functor.tcc"
#include "util/stacktrace.h"
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
// class instance_collection_pool_wrapper helper structs

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Binding functor.  
 */
template <class T>
struct instance_collection_pool_wrapper<T>::collection_writer : 
		public util::persistent_writer_base {
	const footprint&	fp;
	collection_writer(const footprint& f, 
		const persistent_object_manager& m, ostream& o) :
		util::persistent_writer_base(m, o), fp(f) { }

	void
	operator () (const T& t) {
		t.write_object(fp, pom, os);
	}
};	// end struct collection_writer

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Binding functor.  
	Also loads footprint's map!
 */
template <class T>
struct instance_collection_pool_wrapper<T>::collection_loader :
		public util::persistent_loader_base {
	footprint&	fp;
	collection_loader(footprint& f, 
		const persistent_object_manager& m, istream& i) :
		util::persistent_loader_base(m, i), fp(f) { }

	void
	operator () (T& t) {
		t.load_object(fp, pom, is);
	}
};	// end struct collection_loader

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
struct instance_collection_pool_wrapper<T>::connection_writer {
	const pool_bundle_type&		pool_bundle;
	ostream&			os;
	connection_writer(const pool_bundle_type& p, ostream& o) :
		pool_bundle(p), os(o) { }

	void
	operator () (const T& t) {
		t.write_connections(pool_bundle, os);
	}
};	// end struct connection_writer

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
struct instance_collection_pool_wrapper<T>::connection_loader {
	const pool_bundle_type&		pool_bundle;
	istream&			is;
	connection_loader(const pool_bundle_type& p, istream& i) :
		pool_bundle(p), is(i) { }

	void
	operator () (T& t) {
		t.load_connections(pool_bundle, is);
	}
};	// end struct connection_loader

//=============================================================================
// class instance_collection_pool_wrapper method definitions

template <class T>
void
instance_collection_pool_wrapper<T>::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_VERBOSE;
	STACKTRACE_THIS
	const const_iterator b(this->pool.begin()), e(this->pool.end());
	for_each(b, e, util::persistent_collector_ref(m));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
void
instance_collection_pool_wrapper<T>::write_object_base(
		const footprint& f, 
		const persistent_object_manager& m, ostream& o) const {
	STACKTRACE_VERBOSE;
	STACKTRACE_THIS
	const size_t s = this->pool.size();
	write_value(o, s);
	const const_iterator b(this->pool.begin()), e(this->pool.end());
	for_each(b, e, collection_writer(f, m, o));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
void
instance_collection_pool_wrapper<T>::load_object_base(
		footprint& f,
		const persistent_object_manager& m, istream& i) {
	STACKTRACE_VERBOSE;
	STACKTRACE_THIS
	size_t s;
	read_value(i, s);
	this->pool.allocate(s);		// will be contiguous! (first chunk)
	INVARIANT(this->pool.size() == s);
	const iterator b(this->pool.begin()), e(this->pool.end());
	for_each(b, e, collection_loader(f, m, i));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre size has not changed since write_object_base
 */
template <class T>
void
instance_collection_pool_wrapper<T>::write_connections(
		const pool_bundle_type& m, ostream& o) const {
	STACKTRACE_VERBOSE;
	STACKTRACE_THIS
	const const_iterator b(this->pool.begin()), e(this->pool.end());
	for_each(b, e, connection_writer(m, o));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre collection has already been allocated to correct size.  
 */
template <class T>
void
instance_collection_pool_wrapper<T>::load_connections(
		const pool_bundle_type& m, istream& i) {
	STACKTRACE_VERBOSE;
	STACKTRACE_THIS
	const iterator b(this->pool.begin()), e(this->pool.end());
	for_each(b, e, connection_loader(m, i));
}

//=============================================================================
// class instance_collection_pool_bundle method definitions

template <class Tag>
instance_collection_pool_bundle<Tag>::instance_collection_pool_bundle() :
		instance_collection_pool_wrapper<instance_array<Tag, 0> >(), 
		instance_collection_pool_wrapper<instance_array<Tag, 1> >(), 
		instance_collection_pool_wrapper<instance_array<Tag, 2> >(), 
		instance_collection_pool_wrapper<instance_array<Tag, 3> >(), 
		instance_collection_pool_wrapper<instance_array<Tag, 4> >(), 
		instance_collection_pool_wrapper<port_formal_array<Tag> >(), 
		instance_collection_pool_wrapper<port_actual_collection<Tag> >() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
instance_collection_pool_bundle<Tag>::~instance_collection_pool_bundle() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
never_ptr<typename instance_collection_pool_bundle<Tag>::collection_interface_type>
instance_collection_pool_bundle<Tag>::lookup_collection(
		const unsigned char t,
		const collection_index_entry::index_type i) const {
	typedef	never_ptr<collection_interface_type>	return_type;
	STACKTRACE_VERBOSE;
	STACKTRACE_THIS
	switch (t) {
#define	COLLECTION_TYPE_LOOKUP_CASE(v)					\
	case v: return return_type(&instance_collection_pool_wrapper<	\
			typename instance_collection_type_map<v>::	\
			template collection<Tag>::type>::pool[i]);
	COLLECTION_TYPE_LOOKUP_CASE(INSTANCE_COLLECTION_TYPE_SCALAR)
	COLLECTION_TYPE_LOOKUP_CASE(INSTANCE_COLLECTION_TYPE_1D)
	COLLECTION_TYPE_LOOKUP_CASE(INSTANCE_COLLECTION_TYPE_2D)
	COLLECTION_TYPE_LOOKUP_CASE(INSTANCE_COLLECTION_TYPE_3D)
	COLLECTION_TYPE_LOOKUP_CASE(INSTANCE_COLLECTION_TYPE_4D)
	COLLECTION_TYPE_LOOKUP_CASE(INSTANCE_COLLECTION_TYPE_PORT_FORMAL)
	COLLECTION_TYPE_LOOKUP_CASE(INSTANCE_COLLECTION_TYPE_PORT_ACTUAL)
#undef	COLLECTION_TYPE_LOOKUP_CASE
	default:
		ICE(cerr, cerr << "Unknown collection enum: " << t << endl;)
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
typename instance_collection_pool_bundle<Tag>::collection_interface_type*
instance_collection_pool_bundle<Tag>::read_pointer(istream& i) const {
	typedef	collection_interface_type*	return_type;
	STACKTRACE_VERBOSE;
	STACKTRACE_THIS
	unsigned char ct;
	collection_index_entry::index_type ci;
	read_value(i, ct);
	read_value(i, ci);
	return &*this->lookup_collection(ct, ci);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
port_actual_collection<Tag>*
instance_collection_pool_bundle<Tag>::allocate_port_collection(
		const never_ptr<const instance_collection<Tag> > f, 
		const unroll_context& c) {
	STACKTRACE_VERBOSE;
	STACKTRACE_THIS
	port_actual_collection<Tag>* const ret = 
		instance_collection_pool_wrapper<port_actual_collection<Tag> >
			::pool.allocate();	// already default constructed
	ret->~port_actual_collection<Tag>();		// just in case...
	new (ret) port_actual_collection<Tag>(f, c);	// placement construct
	// does not need to register as named with footprint's collection_map
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Replacement for instance_collection::make_port_formal_array.
	This also registers the collection with the footprint!
 */
template <class Tag>
instance_collection<Tag>*
instance_collection_pool_bundle<Tag>::allocate_port_formal(
		footprint& f, 
		const never_ptr<const instance_placeholder<Tag> > p) {
	STACKTRACE_VERBOSE;
	STACKTRACE_THIS
	NEVER_NULL(p);
	const size_t dim = p->get_dimensions();
	switch (dim) {
	case 0: {
		typedef	instance_array<Tag, 0>	array_type;
#define	ALLOCATE_AND_REGISTER_IT					\
		collection_pool<array_type>&				\
			_pool(this->get_collection_pool<array_type>());	\
		array_type* const ret = _pool.allocate();		\
		ret->~array_type();					\
		new (ret) array_type(p);				\
		f.register_collection_map_entry(			\
			ret->get_footprint_key(), 			\
			lookup_collection_pool_index_entry(_pool, *ret)); \
		return ret;
		ALLOCATE_AND_REGISTER_IT
	}
	case 1:
	case 2:
	case 3:
	case 4: {
		typedef	port_formal_array<Tag>	array_type;
		ALLOCATE_AND_REGISTER_IT
	}
	default:
		ICE(cerr, cerr << "FATAL: dimension limit is 4!" << endl;)
		return NULL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Replacement for instance_collection::make_array.
	This also registers the collection with the footprint!
 */
template <class Tag>
instance_collection<Tag>*
instance_collection_pool_bundle<Tag>::allocate_local_collection(
		footprint& f, 
		const never_ptr<const instance_placeholder<Tag> > p) {
	STACKTRACE_VERBOSE;
	STACKTRACE_THIS
	NEVER_NULL(p);
	const size_t dim = p->get_dimensions();
	switch (dim) {
#define	CASE_ALLOCATE_SPARSE_ARRAY(D)					\
	case D: {							\
		typedef	instance_array<Tag, D>	array_type;		\
		ALLOCATE_AND_REGISTER_IT				\
	}
	CASE_ALLOCATE_SPARSE_ARRAY(0)
	CASE_ALLOCATE_SPARSE_ARRAY(1)
	CASE_ALLOCATE_SPARSE_ARRAY(2)
	CASE_ALLOCATE_SPARSE_ARRAY(3)
	CASE_ALLOCATE_SPARSE_ARRAY(4)
#undef	CASE_ALLOCATE_SPARSE_ARRAY
#undef	ALLOCATE_AND_REGISTER_IT
	default:
		ICE(cerr, cerr << "FATAL: dimension limit is 4!" << endl;)
		return NULL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
instance_collection_pool_bundle<Tag>::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_VERBOSE;
	STACKTRACE_THIS
	instance_collection_pool_wrapper<instance_array<Tag, 0> >
		::collect_transient_info_base(m);
	instance_collection_pool_wrapper<instance_array<Tag, 1> >
		::collect_transient_info_base(m);
	instance_collection_pool_wrapper<instance_array<Tag, 2> >
		::collect_transient_info_base(m);
	instance_collection_pool_wrapper<instance_array<Tag, 3> >
		::collect_transient_info_base(m);
	instance_collection_pool_wrapper<instance_array<Tag, 4> >
		::collect_transient_info_base(m);
	instance_collection_pool_wrapper<port_formal_array<Tag> >
		::collect_transient_info_base(m);
	instance_collection_pool_wrapper<port_actual_collection<Tag> >
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
		const footprint& f, 
		const persistent_object_manager& m, ostream& o) const {
	STACKTRACE_VERBOSE;
	STACKTRACE_THIS
	instance_collection_pool_wrapper<instance_array<Tag, 0> >
		::write_object_base(f, m, o);
	instance_collection_pool_wrapper<instance_array<Tag, 1> >
		::write_object_base(f, m, o);
	instance_collection_pool_wrapper<instance_array<Tag, 2> >
		::write_object_base(f, m, o);
	instance_collection_pool_wrapper<instance_array<Tag, 3> >
		::write_object_base(f, m, o);
	instance_collection_pool_wrapper<instance_array<Tag, 4> >
		::write_object_base(f, m, o);
	instance_collection_pool_wrapper<port_formal_array<Tag> >
		::write_object_base(f, m, o);
	instance_collection_pool_wrapper<port_actual_collection<Tag> >
		::write_object_base(f, m, o);

	instance_collection_pool_wrapper<instance_array<Tag, 0> >
		::write_connections(*this, o);
	instance_collection_pool_wrapper<instance_array<Tag, 1> >
		::write_connections(*this, o);
	instance_collection_pool_wrapper<instance_array<Tag, 2> >
		::write_connections(*this, o);
	instance_collection_pool_wrapper<instance_array<Tag, 3> >
		::write_connections(*this, o);
	instance_collection_pool_wrapper<instance_array<Tag, 4> >
		::write_connections(*this, o);
	instance_collection_pool_wrapper<port_formal_array<Tag> >
		::write_connections(*this, o);
	instance_collection_pool_wrapper<port_actual_collection<Tag> >
		::write_connections(*this, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
instance_collection_pool_bundle<Tag>::load_object_base(
		footprint& f, 
		const persistent_object_manager& m, istream& i) {
	STACKTRACE_VERBOSE;
	STACKTRACE_THIS
	instance_collection_pool_wrapper<instance_array<Tag, 0> >
		::load_object_base(f, m, i);
	instance_collection_pool_wrapper<instance_array<Tag, 1> >
		::load_object_base(f, m, i);
	instance_collection_pool_wrapper<instance_array<Tag, 2> >
		::load_object_base(f, m, i);
	instance_collection_pool_wrapper<instance_array<Tag, 3> >
		::load_object_base(f, m, i);
	instance_collection_pool_wrapper<instance_array<Tag, 4> >
		::load_object_base(f, m, i);
	instance_collection_pool_wrapper<port_formal_array<Tag> >
		::load_object_base(f, m, i);
	instance_collection_pool_wrapper<port_actual_collection<Tag> >
		::load_object_base(f, m, i);

	instance_collection_pool_wrapper<instance_array<Tag, 0> >
		::load_connections(*this, i);
	instance_collection_pool_wrapper<instance_array<Tag, 1> >
		::load_connections(*this, i);
	instance_collection_pool_wrapper<instance_array<Tag, 2> >
		::load_connections(*this, i);
	instance_collection_pool_wrapper<instance_array<Tag, 3> >
		::load_connections(*this, i);
	instance_collection_pool_wrapper<instance_array<Tag, 4> >
		::load_connections(*this, i);
	instance_collection_pool_wrapper<port_formal_array<Tag> >
		::load_connections(*this, i);
	instance_collection_pool_wrapper<port_actual_collection<Tag> >
		::load_connections(*this, i);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#undef	STACKTRACE_THIS

#endif	// __HAC_OBJECT_INST_INSTANCE_COLLECTION_POOL_BUNDLE_TCC__

