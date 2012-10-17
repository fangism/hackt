/**
	\file "Object/ref/inst_ref_implementation_fwd.hh"
	Forward declarations of instance reference implementation policies.  
	$Id: inst_ref_implementation_fwd.hh,v 1.5 2006/01/22 18:20:20 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_INST_REF_IMPLEMENTATION_FWD_H__
#define	__HAC_OBJECT_REF_INST_REF_IMPLEMENTATION_FWD_H__

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

#endif	// __HAC_OBJECT_REF_INST_REF_IMPLEMENTATION_FWD_H__

