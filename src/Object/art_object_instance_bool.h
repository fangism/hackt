/**
	\file "art_object_instance_bool.h"
	Class declarations for built-in boolean data instances
	and instance collections.  
	$Id: art_object_instance_bool.h,v 1.9.2.2.2.6 2005/02/15 22:31:39 fang Exp $
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
using util::ring_node_derived;
using namespace util::memory;
using util::multikey;
using util::multikey_set;
using util::multikey_set_element;
using util::multikey_set_element_derived;

//=============================================================================
// class datatype_instance_collection declared in "art_object_instance.h"

class bool_instance;

template <size_t>
class bool_array;

// forward declaration
#if 0
class bool_instance_alias_base;
#endif

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
	instantiate(const never_ptr<const bool_instance_collection> p) {
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
		return instance == i.instance &&
			container == i.container;
	}

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
	};

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

#if 0
/**
	An uninitialized reference to a bool instance.  
	Only after references are connected, are the actual bool instances
	created.  
	Contains attribute fields, later on.  
	This is included directly by bool_array<0>.
 */
class bool_instance_alias_base : 
		public ring_node_derived<bool_instance_alias_info> {
	typedef	bool_instance_alias_base		this_type;
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
	typedef	ring_node_derived<info_type>		ring_node_type;
#if 0
protected:
	info_type					info;
#endif
public:
	/**
		Default constructor will initialize the info.  
	 */
	bool_instance_alias_base() : ring_node_type() { }

	explicit
	bool_instance_alias_base(
		const never_ptr<const bool_instance_collection> p) :
		ring_node_type(info_type(p)) { }
//		ring_node_type(), info(p) { }

	// default copy constructor

	/**
		Technically, references to bool_instance_alias_base
		will never refer to subclass of this --
		The containers of bool_instance_aliases are
		only dimensions-specific, and the ring_node 
		pointer cycles are never used for destruction.  
		Nevertheless, it doesn't hurt to keep this virtual.  
	 */
virtual	~bool_instance_alias_base();

#if 0
	/**
		container is inherited from bool_instance_alias_info
	 */
	bool
	valid(void) const { return container; }

	/**
		Instantiates officially by linking to parent collection.  
	 */
	void
	instantiate(const never_ptr<const bool_instance_collection> p) {
		NEVER_NULL(p);
		INVARIANT(!container);
		container = p;
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

#if 0
	/**
		Whether or not they refer to the same node.
		Check for instantiated?

		Pushed down to children classes.  
	 */
	bool
	operator == (const bool_instance_alias_base& b) const {
		return this->contains(b);
		// return &canonical() == &b.canonical();
	}
#endif

#if 0
	friend
	ostream&
	operator << (ostream&, const bool_instance_alias_base&);
#endif

public:
	// this class is not truly persistent but contains
	// pointers to persistent types.  
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC
};	// end class bool_instance_alias_base
#endif

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
	// need back-reference(s) to owner(s) or hierarchical keys?
//	int		state;

	// need one back-reference to one alias (connected in a ring)
	never_ptr<const bool_instance_alias_base>
			back_ref;

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
#if 0
		public bool_instance_alias_base
#else
		public multikey_set_element_derived<
			D, pint_value_type, bool_instance_alias_base>
#endif
{
private:
	typedef	bool_instance_alias<D>			this_type;
public:
#if 0
	typedef	bool_instance_alias_base		parent_type;
#else
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
#if 0
	/**
		great_great_grandparent_type is bool_instance_alias_info
	 */
	typedef	typename great_grandparent_type::parent_type
						great_great_grandparent_type;
#endif
#endif
public:
#if 0
	typedef	multikey<D, pint_value_type>		key_type;
#else
	typedef	typename parent_type::key_type		key_type;
#endif
	// or simple_type?
public:
	bool_instance_alias() : parent_type() { }

	/**
		Implicit constructor for creating an empty alias element, 
		used for creating keys to search sets.  
	 */
	bool_instance_alias(const key_type& k) : parent_type(k) { }

	bool_instance_alias(const key_type& k, 
		never_ptr<const bool_instance_collection> p) :

			parent_type(k, grandparent_type(

				great_grandparent_type(p))) { }

#if 0
	bool_instance_alias(const never_ptr<const bool_instance_collection> p, 
			const key_type& k) : 
			bool_instance_alias_base(p), key(k) { }
#endif

	~bool_instance_alias();

	void
	write_next_connection(const persistent_object_manager& m, 
		ostream& o) const;


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
	write_next_connection(const persistent_object_manager& m, 
		ostream& o) const;


public:
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC
};

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
// to grant access to this private constructor
friend class bool_instance_collection;
	typedef	bool_array<D>				this_type;
	typedef	bool_instance_collection		parent_type;
public:
	typedef	parent_type::instance_ptr_type		instance_ptr_type;
#if 0
	typedef	multikey_set_element_derived<D, 
			pint_value_type, bool_instance_alias<D> >
							element_type;
#else
	typedef	bool_instance_alias<D>			element_type;
#endif
	typedef	multikey_set<D, element_type>		collection_type;
	typedef	typename element_type::key_type		key_type;
	typedef	typename collection_type::value_type	value_type;
protected:
#if 0
	typedef	typename collection_type::reference	reference;
#else
	typedef	element_type&				reference;
#endif
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
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC_NO_POINTERS
};	// end class bool_array

//-----------------------------------------------------------------------------
template <>
class bool_array<0> : public bool_instance_collection {
friend class bool_instance_collection;
	typedef	bool_instance_collection	parent_type;
	typedef	bool_array<0>			this_type;
public:
#if 0
	typedef	bool_instance_alias_base	instance_type;
#else
	typedef	bool_instance_alias<0>		instance_type;
#endif
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
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC_NO_POINTERS
	LIST_VECTOR_POOL_ESSENTIAL_FRIENDS
	LIST_VECTOR_POOL_STATIC_DECLARATIONS
};	// end class bool_array (specialized)

typedef	bool_array<0>	bool_scalar;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_BOOL_H__

