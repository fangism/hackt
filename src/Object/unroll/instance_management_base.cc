/**
	\file "Object/unroll/instance_management_base.cc"
	Method definitions for basic sequential instance management.  
	This file was moved from "Object/art_object_instance_management_base.cc"
 	$Id: instance_management_base.cc,v 1.14 2006/04/18 18:42:41 fang Exp $
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

sequential_scope::sequential_scope() : instance_management_list() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
sequential_scope::~sequential_scope() {
	STACKTRACE_DTOR("~sequential_scope()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
sequential_scope::dump(ostream& o, const expr_dump_context& dc) const {
	for_each(instance_management_list.begin(), 
		instance_management_list.end(), 
		instance_management_base::dumper(o, dc)
	);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
sequential_scope::append_instance_management(
		excl_ptr<const instance_management_base>& i) {
	STACKTRACE("sequential_scope::append_instance_management()");
	NEVER_NULL(i);
	// PROBLEM ownership isn't being trasnfered:
	// push_back -> insert -> _M_create_node -> std::_Construct( , );
	// std::_Construct takes (T1*, const T2&) arguments
	// _M_create_node takes only const value_type& as argument, 
	// thus preventing transfer...
	// we need a mechanism for explicit transfer, see excl_ptr_ref
	// SOLUTION: we specialized std::_Construct for sticky_ptr's
	// see "util/memory/pointer_classes.h"

#if 0
	instance_management_list.push_back(i);
#else
	// explicitly take ownership (needed for gcc-3.4.0?, but not 3.3?)
	// awkward...
	static excl_ptr<const instance_management_base> null(NULL);
	instance_management_list.push_back(null);
	instance_management_list.back() = i;
#endif

	// accidental deallocation of i here?  not anymore
	NEVER_NULL(instance_management_list.back());
	INVARIANT(!i);
}

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
	const_iterator i(instance_management_list.begin());
	const const_iterator e(instance_management_list.end());
	for ( ; i!=e; i++) {
		if (!(*i)->unroll(c).good) {
			return good_bool(false);
		}
	}
#endif
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
sequential_scope::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT(
		"sequential_scope::collect_transient_info_base()");
//	collect_object_pointer_list(m);
	m.collect_pointer_list(instance_management_list);
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
//	write_object_pointer_list(m, f);
	m.write_pointer_list(f, instance_management_list);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
sequential_scope::load_object_base(
		const persistent_object_manager& m, istream& f) {
	STACKTRACE_PERSISTENT("sequential_scope::load_object_base()");
//	load_object_pointer_list(m, f);
	m.read_pointer_list(f, instance_management_list);
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
			const instance_management_base>
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

