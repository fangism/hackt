/**
	\file "Object/inst/substructure_alias_fwd.hh"
	$Id: substructure_alias_fwd.hh,v 1.4 2006/01/22 18:20:13 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_SUBSTRUCTURE_ALIAS_FWD_H__
#define	__HAC_OBJECT_INST_SUBSTRUCTURE_ALIAS_FWD_H__

namespace HAC {
namespace entity {
//=============================================================================
/**
	Template class for alias info base classes.  
 */
template <bool HasSubstructure> class substructure_alias_base;

// forward declaration of specializations
template <> class substructure_alias_base<true>;
template <> class substructure_alias_base<false>;

// typedefs of specializations
typedef	substructure_alias_base<true>		substructure_alias;
typedef	substructure_alias_base<false>		terminal_alias;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC


#endif	// __HAC_OBJECT_INST_SUBSTRUCTURE_ALIAS_FWD_H__

