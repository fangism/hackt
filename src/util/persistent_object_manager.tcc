/**
	\file "persistent_object_manager.tcc"
	Template methods for persistent_object_manager class.
	$Id: persistent_object_manager.tcc,v 1.9 2004/12/05 05:07:25 fang Exp $
 */

#ifndef	__PERSISTENT_OBJECT_MANAGER_TCC__
#define	__PERSISTENT_OBJECT_MANAGER_TCC__

#include <fstream>

#include "persistent_object_manager.h"
// already includes <iostream>

#include "macros.h"
#include "IO_utils.tcc"

#define	WELCOME_TO_TYPE_REGISTRATION			0

/**
	Application specific explicit static template method instantiation
	for persistent object IO.
	Include "persistent_object_manager.tcc" before calling this macro.
	Make sure to instantiate these in the util namespace.  
 */
#define EXPLICIT_PERSISTENT_IO_METHODS_INSTANTIATION(T)			\
template excl_ptr<T>							\
persistent_object_manager::load_object_from_file<T>(const string&);	\
template excl_ptr<T>							\
persistent_object_manager::self_test<T>(const string&, const T&);	\
template excl_ptr<T>							\
persistent_object_manager::self_test_no_file<T>(const T& m);		\
template excl_ptr<T>							\
persistent_object_manager::get_root<T>(void);



namespace util {
//=============================================================================
#include "using_ostream.h"
using namespace util::memory;

//=============================================================================
// class persistent_object_manager template method definitions

/**
	Concept require: that T has a static persistent::hash_key
	named persistent_type_key, and a static member function
	called construct_empty.  
	These requirements cannot be made explicit in the persistent class.  
	Of course, the persistent_type_key must be properly initialized
	before use.  
 */
template <class T>
int
persistent_object_manager::register_persistent_type(void) {
	reconstruction_function_map_type& m = get_reconstruction_function_map();
	const reconstruct_function_ptr_type probe = m[T::persistent_type_key];
#if WELCOME_TO_TYPE_REGISTRATION
	cerr << "Welcome to persistent type registration, "
		<< T::persistent_type_key << "!  ";
#endif
	if (probe) {
		cerr << "FATAL: Persistent type with key \"" <<
			T::persistent_type_key << "\" already taken!" << endl;
		exit(1);
	} else {
		m[T::persistent_type_key] = &T::construct_empty;
#if 0
		m[T::persistent_type_key] =
			reconstruction_functor(T::construct_empty);
#endif
	}
	// get a unique id
	const size_t s = m.size();
#if WELCOME_TO_TYPE_REGISTRATION
	cerr << "You are number " << s << "." << endl;
#endif
	return s;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class P>
inline
void
persistent_object_manager::__write_pointer(ostream& o, 
		const P& ptr, raw_pointer_tag) const {
	// P is a bare pointer type (T*), according to the raw_pointer_tag
	if (ptr) {
		const persistent* t = IS_A(const persistent*, ptr);
		NEVER_NULL(t);
		write_value(o, lookup_ptr_index(t));
	} else	write_value(o, lookup_ptr_index(NULL));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class P>
inline
void
persistent_object_manager::__write_pointer(ostream& o, 
		const P& ptr, pointer_class_base_tag) const {
	const typename pointer_traits<P>::pointer&
		p = pointer_manipulator::get_pointer(ptr);
	__write_pointer(o, p, __pointer_category(p));
		// should be raw_pointer_tag
		// should not get infinite recursion at compile time b/c
		// P::pointer should be a raw_pointer
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class P>
inline
void
persistent_object_manager::__read_pointer(istream& f, 
		const P& ptr, raw_pointer_tag) const {
	typedef typename pointer_traits<P>::pointer	pointer_type;
	unsigned long i;
	read_value(f, i);
	INVARIANT(i < reconstruction_table.size());
	persistent* o(lookup_obj_ptr(i));
	// for this to work, pointer_type must be a raw_pointer
	const_cast<P&>(ptr) = dynamic_cast<pointer_type>(o);
	if (o) NEVER_NULL(ptr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class P>
inline
void
persistent_object_manager::__read_pointer(istream& f, 
		const P& ptr, single_owner_pointer_tag) const {
	const typename pointer_traits<P>::pointer&
		p = pointer_manipulator::get_pointer(ptr);
	__read_pointer(f, p, __pointer_category(p));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class P>
inline
void
persistent_object_manager::__read_pointer(istream& f, 
		const P& ptr, shared_owner_pointer_tag) const {
	typedef typename pointer_traits<P>::pointer	pointer_type;
	// not reference here, use a local copy first!
	const pointer_type p = pointer_manipulator::get_pointer(ptr);
	__read_pointer(f, p, __pointer_category(p));
	if (p) {
		size_t* c = lookup_ref_count(p);
		NEVER_NULL(c);
		// uses the unsafe constructor
		const_cast<P&>(ptr) = P(p, c);
		// the reference-count pointer is responsible for
		// deleting the size_t*
	} else {
		const_cast<P&>(ptr) = P(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Doesn't actually write out the pointer, but the index 
	representing the object represented by the pointer.
	Precondition: pointer must already be registered.
	\param f output (file) stream.
	\param ptr the pointer (class) object to translate and 
		write out.
 */
template <class P>
void
persistent_object_manager::write_pointer(ostream& f, const P& ptr) const {
#if 0
	// this extracts the naked pointer
	if (ptr) {
		const persistent* o_ptr = IS_A(const persistent*, &*ptr);
		assert(o_ptr);
		write_value(f, lookup_ptr_index(o_ptr));
	} else	write_value(f, lookup_ptr_index(NULL));
#else
	__write_pointer(f, ptr, __pointer_category(ptr));
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	ALERT: this intentially and coercively discards const-ness!
	Need to specialize for reference counter pointers!
 */
template <class P>
void
persistent_object_manager::read_pointer(istream& f, const P& ptr) const {
#if 0
	unsigned long i;
	read_value(f, i);
	assert(i < reconstruction_table.size());
	persistent* o(lookup_obj_ptr(i));
	T* t = dynamic_cast<T*>(o);
	if (o) assert(t);
	const_cast<P<T>& >(ptr) = P<T>(t);
#else
	__read_pointer(f, ptr, __pointer_category(ptr));
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class P>
inline
void
persistent_object_manager::pointer_writer::operator() (const P& p) {
	pom.write_pointer(os, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class P>
inline
void
persistent_object_manager::pointer_reader::operator() (const P& p) {
	pom.read_pointer(is, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Partial specialization of read_pointer for 
	reference-counted pointers.
 */
template <class T>
void
persistent_object_manager::read_pointer(
		istream& f, const count_ptr<const T>& ptr) const {
	long i;
	read_value(f, i);
	persistent* o(lookup_obj_ptr(i));
	if (o) {
		T* t = dynamic_cast<T*>(o);
#if 0
		if (!t) {
			// to catch type mismatch errors!
			// if the below assert(t) should fail
			o->what(cerr << "persistent* o = " << o << " ") << endl;
		}
#endif
		size_t* c = lookup_ref_count(i);
			// will allocate if NULL
		assert(t);
		assert(c);
		// uses the unsafe constructor
		const_cast<count_ptr<const T>& >(ptr) =
			count_ptr<const T>(t, c);
	} else {
		const_cast<count_ptr<const T>& >(ptr) =
			count_ptr<const T>(NULL);
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Writes a sequence of pointers, mapped to indices.
	Container only needs a simple forward iterator interface.  
 */
template <class L>
void
persistent_object_manager::write_pointer_list(ostream& f, const L& l) const {
	// concept requirements:
	// L is a sequence-type container, has begin and end
	// pointer_traits<L::value_type> is raw_pointer or pointer class
	// write number of elements to expect first
	const typename L::size_type s = l.size();
	write_value(f, s);
#if 0
	typename L::const_iterator iter = l.begin();
	const typename L::const_iterator end = l.end();
	for ( ; iter!=end; iter++)
		write_pointer(f, *iter);
#else
	for_each(l.begin(), l.end(), pointer_writer(*this, f));
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reconstructs a sequence of pointers, mapped to indices.  
	Container only needs a simple forward iterator interface.  
 */
template <class L>
void
persistent_object_manager::read_pointer_list(istream& f, L& l) const {
	typedef	typename L::size_type	size_type;
	typedef	typename L::value_type	pointer_type;
	// assert(l.empty()); ?
	size_type s = 0;
	read_value(f, s);
	size_type i = 0;
	for ( ; i<s; i++) {
#if 0
		// won't work for sticky pointer!
		P<T> ptr;
		read_pointer(f, ptr);
		l.push_back(ptr);
#else
		l.push_back(pointer_type());
		read_pointer(f, l.back());		// in-place
#endif
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Writes a map of pointers without their keys.  
	Not used yet.  Enable when it becomes useful...
 */
template <class M>
void
persistent_object_manager::write_pointer_map(ostream& f, const M& m) const {
	typedef typename M::key_type	key_type;
	typedef typename M::value_type	value_type;
	typedef typename M::mapped_type	mapped_type;
	typedef typename M::size_type	size_type;
	typedef typename M::const_iterator	const_iterator;
	size_type s = m.size();
	write_value(f, s);
	const_iterator iter = m.begin();
	const const_iterator end = m.end();
	for ( ; iter!=end; iter++)
		write_pointer(f, iter->second);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// specialized list methods?

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The first non-NULL object is special: it is the root module.
	Returning an excl_ptr guarantees that memory will
	be managed properly.
	When the excl_ptr hits the end of a scope, unless ownership
	has been transferred, the memory should be recursively reclaimed.
	Thus, this is not a const method.
 */
template <class T>
excl_ptr<T>
persistent_object_manager::get_root(void) {
	assert(root);		// necessary?
	// the template keyword is required for gcc-3.3.x, but not for 3.4.x
	return root.template is_a_xfer<T>();
	// this relinquishes ownership and responsibility for deleting
	// to whomever consumes the returned excl_ptr
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
        Loads hierarchical object collection from file.
	\returns a dynamically cast owned pointer to the root object.
 */
template <class T>
excl_ptr<T>
persistent_object_manager::load_object_from_file(const string& s) {
	ifstream f(s.c_str(), ios_base::binary);
	persistent_object_manager pom;
	// don't initialize_null, will be loaded in from table
	pom.load_header(f);
	pom.finish_load(f);
	f.close();				// done with file
	pom.reconstruct();			// allocate-only pass
	if (dump_reconstruction_table)
		pom.dump_text(cerr << endl) << endl;	// debugging only
	// Oh no, partially initialized objects!
	pom.load_objects();
	return pom.get_root<T>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Write the reconstruction table, and loads it back, without
	going through an intermediate file.
	Should essentially make a deep copy of the hierarchical object
	rooted at the global namespace.
 */
template <class T>
excl_ptr<T>
persistent_object_manager::self_test_no_file(const T& m) {
	persistent_object_manager pom;
	pom.initialize_null();			// reserved 0th entry
	m.collect_transient_info(pom);		// recursive visitor
	pom.collect_objects();			// buffers output in segments
	if (dump_reconstruction_table)
		pom.dump_text(cerr << endl) << endl;	// for debugging

	// need to set start of objects? no
	// pretend we wrote it out and read it back in...
	pom.reset_for_loading();
	pom.reconstruct();			// allocate-only pass

	if (dump_reconstruction_table)
		pom.dump_text(cerr << endl) << endl;	// debugging only

	pom.load_objects();
	// must acquire root object in some owned pointer!
	return pom.get_root<T>();		// only this is templated
	// will get de-allocated after return statement is evaluated
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Writes out and reads back in, through an intermediate file.
 */
template <class T>
excl_ptr<T>
persistent_object_manager::self_test(const string& s, const T& m) {
	save_object_to_file(s, m);
	return load_object_from_file<T>(s);
}

//=============================================================================
}	// end namespace util

#endif	//	__PERSISTENT_OBJECT_MANAGER_TCC__

