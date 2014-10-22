/**
	\file "Object/inst/substructure_alias_base.hh"
	$Id: substructure_alias_base.hh,v 1.29 2010/08/24 21:05:42 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_SUBSTRUCTURE_ALIAS_BASE_H__
#define	__HAC_OBJECT_INST_SUBSTRUCTURE_ALIAS_BASE_H__

#include <iosfwd>
#include "Object/inst/substructure_alias_fwd.hh"
#include "Object/inst/subinstance_manager.hh"
#include "util/persistent_fwd.hh"
#include "util/memory/excl_ptr.hh"

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
class state_manager;
template <class> struct global_entry;
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
class substructure_alias_base<true> : public subinstance_manager {
private:
	typedef	substructure_alias_base<true>	this_type;
	typedef	subinstance_manager		parent_type;
	typedef	parent_type::connection_references_type
						connection_references_type;
	typedef	physical_instance_placeholder	port_type;
	typedef	parent_type::relaxed_actuals_type
						relaxed_actuals_type;
public:
	/**
		ID number assigned by unique allocation, after all aliases
		and connections have been processed.
	 */
	size_t					instance_index;

protected:
virtual	~substructure_alias_base() { }

	substructure_alias_base() : parent_type(), instance_index(0) { }

public:
	/**
		Visits children of the subinstance manager and 
		restores parent-child back-link.  
		Just calls subinstance_manager::relink_super_instance_alias().
		Made public to be callable by footprint.
	 */
	void
	restore_parent_child_links(void) {
		relink_super_instance_alias(*this);
	}

protected:
	template <class Tag>
	good_bool
	unroll_port_instances(
			const collection_interface<Tag>& p, 
			const relaxed_actuals_type& a,
			target_context& c) {
		if (parent_type::__unroll_port_instances(p, a, c).good) {
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

	using parent_type::connect_port_aliases_recursive;

public:
virtual	void
	finalize_find(const unroll_context&) = 0;

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
	typedef	subinstance_manager::relaxed_actuals_type
						relaxed_actuals_type;
public:
	size_t					instance_index;
protected:
	substructure_alias_base() : instance_index(0) { }

public:
	void
	deep_copy(const this_type&, const footprint&) const { }
protected:
	/**
		No-op.
	 */
	template <class Tag>
	good_bool
	unroll_port_instances(const collection_interface<Tag>&, 
			const relaxed_actuals_type&,
			const target_context&) const {
		return good_bool(true);
	}

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

	// has no substructure
	good_bool
	connect_port_aliases_recursive(const this_type&,
			const target_context&) const {
		return good_bool(true);
	}

	void
	reconnect_port_aliases_recursive(const this_type&) const { }

protected:
	void
	collect_transient_info_base(const persistent_object_manager&) const { }

	void
	write_object_base(const footprint&, const ostream&) const { }

	void
	load_object_base(const footprint&, const istream&) const { }

};	// end class substructure_alias_base<false>

//=============================================================================
struct instance_index_extractor {

	template <class AliasPtr>
	size_t
	operator () (const AliasPtr& alias_ptr) const {
		NEVER_NULL(alias_ptr);
		return alias_ptr->instance_index;
	}
};	// end struct instance_index_extractor

//=============================================================================
}	// end namespace entity
}	// end namespace HAC


#endif	// __HAC_OBJECT_INST_SUBSTRUCTURE_ALIAS_BASE_H__

