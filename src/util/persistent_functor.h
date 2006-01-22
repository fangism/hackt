/**
	\file "util/persistent_functor.h"
	$Id: persistent_functor.h,v 1.1.4.2 2006/01/22 05:12:50 fang Exp $
 */

#ifndef	__UTIL_PERSISTENT_FUNCTOR_H__
#define	__UTIL_PERSISTENT_FUNCTOR_H__

#include <iosfwd>

namespace util {
// forward declaration is all that is needed
class persistent_object_manager;

//=============================================================================
struct persistent_visitor_base {
	persistent_object_manager&		pom;

	explicit
	persistent_visitor_base(persistent_object_manager& m) :
		pom(m) { }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct persistent_const_visitor_base {
	const persistent_object_manager&	pom;

	explicit
	persistent_const_visitor_base(const persistent_object_manager& m) :
		pom(m) { }
};

//=============================================================================
/**
	\param T is any reference to an object with method 
		collect_transient_info_base()
	TODO: default to member-function, allow overrideable.  
 */
template <class T>
struct persistent_collector_ref : public persistent_visitor_base {
	explicit
	persistent_collector_ref(persistent_object_manager& m)
		: persistent_visitor_base(m) { }

	void
	operator () (const T& t) {
		t.collect_transient_info_base(pom);
	}
};

//-----------------------------------------------------------------------------
/**
	\param T is any pointer or pointer class.  
		The pointee has method collect_transient_info()
		and is derived from util::persistent.  
	TODO: default to member-function, allow overrideable.  
 */
template <class T>
struct persistent_collector_ptr : public persistent_visitor_base {
	explicit
	persistent_collector_ptr(persistent_object_manager& m)
		: persistent_visitor_base(m) { }

	void
	operator () (const T& t) {
		if (t) t->collect_transient_info(pom);
	}
};

//=============================================================================
template <class T>
struct persistent_writer_ref : public persistent_const_visitor_base {
	std::ostream&			os;

	persistent_writer_ref(const persistent_object_manager& m, 
		std::ostream& o)
		: persistent_const_visitor_base(m), os(o) { }

	void
	operator () (const T& t) {
		t.write_object(pom, os);
	}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Not really needed since persistent_object_manager already
	has write_pointer_list template member function.  
 */
template <class T>
struct persistent_writer_ptr : public persistent_const_visitor_base {
	std::ostream&			os;

	persistent_writer_ptr(const persistent_object_manager& m, 
		std::ostream& o)
		: persistent_const_visitor_base(m), os(o) { }

	void
	operator () (const T& t) {
		pom.write_pointer(os, t);
	}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
struct persistent_loader_ref : public persistent_const_visitor_base {
	std::istream&			is;

	persistent_loader_ref(const persistent_object_manager& m, 
		std::istream& i)
		: persistent_const_visitor_base(m), is(i) { }

	void
	operator () (T& t) {
		t.load_object(pom, is);
	}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Not really needed since persistent_object_manager already
	has read_pointer_list template member function.  
 */
template <class T>
struct persistent_loader_ptr : public persistent_const_visitor_base {
	std::istream&			is;

	persistent_loader_ptr(const persistent_object_manager& m, 
		std::istream& i)
		: persistent_const_visitor_base(m), is(i) { }

	void
	operator () (T& t) {
		pom.read_pointer(is, t);
	}
};

//=============================================================================
/**
	TODO: decide whether these belong as template member functions
		of persistent_object_manager.  
	Like util/IO_utils's write_sequence, except that this also
	passes along a persistent_object_manager.
	\param T the sequence type.  Must support iteration.  
 */
template <class T>
void
write_persistent_sequence(const persistent_object_manager&, 
		std::ostream&, const T&);

template <class T>
void
write_persistent_array(const persistent_object_manager&, 
		std::ostream&, const T&);

template <class T>
void
read_persistent_sequence_in_place(const persistent_object_manager&, 
		std::istream&, T&);

#if 0
template <class S>
void
read_persistent_sequence_prealloc(const persistent_object_manager&, 
		std::istream&, S&);
#endif

template <class S>
void
read_persistent_sequence_resize(const persistent_object_manager&, 
		std::istream&, S&);

template <class S>
void
read_persistent_sequence_back_insert(const persistent_object_manager&, 
		std::istream&, S&);


//=============================================================================
}	// end namespace util

#endif	// __UTIL_PERSISTENT_FUNCTOR_H__

