// "multikey_map.h"

#ifndef	__MULTIKEY_MAP_FWD_H__
#define	__MULTIKEY_MAP_FWD_H__

#ifndef	MULTIKEY_MAP_NAMESPACE
#define	MULTIKEY_MAP_NAMESPACE		multikey_map_namespace
#endif

#include <map>

namespace MULTIKEY_MAP_NAMESPACE {
using namespace std;

//=============================================================================
template <size_t D,
	class K = int,
	class T = char*,
	template <class, class> class M = map >
	class multikey_map;

//=============================================================================
}	// end namespace MULTIKEY_MAP_NAMESPACE

#endif	//	__MULTIKEY_MAP_FWD_H__

