/**
	\file "art_object_connect.h"
	Declarations for classes related to connection of physical
	entites or assignments of parameters.
	\todo Move parameter assignment classes into their own file(s).  
 */

#ifndef	__ART_OBJECT_CONNECT_H__
#define	__ART_OBJECT_CONNECT_H__

#include "art_object_base.h"
#include "count_ptr.h"

/*********** note on use of data structures ***************
Lists are needed for sets that need to maintain sequence, such as
formal declarations in definitions.  Type-checking is done in order
of elements, comparing actuals against formals one-by-one.  
For some lists, however, we'd like constant time access to 
elements in the sequence by hashing indices.  Hashlist provides
this added functionality by associating a key to each element in the 
list.  

Maps...

********************** end note **************************/

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
/**
	The namespace of objects that will be returned by the type-checker, 
	and includes the various hierarchical symbol tables in their 
	respective scopes.  
 */
namespace entity {
//=============================================================================
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
class param_expression_assignment : // public object, 
		public instance_management_base {
public:
	typedef	count_ptr<param_expr>				src_ptr_type;
	typedef	count_const_ptr<param_expr>			src_const_ptr_type;
	typedef	count_ptr<param_instance_reference>		dest_ptr_type;
	typedef	count_const_ptr<param_instance_reference>	dest_const_ptr_type;
#if 0
protected:
	typedef	list<count_const_ptr<param_expr> >	ex_list_type;
	// really should be exclusive pointers
	// param_expr may contain references to parameter instances, ok
protected:
	ex_list_type					ex_list;
#endif

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

// virtualize
#if 0
	void	append_param_expression(count_const_ptr<param_expr> e);
	void	prepend_param_expression(count_const_ptr<param_expr> e);
#else
virtual	bool	append_param_instance_reference(const dest_ptr_type& e) = 0;
#endif

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

//	void unroll(void) const;
#if 0
public:
	ART_OBJECT_IO_METHODS
#endif

//	void unroll_params(void) const;
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
	ART_OBJECT_IO_METHODS

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
	ART_OBJECT_IO_METHODS

};	// end class pint_expression_assignment

//=============================================================================
/**
	Class for saving and managing expression assignments.  
	Includes both static and dynamic instance references.  
 */
class instance_reference_connection : public object, 
		public instance_management_base {
protected:
	typedef	list<count_const_ptr<instance_reference_base> >	inst_list_type;
protected:
	// items may be singular or collective instances references.  
	inst_list_type						inst_list;
public:
	instance_reference_connection();
virtual	~instance_reference_connection() { }

// non-virtual
virtual	void	append_instance_reference(
			count_const_ptr<instance_reference_base> i);
};	// end class instance_reference_connection

//-----------------------------------------------------------------------------
/**
	Alias-style instance connection, e.g. x = y = z;
	List items are interpreted as connecting to each other, 
	and thus having the same type and size.  
	Should be no need for sub-typing?
 */
class aliases_connection : public instance_reference_connection {
protected:
	// no additional fields
public:
	aliases_connection();
	~aliases_connection() { }

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	void	prepend_instance_reference(
			count_const_ptr<instance_reference_base> i);

	void unroll(void) const;

public:
	ART_OBJECT_IO_METHODS
};	// end class aliases_connection

//-----------------------------------------------------------------------------
/**
	Port-style instance connection, e.g. x(y,z,w);
	Which is short-hand for x.first = y, x.second = z, ...;
	List items are interpreted as connecting to the ports
	of the instance.  
	Sub-type into process/data/channel?
 */
class port_connection : public instance_reference_connection {
protected:
	/** should be reference to a simple instance, may be indexed.  */
	count_const_ptr<simple_instance_reference>	inst;
private:
	port_connection();
public:
	/** later, accept complex_aggregate_instance_references? */
	port_connection(count_const_ptr<simple_instance_reference> i);
	~port_connection() { }

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	void	append_instance_reference(
			count_const_ptr<instance_reference_base> i);

	void unroll(void) const;

public:
	ART_OBJECT_IO_METHODS
};	// end class port_connection

//-----------------------------------------------------------------------------
#if 0
NOT READY TO UNVEIL
/**
	Wrapper reference to a loop or conditional namespace.  
 */
class dynamic_connection_assignment : public connection_assignment_base {
protected:
	/** the dynamic scope, a loop or conditional */
	never_const_ptr<scopespace>			dscope;
	// may be really static if bounds and conditions can be 
	// resolved as static constants...
public:
	dynamic_connection_assignment(never_const_ptr<scopespace> s);
	~dynamic_connection_assignment() { }

#if 0
	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
#endif
};	// end class dynamic_connection_assignment
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_CONNECT_H__

