// "art_object_expr.h"

#ifndef __ART_OBJECT_EXPR_H__
#define __ART_OBJECT_EXPR_H__

#include <iosfwd>
#include <string>

// #include "art_object.h"	// before including this header
#include "discrete_interval_set.h"

//=============================================================================
// note: need some way of hashing expression? 
//	using string of fully-qualified name?

/***
	Instead of making two flavors of queries, use an integer return
	value instead of a bool to enumerate:
		definitely no, don't know (maybe), definitely yes.  
		do we need may be yes, may be no?
	May be bit fields?
***/

//=============================================================================
namespace ART {
namespace entity {
	using std::string;
	using std::ostream;

//=============================================================================
// class forward declarations
	class object;

	class param_instantiation;
	class pbool_instantiation;
	class pint_instantiation;

	class param_type_reference;
	class pbool_type_reference;
	class pint_type_reference;


// forward declarations of classes defined here (table of contents)
	class param_expr;
	class index_expr;		// BEWARE also in ART::parser!
	class pbool_expr;
	class pint_expr;
//	class param_expr_collective;
	class param_literal;
	class pbool_literal;
	class pint_literal;
	class pint_const;
	class pbool_const;
	class param_unary_expr;
	class pint_unary_expr;
	class pbool_unary_expr;
	class param_binary_expr;
	class arith_expr;
	class relational_expr;
	class logical_expr;
	class range_expr;
	class pint_range;
	class const_range;
	class range_expr_list;
	class const_range_list;
	class dynamic_range_list;
//	class unconditional_range_list;
//	class conditional_range_list;
//	class loop_range_list;
	class index_list;

//============================================================================= 
/**
	Abstract base class for symbolic expressions to be written 
	to an object file.  
	Expression graphs must be directed-acyclic.  
	Deriving from object, so that it may be cached in 
	scopespaces' used_id_map.  
	Should statically sub-type into pints and pbools and pranges...
 */
class param_expr : virtual public object {
public:
	param_expr();
virtual	~param_expr();

virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const = 0;
virtual	string hash_string(void) const = 0;

/** is initialized if is resolved to constant or some other formal */
virtual bool is_initialized(void) const = 0;

/** can be resolved to static constant value */
virtual bool is_static_constant(void) const = 0;

/** doesn't depend on loop variables */
virtual bool is_loop_independent(void) const = 0;

/** doesn't depend on conditional variables */
virtual bool is_unconditional(void) const = 0;
};	// end class param_expr

//-----------------------------------------------------------------------------
/**
	Abstract interface to an index (not a range), which may be 
	a single integer interpreted as a dimension-collapse x[i], 
	or a range that preserves the dimension x[i..j].  
	For example, x[i] refers to a single instance (0-dimension)
	of the x array, whereas x[i..i] refers to a 
	1-dimensional sub-array of x, size 1, in this case.  
	A "range_expr" (below) may be an index, but not vice versa.  
 */
class index_expr : virtual public object {
protected:
public:
	index_expr();
virtual	~index_expr();

virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const = 0;
virtual	string hash_string(void) const = 0;
virtual bool is_initialized(void) const = 0;
virtual bool is_static_constant(void) const = 0;
virtual bool is_loop_independent(void) const = 0;
virtual bool is_unconditional(void) const = 0;
};	// end class index_expr

//-----------------------------------------------------------------------------

// const_index_expr interface?

//-----------------------------------------------------------------------------
/**
	Abstract expression checked to be a single boolean.  
 */
class pbool_expr : virtual public param_expr {
public:
	pbool_expr() : param_expr() { }
virtual	~pbool_expr() { }

virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const = 0;
virtual	string hash_string(void) const = 0;
virtual bool is_initialized(void) const = 0;
virtual bool is_static_constant(void) const = 0;
virtual bool is_loop_independent(void) const = 0;
virtual bool static_constant_bool(void) const = 0;
};	// end class pbool_expr

//-----------------------------------------------------------------------------
/**
	Abstract expression checked to be a single integer.  
 */
class pint_expr : virtual public param_expr, public index_expr {
public:
	pint_expr() : param_expr(), index_expr() { }
virtual	~pint_expr() { }

virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const = 0;
virtual	string hash_string(void) const = 0;
virtual bool is_initialized(void) const = 0;
virtual bool is_static_constant(void) const = 0;
virtual bool is_unconditional(void) const = 0;
virtual bool is_loop_independent(void) const = 0;
virtual int static_constant_int(void) const = 0;
};	// end class pint_expr

//-----------------------------------------------------------------------------
/**
	NOT SURE THIS CLASS IS USEFUL... eventually...
	For arrays of expressions.
	Statically type-checked.  
	Should sub-type...
	Consider multidimensional_qmap for unroll time.
class param_expr_collective : public param_expr {
protected:
	list<excl_ptr<param_expr> >	elist;
public:
	param_expr_collective();
	~param_expr_collective();

	ostream& what(ostream& o) const;
	string hash_string(void) const;
};	// end class param_expr_collective
**/

//-----------------------------------------------------------------------------
/**
	THIS class is POINTLESS, PHASE THIS OUT after 
		literals are merged with instance references.  
	This is the same as param_instance_reference.  

	Abstract interface.  
	A reference to single parameter instance.  
	Actually, can be collective too, just depends on var.  
	Should sub-type into pbool and pint...
 */
class param_literal : virtual public param_expr {
protected:
	/**
		OBSOLETE, virtualized.  
		The referencing pointer to the parameter instance.  
		Is modifiable because parameters may be initialized later. 
	count_ptr<param_instance_reference>	var;
	**/
public:
//	param_literal(count_ptr<param_instance_reference> v);
	param_literal() : param_expr() { }
virtual	~param_literal() { }

virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const = 0;
// virtual	string hash_string(void) const;
	// implement later.
	bool is_initialized(void) const;
	bool is_static_constant(void) const;
virtual	count_const_ptr<param_instance_reference> get_inst_ref(void) const = 0;
virtual	void initialize(count_const_ptr<param_expr> i) = 0;
	bool is_loop_independent(void) const;
	bool is_unconditional(void) const;
};	// end class param_literal

//-----------------------------------------------------------------------------
/**
	PHASE THIS INTO pbool_instance_reference.  Pointless wrapper this is.  

	A reference to single parameter instance.  
	Actually, can be collective too, just depends on var.  
	Should sub-type into pbool and pbool...
 */
class pbool_literal : public pbool_expr, public param_literal {
protected:
	/**
		The referencing pointer to the parameter instance.  
		Is modifiable because parameters may be initialized later. 
	 */
	count_ptr<pbool_instance_reference>	var;
public:
	pbool_literal(count_ptr<pbool_instance_reference> v);
	~pbool_literal();

	count_const_ptr<param_instance_reference> get_inst_ref(void) const;
	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;
	// implement later.
	bool is_initialized(void) const;
	bool is_static_constant(void) const;
	bool is_unconditional(void) const;
	bool is_loop_independent(void) const;
	bool static_constant_bool(void) const;
	void initialize(count_const_ptr<param_expr> i);
};	// end class pbool_literal

//-----------------------------------------------------------------------------
/**
	PHASE THIS INTO pint_instance_reference.  Pointless wrapper this is.  

	A reference to single parameter instance.  
	Actually, can be collective too, just depends on var.  
	Should sub-type into pbool and pint...
 */
class pint_literal : public pint_expr, public param_literal {
protected:
	/**
		The referencing pointer to the parameter instance.  
		Is modifiable because parameters may be initialized later. 
	 */
	count_ptr<pint_instance_reference>	var;
public:
	pint_literal(count_ptr<pint_instance_reference> v);
	~pint_literal();

	count_const_ptr<param_instance_reference> get_inst_ref(void) const;
	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;
	// implement later.
	bool is_initialized(void) const;
	bool is_static_constant(void) const;
	bool is_unconditional(void) const;
	bool is_loop_independent(void) const;
	int static_constant_int(void) const;
	void initialize(count_const_ptr<param_expr> i);
};	// end class pint_literal

//-----------------------------------------------------------------------------
/**
	Constant integer parameters.  
	Currently limited in width by the machine's long size.  
 */
class pint_const : public pint_expr {
protected:
	const long			val;
public:
	pint_const(const long v) : pint_expr(), val(v) { }
	~pint_const() { }
	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;
	bool is_initialized(void) const { return true; }
	bool is_static_constant(void) const { return true; }
	int static_constant_int(void) const { return val; }
	bool is_loop_independent(void) const { return true; }
	bool is_unconditional(void) const { return true; }
};	// end class pint_const

//-----------------------------------------------------------------------------
/**
	Constant boolean parameters, true or false.  
 */
class pbool_const : public pbool_expr {
protected:
	const bool			val;
public:
	pbool_const(const bool v) : pbool_expr(), val(v) { }
	~pbool_const() { }
	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;
	bool is_initialized(void) const { return true; }
	bool is_static_constant(void) const { return true; }
	bool static_constant_bool(void) const { return val; }
	bool is_loop_independent(void) const { return true; }
	bool is_unconditional(void) const { return true; }
};	// end class pbool_const

//-----------------------------------------------------------------------------
#if 0
/**
	Unary parameter expressions.  
	Pointer-class-ize.
	Subclassify into pints and pbools?
 */
class param_unary_expr : public param_expr {
public:
protected:
	const char			op;
	const param_expr*		ex;
public:
	param_unary_expr(const char o, const param_expr* e);
	param_unary_expr(const param_expr* e, const char o);
	~param_unary_expr() { }
	ostream& what(ostream& o) const;
	string hash_string(void) const;
	bool is_initialized(void) const { return ex->is_initialized(); }
	bool is_static_constant(void) const { return ex->is_static_constant(); }
};	// end class param_unary_expr
#endif

//-----------------------------------------------------------------------------
/**
	Only possibilites, unary negation, bit-wise negation.  
 */
class pint_unary_expr : public pint_expr {
protected:
	const char			op;
	count_const_ptr<pint_expr>	ex;
public:
	pint_unary_expr(const char o, count_const_ptr<pint_expr> e);
	pint_unary_expr(count_const_ptr<pint_expr> e, const char o);

	ostream& what(ostream& o) const;
	string hash_string(void) const;
	bool is_initialized(void) const;
	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
	bool is_unconditional(void) const;
	int static_constant_int(void) const;
};	// end class pint_unary_expr

//-----------------------------------------------------------------------------
/**
	Only possibility, NOT-expr.  
 */
class pbool_unary_expr : public pbool_expr {
protected:
	const char			op;
	count_const_ptr<pbool_expr>	ex;
public:
	pbool_unary_expr(const char o, count_const_ptr<pbool_expr> e);
	pbool_unary_expr(count_const_ptr<pbool_expr> e, const char o);

	ostream& what(ostream& o) const;
	string hash_string(void) const;
	bool is_initialized(void) const;
	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
	bool is_unconditional(void) const;
	bool static_constant_bool(void) const;
};	// end class pbool_unary_expr

//-----------------------------------------------------------------------------
#if 0
/**
	Binary parameter expressions.  
	Pointer-class-ize.
	Subclassify into pints and pbools?
 */
class param_binary_expr : public param_expr {
public:
protected:
	const char			op;
	const param_expr*		lx;
	const param_expr*		rx;
public:
	param_binary_expr(const param_expr* l, const char o, 
		const param_expr* r);
	~param_binary_expr() { }
	ostream& what(ostream& o) const;
	string hash_string(void) const;
	bool is_initialized(void) const {
		return lx->is_initialized() && rx->is_initialized();
	}
	bool is_static_constant(void) const {
		return lx->is_static_constant() && rx->is_static_constant();
	}
};	// end class param_binary_expr
#endif

//-----------------------------------------------------------------------------
/**
	Binary arithmetic expression accepts ints and returns an int.  
 */
class arith_expr : public pint_expr {
protected:
	count_const_ptr<pint_expr>	lx;
	count_const_ptr<pint_expr>	rx;
	const char			op;
public:
	arith_expr(count_const_ptr<pint_expr> l, const char o, 
		count_const_ptr<pint_expr> r);
	~arith_expr() { }

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;
	bool is_initialized(void) const;
	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
	bool is_unconditional(void) const;
	int static_constant_int(void) const;
};	// end class arith_expr

//-----------------------------------------------------------------------------
/**
	Binary relational expression accepts ints and returns a bool.  
 */
class relational_expr : public pbool_expr {
protected:
	count_const_ptr<pint_expr>	lx;
	count_const_ptr<pint_expr>	rx;
	const string			op;

public:
	relational_expr(count_const_ptr<pint_expr> l, const string& o, 
		count_const_ptr<pint_expr> r);
	~relational_expr() { }

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;
	bool is_initialized(void) const;
	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
	bool is_unconditional(void) const;
	bool static_constant_bool(void) const;
};	// end class relational_expr

//-----------------------------------------------------------------------------
/**
	Binary logical expression accepts bools and returns a bool.  
 */
class logical_expr : public pbool_expr {
protected:
	count_const_ptr<pbool_expr>	lx;
	count_const_ptr<pbool_expr>	rx;
	const string			op;

public:
	logical_expr(count_const_ptr<pbool_expr> l, const string& o, 
		count_const_ptr<pbool_expr> r);
	~logical_expr() { }

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;
	bool is_initialized(void) const;
	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
	bool is_unconditional(void) const;
	bool static_constant_bool(void) const;
};	// end class logical_expr

//=============================================================================
/**
	Abstract interface to range expression objects.  
	Note: range_expr is not a param_expr.
	Range expressions are not the same as index expressions:
	Range expressions are used exclusively for declarations of arrays, 
	whereas index expressions are used for instance references.  
	Since the syntax allows declaration of x[N] as a short-hand for 
	x[0..N-1], we will need to explicitly convert from the former 
	to the latter.  
 */
class range_expr : virtual public object, public index_expr {
protected:
public:
	range_expr();
virtual	~range_expr();

virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const;		// temporary
virtual	string hash_string(void) const = 0;

/** is initialized if is resolved to constant or some other formal */
virtual bool is_initialized(void) const = 0;

/** is sane if range makes sense */
virtual	bool is_sane(void) const = 0;

/** can be resolved to static constant value */
virtual bool is_static_constant(void) const = 0;

/** doesn't depend on loop variables */
virtual bool is_loop_independent(void) const = 0;

/** doesn't depend on conditional variables */
virtual bool is_unconditional(void) const = 0;
};	// end class range_expr

//-----------------------------------------------------------------------------
/**
	Range expression object, not necessarily constant.  
	Must contain pint_expr's.
	Derive from object or param_expr?
 */
class pint_range : public range_expr {
protected:
	// need to be const, or modifiable?
	count_const_ptr<pint_expr>	lower;
	count_const_ptr<pint_expr>	upper;
public:
	/** implicit conversion from x[N] to x[0..N-1] */
explicit pint_range(count_const_ptr<pint_expr> n);
	pint_range(count_const_ptr<pint_expr> l,
		count_const_ptr<pint_expr> u);
	pint_range(const pint_range& pr);
	~pint_range() { }

	// first, second? pair interface?

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;		// unused?

	bool is_initialized(void) const;
	bool is_sane(void) const;
	bool is_static_constant(void) const;
	// for now just return false, don't bother checking recursively...
	bool is_loop_independent(void) const { return false; }
	bool is_unconditional(void) const { return false; }
	const_range static_constant_range(void) const;
};	// end class pint_range

//-----------------------------------------------------------------------------
/**
	Constant version of range expression.  
 */
class const_range : public range_expr {
protected:
	/** implementation type for range */
	typedef	discrete_interval_set<int>	impl_type;
protected:
#if 0
	const int			lower;
	const int			upper;
#else
	const impl_type			interval;
#endif
public:
	// dispense with pint_const objects here
	const_range();
	/** explicit conversion from x[N] to x[0..N-1] */
explicit const_range(const int n);
	const_range(const int l, const int u);
	const_range(const const_range& r);
protected:
	const_range(const impl_type& i);
public:
	~const_range() { }

	/** use this to query whether or not range is valid */
	bool empty(void) const { return interval.empty(); }
	int lower(void) const {
		assert(!interval.empty());	// only one member
		return interval.begin()->first;
	}
	int upper(void) const {
		assert(!interval.empty());	// only one member
		return interval.begin()->second;
	}
	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;

	const_range static_overlap(const const_range& r) const;
	bool is_initialized(void) const;
	bool is_sane(void) const;
	bool is_static_constant(void) const { return true; }
	bool is_loop_independent(void) const { return true; }
	bool is_unconditional(void) const { return false; }
};	// end class const_range

//=============================================================================
/**
	General interface to multidimensional indices.  
	Make interface like std::list.  
	Instance collection stack item?
	Replace instance_collection_stack_item with this!
 */
class range_expr_list : public object {
protected:
//	never_const_ptr<instantiation_base>	owner;
public:
	range_expr_list();
virtual	~range_expr_list() { }

virtual	size_t size(void) const = 0;
	size_t dimensions(void) const { return size(); }
virtual	const_range_list static_overlap(const range_expr_list& r) const = 0;
};	// end class range_expr_list

//-----------------------------------------------------------------------------
/**
	List of constant range expressions.  
	Would a vector be more appropriate?   consider changing later...
 */
class const_range_list : public range_expr_list, public list<const_range> {
protected:
	// no need for pointers here
	typedef	list<const_range>	list_type;
protected:
//	list_type			index_ranges;
public:
	const_range_list();
	~const_range_list();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	size_t size(void) const;
	const_range_list static_overlap(const range_expr_list& r) const;
};	// end class const_range_list

//-----------------------------------------------------------------------------
/**
	Base class.  
	List of range expressions, not necessarily constant.  
	May contain constant ranges however.  
	Interface methods for unrolling.  

	Replace the dynamic subclasses of instance_collection_stack_item
	with this class by adding attributes...
		is_unconditional
		is_loop_independent
	also cache these results...?
 */
class dynamic_range_list : public range_expr_list,
		public list<count_ptr<pint_range> > {
protected:
	// list of pointers to pint_ranges?  or just copy construct?
	// can't copy construct, is abstract
	typedef	list<count_ptr<pint_range> >	list_type;
protected:
//	list_type			index_ranges;
public:
	dynamic_range_list();
virtual	~dynamic_range_list();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	size_t size(void) const;
	const_range_list static_overlap(const range_expr_list& r) const;
		// false, will be empty
};	// end class dynamic_range_list

//-----------------------------------------------------------------------------
#if 0
/**
	Expression parameters may depend on template formals, for example.
	Expressions are unconditional and loop invariant.  
 */
class unconditional_range_list : public dynamic_range_list {
public:
	unconditional_range_list();
	~unconditional_range_list();

};	// end class unconditional_range_list

//-----------------------------------------------------------------------------
/**
	List of range expressions, that may be found in some
	conditional body.  
	Need back-reference to enclosing conditional body.  
 */
class conditional_range_list : public dynamic_range_list {

};	// end class conditional_range_list

//-----------------------------------------------------------------------------
/**
	List of range expression, some of which may be loop-variant.  
	Need back-reference to encapsulating loop.  
 */
class loop_range_list : public dynamic_range_list {

};	// end class loop_range_list
#endif

//=============================================================================

// make base class for index_list?

//-----------------------------------------------------------------------------
/**
	May contained mixed pint_expr and ranges!
	Don't forget their interpretation differs!
	pint_expr is interpreted as a dimension collapse, 
	whereas range_expr preserves dimension, even if range is one.  
 */
class index_list : public object {
protected:
	typedef	list<count_ptr<index_expr> >	list_type;
protected:
	list_type				indices;
//	size_t					dim;
public:
	index_list();
	~index_list();

// copy over most param_expr interface functions...
	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;

	size_t size(void) const;
	size_t dimensions(void) const;

	bool is_initialized(void) const;
	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
	bool is_unconditional(void) const;
};	// end class index_list

//=============================================================================
}	// end namespace ART
}	// end namespace entity

#endif	// __ART_OBJECT_EXPR_H__

