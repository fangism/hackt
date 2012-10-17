/**
	\file "Object/global_entry_context.hh"
	Structure containing all the minimal information
	needed for a global_entry traversal over instances.  
	$Id: global_entry_context.hh,v 1.14 2011/05/17 21:19:52 fang Exp $
 */

#ifndef	__HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_H__
#define	__HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_H__

#include <iosfwd>
#include <utility>
#include <vector>			// really only need fwd decl.
#include "util/NULL.h"
#include "util/size_t.h"
#include "util/member_saver.hh"
#include "Object/traits/classification_tags_fwd.hh"
#include "Object/ref/reference_enum.hh"	// for global_indexed_reference
#include "Object/devel_switches.hh"

/**
	Define to 1 to keep around global pid in global_entry_context
	as hierarchy is walked.
	Goal: 1
	Cost: little
	Rationale: know which process you're in.
 */
#define	GLOBAL_CONTEXT_GPID		1

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
struct bool_tag;
template <class> class footprint_frame_map;
template <class> class simple_meta_instance_reference;
using std::ostream;
using util::member_saver;
using std::vector;
struct global_process_context;		// from Object/global_entry.h
struct global_process_context_id;	// from Object/global_entry.h
struct global_process_context_ref;	// from Object/global_context_cache.h

/**
	Define to 1 to have reference lookups use footprints'
	context caches.
	Rationale: performance
	Goal: 1
	Status: tested, regression free!
 */
#define	CACHE_REFERENCE_LOOKUP_CONTEXTS		(1 && FOOTPRINT_OWNS_CONTEXT_CACHE)

//=============================================================================
/**
	This structure contains references to the structures required
	to perform a meaninful traversal of the instance hierarchy, 
	such as for cflatting, or allocating global expressions.  
 */
struct global_entry_context_base {
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
struct global_entry_context : public global_entry_context_base {
	typedef	global_entry_context		this_type;
public:
#if CACHE_REFERENCE_LOOKUP_CONTEXTS
	typedef	global_process_context_ref	context_arg_type;
	typedef	global_process_context_ref	context_result_type;
#else
	typedef	global_process_context		context_arg_type;
	typedef	global_process_context_id	context_result_type;
#endif
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
#if GLOBAL_CONTEXT_GPID
	size_t					_gpid;
#endif
public:
	explicit
	global_entry_context(const global_process_context&);

virtual	~global_entry_context();

	ostream&
	dump_context(ostream&) const;

#if GLOBAL_CONTEXT_GPID
	const size_t&
	current_gpid(void) const { return _gpid; }
#endif

	template <class Tag>
	void
	visit_ports(const footprint&);

	template <class Tag>
	void
	visit_local(const footprint&, const bool);

#if 0
	void
	visit_aliases(const footprint&);
#endif

	void
	visit_types(const footprint&);

	void
	visit_recursive_unique(const footprint&);

virtual	void
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

#if FOOTPRINT_OWNS_CONTEXT_CACHE
// call global_context_cache::get_global_context() instead
#else
	void
	construct_global_footprint_frame(
		global_process_context&,
		size_t gpid) const;
#endif

#if AGGREGATE_PARENT_REFS
	// \return true on error
	static
	bool
	construct_global_footprint_frames(
		const footprint& top,
		const meta_instance_reference_base&,
		vector<context_result_type>&);

	static
	bool
	construct_global_footprint_frames(
		const footprint& top,
		const meta_instance_reference_base&,
		const unroll_context&,		// override
		vector<context_result_type>&);

	bool
	construct_global_footprint_frames(
		const meta_instance_reference_base&, 
		const unroll_context&,
		vector<context_result_type>&) const;
#endif

	static
	bool
	construct_global_footprint_frame(
		const footprint& top,
		const meta_instance_reference_base&,
		context_result_type&);

	static
	bool
	construct_global_footprint_frame(
		const footprint& top,
		const meta_instance_reference_base&,
		const unroll_context&,		// override
		context_result_type&);

	bool
	construct_global_footprint_frame(
		const meta_instance_reference_base&, 
		const unroll_context&,
		context_result_type&) const;

private:
	bool
	construct_global_footprint_frame(
		context_arg_type&,
		context_result_type&,
		const meta_instance_reference_base&, 
		const unroll_context&) const;

#if AGGREGATE_PARENT_REFS
	bool
	construct_global_footprint_frames(
		vector<context_arg_type>&,
		vector<context_result_type>&,
		const meta_instance_reference_base&, 
		const unroll_context&) const;
#endif

public:
	// e.g. use this after a cache-lookup
	void
	set_global_context(const global_process_context&);

	void
	report_instantiation_error(ostream&) const;

};	// end struct global_entry_context

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_GLOBAL_ENTRY_CONTEXT_H__

