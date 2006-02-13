/**
	\file "Object/ref/aggregate_meta_instance_reference_base.h"
	$Id: aggregate_meta_instance_reference_base.h,v 1.1.2.1 2006/02/13 21:05:12 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_AGGREGATE_META_INSTANCE_REFERENCE_BASE_H__
#define	__HAC_OBJECT_REF_AGGREGATE_META_INSTANCE_REFERENCE_BASE_H__

#include "util/boolean_types.h"
#include "util/memory/pointer_classes_fwd.h"

namespace HAC {
namespace entity {
class meta_instance_reference_base;
using util::good_bool;
using util::memory::count_ptr;

//=============================================================================
/**
	Common interface for all aggregate value references, 
	needed for polymorphic construction and appending of consituents.  
 */
class aggregate_meta_instance_reference_base {
protected:
	/**
		Aggregation mode:
		if true, then is a concatenation of arrays producing
		an array of the same number of dimensions, 
		else is a *construction* of a higher dimension array
		from subinstances.  
	 */
	bool					_is_concatenation;

	aggregate_meta_instance_reference_base() { }
public:
virtual	~aggregate_meta_instance_reference_base() { }

	void
	set_concatenation_mode(void) { _is_concatenation = true; }

	void
	set_construction_mode(void) { _is_concatenation = false; }

virtual	good_bool
	append_meta_instance_reference(
		const count_ptr<const meta_instance_reference_base>&) = 0;

};	// end class aggregate_meta_instance_reference_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_AGGREGATE_META_INSTANCE_REFERENCE_BASE_H__

