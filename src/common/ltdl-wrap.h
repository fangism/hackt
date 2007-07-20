/**
	"common/ltdl-wrap.h"
	Wrapped interfaces for libltdl.  
	$Id: ltdl-wrap.h,v 1.1.2.1 2007/07/20 21:07:45 fang Exp $
 */

#ifndef	__HAC_COMMON_LTDL_WRAP_H__
#define	__HAC_COMMON_LTDL_WRAP_H__

// relative path given to avoid adding another CPPFLAG
#include "../libltdl/ltdl.h"
// do we ever want to fallback to the system's installed ltdl?
#include "util/attributes.h"
#include "util/type_traits.h"
#include "util/memory/excl_ptr.h"
#include <map>
#include <string>

namespace HAC {
using util::memory::never_ptr;
//=============================================================================
/**
	Class whose sole responsibility is to balance dlinit with dlexit,
	using constructor/destructor duality, thereby guaranteeing
	exception safety and non-leaking.
 */
class ltdl_token {
public:
	ltdl_token() { lt_dlinit(); }
	~ltdl_token() { lt_dlexit(); }

private:
	ltdl_token(const ltdl_token&);

	ltdl_token&
	operator = (const ltdl_token&);

	// also stack-allocate only...
} __ATTRIBUTE_UNUSED__ ;	// end class ltdl_token

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	util::remove_pointer<lt_dlhandle>::type		lt_dlref;
typedef	never_ptr<const lt_dlref>			lt_dlrefptr;
typedef	std::map<std::string, lt_dlrefptr>		ltdl_handle_map_type;

//=============================================================================
}	// end namespace HAC

#endif	// __HAC_COMMON_LTDL_WRAP_H__

