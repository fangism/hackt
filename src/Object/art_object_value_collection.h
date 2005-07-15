/**
	\file "Object/art_object_value_collection.h"
	Parameter instance collection classes for ART.  
	$Id: art_object_value_collection.h,v 1.6.4.5 2005/07/15 03:49:18 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_VALUE_COLLECTION_H__
#define	__OBJECT_ART_OBJECT_VALUE_COLLECTION_H__

#include <iosfwd>
#include "util/string_fwd.h"
#include "util/STL/list_fwd.h"
#include "util/boolean_types.h"
#include "Object/art_object_index.h"
#include "Object/traits/class_traits_fwd.h"
#include "util/memory/count_ptr.h"

#include "util/persistent_fwd.h"
#include "util/new_functor_fwd.h"
#include "util/multikey_fwd.h"
#include "util/multikey_qmap_fwd.h"

namespace ART {
namespace entity {
template <class>
class simple_meta_instance_reference;
class meta_instance_reference_base;
class nonmeta_instance_reference_base;
class fundamental_type_reference;
class param_expr;
class const_param;
class const_range_list;
class const_index_list;
USING_LIST
using std::istream;
using std::ostream;
using std::string;
using util::memory::count_ptr;	// for experimental pointer classes
using util::qmap;
using util::multikey_map;
using util::bad_bool;
using util::good_bool;
using util::persistent;
using util::persistent_object_manager;

class scopespace;

//=============================================================================
#define	VALUE_COLLECTION_TEMPLATE_SIGNATURE				\
template <class Tag>

#define VALUE_COLLECTION_CLASS						\
value_collection<Tag>

/**
	Value collection class template.  
 */
VALUE_COLLECTION_TEMPLATE_SIGNATURE
class value_collection :
	public class_traits<Tag>::value_collection_parent_type {
// friend class pbool_instantiation_statement;
friend class simple_meta_instance_reference<Tag>;
private:
	typedef	VALUE_COLLECTION_CLASS		this_type;
	typedef	typename class_traits<Tag>::value_collection_parent_type
						parent_type;
public:
	typedef	typename class_traits<Tag>::value_type	value_type;
	typedef	typename class_traits<Tag>::simple_meta_instance_reference_type
					simple_meta_instance_reference_type;
	typedef	typename class_traits<Tag>::simple_nonmeta_instance_reference_type
					simple_nonmeta_instance_reference_type;
	typedef	typename class_traits<Tag>::expr_base_type
						expr_type;
	typedef	typename class_traits<Tag>::const_expr_type
						const_expr_type;
	typedef	typename class_traits<Tag>::const_collection_type
						const_collection_type;
	typedef	count_ptr<const expr_type>	init_arg_type;
protected:
	/**
		Expression or value with which parameter is initialized. 
		Recall that parameters are static -- written once only.  
		Not to be used by the hash_string.  
		In the formals context of a template signature, 
		ival is to be interpreted as a default value, in the 
		case where one is not supplied.  
		Or should this be never deleted? cache-owned expressions?
		Screw the cache.  
		Only applicable for simple instances.  
		Collectives won't be checked until unroll time.  
	 */
	count_ptr<const expr_type>		ival;

protected:
	explicit
	value_collection(const size_t d);
public:
	value_collection(const scopespace& o, const string& n, 
		const size_t d);

virtual	~value_collection();

virtual	ostream&
	what(ostream& o) const = 0;

	ostream&
	type_dump(ostream& o) const;

virtual	bool
	is_partially_unrolled(void) const = 0;

virtual	ostream&
	dump_unrolled_values(ostream& o) const = 0;

	// PROBLEM: built-in? needs to be consistent
	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;

	count_ptr<meta_instance_reference_base>
	make_meta_instance_reference(void) const;

	count_ptr<nonmeta_instance_reference_base>
	make_nonmeta_instance_reference(void) const;

	good_bool
	initialize(const init_arg_type& e);

	good_bool
	assign_default_value(const count_ptr<const param_expr>& p);

	count_ptr<const param_expr>
	default_value(void) const;

	count_ptr<const expr_type>
	initial_value(void) const;

	good_bool
	may_type_check_actual_param_expr(const param_expr& pe) const;

	good_bool
	must_type_check_actual_param_expr(const const_param& pe) const;

virtual	good_bool
	instantiate_indices(const const_range_list& i) = 0;

// virtual	bool lookup_value(bool& v) const = 0;

virtual	good_bool
	lookup_value(value_type& v,
		const multikey_index_type& i) const = 0;
	// need methods for looking up dense sub-collections of values?
	// what should they return?
virtual	good_bool
	lookup_value_collection(list<value_type>& l, 
		const const_range_list& r) const = 0;

virtual	const_index_list
	resolve_indices(const const_index_list& l) const = 0;

public:
// really should be protected, usable by pbool_meta_instance_reference::assigner
virtual	bad_bool
	assign(const multikey_index_type& k, const value_type b) = 0;

public:

	static
	this_type*
	make_array(const scopespace& o, const string& n, const size_t d);

public:
	void
	collect_transient_info(persistent_object_manager& m) const;

protected:
	void
	write_object_base(const persistent_object_manager& m, ostream& o) const;

	void
	load_object_base(const persistent_object_manager& m, istream& i);

	// subclasses are responsible for implementing:
	// write_object and load_object.
};	// end class value_collection

//-----------------------------------------------------------------------------
#define VALUE_ARRAY_TEMPLATE_SIGNATURE					\
template <class Tag, size_t D>

#define	VALUE_ARRAY_CLASS						\
value_array<Tag,D>

/**
	Dimension-specific array of parameters.
 */
VALUE_ARRAY_TEMPLATE_SIGNATURE
class value_array : public value_collection<Tag> {
private:
	typedef VALUE_ARRAY_CLASS			this_type;
	typedef	value_collection<Tag>			parent_type;
friend class value_collection<Tag>;
public:
	typedef	typename class_traits<Tag>::value_type	value_type;
	typedef	typename class_traits<Tag>::instance_type
							element_type;

	// later change this to multikey_set or not?
	/// Type for actual values, including validity and status.
	typedef	multikey_map<D, pint_value_type, element_type, qmap>
							collection_type;
	typedef	typename collection_type::key_type	key_type;
	typedef	typename class_traits<Tag>::const_collection_type
							const_collection_type;
private:
	/// the collection of boolean instances
	collection_type					collection;
	// value cache is not persistent
	const_collection_type				cached_values;
	// tracking validity and density of the value cache?

	value_array();

public:
	value_array(const scopespace& o, const string& n);
	~value_array();

	ostream&
	what(ostream& ) const;

	bool
	is_partially_unrolled(void) const;

	ostream&
	dump_unrolled_values(ostream& o) const;

	// update this to accept const_range_list instead
	good_bool
	instantiate_indices(const const_range_list&);

	const_index_list
	resolve_indices(const const_index_list& l) const;

	good_bool
	lookup_value(value_type& v,
		const multikey_index_type& i) const;

	good_bool
	lookup_value_collection(list<value_type>& l,
		const const_range_list& r) const;

	bad_bool
	assign(const multikey_index_type& k, const value_type i);

	/// helper functor for dumping values
	struct key_value_dumper {
		ostream& os;
		key_value_dumper(ostream& o) : os(o) { }

		ostream&
		operator () (const typename collection_type::value_type&);
	};      // end struct key_value_dumper

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC_NO_POINTERS

};	// end class value_array

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	VALUE_SCALAR_TEMPLATE_SIGNATURE					\
template <class Tag>

#define	VALUE_SCALAR_CLASS						\
value_array<Tag,0>

/**
	Specialization of scalar parameter.
 */
VALUE_SCALAR_TEMPLATE_SIGNATURE
class VALUE_SCALAR_CLASS : public value_collection<Tag> {
private:
	typedef	value_collection<Tag>			parent_type;
	typedef	VALUE_SCALAR_CLASS			this_type;
public:
	typedef	typename class_traits<Tag>::instance_type
							instance_type;
	typedef	instance_type				element_type;
	typedef	typename class_traits<Tag>::value_type	value_type;
	typedef	typename class_traits<Tag>::const_expr_type
							const_expr_type;
private:
	instance_type					the_instance;
	const_expr_type					cached_value;
	bool						cache_validity;
public:
	value_array();

	value_array(const scopespace& o, const string& n);

	~value_array();

	ostream&
	what(ostream& ) const;

	bool
	is_partially_unrolled(void) const;

	ostream&
	dump_unrolled_values(ostream& o) const;

	good_bool
	lookup_value(value_type& i) const;

	bad_bool
	assign(const value_type i);

// there are implemented to do nothing but sanity check, 
// since it doesn't even make sense to call these.  
	// update this to accept a const_range_list
	good_bool
	instantiate_indices(const const_range_list&);

	good_bool
	lookup_value(value_type& v, const multikey_index_type& i) const;

	// need methods for looking up dense sub-collections of values?
	// what should they return?
	good_bool
	lookup_value_collection(list<value_type>& l, 
		const const_range_list& r) const;

	bad_bool
	assign(const multikey_index_type& k, const value_type i);

	const_index_list
	resolve_indices(const const_index_list& l) const;

public:
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC_NO_POINTERS
	// POOL?

};	// end class pbool_array specialization

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_VALUE_COLLECTION_H__

