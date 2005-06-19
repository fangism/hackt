/**
	\file "Object/art_object_expr_base.h"
	Base classes related to program expressions, symbolic and parameters.  
	TODO: rename to meta_expr_base.h
	$Id: art_object_expr_base.h,v 1.14 2005/06/19 01:58:39 fang Exp $
 */

#ifndef __OBJECT_ART_OBJECT_EXPR_BASE_H__
#define __OBJECT_ART_OBJECT_EXPR_BASE_H__

#include "util/STL/list_fwd.h"
#include "Object/art_object_data_expr_base.h"
#include "Object/art_object_expr_types.h"
#include "util/boolean_types.h"

//=============================================================================
namespace ART {
namespace entity {
class const_range_list;
class const_param;
class param_expression_assignment;
class const_range;
class const_index;
class const_index_list;
class const_param_expr_list;
class unroll_context;
USING_LIST
using util::good_bool;
using util::bad_bool;
using util::memory::excl_ptr;

//=============================================================================
/**
	Abstract base class for symbolic expressions to be written 
	to an object file.  
	Expression graphs must be directed-acyclic.  
	Deriving from object, so that it may be cached in 
	scopespaces' used_id_map.  
	Should statically sub-type into pints and pbools and pranges...
 */
class param_expr : virtual public persistent {
public:
	param_expr() : persistent() { }

virtual	~param_expr() { }

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o) const = 0;

virtual	size_t
	dimensions(void) const = 0;

virtual	bool
	has_static_constant_dimensions(void) const = 0;

// only call this if dimensions are non-zero and sizes are static constant.  
virtual	const_range_list
	static_constant_dimensions(void) const = 0;

/** is initialized if is resolved to constant or some other formal */
virtual bool
	may_be_initialized(void) const = 0;

virtual bool
	must_be_initialized(void) const = 0;

virtual bool
	may_be_equivalent_generic(const param_expr& p) const = 0;

virtual bool
	must_be_equivalent_generic(const param_expr& p) const = 0;

/** can be resolved to static constant value */
virtual bool
	is_static_constant(void) const = 0;

virtual	count_ptr<const const_param>
	static_constant_param(void) const = 0;

/** doesn't depend on loop variables */
virtual bool
	is_loop_independent(void) const = 0;

/** doesn't depend on conditional variables */
virtual bool
	is_unconditional(void) const = 0;

	static
	excl_ptr<param_expression_assignment>
	make_param_expression_assignment(const count_ptr<const param_expr>& p);

virtual	count_ptr<const_param>
	unroll_resolve(const unroll_context&) const = 0;

private:
virtual	excl_ptr<param_expression_assignment>
	make_param_expression_assignment_private(
		const count_ptr<const param_expr>& p) const = 0;
};	// end class param_expr

//-----------------------------------------------------------------------------
/**
	A list of parameter expressions.  
	Consider splitting into dynamic vs. const?
 */
class param_expr_list : public persistent {
public:
	param_expr_list() : persistent() { }

virtual	~param_expr_list() { }

virtual	size_t
	size(void) const = 0;

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o) const = 0;

virtual	excl_ptr<param_expr_list>
	make_copy(void) const = 0;

virtual	bool
	may_be_initialized(void) const = 0;

virtual	bool
	must_be_initialized(void) const = 0;

virtual	bool
	may_be_equivalent(const param_expr_list& p) const = 0;

virtual	bool
	must_be_equivalent(const param_expr_list& p) const = 0;

virtual	bool
	is_static_constant(void) const = 0;

virtual	bool
	is_loop_independent(void) const = 0;

virtual	excl_ptr<const_param_expr_list>
	unroll_resolve(const unroll_context&) const = 0;

};	// end class param_expr_list

//-----------------------------------------------------------------------------
/**
	Abstract interface to an index (not a range), which may be 
	a single integer interpreted as a dimension-collapse x[i], 
	or a range that preserves the dimension x[i..j].  
	For example, x[i] refers to a single instance (0-dimension)
	of the x array, whereas x[i..i] refers to a 
	1-dimensional sub-array of x, size 1, in this case.  
	A "meta_range_expr" (below) may be an index, but not vice versa.  
 */
class meta_index_expr : virtual public nonmeta_index_expr_base {
	typedef	nonmeta_index_expr_base		parent_type;
	typedef	meta_index_expr			this_type;
protected:
	meta_index_expr() : parent_type() { }

public:
virtual	~meta_index_expr() { }

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o) const = 0;

virtual size_t
	dimensions(void) const = 0;

virtual bool
	may_be_initialized(void) const = 0;

virtual bool
	must_be_initialized(void) const = 0;

virtual bool
	is_static_constant(void) const = 0;

virtual bool
	is_loop_independent(void) const = 0;

virtual bool
	is_unconditional(void) const = 0;

virtual	count_ptr<const_index>
	resolve_index(void) const = 0;

virtual	count_ptr<const_index>
	unroll_resolve_index(const unroll_context&) const = 0;

virtual	bool
	must_be_equivalent_index(const meta_index_expr& ) const = 0;

// additional virtual functions for dimensionality...
};	// end class meta_index_expr

//-----------------------------------------------------------------------------
/**
	May contained mixed pint_expr and ranges!
	Don't forget their interpretation differs!
	pint_expr is interpreted as a dimension collapse, 
	whereas meta_range_expr preserves dimension, even if range is one.  

	Doesn't make sense to ask how many dimensions are in an index_list
	because it depends on the meta_instance_reference to which it is 
	attached.  
	Instead the index list can tell one how may dimensions
	are *collapsed* by the element types.  
 */
class meta_index_list : public persistent {
public:
	meta_index_list() : persistent() { }

virtual	~meta_index_list() { }

// copy over most param_expr interface functions...
virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o) const = 0;

/** NOT THE SAME **/
virtual	size_t
	size(void) const = 0;

virtual	size_t
	dimensions_collapsed(void) const = 0;

virtual	bool
	may_be_initialized(void) const = 0;

virtual	bool
	must_be_initialized(void) const = 0;

virtual	bool
	is_static_constant(void) const = 0;

virtual	bool
	is_loop_independent(void) const = 0;

virtual	bool
	is_unconditional(void) const = 0;

virtual	const_index_list
	resolve_index_list(void) const = 0;

virtual	const_index_list
	unroll_resolve(const unroll_context&) const = 0;

#if 0
virtual	bool
	resolve_multikey(excl_ptr<multikey_index_type>& k) const = 0;
#endif

virtual	bool
	must_be_equivalent_indices(const meta_index_list& ) const = 0;
};	// end class meta_index_list

//=============================================================================
/**
	General interface to multidimensional indices.  
	Make interface like std::list.  
	Instance collection stack item?
	Replace instance_collection_stack_item with this!
	Elements of meta_range_list must be meta_range_expr, 
	i.e. fully expanded, no shorthand ranges.  
 */
class meta_range_list : public persistent {
protected:
public:
	meta_range_list() : persistent() { }

virtual	~meta_range_list() { }

virtual	size_t
	size(void) const = 0;

virtual	ostream&
	dump(ostream&) const = 0;

	size_t
	dimensions(void) const { return size(); }

virtual	bool
	is_static_constant(void) const = 0;

virtual	const_range_list
	static_overlap(const meta_range_list& r) const = 0;

// is this obsolete?
virtual	good_bool
	resolve_ranges(const_range_list& r) const = 0;

virtual	good_bool
	unroll_resolve(const_range_list&, const unroll_context&) const = 0;

virtual	bool
	must_be_formal_size_equivalent(const meta_range_list& ) const = 0;

};	// end class meta_range_list

//=============================================================================
/**
	Abstract expression checked to be a single boolean.  
 */
class pbool_expr : virtual public param_expr, public bool_expr {
public:
	/**
		The global boolean value type, set in "art_object_fwd.h".
		We bother with this typedef for the future potential of 
		using templates to extend to other parameter types.  
	 */
	typedef	pbool_value_type		value_type;
public:
	pbool_expr() : param_expr(), bool_expr() { }

	// temporary de-inline for debugging
virtual	~pbool_expr();

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump_brief(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o) const = 0;

virtual	size_t
	dimensions(void) const = 0;

	GET_DATA_TYPE_REF_PROTO;

virtual	bool
	has_static_constant_dimensions(void) const = 0;

virtual	const_range_list
	static_constant_dimensions(void) const = 0;

virtual bool
	may_be_initialized(void) const = 0;

virtual bool
	must_be_initialized(void) const = 0;

	bool
	may_be_equivalent_generic(const param_expr& p) const;

	bool
	must_be_equivalent_generic(const param_expr& p) const;

virtual	bool
	must_be_equivalent(const pbool_expr& ) const = 0;

virtual bool
	is_static_constant(void) const = 0;

virtual	count_ptr<const const_param>
	static_constant_param(void) const;

virtual bool
	is_loop_independent(void) const = 0;

virtual value_type
	static_constant_value(void) const = 0;

virtual	good_bool
	resolve_value(value_type& i) const = 0;

virtual	const_index_list
	resolve_dimensions(void) const = 0;

virtual	good_bool
	resolve_values_into_flat_list(list<value_type>& l) const = 0;

protected:
	excl_ptr<param_expression_assignment>
	make_param_expression_assignment_private(
		const count_ptr<const param_expr>& p) const;
};	// end class pbool_expr

//-----------------------------------------------------------------------------
/**
	Abstract expression checked to be a single integer.  
 */
class pint_expr : virtual public param_expr, virtual public meta_index_expr, 
		public int_expr {
public:
	/**
		The internal storage type, set in "art_object_fwd.h".
		We bother with this typedef for the future potential of 
		using templates to extend to other parameter types.  
	 */
	typedef	pint_value_type			value_type;
protected:
	pint_expr() : param_expr(), meta_index_expr(), int_expr() { }

public:
	// temporary de-inline for debugging purposes
virtual	~pint_expr();

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump_brief(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o) const = 0;

virtual	size_t
	dimensions(void) const = 0;

	GET_DATA_TYPE_REF_PROTO;

virtual	bool
	has_static_constant_dimensions(void) const = 0;

virtual	const_range_list
	static_constant_dimensions(void) const = 0;

virtual bool
	may_be_initialized(void) const = 0;

virtual bool
	must_be_initialized(void) const = 0;

	bool
	may_be_equivalent_generic(const param_expr& p) const;

	bool
	must_be_equivalent_generic(const param_expr& p) const;

virtual	bool
	must_be_equivalent(const pint_expr& ) const = 0;

	bool
	must_be_equivalent_index(const meta_index_expr& ) const;

virtual bool
	is_static_constant(void) const = 0;

virtual	count_ptr<const const_param>
	static_constant_param(void) const;

virtual bool
	is_unconditional(void) const = 0;

virtual bool
	is_loop_independent(void) const = 0;

virtual value_type
	static_constant_value(void) const = 0;

	count_ptr<const_index>
	resolve_index(void) const;

	count_ptr<const_index>
	unroll_resolve_index(const unroll_context&) const;

virtual	good_bool
	resolve_value(value_type& i) const = 0;

virtual	good_bool
	unroll_resolve_value(const unroll_context&, value_type& i) const = 0;

virtual	const_index_list
	resolve_dimensions(void) const = 0;

virtual	good_bool
	resolve_values_into_flat_list(list<value_type>& l) const = 0;

protected:
	excl_ptr<param_expression_assignment>
	make_param_expression_assignment_private(
		const count_ptr<const param_expr>& p) const;
};	// end class pint_expr

//=============================================================================
/**
	Abstract interface to range expression objects.  
	Note: meta_range_expr is not a param_expr.
	Range expressions are not the same as index expressions:
	Range expressions are used exclusively for declarations of arrays, 
	whereas index expressions are used for instance references.  
	Since the syntax allows declaration of x[N] as a short-hand for 
	x[0..N-1], we will need to explicitly convert from the former 
	to the latter.  
 */
class meta_range_expr : virtual public meta_index_expr, 
		public nonmeta_range_expr_base {
	typedef	meta_index_expr		parent_type;
	typedef	nonmeta_range_expr_base	nonmeta_parent_type;
protected:
	meta_range_expr() : parent_type(), nonmeta_parent_type() { }

public:
virtual	~meta_range_expr() { }

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o) const = 0;

/**
	A range is always 0-dimensional, which is not the same
	as the dimensions *represented* by the range.  
	Is virtual so that sub-classes that use the index interface can
	override this.  
 */
virtual	size_t
	dimensions(void) const { return 0; }

/** is initialized if is resolved to constant or some other formal */
virtual bool
	may_be_initialized(void) const = 0;

virtual bool
	must_be_initialized(void) const = 0;

/** is sane if range makes sense */
virtual	bool
	is_sane(void) const = 0;

/** can be resolved to static constant value */
virtual bool
	is_static_constant(void) const = 0;

/** doesn't depend on loop variables */
virtual bool
	is_loop_independent(void) const = 0;

/** doesn't depend on conditional variables */
virtual bool
	is_unconditional(void) const = 0;

	count_ptr<const_index>
	resolve_index(void) const;

	count_ptr<const_index>
	unroll_resolve_index(const unroll_context&) const;

virtual	good_bool
	resolve_range(const_range& r) const = 0;

virtual	good_bool
	unroll_resolve_range(const unroll_context&, const_range& r) const = 0;

virtual	bool
	must_be_formal_size_equivalent(const meta_range_expr& ) const = 0;

	bool
	must_be_equivalent_index(const meta_index_expr& ) const;

};	// end class meta_range_expr

//=============================================================================
}	// end namespace ART
}	// end namespace entity

#endif	// __OBJECT_ART_OBJECT_EXPR_BASE_H__

