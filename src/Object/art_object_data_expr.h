/**
	\file "Object/art_object_data_expr.h"
	Class definitions for data expressions.
	$Id: art_object_data_expr.h,v 1.2.4.1 2005/07/05 01:16:22 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_DATA_EXPR_H__
#define	__OBJECT_ART_OBJECT_DATA_EXPR_H__

#if 0
#include "Object/art_object_data_expr_base.h"
#include "Object/art_object_expr_base.h"		// for index_expr
#include <list>
#include <string>
#include "util/qmap.h"
#include "util/operators.h"
#include "util/memory/count_ptr.h"

namespace ART {
namespace entity {
using std::string;
using std::list;
using util::qmap;
USING_UTIL_OPERATIONS
using util::persistent_object_manager;
//=============================================================================
/**
	Arithmetic operations on int data types.  
	Want to rip off operator map (or everything) from arith_expr...
 */
class int_arith_expr : public int_expr {
	typedef	int_arith_expr				this_type;
	typedef	int_expr				parent_type;
public:
	/**
		For arbitrary precision integers, will want a type
		fom GMP, such as mpz.  
	 */
	typedef	int_value_type		 		arg_type;
	typedef	int_value_type		 		value_type;
	typedef	binary_arithmetic_operation<value_type, arg_type>
							op_type;
        static const plus<value_type, arg_type>		adder;
        static const minus<value_type, arg_type>	subtractor;
        static const multiplies<value_type, arg_type>	multiplier;
        static const divides<value_type, arg_type>	divider;
        static const modulus<value_type, arg_type>	remainder;
private:
	typedef	qmap<char, const op_type*>		op_map_type;
	typedef	qmap<const op_type*, char>		reverse_op_map_type;
	static const op_map_type			op_map;
	static const reverse_op_map_type		reverse_op_map;
	static const size_t				op_map_size;
	static void op_map_register(const char, const op_type*);
	static size_t op_map_init(void);
public:
	typedef	count_ptr<const int_expr>		operand_ptr_type;
protected:
	operand_ptr_type				lx;
	operand_ptr_type				rx;
	const op_type*					op;
private:
	int_arith_expr();
public:
	int_arith_expr(const operand_ptr_type&, const char, 
		const operand_ptr_type&);
	~int_arith_expr();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	ostream&
	dump_brief(ostream&) const;

	size_t
	dimensions(void) const { return 0; }

	GET_DATA_TYPE_REF_PROTO;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class int_arith_expr

//=============================================================================
/**
	Integer arithmetic on integer data types.  
	(Non-meta language component)
 */
class int_relational_expr : public bool_expr {
	typedef	int_relational_expr		this_type;
public:
	typedef	bool_value_type			value_type;
	typedef	int_value_type			arg_type;
	typedef count_ptr<const int_expr>	operand_ptr_type;
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
	operand_ptr_type			lx;
	operand_ptr_type			rx;
	/**
		Points to the operator functor.  
	 */
	const op_type*                  op;

private:
	int_relational_expr();
public:
	int_relational_expr(const operand_ptr_type&, const string& o,
		const operand_ptr_type&);
	int_relational_expr(const operand_ptr_type& l, const op_type* o,
		const operand_ptr_type& r);

	~int_relational_expr();

	ostream&
	what(ostream& o) const;

	ostream&
	dump_brief(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	size_t
	dimensions(void) const { return 0; }

	GET_DATA_TYPE_REF_PROTO;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};      // end class int_relational_expr

//=============================================================================
/**
	Logical expressions on boolean data types.  
 */
class bool_logical_expr : public bool_expr {
	typedef	bool_logical_expr			this_type;
public:
	typedef bool_value_type				value_type;
	typedef	bool_value_type				arg_type;
	typedef	count_ptr<const bool_expr>		operand_ptr_type;
	typedef	binary_logical_operation<value_type, arg_type>	op_type;
	static const util::logical_and<value_type, arg_type>	op_and;
	static const util::logical_or<value_type, arg_type>	op_or;
	static const util::logical_xor<value_type, arg_type>	op_xor;
private:
	// safe to use naked (never-delete) pointers on static objects
	typedef qmap<string, const op_type*>	op_map_type;
	typedef qmap<const op_type*, string>	reverse_op_map_type;
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
	Pointer to the binary logical functor.  
 */
	const op_type*                  op;

private:
	bool_logical_expr();
public:
	bool_logical_expr(const operand_ptr_type& l, const string& o,
		const operand_ptr_type& r);
	bool_logical_expr(const operand_ptr_type& l, const op_type* o,
		const operand_ptr_type& r);

	~bool_logical_expr();

	ostream&
	what(ostream& o) const;

	ostream&
	dump_brief(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	size_t
	dimensions(void) const { return 0; }

	GET_DATA_TYPE_REF_PROTO;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};      // end class bool_logical_expr

//=============================================================================
/**
	Signed integer data negation.  
 */
class int_negation_expr : public int_expr {
	typedef	int_negation_expr		this_type;
	typedef	int_expr			parent_type;
public:
	typedef	count_ptr<const int_expr>	operand_ptr_type;
private:
	operand_ptr_type		ex;
private:
	int_negation_expr();
public:
	explicit
	int_negation_expr(const operand_ptr_type&);

	~int_negation_expr();

	ostream&
	what(ostream& o) const;

	ostream&
	dump_brief(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	size_t
	dimensions(void) const { return 0; }

	GET_DATA_TYPE_REF_PROTO;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class int_negation_expr

//=============================================================================
// what about ! int -> bool ?
//=============================================================================
/**
	Boolean data negation.  
 */
class bool_negation_expr : public bool_expr {
	typedef	bool_negation_expr		this_type;
	typedef	bool_expr			parent_type;
public:
	typedef	count_ptr<const bool_expr>	operand_ptr_type;
private:
	operand_ptr_type		ex;
private:
	bool_negation_expr();
public:
	explicit
	bool_negation_expr(const operand_ptr_type&);

	~bool_negation_expr();

	ostream&
	what(ostream& o) const;

	ostream&
	dump_brief(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	size_t
	dimensions(void) const { return 0; }

	GET_DATA_TYPE_REF_PROTO;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class bool_negation_expr

//=============================================================================
/**
	In the CHP context, we allow indices to reference both
	int_meta_instance_references and pint_meta_instance_references.  
	Should we allow (param) index expressions where we allow
	data index expressions?
 */
class int_range_expr : public nonmeta_range_expr_base {
	typedef	int_range_expr			this_type;
	typedef	nonmeta_range_expr_base		parent_type;
public:
	typedef	count_ptr<const int_expr>	bound_ptr_type;
protected:
	bound_ptr_type				lower;
	bound_ptr_type				upper;
public:
	int_range_expr();
	int_range_expr(const bound_ptr_type&, const bound_ptr_type&);
	~int_range_expr();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	size_t
	dimensions(void) const { return 0; }	// or bomb

	GET_DATA_TYPE_REF_PROTO;

	PERSISTENT_METHODS_DECLARATIONS
};	// end class int_index

//=============================================================================
/**
	List of indices, which may contain non-meta expressions, 
	that will be run-time evaluated.  
 */
class nonmeta_index_list : public persistent, 
		public list<count_ptr<const nonmeta_index_expr_base> > {
	typedef	nonmeta_index_list		this_type;
	typedef	list<count_ptr<const nonmeta_index_expr_base> >	list_type;
public:
	nonmeta_index_list();
	~nonmeta_index_list();

	size_t
	dimensions_collapsed(void) const;

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	PERSISTENT_METHODS_DECLARATIONS
};	// end class nonmeta_index_list

//=============================================================================
/**
	List of indices, which may contain non-meta expressions, 
	that will be run-time evaluated.  
 */
class int_range_list : public persistent, 
		public list<count_ptr<const int_range_expr> > {
	typedef	int_range_list			this_type;
	typedef	list<count_ptr<const int_range_expr> >	list_type;
public:
	int_range_list();
	~int_range_list();

	size_t
	dimensions_collapsed(void) const;

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	PERSISTENT_METHODS_DECLARATIONS
};	// end class nonmeta_index_list

//=============================================================================
}	// end namespace entity
}	// end namespace ART
#else

#include "Object/expr/int_arith_expr.h"
#include "Object/expr/int_relational_expr.h"
#include "Object/expr/bool_logical_expr.h"
#include "Object/expr/int_negation_expr.h"
#include "Object/expr/bool_negation_expr.h"
#include "Object/expr/int_range_expr.h"
#include "Object/expr/nonmeta_index_list.h"
#include "Object/expr/int_range_list.h"

#endif

#endif	// __OBJECT_ART_OBJECT_DATA_EXPR_H__

