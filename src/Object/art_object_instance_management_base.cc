/**
	\file "art_object_instance_management_base.cc"
	Method definitions for basic sequential instance management.  
 	$Id: art_object_instance_management_base.cc,v 1.1 2004/12/06 07:11:20 fang Exp $
 */

#include <iostream>
#include <algorithm>

#include "ptrs_functional.h"
#include "compose.h"
#include "binders.h"

#include "STL/list.tcc"
#include "art_object_instance_management_base.h"
#include "persistent_object_manager.tcc"

namespace ART {
namespace entity {
using std::mem_fun_ref;
using std::dereference;
using std::istream;
#include "using_ostream.h"
using namespace ADS;

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
		excl_ptr<const instance_management_base> i) {
	instance_management_list.push_back(i);
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
sequential_scope::write_object_pointer_list(
		const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(IS_A(const persistent*, this));
	assert(f.good());
	m.write_pointer_list(f, instance_management_list);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
sequential_scope::load_object_pointer_list(const persistent_object_manager& m) {
	istream& f = m.lookup_read_buffer(IS_A(const persistent*, this));
	assert(f.good());
	m.read_pointer_list(f, instance_management_list);
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

