/**
	\file "util/qmap.h"
	Queryable map with non-modifying (const) lookup method.  
	Non-modifying lookup guarantees that no extraneous empty
	elements are added by lookup.  
	$Id: qmap.h,v 1.10.54.1 2006/01/18 06:25:08 fang Exp $
 */

#ifndef	__UTIL_QMAP_H__
#define	__UTIL_QMAP_H__

#include "util/macros.h"
#include "util/qmap_fwd.h"		// forward declarations only
#include <map>
#include "util/const_assoc_query.h"

namespace util {
using std::map;
using std::pair;

/**
	Extension of Standard Template Library's map container.  
	Adds an lookup operator with constant semantics for querying
	whether or not something is in the map, but without modifying it.  
	Useful for maps of pointers and pointer classes.  
 */
QMAP_TEMPLATE_SIGNATURE
class qmap : public const_assoc_query<map<K,T,C,A> > {
private:
	typedef	map<K,T,C,A>				parent_type;
public:
	// convenient types used in clean(), all other inherited naturally
        typedef typename parent_type::mapped_type	mapped_type;   
	typedef typename parent_type::iterator		iterator;
	typedef typename parent_type::const_iterator	const_iterator;


	void
	clean(void);

	// EVERYTHING else is inherited
};	// end class qmap

//-----------------------------------------------------------------------------
}	// end namespace util

#endif	// __UTIL_QMAP_H__

