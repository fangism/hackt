// "art_object_IO.tcc"
// template methods for persistent_object_manager class

#ifndef	__ART_OBJECT_IO_TCC__
#define	__ART_OBJECT_IO_TCC__

#include "art_object_IO.h"
#include "art_utils.tcc"

namespace ART {
namespace entity {
//=============================================================================
using namespace std;
using namespace PTRS_NAMESPACE;
using namespace COUNT_PTR_NAMESPACE;

//=============================================================================
// class persistent_object_manager template method definitions

/**
	Doesn't actually write out the pointer, but the index 
	representing the object represented by the pointer.
	Precondition: pointer must already be registered.
	\param f output (file) stream.
	\param ptr the pointer (class) object to translate and 
		write out.
 */
template <template <class> class P, class T>
void
persistent_object_manager::write_pointer(
		ostream& f, const P<T>& ptr) const {
	// this extracts the naked pointer
	if (ptr)	write_value(f, lookup_ptr_index(&*ptr));
	else		write_value(f, lookup_ptr_index(NULL));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	ALERT: this intentially and coercively discards const-ness!
	Need to specialize for reference counter pointers!
 */
template <template <class> class P, class T>
void
persistent_object_manager::read_pointer(
		istream& f, const P<T>& ptr) const {
	long i;
	read_value(f, i);
	object* o(lookup_obj_ptr(i));
	T* t = dynamic_cast<T*>(o);
	if (o) assert(t);
	const_cast<P<T>& >(ptr) = P<T>(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Partial specialization of read_pointer for 
	reference-counted pointers.
 */
template <class T>
void
persistent_object_manager::read_pointer(
		istream& f, const count_ptr<T>& ptr) const {
	long i;
	read_value(f, i);
	object* o(lookup_obj_ptr(i));
	if (o) {
		T* t = dynamic_cast<T*>(o);
#if 0
		if (!t) {
			// to catch type mismatch errors!
			// if the below assert(t) should fail
			o->what(cerr << "object* o = " << o << " ") << endl;
		}
#endif
		size_t* c = lookup_ref_count(i);
			// will allocate if NULL
		assert(t);
		assert(c);
		// uses the unsafe constructor
		const_cast<count_ptr<T>& >(ptr) = count_ptr<T>(t, c);
	} else {
		const_cast<count_ptr<T>& >(ptr) = count_ptr<T>(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Partial specialization of read_pointer for 
	reference-counted pointers.
 */
template <class T>
void
persistent_object_manager::read_pointer(
		istream& f, const count_const_ptr<T>& ptr) const {
	long i;
	read_value(f, i);
	object* o(lookup_obj_ptr(i));
	if (o) {
		T* t = dynamic_cast<T*>(o);
#if 0
		if (!t) {
			// to catch type mismatch errors!
			// if the below assert(t) should fail
			o->what(cerr << "object* o = " << o << " ") << endl;
		}
#endif
		size_t* c = lookup_ref_count(i);
			// will allocate if NULL
		assert(t);
		assert(c);
		// uses the unsafe constructor
		const_cast<count_const_ptr<T>& >(ptr) =
			count_const_ptr<T>(t, c);
	} else {
		const_cast<count_const_ptr<T>& >(ptr) =
			count_const_ptr<T>(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Writes a sequence of pointers, mapped to indices.
	Container only needs a simple forward iterator interface.  
 */
template <template <class> class L, template <class> class P, class T >
void
persistent_object_manager::write_pointer_list(
		ostream& f, const L<P<T> >& l) const {
	// write number of elements to expect first
	size_t s = l.size();
	write_value(f, s);
	typename L<P<T> >::const_iterator iter = l.begin();
	const typename L<P<T> >::const_iterator end = l.end();
	for ( ; iter!=end; iter++)
		write_pointer(f, *iter);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reconstructs a sequence of pointers, mapped to indices.  
	Container only needs a simple forward iterator interface.  
 */
template <template <class> class L, template <class> class P, class T >
void
persistent_object_manager::read_pointer_list(istream& f, L<P<T> >& l) const {
	size_t s = 0;
	read_value(f, s);
	size_t i = 0;
	for ( ; i<s; i++) {
		P<T> ptr;
		read_pointer(f, ptr);
		l.push_back(ptr);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Writes a map of pointers without their keys.  
 */
template <template <class, class> class M, class K, 
		template <class> class P, class T >
void
persistent_object_manager::write_pointer_map(
		ostream& f, const M<K, P<T> >& m) const {
	size_t s = m.size();
	write_value(f, s);
	typename M<K, P<T> >::const_iterator iter = m.begin();
	const typename M<K, P<T> >::const_iterator end = m.end();
	for ( ; iter!=end; iter++)
		write_pointer(f, iter->second);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// specialized list methods?

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	//	__ART_OBJECT_IO_TCC__

