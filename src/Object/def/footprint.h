/**
	\file "Object/def/footprint.h"
	Data structure for each complete type's footprint template.  
	$Id: footprint.h,v 1.23.8.4 2007/09/04 04:34:05 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEF_FOOTPRINT_H__
#define	__HAC_OBJECT_DEF_FOOTPRINT_H__

#include <iosfwd>
#include "Object/def/footprint_base.h"
#include "Object/inst/port_alias_tracker.h"
// #include "Object/inst/alias_visitee.h"
#include "Object/inst/collection_index_entry.h"
#include "Object/devel_switches.h"
#if LOCAL_CHP_EVENT_FOOTPRINT
#include "Object/lang/CHP_footprint.h"
#endif

#include "util/boolean_types.h"
#include "util/string_fwd.h"
#include "util/memory/count_ptr.h"
#include "util/memory/excl_ptr.h"
#include "util/persistent.h"
#include "util/memory/chunk_map_pool_fwd.h"

namespace HAC {
class cflat_options;
namespace entity {
namespace PRS {
	class footprint;
}
namespace SPEC {
	class footprint;
}
namespace CHP {
	class concurrent_actions;
}
class instance_collection_base;
class port_formals_manager;
class scopespace;
class port_member_context;
struct alias_visitor;
struct dump_flags;
struct expr_dump_context;

using std::string;
using util::good_bool;
using util::memory::count_ptr;
using util::memory::excl_ptr;

//=============================================================================
/**
	Manages the unroll and creation information for a particular
	complete definition (type).  
	A 'footprint' will be the scratch pad for playing the sequential
	scope of a complete (process) definition.  
	Seems to be only required by processes.  
	Consider using footprint for module and state management.  

	Q: how does a footprint track hierarchical connections?
	A: needs its own set/copy of the instances contained by 
	the parent definition.  This is done with instance_collection_map.  

	NOTE: future, when partial specializations are introduced, 
	may want a back-reference pointer to the referenced 
	specialization definition.  (FAR far future)

	We implement the alias_visitee accept() interface without
	having to derive from it because the traversal is not polymorphic.  

	CONSIDER: adding definition/canonical type back reference?
 */
class footprint :
	public util::persistent, 
	// public alias_visitee, 	// not needed
	private	footprint_base<process_tag>, 
	private	footprint_base<channel_tag>, 
#if ENABLE_DATASTRUCTS
	private	footprint_base<datastruct_tag>, 
#endif
	private	footprint_base<enum_tag>, 
	private	footprint_base<int_tag>, 
	private	footprint_base<bool_tag>, 
	private	value_footprint_base<pbool_tag>, 
	private	value_footprint_base<pint_tag>, 
	private	value_footprint_base<preal_tag> {
// make accessible to base class
template <class> friend class footprint_base;
template <class> friend class value_footprint_base;
	typedef	footprint			this_type;
public:
	/**
		This must remain an instance_collection_base because
		this manages parameter values as well as physical instances.  
	 */
	typedef	never_ptr<instance_collection_base>
					instance_collection_ptr_type;
private:
	/**
		The information needed to encode which pool to 
		fetch the pointer from.  
	 */
	typedef	collection_index_entry	collection_map_entry_type;
	/**
		The type of map used to maintain local copy of instances.  
		Instances contained herein will have no parent scopespace?
		How do we distinguish formal instances from actuals?
		see NOTES:2005-08-20.
		BTW, using count_ptrs for ease of copy-constructibility.  
		Q: do we need a separate port_formals_manager?
	 */
	typedef	std::map<string, collection_map_entry_type>
					instance_collection_map_type;
	typedef	instance_collection_map_type::const_iterator
					const_instance_map_iterator;
	typedef	instance_collection_map_type::iterator
					instance_map_iterator;
	typedef	footprint_base<process_tag>::instance_pool_type	process_instance_pool_type;
	typedef	footprint_base<channel_tag>::instance_pool_type	channel_instance_pool_type;
#if ENABLE_DATASTRUCTS
	typedef	footprint_base<datastruct_tag>::instance_pool_type
							struct_instance_pool_type;
#endif
	typedef	footprint_base<enum_tag>::instance_pool_type	enum_instance_pool_type;
	typedef	footprint_base<int_tag>::instance_pool_type	int_instance_pool_type;
	typedef	footprint_base<bool_tag>::instance_pool_type	bool_instance_pool_type;
private:
	// state information
	// a place to unroll instances and connections
	// a place to create state pseudo-footprint
	// back-reference to definition?  NO, instance_collection_map suffices
	/**
		Whether or not this definition footprint has been unrolled.
		NOTE: this flag is obsolete since we've fused unrolling and
		creating into a single phase.  
	 */
	bool					unrolled;
	/**
		Whether or not the footprint has created unique state
		(WITHOUT external connectivity context).
	 */
	bool					created;
	/**
		The working space collection of instances.  
		Should be imported from the base definition.  
		This *could* be used by the module itself...
		Then all top-level unrolling would be w.r.t.
			the module's footprint.  
		The footprint itself would become part of the
			unrolling context, an output context
			for where the connection operations should happen.  
		References to the instance_collections would have to be
			resolved into these 'actual' instances contained 
			in this map.  
		Thereby making all instances in the module some sort
			of 'local formals'.  
		One issue would be that the namespaces would be flattened
			into one top-level.  
		The strings (keys) need to be composed to their
			fully-scoped names, which is rather unfortunate.  
		A module lookup of instances would require 
			the flattened names.  
		Another options may to make a scopestring class, 
			a sequence of strings, but hash<> would be interesting.
		Addendum (after NOTES:2005-08-20):
			top-level module scope doesn't need to use this
			it can still use top-level instances collections
			directly without further translation. 
	 */
	instance_collection_map_type		instance_collection_map;

	/**
		Fast lookup map of ALL aliases in this scope.  
	 */
	port_alias_tracker			scope_aliases;
	/**
		This keeps track which port members are internally aliased.
	 */
	port_alias_tracker			port_aliases;

	/**
		The set of unrolled production rules, local to this scope.  
		This is populated during the create phase.  
		Privatized implementation.  
	 */
	excl_ptr<PRS::footprint>		prs_footprint;
	/**
		The CHP footprint type is the same as the source tree's
		IR, but with meta-parameter dependencies resolved
		and substituted.  
	 */
	typedef	CHP::concurrent_actions		chp_footprint_type;
	/**
		Meta-param unrolled footprint of CHP, 
		established during the create phase.  
		Privatized implementation.  
		Now managed by the persistent_object_manager, 
		because this pointer needs to be reconstructed
		for references from the chp_event_footprint.
	 */
	excl_ptr<chp_footprint_type>		chp_footprint;
#if LOCAL_CHP_EVENT_FOOTPRINT
	/**
		Local pool of events in the footprint of this definition.
	 */
	CHP::local_event_footprint		chp_event_footprint;
#endif
	/**
		Unrolled specifications, local to this scope.  
		This is populated during the create phase.  
		Privatized implementation.  
	 */
	excl_ptr<SPEC::footprint>		spec_footprint;
	/**
		Lock to detect recursion and cyclic dependencies.  
	 */
	bool					lock_state;
public:
	// lock manager
	class create_lock {
		footprint&		fp;
	public:
		explicit
		create_lock(footprint& f);

		~create_lock() { fp.lock_state = false; }

	private:
		// non-copy-able
		create_lock(const create_lock&);

		create_lock&
		operator = (const create_lock&);
	};
public:
	footprint();
	~footprint();

	size_t
	map_size(void) const { return instance_collection_map.size(); }

	bool
	is_unrolled(void) const { return unrolled; }

	bool
	is_created(void) const { return created; }

	void
	mark_unrolled(void) { unrolled = true; }

	void
	mark_created(void) { created = true; }

	instance_collection_ptr_type
	operator [] (const string&) const;

	// lookup by index and tag
	instance_collection_ptr_type
	operator [] (const collection_map_entry_type&) const;

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	ostream&
	dump_with_collections(ostream&, const dump_flags&, 
		const expr_dump_context&) const;

	const port_alias_tracker&
	get_port_alias_tracker(void) const {
		return port_aliases;
	}

	const port_alias_tracker&
	get_scope_alias_tracker(void) const {
		return scope_aliases;
	}

	template <class Tag>
	typename state_instance<Tag>::pool_type&
	get_instance_pool(void) {
		return *footprint_base<Tag>::_instance_pool;
	}

	template <class Tag>
	const typename state_instance<Tag>::pool_type&
	get_instance_pool(void) const {
		return *footprint_base<Tag>::_instance_pool;
	}

	template <class Tag>
	instance_collection_pool_bundle<Tag>&
	get_instance_collection_pool_bundle(void) {
		return *footprint_base<Tag>::collection_pool_bundle;
	}

	template <class Tag>
	const instance_collection_pool_bundle<Tag>&
	get_instance_collection_pool_bundle(void) const {
		return *footprint_base<Tag>::collection_pool_bundle;
	}

	template <class Tag>
	value_collection_pool_bundle<Tag>&
	get_value_collection_pool_bundle(void) {
		return *value_footprint_base<Tag>::collection_pool_bundle;
	}

	template <class Tag>
	const value_collection_pool_bundle<Tag>&
	get_value_collection_pool_bundle(void) const {
		return *value_footprint_base<Tag>::collection_pool_bundle;
	}

	void
	import_scopespace(const scopespace&);

	void
	import_scopespace_shallow(const scopespace&);

	void
	import_hierarchical_scopespace(const scopespace&);

	void
	clear_instance_collection_map(void);

	// this is defined in Object/inst/instance_collection.cc
	// to break a cyclic library dependence.  :-/
	void
	register_collection_map_entry(const string&, 
		const collection_map_entry_type&);

	good_bool
	create_dependent_types(const footprint&);

	void
	evaluate_scope_aliases(void);

	PRS::footprint&
	get_prs_footprint(void) { return *prs_footprint; }

	const PRS::footprint&
	get_prs_footprint(void) const { return *prs_footprint; }

	chp_footprint_type&
	get_chp_footprint(void)
#if LOCAL_CHP_EVENT_FOOTPRINT
		;
#else
		{ return *chp_footprint; }
#endif

	const chp_footprint_type&
	get_chp_footprint(void) const { 
#if LOCAL_CHP_EVENT_FOOTPRINT
		NEVER_NULL(chp_footprint);
#endif
		return *chp_footprint;
	}

	bool
	has_chp_footprint(void) const { return chp_footprint; }

	const CHP::local_event_footprint&
	get_chp_event_footprint(void) const { return chp_event_footprint; }

	SPEC::footprint&
	get_spec_footprint(void) { return *spec_footprint; }

	const SPEC::footprint&
	get_spec_footprint(void) const { return *spec_footprint; }

	good_bool
	expand_unique_subinstances(state_manager&) const;

	void
	assign_footprint_frame(footprint_frame&, 
		const port_member_context&) const;

	void
	cflat_aliases(ostream&, const state_manager&,
		const cflat_options&) const;

	// eventually pass parameter for warning control 
	good_bool
	connection_diagnostics(void) const;

#if LOCAL_CHP_EVENT_FOOTPRINT
	void
	allocate_chp_events(void);
#endif

	void
	accept(alias_visitor&) const;

public:
	instance_collection_ptr_type
	read_pointer(istream&) const;

// persistent information management
protected:
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

public:
	void
	collect_transient_info(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);

private:
	/**
		Don't want footprint to be copy-constructed.  
	 */
	explicit
	footprint(const footprint&);
};	// end class footprint

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_DEF_FOOTPRINT_H__

