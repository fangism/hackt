/**
	\file "Object/inst/substructure_alias_base.h"
	$Id: substructure_alias_base.h,v 1.24 2008/10/22 22:16:59 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_SUBSTRUCTURE_ALIAS_BASE_H__
#define	__HAC_OBJECT_INST_SUBSTRUCTURE_ALIAS_BASE_H__

#include <iosfwd>
#include "Object/inst/substructure_alias_fwd.h"
#include "Object/inst/subinstance_manager.h"
#include "util/persistent_fwd.h"
#include "util/memory/excl_ptr.h"

namespace HAC {
namespace entity {
struct dump_flags;
class instance_collection_base;
class physical_instance_placeholder;
class unroll_context;
#if RECURSE_COLLECT_ALIASES
class port_alias_tracker;
#endif
class footprint;
class footprint_frame;
class port_member_context;
class state_manager;
template <class> class global_entry;
template <class> class state_instance;
struct alias_printer;
struct alias_matcher_base;
using std::istream;
using std::ostream;
using util::memory::never_ptr;
using util::persistent_object_manager;
//=============================================================================
/**
	Parent to all aliases that may contain substructures.
 */
template <>
class substructure_alias_base<true> : protected subinstance_manager {
private:
	typedef	substructure_alias_base<true>	this_type;
	typedef	subinstance_manager		parent_type;
	typedef	parent_type::connection_references_type
						connection_references_type;
	typedef	physical_instance_placeholder	port_type;
#if ENABLE_RELAXED_TEMPLATE_PARAMETERS
	typedef	parent_type::relaxed_actuals_type
						relaxed_actuals_type;
#endif
public:
	/**
		ID number assigned by unique allocation, after all aliases
		and connections have been processed.
	 */
	size_t					instance_index;

protected:
virtual	~substructure_alias_base() { }

	substructure_alias_base() : parent_type(), instance_index(0) { }

	/**
		Visits children of the subinstance manager and 
		restores parent-child back-link.  
		Just calls subinstance_manager::relink_super_instance_alias().
	 */
	void
	restore_parent_child_links(void) {
		relink_super_instance_alias(*this);
	}

	template <class Tag>
	good_bool
	unroll_port_instances(
			const collection_interface<Tag>& p, 
#if ENABLE_RELAXED_TEMPLATE_PARAMETERS
			const relaxed_actuals_type& a,
#endif
			const unroll_context& c) {
		if (parent_type::__unroll_port_instances(p,
#if ENABLE_RELAXED_TEMPLATE_PARAMETERS
				a,
#endif
				c).good) {
			restore_parent_child_links();
			return good_bool(true);
		} else	return good_bool(false);
	}

	using parent_type::allocate_subinstances;

public:
	using parent_type::lookup_port_instance;

virtual	never_ptr<const physical_instance_collection>
	get_container_base(void) const;

// want to be pure virtual, but cannot be, :S
virtual	ostream&
	dump_hierarchical_name(ostream&, const dump_flags&) const;

virtual	size_t
	hierarchical_depth(void) const;

	using parent_type::dump_ports;

	// simply forwarded call
	good_bool
	connect_ports(const connection_references_type&, const unroll_context&);

#if RECURSE_COLLECT_ALIASES
	using parent_type::collect_port_aliases;
#endif

virtual	this_type&
	__trace_alias_base(const this_type&) const;

protected:
// not used yet
	good_bool
	__allocate_subinstance_footprint(
		footprint_frame&, state_manager&) const;

	using parent_type::__assign_footprint_frame;
	using parent_type::__construct_port_context;
	using parent_type::connect_port_aliases_recursive;

#if ENABLE_RELAXED_TEMPLATE_PARAMETERS
public:
virtual	void
	finalize_find(const unroll_context&) = 0;
#endif

protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;

	void
	load_object_base(const footprint&, istream&);

};	// end class substructure_alias_base<true>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Completely empty base class.  
 */
template <>
class substructure_alias_base<false> {
	typedef	substructure_alias_base<false>		this_type;
protected:
	// has no sub-instances
#if ENABLE_RELAXED_TEMPLATE_PARAMETERS
	typedef	subinstance_manager::relaxed_actuals_type
						relaxed_actuals_type;
#endif
public:
	size_t					instance_index;
protected:
	substructure_alias_base() : instance_index(0) { }
	/**
		No-op.
	 */
	template <class Tag>
	good_bool
	unroll_port_instances(const collection_interface<Tag>&, 
#if ENABLE_RELAXED_TEMPLATE_PARAMETERS
		const relaxed_actuals_type&,
#endif
		const unroll_context&) const { return good_bool(true); }

	void
	allocate_subinstances(footprint&) const { }

	/**
		No-op.  
	 */
	void
	restore_parent_child_links(void) const { }

public:
	ostream&
	dump_ports(ostream& o, const dump_flags&) const { return o; }

#if RECURSE_COLLECT_ALIASES
	void
	collect_port_aliases(const port_alias_tracker&) const { }
#endif

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

	// has no substructure
	good_bool
	connect_port_aliases_recursive(this_type&
#if ENABLE_RELAXED_TEMPLATE_PARAMETERS
			, const unroll_context&
#endif
			) {
		return good_bool(true);
	}

protected:
	void
	collect_transient_info_base(const persistent_object_manager&) const { }

	void
	write_object_base(const footprint&, const ostream&) const { }

	void
	load_object_base(const footprint&, const istream&) const { }

};	// end class substructure_alias_base<false>

//=============================================================================
}	// end namespace entity
}	// end namespace HAC


#endif	// __HAC_OBJECT_INST_SUBSTRUCTURE_ALIAS_BASE_H__

