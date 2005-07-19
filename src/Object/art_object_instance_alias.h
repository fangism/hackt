/**
	\file "Object/art_object_instance_alias.h"
	Class declarations for aliases.
	Definition of implementation is in "art_object_instance_collection.tcc"
	$Id: art_object_instance_alias.h,v 1.5.10.9 2005/07/19 23:28:25 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INSTANCE_ALIAS_H__
#define	__OBJECT_ART_OBJECT_INSTANCE_ALIAS_H__

#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"
#include "util/memory/list_vector_pool_fwd.h"
#include "util/multikey_set.h"
#include "util/ring_node.h"
#include "util/persistent.h"
#include "Object/expr/types.h"
#include "Object/inst/substructure_alias_base.h"
#include "Object/traits/class_traits_fwd.h"

namespace ART {
namespace entity {
class const_param_expr_list;
USING_CONSTRUCT
using std::ostream;
using std::istream;
using std::string;
using util::ring_node_derived;
using util::ring_node_derived_iterator_default;
using util::memory::never_ptr;
using util::memory::count_ptr;
using util::multikey_set_element_derived;
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

//-----------------------------------------------------------------------------
#define	INSTANCE_ALIAS_TEMPLATE_SIGNATURE				\
template <class Tag, size_t D>

#define	INSTANCE_ALIAS_CLASS						\
instance_alias<Tag,D>

#define	KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE			\
template <class Tag>

#define	KEYLESS_INSTANCE_ALIAS_CLASS					\
instance_alias<Tag,0>

/**
	Extends a instance_alias_base_type with a multikey, to be used
	in a set.  

	Note: Don't derive from multikey_set_element.  

	Alternate idea, use a multikey_generic instead of dimension-specific.
 */
INSTANCE_ALIAS_TEMPLATE_SIGNATURE
class instance_alias :
	public multikey_set_element_derived<D, pint_value_type, 
		typename instance_alias_info<Tag>::instance_alias_base_type> {
	typedef	INSTANCE_ALIAS_CLASS			this_type;
public:
	typedef	multikey_set_element_derived<D, pint_value_type, 
		typename class_traits<Tag>::instance_alias_base_type>
							parent_type;
	/**
		Dimension-generic container type.  
	 */
	typedef	typename class_traits<Tag>::instance_collection_generic_type
					instance_collection_generic_type;
	/**
		Dimension-specific container type, 
		should be sub-type of instance_collection_generic_type;
	 */
	// template explicitly required by g++-4.0
	typedef	typename class_traits<Tag>::template instance_array<D>::type
							container_type;

	typedef	typename class_traits<Tag>::instance_alias_base_type
					instance_alias_base_type;
private:
	/**
		grandparent_type is maplikeset_element_derived.
	 */
	typedef	typename parent_type::value_type	grandparent_type;
	/**
		great_grandparent_type is ring_node_derived
	 */
	typedef	typename grandparent_type::parent_type	great_grandparent_type;
public:
	typedef	typename parent_type::key_type		key_type;
	// or simple_type?
	typedef	typename instance_alias_info<Tag>::const_iterator
							const_iterator;
	typedef	typename instance_alias_info<Tag>::iterator
							iterator;
public:
	instance_alias() : parent_type() { }

	/**
		Implicit constructor for creating an empty alias element, 
		used for creating keys to search sets.  
	 */
	instance_alias(const key_type& k) : parent_type(k) { }

	instance_alias(const key_type& k, 
		const never_ptr<const instance_collection_generic_type> p) :
			parent_type(k, grandparent_type(
				great_grandparent_type(p))) { }

	~instance_alias();

	/**
		Implicit conversion need for some multikey_assoc
		algorithms.  
	 */
	operator const key_type& () const { return this->key; }

	const_iterator
	begin(void) const;

	const_iterator
	end(void) const;

	void
	write_next_connection(const persistent_object_manager& m, 
		ostream& o) const;

	void
	load_next_connection(const persistent_object_manager& m, 
		istream& i);

	void
	dump_alias(ostream& o) const;

#if 0
	ostream&
	dump_hierarchical_name(ostream&) const;
#endif

	/**
		Use with maplikeset_element requires comparison operator.  
		Not sure if this is the correct thing to do.  
		TODO: Be sure to review when this is used in the context
		of instance collections and connections.  
	 */
	bool
	operator == (const parent_type& p) const {
		return this->contains(p);
	}

public:
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC

};	// end class instance_alias

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Specialization, not derived from multikey_set_element because
	it has no key.  
	Need final overrider for pure virtual base.  
 */
KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
class instance_alias<Tag,0> :
	public class_traits<Tag>::instance_alias_base_type {
private:
	typedef	KEYLESS_INSTANCE_ALIAS_CLASS		this_type;
	typedef	typename class_traits<Tag>::instance_alias_base_type
							parent_type;
public:
	typedef	typename class_traits<Tag>::instance_alias_base_type
					instance_alias_base_type;
	typedef	typename class_traits<Tag>::instance_collection_generic_type
					instance_collection_generic_type;
	// template explicitly required by g++-4.0
	typedef	typename class_traits<Tag>::template instance_array<0>::type
							container_type;
	typedef	typename instance_alias_info<Tag>::const_iterator
							const_iterator;
	typedef	typename instance_alias_info<Tag>::iterator
							iterator;
public:
	~instance_alias();

	void
	dump_alias(ostream& o) const;

#if 0
	ostream&
	dump_hierarchical_name(ostream&) const;
#endif

	const_iterator
	begin(void) const;

	const_iterator
	end(void) const;

	void
	write_next_connection(const persistent_object_manager& m, 
		ostream& o) const;

	void
	load_next_connection(const persistent_object_manager& m, 
		istream& i);

public:
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC
};	// end class instance_alias<Tag,0>

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INSTANCE_ALIAS_H__

