/**
	\file "art_object_instance_bool.h"
	Class declarations for built-in boolean data instances
	and instance collections.  
	$Id: art_object_instance_bool.h,v 1.9.2.3 2005/02/17 00:10:14 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_BOOL_H__
#define	__ART_OBJECT_INSTANCE_BOOL_H__

#include "art_object_instance.h"
#include "memory/pointer_classes.h"
#include "memory/list_vector_pool_fwd.h"

#include <set>
#include "multikey_set.h"
#include "ring_node.h"


namespace ART {
namespace entity {
USING_LIST
USING_CONSTRUCT
using std::set;
using std::ostream;
using std::string;
// using util::ring_node;
using util::ring_node_derived;
using namespace util::memory;
using util::multikey_set;
// using util::multikey_set_element;
using util::multikey_set_element_derived;

//=============================================================================
// class datatype_instance_collection declared in "art_object_instance.h"

class bool_instance;

template <size_t>
class bool_array;

//=============================================================================
/**
	Information structure for a heirarchical name for a bool.  
	This may be extended arbitrarily to contain attributes.  
	Each object of this type represents a unique qualified name.  
 */
class bool_instance_alias_info {
	typedef	bool_instance_alias_info		this_type;
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

protected:
	// constructors only intended for children classes
	bool_instance_alias_info() :
		instance(NULL), container(NULL) { }

public:
	// constructors only intended for children classes
	bool_instance_alias_info(const never_ptr<const container_type> m) :
		instance(NULL), container(m) { }

public:

	// default copy-constructor

	// default destructor, non-virtual?
virtual	~bool_instance_alias_info();

	// default assignment

	/**
		container is inherited from bool_instance_alias_info
	 */
	bool
	valid(void) const { return container; }

	/**
		Instantiates officially by linking to parent collection.  
	 */
	void
	instantiate(const never_ptr<const container_type> p) {
		NEVER_NULL(p);
		INVARIANT(!container);
		container = p;
	}

	// consider: pure virtual multikey_generic<K>

	/**
		Equality comparison is required to determine
		whether or not an object of this type has default value.
	 */
	bool
	operator == (const this_type& i) const {
		return instance == i.instance && container == i.container;
	}


virtual	void
	dump_alias(ostream& o) const;
	/**
		Wants to be pure virtual but can't...
	 */
virtual	void
	write_next_connection(const persistent_object_manager& m, 
		ostream& o) const;

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
		operator () (const bool_instance_alias_info&);
	};	// end class transient_info_collector

};	// end class bool_instance_alias_info

//-----------------------------------------------------------------------------
/**
	Trying typedef of the alias_base class.  
	Remember that this contains a pointer to the next alias!
	Thus the aliases that derive from this are responsible
	for managing the persistence of the continuation pointers!
 */
typedef	ring_node_derived<bool_instance_alias_info>
						bool_instance_alias_base;

ostream&
operator << (ostream&, const bool_instance_alias_base&);

//-----------------------------------------------------------------------------
/**
	An actual instantiated instance of a bool, 
	what used to be called "node".
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
	This is like PrsNode from prsim.  

	Should be pool allocated for efficiency.  
 */
class bool_instance : public persistent {
	// need one back-reference to one alias (connected in a ring)
	never_ptr<const bool_instance_alias_base>	back_ref;
public:
	bool_instance();

public:
	PERSISTENT_METHODS_DECLARATIONS

};	// end class bool_instance

//-----------------------------------------------------------------------------
/**
	Extends a bool_instance_alias_base with a multikey, to be used
	in a set.  

	Note: Don't derive from multikey_set_element.  

	Alternate idea, use a multikey_generic instead of dimension-specific.
 */
template <size_t D>
class bool_instance_alias :
		public multikey_set_element_derived<
			D, pint_value_type, bool_instance_alias_base> {
	typedef	bool_instance_alias<D>			this_type;
public:
	typedef	multikey_set_element_derived<
			D, pint_value_type, bool_instance_alias_base>
							parent_type;
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
	bool_instance_alias() : parent_type() { }

	/**
		Implicit constructor for creating an empty alias element, 
		used for creating keys to search sets.  
	 */
	bool_instance_alias(const key_type& k) : parent_type(k) { }

	bool_instance_alias(const key_type& k, 
		const never_ptr<const bool_instance_collection> p) :
			parent_type(k, grandparent_type(
				great_grandparent_type(p))) { }

	~bool_instance_alias();

	void
	write_next_connection(const persistent_object_manager& m, 
		ostream& o) const;

	void
	dump_alias(ostream& o) const;

#if 0
	// compare keys
	bool
	operator < (const this_type& b) const {
		return key < b.key;
	}
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

#if 0
	friend
	ostream&
	operator << <>(ostream&, const bool_instance_alias<D>&);
#endif

public:
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC

};	// end class bool_instance_alias

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Specialization, not derived from multikey_set_element because
	it has no key.  
	Need final overrider for pure virtual base.  
 */
template <>
class bool_instance_alias<0> : public bool_instance_alias_base {
private:
	typedef	bool_instance_alias<0>			this_type;
	typedef	bool_instance_alias_base		parent_type;
public:
	~bool_instance_alias();

	void
	dump_alias(ostream& o) const;

	void
	write_next_connection(const persistent_object_manager& m, 
		ostream& o) const;

public:
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC
};	// end class bool_instance_alias<0>

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

#if 0
	// left to children classes to implement, now that there are pointers to visit...

	void
	collect_transient_info(persistent_object_manager& m) const;
#endif

protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class bool_instance_collection

//-----------------------------------------------------------------------------
#define	BOOL_ARRAY_TEMPLATE_SIGNATURE		template <size_t D>

BOOL_ARRAY_TEMPLATE_SIGNATURE
class bool_array : public bool_instance_collection {
// to grant access to this private constructor
friend class bool_instance_collection;
	typedef	bool_array<D>				this_type;
	typedef	bool_instance_collection		parent_type;
public:
	typedef	parent_type::instance_ptr_type		instance_ptr_type;
	typedef	bool_instance_alias<D>			element_type;
	typedef	multikey_set<D, element_type>		collection_type;
	typedef	typename element_type::key_type		key_type;
	typedef	typename collection_type::value_type	value_type;
protected:
	typedef	element_type&				reference;
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

	class element_writer {
		ostream& os;
		const persistent_object_manager& pom;
	public:
		element_writer(const persistent_object_manager& m, ostream& o)
			: os(o), pom(m) { }

		void
		operator () (const element_type& ) const;
	};	// end struct element_writer

	class connection_writer {
		ostream& os;
		const persistent_object_manager& pom;
	public:
		connection_writer(const persistent_object_manager& m, 
			ostream& o) : os(o), pom(m) { }

		void
		operator () (const element_type& ) const;
	};	// end struct connection_writer

	struct key_dumper {
		ostream& os;

		key_dumper(ostream& o) : os(o) { }

		ostream&
		operator () (const value_type& );
	};	// end struct key_dumper

public:
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC
};	// end class bool_array

//-----------------------------------------------------------------------------
template <>
class bool_array<0> : public bool_instance_collection {
friend class bool_instance_collection;
	typedef	bool_instance_collection	parent_type;
	typedef	bool_array<0>			this_type;
public:
	/**
		Instance type is a ring-connectible (key-less) alias.  
	 */
	typedef	bool_instance_alias<0>		instance_type;
private:
	instance_type				the_instance;

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
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC
	LIST_VECTOR_POOL_ESSENTIAL_FRIENDS
	LIST_VECTOR_POOL_STATIC_DECLARATIONS
};	// end class bool_array (specialized)

typedef	bool_array<0>	bool_scalar;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_BOOL_H__

