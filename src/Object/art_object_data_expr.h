/**
	\file "Object/art_object_data_expr.h"
	Class definitions for data expressions.
	$Id: art_object_data_expr.h,v 1.1.4.2 2005/06/08 19:13:20 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_DATA_EXPR_H__
#define	__OBJECT_ART_OBJECT_DATA_EXPR_H__

#include "Object/art_object_data_expr_base.h"
#include "Object/art_object_expr_base.h"		// for index_expr
#include <list>
#include "util/qmap.h"
#include "util/operators.h"
#include "util/memory/count_ptr.h"

namespace ART {
namespace entity {
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
	typedef	pint_value_type		 		arg_type;
	typedef	pint_value_type		 		value_type;
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
	// op_type*
private:
	int_arith_expr();
public:
	int_arith_expr(const operand_ptr_type&, const operand_ptr_type&);
	~int_arith_expr();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	size_t
	dimensions(void) const { return 0; }

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class int_arith_expr

//=============================================================================
/**
	In the CHP context, we allow indices to reference both
	int_meta_instance_references and pint_meta_instance_references.  
	Should we allow (param) index expressions where we allow
	data index expressions?
 */
class int_range_expr : public nonmeta_range_expr_base {
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

	PERSISTENT_METHODS_DECLARATIONS
};	// end class int_index

//=============================================================================
/**
	List of indices, which may contain non-meta expressions, 
	that will be run-time evaluated.  
 */
class nonmeta_index_list : public persistent {
	typedef	nonmeta_index_list		this_type;
	typedef	list<count_ptr<nonmeta_index_expr_base> >	list_type;
public:
	typedef	list_type::const_iterator	const_iterator;
private:
	list_type				indices;
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
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_DATA_EXPR_H__

