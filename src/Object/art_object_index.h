/**
	\file "art_object_index.h"
	Typedef for index types.  
	$Id: art_object_index.h,v 1.1.2.1 2005/01/20 06:46:53 fang Exp $
 */

#ifndef	__ART_OBJECT_INDEX_H__
#define	__ART_OBJECT_INDEX_H__

#include "multikey_fwd.h"
#include "art_object_fwd.h"	// for pint/pbool_value_type

namespace ART {
namespace entity {

typedef	util::multikey_base<pint_value_type>		multikey_index_type;
typedef	util::multikey_generator_base<pint_value_type>	index_generator_type;

}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INDEX_H__

