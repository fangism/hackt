// "art_object_expr.h"

#ifndef __ART_OBJECT_EXPR_H__
#define __ART_OBJECT_EXPR_H__

#include "art_object_inst_ref.h"		// includes "art_object_base.h"
#include "discrete_interval_set_fwd.h"

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

	class param_type_reference;
	class pbool_type_reference;
	class pint_type_reference;

	class param_instantiation;
	class pbool_instantiation;
	class pint_instantiation;

	class param_instance_reference;
	class pint_instance_reference;
	class pbool_instance_reference;

// forward declarations of classes defined here (table of contents)
	class param_expr;
	class index_expr;		// BEWARE also in ART::parser!
	class const_index;
	class pbool_expr;
	class pint_expr;
//	class param_expr_collective;
//	class param_literal;
//	class pbool_literal;		// replaced by pbool_instance_reference
//	class pint_literal;		// replaced by pint_instance_reference
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
	class const_index_list;
	class dynamic_index_list;

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

virtual	size_t dimensions(void) const = 0;

/** is initialized if is resolved to constant or some other formal */
#if 0
virtual bool is_initialized(void) const = 0;
#else
virtual bool may_be_initialized(void) const = 0;
virtual bool must_be_initialized(void) const = 0;
#endif

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
virtual size_t dimensions(void) const = 0;
#if 0
virtual bool is_initialized(void) const = 0;
#else
virtual bool may_be_initialized(void) const = 0;
virtual bool must_be_initialized(void) const = 0;
#endif
virtual bool is_static_constant(void) const = 0;
virtual bool is_loop_independent(void) const = 0;
virtual bool is_unconditional(void) const = 0;

// additional virtual functions for dimensionality...
};	// end class index_expr

//-----------------------------------------------------------------------------
/**
	Abstract interface for constant indices and index ranges.  
 */
class const_index : virtual public index_expr {
protected:
public:
	const_index();
virtual	~const_index();

// same pure virtual functions
};	// end class const_index

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
virtual	size_t dimensions(void) const = 0;
#if 0
virtual bool is_initialized(void) const = 0;
#else
virtual bool may_be_initialized(void) const = 0;
virtual bool must_be_initialized(void) const = 0;
#endif
virtual bool is_static_constant(void) const = 0;
virtual bool is_loop_independent(void) const = 0;
virtual bool static_constant_bool(void) const = 0;
};	// end class pbool_expr

//-----------------------------------------------------------------------------
/**
	Abstract expression checked to be a single integer.  
 */
class pint_expr : virtual public param_expr, virtual public index_expr {
public:
	pint_expr() : param_expr(), index_expr() { }
virtual	~pint_expr() { }

virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const = 0;
virtual	string hash_string(void) const = 0;
virtual	size_t dimensions(void) const = 0;
#if 0
virtual bool is_initialized(void) const = 0;
#else
virtual bool may_be_initialized(void) const = 0;
virtual bool must_be_initialized(void) const = 0;
#endif
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
	size_t dimensions(void) const;
};	// end class param_expr_collective
**/

//=============================================================================
/**
	A reference to a instance of built-in type pbool.  
	Consider multiply deriving from pbool_expr, 
	and replacing pbool_literal.  
 */
class pbool_instance_reference : public param_instance_reference, 
		public pbool_expr {
protected:
	never_ptr<pbool_instantiation>		pbool_inst_ref;
public:
	pbool_instance_reference(never_ptr<pbool_instantiation> pi, 
		excl_ptr<index_list> i);
	~pbool_instance_reference() { }

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	using param_instance_reference::dump;
	never_const_ptr<instantiation_base> get_inst_base(void) const;
	never_const_ptr<param_instantiation>
		get_param_inst_base(void) const;

	size_t dimensions(void) const;

	bool initialize(count_const_ptr<param_expr> i);
	string hash_string(void) const;
	// implement later.
#if 0
	bool is_initialized(void) const;
#else
	// try these
	// using param_instance_reference::may_be_initialized;
	// using param_instance_reference::must_be_initialized;
	bool may_be_initialized(void) const;
	bool must_be_initialized(void) const;
#endif
	bool is_static_constant(void) const;
	bool is_unconditional(void) const;
	bool is_loop_independent(void) const;
	bool static_constant_bool(void) const;
};	// end class pbool_instance_reference

//-----------------------------------------------------------------------------
/**
	A reference to a instance of built-in type pint.  
	Consider multiply deriving from pint_expr, 
	and replacing pint_literal.  
 */
class pint_instance_reference : public param_instance_reference, 
		public pint_expr {
protected:
	never_ptr<pint_instantiation>		pint_inst_ref;
public:
	pint_instance_reference(never_ptr<pint_instantiation> pi, 
		excl_ptr<index_list> i);
	~pint_instance_reference() { }

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	never_const_ptr<instantiation_base> get_inst_base(void) const;
	never_const_ptr<param_instantiation>
		get_param_inst_base(void) const;

	size_t dimensions(void) const;

	bool initialize(count_const_ptr<param_expr> i);
	string hash_string(void) const;
	// implement later.
#if 0
	bool is_initialized(void) const;
#else
	bool may_be_initialized(void) const;
	bool must_be_initialized(void) const;
#endif
	bool is_static_constant(void) const;
	bool is_unconditional(void) const;
	bool is_loop_independent(void) const;
	int static_constant_int(void) const;
};	// end class pint_instance_reference

//=============================================================================
/**
	Constant integer parameters.  
	Currently limited in width by the machine's long size.  
 */
class pint_const : public pint_expr, public const_index {
protected:
	const long			val;
public:
	pint_const(const long v) : pint_expr(), const_index(), val(v) { }
	~pint_const() { }
	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;
	size_t dimensions(void) const { return 0; }

#if 0
	bool is_initialized(void) const { return true; }
#else
	bool may_be_initialized(void) const { return true; }
	bool must_be_initialized(void) const { return true; }
#endif
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
	size_t dimensions(void) const { return 0; }

#if 0
	bool is_initialized(void) const { return true; }
#else
	bool may_be_initialized(void) const { return true; }
	bool must_be_initialized(void) const { return true; }
#endif
	bool is_static_constant(void) const { return true; }
	bool static_constant_bool(void) const { return val; }
	bool is_loop_independent(void) const { return true; }
	bool is_unconditional(void) const { return true; }
};	// end class pbool_const

//-----------------------------------------------------------------------------
/**
	Only possibilities, unary negation, bit-wise negation.  
 */
class pint_unary_expr : public pint_expr {
protected:
	const char			op;
	/** expression argument must be 0-dimensional */
	count_const_ptr<pint_expr>	ex;
public:
	pint_unary_expr(const char o, count_const_ptr<pint_expr> e);
	pint_unary_expr(count_const_ptr<pint_expr> e, const char o);

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;
	size_t dimensions(void) const { return 0; }
#if 0
	bool is_initialized(void) const;
#else
	bool may_be_initialized(void) const
		{ return ex->may_be_initialized(); }
	bool must_be_initialized(void) const
		{ return ex->must_be_initialized(); }
#endif
	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
	bool is_unconditional(void) const;
	int static_constant_int(void) const;
};	// end class pint_unary_expr

//-----------------------------------------------------------------------------
/**
	Only possibility, NOT-expr.  
	Character may be '~' or '!'.  
 */
class pbool_unary_expr : public pbool_expr {
protected:
	const char			op;
	/** argument expression must be 0-dimensional */
	count_const_ptr<pbool_expr>	ex;
public:
	pbool_unary_expr(const char o, count_const_ptr<pbool_expr> e);
	pbool_unary_expr(count_const_ptr<pbool_expr> e, const char o);

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;
	size_t dimensions(void) const { return 0; }
#if 0
	bool is_initialized(void) const;
#else
	bool may_be_initialized(void) const
		{ return ex->may_be_initialized(); }
	bool must_be_initialized(void) const
		{ return ex->must_be_initialized(); }
#endif
	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
	bool is_unconditional(void) const;
	bool static_constant_bool(void) const;
};	// end class pbool_unary_expr

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
	size_t dimensions(void) const { return 0; }
#if 0
	bool is_initialized(void) const;
#else
	bool may_be_initialized(void) const
		{ return lx->may_be_initialized() && 
			rx->may_be_initialized(); }
	bool must_be_initialized(void) const
		{ return lx->must_be_initialized() && 
			rx->must_be_initialized(); }
#endif
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
	size_t dimensions(void) const { return 0; }
#if 0
	bool is_initialized(void) const;
#else
	bool may_be_initialized(void) const
		{ return lx->may_be_initialized() && 
			rx->may_be_initialized(); }
	bool must_be_initialized(void) const
		{ return lx->must_be_initialized() && 
			rx->must_be_initialized(); }
#endif
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
	size_t dimensions(void) const { return 0; }
#if 0
	bool is_initialized(void) const;
#else
	bool may_be_initialized(void) const
		{ return lx->may_be_initialized() && 
			rx->may_be_initialized(); }
	bool must_be_initialized(void) const
		{ return lx->must_be_initialized() && 
			rx->must_be_initialized(); }
#endif
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
class range_expr : virtual public index_expr {
protected:
public:
	range_expr();
virtual	~range_expr();

virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const;		// temporary
virtual	string hash_string(void) const = 0;

/**
	A range is always 0-dimensional, which is not the same
	as the dimensions *represented* by the range.  
	Is virtual so that sub-classes that use the index interface can
	override this.  
 */
virtual	size_t dimensions(void) const { return 0; }
/** is initialized if is resolved to constant or some other formal */
#if 0
virtual bool is_initialized(void) const = 0;
#else
virtual bool may_be_initialized(void) const = 0;
virtual bool must_be_initialized(void) const = 0;
#endif

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

#if 0
	bool is_initialized(void) const;
#else
	bool may_be_initialized(void) const {
		return lower->may_be_initialized() &&
			upper->may_be_initialized();
	}
	bool must_be_initialized(void) const {
		return lower->must_be_initialized() &&
			upper->must_be_initialized();
	}
#endif
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
	Deriving from pair to inherit its interface with first and second.  
 */
class const_range : public range_expr, public const_index,
		public pair<int,int> {
protected:
	typedef	pair<int,int>			parent;
	/** implementation type for range-checking */
	typedef	discrete_interval_set<int>	interval_type;
protected:
//	const interval_type			interval;
public:
	// dispense with pint_const objects here
	const_range();
	/** explicit conversion from x[N] to x[0..N-1] */
explicit const_range(const int n);
explicit const_range(const pint_const& n);
explicit const_range(const parent p);
	const_range(const int l, const int u);
	const_range(const const_range& r);
protected:
	const_range(const interval_type& i);
public:
	~const_range() { }

	/** use this to query whether or not range is valid */
	bool empty(void) const { return first > second; }
	int lower(void) const {
		assert(!empty());
		return first;
//		return interval.begin()->first;
	}
	int upper(void) const {
		assert(!empty());
		return second;
//		return interval.begin()->second;
	}
	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;

	const_range static_overlap(const const_range& r) const;

	bool operator == (const const_range& c) const;
#if 0
	bool is_initialized(void) const;
#else
	bool may_be_initialized(void) const { return !empty(); }
	bool must_be_initialized(void) const { return !empty(); }
#endif
	bool is_sane(void) const;
	bool is_static_constant(void) const { return !empty(); }
	bool is_loop_independent(void) const { return !empty(); }
	bool is_unconditional(void) const { return !empty(); }
};	// end class const_range

//=============================================================================
/**
	General interface to multidimensional indices.  
	Make interface like std::list.  
	Instance collection stack item?
	Replace instance_collection_stack_item with this!
	Elements of range_expr_list must be range_expr, 
	i.e. fully expanded, no shorthand ranges.  
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
public:
	const_range_list();
explicit const_range_list(const const_index_list& i);
	~const_range_list();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	size_t size(void) const;
	const_range_list static_overlap(const range_expr_list& r) const;

	bool operator == (const const_range_list& c) const;
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

	Doesn't make sense to ask how many dimensions are in an index_list
	because it depends on the instance_reference to which it is 
	attached.  
	Instead the index list can tell one how may dimensions
	are *collapsed* by the element types.  
 */
class index_list : public object {
public:
	index_list();
	~index_list();

// copy over most param_expr interface functions...
virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const = 0;
virtual	string hash_string(void) const = 0;

/** NOT THE SAME **/
virtual	size_t size(void) const = 0;
virtual	size_t dimensions_collapsed(void) const = 0;

#if 0
virtual	bool is_initialized(void) const = 0;
#else
virtual	bool may_be_initialized(void) const = 0;
virtual	bool must_be_initialized(void) const = 0;
#endif
virtual	bool is_static_constant(void) const = 0;
virtual	bool is_loop_independent(void) const = 0;
virtual	bool is_unconditional(void) const = 0;
};	// end class index_list

//-----------------------------------------------------------------------------
/**
	Index list whose indices are all constant.
	This means we need a const_index interface to objects.  
 */
class const_index_list : public index_list, 
		private list<count_ptr<const_index> > {
protected:
	/** need list of pointers b/c const_index is abstract */
	typedef	list<count_ptr<const_index> >	parent;
public:
	typedef parent::iterator		iterator;
	typedef parent::const_iterator		const_iterator;
	typedef parent::reverse_iterator	reverse_iterator;
	typedef parent::const_reverse_iterator	const_reverse_iterator;
public:
	const_index_list();
	~const_index_list();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;

/** NOT THE SAME **/
	size_t size(void) const;
	size_t dimensions_collapsed(void) const;

	using parent::begin;
	using parent::end;
	using parent::rbegin;
	using parent::rend;
	void push_back(const count_ptr<const_index>& i);

#if 0
	bool is_initialized(void) const;
#else
	bool may_be_initialized(void) const;
	bool must_be_initialized(void) const;
#endif
	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
	bool is_unconditional(void) const;
};	// end class const_index_list

//-----------------------------------------------------------------------------
/**
	Elements of this index list are no necessarily static constants.  
 */
class dynamic_index_list : public index_list, 
		private list<count_ptr<index_expr> > {
protected:
	typedef	list<count_ptr<index_expr> >	parent;
public:
	typedef parent::iterator		iterator;
	typedef parent::const_iterator		const_iterator;
	typedef parent::reverse_iterator	reverse_iterator;
	typedef parent::const_reverse_iterator	const_reverse_iterator;
public:
	dynamic_index_list();
	~dynamic_index_list();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;

	using parent::begin;
	using parent::end;
	using parent::rbegin;
	using parent::rend;
	void push_back(const count_ptr<index_expr>& i);

/** NOT THE SAME **/
	size_t size(void) const;
	size_t dimensions_collapsed(void) const;

#if 0
	bool is_initialized(void) const;
#else
	bool may_be_initialized(void) const;
	bool must_be_initialized(void) const;
#endif
	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
	bool is_unconditional(void) const;
};	// end class dynamic_index_list

//=============================================================================
}	// end namespace ART
}	// end namespace entity

#endif	// __ART_OBJECT_EXPR_H__

