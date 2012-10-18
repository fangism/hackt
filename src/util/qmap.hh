/**
	\file "util/qmap.hh"
	Queryable map with non-modifying (const) lookup method.  
	Non-modifying lookup guarantees that no extraneous empty
	elements are added by lookup.  
	$Id: qmap.hh,v 1.12 2006/05/08 06:12:14 fang Exp $
 */

#ifndef	__UTIL_QMAP_H__
#define	__UTIL_QMAP_H__

#include "util/macros.h"
#include "util/qmap_fwd.hh"		// forward declarations only
#include <map>
#include "util/const_assoc_query.hh"

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
	typedef	const_assoc_query<parent_type>		impl_type;
public:
	// convenient types used in clean(), all other inherited naturally
	typedef	typename parent_type::mapped_type	mapped_type;   
	typedef	typename parent_type::iterator		iterator;
	typedef	typename parent_type::const_iterator	const_iterator;

	/**
		Strict C++ requires that a default constructor be 
		explicitly declared to be const-initializable.  
		(Not caught by g++, but caught by icc.)
	 */
	qmap() : impl_type() { }

	void
	clean(void);

	// EVERYTHING else is inherited
};	// end class qmap

//-----------------------------------------------------------------------------
}	// end namespace util

#endif	// __UTIL_QMAP_H__

