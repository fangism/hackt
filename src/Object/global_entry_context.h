/**
	\file "Object/global_entry_context.h"
	Structure containing all the minimal information
	needed for a global_entry traversal over instances.  
	$Id: global_entry_context.h,v 1.6.46.1 2010/01/09 03:29:57 fang Exp $
 */

#ifndef	__HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_H__
#define	__HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_H__

#include <iosfwd>
#include "util/NULL.h"
#include "util/size_t.h"
#include "util/member_saver.h"
#include "Object/devel_switches.h"

namespace HAC {
namespace entity {
class module;
class footprint;
class footprint_frame;
class state_manager;
struct bool_tag;
template <class> class footprint_frame_map;
template <class> class simple_meta_instance_reference;
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
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	// need some replacement
#else
	/**
		Top-level state manager.
	 */
	const state_manager*			sm;
#endif
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
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		public member_saver<this_type, 
			const state_manager*, &global_entry_context_base::sm>,
#endif
		public member_saver<this_type, 
			const footprint*, &global_entry_context_base::topfp> {
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		typedef	member_saver<this_type, 
			const state_manager*, &global_entry_context_base::sm>
				manager_saver_type;
#endif
		typedef member_saver<this_type, 
			const footprint*, &global_entry_context_base::topfp>
				footprint_saver_type;
	public:
		module_setter(global_entry_context_base&, const module&);
		~module_setter();
	} __ATTRIBUTE_UNUSED__ ;	// end class module setter

public:
	global_entry_context_base() :
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		sm(NULL),
#endif
		topfp(NULL) { }
	explicit
	global_entry_context_base(
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		const state_manager& _sm, 
#endif
		const footprint& _fp) :
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		sm(&_sm),
#endif
		topfp(&_fp) { }
	// default destructor

#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	const state_manager*
	get_state_manager(void) const { return sm; }
#endif

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
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		footprint_frame_setter(global_entry_context&,
			const size_t pid);
#endif
		~footprint_frame_setter();
	} __ATTRIBUTE_UNUSED__ ;	// end class footprint_frame_setter

public:
	global_entry_context() : global_entry_context_base(), fpf(NULL) { }

	global_entry_context(
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		const state_manager& s,
#endif
		const footprint& _fp, 
		const footprint_frame* const ff = NULL) : 
		global_entry_context_base(
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
			s,
#endif
			_fp), fpf(ff) { }

	const footprint_frame*
	get_footprint_frame(void) const { return fpf; }

	template <class Tag>
	const footprint_frame_map<Tag>&
	get_frame_map(void) const { return fpf; }

	// param is a local or global index, depending on context
	template <class Tag>
	size_t
	lookup_global_id(const size_t) const;

	// note: also covers member_meta_instance_references
	template <class Tag>
	size_t
	lookup_meta_reference_global_index(
		const simple_meta_instance_reference<Tag>&) const;
	

};	// end struct global_entry_context

//-----------------------------------------------------------------------------
/**
	Doesn't need the footprint_frame pointer because we're not
	traversing the hierarchy.  
 */
struct global_entry_dumper : public global_entry_context_base {
public:
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	using global_entry_context_base::sm;
#endif
	using global_entry_context_base::topfp;

	ostream&				os;
	size_t					index;

	global_entry_dumper(ostream& _o,
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		const state_manager& _sm, 
#endif
		const footprint& _fp) :
		global_entry_context_base(
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
			_sm,
#endif
			_fp), os(_o), index(0) { }

};	// end struct global_entry_dumper

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_H__

