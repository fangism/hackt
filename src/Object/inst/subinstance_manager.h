/**
	\file "Object/inst/subinstance_manager.h"
	$Id: subinstance_manager.h,v 1.7 2005/12/13 04:15:33 fang Exp $
 */

#ifndef	__OBJECT_INST_SUBINSTANCE_MANAGER_H__
#define	__OBJECT_INST_SUBINSTANCE_MANAGER_H__

#include <vector>
#include "util/memory/count_ptr.h"
#include "util/boolean_types.h"
#include "Object/inst/substructure_alias_fwd.h"

namespace HAC {
class cflat_options;
namespace entity {
class substructure_manager;
class footprint;
class instance_collection_base;
class physical_instance_collection;
class unroll_context;
class meta_instance_reference_base;
class port_alias_tracker;
class port_member_context;
class state_manager;
class footprint_frame;
template <class> class instance_collection;
struct cflat_aliases_arg_type;
using std::ostream;
using std::istream;
using std::string;
using std::vector;
using util::good_bool;
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
public:
	typedef	count_ptr<instance_collection_type>	entry_value_type;
	// just a synonym
	typedef	entry_value_type			value_type;
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

	ostream&
	dump(ostream&) const;

	// TODO: assertion check that arg is a port member of this type?
	value_type
	lookup_port_instance(const instance_collection_type&) const;

	// want to recursively expand ports when this is instantiated
	template <class Tag>
	void
	unroll_port_instances(const instance_collection<Tag>&, 
		const unroll_context&);
	// unroll_port_instances(const physical_instance_collection&);

	void
	collect_port_aliases(port_alias_tracker&) const;

	good_bool
	connect_ports(const connection_references_type&, const unroll_context&);

	void
	allocate(footprint&);

	static
	good_bool
	synchronize_port_actuals(this_type&, this_type&);

	// const is bogus here (temporary workaround)
	good_bool
	create_state(const this_type&, footprint&);

	void
	inherit_state(const this_type&, const footprint&);

	good_bool
	replay_internal_aliases(void) const;

	void
	construct_port_context(port_member_context&, 
		const footprint_frame&) const;

	void
	assign_footprint_frame(footprint_frame&,
		const port_member_context&) const;

	void
	cflat_aliases(const cflat_aliases_arg_type&) const;

	// for each entry, re-link
	void
	relink_super_instance_alias(const substructure_alias&);

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class subinstance_manager

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_INST_SUBINSTANCE_MANAGER_H__

