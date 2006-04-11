/**
	\file "Object/inst/instance_alias_info.h"
	Class declarations for aliases.
	Definition of implementation is in "art_object_instance_collection.tcc"
	This file came from "Object/art_object_instance_alias.h"
		in a previous life.  
	$Id: instance_alias_info.h,v 1.15 2006/04/11 07:54:42 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_ALIAS_INFO_H__
#define	__HAC_OBJECT_INST_INSTANCE_ALIAS_INFO_H__

#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"
#include "util/union_find.h"
#include "util/persistent_fwd.h"
#include "Object/inst/substructure_alias_base.h"
#include "Object/traits/class_traits_fwd.h"
#include "Object/inst/internal_aliases_policy_fwd.h"

namespace HAC {
namespace entity {
struct dump_flags;
class footprint;
class port_collection_context;
class port_member_context;
class instance_alias_info_actuals;
struct alias_visitor;
struct alias_printer;
template <class> struct alias_matcher;
using std::ostream;
using std::istream;
using util::memory::never_ptr;
using util::memory::count_ptr;
using util::persistent_object_manager;

//=============================================================================
#define	INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE				\
template <class Tag>

#define	INSTANCE_ALIAS_INFO_CLASS					\
instance_alias_info<Tag>

/**
	Information structure for a heirarchical name for a bool.  
	This may be extended arbitrarily to contain attributes.  
	Each object of this type represents a unique qualified name.  
	TODO: parent type that determines whether or not
	this contains relaxed actual parameters, and how they are stored.  
	TODO: factor out the template-independent members/methods of this class.
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
class instance_alias_info :
		public substructure_alias_base<
			class_traits<Tag>::has_substructure>, 
		// was protected
		public class_traits<Tag>::instance_alias_relaxed_actuals_type {
	typedef	INSTANCE_ALIAS_INFO_CLASS	this_type;
friend class instance_alias_info_actuals;
public:
	typedef	class_traits<Tag>		traits_type;
	typedef	typename traits_type::instance_alias_relaxed_actuals_type
						actuals_parent_type;
	typedef	substructure_alias_base<traits_type::has_substructure>
						substructure_parent_type;
	typedef	internal_aliases_policy<traits_type::can_internally_alias>
						internal_alias_policy;
	typedef	typename traits_type::instance_type
						instance_type;
	/**
		The reference count pointer type to the underlying
		uniquely created instance.  
	 */
	typedef	count_ptr<instance_type>	instance_ptr_type;

	/**
		Container type.
	 */
	typedef	typename traits_type::instance_collection_generic_type
					instance_collection_generic_type;
	typedef	instance_collection_generic_type	container_type;
	typedef	never_ptr<const container_type>	container_ptr_type;
	typedef	util::union_find_derived<this_type>
						instance_alias_base_type;
	// we won't have iterator interface anymore!
	typedef	util::union_find_derived_pseudo_iterator_default<this_type>
						iterator_policy;
	typedef	typename iterator_policy::type	pseudo_iterator;
	typedef	typename iterator_policy::const_type
						pseudo_const_iterator;
	typedef	typename actuals_parent_type::alias_actuals_type
						relaxed_actuals_type;
public:
	// size_t instance_index; // was moved to substructure_base class
	/**
		Back-reference to the mother container.
		Consider using this to determine "instantiated" state.  
		NOTE: this could be pushed to children, and replaced
			with virtual interface.  
	 */
	container_ptr_type				container;

protected:
	// constructors only intended for children classes
	instance_alias_info() : container(NULL) { }

	/**
		Plain constructor initializing with container back-ptr.
	 */
	explicit
	instance_alias_info(const container_ptr_type m) :
		container(m) {
	}

	// default copy-constructor

	// default destructor, non-virtual? yes, but protected.
virtual	~instance_alias_info();

	// default assignment

public:
	/**
		container is inherited from instance_alias_info
	 */
	bool
	valid(void) const { return this->container; }

	// don't inline this virtual
	never_ptr<const physical_instance_collection>
	get_container_base(void) const;

	never_ptr<const physical_instance_collection>
	get_supermost_collection(void) const;

	never_ptr<const substructure_alias>
	get_supermost_substructure(void) const;

	void
	check(const container_type* p) const;

virtual	pseudo_const_iterator
	find(void) const;

virtual	pseudo_iterator
	find(void);

public:
	/**
		Instantiates officially by linking to parent collection.  
		FYI: This is only called by instance_array<0> (scalar)
			in instantiate_indices().
	 */
	void
	instantiate(const container_ptr_type p, const unroll_context&);

virtual	void
	finalize_canonicalize(instance_alias_base_type&);

	size_t
	assign_local_instance_id(footprint&);

	/**
		Attaches actual parameters to this alias.  
		TODO: make this policy-specific, of course.  
		NOTE: constness is merely for convenience, promising not
			to modify the key.  
		\return true if successful, didn't collide.
	 */
	using actuals_parent_type::get_relaxed_actuals;
	using actuals_parent_type::dump_actuals;
	using actuals_parent_type::dump_complete_type;
	using actuals_parent_type::attach_actuals;
	using actuals_parent_type::compare_actuals;
	using actuals_parent_type::create_dependent_types;

private:
	using actuals_parent_type::__initialize_assign_footprint_frame;

protected:
	physical_instance_collection&
	trace_collection(const substructure_alias&) const;

#if 0
	physical_instance_collection&
	retrace_collection(const substructure_alias&) const;
#endif

public:

#define	TRACE_ALIAS_BASE_PROTO						\
	typename instance_alias_info<Tag>::substructure_parent_type&	\
	__trace_alias_base(const substructure_alias&) const

#define	TRACE_ALIAS_PROTO						\
	instance_alias_info<Tag>&					\
	trace_alias(const substructure_alias&) const

virtual	TRACE_ALIAS_BASE_PROTO;
virtual	TRACE_ALIAS_PROTO;

public:
	bool
	must_match_type(const this_type&) const;

	// consider: pure virtual multikey_generic<K>

	/**
		Equality comparison is required to determine
		whether or not an object of this type has default value.
	 */
	bool
	operator == (const this_type& i) const {
		return (this->instance_index == i.instance_index)
			&& (this->container == i.container);
	}

	bool
	is_port_alias(void) const;

virtual	ostream&
	dump_alias(ostream& o, const dump_flags&) const;

	ostream&
	dump_aliases(ostream& o) const;

	ostream&
	dump_hierarchical_name(ostream&) const;

	ostream&
	dump_hierarchical_name(ostream&, const dump_flags&) const;

	void
	accept(alias_visitor&) const;

	size_t
	hierarchical_depth(void) const;

	using substructure_parent_type::dump_ports;
	using substructure_parent_type::collect_port_aliases;
	using substructure_parent_type::connect_ports;
	// using substructure_parent_type::lookup_port_instance;
	using substructure_parent_type::replay_substructure_aliases;

	/// called by top-level
	good_bool
	allocate_assign_subinstance_footprint_frame(footprint_frame&, 
		state_manager&, const port_member_context&, 
		const size_t) const;

	/// called recursively
	void
	assign_footprint_frame(footprint_frame&,
		const port_collection_context&, const size_t) const;

	void
	construct_port_context(port_collection_context&, 
		const footprint_frame&, const size_t) const;

private:
	using substructure_parent_type::__allocate_subinstance_footprint;
public:
	using substructure_parent_type::__construct_port_context;

public:
	static
	good_bool
	checked_connect_port(this_type&, this_type&);

	// punting relaxed actuals checking until after unroll phase
	static
	good_bool
	checked_connect_alias(this_type&, this_type&);

	/// counterpart to load_alias_reference (should be pure virtual)
virtual	void
	write_next_connection(const persistent_object_manager& m, 
		ostream& o) const;

// probably need not be virtual, same for all children classes.
virtual	void
	load_next_connection(const persistent_object_manager& m, 
		istream& i);

	/// counterpart to write_next_connection
	static
	instance_alias_base_type&
	load_alias_reference(const persistent_object_manager& m, istream& i);

public:
	void
	collect_transient_info_base(persistent_object_manager& m) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

	class transient_info_collector {
		persistent_object_manager&	manager;
	public:
		explicit
		transient_info_collector(persistent_object_manager& m) :
			manager(m) { }

		void
		operator () (const INSTANCE_ALIAS_INFO_CLASS&);
	};	// end class transient_info_collector

};	// end class instance_alias_info

//-----------------------------------------------------------------------------

template <class Tag>
ostream&
operator << (ostream&,
	const typename instance_alias_info<Tag>::instance_alias_base_type&);

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_ALIAS_INFO_H__

