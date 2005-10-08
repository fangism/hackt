/**
	\file "Object/inst/substructure_alias_base.h"
	$Id: substructure_alias_base.h,v 1.6 2005/10/08 01:39:58 fang Exp $
 */

#ifndef	__OBJECT_INST_SUBSTRUCTURE_ALIAS_BASE_H__
#define	__OBJECT_INST_SUBSTRUCTURE_ALIAS_BASE_H__

#include <iosfwd>
#include "Object/inst/substructure_alias_fwd.h"
#include "Object/inst/subinstance_manager.h"
#include "util/persistent_fwd.h"
#include "Object/def/footprint.h"

namespace ART {
namespace entity {
struct dump_flags;
class instance_collection_base;
class physical_instance_collection;
class unroll_context;
class port_alias_tracker;
class footprint;
class footprint_frame;
class port_member_context;
class state_manager;
template <class> class state_instance;
using std::istream;
using std::ostream;
using util::persistent_object_manager;
//=============================================================================
/**
	Parent to all aliases that may contain substructures.
 */
template <>
class substructure_alias_base<true> {
private:
	typedef	substructure_alias_base<true>	this_type;
	typedef	subinstance_manager::connection_references_type
						connection_references_type;
	typedef	physical_instance_collection	port_type;
protected:
	/**
		Container of sub-instances.  
	 */
	subinstance_manager			subinstances;
public:
virtual	~substructure_alias_base() { }

protected:
	/**
		Visits children of the subinstance manager and 
		restores parent-child back-link.  
		Just calls subinstance_manager::relink_super_instance_alias().
	 */
	void
	restore_parent_child_links(void) {
		subinstances.relink_super_instance_alias(*this);
	}

	template <class Tag>
	void
	unroll_port_instances(const instance_collection<Tag>& p, 
			const unroll_context& c) {
		subinstances.unroll_port_instances(p, c);
		restore_parent_child_links();
	}

	good_bool
	create_subinstance_state(this_type& t, footprint& f) {
		return subinstances.create_state(t.subinstances, f);
		// t.get_back_ref()->subinstances.create_state(subinstances);
	}

	template <class Tag>
	void
	inherit_state(const state_instance<Tag>& t, const footprint& f) {
		subinstances.inherit_state(t.get_back_ref()->subinstances, f);
	}

	void
	allocate_subinstances(footprint&);

	static
	good_bool
	synchronize_port_actuals(this_type& l, this_type& r) {
		return subinstance_manager::synchronize_port_actuals(
			l.subinstances, r.subinstances);
	}

	good_bool
	replay_substructure_aliases(void) const;

public:
	// just a forwarded call
	subinstance_manager::value_type
	lookup_port_instance(const port_type& i) const;

virtual	never_ptr<const physical_instance_collection>
	get_container_base(void) const;

// want to be pure virtual, but cannot be, :S
virtual	ostream&
	dump_hierarchical_name(ostream&, const dump_flags&) const;

virtual	size_t
	hierarchical_depth(void) const;

virtual	size_t
	allocate_state(footprint&) const;

	ostream&
	dump_ports(ostream& o) const { return subinstances.dump(o); }

	// simply forwarded call
	good_bool
	connect_ports(const connection_references_type&, const unroll_context&);

	void
	collect_port_aliases(port_alias_tracker&) const;

virtual	this_type&
	__trace_alias_base(const this_type&) const;

protected:
	good_bool
	__allocate_subinstance_footprint(
		footprint_frame&, state_manager&) const;

	void
	__assign_footprint_frame(footprint_frame&,
		const port_member_context&) const;

	void
	__construct_port_context(port_member_context&,
		const footprint_frame&) const;

protected:
	// call forwarding
	void
	collect_transient_info_base(persistent_object_manager& m) const;

	void
	write_object_base(const persistent_object_manager& m, ostream& o) const;

	void
	load_object_base(const persistent_object_manager& m, istream& i);

};	// end class substructure_alias_base

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Completely empty base class.  
 */
template <>
class substructure_alias_base<false> {
	typedef	substructure_alias_base<false>		this_type;
protected:
	// has no sub-instances
protected:
	/**
		No-op.
	 */
	template <class Tag>
	void
	unroll_port_instances(const instance_collection<Tag>&, 
		const unroll_context&) const { }

	/**
		Has no substructure, thus does not recur.  
	 */
	good_bool
	create_subinstance_state(const this_type&, footprint&) const {
		return good_bool(true);
	}

	/**
		Nothing to copy recursively.  
	 */
	template <class Tag>
	void
	inherit_state(const state_instance<Tag>&, const footprint&) const { }

	void
	allocate_subinstances(footprint&) const { }

	/**
		No-op.  
	 */
	void
	restore_parent_child_links(void) { }

	static
	good_bool
	synchronize_port_actuals(const this_type&, const this_type&) {
		return good_bool(true);
	}

	/**
		No-op, because this has no substructure.  
	 */
	good_bool
	replay_substructure_aliases(void) const {
		return good_bool(true);
	}

public:
	ostream&
	dump_ports(ostream& o) const { return o; }

	void
	collect_port_aliases(const port_alias_tracker&) const { }

	void
	connect_ports(void) const { }

protected:
	good_bool
	__allocate_subinstance_footprint(const footprint_frame&, 
			const state_manager&) const {
		return good_bool(true);
	}


	void
	__assign_footprint_frame(const footprint_frame&,
			const port_member_context&) const {
	}

	void
	__construct_port_context(const port_member_context&, 
			const footprint_frame&) const {
		// No-op.
	}

protected:
	void
	collect_transient_info_base(const persistent_object_manager&) const { }

	void
	write_object_base(const persistent_object_manager&, 
		const ostream&) const { }

	void
	load_object_base(const persistent_object_manager&, 
		const istream&) const { }

};	// end class substructure_alias_base

//=============================================================================
}	// end namespace entity
}	// end namespace ART


#endif	// __OBJECT_INST_SUBSTRUCTURE_ALIAS_BASE_H__

