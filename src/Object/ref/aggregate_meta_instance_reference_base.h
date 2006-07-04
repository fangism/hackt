/**
	\file "Object/ref/aggregate_meta_instance_reference_base.h"
	$Id: aggregate_meta_instance_reference_base.h,v 1.3 2006/07/04 07:26:12 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_AGGREGATE_META_INSTANCE_REFERENCE_BASE_H__
#define	__HAC_OBJECT_REF_AGGREGATE_META_INSTANCE_REFERENCE_BASE_H__

#include "util/memory/pointer_classes_fwd.h"
#include "Object/ref/aggregate_reference_collection_base.h"

namespace HAC {
namespace entity {
class meta_instance_reference_base;
using util::memory::count_ptr;

//=============================================================================
/**
	Common interface for all aggregate value references, 
	needed for polymorphic construction and appending of consituents.  
 */
class aggregate_meta_instance_reference_base :
		public aggregate_reference_collection_base {
protected:

	aggregate_meta_instance_reference_base() { }

	explicit
	aggregate_meta_instance_reference_base(const bool b) :
		aggregate_reference_collection_base(b) { }

public:
virtual	~aggregate_meta_instance_reference_base() { }

virtual	good_bool
	append_meta_instance_reference(
		const count_ptr<const meta_instance_reference_base>&) = 0;

};	// end class aggregate_meta_instance_reference_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_AGGREGATE_META_INSTANCE_REFERENCE_BASE_H__

