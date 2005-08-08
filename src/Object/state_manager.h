/**
	\file "Object/state_manager.h"
	Declaration for the creation state management facilities.  
	$Id: state_manager.h,v 1.2 2005/08/08 16:51:07 fang Exp $
 */

#ifndef	__OBJECT_STATE_MANAGER_H__
#define	__OBJECT_STATE_MANAGER_H__

#include <iosfwd>
#include "util/persistent_fwd.h"

namespace ART {
namespace entity {
using std::istream;
using std::ostream;
using util::persistent_object_manager;

//=============================================================================
/**
	The manager interface for persistent state.  
	This class is instanceless.
 */
class state_manager {
public:
	static
	ostream&
	dump_state(ostream&);

	static
	void
	collect_state(persistent_object_manager&);

	static
	void
	write_state(const persistent_object_manager&, ostream&);

	static
	void
	load_state(const persistent_object_manager&, istream&);

};	// end class state_manager

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_STATE_MANAGER_H__

