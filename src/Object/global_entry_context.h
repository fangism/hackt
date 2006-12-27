/**
	\file "Object/global_entry_context.h"
	Structure containing all the minimal information
	needed for a global_entry traversal over instances.  
	$Id: global_entry_context.h,v 1.3.42.2 2006/12/27 06:01:32 fang Exp $
 */

#ifndef	__HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_H__
#define	__HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_H__

#include <iosfwd>
#include "util/NULL.h"
#include "util/size_t.h"
#include "util/member_saver.h"

namespace HAC {
namespace entity {
class module;
class footprint;
class footprint_frame;
class state_manager;
struct bool_tag;
template <class> class footprint_frame_map;
using std::ostream;
using util::member_saver;

//=============================================================================
/**
	This structure contains references to the structures required
	to perform a meaninful traversal of the instance hierarchy, 
	such as for cflatting, or allocating global expressions.  
 */
class global_entry_context_base {
	typedef	global_entry_context_base	this_type;
protected:
	/**
		Top-level state manager.
	 */
	const state_manager*			sm;
	/**
		Top-level footprint for global lookups.  
	 */
	const footprint*			topfp;

public:
	/**
		Sets the footprint and state_manager pointers of the 
		global_entry_context_base for the duration of the scope.  
	 */
	class module_setter :
		public member_saver<this_type, 
			const state_manager*, &global_entry_context_base::sm>,
		public member_saver<this_type, 
			const footprint*, &global_entry_context_base::topfp> {
		typedef	member_saver<this_type, 
			const state_manager*, &global_entry_context_base::sm>
				manager_saver_type;
		typedef member_saver<this_type, 
			const footprint*, &global_entry_context_base::topfp>
				footprint_saver_type;
	public:
		module_setter(global_entry_context_base&, const module&);
		~module_setter();
	} __ATTRIBUTE_UNUSED__ ;	// end class module setter

public:
	global_entry_context_base() : sm(NULL), topfp(NULL) { }
	global_entry_context_base(const state_manager& _sm, 
		const footprint& _fp) : sm(&_sm), topfp(&_fp) { }
	// default destructor

	const state_manager*
	get_state_manager(void) const { return sm; }

	const footprint*
	get_top_footprint_ptr(void) const { return topfp; }

};	// end struct global_entry_context_base

//-----------------------------------------------------------------------------
/**
	Add a footprint frame when recursively traversing substructures
	such as processes.  
 */
class global_entry_context : public global_entry_context_base {
	typedef	global_entry_context		this_type;
protected:
	/**
		Local footprint frame.  
		Use fpf->_footprint for local-to-global index translation.  
	 */
	const footprint_frame*			fpf;

public:
	/**
		Sets the footprint_frame for the duration of the scope.  
	 */
	class footprint_frame_setter :
		public member_saver<this_type,
			const footprint_frame*, &global_entry_context::fpf> {
		// global_entry_context&			cc;
		typedef member_saver<this_type,
			const footprint_frame*, &global_entry_context::fpf>
						frame_saver_type;
	public:
		footprint_frame_setter(global_entry_context&,
			const footprint_frame&);
		~footprint_frame_setter();
	} __ATTRIBUTE_UNUSED__ ;	// end class footprint_frame_setter

public:
	global_entry_context() : global_entry_context_base(), fpf(NULL) { }

	global_entry_context(const state_manager& s, const footprint& _fp, 
		const footprint_frame* const ff) : 
		global_entry_context_base(s, _fp), fpf(ff) { }

	const footprint_frame*
	get_footprint_frame(void) const { return fpf; }

	template <class Tag>
	const footprint_frame_map<Tag>&
	get_frame_map(void) const { return fpf; }

	template <class Tag>
	size_t
	lookup_global_id(const size_t) const;

};	// end struct global_entry_context

//-----------------------------------------------------------------------------
/**
	Doesn't need the footprint_frame pointer because we're not
	traversing the hierarchy.  
 */
struct global_entry_dumper : public global_entry_context_base {
public:
	using global_entry_context_base::sm;
	using global_entry_context_base::topfp;

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

