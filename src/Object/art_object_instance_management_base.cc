/**
	\file "art_object_instance_management_base.cc"
	Method definitions for basic sequential instance management.  
 	$Id: art_object_instance_management_base.cc,v 1.4 2005/01/12 04:14:18 fang Exp $
 */

#include <iostream>
#include <algorithm>

#include "ptrs_functional.h"
#include "compose.h"
#include "binders.h"

#include "STL/list.tcc"
#include "art_object_instance_management_base.h"
#include "persistent_object_manager.tcc"
#include "stacktrace.h"

namespace ART {
namespace entity {
using std::mem_fun_ref;
using std::dereference;
using std::istream;
#include "using_ostream.h"
using namespace ADS;
using util::stacktrace;

//=============================================================================
// class sequential_scope method definitions

sequential_scope::sequential_scope() : instance_management_list() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
sequential_scope::~sequential_scope() { }

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
	instance_management_list.push_back(i);
	// accidental deallocation of i here?  not anymore
	NEVER_NULL(instance_management_list.back());
	INVARIANT(!i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This may be temporary.  
 */
void
sequential_scope::unroll(void) const {
	for_each(instance_management_list.begin(), 
		instance_management_list.end(), 
	unary_compose_void(
		mem_fun_ref(&instance_management_base::unroll), 
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
	for_each(instance_management_list.begin(), 
		instance_management_list.end(), 
	unary_compose_void(
		bind2nd_argval_void(mem_fun_ref(
			&instance_management_base::collect_transient_info), m), 
		dereference<sticky_ptr, const instance_management_base>()
		// const_dereference<excl_const_ptr, instance_management_base>()
	)
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
sequential_scope::collect_transient_info_base(
		persistent_object_manager& m) const {
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
sequential_scope::write_object_base(
		const persistent_object_manager& m, ostream& f) const {
	write_object_pointer_list(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inline
void
sequential_scope::load_object_pointer_list(
		const persistent_object_manager& m, istream& f) {
	m.read_pointer_list(f, instance_management_list);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
sequential_scope::load_object_base(
		const persistent_object_manager& m, istream& f) {
	load_object_pointer_list(m, f);
}


//=============================================================================
// class instance_management_base::dumper method definitions

instance_management_base::dumper::dumper(ostream& o) : os(o) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <template <class> class P>
ostream&
instance_management_base::dumper::operator () (
		const P<const instance_management_base>& i) const {
	return i->dump(os) << endl;
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

