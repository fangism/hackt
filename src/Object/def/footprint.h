/**
	\file "Object/def/footprint.h"
	Data structure for each complete type's footprint template.  
	$Id: footprint.h,v 1.1.2.1 2005/08/10 20:30:53 fang Exp $
 */

#ifndef	__OBJECT_DEF_FOOTPRINT_H__
#define	__OBJECT_DEF_FOOTPRINT_H__

#include <iosfwd>
#include "Object/inst/state_instance.h"
#include "Object/inst/instance_pool.h"
#include "Object/traits/classification_tags.h"
#include "util/persistent_fwd.h"
#include "util/string_fwd.h"
#include "util/hash_qmap.h"
#include "util/memory/count_ptr.h"

namespace ART {
namespace entity {
class instance_collection_base;
using std::string;
using std::istream;
using std::ostream;
using util::memory::count_ptr;
using util::persistent_object_manager;
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
	the parent definition.  

	NOTE: future, when partial specializations are introduced, 
	may want a back-reference pointer to the referenced 
	specialization definition.  (FAR far future)
 */
class footprint {
private:
	/**
		The type of map used to maintain local copy of instances.  
		Instances contained herein will have no parent scopespace?
		How do we distinguish formal instances from actuals?
	 */
	typedef	util::hash_qmap<string, count_ptr<instance_collection_base> >
					instance_collection_map_type;
private:
	// state information
	// a place to unroll instances and connections
	// a place to create state pseudo-footprint
	// back-reference to definition?
	/**
		Whether or not this definition footprint has been unrolled.
	 */
	bool				unrolled;
	/**
		Whether or not the footprint has created unique state
		(WITHOUT external connectivity context).
	 */
	bool				created;
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
	 */
	instance_collection_map_type	instance_collection_map;
#if 0
	TODO:
	write an export interface for just the ports
	so the invoker may determine which ports (if any) 
	are internally connected.

	// footprint should include pools for the various types
	// will also need some template member
	state_instance<process_tag>::pool_type		process_pool;
	state_instance<channel_tag>::pool_type		channel_pool;
	state_instance<datastruct_tag>::pool_type	struct_pool;
	state_instance<enum_tag>::pool_type		enum_pool;
	state_instance<int_tag>::pool_type		int_pool;
	state_instance<bool_tag>::pool_type		bool_pool;
#endif
public:
	footprint();
	~footprint();

	bool
	is_unrolled(void) const { return unrolled; }

	bool
	is_created(void) const { return created; }

#if 0
	/// import instance map from scopespace
	// or virtual definition_base::export_instances_to_footprint();
	// might also be applicable to module::global_namespace
	void
	import_instances(const scopespace::used_id_map_type&);
#endif

// persistent information management
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);
};	// end class footprint

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_DEF_FOOTPRINT_H__

