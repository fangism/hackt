// "art_object_IO_fwd.h"
// header file for everything related to persistent object management

// include "hash_specializations.h" before this file for 
//	hash<> specializations to take effect

#ifndef	__ART_OBJECT_IO_FWD__
#define	__ART_OBJECT_IO_FWD__

namespace ART {
namespace entity {
//=============================================================================
// sad: unreliable compiler support for enum forward declarations
// thus, full definition must go here.

/**
	Use these enumerations to lookup which function to call
	to reconstruct an object from a binary file stream.
	Only concrete classes need to register with this.
 */
enum type_index_enum {
	NULL_TYPE,			// reserved = 0
	// can also be used to denote end of object stream

	NAMESPACE_TYPE,
	// more class constants here...

	MAX_TYPE_INDEX_ENUM		// reserved
};

//=============================================================================
// documentation description in "art_object_IO.h"
class persistent_object_manager;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	//	__ART_OBJECT_IO_FWD__

