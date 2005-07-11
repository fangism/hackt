/**
	\file "Object/inst/substructure_alias_fwd.h"
	$Id: substructure_alias_fwd.h,v 1.1.2.1 2005/07/11 20:19:25 fang Exp $
 */

#ifndef	__OBJECT_INST_SUBSTRUCTURE_ALIAS_FWD_H__
#define	__OBJECT_INST_SUBSTRUCTURE_ALIAS_FWD_H__

namespace ART {
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
}	// end namespace ART


#endif	// __OBJECT_INST_SUBSTRUCTURE_ALIAS_FWD_H__

