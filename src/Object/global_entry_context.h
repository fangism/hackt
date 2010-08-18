/**
	\file "Object/global_entry_context.h"
	Structure containing all the minimal information
	needed for a global_entry traversal over instances.  
	$Id: global_entry_context.h,v 1.9.2.1 2010/08/18 23:39:39 fang Exp $
 */

#ifndef	__HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_H__
#define	__HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_H__

#include <iosfwd>
#include <utility>
#include "util/NULL.h"
#include "util/size_t.h"
#include "util/member_saver.h"
#include "Object/traits/classification_tags_fwd.h"
#include "Object/ref/reference_enum.h"	// for global_indexed_reference
#include "util/tokenize_fwd.h"		// for string_list

namespace util {
template <class, class>
class tree_cache;
}

namespace HAC {
namespace entity {
class module;
class unroll_context;
class footprint;
class footprint_frame;
class state_manager;
struct global_offset;
template <class> class state_instance;
class meta_instance_reference_base;
using util::string_list;
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
	// need some replacement, nope
	/**
		Top-level footprint for global lookups.  
		This really should be reference, since it is required
		and immutable.
	 */
	const footprint*			topfp;

public:
	// requires top-footprint
	explicit
	global_entry_context_base(const footprint& _fp) :
		topfp(&_fp) { }
	// default destructor

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
public:
	/**
		Local footprint frame.  
		Use fpf->_footprint for local-to-global index translation.  
	 */
	const footprint_frame*			fpf;
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

public:
	global_entry_context(const footprint_frame&, const global_offset&);

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

	const footprint_frame*
	get_footprint_frame(void) const { return fpf; }

	template <class Tag>
	const footprint_frame_map<Tag>&
	get_frame_map(void) const { return fpf; }

	const footprint&
	get_current_footprint(void) const;

	// return read-only
	const global_offset*
	get_offset(void) const { return g_offset; }

	// param is a local or global index, depending on context
	template <class Tag>
	size_t
	lookup_global_id(const size_t) const;

	// note: also covers member_meta_instance_references
	template <class Tag>
	size_t
	lookup_meta_reference_global_index(
		const simple_meta_instance_reference<Tag>&, 
		const unroll_context* = NULL) const;

	typedef	std::pair<footprint_frame, global_offset>
					cache_entry_type;
	typedef	util::tree_cache<size_t, cache_entry_type>
					frame_cache_type;

	void
	construct_global_footprint_frame(footprint_frame&, 
		global_offset&, size_t pid) const;

	const cache_entry_type&
	lookup_global_footprint_frame_cache(size_t pid,
		frame_cache_type*) const;

	// \return lpid of returned process frame, 0 on error
	size_t
	construct_global_footprint_frame(footprint_frame&, 
		footprint_frame&, 
		global_offset&, const meta_instance_reference_base&) const;

	// e.g. use this after a cache-lookup
	void
	set_global_context(const cache_entry_type& c);

};	// end struct global_entry_context

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_H__

