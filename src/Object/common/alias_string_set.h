/**
	\file "Object/common/alias_string_set.h"
	$Id: alias_string_set.h,v 1.1.2.1 2005/09/16 07:19:36 fang Exp $
 */

#ifndef	__OBJECT_COMMON_ALIAS_STRING_SET_H__
#define	__OBJECT_COMMON_ALIAS_STRING_SET_H__

#include <iosfwd>
#include <string>
#include <list>

namespace ART {
namespace entity {
using std::string;
using std::list;
using std::ostream;

typedef	list<list<string> >		string_list_list_type;

//=============================================================================
/**
	Contains a list of set of strings.  
	From front to back, the level of hierarchy goes deeper.  
 */
class alias_string_set : public string_list_list_type {
	typedef	string_list_list_type	parent_type;
public:
	alias_string_set();
	~alias_string_set();

	void
	push_back(void);

	ostream&
	dump(ostream&) const;

};	// end class alias_string_set

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_COMMON_ALIAS_STRING_SET_H__

