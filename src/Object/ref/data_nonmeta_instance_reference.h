/**
	\file "Object/ref/data_nonmeta_instance_reference.h"
	$Id: data_nonmeta_instance_reference.h,v 1.1.2.1 2006/03/19 06:14:12 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_DATA_NONMETA_INSTANCE_REFERENCE_H__
#define	__HAC_OBJECT_REF_DATA_NONMETA_INSTANCE_REFERENCE_H__

#include "Object/ref/nonmeta_instance_reference_base.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
class data_expr;
class data_type_reference;
using util::memory::count_ptr;

//=============================================================================
/**
	Base class for all nonmeta references that are classified as
	data references, including meta-parameter references.  
 */
class data_nonmeta_instance_reference :
		public nonmeta_instance_reference_base {
public:
	typedef	nonmeta_instance_reference_base		parent_type;
	data_nonmeta_instance_reference() : parent_type() { }

virtual	~data_nonmeta_instance_reference() { }

#if NONMETA_TYPE_EQUIVALENCE
virtual	bool
	may_accept_expr_type(const data_expr&) const = 0;
#else
virtual	count_ptr<const data_type_reference>
	get_data_type_ref(void) const = 0;
#endif

};	// end class data_nonmeta_instance_reference

//=============================================================================
}	// end namespace entity
}	// end namespace HAC


#endif	// __HAC_OBJECT_REF_DATA_NONMETA_INSTANCE_REFERENCE_H__

