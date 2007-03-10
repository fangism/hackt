/**
	\file "Object/ref/data_nonmeta_instance_reference.h"
	$Id: data_nonmeta_instance_reference.h,v 1.7.2.1 2007/03/10 20:32:31 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_DATA_NONMETA_INSTANCE_REFERENCE_H__
#define	__HAC_OBJECT_REF_DATA_NONMETA_INSTANCE_REFERENCE_H__

#include "Object/ref/nonmeta_instance_reference_base.h"
#include "util/memory/count_ptr.h"
#include "Object/type/canonical_type_fwd.h"
#include "Object/ref/reference_set.h"	// for update_reference_array_type

namespace HAC {
namespace entity {
class data_expr;
class data_type_reference;
class unroll_context;
class nonmeta_context_base;
class channel_data_reader;	// from "Object/nonmeta_channel_manipulator.h"
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
	typedef	global_references_set	 		assign_update_arg_type;
	data_nonmeta_instance_reference() : parent_type() { }

virtual	~data_nonmeta_instance_reference() { }

virtual	count_ptr<const data_type_reference>
	get_unresolved_data_type_ref(void) const = 0;

// should match with prototype defined in "Object/expr/data_expr.h"
virtual	canonical_generic_datatype
	get_resolved_data_type_ref(const unroll_context&) const = 0;

virtual	bool
	is_lvalue(void) const = 0;

#define	UNROLL_RESOLVE_COPY_NONMETA_DATA_REFERENCE_PROTO		\
	count_ptr<const data_nonmeta_instance_reference>		\
	unroll_resolve_copy(const unroll_context&, 			\
		const count_ptr<const data_nonmeta_instance_reference>&) const

	UNROLL_RESOLVE_COPY_NONMETA_DATA_REFERENCE_PROTO;

#define	NONMETA_ASSIGN_PROTO						\
	void								\
	nonmeta_assign(const count_ptr<const data_expr>&, 		\
		const nonmeta_context_base&,				\
		assign_update_arg_type&) const

#define	DIRECT_ASSIGN_PROTO						\
	void								\
	direct_assign(const nonmeta_context_base&,			\
		assign_update_arg_type&, channel_data_reader&) const

virtual	NONMETA_ASSIGN_PROTO = 0;
virtual	DIRECT_ASSIGN_PROTO = 0;

};	// end class data_nonmeta_instance_reference

//=============================================================================
}	// end namespace entity
}	// end namespace HAC


#endif	// __HAC_OBJECT_REF_DATA_NONMETA_INSTANCE_REFERENCE_H__

