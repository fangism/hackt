/**
	\file "Object/def/footprint.h"
	Data structure for each complete type's footprint template.  
	$Id: footprint.h,v 1.1.2.10 2005/08/29 21:32:03 fang Exp $
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

#include "util/boolean_types.h"
#include "util/persistent_fwd.h"
#include "util/string_fwd.h"
#include "util/hash_qmap.h"
#include "util/memory/count_ptr.h"

namespace ART {
namespace entity {
class instance_collection_base;
class scopespace;
using std::string;
using std::istream;
using std::ostream;
using util::good_bool;
using util::memory::count_ptr;
using util::persistent_object_manager;

template <class Tag>
struct footprint_pool_getter;

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
template <class> friend struct footprint_pool_getter;
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
		BTW, using count_ptrs for ease of coy-constructibility.  
		Q: do we need a separate port_formals_manager?
	 */
	typedef	util::hash_qmap<string, instance_collection_ptr_type>
					instance_collection_map_type;
	typedef	instance_collection_map_type::const_iterator
					const_instance_map_iterator;
	typedef	state_instance<process_tag>::pool_type	process_pool_type;
	typedef	state_instance<channel_tag>::pool_type	channel_pool_type;
	typedef	state_instance<datastruct_tag>::pool_type
							struct_pool_type;
	typedef	state_instance<enum_tag>::pool_type	enum_pool_type;
	typedef	state_instance<int_tag>::pool_type	int_pool_type;
	typedef	state_instance<bool_tag>::pool_type	bool_pool_type;
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

	// footprint should include pools for the various types
	// will also need some template member accessor
	process_pool_type			process_pool;
	channel_pool_type			channel_pool;
	struct_pool_type			struct_pool;
	enum_pool_type				enum_pool;
	int_pool_type				int_pool;
	bool_pool_type				bool_pool;

#if 0
	TODO:
	write an export interface for just the ports
	so the invoker may determine which ports (if any) 
	are internally connected.
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

	void
	import_scopespace(const scopespace&);

	good_bool
	create_dependent_types(void) const;

// persistent information management
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);
};	// end class footprint

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Templated method of accessing member pools.  
 */
#define	SPECIALIZE_FOOTPRINT_POOL_GETTER(Tag,Member)			\
template <>								\
struct footprint_pool_getter<Tag> {					\
	typedef	state_instance<Tag>::pool_type	pool_type;		\
									\
	inline								\
	pool_type&							\
	operator () (footprint& f) const				\
		{ return f.Member; }					\
									\
	inline								\
	const pool_type&						\
	operator () (const footprint& f) const				\
		{ return f.Member; }					\
};	// end struct pool_getter

SPECIALIZE_FOOTPRINT_POOL_GETTER(process_tag, process_pool)
SPECIALIZE_FOOTPRINT_POOL_GETTER(channel_tag, channel_pool)
SPECIALIZE_FOOTPRINT_POOL_GETTER(datastruct_tag, struct_pool)
SPECIALIZE_FOOTPRINT_POOL_GETTER(enum_tag, enum_pool)
SPECIALIZE_FOOTPRINT_POOL_GETTER(int_tag, int_pool)
SPECIALIZE_FOOTPRINT_POOL_GETTER(bool_tag, bool_pool)

#undef	SPECIALIZE_FOOTPRINT_POOL_GETTER

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_DEF_FOOTPRINT_H__

