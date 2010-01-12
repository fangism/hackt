/**
	"Object/common/cflat_args.h"
	Common aggregate argument types for various cflat methods.  
	$Id: cflat_args.h,v 1.7.124.1 2010/01/12 02:48:43 fang Exp $
 */

#ifndef	__HAC_OBJECT_COMMON_CFLAT_ARGS_H__
#define	__HAC_OBJECT_COMMON_CFLAT_ARGS_H__

#include <iosfwd>
#include <string>
#include "util/member_saver.h"
#include "Object/devel_switches.h"

namespace HAC {
class cflat_options;
namespace entity {
using std::string;
using std::ostream;
class footprint;
class footprint_frame;
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
class state_manager;
#endif
struct wire_alias_set;	// defined in "Object/common/alias_string_cache.h"

//=============================================================================
/**
	The base structure for traversals of the name object (alias)
	hierarchy.  
	Walkers should be based on this...
 */
struct cflat_args_base {
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	/**
		The state manager contains the information about the
		globally allocated unique instances, 
		including footprint frames.  
	 */
	const state_manager&		sm;
#endif
	/**
		Should be the top-level footprint belonging to the module, 
		needed for dumping correct canonical name.  
	 */
	const footprint&		topfp;
	/**
		If this is NULL, then we are at top-level.  
		Is a never-delete pointer.  
		The footprint frame maps the local aliases to globally
		allocated unique instances.  
	 */
	const footprint_frame*		fpf;
public:
	cflat_args_base(
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
			const state_manager& _sm, 
#endif
			const footprint& _f,
			const footprint_frame* const _fpf) : 
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
			sm(_sm),
#endif
			topfp(_f), fpf(_fpf) { }

	typedef	util::member_saver<cflat_args_base, const footprint_frame*, 
				&cflat_args_base::fpf>
					save_frame;

};	// end struct cflat_args_base

//=============================================================================
/**
	Argument type for cflat_aliases methods, merely for convenience.  
	Reference members never change.  
 */
struct cflat_aliases_arg_type : public cflat_args_base {
	ostream&			o;
	/**
		cflat mode and style flags.  
	 */
	const cflat_options&		cf;
	/**
		\pre is already sized properly to accomodate total
			number of allocated bool nodes.  
	 */
	wire_alias_set&			wires;
	/**
		Cumulative hierarchical name (top-down).  
		The prefix grows with each level of instance hierarchy.  
	 */
	string				prefix;
public:
	cflat_aliases_arg_type(ostream& _o,
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
			const state_manager& _sm, 
#endif
			const footprint& _f, 
			const footprint_frame* const _fpf, 
			const cflat_options& _cf,
			wire_alias_set& _w,
			const string& _p = string()) :
			cflat_args_base(
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
				_sm,
#endif
				_f, _fpf), 
			o(_o),
			cf(_cf),
			wires(_w), 
			prefix(_p) {
	}
	// default copy-constructor
	// default destructor

	typedef	util::member_saver<cflat_aliases_arg_type, string, 
			&cflat_aliases_arg_type::prefix>
				save_prefix;

};	// end struct cflat_aliases_arg_type

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_COMMON_CFLAT_ARGS_H__

