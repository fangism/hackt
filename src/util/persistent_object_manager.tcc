/**
	\file "persistent_object_manager.tcc"
	Template methods for persistent_object_manager class.
	$Id: persistent_object_manager.tcc,v 1.11.2.1.2.3 2005/02/02 15:15:52 fang Exp $
 */

#ifndef	__UTIL_PERSISTENT_OBJECT_MANAGER_TCC__
#define	__UTIL_PERSISTENT_OBJECT_MANAGER_TCC__

// #define	ENABLE_STACKTRACE		1
// depend on whatever file includes this

#include <fstream>

#include "persistent_object_manager.h"
// already includes <iostream>

#include "macros.h"
#include "stacktrace.h"
#include "IO_utils.tcc"

#include "what.tcc"
#if ENABLE_STACKTRACE
#include "sstream.h"
#endif




#define	WELCOME_TO_TYPE_REGISTRATION			0

/**
	Application specific explicit static template method instantiation
	for persistent object IO.
	Include "persistent_object_manager.tcc" before calling this macro.
	Make sure to instantiate these in the util namespace.  
 */
#define EXPLICIT_PERSISTENT_IO_METHODS_INSTANTIATION(T)

#if 0
#define EXPLICIT_PERSISTENT_IO_METHODS_INSTANTIATION(T)			\
template excl_ptr<T>							\
persistent_object_manager::load_object_from_file<T>(const string&);	\
template excl_ptr<T>							\
persistent_object_manager::self_test<T>(const string&, const T&);	\
template excl_ptr<T>							\
persistent_object_manager::self_test_no_file<T>(const T& m);		\
template excl_ptr<T>							\
persistent_object_manager::get_root<T>(void);
#endif


namespace util {
//=============================================================================
#include "using_ostream.h"
USING_STACKTRACE
using namespace util::memory;
using util::what;
#if ENABLE_STACKTRACE
using std::ostringstream;
#endif

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
	reconstruction_function_map_type& m = reconstruction_function_map();
	const persistent::hash_key& type_key = persistent_traits<T>::type_key;
	const reconstruct_function_ptr_type probe = m[type_key];
#if WELCOME_TO_TYPE_REGISTRATION
	cerr << "Welcome to persistent type registration, "
		<< type_key << "!  ";
#endif
	if (probe) {
		cerr << "FATAL: Persistent type with key \"" <<
			type_key << "\" already taken!" << endl;
		THROW_EXIT;
	} else {
		m[type_key] = persistent_traits<T>::reconstructor;
	}
	// get a unique id
	const size_t s = m.size();
#if WELCOME_TO_TYPE_REGISTRATION
	cerr << "You are number " << s << "." << endl;
#endif
	return s;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Translates a raw pointer into an integer index and writes
	it out to binary.  
	\param ptr pointer need NOT be statically derived from
		util::persistent, but must at least be dynamically
		cast-able to util::persistent.  
 */
template <class P>
inline
void
persistent_object_manager::__write_pointer(ostream& o, 
		const P& ptr, raw_pointer_tag) const {
	// P is a bare pointer type (T*), according to the raw_pointer_tag
	if (ptr) {
		const persistent* t = IS_A(const persistent*, ptr);
		// really should write a concept check
		if (!t) {
			cerr << "Pointer @ " << ptr << " of type " <<
				util::what<typename util::memory::
					pointer_traits<P>::element_type
				>::name() << " is not derived from "
				"util::persistent, and this is unusable "
				"by write_pointer." << endl;
			THROW_EXIT;
		}
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
#if ENABLE_STACKTRACE
	static ostringstream oss;
	static const ostream& oss_ref = 
		oss << "__read_pointer-single<" <<
			what<typename P::element_type>::name() << ">()";
	STACKTRACE(oss.str());
#endif
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
#if ENABLE_STACKTRACE
	static ostringstream oss;
	static const ostream& oss_ref = 
		oss << "__read_pointer-shared<" <<
			what<typename P::element_type>::name() << ">()";
	STACKTRACE(oss.str());
#endif
	// not reference here, use a local copy first!
	const pointer_type p = pointer_manipulator::get_pointer(ptr);
	__read_pointer(f, p, __pointer_category(p));
	if (p) {
		size_t* c = lookup_ref_count(p);
		NEVER_NULL(c);
#if 0
		cerr << "ref_count @ " << p << " = " << *c << endl;
#endif
		// uses the unsafe constructor
		const_cast<P&>(ptr) = P(p, c);
		// the reference-count pointer is responsible for
		// deleting the size_t*
#if 0
		cerr << "ref_count @ " << p << " = " << *c << endl;
#endif
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
	__write_pointer(f, ptr, __pointer_category(ptr));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	ALERT: this intentially and coercively discards const-ness!
	Need to specialize for reference counter pointers!
 */
template <class P>
void
persistent_object_manager::read_pointer(istream& f, const P& ptr) const {
	STACKTRACE("pom::read_pointer()");
	__read_pointer(f, ptr, __pointer_category(ptr));
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
	for_each(l.begin(), l.end(), pointer_writer(*this, f));
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
	STACKTRACE("pom::read_pointer_list()");
	size_type s = 0;
	read_value(f, s);
	size_type i = 0;
	for ( ; i<s; i++) {
		// this should work for sticky pointer!
		l.push_back(pointer_type());
		read_pointer(f, l.back());		// in-place
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
#if 0
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param P is generic pointer to a persistent type.  
 */
template <class P>
void
persistent_object_manager::load_object(const P& p) const {
	__load_object(p, __pointer_category(p));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param P is generic pointer to a persistent type.  
 */
template <class P>
void
persistent_object_manager::__load_object(const P& p, 
		pointer_class_base_tag) const {
	__load_object(&*p, __pointer_category(&*p));
}

//=============================================================================
// class persistent_traits method definitions (default)

/**
	Initially null.
	Will be set by constructing an object.  
 */
template <class T>
persistent::hash_key
persistent_traits<T>::type_key;

template <class T>
int
persistent_traits<T>::type_id = 0;

template <class T>
reconstruct_function_ptr_type
persistent_traits<T>::reconstructor = &T::construct_empty;

template <class T>
persistent*
persistent_traits<T>::null = static_cast<T*>(NULL);

template <class T>
persistent_traits<T>::persistent_traits(const string& s) {
	INVARIANT(!type_id);
	INVARIANT(type_key == persistent::hash_key::null);
	type_key = s;
	type_id = persistent_object_manager::register_persistent_type<T>();
}

//=============================================================================
}	// end namespace util

#endif	//	__UTIL_PERSISTENT_OBJECT_MANAGER_TCC__

