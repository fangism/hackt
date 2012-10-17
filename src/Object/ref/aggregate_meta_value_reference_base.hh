/**
	\file "Object/ref/aggregate_meta_value_reference_base.hh"
	$Id: aggregate_meta_value_reference_base.hh,v 1.4 2010/01/03 01:34:41 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_AGGREGATE_META_VALUE_REFERENCE_BASE_H__
#define	__HAC_OBJECT_REF_AGGREGATE_META_VALUE_REFERENCE_BASE_H__

#include "util/memory/pointer_classes_fwd.hh"
#include "Object/ref/aggregate_reference_collection_base.hh"

namespace HAC {
namespace entity {
class param_expr;
using util::memory::count_ptr;

//=============================================================================
/**
	Common interface for all aggregate value references, 
	needed for polymorphic construction and appending of consituents.  
 */
class aggregate_meta_value_reference_base :
		public aggregate_reference_collection_base {
protected:
	aggregate_meta_value_reference_base() { }
public:
virtual	~aggregate_meta_value_reference_base();

virtual	good_bool
	append_meta_value_reference(const count_ptr<const param_expr>&) = 0;

};	// end class aggregate_meta_value_reference_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_AGGREGATE_META_VALUE_REFERENCE_BASE_H__

