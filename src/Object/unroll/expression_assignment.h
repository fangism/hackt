/**
	\file "Object/unroll/expression_assignment.h"
	Declarations for classes related to connection of 
	assignments of parameters.
	This file came from "Object/art_object_assign.h" in a previous life.  
	$Id: expression_assignment.h,v 1.3.8.1 2005/10/11 02:41:28 fang Exp $
 */

#ifndef	__OBJECT_UNROLL_EXPRESSION_ASSIGNMENT_H__
#define	__OBJECT_UNROLL_EXPRESSION_ASSIGNMENT_H__

#include <list>
#include "Object/unroll/param_expression_assignment.h"
#include "Object/traits/class_traits.h"
#include "util/memory/list_vector_pool_fwd.h"

namespace ART {
namespace entity {
using std::list;
USING_CONSTRUCT

//=============================================================================
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
	unroll(const unroll_context&) const;

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
		const const_collection_type&, const unroll_context&);
	
};	// end class expression_assignment

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_UNROLL_EXPRESSION_ASSIGNMENT_H__

