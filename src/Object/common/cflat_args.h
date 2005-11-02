/**
	"Object/common/cflat_args.h"
	Common aggregate argument types for various cflat methods.  
	$Id: cflat_args.h,v 1.2 2005/11/02 22:53:45 fang Exp $
 */

#ifndef	__OBJECT_COMMON_CFLAT_ARGS_H__
#define	__OBJECT_COMMON_CFLAT_ARGS_H__

#include <iosfwd>
#include <string>

namespace ART {
class cflat_options;
namespace entity {
using std::string;
using std::ostream;
class footprint;
class footprint_frame;
class state_manager;

//=============================================================================
/**
	Argument type for cflat_aliases methods, merely for convenience.  
	Reference members never change.  
 */
struct cflat_aliases_arg_type {
	ostream&			o;
	const state_manager&		sm;
	/**
		Should be the top-level footprint belonging to the module, 
		needed for dumping correct canonical name.  
	 */
	const footprint&		topfp;
	/**
		If this is NULL, then we are at top-level.  
		Is a never-delete pointer.  
	 */
	const footprint_frame*		fpf;
	const cflat_options&		cf;
	string				prefix;

	cflat_aliases_arg_type(ostream& _o, const state_manager& _sm, 
			const footprint& _f, 
			const footprint_frame* const _fpf, 
			const cflat_options& _cf,
			const string& _p = string()) :
			o(_o), sm(_sm), 
			topfp(_f),
			fpf(_fpf), 
			cf(_cf), prefix(_p) {
	}
	// default copy-constructor
	// default destructor

};	// end struct cflat_aliases_arg_type

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_COMMON_CFLAT_ARGS_H__

