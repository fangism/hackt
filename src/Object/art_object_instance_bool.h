/**
	\file "art_object_instance_bool.h"
	Class declarations for built-in boolean data instances
	and instance collections.  
	$Id: art_object_instance_bool.h,v 1.9.2.2.2.1 2005/02/11 06:14:26 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_BOOL_H__
#define	__ART_OBJECT_INSTANCE_BOOL_H__

#include "art_object_instance.h"
#include "memory/pointer_classes.h"
#include "memory/list_vector_pool_fwd.h"

#include "multikey_set.h"
#include "ring_node.h"
#include "multikey.h"
#include <set>

// #include "multikey_qmap_fwd.h"

namespace ART {
namespace entity {
USING_LIST
USING_CONSTRUCT
using std::set;
using std::ostream;
using std::string;
using util::ring_node;
using namespace util::memory;
using MULTIKEY_NAMESPACE::multikey;
using util::multikey_set;
using util::multikey_set_element;
// using QMAP_NAMESPACE::qmap;
// using MULTIKEY_MAP_NAMESPACE::multikey_map;

//=============================================================================
// class datatype_instance_collection declared in "art_object_instance.h"

// forward declaration
class bool_instance_alias_base;

//=============================================================================
/**
	An actual instantiated instance of a bool, 
	what used to be called "node".
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
	This is like PrsNode from prsim.  

	Should be pool allocated for efficiency.  
 */
struct bool_instance {
	// need back-reference(s) to owner(s) or hierarchical keys?
	int		state;

public:
	bool_instance();

};	// end class bool_instance

//-----------------------------------------------------------------------------
/**
	Information structure for a heirarchical name for a bool.  
	This may be extended arbitrarily to contain attributes.  
	Each object of this type represents a unique qualified name.  
 */
class bool_instance_alias_info {
public:
	/**
		The reference count pointer type to the underlying
		uniquely created instance.  
	 */
	typedef	count_ptr<bool_instance>		instance_ptr_type;

	/**
		Container type.
	 */
	typedef	bool_instance_collection		container_type;

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
	never_ptr<const container_type>			container;
#if 0
	/**
		Whether or not this is truly instantiated.  
		Depends on implementation.  
	 */
	bool						instantiated;
#endif
public:
	bool_instance_alias_info() :
		instance(NULL), container(NULL) { }

	bool_instance_alias_info(const never_ptr<const container_type> m) :
		instance(NULL), container(m) { }

	// default copy-constructor

	// default destructor

	// default assignment
	
};	// end class bool_instance_alias_info

//-----------------------------------------------------------------------------
/**
	An uninitialized reference to a bool instance.  
	Only after references are connected, are the actual bool instances
	created.  
	Contains attribute fields, later on.  
 */
class bool_instance_alias_base : public ring_node<bool_instance_alias_info> {
public:
	/**
		Information contained herein (inside the ring_node).
	 */
	typedef	bool_instance_alias_info		info_type;
	/**
		This parent-type contains the next pointer to alias, 
		which is initialized as pointing to itself.  
		The field "value" (info_type) is inherited publicly.  
	 */
	typedef	ring_node<info_type>			ring_node_type;
public:
	/**
		Default constructor will initialize the info.  
	 */
	bool_instance_alias_base() : ring_node_type() { }

	bool_instance_alias_base(
		const never_ptr<const bool_instance_collection> p) :
		ring_node_type(info_type(p)) { }

#if 0
	explicit
	bool_instance_alias(const alias_ptr_type& p) :
		instance(NULL), alias(p), instantiated(true) { }
#endif

	// default copy constructor

virtual	~bool_instance_alias_base() { }

	bool
	valid(void) const { return value.container; }

#if 0
	void
	instantiate(void) {
		INVARIANT(!value.instantiated);
		value.instantiated = true;
	}
#else
	/**
		Instantiates officially by linking to parent collection.  
	 */
	void
	instantiate(const never_ptr<const bool_instance_collection> p) {
		NEVER_NULL(p);
		INVARIANT(!value.container);
		value.container = p;
	}
#endif
#if 0
	/// dereference, create
	bool_instance&
	operator * () const;
#endif

#if 0
	const bool_instance_alias&
	canonical(void) const {
		alias_ptr_type ptr = alias;
		while (ptr) {
			ptr = ptr->alias;
		}
		return *ptr;
	}
#endif

#if 0
	// possible obfuscation
	/**
		Alias connection.  
	 */
	bool_instance_alias&
	operator = (const bool_instance_alias& b) {
		alias = alias_ptr_type(&b);
		return *this;
	}
#endif

	/**
		Whether or not they refer to the same node.
		Check for instantiated?
	 */
	bool
	operator == (const bool_instance_alias_base& b) const {
		return this->contains(b);
		// return &canonical() == &b.canonical();
	}

	friend
	ostream&
	operator << (ostream&, const bool_instance_alias_base&);

};	// end class bool_instance_alias

ostream&
operator << (ostream&, const bool_instance_alias_base&);

//-----------------------------------------------------------------------------
/**
	Extends a bool_instance_alias_base with a multikey, to be used
	in a set.  

	Alternate idea, use a multikey_generic instead of dimension-specific.
 */
template <size_t D>
class bool_instance_alias :
#if 1
	public bool_instance_alias_base
#else
	public multikey_set_element<D, pint_value, bool_instance_alias_base>
#endif
{
private:
	typedef	bool_instance_alias<D>			this_type;
public:
	typedef	multikey<D, pint_value_type>		key_type;
protected:
	key_type					key;
public:
	bool_instance_alias() : bool_instance_alias_base(), key() { }

	/**
		Implicit constructor for creating an empty alias element, 
		used for creating keys to search sets.  
	 */
	bool_instance_alias(const key_type& k) : 
			bool_instance_alias_base(), key(k) { }

	bool_instance_alias(const never_ptr<const bool_instance_collection> p, 
			const key_type& k) : 
			bool_instance_alias_base(p), key(k) { }

	~bool_instance_alias();

	// compare keys
	bool
	operator < (const this_type& b) const {
		return key < b.key;
	}

};	// end class bool_instance_alias

//-----------------------------------------------------------------------------
/**
	Interface to collection of data-bool instance aliases.  
 */
class bool_instance_collection : public datatype_instance_collection {
private:
	typedef	datatype_instance_collection		parent_type;
public:
	typedef	parent_type::type_ref_ptr_type		type_ref_ptr_type;
	typedef	never_ptr<bool_instance_alias_base>	instance_ptr_type;
	/// boolean data (node) has no parameters
	typedef	void					param_type;
protected:
	/// private empty constructor
	explicit
	bool_instance_collection(const size_t d) : parent_type(d) { }
public:
	bool_instance_collection(const scopespace& o, const string& n, 
		const size_t d);

virtual	~bool_instance_collection();

virtual	ostream&
	what(ostream&) const = 0;

	ostream&
	type_dump(ostream&) const;

virtual	bool
	is_partially_unrolled(void) const = 0;

#if 0
	// this could just return hard-coded built-in type...
	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;
#endif

	bool
	commit_type(const type_ref_ptr_type& );

	count_ptr<instance_reference_base>
	make_instance_reference(void) const;

virtual	void
	instantiate_indices(const index_collection_item_ptr_type& i) = 0;

virtual instance_ptr_type
	lookup_instance(const multikey_index_type& i) const = 0;

virtual	bool
	lookup_instance_collection(list<instance_ptr_type>& l, 
		const const_range_list& r) const = 0;

virtual	const_index_list
	resolve_indices(const const_index_list& l) const = 0;

#if 0
virtual bool
	connect(const multikey_index_type& k, const bool_instance_alias& b) = 0;
#endif
public:

	static
	bool_instance_collection*
	make_bool_array(const scopespace& o, const string& n, const size_t d);

	static
	persistent*
	construct_empty(const int);

	void
	collect_transient_info(persistent_object_manager& m) const;

protected:
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class bool_instance_collection

//-----------------------------------------------------------------------------
#define	BOOL_ARRAY_TEMPLATE_SIGNATURE		template <size_t D>

BOOL_ARRAY_TEMPLATE_SIGNATURE
class bool_array : public bool_instance_collection {
private:
friend class bool_instance_collection;
	typedef	bool_array<D>				this_type;
	typedef	bool_instance_collection		parent_type;
public:
	typedef	parent_type::instance_ptr_type		instance_ptr_type;
#if 0
	typedef	bool_instance_alias<D>			element_type;
#else
	typedef	multikey_set_element<D, 
			pint_value_type, bool_instance_alias<D> >
							element_type;
#endif
//	typedef	multikey_map<D, pint_value_type, element_type, qmap>
	typedef	multikey_set<D, element_type>
							collection_type;
#if 0
	typedef	typename element_type::key_type		key_type;
#else
	typedef	typename collection_type::key_type	key_type;
#endif

protected:
	typedef	typename collection_type::iterator	iterator;
	typedef	typename collection_type::const_iterator
							const_iterator;
private:
	collection_type					collection;

private:
	bool_array();

public:
	bool_array(const scopespace& o, const string& n);
	~bool_array();

	ostream&
	what(ostream&) const;

	bool
	is_partially_unrolled(void) const;

	ostream&
	dump_unrolled_instances(ostream& o) const;

	void
	instantiate_indices(const index_collection_item_ptr_type& i);

	instance_ptr_type
	lookup_instance(const multikey_index_type& l) const;

	bool
	lookup_instance_collection(list<instance_ptr_type>& l, 
		const const_range_list& r) const;

	const_index_list
	resolve_indices(const const_index_list& l) const;

	struct key_dumper {
		ostream& os;

		key_dumper(ostream& o) : os(o) { }

		ostream&
		operator () (const typename collection_type::value_type& );
	};	// end struct key_dumper

public:
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC_NO_POINTERS
};	// end class bool_array

//-----------------------------------------------------------------------------
template <>
class bool_array<0> : public bool_instance_collection {
friend class bool_instance_collection;
	typedef	bool_instance_collection	parent_type;
	typedef	bool_array<0>			this_type;
private:
	bool_instance_alias_base		the_instance;

private:
	bool_array();

public:
	bool_array(const scopespace& o, const string& n);
	~bool_array();

	ostream&
	what(ostream&) const;

	bool
	is_partially_unrolled(void) const;

	ostream&
	dump_unrolled_instances(ostream& o) const;

	void
	instantiate_indices(const index_collection_item_ptr_type& i);

	instance_ptr_type
	lookup_instance(const multikey_index_type& l) const;

	bool
	lookup_instance_collection(list<instance_ptr_type>& l, 
		const const_range_list& r) const;

	const_index_list
	resolve_indices(const const_index_list& l) const;

public:
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC_NO_POINTERS
	LIST_VECTOR_POOL_ESSENTIAL_FRIENDS
	LIST_VECTOR_POOL_STATIC_DECLARATIONS
};	// end class bool_array (specialized)

typedef	bool_array<0>	bool_scalar;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_BOOL_H__

