/**
	\file "Object/cflat_context.h"
	Structure containing all the minimal information
	needed for a cflat traversal over instances.  
	$Id: cflat_context.h,v 1.1.2.4 2006/01/02 23:13:32 fang Exp $
 */

#ifndef	__HAC_OBJECT_CFLAT_CONTEXT_H__
#define	__HAC_OBJECT_CFLAT_CONTEXT_H__

#include <iosfwd>
#include "util/macros.h"

namespace HAC {
namespace entity {
class module;
class footprint;
class footprint_frame;
class state_manager;
struct bool_tag;
template <class> class footprint_frame_map;
using std::ostream;

//=============================================================================
/**
	This structure contains references to the structures required
	to perform a meaninful traversal of the instance hierarchy, 
	such as for cflatting, or allocating global expressions.  
 */
class cflat_context_base {
protected:
	const state_manager*			sm;
	const footprint*			fp;

public:
	/**
		Sets the footprint and state_manager pointers of the 
		cflat_context_base for the duration of the scope.  
	 */
	class module_setter {
		cflat_context_base&		ccb;
	public:
		module_setter(cflat_context_base&, const module&);
		~module_setter();
	};	// end class module setter

public:
	cflat_context_base() : sm(NULL), fp(NULL) { }
	// default destructor

};	// end struct cflat_context_base

//-----------------------------------------------------------------------------
/**
	Add a footprint frame when recursively traversing substructures
	such as processes.  
 */
class cflat_context : public cflat_context_base {
protected:
	const footprint_frame*			fpf;

public:
	/**
		Sets the footprint_frame for the duration of the scope.  
	 */
	class footprint_frame_setter {
		cflat_context&			cc;
	public:
		footprint_frame_setter(cflat_context&, const footprint_frame&);
		~footprint_frame_setter();
	};	// end class footprint_frame_setter

public:
	cflat_context() : fpf(NULL) { }

	template <class Tag>
	const footprint_frame_map<Tag>&
	get_frame_map(void) const { return fpf; }

};	// end struct cflat_context

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_CFLAT_CONTEXT_H__

