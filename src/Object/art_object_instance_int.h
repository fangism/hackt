/**
	\file "art_object_instance_int.h"
	Class declarations for built-in and user-defined data instances
	and instance collections.  
	$Id: art_object_instance_int.h,v 1.9.2.2.2.2 2005/02/15 07:32:03 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_INT_H__
#define	__ART_OBJECT_INSTANCE_INT_H__

#include "art_object_instance.h"
#include "memory/pointer_classes.h"

#include "multikey_fwd.h"
#include "multikey_qmap_fwd.h"


namespace ART {
namespace entity {
USING_LIST
using std::string;
using namespace util::memory;
using util::qmap;
using util::multikey_map;

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
	int_instance_alias() : instance(NULL), alias(NULL), 
		instantiated(false) { }

	// default destructor suffices

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
	typedef	parent_type::type_ref_ptr_type		type_ref_ptr_type;
	typedef	never_ptr<int_instance_alias>		instance_ptr_type;
	typedef	pint_value_type				param_type;
private:
	/**
		The bit-width of the integers in this collection.  
		Per collection, this is set once at unroll-time.
	 */
	param_type					int_width;
protected:
	explicit
	int_instance_collection(const size_t d) :
		parent_type(d), int_width(0) { }
public:
	int_instance_collection(const scopespace& o, const string& n, 
		const size_t d);

virtual	~int_instance_collection();

	size_t
	width(void) const { return int_width; }

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

	never_ptr<const const_param_expr_list>
	get_actual_param_list(void) const;

virtual instance_ptr_type
	lookup_instance(const multikey_index_type& i) const = 0;

virtual	bool
	lookup_instance_collection(list<instance_ptr_type>& l, 
		const const_range_list& r) const = 0;

virtual	const_index_list
	resolve_indices(const const_index_list& l) const = 0;

#if 0
virtual int
	connect(const multikey_index_type& k, const int_instance_alias& b) = 0;
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
	void
	write_object_base(const persistent_object_manager& m, ostream& ) const;

	void
	load_object_base(const persistent_object_manager& m, istream& );

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
	typedef	int_instance_alias			element_type;
	typedef	multikey_map<D, pint_value_type, element_type, qmap>
							collection_type;
	typedef	typename collection_type::key_type	key_type;
private:
	collection_type					collection;
private:
	int_array();
public:
	int_array(const scopespace& o, const string& n);
	~int_array();

	ostream&
	what(ostream& o) const;

	bool
	is_partially_unrolled(void) const;

	ostream&
	dump_unrolled_instances(ostream& o) const;

	void
	instantiate_indices(const index_collection_item_ptr_type& i);

	const_index_list
	resolve_indices(const const_index_list& l) const;

	instance_ptr_type
	lookup_instance(const multikey_index_type& l) const;

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
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC_NO_POINTERS
};	// end class int_array

//-----------------------------------------------------------------------------
template <>
class int_array<0> : public int_instance_collection {
private:
	typedef	int_instance_collection		parent_type;
friend class int_instance_collection;
public:
	typedef	parent_type::instance_ptr_type	instance_ptr_type;
private:
	int_instance_alias			the_instance;

private:
	int_array();

public:
	int_array(const scopespace& o, const string& n);
	~int_array();

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
};	// end class int_array (specialized)

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_INT_H__

