/**
	\file "art_object_instance_int.h"
	Class declarations for built-in and user-defined data instances
	and instance collections.  
	$Id: art_object_instance_int.h,v 1.4 2004/12/12 22:26:34 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_INT_H__
#define	__ART_OBJECT_INSTANCE_INT_H__

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
	An actual instantiated instance of a int.
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
 */
struct int_instance {
	// need back-reference(s) to owner(s) or hierarchical keys?
	int		state;

public:
	int_instance();

};	// end class int_instance

//-----------------------------------------------------------------------------
/**
	An uninitialized reference to a int instance.  
	Only after references are connected, are the actual int instances
	created.  
	Contains attribute fields.  
 */
class int_instance_alias {
public:
	typedef	never_ptr<const int_instance_alias>	alias_ptr_type;
private:
	// consider alloc_ptr
	count_ptr<int_instance>			instance;
	alias_ptr_type				alias;
	// validity fields?
	bool					instantiated;
public:

	bool
	valid(void) const { return instantiated; }

	void
	instantiate(void) { INVARIANT(!instantiated); instantiated = true; }

	const int_instance_alias&
	canonical(void) const {
		alias_ptr_type ptr = alias;
		while (ptr) {
			ptr = ptr->alias;
		}
		return *ptr;
	}

	/// dereference, create
	int_instance&
	operator * () const;

#if 0
	/**
		Alias connection.  
	 */
	int_instance_alias&
	operator = (const int_instance_alias& b) {
		alias = alias_ptr_type(&b);
		return *this;
	}
#endif

	/**
		Whether or not they refer to the same node.
		Check for instantiated?
	 */
	bool
	operator == (const int_instance_alias& i) const {
		return &canonical() == &i.canonical();
	}

	friend
	ostream&
	operator << (ostream&, const int_instance_alias&);

};	// end class int_instance_alias

ostream&
operator << (ostream&, const int_instance_alias&);

//-----------------------------------------------------------------------------
/**
	Interface to collection of data-int instance aliases.  
	TODO: need to add a width parameter, or at least reference
		the fully-specified type.
 */
class int_instance_collection : public datatype_instance_collection {
private:
	typedef	datatype_instance_collection		parent_type;
public:
	typedef	never_ptr<int_instance_alias>		instance_ptr_type;
	typedef	multikey_base<int>			unroll_index_type;
protected:
	explicit
	int_instance_collection(const size_t d) : parent_type(d) { }
public:
	int_instance_collection(const scopespace& o, const string& n, 
		const size_t d);

virtual	~int_instance_collection();

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
virtual int
	connect(const unroll_index_type& k, const int_instance_alias& b) = 0;
#endif
public:

	static
	int_instance_collection*
	make_int_array(const scopespace& o, const string& n, const size_t d);

	static
	persistent*
	construct_empty(const int);

	void
	collect_transient_info(persistent_object_manager& m) const;

protected:
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class int_instance_collection

//-----------------------------------------------------------------------------
#define	INT_ARRAY_TEMPLATE_SIGNATURE		template <size_t D>

INT_ARRAY_TEMPLATE_SIGNATURE
class int_array : public int_instance_collection {
private:
	typedef	int_instance_collection			parent_type;
friend class int_instance_collection;
public:
	typedef	parent_type::instance_ptr_type		instance_ptr_type;
	typedef parent_type::unroll_index_type		unroll_index_type;
	typedef	int_instance_alias			element_type;
	typedef	multikey_qmap<D, int, element_type>	collection_type;

private:
	collection_type					collection;
private:
	int_array();
public:
	int_array(const scopespace& o, const string& n);
	~int_array();

	ostream&
	what(ostream& o) const;

#if 0
	size_t
	dimensions(void) const { return D; }
#elif 0
	size_t
	dimensions(void) const;
#endif

	bool
	is_partially_unrolled(void) const;

	ostream&
	dump_unrolled_instances(ostream& o) const;

	void
	instantiate_indices(const index_collection_item_ptr_type& i);

	const_index_list
	resolve_indices(const const_index_list& l) const;

	instance_ptr_type
	lookup_instance(const unroll_index_type& l) const;

	bool
	lookup_instance_collection(list<instance_ptr_type>& l, 
		const const_range_list& r) const;

	struct key_dumper {
		ostream& os;

		key_dumper(ostream& o) : os(o) { }

		ostream&
		operator () (const typename collection_type::value_type& );
	};	// end struct key_dumper

public:
	PERSISTENT_METHODS_NO_ALLOC_NO_POINTERS
};	// end class int_array

//-----------------------------------------------------------------------------
template <>
class int_array<0> : public int_instance_collection {
private:
	typedef	int_instance_collection		parent_type;
friend class int_instance_collection;
public:
	typedef	parent_type::instance_ptr_type	instance_ptr_type;
	typedef	parent_type::unroll_index_type	unroll_index_type;
private:
	int_instance_alias			the_instance;

private:
	int_array();

public:
	int_array(const scopespace& o, const string& n);
	~int_array();

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
};	// end class int_array (specialized)

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_INT_H__

