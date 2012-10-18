/**
	\file "util/persistent_functor.hh"
	$Id: persistent_functor.hh,v 1.7 2010/04/02 22:19:22 fang Exp $
 */

#ifndef	__UTIL_PERSISTENT_FUNCTOR_H__
#define	__UTIL_PERSISTENT_FUNCTOR_H__

#include <iosfwd>
#include "util/STL/algorithm_fwd.hh"

namespace util {
// forward declaration is all that is needed
class persistent_object_manager;

//=============================================================================
/**
	When persistent object manager is something different.
 */
template <class T>
struct foreign_persistent_visitor_base {
	T&					pom;
	explicit
	foreign_persistent_visitor_base(T& t) : pom(t) { }
};	// end struct foreign_persistent_visitor_base

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	foreign_persistent_visitor_base<persistent_object_manager>
					persistent_visitor_base;
typedef	foreign_persistent_visitor_base<const persistent_object_manager>
					persistent_const_visitor_base;

//=============================================================================
/**
	\param T is any reference to an object with method 
		collect_transient_info_base()
	TODO: default to member-function, allow overrideable.  
 */
template <class P>
struct foreign_persistent_collector_ref :
		public foreign_persistent_visitor_base<P> {
	explicit
	foreign_persistent_collector_ref(P& m)
		: foreign_persistent_visitor_base<P>(m) { }

	template <class T>
	void
	operator () (const T& t) const {
		t.collect_transient_info_base(this->pom);
	}
};	// end struct foreign_persistent_collector_ref

typedef	foreign_persistent_collector_ref<persistent_object_manager>
					persistent_collector_ref;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor for when the elements of a collection are, themselves, 
	containers, such as 2D containers.  
	This works for lists and vectors and sets, but not maps.
 */
template <class P>
struct foreign_persistent_sequence_collector_ref :
		public foreign_persistent_visitor_base<P> {
	explicit
	foreign_persistent_sequence_collector_ref(P& m)
		: foreign_persistent_visitor_base<P>(m) { }

	template <class T>
	void
	operator () (const T& t) const {
		std::for_each(t.begin(), t.end(), 
			persistent_collector_ref(this->pom)
		);
	}

};	// end struct foreign_persistent_sequence_collector_ref

typedef	foreign_persistent_sequence_collector_ref<persistent_object_manager>
					persistent_sequence_collector_ref;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Can override the member function used to collect.
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
template <class P>
struct foreign_persistent_collector_ptr :
		public foreign_persistent_visitor_base<P> {
	explicit
	foreign_persistent_collector_ptr(P& m)
		: foreign_persistent_visitor_base<P>(m) { }

	template <class T>
	void
	operator () (const T& t) const {
		if (t) t->collect_transient_info(this->pom);
	}
};	// end struct persistent_collector_ptr

typedef	foreign_persistent_collector_ptr<persistent_object_manager>
					persistent_collector_ptr;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor for when the elements of a collection are, themselves, 
	containers, such as 2D containers.  
	This works for lists and vectors and sets, but not maps.
 */
template <class P>
struct foreign_persistent_sequence_collector_ptr :
		public foreign_persistent_visitor_base<P> {
	explicit
	foreign_persistent_sequence_collector_ptr(P& m)
		: foreign_persistent_visitor_base<P>(m) { }

	template <class T>
	void
	operator () (const T& t) const {
		std::for_each(t.begin(), t.end(), 
			foreign_persistent_collector_ptr<P>(this->pom)
		);
	}

};	// end struct persistent_sequence_collector_ptr

//=============================================================================
template <class P>
struct foreign_persistent_writer_base :
		public foreign_persistent_visitor_base<const P> {
	std::ostream&			os;

	foreign_persistent_writer_base(const P& m, std::ostream& o)
		: foreign_persistent_visitor_base<const P>(m), os(o) { }
};

typedef	foreign_persistent_writer_base<persistent_object_manager>
					persistent_writer_base;

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
template <class P>
struct foreign_persistent_writer_ref :
		public foreign_persistent_writer_base<P> {

	foreign_persistent_writer_ref(const P& m, std::ostream& o)
		: foreign_persistent_writer_base<P>(m, o) { }

	template <class T>
	void
	operator () (const T& t) const {
		t.write_object(this->pom, this->os);
	}
};

typedef	foreign_persistent_writer_ref<persistent_object_manager>
					persistent_writer_ref;

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
template <class P>
struct foreign_persistent_loader_base :
		public foreign_persistent_visitor_base<const P> {
	std::istream&			is;

	foreign_persistent_loader_base(const P& m, std::istream& i)
		: foreign_persistent_visitor_base<const P>(m), is(i) { }
};

typedef	foreign_persistent_loader_base<persistent_object_manager>
					persistent_loader_base;

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
template <class P>
struct foreign_persistent_loader_ref :
		public foreign_persistent_loader_base<const P> {

	foreign_persistent_loader_ref(const P& m, std::istream& i)
		: foreign_persistent_loader_base<const P>(m, i) { }

	template <class T>
	void
	operator () (T& t) const {
		t.load_object(this->pom, this->is);
	}
};

typedef	foreign_persistent_loader_ref<persistent_object_manager>
					persistent_loader_ref;

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
	\param P persistent_object_manager type
	\param T the sequence type.  Must support iteration.  
 */
template <class P, class T>
void
write_persistent_sequence(const P&, std::ostream&, const T&);

template <class P, class T>
void
write_persistent_array(const P&, std::ostream&, const T&);

template <class P, class T>
void
read_persistent_sequence_in_place(const P&, std::istream&, T&);

#if 0
template <class S>
void
read_persistent_sequence_prealloc(const persistent_object_manager&, 
		std::istream&, S&);
#endif

template <class P, class S>
void
read_persistent_sequence_resize(const P&, std::istream&, S&);

template <class P, class S>
void
read_persistent_sequence_back_insert(const P&, std::istream&, S&);


template <class P, class S>
void
read_persistent_set_insert(const P&, std::istream&, S&);

//=============================================================================
}	// end namespace util

#endif	// __UTIL_PERSISTENT_FUNCTOR_H__

