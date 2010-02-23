/**
	\file "Object/global_entry_context.h"
	Structure containing all the minimal information
	needed for a global_entry traversal over instances.  
	$Id: global_entry_context.h,v 1.6.46.7 2010/02/23 22:34:02 fang Exp $
 */

#ifndef	__HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_H__
#define	__HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_H__

#include <iosfwd>
#include "util/NULL.h"
#include "util/size_t.h"
#include "util/member_saver.h"
#include "Object/devel_switches.h"
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
#include "Object/traits/classification_tags_fwd.h"
#endif

namespace HAC {
namespace entity {
class module;
class footprint;
class footprint_frame;
class state_manager;
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
struct global_offset;
template <class> class state_instance;
#endif
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
	// need some replacement, nope
#else
	/**
		Top-level state manager.
	 */
	const state_manager*			sm;
#endif
	/**
		Top-level footprint for global lookups.  
		This really should be reference, since it is required
		and immutable.
	 */
	const footprint*			topfp;

public:
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
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
#endif
public:
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	global_entry_context_base() : sm(NULL), topfp(NULL) { }
#endif

	// requires top-footprint
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

	const footprint&
	get_top_footprint(void) const { return *topfp; }

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
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
public:
#else
protected:
#endif
	/**
		Local footprint frame.  
		Use fpf->_footprint for local-to-global index translation.  
	 */
	const footprint_frame*			fpf;
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	/**
		Offset needed to compute effective global IDs of 
		local instances.
	 */
	const global_offset*			parent_offset;
protected:
	/**
		This is incremented with each local process iterated.
		Should be set by visit_local for traversals that need it.
	 */
	global_offset*				g_offset;
#endif
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
public:
// just use util::value_saver on protected members
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

#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	typedef member_saver<this_type,
		global_offset*, &global_entry_context::g_offset>
					global_offset_setter;
#endif
#endif

public:
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	global_entry_context() : global_entry_context_base(), fpf(NULL)
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
		, parent_offset(NULL), g_offset(NULL)
#endif
		{ }
#endif

#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	global_entry_context(const footprint_frame&, const global_offset&);
#else
	global_entry_context(const state_manager& s, const footprint& _fp) : 
		global_entry_context_base(s, _fp), fpf(&ff) { }
#endif

#if MEMORY_MAPPED_GLOBAL_ALLOCATION
virtual	~global_entry_context();

	ostream&
	dump_context(ostream&) const;

	template <class Tag>
	void
	visit_local(const footprint&, const bool);

#if 0
	void
	visit_aliases(const footprint&);
#endif

	void
	visit_recursive(const footprint&);

public:
	bool
	at_top(void) const;
virtual	void
	visit(const state_instance<process_tag>&);
virtual	void
	visit(const state_instance<channel_tag>&);
virtual	void
	visit(const state_instance<enum_tag>&);
virtual	void
	visit(const state_instance<int_tag>&);
virtual	void
	visit(const state_instance<bool_tag>&);
virtual	void
	visit(const footprint&);
#endif

	const footprint_frame*
	get_footprint_frame(void) const { return fpf; }

	template <class Tag>
	const footprint_frame_map<Tag>&
	get_frame_map(void) const { return fpf; }

#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	const footprint&
	get_current_footprint(void) const;

	// return read-only
	const global_offset*
	get_offset(void) const { return g_offset; }
#endif

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
struct global_entry_dumper :
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
		public global_entry_context	// needs footprint_frame
#else
		public global_entry_context_base
#endif
{
public:
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	typedef	global_entry_context		parent_type;
#else
	typedef	global_entry_context_base	parent_type;
	using global_entry_context_base::sm;
#endif
	using global_entry_context_base::topfp;

	ostream&				os;
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	// global index, 1-based
	size_t					index;
#endif
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	// parent process index (1-based, 0 means top-level)
	size_t					pid;
#endif

	static const char			table_header[];

	global_entry_dumper(ostream& _o,
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		const state_manager& _sm, 
		const footprint& _fp
#else
		const footprint_frame& ff,
		const global_offset& g
#endif
		) :
		parent_type(
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
			ff, g
#else
			_sm, _fp
#endif
			), os(_o)
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
			, index(0)
#else
			, pid(0)
#endif
 { }

#if MEMORY_MAPPED_GLOBAL_ALLOCATION
virtual	~global_entry_dumper();

virtual	void
	visit(const footprint&);
virtual	void
	visit(const state_instance<process_tag>&);
virtual	void
	visit(const state_instance<channel_tag>&);
virtual	void
	visit(const state_instance<enum_tag>&);
virtual	void
	visit(const state_instance<int_tag>&);
virtual	void
	visit(const state_instance<bool_tag>&);

protected:
	template <class Tag>
	void
	__default_visit(const state_instance<Tag>&);
#endif

};	// end struct global_entry_dumper

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
template <class Tag>
struct global_allocation_dumper : public global_entry_dumper {
	global_allocation_dumper(ostream& o,
		const footprint_frame& ff, global_offset& g) :
		global_entry_dumper(o, ff, g) { }

	void
	visit(const footprint&);

	using global_entry_dumper::visit;

};	// end struct global_allocation_dumper
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_H__

