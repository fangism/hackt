/**
	\file "art_object_connect.tcc"
	Method definitions pertaining to connections and assignments.  
 	$Id: art_object_connect.tcc,v 1.1.2.2 2005/02/24 02:26:48 fang Exp $
 */

#ifndef	__ART_OBJECT_CONNECT_TCC__
#define	__ART_OBJECT_CONNECT_TCC__

// compilation switches for debugging
#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_DESTRUCTORS		0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS		0 && ENABLE_STACKTRACE
#endif

#include <iostream>
#include <vector>

#include "art_object_connect.h"

#include "persistent_object_manager.h"
#include "STL/list.h"
#include "stacktrace.h"
#include "what.h"
#include "binders.h"
#include "compose.h"
#include "dereference.h"

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


//=============================================================================
namespace ART {
namespace entity {
USING_IO_UTILS
using std::vector;
using util::persistent_traits;
#include "using_ostream.h"
using std::mem_fun_ref;
using util::dereference;
USING_UTIL_COMPOSE
USING_STACKTRACE

//=============================================================================
// class alias_connection method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
ALIAS_CONNECTION_CLASS::alias_connection() :
		parent_type(), inst_list() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
ALIAS_CONNECTION_CLASS::~alias_connection() {
	STACKTRACE_DTOR("~alias_connection<>()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
ostream&
ALIAS_CONNECTION_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
ostream&
ALIAS_CONNECTION_CLASS::dump(ostream& o) const {
	INVARIANT(inst_list.size() > 1);
	const_iterator iter = inst_list.begin();
	const const_iterator end = inst_list.end();
	NEVER_NULL(*iter);
	(*iter)->dump(o);
	for (iter++; iter!=end; iter++) {
		NEVER_NULL(*iter);
		(*iter)->dump(o << " = ");
	}
	return o << ';';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Initializes an instance reference connection with the
	first instance reference.  
	\param i instance reference to connect, may not be NULL.
 */
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
void
ALIAS_CONNECTION_CLASS::append_instance_reference(
		const generic_inst_ptr_type& i) {
	NEVER_NULL(i);
	// need dynamic cast
	const inst_ref_ptr_type
		irp(i.template is_a<const instance_reference_type>());
		// gcc-3.3 slightly crippled, needs template keyword :(
	NEVER_NULL(irp);
	inst_list.push_back(irp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Connects the referenced instance aliases.  
 */
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
void
ALIAS_CONNECTION_CLASS::unroll(unroll_context& c) const {
	typedef	vector<alias_collection_type>	alias_collection_array_type;
	what(cerr << "Fang, finish ") << "::unroll()!" << endl;
//	Create a vector of alias_collection_type (packed_array_generic)
	alias_collection_array_type ref_array(inst_list.size());
	const_iterator iter = inst_list.begin();
	const const_iterator end = inst_list.end();
	typename alias_collection_array_type::iterator
		ref_iter = ref_array.begin();
	bool err = false;
	for ( ; iter != end; iter++, ref_iter++) {
		NEVER_NULL(*iter);
		if ((*iter)->unroll_references(c, *ref_iter))
			err = true;
	}
	if (err) {
		what(cerr << "ERROR: unrolling instance references in ") <<
			"::unroll()." << endl;
		return;
	}
/***
	Make sure each packed array has the same dimensions.  
	Type-check.
		Collectible vs. connectible!  Different semantics!
		Collectible !=> connectible!  Must check each reference!
	Use vector of iterators to walk?
	Fancy: cache type-checking...
***/
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
persistent*
ALIAS_CONNECTION_CLASS::construct_empty(const int) {
	return new this_type;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
void
ALIAS_CONNECTION_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	// improper key!!!
	STACKTRACE_PERSISTENT("alias_connection<>::collect_transients()");
//	cerr << persistent_traits<this_type>::type_key << endl;
#if 1
	const_iterator iter = inst_list.begin();
	const const_iterator end = inst_list.end();
	for ( ; iter!=end; iter++) {
		(*iter)->collect_transient_info(m);
	}
#else
	for_each(inst_list.begin(), inst_list.end(),
	unary_compose_void(
		bind2nd_argval_void(mem_fun_ref(
			&instance_reference_type::collect_transient_info), m),
		dereference<inst_ref_ptr_type>()
	)
	);
#endif
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
void
ALIAS_CONNECTION_CLASS::write_object(const persistent_object_manager& m,
		ostream& o) const {
	STACKTRACE_PERSISTENT("alias_connection<>::write_object()");
	m.write_pointer_list(o, inst_list);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
void
ALIAS_CONNECTION_CLASS::load_object(const persistent_object_manager& m,
		istream& i) {
	STACKTRACE_PERSISTENT("alias_connection<>::load_object()");
	m.read_pointer_list(i, inst_list);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_CONNECT_TCC__

