/**
	\file "Object/ref/aggregate_reference_collection_base.tcc"
	Implementation of template methods.  
	$Id: aggregate_reference_collection_base.tcc,v 1.2 2006/07/04 07:26:13 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_AGGREGATE_REFERENCE_COLLECTION_BASE_TCC__
#define	__HAC_OBJECT_REF_AGGREGATE_REFERENCE_COLLECTION_BASE_TCC__

#include "Object/ref/aggregate_reference_collection_base.h"
#include "util/packed_array.tcc"
#include "util/type_traits.h"
#include "util/static_assert.h"

namespace HAC {
namespace entity {
//=============================================================================
// class aggregate_reference_collection_base method definitions

/**
	For concatention we initialize the size by zero-ing out the
	first dimension.  
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
void
aggregate_reference_collection_base::resize_packed_array_for_concatenation(
		PACKED_ARRAY_GENERIC_CLASS& a,
		const typename PACKED_ARRAY_GENERIC_CLASS::key_type& f) {
	typedef	typename PACKED_ARRAY_GENERIC_CLASS::key_type	key_type;
	key_type new_size(f);	// copy dimensions
	new_size.front() = 0;	// temporarily clear out first dimension (thin)
	a.resize(new_size);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For concatention we initialize the size by zero-ing out the
	first dimension.  
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
void
aggregate_reference_collection_base::resize_packed_array_for_construction(
		PACKED_ARRAY_GENERIC_CLASS& a,
		const typename PACKED_ARRAY_GENERIC_CLASS::key_type& f) {
	typedef	typename PACKED_ARRAY_GENERIC_CLASS::key_type	key_type;
	// just copy pointer value-references over
	key_type new_size(f.dimensions() +1);	// add one dimension
	new_size.front() = 0;		// initialize first dim. to zero
	std::copy(f.begin(), f.end(), new_size.begin() +1);
	a.resize(new_size);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Auto-selects depending on _is_concatenation.  
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
void
aggregate_reference_collection_base::resize_packed_array(
		PACKED_ARRAY_GENERIC_CLASS& a,
		const typename PACKED_ARRAY_GENERIC_CLASS::key_type& f) const {
	if (this->_is_concatenation) {
		resize_packed_array_for_concatenation(a, f);
	} else {
		resize_packed_array_for_construction(a, f);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For the sake of code reuse, minimizing replication, 
	maximizing maintainability:
	Package sequence of functions for resizing packed collection
	of references according to subreference sizes.  
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
good_bool
aggregate_reference_collection_base::check_and_resize_packed_array(
		PACKED_ARRAY_GENERIC_CLASS& a, const size_array_type& s) const {
	typedef	typename PACKED_ARRAY_GENERIC_CLASS::key_type	_key_type;
	typedef	size_array_type::value_type		key_type;
	typedef	util::is_same<key_type, _key_type>	type_compare;
	UTIL_STATIC_ASSERT_DEPENDENT(type_compare::value);
	const key_type& f(s.front());
	if (this->_is_concatenation) {
		// concatenation of arrays into like-dimension larger arrays
		if (!check_concatenable_subarray_sizes(s).good) {
			// already have error message
			return good_bool(false);
		}
		resize_packed_array_for_concatenation(a, f);
	} else {
		// is array construction
		if (!check_constructible_subarray_sizes(s).good) {
			// already have error message
			return good_bool(false);
		}
		resize_packed_array_for_construction(a, f);
	}
	return good_bool(true);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_AGGREGATE_REFERENCE_COLLECTION_BASE_TCC__

