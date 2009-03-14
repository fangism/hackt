/**
	\file "util/uninitialized.h"
	Utility for annotating explicit uninitialized construction.
	$Id: uninitialized.h,v 1.1 2009/03/14 01:46:24 fang Exp $
 */

#ifndef	__UTIL_UNINITIALIZED_H__
#define	__UTIL_UNINITIALIZED_H__

namespace util {
/**
	Tag uset to document an explicit incomplete construct that
	does NOT initialize all of its members.  
 */
struct uninitialized_tag_type { };
/**
	We declare a global dummy object so that one only needs to
	pass a pre-constructed reference (which will end up being unused.
 */
extern const uninitialized_tag_type	uninitialized_tag;

/***
Example:

class partially_initializable : public base {
	Class1		member1;
	Class2		member2;
public:
	explicit
	partially_initializable(const uninitialized_tag_type&) { }
		// does not initialize ANY members!
};
***/
}	// end namespace util

#endif	// __UTIL_UNINITIALIZED_H__

