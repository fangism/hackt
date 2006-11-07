/**
	\file "Object/inst/collection_index_entry.h"
	$Id: collection_index_entry.h,v 1.2 2006/11/07 06:34:37 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_COLLECTION_INDEX_ENTRY_H__
#define	__HAC_OBJECT_INST_COLLECTION_INDEX_ENTRY_H__

namespace HAC {
namespace entity {
//=============================================================================
/**
	The information needed to encode which pool to 
	fetch the pointer from.  
	This uniquely identifies a collection belonging to a footprint.  
 */
struct collection_index_entry {
	/**
		To be on the safe side, this should probably be size_t...
	 */
#if 0
	typedef	unsigned short	index_type;
#else
	typedef	unsigned int	index_type;
#endif
	unsigned char		meta_type;
	unsigned char		pool_type;
	index_type		index;		// enough?
};	// end struct collection_index_entry

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_COLLECTION_INDEX_ENTRY_H__

