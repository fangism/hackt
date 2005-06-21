/**
	\file "util/persistent_object_manager.tcc"
	Template methods for persistent_object_manager class.
	$Id: persistent_object_manager.tcc,v 1.18.2.1 2005/06/21 01:08:25 fang Exp $
 */

#ifndef	__UTIL_PERSISTENT_OBJECT_MANAGER_TCC__
#define	__UTIL_PERSISTENT_OBJECT_MANAGER_TCC__

// #define	ENABLE_STACKTRACE		1
// depend on whatever file includes this

#include "util/persistent_object_manager.h"

#ifndef	EXTERN_TEMPLATE_UTIL_PERSISTENT_OBJECT_MANAGER

#include <fstream>
#include <iostream>

#include "util/macros.h"
#include "util/stacktrace.h"

#ifdef	EXCLUDE_DEPENDENT_TEMPLATES_UTIL_PERSISTENT_OBJECT_MANAGER
#define	EXTERN_TEMPLATE_UTIL_NEW_FUNCTOR
#define	EXTERN_TEMPLATE_UTIL_IO_UTILS
#define	EXTERN_TEMPLATE_UTIL_WHAT
#endif

#include "util/new_functor.tcc"
#include "util/IO_utils.tcc"
#include "util/what.tcc"

#if ENABLE_STACKTRACE
#include "util/sstream.h"
#endif


#define	WELCOME_TO_TYPE_REGISTRATION			0

namespace util {
//=============================================================================
#include "util/using_ostream.h"
USING_STACKTRACE
using namespace util::memory;
using util::what;
#if ENABLE_STACKTRACE
using std::ostringstream;
#endif

//=============================================================================
// class persistent_object_manager template method definitions

/**
	The default means of registering a persistent type statically.  
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
	typedef	persistent_traits<T>	traits_type;
	reconstruction_function_map_type& m = reconstruction_function_map();
	const persistent::hash_key& type_key = traits_type::type_key;
	const aux_alloc_arg_type sub_index = traits_type::sub_index;
	const reconstruct_function_ptr_type
		cf = &traits_type::empty_constructor;
	return register_persistent_type<T>(sub_index, cf);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Manual override for persistent type registration, 
	where one may pass in any custom construction functor.  
 */
template <class T>
int
persistent_object_manager::register_persistent_type(
		const aux_alloc_arg_type a, 
		const reconstruct_function_ptr_type cf) {
	typedef	persistent_traits<T>	traits_type;
	NEVER_NULL(cf);
	reconstruction_function_map_type& m = reconstruction_function_map();
	const persistent::hash_key& type_key = traits_type::type_key;
	const size_t sub_index = a;
	INVARIANT(type_key != persistent::hash_key::null);
	reconstructor_vector_type& ctor_vec = m[type_key];
		// will create an empty vec if not already there
	if (sub_index >= ctor_vec.size())
		ctor_vec.resize(sub_index+1, NULL);
		// automatically resize if necessary
#if WELCOME_TO_TYPE_REGISTRATION
	cerr << "Welcome to persistent type registration, "
		<< type_key << '[' << sub_index << "]!  ";
#endif
	const reconstruct_function_ptr_type probe = ctor_vec[sub_index];
	if (probe) {
		cerr << "FATAL: Persistent type with key \"" <<
			type_key << "\" and sub-index " << sub_index <<
			" already taken!" << endl;
		THROW_EXIT;
	} else {
		ctor_vec[sub_index] = cf;
	}
	// get a unique id
	const int s = registered_type_sequence_number();
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
		const P& ptr, const raw_pointer_tag) const {
	// P is a bare pointer type (T*), according to the raw_pointer_tag
	if (ptr) {
		const persistent* t = IS_A(const persistent*, ptr);
		// really should write a concept check
		if (!t) {
			cerr << "Pointer @ " << ptr << " of type " <<
				util::what<typename util::memory::
					pointer_traits<P>::element_type
				>::name() << " is not derived from "
				"util::persistent, and thus is unusable "
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
		const P& ptr, const pointer_class_base_tag) const {
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
persistent_object_manager::visit_info*
persistent_object_manager::__read_pointer(istream& f, 
		const P& ptr, const raw_pointer_tag) const {
	typedef	persistent_object_manager::visit_info*	return_type;
	typedef typename pointer_traits<P>::pointer	pointer_type;
	unsigned long i;
	read_value(f, i);
	INVARIANT(check_reconstruction_table_range(i));
#if 0
	persistent* o(lookup_obj_ptr(i));
#else
	const std::pair<persistent*, visit_info*> pv(lookup_ptr_visit_info(i));
	persistent* const o(pv.first);
#endif
	// for this to work, pointer_type must be a raw_pointer
	const_cast<P&>(ptr) = dynamic_cast<pointer_type>(o);
	if (o) NEVER_NULL(ptr);
#if 1
	return pv.second;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class P>
inline
persistent_object_manager::visit_info*
persistent_object_manager::__read_pointer(istream& f, 
		const P& ptr, const single_owner_pointer_tag) const {
#if ENABLE_STACKTRACE
	static ostringstream oss;
	static const ostream& oss_ref = 
		oss << "__read_pointer-single<" <<
			what<typename P::element_type>::name() << ">()";
	STACKTRACE(oss.str());
#endif
	const typename pointer_traits<P>::pointer&
		p = pointer_manipulator::get_pointer(ptr);
	return __read_pointer(f, p, __pointer_category(p));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reads a pointer in reference-count mode.  
 */
template <class P>
inline
persistent_object_manager::visit_info*
persistent_object_manager::__read_pointer(istream& f, 
		const P& ptr, const shared_owner_pointer_tag) const {
	typedef	persistent_object_manager::visit_info*	return_type;
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
	const return_type ret = __read_pointer(f, p, __pointer_category(p));
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
	return ret;
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

	Marking and classifying visits is the manager's way of
	knowing which addresses need to be deleted upon
	termination of the reconstruction table.  
 */
template <class P>
void
persistent_object_manager::read_pointer(istream& f, const P& ptr) const {
	STACKTRACE("pom::read_pointer()");
	visit_info* v = __read_pointer(f, ptr, __pointer_category(ptr));
	NEVER_NULL(v);
	// really the value of the pointer is irrelevant, just the ownership
	v->mark_visit(ptr);
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
	Visits a sequence of pointers, calls registration.  
	Container only needs a simple forward iterator interface.  
 */
template <class L>
void
persistent_object_manager::collect_pointer_list(const L& l) {
	// concept requirements:
	// L is a sequence-type container, has begin and end
	// L must have public const_iterator type
	// pointer_traits<L::value_type> is raw_pointer or pointer class
	typedef	typename L::const_iterator	const_iterator;
	const_iterator i = l.begin();
	const const_iterator e = l.end();
	for ( ; i!=e; i++) {
		if (*i)
			(*i)->collect_transient_info(*this);
		// else skip
	}
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
/**
	\param P is generic pointer to a persistent type.  
 */
template <class P>
void
persistent_object_manager::load_object_once(const P& p) const {
	__load_object_once(p, __pointer_category(p));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param P is generic pointer to a persistent type.  
 */
template <class P>
void
persistent_object_manager::__load_object_once(const P& p, 
		pointer_class_base_tag) const {
	NEVER_NULL(p);
	__load_object_once(&*p, __pointer_category(&*p));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Wrapper interface to collecting transient objects.
	\param P pointer or pointer-class type.
	\param p pointer to persistent object, allowed to be NULL.  
 */
template <class P>
void
persistent_object_manager::collect_transient_object(const P& p) {
	if (p) {
		// returns a pointer to the visit_info sub-object
		// if this was first visit.  
		visit_info* v = p->register_transient_object(m);
		if (v) {
			v->mark_visit(p);
		}
	}
}
#endif

//=============================================================================
// class persistent_object_manager::visit_info method definitions

template <class P>
void
persistent_object_manager::visit_info::mark_visit(const P& p) {
	if (p) {
		__mark_visit(p, __pointer_category(p));
		total_visits++;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Raw-pointers are classified as un-owned visits.
 */
template <class P>
void
persistent_object_manager::visit_info::__mark_visit(const P*, 
		const raw_pointer_tag) {
	raw_visits++;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class P>
void
persistent_object_manager::visit_info::__mark_visit(const P&, 
		const never_owner_pointer_tag) {
	unowned_visits++;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Here the pointer is needed to determine dynamic ownership.
 */
template <class P>
void
persistent_object_manager::visit_info::__mark_visit(const P& p, 
		const sometimes_owner_pointer_tag) {
	if (p.owned())
		owned_visits++;
	else	unowned_visits++;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class P>
void
persistent_object_manager::visit_info::__mark_visit(const P&, 
		const exclusive_owner_pointer_tag) {
	owned_visits++;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class P>
void
persistent_object_manager::visit_info::__mark_visit(const P&, 
		const shared_owner_pointer_tag) {
	shared_visits++;
}

//=============================================================================
}	// end namespace util

#endif	// EXTERN_TEMPLATE_UTIL_PERSISTENT_OBJECT_MANAGER
#endif	//	__UTIL_PERSISTENT_OBJECT_MANAGER_TCC__

