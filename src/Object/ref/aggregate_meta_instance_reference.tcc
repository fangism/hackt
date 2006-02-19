/**
	\file "Object/ref/aggregate_meta_instance_reference.tcc"
	Implementation of aggregate_meta_instance_reference class.  
	$Id: aggregate_meta_instance_reference.tcc,v 1.1.2.2 2006/02/19 06:09:03 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_AGGREGATE_META_INSTANCE_REFERENCE_TCC__
#define	__HAC_OBJECT_REF_AGGREGATE_META_INSTANCE_REFERENCE_TCC__

#include <iostream>
#include "Object/ref/aggregate_meta_instance_reference.h"

namespace HAC {
namespace entity {
//=============================================================================
// class aggregate_meta_instance_reference method definitions

AGGREGATE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
AGGREGATE_META_INSTANCE_REFERENCE_CLASS::aggregate_meta_instance_reference() :
		aggregate_meta_instance_reference_base(), 
		parent_type(), 
		subreferences() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
AGGREGATE_META_INSTANCE_REFERENCE_CLASS::aggregate_meta_instance_reference(
		const bool c) :
		aggregate_meta_instance_reference_base(c), 
		parent_type(), 
		subreferences() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
AGGREGATE_META_INSTANCE_REFERENCE_CLASS::
		~aggregate_meta_instance_reference() { }

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_AGGREGATE_META_INSTANCE_REFERENCE_TCC__

