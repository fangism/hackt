/**
	\file "Object/ref/data_nonmeta_instance_reference.h"
	$Id: data_nonmeta_instance_reference.h,v 1.2.16.2 2006/06/05 04:02:50 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_DATA_NONMETA_INSTANCE_REFERENCE_H__
#define	__HAC_OBJECT_REF_DATA_NONMETA_INSTANCE_REFERENCE_H__

#include "Object/ref/nonmeta_instance_reference_base.h"
#include "Object/devel_switches.h"
#include "util/memory/count_ptr.h"

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

virtual	count_ptr<const data_type_reference>
	get_data_type_ref(void) const = 0;

virtual	bool
	is_lvalue(void) const = 0;

#if COW_UNROLL_DATA_EXPR
#define	UNROLL_RESOLVE_COPY_NONMETA_DATA_REFERENCE_PROTO		\
	count_ptr<const data_nonmeta_instance_reference>		\
	unroll_resolve_copy(const unroll_context&, 			\
		const count_ptr<const data_nonmeta_instance_reference>&) const

	UNROLL_RESOLVE_COPY_NONMETA_DATA_REFERENCE_PROTO;
#endif
};	// end class data_nonmeta_instance_reference

//=============================================================================
}	// end namespace entity
}	// end namespace HAC


#endif	// __HAC_OBJECT_REF_DATA_NONMETA_INSTANCE_REFERENCE_H__

