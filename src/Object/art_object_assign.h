/**
	\file "art_object_assign.h"
	Declarations for classes related to connection of 
	assignments of parameters.
	$Id: art_object_assign.h,v 1.2 2004/11/02 07:51:44 fang Exp $
 */

#ifndef	__ART_OBJECT_ASSIGN_H__
#define	__ART_OBJECT_ASSIGN_H__

#include "art_object_base.h"
#include "count_ptr.h"

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
namespace entity {
	using namespace std;
	using namespace PTRS_NAMESPACE;	// for experimental pointer classes

//=============================================================================
/**
	Class for saving and managing expression assignments.  
	Includes both static and dynamic expressions.  
	Consider separating, rhs from the rest?
		rhs is any param_expr, while the rest are 
		instance_references, may eventually be complex-aggregate.
	Consider sub-typing into pint and pbool assignments, 
		since types are static.  
 */
class param_expression_assignment : public instance_management_base {
public:
	typedef	count_ptr<param_expr>				src_ptr_type;
	typedef	count_const_ptr<param_expr>			src_const_ptr_type;
	typedef	count_ptr<param_instance_reference>		dest_ptr_type;
	typedef	count_const_ptr<param_instance_reference>	dest_const_ptr_type;

// protected:
//	/** cached value for dimensions, computed on construction */
//	size_t			dimension;

public:
	param_expression_assignment();
virtual	~param_expression_assignment();

// virtualize
virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const = 0;
virtual	size_t	size(void) const = 0;

virtual	bool	append_param_instance_reference(const dest_ptr_type& e) = 0;

	/**
		Helper class for appending instance references to
		a parameter assignment list.  
		Written as a binary operator to accumulate error conditions.  
		Used by object_list::make_param_expression_assignment.
	 */
	class instance_reference_appender {
	protected:
		size_t				index;
		param_expression_assignment&	ex_ass;
	public:
		instance_reference_appender(param_expression_assignment& p) :
			index(0), ex_ass(p) { }
		bool
		operator () (const bool b, const object_list::value_type& i);
	};	// end class instance_reference_appender

protected:
	bool	validate_dimensions_match(
			const dest_const_ptr_type&, const size_t ) const;
	bool	validate_reference_is_uninitialized(
			const dest_const_ptr_type&) const;

};	// end class param_expression_assignment

//-----------------------------------------------------------------------------
/**
	pbool-specific version of expression assignments.  
 */
class pbool_expression_assignment : public param_expression_assignment, 
		public object {
public:
	typedef	param_expression_assignment		parent_type;
	typedef	count_ptr<pbool_instance_reference>	dest_ptr_type;
	typedef	count_const_ptr<pbool_instance_reference>
							dest_const_ptr_type;
	typedef	list<dest_const_ptr_type>		dest_list_type;
	typedef	count_ptr<pbool_expr>			src_ptr_type;
	typedef	count_const_ptr<pbool_expr>		src_const_ptr_type;
protected:
	/** right-hand-side expression */
	src_const_ptr_type				src;
	/** left-hand-side destinations, where to assign expr. */
	dest_list_type					dests;
private:
	pbool_expression_assignment();
public:
	pbool_expression_assignment(const src_const_ptr_type& s);
	~pbool_expression_assignment();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	size_t	size(void) const;

	bool	append_param_instance_reference(
			const parent_type::dest_ptr_type& e);

	void unroll(void) const;

public:
	/** helper class for printing dump of list */
	class dumper {
	private:
		size_t index;
		ostream& os;
	public:
		dumper(ostream& o, const size_t i = 0);
		void operator () (const dest_list_type::value_type& i);
	};	// end class dumper

public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS

};	// end class pbool_expression_assignment

//-----------------------------------------------------------------------------
/**
	pint-specific version of expression assignments.  
 */
class pint_expression_assignment : public param_expression_assignment, 
		public object {
public:
	typedef	param_expression_assignment		parent_type;
	typedef	count_ptr<pint_instance_reference>	dest_ptr_type;
	typedef	count_const_ptr<pint_instance_reference>
							dest_const_ptr_type;
	typedef	list<dest_const_ptr_type>		dest_list_type;
	typedef	count_ptr<pint_expr>			src_ptr_type;
	typedef	count_const_ptr<pint_expr>		src_const_ptr_type;
protected:
	/** right-hand-side expression */
	src_const_ptr_type				src;
	/** left-hand-side destinations, where to assign expr. */
	dest_list_type					dests;
private:
	pint_expression_assignment();
public:
	pint_expression_assignment(const src_const_ptr_type& s);
	~pint_expression_assignment();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	size_t	size(void) const;

	bool	append_param_instance_reference(
			const parent_type::dest_ptr_type& e);

	void unroll(void) const;

public:
	/** helper class for printing dump of list */
	class dumper {
	private:
		size_t index;
		ostream& os;
	public:
		dumper(ostream& o, const size_t i = 0);
		void operator () (const dest_list_type::value_type& i);
	};	// end class dumper

public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS

};	// end class pint_expression_assignment

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_ASSIGN_H__

