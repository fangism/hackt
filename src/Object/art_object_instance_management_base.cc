/**
	\file "art_object_instance_management_base.cc"
	Method definitions for basic sequential instance management.  
 	$Id: art_object_instance_management_base.cc,v 1.8.8.3 2005/02/03 01:29:24 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_MANAGEMENT_BASE_CC__
#define	__ART_OBJECT_INSTANCE_MANAGEMENT_BASE_CC__

// compilation switches for debugging
#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_DESTRUCTORS		0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS		0 && ENABLE_STACKTRACE

#include <iostream>
#include <algorithm>

#include "ptrs_functional.h"
#include "compose.h"
#include "binders.h"

#include "STL/list.tcc"
#include "art_object_instance_management_base.h"
#include "persistent_object_manager.tcc"
#include "stacktrace.h"

// conditional defines, after including "stactrace.h"
#if STACKTRACE_DESTRUCTORS
	#define	STACKTRACE_DTOR(x)		STACKTRACE(x)
#else
	#define	STACKTRACE_DTOR(x)
#endif

#if STACKTRACE_PERSISTENTS
	#define	STACKTRACE_PERSISTENT(x)	STACKTRACE(x)
#else
	#define	STACKTRACE_PERSISTENT(x)
#endif


namespace ART {
namespace entity {
using std::mem_fun_ref;
using std::dereference;
using std::istream;
#include "using_ostream.h"
USING_UTIL_COMPOSE
USING_STACKTRACE

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
sequential_scope::dump(ostream& o) const {
	for_each(instance_management_list.begin(), 
		instance_management_list.end(), 
		instance_management_base::dumper(o)
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
void
sequential_scope::unroll(unroll_context& c) const {
	STACKTRACE("sequential_scope::unroll()");
	for_each(instance_management_list.begin(), 
		instance_management_list.end(), 
	unary_compose_void(
		bind2nd_argval_void(
			mem_fun_ref(&instance_management_base::unroll), c), 
		dereference<sticky_ptr, const instance_management_base>()
		// const_dereference<excl_const_ptr, instance_management_base>()
	)
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inline
void
sequential_scope::collect_object_pointer_list(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT(
		"sequential_scope::collect_object_pointer_list()");
#if 0
	// for debugging purposes...
	instance_management_list_type::const_iterator
		i = instance_management_list.begin();
	const instance_management_list_type::const_iterator
		e = instance_management_list.end();
	for ( ; i!=e; i++) {
#if 0
		STACKTRACE_PERSISTENT("for all instance_management_list:");
		NEVER_NULL(*i);
		(*i)->what(cerr << "at " << &**i << ", ") << endl;
#endif
		(*i)->collect_transient_info(m);
	}
#else
	for_each(instance_management_list.begin(), 
		instance_management_list.end(), 
	unary_compose_void(
		bind2nd_argval_void(mem_fun_ref(
			&instance_management_base::collect_transient_info), m), 
		dereference<sticky_ptr, const instance_management_base>()
		// const_dereference<excl_const_ptr, instance_management_base>()
	)
	);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
sequential_scope::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT(
		"sequential_scope::collect_transient_info_base()");
	collect_object_pointer_list(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inline
void
sequential_scope::write_object_pointer_list(
		const persistent_object_manager& m, ostream& f) const {
	m.write_pointer_list(f, instance_management_list);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
sequential_scope::write_object_base_fake(
		const persistent_object_manager& m, ostream& f) {
	static const instance_management_list_type dummy;
	m.write_pointer_list(f, dummy);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
sequential_scope::write_object_base(
		const persistent_object_manager& m, ostream& f) const {
	write_object_pointer_list(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inline
void
sequential_scope::load_object_pointer_list(
		const persistent_object_manager& m, istream& f) {
	STACKTRACE_PERSISTENT("sequential_scope::load_object_pointer_list()");
	m.read_pointer_list(f, instance_management_list);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
sequential_scope::load_object_base(
		const persistent_object_manager& m, istream& f) {
	STACKTRACE_PERSISTENT("sequential_scope::load_object_base()");
	load_object_pointer_list(m, f);
}

//=============================================================================
// class instance_management_base::dumper method definitions

instance_management_base::dumper::dumper(ostream& o) : os(o) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param P pointer class template for the instance management object.
	\param i pointer to the instance management object to dump.
 */
template <template <class> class P>
ostream&
instance_management_base::dumper::operator () (
		const P<const instance_management_base>& i) const {
	return i->dump(os) << endl;
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

// cleaning up macros used for this module
#undef	ENABLE_STACKTRACE
#undef	STACKTRACE_DESTRUCTORS
#undef	STACKTRACE_DTOR
#undef	STACKTRACE_PERSISTENTS
#undef	STACKTRACE_PERSISTENT

#endif	// __ART_OBJECT_INSTANCE_MANAGEMENT_BASE_CC__

