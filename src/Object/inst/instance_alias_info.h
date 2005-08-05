/**
	\file "Object/inst/instance_alias_info.h"
	Class declarations for aliases.
	Definition of implementation is in "art_object_instance_collection.tcc"
	This file came from "Object/art_object_instance_alias.h"
		in a previous life.  
	$Id: instance_alias_info.h,v 1.2.4.2 2005/08/05 23:26:46 fang Exp $
 */

#ifndef	__OBJECT_INST_INSTANCE_ALIAS_INFO_H__
#define	__OBJECT_INST_INSTANCE_ALIAS_INFO_H__

#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"
#include "util/ring_node.h"
#include "util/persistent_fwd.h"
#include "Object/inst/substructure_alias_base.h"
#include "Object/traits/class_traits_fwd.h"

namespace ART {
namespace entity {
using std::ostream;
using std::istream;
using util::ring_node_derived;
using util::ring_node_derived_iterator_default;
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
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
class instance_alias_info :
		public substructure_alias_base<
			class_traits<Tag>::has_substructure>, 
		public class_traits<Tag>::instance_alias_relaxed_actuals_type {
	typedef	INSTANCE_ALIAS_INFO_CLASS	this_type;
public:
	typedef	typename class_traits<Tag>::instance_alias_relaxed_actuals_type
						actuals_parent_type;
	typedef	substructure_alias_base<class_traits<Tag>::has_substructure>
						substructure_parent_type;
	typedef	typename class_traits<Tag>::instance_type
						instance_type;
	/**
		The reference count pointer type to the underlying
		uniquely created instance.  
	 */
	typedef	count_ptr<instance_type>	instance_ptr_type;

	/**
		Container type.
	 */
	typedef	typename class_traits<Tag>::instance_collection_generic_type
						container_type;
	typedef	never_ptr<const container_type>	container_ptr_type;
	typedef	ring_node_derived<this_type>	instance_alias_base_type;

#if 1
	// workaround for shortcoming of gcc: direct reference to
	// the resolved type.  
	// since ring_node_derived derives from the parameter type, 
	// it is considered an incomplete type, and hence unusable.  
	typedef	typename ring_node_derived_iterator_default<this_type>::type
						iterator;
	typedef	typename ring_node_derived_iterator_default<this_type>::const_type
						const_iterator;
#else
	// this is what we really mean
	// doesn't like this: thinks ring_node_derived<...> is incomplete.
	// because this_type is still incomplete, BAH!
	typedef	typename instance_alias_base_type::const_iterator
						const_iterator;
	typedef	typename instance_alias_base_type::iterator
						iterator;
#endif
	typedef	typename actuals_parent_type::alias_actuals_type
						relaxed_actuals_type;
public:
	/**
		During finalization phase, this will be constructed, 
		and references will be copied to neighbors.  
	 */
	instance_ptr_type				instance;
	/**
		Back-reference to the mother container.
		Consider using this to determine "instantiated" state.  
	 */
	container_ptr_type				container;

protected:
	// constructors only intended for children classes
	instance_alias_info() :
		instance(NULL), container(NULL) { }

public:
	/**
		TODO: the following comment is not true.
		This constructor initializes the mother container pointer
		AND (implicitly) recursively instantiates public ports.  
		TODO: does recursive instantiation require actuals?
		Called from instance_array<>::instantiate_indices.
		Perhaps introduce constructor with actuals argument?
	 */
	instance_alias_info(const container_ptr_type m) :
		instance(NULL), container(m) {
#if 0
		// cancel this idea:
		NEVER_NULL(container);
		substructure_parent_type::unroll_port_instances(
			*this->container);
#endif
	}

public:

	// default copy-constructor

	// default destructor, non-virtual?
virtual	~instance_alias_info();

	// default assignment

	/**
		container is inherited from instance_alias_info
	 */
	bool
	valid(void) const { return this->container; }

	void
	check(const container_type* p) const;

// should be pure virtual (but can't)
virtual	const_iterator
	begin(void) const;

// should be pure virtual (but can't)
virtual	const_iterator
	end(void) const;

private:
virtual	iterator
	begin(void);

virtual	iterator
	end(void);

public:
	/**
		Instantiates officially by linking to parent collection.  
		FYI: This is only called by instance_array<0> (scalar)
			in instantiate_indices().
		TODO (2005-07-12): 
			recursively unroll public ports, using 
			subinstance_manager
	 */
	void
	instantiate(const container_ptr_type p, const unroll_context&);

	good_bool
	allocate_state(const unroll_context&) const;

	/**
		Attaches actual parameters to this alias.  
		TODO: make this policy-specific, of course.  
		NOTE: constness is merely for convenience, promising not
			to modify the key.  
		\return true if successful, didn't collide.
	 */
	using actuals_parent_type::attach_actuals;
	using actuals_parent_type::compare_and_update_actuals;
	using actuals_parent_type::compare_actuals;

	const relaxed_actuals_type&
	find_relaxed_actuals(void) const;

	bool
	must_match_type(const this_type&) const;

	// consider: pure virtual multikey_generic<K>

	/**
		Equality comparison is required to determine
		whether or not an object of this type has default value.
	 */
	bool
	operator == (const this_type& i) const {
		return (this->instance == i.instance) &&
			(this->container == i.container);
	}


virtual	void
	dump_alias(ostream& o) const;

	ostream&
	dump_hierarchical_name(ostream&) const;

	/**
		Wants to be pure virtual but can't...
	 */
virtual	void
	write_next_connection(const persistent_object_manager& m, 
		ostream& o) const;

// probably need not be virtual, same for all children classes.
virtual	void
	load_next_connection(const persistent_object_manager& m, 
		istream& i);

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
}	// end namespace ART

#endif	// __OBJECT_INST_INSTANCE_ALIAS_INFO_H__

