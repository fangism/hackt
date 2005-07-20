/**
	\file "Object/art_object_assign.h"
	Declarations for classes related to connection of 
	assignments of parameters.
	$Id: art_object_assign.h,v 1.21 2005/07/20 20:59:54 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_ASSIGN_H__
#define	__OBJECT_ART_OBJECT_ASSIGN_H__

#include "util/boolean_types.h"
#include "Object/art_object_instance_management_base.h"
#include "Object/traits/class_traits.h"
#include "util/memory/count_ptr.h"
#include "util/memory/list_vector_pool_fwd.h"

namespace ART {
namespace entity {
class simple_param_meta_value_reference;
USING_LIST
USING_CONSTRUCT
using std::ostream;
using util::memory::count_ptr;	// for experimental pointer classes
using util::bad_bool;
using util::good_bool;
class param_expr;
class unroll_context;

//=============================================================================
/**
	Class for saving and managing expression assignments.  
	Includes both static and dynamic expressions.  
	Consider separating, rhs from the rest?
		rhs is any param_expr, while the rest are 
		meta_instance_references, may eventually be complex-aggregate.
	Consider sub-typing into pint and pbool assignments, 
		since types are static.  
 */
class param_expression_assignment : public instance_management_base {
public:
	typedef	count_ptr<param_expr>				src_ptr_type;
	typedef	count_ptr<const param_expr>			src_const_ptr_type;
	typedef	count_ptr<simple_param_meta_value_reference>		dest_ptr_type;
	typedef	count_ptr<const simple_param_meta_value_reference>	dest_const_ptr_type;

// protected:
//	/** cached value for dimensions, computed on construction */
//	size_t			dimension;

public:
	param_expression_assignment();
virtual	~param_expression_assignment();

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o) const = 0;

virtual	size_t
	size(void) const = 0;

virtual	bad_bool
	append_simple_param_meta_value_reference(const dest_ptr_type& e) = 0;

virtual	UNROLL_META_EVALUATE_PROTO = 0;

	/**
		Helper class for appending instance references to
		a parameter assignment list.  
		Written as a binary operator to accumulate error conditions.  
		Used by object_list::make_param_expression_assignment.
	 */
	class meta_instance_reference_appender {
		// used to be object_list::value_type
		typedef	count_ptr<param_expr>	arg_type;
	protected:
		size_t				index;
		param_expression_assignment&	ex_ass;
	public:
		explicit
		meta_instance_reference_appender(param_expression_assignment& p) :
			index(0), ex_ass(p) { }

		bad_bool
		operator () (const bad_bool b, const arg_type& i);
	};	// end class meta_instance_reference_appender

protected:
	good_bool
	validate_dimensions_match(const dest_const_ptr_type&, 
		const size_t ) const;

	good_bool
	validate_reference_is_uninitialized(const dest_const_ptr_type&) const;

};	// end class param_expression_assignment

//-----------------------------------------------------------------------------
#define	EXPRESSION_ASSIGNMENT_TEMPLATE_SIGNATURE			\
template <class Tag>

#define	EXPRESSION_ASSIGNMENT_CLASS					\
expression_assignment<Tag>

EXPRESSION_ASSIGNMENT_TEMPLATE_SIGNATURE
class expression_assignment :
	public class_traits<Tag>::expression_assignment_parent_type {
private:
	typedef	EXPRESSION_ASSIGNMENT_CLASS		this_type;
public:
	typedef	typename class_traits<Tag>::expression_assignment_parent_type
							parent_type;
	typedef	typename class_traits<Tag>::simple_meta_instance_reference_type
							value_reference_type;
	typedef	typename class_traits<Tag>::expr_base_type
							expr_type;
	typedef	typename class_traits<Tag>::const_expr_type
							const_expr_type;
	typedef	typename class_traits<Tag>::const_collection_type
							const_collection_type;
	typedef	count_ptr<value_reference_type>	dest_ptr_type;
	typedef	count_ptr<const value_reference_type>	dest_const_ptr_type;
	typedef	list<dest_const_ptr_type>		dest_list_type;
	typedef	count_ptr<expr_type>			src_ptr_type;
	typedef	count_ptr<const expr_type>		src_const_ptr_type;
protected:
	/** right-hand-side expression */
	src_const_ptr_type				src;
	/** left-hand-side destinations, where to assign expr. */
	dest_list_type					dests;
private:
	expression_assignment();
public:
	explicit
	expression_assignment(const src_const_ptr_type& s);

	~expression_assignment();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	size_t
	size(void) const;

	bad_bool
	append_simple_param_meta_value_reference(
		const typename parent_type::dest_ptr_type& e);

	good_bool
	unroll(unroll_context& ) const;

	UNROLL_META_EVALUATE_PROTO;

public:
	/** helper class for printing dump of list */
	class dumper {
	private:
		size_t index;
		ostream& os;
	public:
		explicit
		dumper(ostream& o, const size_t i = 0);

		void
		operator () (const typename dest_list_type::value_type& i);
	};	// end class dumper

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

	LIST_VECTOR_POOL_ESSENTIAL_FRIENDS
	LIST_VECTOR_POOL_DEFAULT_STATIC_DECLARATIONS

private:
	typedef	typename dest_list_type::const_iterator	const_dest_iterator;

	static
	good_bool
	assign_dests(const_dest_iterator, const const_dest_iterator&, 
		const const_collection_type&);
	
};	// end cllass expression_assignment

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_ASSIGN_H__

