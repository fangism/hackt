/**
	\file "art_object_instance_param.h"
	Parameter instance collection classes for ART.  
	$Id: art_object_instance_param.h,v 1.14 2005/03/01 04:50:58 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_PARAM_H__
#define	__ART_OBJECT_INSTANCE_PARAM_H__

#include "boolean_types.h"
#include "art_object_instance_base.h"
#include "art_object_expr_param_ref.h"	// for typedef init_arg_type
#include "memory/count_ptr.h"

#include "multikey_fwd.h"
#include "multikey_qmap_fwd.h"

namespace ART {
namespace entity {

USING_LIST
using namespace util::memory;	// for experimental pointer classes
using util::qmap;
using util::multikey_map;
using util::bad_bool;
using util::good_bool;

//=============================================================================
// class instance_collection_base declared in "art_object_instance_base.h"

//=============================================================================
/**
	Instance of a built-in parameter type, such as pint and pbool.  
	Virtualizing.  
	Going to sub-type into pint and pbool.  
	TO DO: derive from a interface for template_argument.  
 */
class param_instance_collection : public instance_collection_base {
private:
	typedef	instance_collection_base	parent_type;
public:
	typedef	parent_type::inst_ref_ptr_type	inst_ref_ptr_type;
	typedef	parent_type::member_inst_ref_ptr_type
						member_inst_ref_ptr_type;

protected:
	param_instance_collection(const size_t d);
public:
	param_instance_collection(const scopespace& o, const string& n, 
		const size_t d);

virtual	~param_instance_collection();

virtual	ostream&
	what(ostream& o) const = 0;

	ostream&
	dump(ostream& o) const;

virtual	bool
	is_partially_unrolled(void) const = 0;

virtual	ostream&
	dump_unrolled_values(ostream& o) const = 0;

virtual	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const = 0;

virtual	count_ptr<instance_reference_base>
	make_instance_reference(void) const = 0;

	/** should just assert fail, forbid reference to param members */
	member_inst_ref_ptr_type
	make_member_instance_reference(const inst_ref_ptr_type& b) const;

	/** appropriate for the context of a template parameter formal */
virtual	count_ptr<const param_expr>
	default_value(void) const = 0;

/**
	A parameter is considered "usable" if it is either initialized
	to another expression, or it is a template formal.  

	Think we may need two flavors of initialization query, 
	because of conservativeness and imprecision:
	"may_be_initialized" and "definitely_initialized"

	TECHNICALITY: what about conditional assignments to a variable?
	Will need assignment expression stack to resolve statically...
	\return true if initialized to an expression.  
	\sa initialize
 */
	bool
	may_be_initialized(void) const;

	bool
	must_be_initialized(void) const;

virtual	good_bool
	assign_default_value(const count_ptr<const param_expr>& p) = 0;

// used by definition_base::certify_template_arguments
virtual	good_bool
	type_check_actual_param_expr(const param_expr& pe) const = 0;

/**
	whether or not this can be resolved to some static constant value.
	Will also need two flavors.  
 */
	bool
	is_static_constant(void) const;

#if 0
NOTE: these functions should only be applicable to param_instance_references.  
/**
	Whether or not this parameter is itself a loop index,
	or if indexed, its indices depend on some loop index.
	This may be applicable to instance_collection_base in general.  
 */
	bool is_loop_independent(void) const;

	bool is_unconditional(void) const;
#endif
protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class param_instance_collection

//-----------------------------------------------------------------------------
/**
	A run-time instance of a boolean parameter. 
 */
struct pbool_instance {
public:
	typedef	pbool_value_type		value_type;
public:
	/**
		The unroll-time value of this pbool parameter.
	 */
	value_type	value : 1;
	/**
		Whether or not this instance was truly instantiated,
		Safeguards against extraneous instances in arrays.  
	 */
	bool		instantiated : 1;
	/**	Whether or not value has been initialized exactly 
		once to a value
	 */
	bool		valid : 1;
public:
	pbool_instance() : value(false), instantiated(false), valid(false) { }

	explicit
	pbool_instance(const value_type b) :
		value(b), instantiated(true), valid(false) { }
	// default copy constructor
	// default destructor

	/**
		\return false on error, true on success.  
	 */
	good_bool
	operator = (const value_type b) {
		assert(instantiated);
		if (valid)
			// error: already initialized
			// or allow multiple assignments with the same value?
			return good_bool(false);
		else {
			value = b;
			valid = true;
			return good_bool(true);
		}
	}

};	// end struct pbool_instance

bool
operator == (const pbool_instance& p, const pbool_instance& q);

ostream&
operator << (ostream& o, const pbool_instance& p);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Hard-wired to pbool_type, defined in "art_built_ins.h".  
 */
class pbool_instance_collection : public param_instance_collection {
public:
	typedef	pbool_value_type		value_type;
	typedef	pbool_instance_reference	reference_type;
	typedef	reference_type::init_arg_type	init_arg_type;
private:
	typedef	pbool_instance_collection	this_type;
	typedef	param_instance_collection	parent_type;
// friend class pbool_instantiation_statement;
friend class pbool_instance_reference;
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
	count_ptr<const pbool_expr>		ival;

protected:
	explicit
	pbool_instance_collection(const size_t d);
public:
	pbool_instance_collection(const scopespace& o, const string& n, 
		const size_t d);

virtual	~pbool_instance_collection();

	ostream&
	what(ostream& o) const;

	ostream&
	type_dump(ostream& o) const;

virtual	bool
	is_partially_unrolled(void) const = 0;

virtual	ostream&
	dump_unrolled_values(ostream& o) const = 0;

	// PROBLEM: built-in? needs to be consistent
	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;

	count_ptr<instance_reference_base>
	make_instance_reference(void) const;

	good_bool
	initialize(const init_arg_type& e);

	good_bool
	assign_default_value(const count_ptr<const param_expr>& p);

	count_ptr<const param_expr>
	default_value(void) const;

	count_ptr<const pbool_expr>
	initial_value(void) const;

	good_bool
	type_check_actual_param_expr(const param_expr& pe) const;

virtual	void
	instantiate_indices(const index_collection_item_ptr_type& i) = 0;

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
// really should be protected, usable by pbool_instance_reference::assigner
virtual	bad_bool
	assign(const multikey_index_type& k, const value_type b) = 0;

public:

	static
	pbool_instance_collection*
	make_pbool_array(const scopespace& o, const string& n, const size_t d);

	// only intended for children class
	// need not be virtual, no pointers in subclasses
	static
	persistent*
	construct_empty(const int);

protected:
	void
	collect_transient_info(persistent_object_manager& m) const;

	void
	write_object_base(const persistent_object_manager& m, ostream& o) const;

	void
	load_object_base(const persistent_object_manager& m, istream& i);

	// subclasses are responsible for implementing:
	// write_object and load_object.
};	// end class pbool_instance_collection

//-----------------------------------------------------------------------------
#define	PBOOL_ARRAY_TEMPLATE_SIGNATURE		template <size_t D>

/**
	Dimension-specific array of boolean parameters.
 */
PBOOL_ARRAY_TEMPLATE_SIGNATURE
class pbool_array : public pbool_instance_collection {
private:
	typedef pbool_array<D>				this_type;
	typedef	pbool_instance_collection		parent_type;
friend class pbool_instance_collection;
public:
	typedef	pbool_instance::value_type		value_type;
	typedef	pbool_instance				element_type;
	/// Type for actual values, including validity and status.
	typedef	multikey_map<D, pint_value_type, element_type, qmap>
							collection_type;
	typedef	typename collection_type::key_type	key_type;
protected:
	/// the collection of boolean instances
	collection_type					collection;
	pbool_array();

public:
	pbool_array(const scopespace& o, const string& n);
	~pbool_array();

	bool
	is_partially_unrolled(void) const;

	ostream&
	dump_unrolled_values(ostream& o) const;

	void
	instantiate_indices(const index_collection_item_ptr_type& i);

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
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC_NO_POINTERS

};	// end class pbool_array

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Specialization of scalar boolean parameter.
 */
template <>
class pbool_array<0> : public pbool_instance_collection {
private:
	typedef	pbool_instance_collection		parent_type;
	typedef	pbool_array<0>				this_type;
public:
	typedef	pbool_instance				instance_type;
	typedef	pbool_instance				element_type;
	typedef	pbool_instance::value_type		value_type;
protected:
	instance_type					the_instance;
public:
	pbool_array();

	pbool_array(const scopespace& o, const string& n);

	~pbool_array() { }

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
	void
	instantiate_indices(const index_collection_item_ptr_type& i);

	good_bool
	lookup_value(value_type& v,
		const multikey_index_type& i) const;
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

};	// end class pbool_array specialization

typedef	pbool_array<0>			pbool_scalar;

//=============================================================================
/**
	Run-time instance of integer parameter.  
 */
struct pint_instance {
public:
	typedef pint_value_type		value_type;
public:
	/**
		The unroll-time value of this pint parameter.
	 */
	value_type	value;
	/**
		Whether or not this instance was truly instantiated,
		Safeguards against extraneous instances in arrays.  
	 */
	bool		instantiated : 1;
	/**
		Whether or not value has been initialized exactly 
		once to a value.
	 */
	bool		valid : 1;
public:
	pint_instance() : value(-1), instantiated(false), valid(false) { }

	explicit
	pint_instance(const bool b) :
		value(-1), instantiated(b), valid(false) { }

	explicit
	pint_instance(const value_type v) :
		value(v), instantiated(true), valid(true) { }
	// default copy constructor
	// default destructor

	/**
		\return false on error, true on success.  
	 */
	good_bool
	operator = (const value_type i) {
		assert(instantiated);
		if (valid)
			// error: already initialized
			// or allow multiple assignments with the same value?
			return good_bool(false);
		else {
			value = i;
			valid = true;
			return good_bool(true);
		}
	}

#if 0
	// no longer used
	struct is_valid {
		/** for unary predication */
		bool
		operator () (const pint_instance& pi) const { return pi.valid; }

		/** for validity accumulation */
		bool
		operator () (const bool b, const pint_instance& pi) const {
			return b && pi.valid;
		}
	};	// end struct is_valid
#endif
};	// end struct pint_instance

bool
operator == (const pint_instance& p, const pint_instance& q);

ostream&
operator << (ostream& o, const pint_instance& p);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Collection of parameter integers, 
	generalized to any number of dimensions.  
	Hard-wired to pint_type, defined in "art_built_ins.h".  
 */
class pint_instance_collection : public param_instance_collection {
friend class pint_instance_reference;
public:
	typedef	pint_value_type			value_type;
	typedef	pint_instance_reference		reference_type;
	typedef	reference_type::init_arg_type	init_arg_type;
private:
	typedef	pint_instance_collection	this_type;
	typedef	param_instance_collection	parent_type;
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

		Q: Should we actaully store ival persistently or just 
		reconstruct it during unrolling?
	 */
	count_ptr<const pint_expr>		ival;

protected:
	explicit
	pint_instance_collection(const size_t d);
public:
	pint_instance_collection(const scopespace& o, const string& n, 
		const size_t d);

	// keep these for built-in int datatype initialization
	pint_instance_collection(const scopespace& o, const string& n, 
		const size_t d, const count_ptr<const pint_const>& i);

virtual	~pint_instance_collection();

	ostream&
	what(ostream& o) const;

	ostream&
	type_dump(ostream& o) const;

virtual	bool
	is_partially_unrolled(void) const = 0;

virtual	ostream&
	dump_unrolled_values(ostream& o) const = 0;

	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;

	count_ptr<instance_reference_base>
	make_instance_reference(void) const;

	good_bool
	initialize(const init_arg_type& e);

	good_bool
	assign_default_value(const count_ptr<const param_expr>& p);

	count_ptr<const param_expr>
	default_value(void) const;

	count_ptr<const pint_expr>
	initial_value(void) const;

	good_bool
	type_check_actual_param_expr(const param_expr& pe) const;


virtual	void
	instantiate_indices(const index_collection_item_ptr_type& i) = 0;

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
// really should be protected, usable by pint_instance_reference::assigner
virtual	bad_bool
	assign(const multikey_index_type& k, const value_type i) = 0;

public:
	// subclasses will share this persistent type entry

	static
	pint_instance_collection*
	make_pint_array(const scopespace& o, const string& n, const size_t d);
	// need not be virtual, no pointers in subclasses

	static
	persistent*
	construct_empty(const int);

protected:
	void
	collect_transient_info(persistent_object_manager& m) const;

	void
	write_object_base(const persistent_object_manager& m, ostream& o) const;

	void
	load_object_base(const persistent_object_manager& m, istream& i);

	// subclasses are responsible for implementing:
	// write_object and load_object.
};	// end class pint_instance_collection

//-----------------------------------------------------------------------------
#define	PINT_ARRAY_TEMPLATE_SIGNATURE	template <size_t D>

/**
	Dimension-specific array of integer parameters.  
 */
PINT_ARRAY_TEMPLATE_SIGNATURE
class pint_array : public pint_instance_collection {
private:
	typedef	pint_array<D>				this_type;
	typedef	pint_instance_collection		parent_type;
public:
	typedef	pint_instance::value_type		value_type;
	typedef	pint_instance				element_type;
	/**
		Type for actual values, including validity and status.
	 */
	typedef	multikey_map<D, pint_value_type, element_type, qmap>
							collection_type;
	typedef	typename collection_type::key_type	key_type;
protected:
	/** The collection of value instances */
	collection_type					collection;
public:
	pint_array();

	pint_array(const scopespace& o, const string& n);

	~pint_array();

	bool
	is_partially_unrolled(void) const;

	ostream&
	dump_unrolled_values(ostream& o) const;

	void
	instantiate_indices(const index_collection_item_ptr_type& i);

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
	};	// end struct key_value_dumper

public:
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC_NO_POINTERS
};	// end class pint_array

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Specialization of scalar integer parameter.
 */
template <>
class pint_array<0> : public pint_instance_collection {
private:
	typedef	pint_instance_collection		parent_type;
	typedef	pint_array<0>				this_type;
public:
	typedef	pint_instance				instance_type;
	typedef	pint_instance				element_type;
	typedef	element_type::value_type		value_type;
protected:
	instance_type					the_instance;
public:
	pint_array();

	pint_array(const scopespace& o, const string& n);

	/**
		Special case tailor-made for built-in integer datatype
		definition construction.
	 */
	pint_array(const scopespace& o, const string& n, 
		const count_ptr<const pint_const>& i);

	~pint_array();

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
	void
	instantiate_indices(const index_collection_item_ptr_type& i);

	good_bool
	lookup_value(value_type& v,
		const multikey_index_type& i) const;
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
	LIST_VECTOR_POOL_ESSENTIAL_FRIENDS
	LIST_VECTOR_POOL_ROBUST_STATIC_DECLARATIONS
};	// end class pint_array specialization

typedef	pint_array<0>			pint_scalar;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_PARAM_H__

