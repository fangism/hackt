/**
	\file "art_object_instance_bool.h"
	Class declarations for built-in boolean data instances
	and instance collections.  
	$Id: art_object_instance_bool.h,v 1.5 2004/12/12 22:26:33 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_BOOL_H__
#define	__ART_OBJECT_INSTANCE_BOOL_H__

#include "art_object_instance.h"
#include "memory/pointer_classes.h"

#include "multikey_fwd.h"
#include "multikey_qmap_fwd.h"

namespace ART {
namespace parser {
class token_identifier;
}

namespace entity {
USING_LIST
using std::string;
using parser::token_identifier;
using namespace util::memory;
using namespace MULTIKEY_NAMESPACE;
using namespace MULTIKEY_MAP_NAMESPACE;

//=============================================================================
// class datatype_instance_collection declared in "art_object_instance.h"

//=============================================================================
/**
	An actual instantiated instance of a bool, 
	what used to be called "node".
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
	This is like PrsNode from prsim.  
 */
struct bool_instance {
	// need back-reference(s) to owner(s) or hierarchical keys?
	int		state;

public:
	bool_instance();

};	// end class bool_instance

//-----------------------------------------------------------------------------
/**
	An uninitialized reference to a bool instance.  
	Only after references are connected, are the actual bool instances
	created.  
	Contains attribute fields.  
 */
class bool_instance_alias {
public:
	typedef	never_ptr<const bool_instance_alias>	alias_ptr_type;
private:
	// consider alloc_ptr...
	count_ptr<bool_instance>		instance;
	alias_ptr_type				alias;
	// validity fields?
	bool					instantiated;
public:

	bool_instance_alias() : instance(NULL), alias(NULL), 
		instantiated(false) { }

explicit bool_instance_alias(const alias_ptr_type& p) :
		instance(NULL), alias(p), instantiated(true) { }

	// default copy constructor

	// default destructor

	bool
	valid(void) const { return instantiated; }

	void
	instantiate(void) { INVARIANT(!instantiated); instantiated = true; }

	/// dereference, create
	bool_instance&
	operator * () const;

	const bool_instance_alias&
	canonical(void) const {
		alias_ptr_type ptr = alias;
		while (ptr) {
			ptr = ptr->alias;
		}
		return *ptr;
	}

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
	operator == (const bool_instance_alias& b) const {
		return &canonical() == &b.canonical();
	}

	friend
	ostream&
	operator << (ostream&, const bool_instance_alias&);

};	// end class bool_instance_alias

ostream&
operator << (ostream&, const bool_instance_alias&);

//-----------------------------------------------------------------------------
/**
	Interface to collection of data-bool instance aliases.  
 */
class bool_instance_collection : public datatype_instance_collection {
private:
	typedef	datatype_instance_collection		parent_type;
public:
	typedef	never_ptr<bool_instance_alias>		instance_ptr_type;
	typedef	multikey_base<int>			unroll_index_type;
protected:
	/// private empty constructor
	explicit
	bool_instance_collection(const size_t d) : parent_type(d) { }
public:
	bool_instance_collection(const scopespace& o, const string& n, 
		const size_t d);

virtual	~bool_instance_collection();

#if 0
virtual	size_t dimensions(void) const = 0;
#endif

virtual	ostream&
	what(ostream&) const = 0;

virtual	bool
	is_partially_unrolled(void) const = 0;

#if 0
	// this could just return hard-coded built-in type...
	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;
#endif

	count_ptr<instance_reference_base>
	make_instance_reference(void) const;

virtual	void
	instantiate_indices(const index_collection_item_ptr_type& i) = 0;

virtual instance_ptr_type
	lookup_instance(const unroll_index_type& i) const = 0;

virtual	bool
	lookup_instance_collection(list<instance_ptr_type>& l, 
		const const_range_list& r) const = 0;

virtual	const_index_list
	resolve_indices(const const_index_list& l) const = 0;

#if 0
virtual bool
	connect(const unroll_index_type& k, const bool_instance_alias& b) = 0;
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
	typedef	bool_instance_collection		parent_type;
public:
	typedef	parent_type::instance_ptr_type		instance_ptr_type;
	typedef parent_type::unroll_index_type		unroll_index_type;
	typedef	bool_instance_alias			element_type;
	typedef	multikey_qmap<D, int, element_type>	collection_type;

private:
	collection_type					collection;

private:
	bool_array();

public:
	bool_array(const scopespace& o, const string& n);
	~bool_array();

#if 0
	size_t
	dimensions(void) const { return D; }
#elif 0
	// cannot inline anymore because it is called by the base
	// class will call it from other functions, we need the symbol
	// to be emitted.  
	size_t
	dimensions(void) const;
#endif

	ostream&
	what(ostream&) const;

	bool
	is_partially_unrolled(void) const;

	ostream&
	dump_unrolled_instances(ostream& o) const;

	void
	instantiate_indices(const index_collection_item_ptr_type& i);

	instance_ptr_type
	lookup_instance(const unroll_index_type& l) const;

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
	PERSISTENT_METHODS_NO_ALLOC_NO_POINTERS
};	// end class bool_array

//-----------------------------------------------------------------------------
template <>
class bool_array<0> : public bool_instance_collection {
friend class bool_instance_collection;
	typedef	bool_instance_collection	parent_type;
private:
	bool_instance_alias			the_instance;

private:
	bool_array();

public:
	bool_array(const scopespace& o, const string& n);
	~bool_array();

#if 0
	size_t
	dimensions(void) const { return 0; }
#elif 0
	size_t
	dimensions(void) const;
#endif

	ostream&
	what(ostream&) const;

	bool
	is_partially_unrolled(void) const;

	ostream&
	dump_unrolled_instances(ostream& o) const;

	void
	instantiate_indices(const index_collection_item_ptr_type& i);

	instance_ptr_type
	lookup_instance(const unroll_index_type& l) const;

	bool
	lookup_instance_collection(list<instance_ptr_type>& l, 
		const const_range_list& r) const;

	const_index_list
	resolve_indices(const const_index_list& l) const;

public:
	PERSISTENT_METHODS_NO_ALLOC_NO_POINTERS

};	// end class bool_array (specialized)

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_BOOL_H__

