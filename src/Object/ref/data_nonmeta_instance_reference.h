/**
	\file "Object/ref/data_nonmeta_instance_reference.h"
	$Id: data_nonmeta_instance_reference.h,v 1.3.8.1.2.1 2006/09/05 03:55:54 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_DATA_NONMETA_INSTANCE_REFERENCE_H__
#define	__HAC_OBJECT_REF_DATA_NONMETA_INSTANCE_REFERENCE_H__

#include "Object/ref/nonmeta_instance_reference_base.h"
#include "util/memory/count_ptr.h"
#include "Object/devel_switches.h"

namespace HAC {
namespace entity {
class data_expr;
class data_type_reference;
class unroll_context;
using util::memory::count_ptr;

//=============================================================================
/**
	Base class for all nonmeta references that are classified as
	data references, including meta-parameter references.  
 */
class data_nonmeta_instance_reference :
		public nonmeta_instance_reference_base {
private:
	typedef	data_nonmeta_instance_reference		this_type;
public:
	typedef	nonmeta_instance_reference_base		parent_type;
	data_nonmeta_instance_reference() : parent_type() { }

virtual	~data_nonmeta_instance_reference() { }

#if USE_UNRESOLVED_DATA_TYPES
virtual	count_ptr<const data_type_reference>
	get_unresolved_data_type_ref(void) const = 0;
#endif

#if USE_RESOLVED_DATA_TYPES
// should be canonical data type
virtual	count_ptr<const data_type_reference>
	get_resolved_data_type_ref(void) const = 0;
#endif

virtual	bool
	is_lvalue(void) const = 0;

#define	UNROLL_RESOLVE_COPY_NONMETA_DATA_REFERENCE_PROTO		\
	count_ptr<const data_nonmeta_instance_reference>		\
	unroll_resolve_copy(const unroll_context&, 			\
		const count_ptr<const data_nonmeta_instance_reference>&) const

	UNROLL_RESOLVE_COPY_NONMETA_DATA_REFERENCE_PROTO;
};	// end class data_nonmeta_instance_reference

//=============================================================================
}	// end namespace entity
}	// end namespace HAC


#endif	// __HAC_OBJECT_REF_DATA_NONMETA_INSTANCE_REFERENCE_H__

