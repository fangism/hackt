/**
	\file "Object/unroll/alias_connection.tcc"
	Method definitions pertaining to connections and assignments.  
	This file was moved from "Object/art_object_connect.tcc".
 	$Id: alias_connection.tcc,v 1.11.4.2 2006/03/14 22:16:55 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_ALIAS_CONNECTION_TCC__
#define	__HAC_OBJECT_UNROLL_ALIAS_CONNECTION_TCC__

// compilation switches for debugging
#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_DESTRUCTORS		0 && ENABLE_STACKTRACE
#endif

#include <iostream>
#include <functional>
#include <algorithm>

#include "Object/unroll/alias_connection.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/expr/expr_dump_context.h"

#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.h"
#include "util/what.h"
#include "util/binders.h"
#include "util/compose.h"
#include "util/dereference.h"
#include "util/memory/count_ptr.tcc"
#include "util/reserve.h"

//=============================================================================
namespace HAC {
namespace entity {
class const_param_expr_list;
template <class> class instance_alias_info;
USING_IO_UTILS
using std::vector;
using util::persistent_traits;
#include "util/using_ostream.h"
using std::mem_fun_ref;
using std::transform;
using std::find_if;
using util::dereference;
using util::memory::never_ptr;
USING_UTIL_COMPOSE

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
ALIAS_CONNECTION_CLASS::dump(ostream& o, const expr_dump_context& dc) const {
	INVARIANT(inst_list.size() > 1);
	const_iterator iter(inst_list.begin());
	const const_iterator end(inst_list.end());
	NEVER_NULL(*iter);
	(*iter)->dump(o, dc);
	for (iter++; iter!=end; iter++) {
		NEVER_NULL(*iter);
		(*iter)->dump(o << " = ", dc);
	}
	return o << ';';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
void
ALIAS_CONNECTION_CLASS::reserve(const size_t s) {
	util::reserve(inst_list, s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Initializes an instance reference connection with the
	first instance reference.  
	\pre already type checked?
	\param i instance reference to connect, may not be NULL.
 */
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
void
ALIAS_CONNECTION_CLASS::append_meta_instance_reference(
		const generic_inst_ptr_type& i) {
	NEVER_NULL(i);
	// need dynamic cast
	const inst_ref_ptr_type
		irp(i.template is_a<const meta_instance_reference_type>());
		// gcc-3.3 slightly crippled, needs template keyword :(
	NEVER_NULL(irp);
	inst_list.push_back(irp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Connects the referenced instance aliases.  
	NEW INVARIANTS: 2005-08-17
	\pre either all aliases in each ring have the same relaxed
		actuals, or they are all NULL.  
	\post unless there is an error, all aliases in the newly connected 
		rings have equivalent relaxed actuals, although synchronizing
		their pointers is optional (more time-consuming, 
		smaller objects).  
		This means when we connect, we don't have to search
		each ring for aliases.  
	NOTE: changes made here should also propagate to 
		simple_meta_instance_reference::connect_port.
 */
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
good_bool
ALIAS_CONNECTION_CLASS::unroll(const unroll_context& c) const {
	typedef	vector<alias_collection_type>	alias_collection_array_type;
	typedef	vector<typename alias_collection_type::iterator>
					alias_collection_iterator_array_type;
	typedef	vector<typename alias_collection_type::const_iterator>
				alias_collection_const_iterator_array_type;
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "&c = " << &c << endl;
#endif
/***
	We're connecting a series of references x = y = z = ...
	(where each reference may be collective.)
	Create an array of collections of aliases  (packed_array_generic)
	as or workspace to iterator over and form connections.  
***/
	const size_t num_refs = inst_list.size();
	alias_collection_array_type ref_array(num_refs);
	INVARIANT(ref_array.size() >= 1);
	const_iterator iter(inst_list.begin());
	const const_iterator end(inst_list.end());
	typename alias_collection_array_type::iterator
		ref_iter(ref_array.begin());
	bool err = false;
/***
	Into each alias-collection in the array, 
	unpack the packed-referenced collection of aliases.  
	// consider rewriting as transform? but with error handling?
***/
	for ( ; iter != end; iter++, ref_iter++) {
		NEVER_NULL(*iter);
		if ((*iter)->unroll_references(c, *ref_iter).bad)
			err = true;
	}
	if (err) {
		what(cerr << "ERROR: unrolling instance references in ") <<
			"::unroll()." << endl;
		return good_bool(false);
	}
/***
	Make sure each alias collection in the array has the same dimensions.  
	Compare every collection after the first to the first.  
	Any mismatch is an error.  
***/
	typename alias_collection_array_type::const_iterator
		cref_iter(ref_array.begin());
	const typename alias_collection_array_type::const_iterator
		ref_end(ref_array.end());
	const typename alias_collection_type::key_type
		head_size(cref_iter->size());
	size_t j = 2;
	for (cref_iter++; cref_iter != ref_end; cref_iter++, j++) {
		const typename alias_collection_type::key_type
			cref_size(cref_iter->size());
		if (cref_size != head_size) {
			what(cerr << "ERROR: unrolling packed instance "
				"aliases in ") << ':'  << endl <<
				"\tsize of reference " <<
				j << " (";
			inst_list[j-1]->dump(cerr,
				expr_dump_context::brief) <<
				") = " << cref_size <<
				endl << "\tsize of reference 1 (";
			inst_list.front()->dump(cerr, 
				expr_dump_context::brief) <<
				") = " << head_size << endl;
			err = true;
		}
	}
	// to be continued...
	if (err) {
		return good_bool(false);
	}
/***
	Since every alias collection has same type, we can use the same
	key to generate keys for all of the collections.  
	We have no need for multidimensional keys because all sizes are the
	same and all offset of the unpacked alias collection are zeroed.  
	Construct an array of iterators, initialized to each alias 
	collection's begin().  
***/
	alias_collection_iterator_array_type ref_iter_array(num_refs);
	transform(ref_array.begin(), ref_array.end(), ref_iter_array.begin(), 
		// explicit arguments help template argument deduction
		// otherwise, may accidentally use begin() const, 
		// which returns a const_iterator.
		mem_fun_ref<typename alias_collection_type::iterator, 
			alias_collection_type>(&alias_collection_type::begin)
	);
#if 0
	// just testing...
	alias_collection_const_iterator_array_type
		ref_end_array(num_refs);	// all default values
	transform(ref_array.begin(), ref_array.end(), ref_end_array.begin(), 
		mem_fun_ref(&alias_collection_type::end)
	);
#endif
/***
	Type-check during connection.
		Collectible vs. connectible!  Different semantics!
		Collectible !=> connectible!  Must check each reference!
	Use vector of iterators to walk?
	Fancy: cache type-checking...
	TEMPORARY: plain type-check

	For each element in each collection, we walk the whole array 
	of aliases while advancing the entire array of iterators in lock-step.  
***/
	do {
		// convenient scope-local typedef
		typedef	typename alias_collection_type::iterator iterator_type;
		typedef	typename alias_collection_type::const_iterator
							const_iterator_type;
		typedef	typename alias_collection_iterator_array_type::iterator
						iter_iter_type;
		// ref_iter_head is the element in the
		// first packed alias collection
		const iter_iter_type ref_iter_head(ref_iter_array.begin());
		const iter_iter_type ref_iter_end(ref_iter_array.end());
		const never_ptr<instance_alias_base_type> head(**ref_iter_head);
		NEVER_NULL(head);
		// ref_iter_iter will walk along the array of references
		// starting with the second packed collection
		iter_iter_type ref_iter_iter(ref_iter_head);
		// we postpone relaxed actuals checking until the create phase,
		// after having collected all union_find sets
		// skip the head, don't try to connect it to itself
		for (ref_iter_iter++; ref_iter_iter != ref_iter_end;
				ref_iter_iter++) {
			// this loop connects the first alias in the list
			// to the others, a 1-to-N connection.
			// HOWEVER, we need to check actuals for against 
			// all connectees, not just the first.
			// make the connection!
			const never_ptr<instance_alias_base_type>
				connectee(**ref_iter_iter);
			NEVER_NULL(connectee);
			// all type-checking is done in this call:
			// punt relaxed type checking until create
			if (!instance_alias_base_type::checked_connect_alias(
					*head, *connectee).good) {
				// already have error message
				return good_bool(false);
			}
		}
		for_each(ref_iter_head, ref_iter_end, 
			// ambiguous, postfix or prefix (doesn't matter)
			// returning iterator_type& forces prefix version
			mem_fun_ref<iterator_type&>(&iterator_type::operator++)
		);
	} while (ref_iter_array.front() != ref_array.front().end());
	return good_bool(true);
}	// end alias_connection::unroll()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
void
ALIAS_CONNECTION_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	STACKTRACE_PERSISTENT("alias_connection<>::collect_transients()");
#if 1
	const_iterator iter(inst_list.begin());
	const const_iterator end(inst_list.end());
	for ( ; iter!=end; iter++) {
		(*iter)->collect_transient_info(m);
	}
#else
	for_each(inst_list.begin(), inst_list.end(),
	unary_compose_void(
		bind2nd_argval_void(mem_fun_ref(
			&simple_meta_instance_reference_type::collect_transient_info), m),
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
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_ALIAS_CONNECTION_TCC__

