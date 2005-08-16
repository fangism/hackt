/**
	\file "Object/inst/substructure_alias_base.h"
	$Id: substructure_alias_base.h,v 1.3.4.1 2005/08/16 20:32:15 fang Exp $
 */

#ifndef	__OBJECT_INST_SUBSTRUCTURE_ALIAS_BASE_H__
#define	__OBJECT_INST_SUBSTRUCTURE_ALIAS_BASE_H__

#include <iosfwd>
#include "Object/inst/substructure_alias_fwd.h"
#include "Object/inst/subinstance_manager.h"
#include "util/persistent_fwd.h"
#include "Object/devel_switches.h"
#if USE_MODULE_FOOTPRINT
#include "Object/def/footprint.h"
#endif

namespace ART {
namespace entity {
class instance_collection_base;
class unroll_context;
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

#if USE_MODULE_FOOTPRINT
	void
	create_subinstance_state(this_type& t, footprint& f) {
		subinstances.create_state(t.subinstances, f);
		// t.get_back_ref()->subinstances.create_state(subinstances);
	}

	template <class Tag>
	void
	inherit_state(const state_instance<Tag>& t, const footprint& f) {
		subinstances.inherit_state(t.get_back_ref()->subinstances, f);
	}

	void
	allocate_subinstances(footprint&);
#else
	void
	create_subinstance_state(this_type& t) {
		subinstances.create_state(t.subinstances);
		// t.get_back_ref()->subinstances.create_state(subinstances);
	}

	template <class Tag>
	void
	inherit_state(const state_instance<Tag>& t) {
		subinstances.inherit_state(t.get_back_ref()->subinstances);
	}

	void
	allocate_subinstances(void);
#endif

public:
	// just a forwarded call
	subinstance_manager::value_type
	lookup_port_instance(const instance_collection_base& i) const;

// want to be pure virtual, but cannot be, :S
virtual	ostream&
	dump_hierarchical_name(ostream&) const;

	ostream&
	dump_ports(ostream& o) const { return subinstances.dump(o); }

	// simply forwarded call
	good_bool
	connect_ports(const connection_references_type&, 
		const unroll_context&);

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
	void
	create_subinstance_state(const this_type&
#if USE_MODULE_FOOTPRINT
		, footprint&
#endif
		) const { }

#if USE_MODULE_FOOTPRINT
	template <class Tag>
	void
	inherit_state(const state_instance<Tag>&, const footprint&) const { }

	void
	allocate_subinstances(footprint&) const { }
#else
	/**
		Nothing to copy recursively.  
	 */
	template <class Tag>
	void
	inherit_state(const state_instance<Tag>&) const { }

	void
	allocate_subinstances(void) const { }
#endif

	/**
		No-op.  
	 */
	void
	restore_parent_child_links(void) { }

public:
	ostream&
	dump_ports(ostream& o) const { return o; }

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

