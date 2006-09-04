/**
	\file "Object/inst/subinstance_manager.h"
	$Id: subinstance_manager.h,v 1.13.20.2 2006/09/04 05:44:13 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_SUBINSTANCE_MANAGER_H__
#define	__HAC_OBJECT_INST_SUBINSTANCE_MANAGER_H__

#include <vector>
#include "util/memory/count_ptr.h"
#include "util/boolean_types.h"
#include "Object/inst/substructure_alias_fwd.h"
#include "Object/inst/alias_visitee.h"
#include "Object/devel_switches.h"

namespace HAC {
class cflat_options;
namespace entity {
class substructure_manager;
class footprint;
class instance_collection_base;
class physical_instance_collection;
#if USE_INSTANCE_PLACEHOLDERS
class physical_instance_placeholder;
#endif
class unroll_context;
class meta_instance_reference_base;
class port_alias_tracker;
class port_member_context;
class state_manager;
class footprint_frame;
template <class> class instance_collection;
class cflat_visitor;
struct dump_flags;
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
#if USE_INSTANCE_PLACEHOLDERS
	typedef	physical_instance_placeholder		lookup_arg_type;
#else
	typedef	physical_instance_collection		lookup_arg_type;
#endif
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
	dump(ostream&, const dump_flags&) const;

	// TODO: assertion check that arg is a port member of this type?
	value_type
	lookup_port_instance(const lookup_arg_type&) const;

#if 0
	// OBSOLETE -- remove
	value_type
	lookup_member_instance(const lookup_arg_type&) const;
#endif

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

	good_bool
	connect_port_aliases_recursive(this_type&);

	void
	allocate(footprint&);

	good_bool
	replay_internal_aliases(void) const;

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
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class subinstance_manager

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_SUBINSTANCE_MANAGER_H__

