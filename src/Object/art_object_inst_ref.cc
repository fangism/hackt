/**
	\file "art_object_inst_ref.cc"
	Method definitions for the instance_reference family of objects.
 	$Id: art_object_inst_ref.cc,v 1.21.2.5.2.4 2005/02/23 21:12:35 fang Exp $
 */

#ifndef	__ART_OBJECT_INST_REF_CC__
#define	__ART_OBJECT_INST_REF_CC__

#include <iostream>

#include "STL/list.tcc"
#include "multidimensional_sparse_set.tcc"

#include "art_object_type_ref_base.h"
#include "art_object_instance.h"
#include "art_object_instance_param.h"
#include "art_object_namespace.h"
#include "art_object_inst_ref.tcc"
#include "art_object_member_inst_ref.tcc"
#include "art_object_inst_stmt_base.h"
#include "art_object_expr.h"		// for dynamic_range_list
#include "art_object_control.h"
#include "art_object_connect.h"		// for aliases_connection_base
#include "persistent_object_manager.tcc"
#include "art_built_ins.h"
#include "art_object_type_hash.h"

#if USE_CLASSIFICATION_TAGS
#include "art_object_classification_details.h"
#endif

//=============================================================================
// DEBUG OPTIONS -- compare to MASTER_DEBUG_LEVEL from "art_debug.h"

namespace util {
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::process_instance_reference, 
		SIMPLE_PROCESS_INSTANCE_REFERENCE_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::channel_instance_reference, 
		SIMPLE_CHANNEL_INSTANCE_REFERENCE_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::process_member_instance_reference, 
		MEMBER_PROCESS_INSTANCE_REFERENCE_TYPE_KEY)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::channel_member_instance_reference, 
		MEMBER_CHANNEL_INSTANCE_REFERENCE_TYPE_KEY)

SPECIALIZE_UTIL_WHAT(ART::entity::process_instance_reference, 
		"process-inst-ref")
SPECIALIZE_UTIL_WHAT(ART::entity::channel_instance_reference, 
		"channel-inst-ref")
SPECIALIZE_UTIL_WHAT(ART::entity::process_member_instance_reference, 
		"process-member-inst-ref")
SPECIALIZE_UTIL_WHAT(ART::entity::channel_member_instance_reference, 
		"channel-member-inst-ref")
}	// end namespace util

//=============================================================================
namespace ART {
namespace entity {
#include "using_ostream.h"
using util::multidimensional_sparse_set_traits;
using util::multidimensional_sparse_set;
using util::write_value;
using util::read_value;
using util::persistent_traits;

//=============================================================================
// class instance_reference_base method definitions

/**
	Wrapped interface to constructing type-specific alias connections.  
 */
excl_ptr<aliases_connection_base>
instance_reference_base::make_aliases_connection(
		const count_ptr<const instance_reference_base>& i) {
	NEVER_NULL(i);
	return i->make_aliases_connection_private();
	// have the option of adding first instance here...
	// ret->append_instance_reference(i);
}

//=============================================================================
// class simple_instance_reference::mset_base definition

class simple_instance_reference::mset_base {
public:
	typedef	multidimensional_sparse_set_traits<
			pint_value_type, const_range, list>
						traits_type;
	typedef	traits_type::range_type		range_type;
	typedef	traits_type::range_list_type	range_list_type;

	/// dimensions limit
	enum { LIMIT = 4 };

virtual	~mset_base() { }

virtual	range_list_type
	compact_dimensions(void) const = 0;

virtual	range_list_type
	query_compact_dimensions(const range_list_type& r) const = 0;

virtual	bool
	add_ranges(const range_list_type& r) = 0;

virtual	bool
	subtract_sparse_set(const mset_base& s) = 0;

virtual	void
	clear(void) = 0;

virtual	bool
	empty(void) const = 0;

virtual	ostream&
	dump(ostream& o) const = 0;

	static
	mset_base*
	make_multidimensional_sparse_set(const size_t d);

};	// end class simple_instance_reference::mset_base

//=============================================================================
/**
	Wrapper class to sparse set implementation, 
	implements a limited interface for use in this module.  
 */
template <size_t D>
class simple_instance_reference::mset :
		public simple_instance_reference::mset_base {
protected:
	typedef	multidimensional_sparse_set<D, pint_value_type, const_range>
							impl_type;
	typedef	simple_instance_reference::mset_base	base_type;
	typedef	mset<D>					this_type;
public:
	typedef base_type::range_type			range_type;
	typedef base_type::range_list_type		range_list_type;
protected:
	impl_type			sset;
public:

	// standard destructor

	range_list_type
	compact_dimensions(void) const {
		return sset.compact_dimensions();
	}

	range_list_type
	query_compact_dimensions(const range_list_type& r) const {
		return sset.query_compact_dimensions(r);
	}

	bool
	add_ranges(const range_list_type& r) {
		return sset.add_ranges(r);
	}

	bool
	subtract_sparse_set(const mset_base& s) {
		const this_type* t = IS_A(const this_type*, &s);
		INVARIANT(t);
		return sset.subtract(t->sset);
	}

	void
	clear(void) { sset.clear(); }

	bool
	empty(void) const { return sset.empty(); }

	ostream&
	dump(ostream& o) const {
		return sset.dump(o);
	}


};	// end class simple_instance_reference::mset

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
simple_instance_reference::mset_base*
simple_instance_reference::mset_base::make_multidimensional_sparse_set(
		const size_t d) {
	INVARIANT(d > 0 && d <= LIMIT);
	switch(d) {
		case 1: return new simple_instance_reference::mset<1>();
		case 2: return new simple_instance_reference::mset<2>();
		case 3: return new simple_instance_reference::mset<3>();
		case 4: return new simple_instance_reference::mset<4>();
		// add more cases if LIMIT is ever extended.
		default: return NULL;
	}
}

//=============================================================================
// class simple_instance_reference method definitions

/**
	Private empty constructor.
 */
simple_instance_reference::simple_instance_reference() :
		array_indices(NULL), inst_state() {
	// no assert
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
simple_instance_reference::simple_instance_reference(
		const instantiation_state& st) :
		array_indices(NULL), 
		inst_state(st) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	May be obsolete...
 */
simple_instance_reference::simple_instance_reference(
		excl_ptr<index_list>& i, 
		const instantiation_state& st) :
		array_indices(i), 
		inst_state(st) {
	// in sub-type constructors, 
	// INVARIANT(array_indices->size < get_inst_base->dimensions());
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
simple_instance_reference::~simple_instance_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dimensionality of an indexed references depends on
	which indexed dimensions are collapsed x[i], and which
	are not x[i..j].  
	A fully collapsed index list is equivalent to a 0-d array
	or a single instance.  
	An expression without indices has dimensionality equal
	to the of the instance collection.  
	\return the dimensions of the referenced array.  
 */
size_t
simple_instance_reference::dimensions(void) const {
	size_t dim = get_inst_base()->get_dimensions();
	if (array_indices) {
		const size_t c = array_indices->dimensions_collapsed();
		INVARIANT(c <= dim);
		return dim -c;
	}
	else return dim;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
simple_instance_reference::get_type_ref(void) const {
	return get_inst_base()->get_type_ref();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const definition_base>
simple_instance_reference::get_base_def(void) const {
	return get_inst_base()->get_base_def();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Queries whether or not there were any dynamic instances added
	to the collection from the initial instantiation up to the
	point of references of this instance reference.  
	Note: This routine is independent of the array_indices.  
	\return true if all instance additions upto this point
		were static constant (sparse or dense) indices.  
 */
bool
simple_instance_reference::is_static_constant_collection(void) const {
	instantiation_state iter = inst_state;
	const instantiation_state
		end(get_inst_base()->collection_state_end());
	for ( ; iter!=end; iter++) {
		const count_ptr<const dynamic_range_list>
			drl((*iter)->get_indices().is_a<const dynamic_range_list>());
		if (drl) {
			if (!drl->is_static_constant())
				return false;
			// unconditional false is too conservative
		}
		else	INVARIANT((*iter)->get_indices()
				.is_a<const const_range_list>());
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	May need to perform dimension-collapsing in some cases.  
	Precondition: array indices already bound-checked if static constant.  
	Prerequisites for calling this method: non-zero dimension.  
	This function guarantees nothing about the packed-ness of the
	current state of the collection.  
	Cases: 
	1) Reference is not-indexed to a non-collective instance set.  
		Then this is zero-dimensional (scalar).  
	2) Reference is not-indexed to a collective instance set.
		See if the instance collection from the point of reference
		has statically resolvable dimensions.  
	3) Reference is partially-indexed to a collective instance set.  
	4) Reference is fully-indexed to a collective instance set, 
		down to the last dimension.  
	When in doubt, this is just a compile-time check, 
		can conservatively return false.  
 */
bool
simple_instance_reference::has_static_constant_dimensions(void) const {
	// case 1: instance collection is not collective. 
	const size_t base_dim = get_inst_base()->get_dimensions();
	if (base_dim == 0)
		return true;
	// case 2: reference is not-indexed, and instance is collective.
	// implicitly refers to the entire collection.
	// (same case if dimensions are under-specified)
	else if (!array_indices) {
		// is the entire collection known statically?
		return is_static_constant_collection();
	} else if (array_indices->size() < base_dim) {
		// case 3: partially-specified indices, implicit sub-collections
		if (!array_indices->is_static_constant())
			return false;
		else return is_static_constant_collection();
		// else we know entire collection statically.
	} else {
		// case 4: fully-indexed down to last dimension
		return array_indices->is_static_constant();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Preconditions: 
	If any instance additions are dynamic, 
	conservatively return true.  
	Cases: {collective, scalar} x {non-indexed, partial, fully-indexed}
	In the conservative case, return true.  
 */
bool
simple_instance_reference::may_be_densely_packed(void) const {
	const size_t base_dim = get_inst_base()->get_dimensions();
	// if not collective, then return true (not really applicable)
	if (base_dim == 0)
		return true;
	// else is collective
	if (array_indices) {
		const never_ptr<const index_list> il(array_indices);
		const never_ptr<const const_index_list>
			cil(il.is_a<const const_index_list>());
		if (!cil)
			return true;
		if (array_indices->size() < base_dim) {
			// Array indices are underspecified. 
			// Unpack instance collection into
			// multidimensional_sparse_set
			const excl_ptr<const mset_base>
				fui = unroll_static_instances(base_dim)
					.as_a_xfer<const mset_base>();
			// need explicit transfer :S
			NEVER_NULL(fui);
			// convert index to ranges
			const const_range_list crl(*cil);
			const mset_base::range_list_type
				rl(fui->query_compact_dimensions(crl));
			return !rl.empty();
		} else {
			// array indices are fully specified, and constant
			return true;
		}
	} else {
		// not indexed, implicitly refers to entire collection
		// TO DO: unpack instance collection into
		// multidimensional_sparse_set
		const excl_ptr<const mset_base>
			fui = unroll_static_instances(base_dim)
				.as_a_xfer<const mset_base>();
		NEVER_NULL(fui);
		const mset_base::range_list_type
			rl(fui->compact_dimensions());
		return !rl.empty();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Much like may_be_densely_packed, but conservatively returns false.  
 */
bool
simple_instance_reference::must_be_densely_packed(void) const {
	const size_t base_dim = get_inst_base()->get_dimensions();
	// if not collective, then return true (not really applicable)
	if (base_dim == 0)
		return true;
	// else is collective
	if (array_indices) {
		const never_ptr<const index_list> il(array_indices);
		const never_ptr<const const_index_list>
			cil(il.is_a<const const_index_list>());
		if (!cil)
			return false;		// only difference from above
		if (array_indices->size() < base_dim) {
			// array indices are underspecified
			// TO DO: unpack instance collection into
			// multidimensional_sparse_set
			const excl_ptr<const mset_base>
				fui = unroll_static_instances(base_dim)
					.as_a_xfer<const mset_base>();
			NEVER_NULL(fui);
			// convert index to ranges
			const const_range_list crl(*cil);
			const mset_base::range_list_type
				rl(fui->query_compact_dimensions(crl));
			return !rl.empty();
		} else {
			// array indices are fully specified, and constant
			return true;
		}
	} else {
		// not indexed, implicitly refers to entire collection
		// TO DO: unpack instance collection into
		// multidimensional_sparse_set
		const excl_ptr<const mset_base>
			fui = unroll_static_instances(base_dim)
				.as_a_xfer<const mset_base>();
		NEVER_NULL(fui);
		const mset_base::range_list_type
			rl(fui->compact_dimensions());
		return !rl.empty();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Repacks the instance collection form the point of reference 
	into a dense array, if possible.  
	No dimension-collapsing in this routine!
	Just checks for coverage and compactness.  
	If fails, returns empty list.  
	Prerequisites: must already satisfy:
		non-zero dimensions
		must_be_densely_packed (or not)
		has_static_constant_dimensions
	\return dense range list representation of the instance collection
		if it is indeed compact, else returns an empty list.  
	\sa implicit_static_constant_indices
 */
const_range_list
simple_instance_reference::static_constant_dimensions(void) const {
	const size_t base_dim = get_inst_base()->get_dimensions();
	INVARIANT(base_dim);		// must have no-zero dimensions
	if (array_indices) {
		const never_ptr<const index_list> il(array_indices);
		const never_ptr<const const_index_list>
			cil(il.is_a<const const_index_list>());
		if (!cil)	// is dynamic
			return const_range_list();
		// array indices are underspecified or fully specified
		const excl_ptr<const mset_base>
			fui = unroll_static_instances(base_dim)
				.as_a_xfer<const mset_base>();
		NEVER_NULL(fui);
		// convert index to ranges, but DON'T COLLAPSE dimensions yet!
		// should collapse dimensions AFTER checking coverage
		// and compactness of the range.  
		const const_range_list crl(*cil);
		const mset_base::range_list_type
			rl(fui->query_compact_dimensions(crl));
		return const_range_list(rl);	// will probably have to convert
	} else {
		// not indexed, implicitly refers to entire collection
		// no dimensions will be collapsed
		const excl_ptr<const mset_base>
			fui = unroll_static_instances(base_dim)
				.as_a_xfer<const mset_base>();
		NEVER_NULL(fui);
		const mset_base::range_list_type
			rl(fui->compact_dimensions());
		return const_range_list(rl);	// will probably have to convert
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prerequisites: must_be_densely_packed, is_static_constant_collection, 
		have non-zero dimension.  
	For a collection reference with underspecified indices, 
	this fills in the remaining dimensions, ONLY IF the remaining 
	dimensions are packed.  
	Very similar to must_be_densely_packed.  
	\return indices filled with dimensions for unspecified dimensions, 
		else will assert fail if anything goes wrong.  
 */
const_index_list
simple_instance_reference::implicit_static_constant_indices(void) const {
	const size_t base_dim = get_inst_base()->get_dimensions();
	// if not collective, then return true (not really applicable)
	INVARIANT(base_dim);		// non-zero dimension only!
	// else is collective
	if (array_indices) {
		const never_ptr<const index_list> il(array_indices);
		const never_ptr<const const_index_list>
			cil(il.is_a<const const_index_list>());
		NEVER_NULL(cil);
		const size_t a_size = array_indices->size();
		// or compute equivalent from a const dynamic_index_list?
		if (a_size < base_dim) {
			// array indices are underspecified
			// TO DO: unpack instance collection into
			// multidimensional_sparse_set
			const excl_ptr<const mset_base>
				fui = unroll_static_instances(base_dim)
					.as_a_xfer<const mset_base>();
			NEVER_NULL(fui);
			// convert index to ranges
			const const_range_list crl(*cil);
			const mset_base::range_list_type
				rl(fui->query_compact_dimensions(crl));
			// is a list<const_range>, must convert to index_list
			INVARIANT(!rl.empty());

			// add implied indices (ranges) back to original
			// sanity consistency check first...
			mset_base::range_list_type::const_iterator
				ri = rl.begin();
			const_index_list::const_iterator
				ai = cil->begin();
			for ( ; ai!=cil->end(); ai++, ri++) {
				NEVER_NULL(*ai);
				INVARIANT(**ai == *ri);	// consistency check
			}
			const_index_list ret(*cil);	// copy, then append
			for ( ; ri!=rl.end(); ri++) {
				ret.push_back(count_ptr<const_index>(
					new const_range(*ri)));
			}
			return ret;
		} else {
			// array indices are fully specified, and constant
			return const_index_list(*cil);
		}
	} else {
		// not indexed, implicitly refers to entire collection
		// TO DO: unpack instance collection into
		// multidimensional_sparse_set
		const excl_ptr<const mset_base>
			fui = unroll_static_instances(base_dim)
				.as_a_xfer<const mset_base>();
		NEVER_NULL(fui);
		const mset_base::range_list_type
			rl(fui->compact_dimensions());
		mset_base::range_list_type::const_iterator
			ri = rl.begin();
		const_index_list ret;
		for ( ; ri!=rl.end(); ri++) {
			ret.push_back(count_ptr<const_index>(
				new const_range(*ri)));
		}
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
simple_instance_reference::dump(ostream& o) const {
	what(o) << " " << get_inst_base()->get_name();
	if (array_indices) {
		array_indices->dump(o);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints out the type and size (if known).  
 */
ostream&
simple_instance_reference::dump_type_size(ostream& o) const {
	get_type_ref()->dump(o);
	if (!dimensions()) {
		return o;
	} else if (has_static_constant_dimensions()) {
		const const_index_list
			cil(implicit_static_constant_indices());
		// consider making this into a method:
		const_index_list::const_iterator i = cil.begin();
		for ( ; i!=cil.end(); i++) {
			const count_ptr<const const_index> ind(*i);
			const count_ptr<const const_range>
				cr(ind.is_a<const const_range>());
			if (cr) {
				const int diff = cr->second -cr->first +1;
				o << "[" << diff << "]";
			}
			// else don't print collapsed dimensions
		}
	} else {
		// don't know all dimensions statically
		o << "{" << dimensions() << "-dim}";
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
simple_instance_reference::hash_string(void) const {
	string ret(get_inst_base()->get_qualified_name());
	if (array_indices) {
		ret += array_indices->hash_string();
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds indices to instance reference.  
	Must check with number of number of dimensions.  
	(Should we allow under-specification of dimensions?
		i.e. x[i] of a 2-D array to be a 1D array reference.)
	Then referring to x is referring to the entire array of x.  

	Fancy: an indexed instance reference may be under-specified
	Referring to partial sub-arrays, e.g. x[i] where x is 2-dimensional.  
	In this case, when we check for static instance coverage, 
	
	\return true if successful, else false.  
 */
bool
simple_instance_reference::attach_indices(excl_ptr<index_list>& i) {
	// make sure not already indexed
	// side note: if indexing were truly recursive and not list-based, 
	//	we'd be able to append indices one-by-one.  
	INVARIANT(!array_indices);
	NEVER_NULL(i);
	// dimension-check:
	const never_ptr<const instance_collection_base> inst_base(get_inst_base());
	// number of indices must be <= dimension of instance collection.  
	const size_t max_dim = dimensions();	// depends on indices
	if (i->size() > max_dim) {
		cerr << "ERROR: instance collection " << inst_base->get_name()
			<< " is " << max_dim << "-dimensional, and thus, "
			"cannot be indexed " << i->size() <<
			"-dimensionally!  ";
			// caller will say where
		return false;
	} 
	// else proceed...

	// allow under-specified dimensions?  yeah for now...
	// if indices are constant, check for total overlap
	// with existing instances from the point of reference.

	// mset_base typedef'd privately
	// overriding default implementation with pair<int, int>
	INVARIANT(max_dim <= mset_base::LIMIT);
	never_ptr<const index_list> il(i);
	never_ptr<const const_index_list>
		cil(il.is_a<const const_index_list>());
	if (!cil) {	// is dynamic, conservatively covers anything
		never_ptr<const dynamic_index_list>
			dil(il.is_a<const dynamic_index_list>());
		NEVER_NULL(dil);
		array_indices = i;
		return true;
	}
	// else is constant index list, can compute coverage
	//	using multidimensional_sparse_set

	// eventually replace the following loop with unroll_static_instances
	const size_t cil_size = cil->size();
	const excl_ptr<mset_base>
		cov(mset_base::make_multidimensional_sparse_set(cil_size));
	NEVER_NULL(cov);
	{
		const_range_list cirl(*cil);
		// if dimensions are underspecified, then
		// we need to trim the lower dimension indices.
		cov->add_ranges(cirl);
	}
	const excl_ptr<mset_base> inst = unroll_static_instances(cil_size);
	if (inst) {
		cov->subtract_sparse_set(*inst);
		// make sure to clean if empty in subtract() method
	}
	else	// was dynamic, potentially covering all indices
		cov->clear();

	// if this point reached, then all instance additions
	// were static constants.
	// now, covered set must completely contain indices
	if (!cov->empty()) {
		// empty means covered.  
		cerr << "ERROR: The following referenced indices of \""
			<< get_inst_base()->get_name() <<
			"\" have definitely not been instantiated: {";
		cov->dump(cerr << endl) << "} ";
		// cerr << where() << endl;	// caller
		// fancy: list indices not instantiated?
		return false;
	}
	array_indices = i;
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks "may" type-equivalence of two instance references.  
	Is conservative.  
	An unpacked collection reference is equivalent to nothing.  
	Prints descriptive error if false is returned.  
	\return true if referenced instances' types may be equivalent.  
 */
bool
simple_instance_reference::may_be_type_equivalent(
		const instance_reference_base& i) const {
	const never_ptr<const instance_collection_base>
		lib(get_inst_base());
	const never_ptr<const instance_collection_base>
		rib(i.get_inst_base());
	const count_ptr<const fundamental_type_reference>
		ltr(lib->get_type_ref());
	const count_ptr<const fundamental_type_reference>
		rtr(rib->get_type_ref());
	const bool type_eq = ltr->may_be_equivalent(*rtr);
	// if base types differ, then cannot be equivalent
	if (!type_eq) {
		ltr->dump(cerr << "Types do not match! got: ") << " and: ";
		rtr->dump(cerr) << "." << endl;
		return false;
	}
	// else they match, continue to check dimensionality and size.  

	// TO DO: factor this section code out to a method for re-use.  
	// note: is dimensions of the *reference* not the instantiation!
	const size_t lid = dimensions();
	const size_t rid = i.dimensions();
	if (lid != rid) {
		cerr << "Dimensions do not match! got: " << dimensions()
			<< " and: " << i.dimensions() << "." << endl;
		return false;
	}
	// catch cases where one of them is scalar (zero-dimensional)
	if (!lid) {
		INVARIANT(!rid);
		return true;
	}
	// else fall-through handle multidimensional case

	// check for packed-ness of instance reference?
	// if indices only partially specify dimensions.  
	const bool l_may_pack = may_be_densely_packed();
	const bool r_may_pack = i.may_be_densely_packed();
	if (!l_may_pack || !r_may_pack) {
		// we know statically that one of them is not packed, 
		// which immediately disqualifies them from equivalence.  
		if (!l_may_pack)
			cerr << "Left instance reference not packed!" << endl;
		if (!r_may_pack)
			cerr << "Right instance reference not packed!" << endl;
		return false;
	}

	// We already know that both references *may* be packed.  
	// We can only do precise analysis if we know that
	// both instance references *must* be packed.
	const bool l_must_pack = must_be_densely_packed();
	const bool r_must_pack = i.must_be_densely_packed();
	// Otherwise, we can only conservatively return true.  
	if (!l_must_pack || !r_must_pack)
		return true;

	// Here, we know we can obtain the implicit packed indices, 
	// and then compare them.  

	const simple_instance_reference*
		sir = IS_A(const simple_instance_reference*, &i);
	if (!sir) {
		// then is not a simple_instance_reference, 
		// is complex-aggregate, which is not handled yet
		// eventually get around to this
		return true;
	}
	const const_index_list lindex(implicit_static_constant_indices());
	const const_index_list rindex(sir->implicit_static_constant_indices());

	// or just collapse these to ranges directly?
	const const_range_list ldim = 
		lindex.collapsed_dimension_ranges();
	const const_range_list rdim = 
		rindex.collapsed_dimension_ranges();

	const bool ret = ldim.is_size_equivalent(rdim);
	if (!ret) {
		ldim.dump(cerr << "got: ") << " and: ";
		rdim.dump(cerr) << endl;
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	"must" type-equivalence.  
 */
bool
simple_instance_reference::must_be_type_equivalent(
		const instance_reference_base& i) const {
	// fix me...
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For collection with only static constant additions, 
	this returns the unrolled multidimensional set of instantiated
	indices up to the point of reference.  
	No dimension trimming.  
	\param dim the number of dimensions to expand, 
		which must be <= the instantiation's dimensions.  
	\return newly allocated set of unrolled index instances
		if everything is constant, else returns null, 
		if there is even a single non-const range_list.  
 */
excl_ptr<simple_instance_reference::mset_base>
simple_instance_reference::unroll_static_instances(const size_t dim) const {
	INVARIANT(dim <= get_inst_base()->get_dimensions());
	instantiation_state iter = inst_state;
	const instantiation_state end(get_inst_base()->collection_state_end());
	excl_ptr<mset_base>
		cov(mset_base::make_multidimensional_sparse_set(dim));
	NEVER_NULL(cov);
	for ( ; iter!=end; iter++) {
		if ((*iter)->get_indices().is_a<const dynamic_range_list>())
		{
			// all we can do conservatively...
			return excl_ptr<mset_base>(NULL);
		} else {
			count_ptr<const const_range_list>
				crlp((*iter)->get_indices().is_a<const const_range_list>());
			NEVER_NULL(crlp);
			const_range_list crl(*crlp);	// make deep copy
			// dimension-trimming
			while(crl.size() > dim)
				crl.pop_back();
			const bool overlap = cov->add_ranges(crl);
			INVARIANT(!overlap);		// sanity check!
		}
	}
	return cov;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Common persistence visitor for all subclasses.  
 */
void
simple_instance_reference::collect_transient_info_base(
		persistent_object_manager& m) const {
	if (array_indices)
		array_indices->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param m the persistent object manager for pointer translation. 
	\param o the output stream to write binary.
	\pre o MUST BE the stream corresponding to this object.
 */
void
simple_instance_reference::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	write_instance_collection_state(o);
	m.write_pointer(o, array_indices);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Note: the instantiation base must be loaded before the
		state information, because the instantiation state
		depends on the instantiation base being complete.
	\param m the persistent object manager for pointer translation. 
	\param i the input stream to read binary.
	\pre i MUST BE the stream corresponding to this object.
 */
void
simple_instance_reference::load_object_base(
		const persistent_object_manager& m, istream& i) {
	load_instance_collection_state(i);
	m.read_pointer(i, array_indices);
	// must load the 
	if (array_indices)
		m.load_object_once(array_indices);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The instantiation state can be store as an index into
	an instantiation's index collection.  
	To do this we just use the distance from inst_state to the "end".
 */
void
simple_instance_reference::write_instance_collection_state(ostream& f) const {
	const instantiation_state end =
		get_inst_base()->collection_state_end();
	// assuming this is safe, of course...
	size_t dist = distance(inst_state, end);
	write_value(f, dist);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Restores the reference to an instantiation's state, 
	based on an index from the end, counting backwards.  
 */
void
simple_instance_reference::load_instance_collection_state(istream& f) {
	instantiation_state iter =
		get_inst_base()->collection_state_end();
	size_t i = 0;
	size_t max;
	read_value(f, max);
	for ( ; i<max; i++)
		iter--;
	const_cast<instantiation_state&>(inst_state) = iter;
}

//=============================================================================
#if 0
PHASE IN later...
// class collective_instance_reference method definitions

collective_instance_reference::collective_instance_reference(
		never_ptr<const instance_reference_base> b, 
		const param_expr* l, const param_expr* r) :
		instance_reference_base(), 
		lower_index(never_ptr<const param_expr>(l)),
		upper_index(never_ptr<const param_expr>(r)) {
}

collective_instance_reference::~collective_instance_reference() {
}

ostream&
collective_instance_reference::what(ostream& o) const {
	return o << "collective-inst-ref";
}

ostream&
collective_instance_reference::dump(ostream& o) const {
	return what(o);
}

string
collective_instance_reference::hash_string(void) const {
	string ret(base_array->hash_string());
	ret += "[";
	ret += lower_index->hash_string();
	if (upper_index) {
		ret += "..";
		ret += upper_index->hash_string();
	}
	ret += "]";
	return ret;
}
#endif

//=============================================================================
// class param_instance_reference method definitions

/**
	Private empty constructor.
 */
param_instance_reference::param_instance_reference() :
		simple_instance_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i index list.
	\param st the current state of the instance collection
		at the time of reference.  
 */
param_instance_reference::param_instance_reference(
		const instantiation_state& st) :
		simple_instance_reference(st) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	\param i index list.
	\param st the current state of the instance collection
		at the time of reference.  
 */
param_instance_reference::param_instance_reference(
		excl_ptr<index_list>& i, 
		const instantiation_state& st) :
		simple_instance_reference(i, st) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
param_instance_reference::may_be_initialized(void) const {
	never_ptr<const instance_collection_base> i(get_inst_base());
	NEVER_NULL(i);
	return i.is_a<const param_instance_collection>()->may_be_initialized();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
param_instance_reference::must_be_initialized(void) const {
	never_ptr<const instance_collection_base> i(get_inst_base());
	NEVER_NULL(i);
	return i.is_a<const param_instance_collection>()->must_be_initialized();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Under what conditions is a reference to a param instance
	static and constant?
	Resolved to a compile-time constant value.  
	May refer to parameters that are unconditionally initialized
	exactly once.  
	For collective variables, we just conservatively return false.  
	Oh yeah, how do we keep track whether or not a variable
	has been conditionally initialized?
	We may need some assignment stack.... PUNT!
 */
bool
param_instance_reference::is_static_constant(void) const {
	// "collective": if either reference is indexed, 
	// 	(mind, this is conservative and not precise because
	//	we don't track values of arrays at compile-time)
	//	More thoughts later on how to be more precise...
	if (array_indices)
		return false;
	// or the instance_collection_base is collective (not 0-dimensional)
	else if (get_inst_base()->get_dimensions())
		return false;
	// else if singular, whether or not it is initialized once
	// to another static constant (parameter variable or value).  
	else 
		return get_param_inst_base()->is_static_constant();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	A reference is loop independent if it's indices
	contain no references to index loop variables.  
	Or if it's simply not in a loop, since references are type-checked.  
	BEWARE: referring to a collective instance without indexing
	can also be loop-variant if instances are added to the collection
	within some loop.  
	See if the iterator into instance_collection_stack points
	to a loop-scope.  
	(Of course, we need to actually use the instance_collection stack...)
 */
bool
param_instance_reference::is_loop_independent(void) const {
	if (array_indices)
		return array_indices->is_loop_independent();
	else 
		// no array indices, see if instance_collection_base is collective
	if (get_inst_base()->get_dimensions()) {
		// if collective, check if the instance_reference itself 
		// is found within a loop that adds to the collection...
		// ... but I'm too lazy to do this entirely now
		return false;
		// FIX ME later
	} else {
		// is 0-dimension, look up and see if it happens to be
		// a loop index variable.  
		// Who owns the param_inst_base?
		never_ptr<const scopespace>
			owner(get_inst_base()->get_owner());
		return !owner.is_a<const loop_scope>();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Whether or not this instance reference is found inside a 
	conditional body.  
	Check the iterator into the instantiation base's
	collection stack.  
	Is point of reference in some conditional body
		(may be nested in loop, don't forget!)
	What about the indices themselves?  (shouldn't need to check?)
 */
bool
param_instance_reference::is_unconditional(void) const {
	if (array_indices)
		return array_indices->is_unconditional();
	// else see if point of reference is within some conditional scope
	else if (get_inst_base()->get_dimensions()) {
		// if collective, see if the instance_reference itself
		// is found within a conditional body, by walking the 
		// collection_state_iterator forward.
		// ... but I'm too lazy to do this now, FIX ME later
		// what about checking history of assignments???
		//	implies assignment stack...
		return false;		// extremely conservative
	} else {
		// also need to check assignment stack...
		return true;
	}
}

//=============================================================================
// class process_instance_reference method definitions
// replaced with instance_reference template

//=============================================================================
// class datatype_instance_reference method definitions

/**
	Private empty constructor.  
 */
datatype_instance_reference::datatype_instance_reference() :
		simple_instance_reference() {
	// no assert
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
datatype_instance_reference::datatype_instance_reference(
		const instantiation_state& s) :
		simple_instance_reference(s) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
datatype_instance_reference::datatype_instance_reference(
		excl_ptr<index_list>& i, const instantiation_state& s) :
		simple_instance_reference(i, s) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
datatype_instance_reference::~datatype_instance_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
datatype_instance_reference::what(ostream& o) const {
	return o << "datatype-inst-ref";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
USE simple_instance_reference::dump
ostream&
datatype_instance_reference::dump(ostream& o) const {
	what(o) << ": ";
	data_inst_ref->dump(o);
	if (array_indices)
		array_indices->dump(o);
	return o;
}
#endif

//=============================================================================
// class channel_instance_reference method definitions
// replaced with instance_reference template

//=============================================================================
// explicit template instantiations

#if USE_CLASSIFICATION_TAGS
template class
instance_reference<channel_instance_collection, simple_instance_reference>;
template class
instance_reference<process_instance_collection, simple_instance_reference>;
template class member_instance_reference<channel_tag>;
template class member_instance_reference<process_tag>;
#else
template class
instance_reference<channel_instance_collection, simple_instance_reference>;
template class
instance_reference<process_instance_collection, simple_instance_reference>;
template class member_instance_reference<channel_instance_reference>;
template class member_instance_reference<process_instance_reference>;
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INST_REF_CC__

