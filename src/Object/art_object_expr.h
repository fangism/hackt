/**
	\file "art_object_expr.h"
	Classes related to program expressions, symbolic and parameters.  
	$Id: art_object_expr.h,v 1.17 2004/12/11 06:22:42 fang Exp $
 */

#ifndef __ART_OBJECT_EXPR_H__
#define __ART_OBJECT_EXPR_H__

#include "art_object_expr_const.h"	// include "art_object_expr_base.h"
#include "qmap.h"
#include "operators.h"

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

USING_LIST
using std::string;
using std::ostream;
USING_UTIL_OPERATIONS
using QMAP_NAMESPACE::qmap;
using namespace MULTIKEY_NAMESPACE;

//=============================================================================
/**
	Generalized list of parameter expressions, can be dynamic.  
 */
class dynamic_param_expr_list : public param_expr_list, 
		public list<count_ptr<const param_expr> > {
friend class const_param_expr_list;
protected:
	typedef	list<count_ptr<const param_expr> >	parent;
public:
	typedef parent::iterator		iterator;
	typedef parent::const_iterator		const_iterator;
	typedef parent::reverse_iterator	reverse_iterator;
	typedef parent::const_reverse_iterator	const_reverse_iterator;
public:
	dynamic_param_expr_list();
// lazy: use default copy constructor
//	dynamic_param_expr_list(const dynamic_param_expr_list& pl);
	~dynamic_param_expr_list();

	size_t size(void) const;

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;

	excl_ptr<param_expr_list>
		make_copy(void) const;

	bool may_be_initialized(void) const;
	bool must_be_initialized(void) const;

#if 0
	list<const param_expr&>	get_const_ref_list(void) const;
#else
	bool may_be_equivalent(const param_expr_list& p) const;
	bool must_be_equivalent(const param_expr_list& p) const;
#endif

	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
#if 0
private:
	bool may_be_equivalent_const(const const_param_expr_list& p) const;
	bool may_be_equivalent_dynamic(const dynamic_param_expr_list& p) const;
	bool must_be_equivalent_const(const const_param_expr_list& p) const;
	bool must_be_equivalent_dynamic(const dynamic_param_expr_list& p) const;
#endif
public:
	PERSISTENT_METHODS
};	// end class dynamic_param_expr_list

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

	bool may_be_initialized(void) const;
	bool must_be_initialized(void) const;
	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
	bool is_unconditional(void) const;

	const_index_list resolve_index_list(void) const;
#if 0
	bool resolve_multikey(excl_ptr<multikey_base<int> >& k) const;
#endif
public:
	PERSISTENT_METHODS
};	// end class dynamic_index_list

//=============================================================================
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
	typedef	list_type::iterator			iterator;
	typedef	list_type::const_iterator		const_iterator;
	typedef	list_type::reverse_iterator		reverse_iterator;
	typedef	list_type::const_reverse_iterator	const_reverse_iterator;
public:
	dynamic_range_list();
virtual	~dynamic_range_list();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	size_t size(void) const;
	bool is_static_constant(void) const;
	const_range_list static_overlap(const range_expr_list& r) const;
		// false, will be empty
	bool resolve_ranges(const_range_list& r) const;
public:
	PERSISTENT_METHODS
};	// end class dynamic_range_list

//=============================================================================
/**
	Only possibilities, unary negation, bit-wise negation.  
 */
class pint_unary_expr : public pint_expr {
public:
	typedef	char			op_type;
protected:
	const op_type			op;
	/** expression argument must be 0-dimensional */
	count_ptr<const pint_expr>	ex;
private:
	pint_unary_expr();
public:
	pint_unary_expr(const op_type o, count_ptr<const pint_expr> e);
	pint_unary_expr(count_ptr<const pint_expr> e, const op_type o);

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;
	size_t dimensions(void) const { return 0; }
	bool has_static_constant_dimensions(void) const { return true; }
	const_range_list static_constant_dimensions(void) const
		{ return const_range_list(); }
	bool may_be_initialized(void) const
		{ return ex->may_be_initialized(); }
	bool must_be_initialized(void) const
		{ return ex->must_be_initialized(); }
	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
	bool is_unconditional(void) const;
	int static_constant_int(void) const;

	bool resolve_value(int& i) const;
	const_index_list resolve_dimensions(void) const;
	bool resolve_values_into_flat_list(list<int>& l) const;
public:
	PERSISTENT_METHODS
};	// end class pint_unary_expr

//-----------------------------------------------------------------------------
/**
	Only possibility, NOT-expr.  
	Character may be '~' or '!'.  
 */
class pbool_unary_expr : public pbool_expr {
public:
	typedef	char			op_type;
protected:
	const op_type			op;
	/** argument expression must be 0-dimensional */
	count_ptr<const pbool_expr>	ex;
private:
	pbool_unary_expr();
public:
	pbool_unary_expr(const op_type o, count_ptr<const pbool_expr> e);
	pbool_unary_expr(count_ptr<const pbool_expr> e, const op_type o);

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;
	size_t dimensions(void) const { return 0; }
	bool has_static_constant_dimensions(void) const { return true; }
	const_range_list static_constant_dimensions(void) const
		{ return const_range_list(); }
	bool may_be_initialized(void) const
		{ return ex->may_be_initialized(); }
	bool must_be_initialized(void) const
		{ return ex->must_be_initialized(); }
	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
	bool is_unconditional(void) const;
	bool static_constant_bool(void) const;

	bool resolve_value(bool& i) const;
	const_index_list resolve_dimensions(void) const;
	bool resolve_values_into_flat_list(list<bool>& l) const;
public:
	PERSISTENT_METHODS
};	// end class pbool_unary_expr

//-----------------------------------------------------------------------------
/**
	Binary arithmetic expression accepts ints and returns an int.  
 */
class arith_expr : public pint_expr {
public:
	typedef	binary_arithmetic_operation<int,int>	op_type;
	static const plus<int,int>		adder;
	static const minus<int,int>		subtractor;
	static const multiplies<int,int>	multiplier;
	static const divides<int,int>		divider;
	static const modulus<int,int>		remainder;

private:
	// safe to use naked (never-delete) pointers on static objects
	typedef	qmap<char, const op_type*>	op_map_type;
	typedef	qmap<const op_type*, char>	reverse_op_map_type;
	static const op_map_type		op_map;
	static const reverse_op_map_type	reverse_op_map;
	static const size_t			op_map_size;
	static void op_map_register(const char, const op_type* );
	static size_t op_map_init(void);
protected:
	count_ptr<const pint_expr>	lx;
	count_ptr<const pint_expr>	rx;
#if 0
	const char			op;
#else
	/**
		Safe to use a naked pointer, b/c/ refers to a static object.  
	 */
	const op_type*			op;
#endif
private:
	arith_expr();
public:
	// change: const ptr& arguments
	arith_expr(count_ptr<const pint_expr> l, const char o, 
		count_ptr<const pint_expr> r);
	~arith_expr();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;
	size_t dimensions(void) const { return 0; }
	bool has_static_constant_dimensions(void) const { return true; }
	const_range_list static_constant_dimensions(void) const
		{ return const_range_list(); }
	bool may_be_initialized(void) const
		{ return lx->may_be_initialized() && 
			rx->may_be_initialized(); }
	bool must_be_initialized(void) const
		{ return lx->must_be_initialized() && 
			rx->must_be_initialized(); }
	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
	bool is_unconditional(void) const;
	int static_constant_int(void) const;
	bool resolve_value(int& i) const;
	const_index_list resolve_dimensions(void) const;
	bool resolve_values_into_flat_list(list<int>& l) const;
public:
	PERSISTENT_METHODS
};	// end class arith_expr

//-----------------------------------------------------------------------------
/**
	Binary relational expression accepts ints and returns a bool.  
 */
class relational_expr : public pbool_expr {
public:
	typedef	binary_relational_operation<bool,int>	op_type;
	static const equal_to<bool,int>		op_equal_to;
	static const not_equal_to<bool,int>	op_not_equal_to;
	static const less<bool,int>		op_less;
	static const greater<bool,int>		op_greater;
	static const less_equal<bool,int>	op_less_equal;
	static const greater_equal<bool,int>	op_greater_equal;
private:
	// safe to use naked (never-delete) pointers on static objects
	typedef	qmap<string, const op_type*>	op_map_type;
	typedef	qmap<const op_type*, string>	reverse_op_map_type;
	static const op_map_type		op_map;
	static const reverse_op_map_type	reverse_op_map;
	static const size_t			op_map_size;
	static void op_map_register(const string&, const op_type* );
	static size_t op_map_init(void);

protected:
	count_ptr<const pint_expr>	lx;
	count_ptr<const pint_expr>	rx;
#if 0
	const string			op;
#else
	const op_type*			op;
#endif

private:
	relational_expr();
public:
	relational_expr(count_ptr<const pint_expr> l, const string& o, 
		count_ptr<const pint_expr> r);
	~relational_expr();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;
	size_t dimensions(void) const { return 0; }
	bool has_static_constant_dimensions(void) const { return true; }
	const_range_list static_constant_dimensions(void) const
		{ return const_range_list(); }
	bool may_be_initialized(void) const
		{ return lx->may_be_initialized() && 
			rx->may_be_initialized(); }
	bool must_be_initialized(void) const
		{ return lx->must_be_initialized() && 
			rx->must_be_initialized(); }
	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
	bool is_unconditional(void) const;
	bool static_constant_bool(void) const;

	bool resolve_value(bool& i) const;
	const_index_list resolve_dimensions(void) const;
	bool resolve_values_into_flat_list(list<bool>& l) const;
public:
	PERSISTENT_METHODS
};	// end class relational_expr

//-----------------------------------------------------------------------------
/**
	Binary logical expression accepts bools and returns a bool.  
 */
class logical_expr : public pbool_expr {
public:
	typedef	binary_logical_operation<bool,bool>	op_type;
	static const util::logical_and<bool,bool>	op_and;
	static const util::logical_or<bool,bool>	op_or;
	static const util::logical_xor<bool,bool>	op_xor;
private:
	// safe to use naked (never-delete) pointers on static objects
	typedef	qmap<string, const op_type*>	op_map_type;
	typedef	qmap<const op_type*, string>	reverse_op_map_type;
	static const op_map_type		op_map;
	static const reverse_op_map_type	reverse_op_map;
	static const size_t			op_map_size;
	static void op_map_register(const string&, const op_type* );
	static size_t op_map_init(void);

protected:
	count_ptr<const pbool_expr>	lx;
	count_ptr<const pbool_expr>	rx;
#if 0
	const string			op;
#else
	const op_type*			op;
#endif

private:
	logical_expr();
public:
	logical_expr(count_ptr<const pbool_expr> l, const string& o, 
		count_ptr<const pbool_expr> r);
	~logical_expr();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;
	size_t dimensions(void) const { return 0; }
	bool has_static_constant_dimensions(void) const { return true; }
	const_range_list static_constant_dimensions(void) const
		{ return const_range_list(); }
	bool may_be_initialized(void) const
		{ return lx->may_be_initialized() && 
			rx->may_be_initialized(); }
	bool must_be_initialized(void) const
		{ return lx->must_be_initialized() && 
			rx->must_be_initialized(); }
	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
	bool is_unconditional(void) const;
	bool static_constant_bool(void) const;

	bool resolve_value(bool& i) const;
	const_index_list resolve_dimensions(void) const;
	bool resolve_values_into_flat_list(list<bool>& l) const;
public:
	PERSISTENT_METHODS
};	// end class logical_expr

//=============================================================================
/**
	Range expression object, not necessarily constant.  
	Must contain pint_expr's.
	Derive from object or param_expr?
 */
class pint_range : public range_expr {
protected:
	// need to be const, or modifiable?
	count_ptr<const pint_expr>	lower;
	count_ptr<const pint_expr>	upper;
private:
	pint_range();
public:
	/** implicit conversion from x[N] to x[0..N-1] */
explicit pint_range(count_ptr<const pint_expr> n);
	pint_range(count_ptr<const pint_expr> l,
		count_ptr<const pint_expr> u);
	pint_range(const pint_range& pr);
	~pint_range();

	// first, second? pair interface?

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	string hash_string(void) const;		// unused?

	bool may_be_initialized(void) const {
		return lower->may_be_initialized() &&
			upper->may_be_initialized();
	}
	bool must_be_initialized(void) const {
		return lower->must_be_initialized() &&
			upper->must_be_initialized();
	}
	bool is_sane(void) const;
	bool is_static_constant(void) const;
	// for now just return false, don't bother checking recursively...
	bool is_loop_independent(void) const { return false; }
	bool is_unconditional(void) const { return false; }
	const_range static_constant_range(void) const;

	bool resolve_range(const_range& r) const;
public:
	PERSISTENT_METHODS
};	// end class pint_range

//=============================================================================
}	// end namespace ART
}	// end namespace entity

#endif	// __ART_OBJECT_EXPR_H__

