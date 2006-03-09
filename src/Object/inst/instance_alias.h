/**
	\file "Object/inst/instance_alias.h"
	Class declarations for aliases.
	Definition of implementation is in "art_object_instance_collection.tcc"
	This file came from "Object/art_object_instance_alias.h"
		in a previous life.  
	$Id: instance_alias.h,v 1.8.4.1 2006/03/09 05:51:09 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_ALIAS_H__
#define	__HAC_OBJECT_INST_INSTANCE_ALIAS_H__

#include "Object/inst/instance_alias_info.h"
#include "Object/expr/types.h"		// for pint_value_type
#include "util/multikey_set.h"

namespace HAC {
namespace entity {
class const_param_expr_list;
using std::string;
using util::multikey_set_element_derived;

//=============================================================================
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
	typedef	class_traits<Tag>			traits_type;
	typedef	multikey_set_element_derived<D, pint_value_type, 
		typename traits_type::instance_alias_base_type>
							parent_type;
	/**
		Dimension-generic container type.  
	 */
	typedef	typename traits_type::instance_collection_generic_type
					instance_collection_generic_type;
	/**
		Dimension-specific container type, 
		should be sub-type of instance_collection_generic_type;
	 */
	// template explicitly required by g++-4.0
	typedef	typename traits_type::template instance_array<D>::type
							container_type;

	typedef	typename traits_type::instance_alias_base_type
					instance_alias_base_type;
	typedef	typename traits_type::instance_alias_info_type
					instance_alias_info_type;
private:
	/**
		grandparent_type is maplikeset_element_derived.
	 */
	typedef	typename parent_type::value_type	grandparent_type;
	/**
		great_grandparent_type is ring_node_derived
	 */
	typedef	typename grandparent_type::parent_type	great_grandparent_type;
	typedef	typename instance_alias_info_type::internal_alias_policy
							internal_alias_policy;
public:
	typedef	typename parent_type::key_type		key_type;
	// or simple_type?
#if USE_ALIAS_RING_NODES
	typedef	typename instance_alias_info_type::const_iterator
							const_iterator;
	typedef	typename instance_alias_info_type::iterator
							iterator;
#else
	typedef	typename instance_alias_info_type::pseudo_const_iterator
							pseudo_const_iterator;
	typedef	typename instance_alias_info_type::pseudo_iterator
							pseudo_iterator;
#endif
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

#if USE_ALIAS_RING_NODES
	const_iterator
	begin(void) const;

	const_iterator
	end(void) const;

private:
	iterator
	begin(void);

	iterator
	end(void);
#else
	pseudo_iterator
	find(void);

	pseudo_const_iterator
	find(void) const;
#endif

public:
	using parent_type::instantiate;
#if !SEPARATE_ALLOCATE_SUBPASS
	using parent_type::allocate_state;
#endif

	void
	write_next_connection(const persistent_object_manager& m, 
		ostream& o) const;

	void
	load_next_connection(const persistent_object_manager& m, 
		istream& i);

	ostream&
	dump_alias(ostream& o, const dump_flags&) const;

	TRACE_ALIAS_BASE_PROTO;
	TRACE_ALIAS_PROTO;

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
public:
	typedef	class_traits<Tag>			traits_type;
	typedef	typename traits_type::instance_alias_base_type
							parent_type;
	typedef	typename traits_type::instance_alias_base_type
					instance_alias_base_type;
	typedef	typename traits_type::instance_collection_generic_type
					instance_collection_generic_type;
	typedef	typename traits_type::instance_alias_info_type
					instance_alias_info_type;
	// template explicitly required by g++-4.0
	typedef	typename traits_type::template instance_array<0>::type
							container_type;
#if USE_ALIAS_RING_NODES
	typedef	typename instance_alias_info_type::const_iterator
							const_iterator;
	typedef	typename instance_alias_info_type::iterator
							iterator;
#else
	typedef	typename instance_alias_info_type::pseudo_const_iterator
							pseudo_const_iterator;
	typedef	typename instance_alias_info_type::pseudo_iterator
							pseudo_iterator;
#endif
public:
	~instance_alias();

	ostream&
	dump_alias(ostream& o, const dump_flags&) const;

#if USE_ALIAS_RING_NODES
	const_iterator
	begin(void) const;

	const_iterator
	end(void) const;

private:
	iterator
	begin(void);

	iterator
	end(void);
#else
	pseudo_iterator
	find(void);

	pseudo_const_iterator
	find(void) const;
#endif

public:
	TRACE_ALIAS_BASE_PROTO;
	TRACE_ALIAS_PROTO;

public:
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
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_ALIAS_H__

