/**
	\file "Object/ref/aggregate_reference_collection_base.h"
	Base class for aggregate collection features.  
	$Id: aggregate_reference_collection_base.h,v 1.2 2006/07/04 07:26:13 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_AGGREGATE_REFERENCE_COLLECTION_BASE_H__
#define	__HAC_OBJECT_REF_AGGREGATE_REFERENCE_COLLECTION_BASE_H__

#include <vector>			// only need fwd decl
#include "util/boolean_types.h"
#include "util/packed_array_fwd.h"
#include "Object/common/multikey_index.h"

namespace HAC {
namespace entity {
using util::good_bool;
using util::packed_array_generic;

//=============================================================================
/**
	NOTE: this class is NOT to be used polymorphically.  
	It just contains common functionality.  
 */
class aggregate_reference_collection_base {
public:
	typedef std::vector<multikey_index_type>
						size_array_type;
protected:
	/**
		Aggregation mode:
		if true, then is a concatenation of arrays producing
		an array of the same number of dimensions, 
		else is a *construction* of a higher dimension array
		from subinstances.  
	 */
	bool				_is_concatenation;

	aggregate_reference_collection_base() { }

	explicit
	aggregate_reference_collection_base(const bool b) :
		_is_concatenation(b) { }

	~aggregate_reference_collection_base() { }

public:
	void
	set_concatenation_mode(void) { _is_concatenation = true; }

	void
	set_construction_mode(void) { _is_concatenation = false; }

protected:
	static
	good_bool
	check_concatenable_subarray_sizes(const size_array_type&);

	static
	good_bool
	check_constructible_subarray_sizes(const size_array_type&);

	good_bool
	check_subarray_sizes(const size_array_type&) const;

	PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
	static
	void
	resize_packed_array_for_concatenation(
		PACKED_ARRAY_GENERIC_CLASS&,
		const typename PACKED_ARRAY_GENERIC_CLASS::key_type&);

	PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
	static
	void
	resize_packed_array_for_construction(
		PACKED_ARRAY_GENERIC_CLASS&,
		const typename PACKED_ARRAY_GENERIC_CLASS::key_type&);

	PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
	void
	resize_packed_array(PACKED_ARRAY_GENERIC_CLASS&,
		const typename PACKED_ARRAY_GENERIC_CLASS::key_type&) const;

	PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
	good_bool
	check_and_resize_packed_array(PACKED_ARRAY_GENERIC_CLASS&, 
		const size_array_type&) const;

};	// end class aggregate_reference_collection_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_AGGREGATE_REFERENCE_COLLECTION_BASE_H__

