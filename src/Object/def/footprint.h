/**
	\file "Object/def/footprint.h"
	Data structure for each complete type's footprint template.  
	$Id: footprint.h,v 1.3.2.3 2005/10/04 21:24:21 fang Exp $
 */

#ifndef	__OBJECT_DEF_FOOTPRINT_H__
#define	__OBJECT_DEF_FOOTPRINT_H__

#include <iosfwd>
#include "Object/inst/instance_pool.h"
#include "Object/traits/classification_tags.h"

#include "Object/inst/process_instance.h"
#include "Object/inst/channel_instance.h"
#include "Object/inst/struct_instance.h"
#include "Object/inst/enum_instance.h"
#include "Object/inst/int_instance.h"
#include "Object/inst/bool_instance.h"
#include "Object/inst/port_alias_tracker.h"
#include "Object/devel_switches.h"
#if USE_PRS_FOOTPRINT
#include "Object/lang/PRS_footprint.h"
#endif

#include "util/boolean_types.h"
#include "util/persistent_fwd.h"
#include "util/string_fwd.h"
#include "util/hash_qmap.h"
#include "util/memory/count_ptr.h"

namespace ART {
namespace entity {
class instance_collection_base;
class port_formals_manager;
class scopespace;
class state_manager;
class footprint_frame;
class port_member_context;
using std::string;
using std::istream;
using std::ostream;
using util::good_bool;
using util::memory::count_ptr;
using util::persistent_object_manager;

//=============================================================================
template <class Tag>
class footprint_base {
protected:
	typedef	typename state_instance<Tag>::pool_type	pool_type;
private:
	typedef	typename pool_type::const_iterator	const_iterator;
protected:
	pool_type					_pool;

	footprint_base();
	~footprint_base();

	good_bool
	__allocate_global_state(state_manager&) const;

	good_bool
	__expand_unique_subinstances(const footprint_frame&,
		state_manager&, const size_t) const;

};	// end class footprint_base

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

	CONSIDER: adding definition/canonical type back reference?
 */
class footprint :
	private	footprint_base<process_tag>, 
	private	footprint_base<channel_tag>, 
	private	footprint_base<datastruct_tag>, 
	private	footprint_base<enum_tag>, 
	private	footprint_base<int_tag>, 
	private	footprint_base<bool_tag> {
private:
	/**
		This must remain an instance_collection_base because
		this manages parameter values as well as physical instances.  
	 */
	typedef	count_ptr<instance_collection_base>
					instance_collection_ptr_type;
	/**
		The type of map used to maintain local copy of instances.  
		Instances contained herein will have no parent scopespace?
		How do we distinguish formal instances from actuals?
		see NOTES:2005-08-20.
		BTW, using count_ptrs for ease of copy-constructibility.  
		Q: do we need a separate port_formals_manager?
	 */
	typedef	util::hash_qmap<string, instance_collection_ptr_type>
					instance_collection_map_type;
	typedef	instance_collection_map_type::const_iterator
					const_instance_map_iterator;
	typedef	footprint_base<process_tag>::pool_type	process_pool_type;
	typedef	footprint_base<channel_tag>::pool_type	channel_pool_type;
	typedef	footprint_base<datastruct_tag>::pool_type
							struct_pool_type;
	typedef	footprint_base<enum_tag>::pool_type	enum_pool_type;
	typedef	footprint_base<int_tag>::pool_type	int_pool_type;
	typedef	footprint_base<bool_tag>::pool_type	bool_pool_type;
private:
	// state information
	// a place to unroll instances and connections
	// a place to create state pseudo-footprint
	// back-reference to definition?  NO, instance_collection_map suffices
	/**
		Whether or not this definition footprint has been unrolled.
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

#if USE_SCOPE_ALIASES
	/**
		Fast lookup map of ALL aliases in this scope.  
	 */
	port_alias_tracker			scope_aliases;
#endif
	/**
		This keeps track which port members are internally aliased.
	 */
	port_alias_tracker			port_aliases;

#if USE_PRS_FOOTPRINT
	/**
		The set of unrolled production rules, local to this scope.  
	 */
	PRS::footprint				prs_footprint;
#endif

public:
	footprint();
	~footprint();

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

	ostream&
	dump(ostream&) const;

	ostream&
	dump_with_collections(ostream&) const;

	const port_alias_tracker&
	get_port_alias_tracker(void) const {
		return port_aliases;
	}

#if USE_SCOPE_ALIASES
	const port_alias_tracker&
	get_scope_alias_tracker(void) const {
		return scope_aliases;
	}
#endif

	template <class Tag>
	typename state_instance<Tag>::pool_type&
	get_pool(void) {
		return footprint_base<Tag>::_pool;
	}

	template <class Tag>
	const typename state_instance<Tag>::pool_type&
	get_pool(void) const {
		return footprint_base<Tag>::_pool;
	}

	void
	import_scopespace(const scopespace&);

	void
	import_hierarchical_scopespace(const scopespace&);

	void
	clear_instance_collection_map(void);

	good_bool
	create_dependent_types(void) const;

#if USE_SCOPE_ALIASES
	void
	evaluate_scope_aliases(void);
#else
	void
	evaluate_port_aliases(const port_formals_manager&);
#endif

#if USE_PRS_FOOTPRINT
	PRS::footprint&
	get_prs_footprint(void) { return prs_footprint; }
#endif

	good_bool
	expand_unique_subinstances(state_manager&) const;

	void
	assign_footprint_frame(footprint_frame&, 
		const port_member_context&) const;

public:
// persistent information management
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

#if 0
private:
	/**
		Don't want footprint to be copy-constructed.  
		But std::pair requires it in the footprint_manager.
		TODO: allow only a few friends in STL to use it.  
	 */
	explicit
	footprint(const footprint&);
#endif
};	// end class footprint

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_DEF_FOOTPRINT_H__

