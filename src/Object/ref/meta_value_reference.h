/**
	\file "Object/ref/meta_value_reference.h"
	$Id: meta_value_reference.h,v 1.1.4.2 2006/02/19 03:53:10 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_META_VALUE_REFERENCE_H__
#define	__HAC_OBJECT_REF_META_VALUE_REFERENCE_H__

#include "Object/ref/meta_value_reference_base.h"
#include "Object/traits/class_traits_fwd.h"

namespace HAC {
namespace entity {

//=============================================================================
#define	META_VALUE_REFERENCE_TEMPLATE_SIGNATURE	template <class Tag>
#define	META_VALUE_REFERENCE_CLASS		meta_value_reference<Tag>

/**
	Base class for simple and aggregate (member?)
	parameter value references.  
 */
META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
class meta_value_reference :
		public meta_value_reference_base, 
		public class_traits<Tag>::expr_base_type {
	typedef	META_VALUE_REFERENCE_CLASS	this_type;
	typedef	class_traits<Tag>		traits_type;
public:
	typedef	typename traits_type::expr_base_type
						expr_base_type;
	typedef	expr_base_type			interface_type;
protected:
	meta_value_reference() :
		meta_value_reference_base(), expr_base_type() { }
public:
virtual	~meta_value_reference() { }

};	// end class meta_value_reference

//=============================================================================
}	// end namespace entity
}	// end namespace HAC


#endif	// __HAC_OBJECT_REF_META_VALUE_REFERENCE_H__
