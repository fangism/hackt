/**
	\file "Object/art_object_expr.h"
	Classes related to program expressions, symbolic and parameters.  
	$Id: art_object_expr.h,v 1.29.4.3 2005/07/05 01:16:24 fang Exp $
 */

#ifndef __OBJECT_ART_OBJECT_EXPR_H__
#define __OBJECT_ART_OBJECT_EXPR_H__

#if 0
#include "Object/art_object_expr_const.h"	// include "art_object_expr_base.h"
#include "util/memory/count_ptr.h"
#include "util/qmap.h"
#include "util/operators.h"

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
class pint_range;
USING_LIST
using std::string;
using std::ostream;
USING_UTIL_OPERATIONS
using util::qmap;
using util::memory::count_ptr;

//=============================================================================
/**
	Generalized list of parameter expressions, can be dynamic.  
 */
class dynamic_param_expr_list : public param_expr_list, 
		public vector<count_ptr<const param_expr> > {
friend class const_param_expr_list;
	typedef	dynamic_param_expr_list			this_type;
	typedef	param_expr_list				interface_type;
protected:
	typedef	vector<count_ptr<const param_expr> >	parent_type;
public:
	typedef parent_type::iterator			iterator;
	typedef parent_type::const_iterator		const_iterator;
	typedef parent_type::reverse_iterator		reverse_iterator;
	typedef parent_type::const_reverse_iterator	const_reverse_iterator;
public:
	dynamic_param_expr_list();
// lazy: use default copy constructor
//	dynamic_param_expr_list(const dynamic_param_expr_list& pl);
	~dynamic_param_expr_list();

	size_t
	size(void) const;

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	excl_ptr<param_expr_list>
	make_copy(void) const;

	count_ptr<const param_expr>
	operator [] (const size_t) const;

	bool
	may_be_initialized(void) const;

	bool
	must_be_initialized(void) const;

	bool
	may_be_equivalent(const param_expr_list& p) const;

	bool
	must_be_equivalent(const param_expr_list& p) const;

	bool
	is_static_constant(void) const;

	bool
	is_loop_independent(void) const;

	unroll_resolve_return_type
	unroll_resolve(const unroll_context&) const;

	good_bool
	certify_template_arguments(const template_formals_list_type&);

public:
	PERSISTENT_METHODS_DECLARATIONS
};	// end class dynamic_param_expr_list

//-----------------------------------------------------------------------------
/**
	Elements of this index list are no necessarily static constants.  
 */
class dynamic_meta_index_list : public meta_index_list, 
		private list<count_ptr<meta_index_expr> > {
	typedef	dynamic_meta_index_list			this_type;
protected:
	typedef	list<count_ptr<meta_index_expr> >	parent_type;
public:
	typedef parent_type::iterator			iterator;
	typedef parent_type::const_iterator		const_iterator;
	typedef parent_type::reverse_iterator		reverse_iterator;
	typedef parent_type::const_reverse_iterator	const_reverse_iterator;
public:
	dynamic_meta_index_list();
	~dynamic_meta_index_list();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	using parent_type::begin;
	using parent_type::end;
	using parent_type::rbegin;
	using parent_type::rend;

	void
	push_back(const count_ptr<meta_index_expr>& i);

/** NOT THE SAME **/
	size_t
	size(void) const;

	size_t
	dimensions_collapsed(void) const;

	bool
	may_be_initialized(void) const;

	bool
	must_be_initialized(void) const;

	bool
	is_static_constant(void) const;

	bool
	is_loop_independent(void) const;

	bool
	is_unconditional(void) const;

	const_index_list
	resolve_index_list(void) const;

#if 0
	bool
	resolve_multikey(excl_ptr<multikey_index_type>& k) const;
#endif

	const_index_list
	unroll_resolve(const unroll_context&) const;

	bool
	must_be_equivalent_indices(const meta_index_list& ) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
};	// end class dynamic_meta_index_list

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
class dynamic_meta_range_list : public meta_range_list,
		public list<count_ptr<pint_range> > {
	typedef	dynamic_meta_range_list			this_type;
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
	dynamic_meta_range_list();

	~dynamic_meta_range_list();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	size_t
	size(void) const;

	bool
	is_static_constant(void) const;

	const_range_list
	static_overlap(const meta_range_list& r) const;
		// false, will be empty
	good_bool
	resolve_ranges(const_range_list& r) const;

	good_bool
	unroll_resolve(const_range_list&, const unroll_context&) const;

	bool
	must_be_formal_size_equivalent(const meta_range_list& ) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
};	// end class dynamic_meta_range_list

//=============================================================================
/**
	Only possibilities, unary negation, bit-wise negation.  
 */
class pint_unary_expr : public pint_expr {
	typedef	pint_unary_expr			this_type;
public:
	typedef	pint_value_type		value_type;
	typedef	char			op_type;
protected:
	const op_type			op;
	/** expression argument must be 0-dimensional */
	count_ptr<const pint_expr>	ex;
private:
	pint_unary_expr();
public:
	pint_unary_expr(const op_type o, const count_ptr<const pint_expr>& e);
	pint_unary_expr(const count_ptr<const pint_expr>& e, const op_type o);

	// default destructor

	ostream&
	what(ostream& o) const;

	ostream&
	dump_brief(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	size_t
	dimensions(void) const { return 0; }

	bool
	has_static_constant_dimensions(void) const { return true; }

	const_range_list
	static_constant_dimensions(void) const
		{ return const_range_list(); }

	bool
	may_be_initialized(void) const { return ex->may_be_initialized(); }

	bool
	must_be_initialized(void) const { return ex->must_be_initialized(); }

	bool
	is_static_constant(void) const;

	bool
	is_loop_independent(void) const;

	bool
	is_unconditional(void) const;

	value_type
	static_constant_value(void) const;

	bool
	must_be_equivalent(const pint_expr& ) const;

	good_bool
	resolve_value(value_type& i) const;

	good_bool
	unroll_resolve_value(const unroll_context&, value_type& i) const;

	const_index_list
	resolve_dimensions(void) const;

	good_bool
	resolve_values_into_flat_list(list<value_type>& l) const;

	count_ptr<const_param>
	unroll_resolve(const unroll_context&) const;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class pint_unary_expr

//-----------------------------------------------------------------------------
/**
	Only possibility, NOT-expr.  
	Character may be '~' or '!'.  
 */
class pbool_unary_expr : public pbool_expr {
	typedef	pbool_unary_expr		this_type;
public:
	typedef	pbool_value_type	value_type;
	typedef	char			op_type;
protected:
	const op_type			op;
	/** argument expression must be 0-dimensional */
	count_ptr<const pbool_expr>	ex;
private:
	pbool_unary_expr();
public:
	pbool_unary_expr(const op_type o, const count_ptr<const pbool_expr>& e);
	pbool_unary_expr(const count_ptr<const pbool_expr>& e, const op_type o);

	ostream&
	what(ostream& o) const;

	ostream&
	dump_brief(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	size_t
	dimensions(void) const { return 0; }

	bool
	has_static_constant_dimensions(void) const { return true; }

	const_range_list
	static_constant_dimensions(void) const
		{ return const_range_list(); }

	bool
	may_be_initialized(void) const { return ex->may_be_initialized(); }

	bool
	must_be_initialized(void) const { return ex->must_be_initialized(); }

	bool
	is_static_constant(void) const;

	bool
	is_loop_independent(void) const;

	bool
	is_unconditional(void) const;

	value_type
	static_constant_value(void) const;

	bool
	must_be_equivalent(const pbool_expr& ) const;

	good_bool
	resolve_value(value_type& i) const;

	good_bool
	unroll_resolve_value(const unroll_context&, value_type& i) const;

	const_index_list
	resolve_dimensions(void) const;

	good_bool
	resolve_values_into_flat_list(list<value_type>& l) const;

	count_ptr<const_param>
	unroll_resolve(const unroll_context&) const;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class pbool_unary_expr

//-----------------------------------------------------------------------------
/**
	Binary arithmetic expression accepts ints and returns an int.  
 */
class pint_arith_expr : public pint_expr {
	typedef	pint_arith_expr			this_type;
public:
	typedef	pint_value_type			arg_type;
	typedef	pint_value_type			value_type;
	typedef	count_ptr<const pint_expr>	operand_ptr_type;
	typedef	binary_arithmetic_operation<value_type, arg_type>
						op_type;
	static const plus<value_type, arg_type>		adder;
	static const minus<value_type, arg_type>	subtractor;
	static const multiplies<value_type, arg_type>	multiplier;
	static const divides<value_type, arg_type>	divider;
	static const modulus<value_type, arg_type>	remainder;

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
	operand_ptr_type		lx;
	operand_ptr_type		rx;

	/**
		Safe to use a naked pointer, b/c/ refers to a static object.  
	 */
	const op_type*			op;
private:
	pint_arith_expr();
public:
	// change: const ptr& arguments
	pint_arith_expr(const operand_ptr_type& l, const char o, 
		const operand_ptr_type& r);

	~pint_arith_expr();

	ostream&
	what(ostream& o) const;

	ostream&
	dump_brief(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	size_t
	dimensions(void) const { return 0; }

	bool
	has_static_constant_dimensions(void) const { return true; }

	const_range_list
	static_constant_dimensions(void) const
		{ return const_range_list(); }

	bool
	may_be_initialized(void) const
		{ return lx->may_be_initialized() && rx->may_be_initialized(); }

	bool
	must_be_initialized(void) const {
		return lx->must_be_initialized() && rx->must_be_initialized();
	}

	bool
	is_static_constant(void) const;

	bool
	is_loop_independent(void) const;

	bool
	is_unconditional(void) const;

	value_type
	static_constant_value(void) const;

	bool
	must_be_equivalent(const pint_expr& ) const;

	good_bool
	resolve_value(value_type& i) const;

	good_bool
	unroll_resolve_value(const unroll_context&, value_type& i) const;

	const_index_list
	resolve_dimensions(void) const;

	good_bool
	resolve_values_into_flat_list(list<value_type>& l) const;

	count_ptr<const_param>
	unroll_resolve(const unroll_context&) const;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class pint_arith_expr

//-----------------------------------------------------------------------------
/**
	Binary relational expression accepts ints and returns a bool.  
 */
class pint_relational_expr : public pbool_expr {
	typedef	pint_relational_expr			this_type;
public:
	typedef	pbool_value_type		value_type;
	typedef	pint_value_type			arg_type;
	typedef	count_ptr<const pint_expr>	operand_ptr_type;
	typedef	binary_relational_operation<value_type, arg_type>
							op_type;
	static const equal_to<value_type, arg_type>	op_equal_to;
	static const not_equal_to<value_type, arg_type>	op_not_equal_to;
	static const less<value_type, arg_type>		op_less;
	static const greater<value_type, arg_type>	op_greater;
	static const less_equal<value_type, arg_type>	op_less_equal;
	static const greater_equal<value_type, arg_type>
							op_greater_equal;
private:
	// safe to use naked (never-delete) pointers on static objects
	typedef	qmap<string, const op_type*>	op_map_type;
	typedef	qmap<const op_type*, string>	reverse_op_map_type;
public:
	static const op_map_type		op_map;
private:
	static const reverse_op_map_type	reverse_op_map;
	static const size_t			op_map_size;
	static void op_map_register(const string&, const op_type* );
	static size_t op_map_init(void);

protected:
	operand_ptr_type		lx;
	operand_ptr_type		rx;
	/**
		Points to the operator functor.  
	 */
	const op_type*			op;

private:
	pint_relational_expr();
public:
	pint_relational_expr(const operand_ptr_type& l, const string& o, 
		const operand_ptr_type& r);
	pint_relational_expr(const operand_ptr_type& l, const op_type* o, 
		const operand_ptr_type& r);

	~pint_relational_expr();

	ostream&
	what(ostream& o) const;

	ostream&
	dump_brief(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	size_t
	dimensions(void) const { return 0; }

	bool
	has_static_constant_dimensions(void) const { return true; }

	const_range_list
	static_constant_dimensions(void) const
		{ return const_range_list(); }

	bool
	may_be_initialized(void) const
		{ return lx->may_be_initialized() && rx->may_be_initialized(); }

	bool
	must_be_initialized(void) const {
		return lx->must_be_initialized() && rx->must_be_initialized();
	}

	bool
	is_static_constant(void) const;

	bool
	is_loop_independent(void) const;

	bool
	is_unconditional(void) const;

	value_type
	static_constant_value(void) const;

	bool
	must_be_equivalent(const pbool_expr& ) const;

	good_bool
	resolve_value(value_type& i) const;

	good_bool
	unroll_resolve_value(const unroll_context&, value_type& i) const;

	const_index_list
	resolve_dimensions(void) const;

	good_bool
	resolve_values_into_flat_list(list<value_type>& l) const;

	count_ptr<const_param>
	unroll_resolve(const unroll_context&) const;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class pint_relational_expr

//-----------------------------------------------------------------------------
/**
	Binary logical expression accepts bools and returns a bool.  
 */
class pbool_logical_expr : public pbool_expr {
	typedef	pbool_logical_expr			this_type;
public:
	typedef	pbool_value_type			value_type;
	typedef	pbool_value_type			arg_type;
	typedef	count_ptr<const pbool_expr>		operand_ptr_type;
	typedef	binary_logical_operation<value_type, arg_type>	op_type;
	static const util::logical_and<value_type, arg_type>	op_and;
	static const util::logical_or<value_type, arg_type>	op_or;
	static const util::logical_xor<value_type, arg_type>	op_xor;
private:
	// safe to use naked (never-delete) pointers on static objects
	typedef	qmap<string, const op_type*>	op_map_type;
	typedef	qmap<const op_type*, string>	reverse_op_map_type;
public:
	static const op_map_type		op_map;
private:
	static const reverse_op_map_type	reverse_op_map;
	static const size_t			op_map_size;
	static void op_map_register(const string&, const op_type* );
	static size_t op_map_init(void);

protected:
	count_ptr<const pbool_expr>	lx;
	count_ptr<const pbool_expr>	rx;
/**
	Pointer to the binary logical functor.  
 */
	const op_type*			op;

private:
	pbool_logical_expr();
public:
	pbool_logical_expr(const operand_ptr_type& l, const string& o, 
		const operand_ptr_type& r);
	pbool_logical_expr(const operand_ptr_type& l, const op_type* o, 
		const operand_ptr_type& r);

	~pbool_logical_expr();

	ostream&
	what(ostream& o) const;

	ostream&
	dump_brief(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	size_t
	dimensions(void) const { return 0; }

	bool
	has_static_constant_dimensions(void) const { return true; }

	const_range_list
	static_constant_dimensions(void) const
		{ return const_range_list(); }

	bool
	may_be_initialized(void) const
		{ return lx->may_be_initialized() && rx->may_be_initialized(); }

	bool
	must_be_initialized(void) const {
		return lx->must_be_initialized() && rx->must_be_initialized();
	}

	bool
	is_static_constant(void) const;

	bool
	is_loop_independent(void) const;

	bool
	is_unconditional(void) const;

	value_type
	static_constant_value(void) const;

	bool
	must_be_equivalent(const pbool_expr& ) const;

	good_bool
	resolve_value(value_type& i) const;

	good_bool
	unroll_resolve_value(const unroll_context&, value_type& i) const;

	const_index_list
	resolve_dimensions(void) const;

	good_bool
	resolve_values_into_flat_list(list<value_type>& l) const;

	count_ptr<const_param>
	unroll_resolve(const unroll_context&) const;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class pbool_logical_expr

//=============================================================================
/**
	Range expression object, not necessarily constant.  
	Must contain pint_expr's.
	Derive from object or param_expr?
 */
class pint_range : public meta_range_expr {
	typedef	meta_range_expr				parent_type;
	typedef	pint_range				this_type;
protected:
	// need to be const, or modifiable?
	count_ptr<const pint_expr>	lower;
	count_ptr<const pint_expr>	upper;
private:
	pint_range();
public:
	/** implicit conversion from x[N] to x[0..N-1] */
	explicit
	pint_range(const count_ptr<const pint_expr>& n);

	pint_range(const count_ptr<const pint_expr>& l,
		const count_ptr<const pint_expr>& u);

#if 0
	pint_range(const pint_range& pr);
#endif

	~pint_range();

	// first, second? pair interface?

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	bool
	may_be_initialized(void) const {
		return lower->may_be_initialized() &&
			upper->may_be_initialized();
	}

	bool
	must_be_initialized(void) const {
		return lower->must_be_initialized() &&
			upper->must_be_initialized();
	}

	bool
	is_sane(void) const;

	bool
	is_static_constant(void) const;

	// for now just return false, don't bother checking recursively...
	bool
	is_loop_independent(void) const { return false; }

	bool
	is_unconditional(void) const { return false; }

	const_range
	static_constant_range(void) const;

	good_bool
	resolve_range(const_range& r) const;

	good_bool
	unroll_resolve_range(const unroll_context&, const_range& r) const;

	bool
	must_be_formal_size_equivalent(const meta_range_expr& ) const;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class pint_range

//=============================================================================
}	// end namespace ART
}	// end namespace entity
#else

#include "Object/expr/dynamic_param_expr_list.h"
#include "Object/expr/dynamic_meta_index_list.h"
#include "Object/expr/dynamic_meta_range_list.h"
#include "Object/expr/pint_unary_expr.h"
#include "Object/expr/pbool_unary_expr.h"
#include "Object/expr/pint_arith_expr.h"
#include "Object/expr/pint_relational_expr.h"
#include "Object/expr/pbool_logical_expr.h"
#include "Object/expr/pint_range.h"

#endif

#endif	// __OBJECT_ART_OBJECT_EXPR_H__

