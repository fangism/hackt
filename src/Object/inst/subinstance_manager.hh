/**
	\file "Object/inst/subinstance_manager.hh"
	$Id: subinstance_manager.hh,v 1.26 2010/04/07 00:12:45 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_SUBINSTANCE_MANAGER_HH__
#define	__HAC_OBJECT_INST_SUBINSTANCE_MANAGER_HH__

#include <iosfwd>
#include <vector>
#include "util/memory/count_ptr.hh"
#include "util/boolean_types.hh"
#include "Object/inst/substructure_alias_fwd.hh"
#include "Object/inst/alias_visitee.hh"
#include "Object/unroll/target_context.hh"
#include "Object/devel_switches.hh"
#include "util/memory/excl_ptr.hh"

namespace HAC {
struct cflat_options;
namespace entity {
class substructure_manager;
class footprint;
class instance_collection_base;
class physical_instance_collection;
class physical_instance_placeholder;
class unroll_context;
class meta_instance_reference_base;
#if RECURSE_COLLECT_ALIASES
class port_alias_tracker;
#endif
class port_member_context;
class state_manager;
class footprint_frame;
template <class> class collection_interface;
struct dump_flags;
class const_param_expr_list;
using std::ostream;
using std::istream;
using std::string;
using std::vector;
using util::good_bool;
using util::memory::never_ptr;
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
	typedef	never_ptr<instance_collection_type>	entry_value_type;
	// just a synonym
	typedef	entry_value_type			value_type;
	/**
		TODO: use lightweight valarray.
	 */
	typedef	vector<value_type>			array_type;
	typedef	vector<count_ptr<const meta_instance_reference_base> >
						connection_references_type;
	typedef	count_ptr<const const_param_expr_list>	relaxed_actuals_type;
protected:
	typedef	array_type::const_iterator		const_iterator;
	typedef	array_type::iterator			iterator;

protected:
	array_type					subinstance_array;
public:
	subinstance_manager();

protected:
	// explicit custom copy constructor
	explicit
	subinstance_manager(const this_type&);

public:
	~subinstance_manager();

	void
	deep_copy(const subinstance_manager&, footprint&);

	bool
	empty(void) const { return subinstance_array.empty(); }

	size_t
	size(void) const { return subinstance_array.size(); }

	void
	reserve(const size_t);

	void
	push_back(const entry_value_type&);

	array_type&
	get_array(void) { return subinstance_array; }

	const array_type&
	get_array(void) const { return subinstance_array; }

	ostream&
	dump_ports(ostream&, const dump_flags&) const;

	// TODO: assertion check that arg is a port member of this type?
	value_type
	lookup_port_instance(const lookup_arg_type&) const;

	// want to recursively expand ports when this is instantiated
	template <class Tag>
	good_bool
	__unroll_port_instances(
		const collection_interface<Tag>&, 
		const relaxed_actuals_type&,
		target_context&);

#if RECURSE_COLLECT_ALIASES
	void
	collect_port_aliases(port_alias_tracker&) const;
#endif

	good_bool
	__connect_ports(const connection_references_type&,
		const unroll_context&);

	good_bool
	connect_implicit_ports(const connection_references_type&,
		const unroll_context&);

	good_bool
	connect_port_aliases_recursive(this_type&, target_context&);

	void
	reconnect_port_aliases_recursive(this_type&);

	void
	allocate_subinstances(footprint&);

	void
	accept(alias_visitor&) const;

	// for each entry, re-link
	void
	relink_super_instance_alias(const substructure_alias&);

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const footprint&, ostream&) const;

	void
	load_object_base(const footprint&, istream&);

};	// end class subinstance_manager

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_SUBINSTANCE_MANAGER_HH__

