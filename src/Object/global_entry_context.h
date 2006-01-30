/**
	\file "Object/global_entry_context.h"
	Structure containing all the minimal information
	needed for a global_entry traversal over instances.  
	$Id: global_entry_context.h,v 1.2 2006/01/30 07:41:58 fang Exp $
 */

#ifndef	__HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_H__
#define	__HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_H__

#include <iosfwd>
#include "util/NULL.h"
#include "util/size_t.h"

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
	such as for global_entryting, or allocating global expressions.  
 */
class global_entry_context_base {
protected:
	const state_manager*			sm;
	const footprint*			fp;

public:
	/**
		Sets the footprint and state_manager pointers of the 
		global_entry_context_base for the duration of the scope.  
	 */
	class module_setter {
		global_entry_context_base&		ccb;
	public:
		module_setter(global_entry_context_base&, const module&);
		~module_setter();
	};	// end class module setter

public:
	global_entry_context_base() : sm(NULL), fp(NULL) { }
	global_entry_context_base(const state_manager& _sm, 
		const footprint& _fp) : sm(&_sm), fp(&_fp) { }
	// default destructor

};	// end struct global_entry_context_base

//-----------------------------------------------------------------------------
/**
	Add a footprint frame when recursively traversing substructures
	such as processes.  
 */
class global_entry_context : public global_entry_context_base {
protected:
	const footprint_frame*			fpf;

public:
	/**
		Sets the footprint_frame for the duration of the scope.  
	 */
	class footprint_frame_setter {
		global_entry_context&			cc;
	public:
		footprint_frame_setter(global_entry_context&,
			const footprint_frame&);
		~footprint_frame_setter();
	};	// end class footprint_frame_setter

public:
	global_entry_context() : fpf(NULL) { }

	template <class Tag>
	const footprint_frame_map<Tag>&
	get_frame_map(void) const { return fpf; }

};	// end struct global_entry_context

//-----------------------------------------------------------------------------
/**
	Doesn't need the footprint_frame pointer because we're not
	traversing the hierarchy.  
 */
struct global_entry_dumper : public global_entry_context_base {
public:
	using global_entry_context_base::sm;
	using global_entry_context_base::fp;

	ostream&				os;
	size_t					index;

	global_entry_dumper(ostream& _o, const state_manager& _sm, 
		const footprint& _fp) :
		global_entry_context_base(_sm, _fp), os(_o), index(0) { }

};	// end struct global_entry_dumper

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_H__

