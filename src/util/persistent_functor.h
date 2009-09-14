/**
	\file "util/persistent_functor.h"
	$Id: persistent_functor.h,v 1.6 2009/09/14 21:17:17 fang Exp $
 */

#ifndef	__UTIL_PERSISTENT_FUNCTOR_H__
#define	__UTIL_PERSISTENT_FUNCTOR_H__

#include <iosfwd>
#include "util/STL/algorithm_fwd.h"

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
struct persistent_collector_ref : public persistent_visitor_base {
	explicit
	persistent_collector_ref(persistent_object_manager& m)
		: persistent_visitor_base(m) { }

	template <class T>
	void
	operator () (const T& t) const {
		t.collect_transient_info_base(pom);
	}
};	// end struct persistent_collector_ref


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor for when the elements of a collection are, themselves, 
	containers, such as 2D containers.  
	This works for lists and vectors and sets, but not maps.
 */
struct persistent_sequence_collector_ref : public persistent_visitor_base {
	explicit
	persistent_sequence_collector_ref(persistent_object_manager& m)
		: persistent_visitor_base(m) { }

	template <class T>
	void
	operator () (const T& t) const {
		std::for_each(t.begin(), t.end(), 
			persistent_collector_ref(pom)
		);
	}

};	// end struct persistent_sequence_collector_ref

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Can override 
 */
template <class T>
struct persistent_collector : public persistent_visitor_base {
	typedef	void (T::*collector_func) (persistent_object_manager&) const;
	collector_func				collector;

	persistent_collector(const collector_func f, 
			persistent_object_manager& m) :
			persistent_visitor_base(m), collector(f) { }

	void
	operator () (const T& t) const {
		t.*collector(this->pom);
	}

};	// end struct persistent_collector

//-----------------------------------------------------------------------------
/**
	\param T is any pointer or pointer class.  
		The pointee has method collect_transient_info()
		and is derived from util::persistent.  
	TODO: default to member-function, allow overrideable.  
 */
struct persistent_collector_ptr : public persistent_visitor_base {
	explicit
	persistent_collector_ptr(persistent_object_manager& m)
		: persistent_visitor_base(m) { }

	template <class T>
	void
	operator () (const T& t) const {
		if (t) t->collect_transient_info(pom);
	}
};	// end struct persistent_collector_ptr

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor for when the elements of a collection are, themselves, 
	containers, such as 2D containers.  
	This works for lists and vectors and sets, but not maps.
 */
struct persistent_sequence_collector_ptr : public persistent_visitor_base {
	explicit
	persistent_sequence_collector_ptr(persistent_object_manager& m)
		: persistent_visitor_base(m) { }

	template <class T>
	void
	operator () (const T& t) const {
		std::for_each(t.begin(), t.end(), 
			persistent_collector_ptr(pom)
		);
	}

};	// end struct persistent_sequence_collector_ptr

//=============================================================================
struct persistent_writer_base : public persistent_const_visitor_base {
	std::ostream&			os;

	persistent_writer_base(const persistent_object_manager& m, 
		std::ostream& o)
		: persistent_const_visitor_base(m), os(o) { }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor with customizable member function.  
 */
template <class T>
struct persistent_writer : public persistent_writer_base {
	typedef	void (T::*writer_func) (
			const persistent_object_manager&, std::ostream&) const;
	writer_func				writer;

	persistent_writer(const writer_func f, 
		const persistent_object_manager& m, std::ostream& o)
		: persistent_writer_base(m, o), writer(f) { }

	void
	operator () (const T& t) const {
		(t.*writer)(this->pom, this->os);
	}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct persistent_writer_ref : public persistent_writer_base {

	persistent_writer_ref(const persistent_object_manager& m, 
		std::ostream& o)
		: persistent_writer_base(m, o) { }

	template <class T>
	void
	operator () (const T& t) const {
		t.write_object(this->pom, this->os);
	}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
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
	operator () (const T& t) const {
		pom.write_pointer(os, t);
	}
};
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct persistent_loader_base : public persistent_const_visitor_base {
	std::istream&			is;

	persistent_loader_base(const persistent_object_manager& m, 
		std::istream& i)
		: persistent_const_visitor_base(m), is(i) { }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Member-function customizable functor.  
 */
template <class T>
struct persistent_loader : public persistent_loader_base {
	typedef	void (T::*loader_func) (
			const persistent_object_manager&, std::istream&);
	loader_func				loader;

	persistent_loader(const loader_func f, 
		const persistent_object_manager& m, 
		std::istream& i)
		: persistent_loader_base(m, i), loader(f) { }

	void
	operator () (T& t) const {
		(t.*loader)(this->pom, this->is);
	}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct persistent_loader_ref : public persistent_loader_base {

	persistent_loader_ref(const persistent_object_manager& m, 
		std::istream& i)
		: persistent_loader_base(m, i) { }

	template <class T>
	void
	operator () (T& t) const {
		t.load_object(this->pom, this->is);
	}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
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
	operator () (T& t) const {
		pom.read_pointer(is, t);
	}
};
#endif

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


template <class S>
void
read_persistent_set_insert(const persistent_object_manager&, 
		std::istream&, S&);

//=============================================================================
}	// end namespace util

#endif	// __UTIL_PERSISTENT_FUNCTOR_H__

