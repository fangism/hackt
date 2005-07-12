/**
	\file "Object/art_object_connect.tcc"
	Method definitions pertaining to connections and assignments.  
 	$Id: art_object_connect.tcc,v 1.8.4.3.2.1 2005/07/12 23:30:57 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_CONNECT_TCC__
#define	__OBJECT_ART_OBJECT_CONNECT_TCC__

// compilation switches for debugging
#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_DESTRUCTORS		0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS		0 && ENABLE_STACKTRACE
#endif

#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>

#include "Object/art_object_connect.h"
#include "Object/art_object_inst_ref_subtypes.h"

#include "util/persistent_object_manager.h"
#include "util/STL/list.h"
#include "util/stacktrace.h"
#include "util/what.h"
#include "util/binders.h"
#include "util/compose.h"
#include "util/dereference.h"
#include "util/memory/count_ptr.tcc"

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
class const_param_expr_list;
template <class> class instance_alias_info;
USING_IO_UTILS
using std::vector;
using util::persistent_traits;
#include "util/using_ostream.h"
using std::mem_fun_ref;
using std::transform;
using util::dereference;
using util::memory::never_ptr;
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
	const_iterator iter(inst_list.begin());
	const const_iterator end(inst_list.end());
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
ALIAS_CONNECTION_CLASS::append_meta_instance_reference(
		const generic_inst_ptr_type& i) {
	NEVER_NULL(i);
	// need dynamic cast
	const inst_ref_ptr_type
		irp(i.template is_a<const simple_meta_instance_reference_type>());
		// gcc-3.3 slightly crippled, needs template keyword :(
	NEVER_NULL(irp);
	inst_list.push_back(irp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	ALIAS_CONNECTION_UNROLL_VERBOSE		0
/**
	Connects the referenced instance aliases.  
	TODO: document this method with pseudocode, it's important.  
 */
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
good_bool
ALIAS_CONNECTION_CLASS::unroll(unroll_context& c) const {
	typedef	vector<alias_collection_type>	alias_collection_array_type;
	typedef	vector<typename alias_collection_type::iterator>
					alias_collection_iterator_array_type;
	typedef	vector<typename alias_collection_type::const_iterator>
				alias_collection_const_iterator_array_type;
//	what(cerr << "Fang, finish ") << "::unroll()!" << endl;
//	Create a vector of alias_collection_type (packed_array_generic)
	const size_t num_refs = inst_list.size();
	alias_collection_array_type ref_array(num_refs);
	INVARIANT(ref_array.size() >= 1);
	const_iterator iter(inst_list.begin());
	const const_iterator end(inst_list.end());
	typename alias_collection_array_type::iterator
		ref_iter(ref_array.begin());
	bool err = false;
	// transform?
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
	Make sure each packed array has the same dimensions.  
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
				"aliases in ") << ": size of reference " <<
				j << " = " << cref_size << endl;
			cerr << "\tsize of reference 1 = " << head_size << endl;
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
***/
	alias_collection_iterator_array_type
		ref_iter_array(num_refs);	// all default values
	// initialize array iterators with pointer to first element 
	// of each packed alias collection.
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
***/
	do {
		// convenient scope-local typedef
		typedef	typename alias_collection_type::iterator iterator_type;
		typedef	typename alias_collection_type::const_iterator
							const_iterator_type;
		typedef	typename alias_collection_iterator_array_type::iterator
						iter_iter_type;
#if ALIAS_CONNECTION_UNROLL_VERBOSE
		cerr << "Connecting packed aliases..." << endl;
#endif
		// ref_iter_head is the element in the
		// first packed alias collection
		const iter_iter_type ref_iter_head(ref_iter_array.begin());
		const iter_iter_type ref_iter_end(ref_iter_array.end());
		const never_ptr<instance_alias_base_type> head(**ref_iter_head);
		NEVER_NULL(head);
		// ref_iter_iter will walk along the array of references
		// starting with the second packed collection
		iter_iter_type ref_iter_iter(ref_iter_head);
		ref_iter_iter++;
		/***
			Aliases in this sequence must be connectibly
			type-equivalent, i.e. their relaxed parameters
			(if applicable) MUST be equal.  
			We use the following placeholder to track
			the actuals as aliases references are visited
			from left to right.  
			While it is NULL, connections are allowed, 
			there is no need to check.  
		***/
		typedef	count_ptr<const const_param_expr_list>
				relaxed_actuals_ptr_type;
		relaxed_actuals_ptr_type
			current_relaxed_actuals(head->find_relaxed_actuals());
		for ( ; ref_iter_iter != ref_iter_end; ref_iter_iter++) {
			// this loop connects the first alias in the list
			// to the others, a 1-to-N connection.
			// HOWEVER, we need to check actuals for against 
			// all connectees, not just the first.
			// make the connection!
#if ALIAS_CONNECTION_UNROLL_VERBOSE
			cerr << "Connecting one alias..." << endl;
#endif
			const never_ptr<instance_alias_base_type>
				connectee(**ref_iter_iter);
			NEVER_NULL(connectee);
			// TODO: type-check for connectibility here!!!
#if ALIAS_CONNECTION_UNROLL_VERBOSE
			cerr << "size before: " << head->size();
#endif
			if (!head->must_match_type(*connectee)) {
				// already have error message
				return good_bool(false);
			}
#if 1
			// need to compare relaxed actuals if applicable!
			// this is util::ring_node::merge()
			// 2005-07-09: added actuals check
			// TODO: factors this out into a policy so that
			// meta-types that don't have relaxed actuals
			// may pass through this as a No-op!
			const relaxed_actuals_ptr_type&
				connectee_actuals(
					connectee->find_relaxed_actuals());
			typedef	typename instance_alias_base_type::actuals_parent_type
						relaxed_actuals_policy;
			if (!relaxed_actuals_policy::compare_and_update_actuals(
					current_relaxed_actuals,
					connectee_actuals).good) {
				// already have error message
				return good_bool(false);
			}
#endif
			// TODO: policy-determined recursive alias connection
			// checking.  
			head->merge(*connectee);
#if ALIAS_CONNECTION_UNROLL_VERBOSE
			cerr << ", size after: " << head->size() << endl;
#endif
		}
		for_each(ref_iter_head, ref_iter_end, 
			// ambiguous, postfix or prefix (doesn't matter)
			// returning iterator_type& forces prefix version
			mem_fun_ref<iterator_type&>(&iterator_type::operator++)
		);
	} while (ref_iter_array.front() != ref_array.front().end());
	return good_bool(true);
}

#undef	ALIAS_CONNECTION_UNROLL_VERBOSE

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
good_bool
ALIAS_CONNECTION_CLASS::unroll_meta_connect(unroll_context& c) const {
	return this->unroll(c);
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
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_CONNECT_TCC__

