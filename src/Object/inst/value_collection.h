/**
	\file "Object/inst/value_collection.h"
	Parameter instance collection classes for HAC.  
	This file was "Object/art_object_value_collection.h"
		in a previous life.  
	$Id: value_collection.h,v 1.9.2.1.2.3 2006/02/18 01:52:35 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_VALUE_COLLECTION_H__
#define	__HAC_OBJECT_INST_VALUE_COLLECTION_H__

#include <iosfwd>
#include "Object/devel_switches.h"
#include "util/string_fwd.h"
#include "util/STL/list_fwd.h"
#include "util/boolean_types.h"
#include "Object/inst/instance_collection_base.h"	// for macros
#include "Object/common/multikey_index.h"
#include "Object/traits/class_traits_fwd.h"
#include "util/memory/count_ptr.h"

#include "util/persistent_fwd.h"
#include "util/new_functor_fwd.h"
#include "util/multikey_fwd.h"
#include "util/multikey_qmap_fwd.h"

namespace HAC {
namespace entity {
template <class>
class simple_meta_value_reference;
// template <class>
// class simple_meta_instance_reference;
class meta_instance_reference_base;
#if DECOUPLE_INSTANCE_REFERENCE_HIERARCHY
class meta_value_reference_base;
#endif
class nonmeta_instance_reference_base;
class fundamental_type_reference;
class param_type_reference;
class param_expr;
class const_param;
class const_range_list;
class const_index_list;
class scopespace;
class unroll_context;
template <class> class param_instantiation_statement;
using std::list;
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
// friend class simple_meta_instance_reference<Tag>;
friend class simple_meta_value_reference<Tag>;
public:
	typedef	class_traits<Tag>		traits_type;
private:
	typedef	VALUE_COLLECTION_CLASS		this_type;
	typedef	typename traits_type::value_collection_parent_type
						parent_type;
public:
	typedef	typename traits_type::value_type	value_type;
	typedef	typename traits_type::simple_meta_value_reference_type
					simple_meta_value_reference_type;
	typedef	typename traits_type::simple_nonmeta_instance_reference_type
					simple_nonmeta_instance_reference_type;
	typedef	typename traits_type::expr_base_type
						expr_type;
	typedef	typename traits_type::const_expr_type
						const_expr_type;
	typedef	typename traits_type::const_collection_type
						const_collection_type;
	typedef	count_ptr<const expr_type>	init_arg_type;
protected:
#if 1 || ENABLE_STATIC_COMPILE_CHECKS
	/**
		TODO: 20060214: eliminate static initial value analysis?

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
#endif

#if !ENABLE_STATIC_COMPILE_CHECKS
//	typedef param_instantiation_statement<Tag>
	typedef typename traits_type::instantiation_statement_type
					initial_instantiation_statement_type;
	typedef	never_ptr<const initial_instantiation_statement_type>
				initial_instantiation_statement_ptr_type;
	initial_instantiation_statement_ptr_type
					initial_instantiation_statement_ptr;
#endif

protected:
	explicit
	value_collection(const size_t d);

	value_collection(const this_type& t, const footprint& f);

private:
virtual	MAKE_INSTANCE_COLLECTION_FOOTPRINT_COPY_PROTO = 0;

public:
	value_collection(const scopespace& o, const string& n, 
		const size_t d);

virtual	~value_collection();

virtual	ostream&
	what(ostream& o) const = 0;

	ostream&
	type_dump(ostream& o) const;

#if !ENABLE_STATIC_COMPILE_CHECKS
	void
	attach_initial_instantiation_statement(
		const initial_instantiation_statement_ptr_type i) {
		NEVER_NULL(i);
		if (!initial_instantiation_statement_ptr) {
			initial_instantiation_statement_ptr = i;
		}
	}

	index_collection_item_ptr_type
	get_initial_instantiation_indices(void) const;
#endif

virtual	bool
	is_partially_unrolled(void) const = 0;

virtual	ostream&
	dump_unrolled_values(ostream& o) const = 0;

	ostream&
	dump_formal(ostream&, const unroll_context&) const;

	// PROBLEM: built-in? needs to be consistent
	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;

	count_ptr<const param_type_reference>
	get_param_type_ref(void) const;

#if DECOUPLE_INSTANCE_REFERENCE_HIERARCHY
	count_ptr<meta_value_reference_base>
	make_meta_value_reference(void) const;
#else
	count_ptr<meta_instance_reference_base>
	make_meta_instance_reference(void) const;

	count_ptr<nonmeta_instance_reference_base>
	make_nonmeta_instance_reference(void) const;
#endif

	good_bool
	initialize(const init_arg_type& e);

	good_bool
	assign_default_value(const count_ptr<const param_expr>& p);

	count_ptr<const param_expr>
	default_value(void) const;

	count_ptr<const expr_type>
	initial_value(void) const;

	good_bool
	may_type_check_actual_param_expr(const param_expr&) const;

	good_bool
	must_type_check_actual_param_expr(const const_param&,
		const unroll_context&) const;

virtual	good_bool
	instantiate_indices(const const_range_list& i) = 0;

#define	LOOKUP_VALUE_INDEXED_PROTO					\
	good_bool							\
	lookup_value(value_type& v, const multikey_index_type& i, 	\
		const unroll_context&) const

virtual	LOOKUP_VALUE_INDEXED_PROTO = 0;
	// need methods for looking up dense sub-collections of values?
	// what should they return?

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
	typedef	class_traits<Tag>			traits_type;
	typedef	typename traits_type::value_type	value_type;
	typedef	typename traits_type::instance_type
							element_type;

	// later change this to multikey_set or not?
	/// Type for actual values, including validity and status.
	typedef	multikey_map<D, pint_value_type, element_type, qmap>
							collection_type;
	typedef	typename collection_type::key_type	key_type;
	typedef	typename traits_type::const_collection_type
							const_collection_type;
private:
	/// the collection of boolean instances
	collection_type					collection;
	// value cache is not persistent
	const_collection_type				cached_values;
	// tracking validity and density of the value cache?

	value_array();

	value_array(const this_type&, const footprint&);

	MAKE_INSTANCE_COLLECTION_FOOTPRINT_COPY_PROTO;

public:
	value_array(const scopespace& o, const string& n);
	~value_array();

	ostream&
	what(ostream& ) const;

	bool
	is_partially_unrolled(void) const;

	bool
	is_loop_variable(void) const;

	ostream&
	dump_unrolled_values(ostream& o) const;

	// update this to accept const_range_list instead
	good_bool
	instantiate_indices(const const_range_list&);

	const_index_list
	resolve_indices(const const_index_list& l) const;

	LOOKUP_VALUE_INDEXED_PROTO;

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
	typedef	class_traits<Tag>			traits_type;
	typedef	typename traits_type::instance_type
							instance_type;
	typedef	instance_type				element_type;
	typedef	typename traits_type::value_type	value_type;
	typedef	typename traits_type::expr_base_type	expr_type;
	typedef	typename traits_type::const_expr_type	const_expr_type;
private:
	instance_type					the_instance;
	const_expr_type					cached_value;
	bool						cache_validity;
public:
	value_array();

	value_array(const scopespace& o, const string& n);

private:
	value_array(const this_type&, const footprint&);

	MAKE_INSTANCE_COLLECTION_FOOTPRINT_COPY_PROTO;

public:
	~value_array();

	ostream&
	what(ostream& ) const;

	bool
	is_partially_unrolled(void) const;

	bool
	is_loop_variable(void) const;

	ostream&
	dump_unrolled_values(ostream& o) const;

	good_bool
	lookup_value(value_type& i, const unroll_context&) const;

	bad_bool
	assign(const value_type i);

// there are implemented to do nothing but sanity check, 
// since it doesn't even make sense to call these.  
	// update this to accept a const_range_list
	good_bool
	instantiate_indices(const const_range_list&);

	LOOKUP_VALUE_INDEXED_PROTO;
	// need methods for looking up dense sub-collections of values?
	// what should they return?

	bad_bool
	assign(const multikey_index_type& k, const value_type i);

	this_type&
	operator = (const value_type);

	const_index_list
	resolve_indices(const const_index_list& l) const;

public:
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC_NO_POINTERS
	// POOL?

};	// end class value_array specialization

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_VALUE_COLLECTION_H__

