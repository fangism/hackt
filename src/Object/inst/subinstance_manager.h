/**
	\file "Object/inst/subinstance_manager.h"
	$Id: subinstance_manager.h,v 1.15.4.2 2006/11/02 06:18:47 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_SUBINSTANCE_MANAGER_H__
#define	__HAC_OBJECT_INST_SUBINSTANCE_MANAGER_H__

#include <vector>
#include "util/memory/count_ptr.h"
#include "util/boolean_types.h"
#include "Object/inst/substructure_alias_fwd.h"
#include "Object/inst/alias_visitee.h"
#include "Object/devel_switches.h"
#if ALLOCATE_PORT_ACTUAL_COLLECTIONS
#include "util/memory/excl_ptr.h"
#endif

namespace HAC {
class cflat_options;
namespace entity {
class substructure_manager;
class footprint;
class instance_collection_base;
class physical_instance_collection;
class physical_instance_placeholder;
class unroll_context;
class meta_instance_reference_base;
class port_alias_tracker;
class port_member_context;
class state_manager;
class footprint_frame;
#if ALLOCATE_PORT_ACTUAL_COLLECTIONS
template <class> class collection_interface;
#else
template <class> class instance_collection;
#endif
class cflat_visitor;
struct dump_flags;
using std::ostream;
using std::istream;
using std::string;
using std::vector;
using util::good_bool;
#if ALLOCATE_PORT_ACTUAL_COLLECTIONS
using util::memory::never_ptr;
#endif
using util::memory::count_ptr;
using util::persistent_object_manager;

//=============================================================================
/**
	Contains an array of sub-instances, children collection of aliases.  
	Definitions will cache canonical maps containing already 
	instantiated types.  
	NOTE: this only applies to public ports, 
		not any of the private internals!
	This will be tied closely to the port_formals_manager class.  
	Also interesting to note is that this level does NOT even depend
		on the strict template formals, as it only depends on
		the list of names in the port formals list, not their sizes.
		The types/sizes of the port actual entries will, however, 
		depend on the template parameters.  
	TODO: fuse this class into substructure_alias_base<true>.
 */
class subinstance_manager {
friend class substructure_manager;
	typedef	subinstance_manager			this_type;
	typedef	physical_instance_collection	instance_collection_type;
	typedef	physical_instance_placeholder		lookup_arg_type;
public:
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
	typedef	never_ptr<instance_collection_type>	entry_value_type;
#else
	typedef	count_ptr<instance_collection_type>	entry_value_type;
#endif
	// just a synonym
	typedef	entry_value_type			value_type;
	/**
		TODO: use lightweight valarray.
	 */
	typedef	vector<value_type>			array_type;
	typedef	vector<count_ptr<const meta_instance_reference_base> >
						connection_references_type;
protected:
	typedef	array_type::const_iterator		const_iterator;
	typedef	array_type::iterator			iterator;

protected:
	array_type					subinstance_array;
public:
	subinstance_manager();

	// explicit custom copy constructor
	explicit
	subinstance_manager(const this_type&);

	~subinstance_manager();

	bool
	empty(void) const { return subinstance_array.empty(); }

	size_t
	size(void) const { return subinstance_array.size(); }

	void
	reserve(const size_t);

	void
	push_back(const entry_value_type&);

#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
	array_type&
	get_array(void) { return subinstance_array; }

	const array_type&
	get_array(void) const { return subinstance_array; }
#endif

	ostream&
	dump(ostream&, const dump_flags&) const;

	// TODO: assertion check that arg is a port member of this type?
	value_type
	lookup_port_instance(const lookup_arg_type&) const;

	// want to recursively expand ports when this is instantiated
	template <class Tag>
	void
	unroll_port_instances(
#if ALLOCATE_PORT_ACTUAL_COLLECTIONS
		const collection_interface<Tag>&, 
#else
		const instance_collection<Tag>&, 
#endif
		const unroll_context&);
	// unroll_port_instances(const physical_instance_collection&);

	void
	collect_port_aliases(port_alias_tracker&) const;

	good_bool
	connect_ports(const connection_references_type&, const unroll_context&);

	good_bool
	connect_port_aliases_recursive(this_type&);

	void
	allocate(footprint&);

	void
	construct_port_context(port_member_context&, 
		const footprint_frame&) const;

	void
	assign_footprint_frame(footprint_frame&,
		const port_member_context&) const;

	void
	accept(alias_visitor&) const;

	// for each entry, re-link
	void
	relink_super_instance_alias(const substructure_alias&);

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
		const footprint&, 
#else
		const persistent_object_manager&, 
#endif
		ostream&) const;

	void
	load_object_base(
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
		const footprint&, 
#else
		const persistent_object_manager&, 
#endif
		istream&);

};	// end class subinstance_manager

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_SUBINSTANCE_MANAGER_H__

