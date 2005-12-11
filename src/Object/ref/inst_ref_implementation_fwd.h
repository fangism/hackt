/**
	\file "Object/ref/inst_ref_implementation_fwd.h"
	Forward declarations of instance reference implementation policies.  
	$Id: inst_ref_implementation_fwd.h,v 1.2.34.1 2005/12/11 00:45:44 fang Exp $
 */

#ifndef	__OBJECT_REF_INST_REF_IMPLEMENTATION_H__
#define	__OBJECT_REF_INST_REF_IMPLEMENTATION_H__

namespace HAC {
namespace entity {

//=============================================================================
/**
	Substructure-dependent implementation functions.  
 */
template <bool HasSubstructure>
struct simple_meta_instance_reference_implementation;
template <>
struct simple_meta_instance_reference_implementation<true>;
template <>
struct simple_meta_instance_reference_implementation<false>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_REF_INST_REF_IMPLEMENTATION_H__

