/**
	\file "Object/art_object_instance_alias.h"
	Class declarations for aliases.
	Definition of implementation is in "art_object_instance_collection.tcc"
	$Id: art_object_instance_alias.h,v 1.5.10.1 2005/07/05 07:59:44 fang Exp $
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
#include "Object/art_object_classification_fwd.h"

namespace ART {
namespace entity {
USING_CONSTRUCT
using std::ostream;
using std::istream;
using std::string;
using util::ring_node_derived;
using util::memory::never_ptr;
using util::memory::count_ptr;
using util::multikey_set_element_derived;
using util::persistent_object_manager;

//=============================================================================
// class datatype_instance_collection declared in "art_object_instance.h"

// class instance;

template <class, size_t>
class instance_array;

//=============================================================================
#define	INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE				\
template <class Tag>

#define	INSTANCE_ALIAS_INFO_CLASS					\
instance_alias_info<Tag>

/**
	Information structure for a heirarchical name for a bool.  
	This may be extended arbitrarily to contain attributes.  
	Each object of this type represents a unique qualified name.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
class instance_alias_info {
	typedef	INSTANCE_ALIAS_INFO_CLASS	this_type;
public:
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
	// constructors only intended for children classes
	instance_alias_info(const container_ptr_type m) :
		instance(NULL), container(m) { }

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

	/**
		Instantiates officially by linking to parent collection.  
	 */
	void
	instantiate(const container_ptr_type p) {
		NEVER_NULL(p);
		INVARIANT(!this->container);
		this->container = p;
	}

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

	void
	write_next_connection(const persistent_object_manager& m, 
		ostream& o) const;

	void
	load_next_connection(const persistent_object_manager& m, 
		istream& i);

	void
	dump_alias(ostream& o) const;

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
public:
	~instance_alias();

	void
	dump_alias(ostream& o) const;

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

