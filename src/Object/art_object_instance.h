/**
	\file "art_object_instance.h"
	Instance collection and statement classes for ART.  
	$Id: art_object_instance.h,v 1.18 2004/11/30 01:25:10 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_H__
#define	__ART_OBJECT_INSTANCE_H__

#include "art_object_base.h"
#include "memory/pointer_classes.h"

#include "multikey_fwd.h"
#include "multikey_qmap_fwd.h"

namespace ART {
//=============================================================================
// forward declarations from outside namespaces
namespace parser {
	// note: methods may specify string as formal types, 
	// but you can still pass token_identifiers and token_strings
	// because they are derived from string.
	class token_string;
	class token_identifier;
	class qualified_id_slice;
	class qualified_id;
	class context;
}
using namespace parser;

//=============================================================================
/**
	The namespace of objects that will be returned by the type-checker, 
	and includes the various hierarchical symbol tables in their 
	respective scopes.  
 */
namespace entity {
//=============================================================================
	using namespace std;
	using namespace util::memory;	// for experimental pointer classes
	using namespace MULTIKEY_NAMESPACE;
	using namespace MULTIKEY_MAP_NAMESPACE;

//=============================================================================
// class instance_collection_base declared in "art_object_base.h"

//=============================================================================
/**
	Process instantiation.  
	Type information is now in the instance_collection_list.
 */
class process_instance_collection : public instance_collection_base {
protected:
	// reserve these for connections between instance_references
	// list of template actuals
	// list of port actuals

private:
	process_instance_collection();
public:
	process_instance_collection(const scopespace& o, 
		const string& n, 
		const size_t d);
	~process_instance_collection();

	ostream& what(ostream& o) const;
//	ostream& dump(ostream& o) const;
	count_ptr<const fundamental_type_reference> get_type_ref(void) const;
	// why is this a never_ptr?
	count_ptr<instance_reference_base>
		make_instance_reference(void) const;
	count_ptr<member_instance_reference_base>
		make_member_instance_reference(
			count_ptr<const simple_instance_reference> b) const;

public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS

};	// end class process_instance_collection

//=============================================================================
/// Instantiation of a data type, either inside or outside definition.  
class datatype_instance_collection : public instance_collection_base {
protected:
private:
	datatype_instance_collection();
public:
	datatype_instance_collection(const scopespace& o, 
		const string& n, const size_t d);
	~datatype_instance_collection();

	ostream& what(ostream& o) const;
	count_ptr<const fundamental_type_reference> get_type_ref(void) const;

// need to do this for real... using object not parse tree
//	bool equals_template_formal(const template_formal_decl& tf) const;
	// why is this never?
	count_ptr<instance_reference_base>
		make_instance_reference(void) const;
	count_ptr<member_instance_reference_base>
		make_member_instance_reference(
			count_ptr<const simple_instance_reference> b) const;
public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class datatype_instance_collection

//=============================================================================
/**
	Instantiation of a channel type.  
	Final class?
 */
class channel_instance_collection : public instance_collection_base {
protected:
private:
	channel_instance_collection();
public:
	channel_instance_collection(const scopespace& o, 
		const string& n, const size_t d);
	~channel_instance_collection();

	ostream& what(ostream& o) const;
	count_ptr<const fundamental_type_reference> get_type_ref(void) const;
	count_ptr<instance_reference_base>
		make_instance_reference(void) const;
	count_ptr<member_instance_reference_base>
		make_member_instance_reference(
			count_ptr<const simple_instance_reference> b) const;
public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class channel_instance_collection

//=============================================================================
/**
	Instance of a built-in parameter type, such as pint and pbool.  
	Virtualizing.  
	Going to sub-type into pint and pbool.  
	TO DO: derive from a interface for template_argument.  
 */
class param_instance_collection : public instance_collection_base {
protected:
	/**
		OBSOLETE after sub-typing, and hard-wiring.  
		Type refers directly to a definition in this case, 
		because parameters are never templatable.  
	never_ptr<const param_type_reference>	type;
	**/

protected:
	param_instance_collection();
public:
	param_instance_collection(const scopespace& o, const string& n, 
		const size_t d);
virtual	~param_instance_collection();

virtual	ostream& what(ostream& o) const = 0;
	ostream& dump(ostream& o) const;

virtual	count_ptr<const fundamental_type_reference>
		get_type_ref(void) const = 0;
	// why is this never?
virtual	count_ptr<instance_reference_base>
		make_instance_reference(void) const = 0;
	/** should just assert fail, forbid reference to param members */
	count_ptr<member_instance_reference_base>
		make_member_instance_reference(
			count_ptr<const simple_instance_reference> b) const;

	/** appropriate for the context of a template parameter formal */
virtual	count_ptr<const param_expr> default_value(void) const = 0;

	bool is_template_formal(void) const;

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
	bool may_be_initialized(void) const;
	bool must_be_initialized(void) const;

virtual	bool assign_default_value(count_ptr<const param_expr> p) = 0;

// used by definition_base::certify_template_arguments
virtual	bool type_check_actual_param_expr(const param_expr& pe) const = 0;

/**
	whether or not this can be resolved to some static constant value.
	Will also need two flavors.  
 */
	bool is_static_constant(void) const;

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
};	// end class param_instance_collection

//-----------------------------------------------------------------------------
/**
	A run-time instance of a boolean parameter. 
 */
struct pbool_instance {
public:
	typedef	bool		value_type;
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
explicit pbool_instance(const value_type b) :
		value(b), instantiated(true), valid(false) { }
	// default copy constructor
	// default destructor

	/**
		\return false on error, true on success.  
	 */
	bool operator = (const value_type b) {
		assert(instantiated);
		if (valid)
			// error: already initialized
			// or allow multiple assignments with the same value?
			return false;
		else {
			value = b;
			valid = true;
			return true;
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
// friend class pbool_instantiation_statement;
friend class pbool_instance_reference;
public:
	// int or size_t (unsigned)?
	typedef	multikey_qmap_base<int, pbool_instance>		collection_type;
	typedef	multikey_qmap<0, int, pbool_instance>		scalar_type;
        typedef multikey_qmap_base<int, bool>			value_type;
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
	/**
		The unrolled collection of pbool instances.  
	 */
	excl_ptr<collection_type>		collection;

private:
	pbool_instance_collection();
public:
	pbool_instance_collection(const scopespace& o, const string& n);
	pbool_instance_collection(const scopespace& o, const string& n, 
		const size_t d);

	ostream& what(ostream& o) const;

	// PROBLEM: built-in? needs to be consistent
	count_ptr<const fundamental_type_reference>
		get_type_ref(void) const;
	// why never?
	count_ptr<instance_reference_base>
		make_instance_reference(void) const;

	bool initialize(count_ptr<const pbool_expr> e);
	bool assign_default_value(count_ptr<const param_expr> p);
	count_ptr<const param_expr> default_value(void) const;
	count_ptr<const pbool_expr> initial_value(void) const;

	bool type_check_actual_param_expr(const param_expr& pe) const;

	void instantiate_indices(const index_collection_item_ptr_type& i);

	bool lookup_value(bool& v) const;
	bool lookup_value(bool& v, const multikey_base<int>& i) const;
	// need methods for looking up dense sub-collections of values?
	// what should they return?
	bool lookup_value_collection(list<bool>& l, 
		const const_range_list& r) const;

	const_index_list resolve_indices(const const_index_list& l) const;

public:
// really should be protected, usable by pbool_instance_reference::assigner
	bool assign(const bool b);
	bool assign(const multikey_base<int>& k, const bool b);

public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class pbool_instance_collection

//-----------------------------------------------------------------------------
/**
	Run-time instance of integer parameter.  
 */
struct pint_instance {
public:
	typedef int	value_type;
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
explicit pint_instance(const bool b) :
		value(-1), instantiated(b), valid(false) { }
explicit pint_instance(const value_type v) :
		value(v), instantiated(true), valid(true) { }
	// default copy constructor
	// default destructor

	/**
		\return false on error, true on success.  
	 */
	bool operator = (const value_type i) {
		assert(instantiated);
		if (valid)
			// error: already initialized
			// or allow multiple assignments with the same value?
			return false;
		else {
			value = i;
			valid = true;
			return true;
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
// temporary switch
#define SUBCLASS_PINT_ARRAY	1
/**
	Collection of parameter integers, 
	generalized to any number of dimensions.  
	Hard-wired to pint_type, defined in "art_built_ins.h".  
 */
class pint_instance_collection : public param_instance_collection {
// friend class pint_instantiation_statement;
friend class pint_instance_reference;
// friend class pint_instance_reference::assigner;
public:
#if !SUBCLASS_PINT_ARRAY
	// int or size_t (unsigned)?
	typedef	multikey_qmap_base<int, pint_instance>		collection_type;
	typedef	multikey_qmap<0, int, pint_instance>		scalar_type;
	typedef	multikey_qmap_base<int, int>			value_type;
#endif	// !SUBCLASS_PINT_ARRAY
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
#if !SUBCLASS_PINT_ARRAY
	/**
		The unrolled collection of pint instances.  
	 */
	excl_ptr<collection_type>		collection;
#endif

protected:
	pint_instance_collection();
public:
	pint_instance_collection(const scopespace& o, const string& n);
	pint_instance_collection(const scopespace& o, const string& n, 
		const size_t d);
	// keep these for built-in int datatype initialization
	pint_instance_collection(const scopespace& o, const string& n, 
		count_ptr<const pint_expr> i);
	pint_instance_collection(const scopespace& o, const string& n, 
		const size_t d, count_ptr<const pint_expr> i);
#if SUBCLASS_PINT_ARRAY
virtual	~pint_instance_collection();
virtual	size_t dimensions(void) const = 0;
#endif

	ostream& what(ostream& o) const;

	count_ptr<const fundamental_type_reference>
		get_type_ref(void) const;
	// why never?
	count_ptr<instance_reference_base>
		make_instance_reference(void) const;

	bool initialize(count_ptr<const pint_expr> e);
	bool assign_default_value(count_ptr<const param_expr> p);
	count_ptr<const param_expr> default_value(void) const;
	count_ptr<const pint_expr> initial_value(void) const;

	bool type_check_actual_param_expr(const param_expr& pe) const;


#if SUBCLASS_PINT_ARRAY
virtual	void instantiate_indices(const index_collection_item_ptr_type& i) = 0;
virtual	bool lookup_value(int& v, const multikey_base<int>& i) const = 0;
	// need methods for looking up dense sub-collections of values?
	// what should they return?
virtual	bool lookup_value_collection(list<int>& l, 
		const const_range_list& r) const = 0;

virtual	const_index_list resolve_indices(const const_index_list& l) const = 0;
#else
	void instantiate_indices(const index_collection_item_ptr_type& i);
	bool lookup_value(int& v) const;
	bool lookup_value(int& v, const multikey_base<int>& i) const;
	// need methods for looking up dense sub-collections of values?
	// what should they return?
	bool lookup_value_collection(list<int>& l, 
		const const_range_list& r) const;

	const_index_list resolve_indices(const const_index_list& l) const;
#endif

public:
// really should be protected, usable by pint_instance_reference::assigner
#if SUBCLASS_PINT_ARRAY
	// why have these at all?
virtual	bool assign(const multikey_base<int>& k, const int i) = 0;
#else
	bool assign(const int i);
	bool assign(const multikey_base<int>& k, const int i);
#endif
public:
	// subclasses will share this persistent type entry
	PERSISTENT_STATIC_MEMBERS_DECL
#if !SUBCLASS_PINT_ARRAY
	PERSISTENT_METHODS
#else
	static pint_instance_collection* make_pint_array(
		const scopespace& o, const string& n, const size_t d);
	// need not be virtual, no pointers in subclasses
	static persistent* construct_empty(const int);
	void collect_transient_info(persistent_object_manager& m) const;
	void write_object_base(const persistent_object_manager& m) const;
	void load_object_base(persistent_object_manager& m);

	// subclasses are responsible for implementing:
	// write_object and load_object.
#endif
};	// end class pint_instance_collection

//-----------------------------------------------------------------------------
#if SUBCLASS_PINT_ARRAY
#define	PINT_ARRAY_TEMPLATE_SIGNATURE	template <size_t D>

/**
	Dimension-specific array of integer parameters.  
 */
PINT_ARRAY_TEMPLATE_SIGNATURE
class pint_array : public pint_instance_collection {
public:
	/**
		Type for actual values, including validity and status.
	 */
	typedef	multikey_qmap<D, int, pint_instance>	collection_type;
	/**
		Collection of valid values passed around.
	 */
	typedef	multikey_qmap<D, int, int>		value_type;
protected:
	/** The collection of value instances */
	collection_type					collection;
public:
	pint_array();
	pint_array(const scopespace& o, const string& n);
	~pint_array();

	size_t dimensions(void) const { return D; }
	void instantiate_indices(const index_collection_item_ptr_type& i);
	const_index_list resolve_indices(const const_index_list& l) const;
	bool lookup_value(int& v, const multikey_base<int>& i) const;
	bool lookup_value_collection(list<int>& l, 
		const const_range_list& r) const;
	bool assign(const multikey_base<int>& k, const int i);

public:
//	PERSISTENT_METHODS
	void write_object(const persistent_object_manager& m) const;
	void load_object(persistent_object_manager& m);
};	// end class pint_array

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Specialization of scalar integer parameter.
 */
template <>
class pint_array<0> : public pint_instance_collection {
public:
	typedef	pint_instance				instance_type;
	typedef	int					value_type;
protected:
	instance_type					the_instance;
public:
	pint_array();
	pint_array(const scopespace& o, const string& n);
	pint_array(const scopespace& o, const string& n, 
		count_ptr<const pint_expr> i);
	~pint_array() { }

	size_t dimensions(void) const { return 0; }
	bool lookup_value(int& i) const;
	bool assign(const int i);

// there are implemented to do nothing but sanity check, 
// since it doesn't even make sense to call these.  
	void instantiate_indices(const index_collection_item_ptr_type& i);
	bool lookup_value(int& v, const multikey_base<int>& i) const;
	// need methods for looking up dense sub-collections of values?
	// what should they return?
	bool lookup_value_collection(list<int>& l, 
		const const_range_list& r) const;
	bool assign(const multikey_base<int>& k, const int i);

	const_index_list resolve_indices(const const_index_list& l) const;

public:
//	PERSISTENT_METHODS
	void write_object(const persistent_object_manager& m) const;
	void load_object(persistent_object_manager& m);
};	// end class pint_array specialization

#endif	// SUBCLASS_PINT_ARRAY

//=============================================================================
//=============================================================================

/**
	This node retains the information for an instantiation statement.  
	This is what will be unrolled.  
	No parent, is a globally sequential item.  
	Every sub-class will contain a modifiable
	back-reference to an (sub-type of) instance_collection_base, 
	where the collection will be unrolled.  
	Should this point to an unrolled instance?
	No, it will be looked up.  
 */
class instantiation_statement : public instance_management_base {
protected:
	index_collection_item_ptr_type		indices;

protected:
	instantiation_statement();
public:
	instantiation_statement(
		const index_collection_item_ptr_type& i);
virtual	~instantiation_statement();

	ostream& dump(ostream& o) const;

virtual	void attach_collection(never_ptr<instance_collection_base> i) = 0;
virtual	never_ptr<instance_collection_base>
		get_inst_base(void) = 0;
virtual	never_ptr<const instance_collection_base>
		get_inst_base(void) const = 0;

	string get_name(void) const;
	size_t dimensions(void) const;
	index_collection_item_ptr_type get_indices(void) const;

virtual	count_ptr<const fundamental_type_reference>
		get_type_ref(void) const = 0;

// should be pure virtual eventually
virtual	void unroll(void) const;

/***
	case: A top-level instantiation is called.
	case: won't happen?  instantiation inside definition called
***/
	// unroll (context?)
};	// end class instantiation_statement

//=============================================================================
/**
	Abstract base class for built-in parameter type instantiations.  
 */
class param_instantiation_statement : public instantiation_statement {
protected:
	param_instantiation_statement();
public:
	param_instantiation_statement(const index_collection_item_ptr_type& i);
virtual	~param_instantiation_statement();

};	// end class param_instantiation_statement

//-----------------------------------------------------------------------------
class pbool_instantiation_statement : public object, 
		public param_instantiation_statement {
public:
	typedef	pbool_instance_collection	collection_type;
	typedef	count_ptr<const param_type_reference>	type_ptr_type;
protected:
	// doesn't have a type_ref, hard-coded to built-ins
	never_ptr<collection_type>		inst_base;
private:
	pbool_instantiation_statement();
public:
	pbool_instantiation_statement(const index_collection_item_ptr_type& i);
	~pbool_instantiation_statement();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	void attach_collection(never_ptr<instance_collection_base> i);
	never_ptr<instance_collection_base> get_inst_base(void);
	never_ptr<const instance_collection_base> get_inst_base(void) const;
	count_ptr<const fundamental_type_reference> get_type_ref(void) const;

	void unroll(void) const;

public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS

};	// end class pbool_instantiation_statement

//-----------------------------------------------------------------------------
class pint_instantiation_statement : public object, 
		public param_instantiation_statement {
public:
	typedef	pint_instance_collection	collection_type;
	typedef	count_ptr<const param_type_reference>	type_ptr_type;
protected:
	// doesn't have a type_ref, hard-coded to built-ins
	never_ptr<collection_type>		inst_base;
private:
	pint_instantiation_statement();
public:
	pint_instantiation_statement(const index_collection_item_ptr_type& i);
	~pint_instantiation_statement();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	void attach_collection(never_ptr<instance_collection_base> i);
	never_ptr<instance_collection_base> get_inst_base(void);
	never_ptr<const instance_collection_base> get_inst_base(void) const;
	count_ptr<const fundamental_type_reference> get_type_ref(void) const;

	void unroll(void) const;

public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS

};	// end class pint_instantiation_statement

//=============================================================================
class process_instantiation_statement : public object, 
		public instantiation_statement {
public:
	typedef	process_instance_collection	collection_type;
	typedef	count_ptr<const process_type_reference>	type_ptr_type;
protected:
	const type_ptr_type			type;
	never_ptr<collection_type>		inst_base;
private:
	process_instantiation_statement();
public:
	process_instantiation_statement(const type_ptr_type& t, 
		const index_collection_item_ptr_type& i);
	~process_instantiation_statement();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	void attach_collection(never_ptr<instance_collection_base> i);
	never_ptr<instance_collection_base> get_inst_base(void);
	never_ptr<const instance_collection_base> get_inst_base(void) const;
	count_ptr<const fundamental_type_reference> get_type_ref(void) const;

public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS

};	// end class process_instantiation_statement

//=============================================================================
class channel_instantiation_statement : public object, 
		public instantiation_statement {
public:
	typedef	channel_instance_collection	collection_type;
	typedef	count_ptr<const channel_type_reference>	type_ptr_type;
protected:
	const type_ptr_type			type;
	never_ptr<collection_type>		inst_base;
private:
	channel_instantiation_statement();
public:
	channel_instantiation_statement(const type_ptr_type& t, 
		const index_collection_item_ptr_type& i);
	~channel_instantiation_statement();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	void attach_collection(never_ptr<instance_collection_base> i);
	never_ptr<instance_collection_base> get_inst_base(void);
	never_ptr<const instance_collection_base> get_inst_base(void) const;
	count_ptr<const fundamental_type_reference> get_type_ref(void) const;

public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS

};	// end class channel_instantiation_statement

//=============================================================================
class data_instantiation_statement : public object, 
		public instantiation_statement {
public:
	typedef	datatype_instance_collection	collection_type;
	typedef	count_ptr<const data_type_reference>	type_ptr_type;
protected:
	const type_ptr_type			type;
	never_ptr<collection_type>		inst_base;
private:
	data_instantiation_statement();
public:
	data_instantiation_statement(const type_ptr_type& t, 
		const index_collection_item_ptr_type& i);
	~data_instantiation_statement();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	void attach_collection(never_ptr<instance_collection_base> i);
	never_ptr<instance_collection_base> get_inst_base(void);
	never_ptr<const instance_collection_base> get_inst_base(void) const;
	count_ptr<const fundamental_type_reference> get_type_ref(void) const;

public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS

};	// end class data_instantiation_statement

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_H__

