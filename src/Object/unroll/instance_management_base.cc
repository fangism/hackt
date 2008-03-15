/**
	\file "Object/unroll/instance_management_base.cc"
	Method definitions for basic sequential instance management.  
	This file was moved from "Object/art_object_instance_management_base.cc"
 	$Id: instance_management_base.cc,v 1.17.2.1 2008/03/15 03:33:53 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_INSTANCE_MANAGEMENT_BASE_CC__
#define	__HAC_OBJECT_UNROLL_INSTANCE_MANAGEMENT_BASE_CC__

// compilation switches for debugging
#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_DESTRUCTORS		(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_PERSISTENTS		(0 && ENABLE_STACKTRACE)

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <iostream>
#include <algorithm>
#include <list>

#include "Object/unroll/instance_management_base.h"
#include "Object/unroll/sequential_scope.h"

#include "util/memory/count_ptr.tcc"
#include "util/dereference.h"
#include "util/compose.h"
#include "util/binders.h"
#include "util/static_assert.h"
#include "util/type_traits.h"
#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.h"
#include "util/indent.h"


namespace HAC {
namespace entity {
using util::is_same;
using util::dereference;
using util::auto_indent;
using std::istream;
using std::for_each;
#include "util/using_ostream.h"
USING_UTIL_COMPOSE

//=============================================================================
// class instance_management_base method definitions

//=============================================================================
// class sequential_scope method definitions

sequential_scope::sequential_scope() : 
		parent_type()
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
sequential_scope::~sequential_scope() {
	STACKTRACE_DTOR("~sequential_scope()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
sequential_scope::dump(ostream& o, const expr_dump_context& dc) const {
	for_each(begin(), end(), instance_management_base::dumper(o, dc));
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
void
sequential_scope::append_instance_management(
		const count_ptr<const instance_management_base>& i) {
	push_back(i);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This may be temporary.  
 */
good_bool
sequential_scope::unroll(const unroll_context& c) const {
	STACKTRACE("sequential_scope::unroll()");
#if 0
	for_each(instance_management_list.begin(), 
		instance_management_list.end(), 
	unary_compose_void(
		bind2nd_argval_void(
			mem_fun_ref(&instance_management_base::unroll), c), 
		dereference<sticky_ptr<const instance_management_base> >()
	)
	);
#else
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		if (!(*i)->unroll(c).good) {
			return good_bool(false);
		}
	}
#endif
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Predicated unroll.  
 */
good_bool
sequential_scope::unroll_if(const unroll_context& c, 
		bool (*pred)(const instance_management_base*)) const {
	STACKTRACE("sequential_scope::unroll()");
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		if (*i && (*pred)(&**i)) {
			if (!(*i)->unroll(c).good) {
				return good_bool(false);
			}
		}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
sequential_scope::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT(
		"sequential_scope::collect_transient_info_base()");
	m.collect_pointer_list(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
sequential_scope::write_object_base_fake(
		const persistent_object_manager& m, ostream& f) {
	static const instance_management_list_type dummy;
	STACKTRACE_PERSISTENT("sequential_scope::write_object_base_fake()");
	m.write_pointer_list(f, dummy);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
sequential_scope::write_object_base(
		const persistent_object_manager& m, ostream& f) const {
	STACKTRACE_PERSISTENT("sequential_scope::write_object_base()");
	m.write_pointer_list(f, *this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
sequential_scope::load_object_base(
		const persistent_object_manager& m, istream& f) {
	STACKTRACE_PERSISTENT("sequential_scope::load_object_base()");
	m.read_pointer_list(f, *this);
}

//=============================================================================
// class instance_management_base::dumper method definitions

instance_management_base::dumper::dumper(ostream& o, 
		const expr_dump_context& dc) : os(o), edc(dc) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param P pointer class template for the instance management object.
	\param i pointer to the instance management object to dump.
 */
template <class P>
ostream&
instance_management_base::dumper::operator () (const P& i) const {
	typedef	is_same<typename P::element_type,
			instance_management_list_type::value_type::element_type>
					__type_constraint1;
	UTIL_STATIC_ASSERT_DEPENDENT(__type_constraint1::value);
	return i->dump(os << auto_indent, edc) << endl;
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

// cleaning up macros used for this module
#undef	ENABLE_STACKTRACE
#undef	STACKTRACE_DESTRUCTORS
#undef	STACKTRACE_DTOR
#undef	STACKTRACE_PERSISTENTS
#undef	STACKTRACE_PERSISTENT

DEFAULT_STATIC_TRACE_END

#endif	// __HAC_OBJECT_UNROLL_INSTANCE_MANAGEMENT_BASE_CC__

