/**
	\file "art_object_inst_stmt.h"
	Instance statement classes for ART.  
	$Id: art_object_inst_stmt.h,v 1.12.2.5 2005/03/11 07:52:38 fang Exp $
 */

#ifndef	__ART_OBJECT_INST_STMT_H__
#define	__ART_OBJECT_INST_STMT_H__

#include "art_object_inst_stmt_base.h"
#include "memory/count_ptr.h"
#include "memory/list_vector_pool_fwd.h"

// temporary switch
#define	USE_INST_STMT_TEMPLATE		1

namespace ART {
namespace entity {
//=============================================================================
USING_LIST
USING_CONSTRUCT
using namespace util::memory;	// for experimental pointer classes

//=============================================================================
#if USE_INST_STMT_TEMPLATE

#define	INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE			\
template <class Tag>

#define	INSTANTIATION_STATEMENT_CLASS					\
instantiation_statement<Tag>

/**
	Generic instantiation statement template class.  
 */
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
class instantiation_statement :
	public class_traits<Tag>::instantiation_statement_parent_type, 
	public class_traits<Tag>::instantiation_statement_type_ref_base {
private:
	typedef	INSTANTIATION_STATEMENT_CLASS		this_type;
	typedef	typename
		class_traits<Tag>::instantiation_statement_parent_type
							parent_type;
	/**
		The parent from which to inherit a type reference pointer,
		if applicable.
		If this class is empty, then it will be optimized out
		for space (EBCO).  
	 */
	typedef	typename
		class_traits<Tag>::instantiation_statement_type_ref_base
							type_ref_parent_type;
public:
	typedef	typename class_traits<Tag>::instance_collection_generic_type
							collection_type;
	typedef	never_ptr<collection_type>		collection_ptr_type;
	typedef	typename class_traits<Tag>::type_ref_ptr_type
							type_ref_ptr_type;
private:
	never_ptr<collection_type>			inst_base;
private:
	instantiation_statement();
public:
#if 0
	explicit
	instantiation_statement(const index_collection_item_ptr_type&);
#endif

	instantiation_statement(const type_ref_ptr_type& t, 
		const index_collection_item_ptr_type& i);

	~instantiation_statement();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	void
	attach_collection(const never_ptr<instance_collection_base> i);

	never_ptr<instance_collection_base>
	get_inst_base(void);

	never_ptr<const instance_collection_base>
	get_inst_base(void) const;

	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;

	void
	unroll(unroll_context& ) const;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

//	LIST_VECTOR_POOL_ESSENTIAL_FRIENDS
//	LIST_VECTOR_POOL_DEFAULT_STATIC_DECLARATIONS
};	// end class instantiation_statement

#endif	// USE_INST_STMT_TEMPLATE

//=============================================================================
/**
	Abstract base class for built-in parameter type instantiations.  
 */
class param_instantiation_statement : public instantiation_statement_base {
private:
	typedef	instantiation_statement_base		parent_type;
protected:
	param_instantiation_statement() : parent_type() { }
public:
	explicit
	param_instantiation_statement(const index_collection_item_ptr_type& i);

virtual	~param_instantiation_statement() { }

protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;

};	// end class param_instantiation_statement

//-----------------------------------------------------------------------------
#if !USE_INST_STMT_TEMPLATE
/**
	Boolean parameter instantiation statement.
 */
class pbool_instantiation_statement : public param_instantiation_statement {
private:
	typedef	param_instantiation_statement	parent_type;
	typedef	pbool_instantiation_statement	this_type;
public:
	typedef	pbool_instance_collection	collection_type;
	typedef	count_ptr<const param_type_reference>	type_ptr_type;
protected:
	// doesn't have a type_ref, hard-coded to built-ins
	never_ptr<collection_type>		inst_base;
private:
	pbool_instantiation_statement();
public:
	explicit
	pbool_instantiation_statement(const index_collection_item_ptr_type& i);

	~pbool_instantiation_statement();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	void
	attach_collection(const never_ptr<instance_collection_base> i);

	never_ptr<instance_collection_base>
	get_inst_base(void);

	never_ptr<const instance_collection_base>
	get_inst_base(void) const;

	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;

	void
	unroll(unroll_context& ) const;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

	LIST_VECTOR_POOL_ESSENTIAL_FRIENDS
	LIST_VECTOR_POOL_DEFAULT_STATIC_DECLARATIONS
// private:
};	// end class pbool_instantiation_statement

//-----------------------------------------------------------------------------
/**
	Integer parameter instantiation statement.
 */
class pint_instantiation_statement : public param_instantiation_statement {
private:
	typedef	param_instantiation_statement	parent_type;
	typedef	pint_instantiation_statement	this_type;
public:
	typedef	pint_instance_collection	collection_type;
	typedef	count_ptr<const param_type_reference>	type_ptr_type;
protected:
	// doesn't have a type_ref, hard-coded to built-ins
	never_ptr<collection_type>		inst_base;
private:
	pint_instantiation_statement();
public:
	explicit
	pint_instantiation_statement(const index_collection_item_ptr_type& i);

	~pint_instantiation_statement();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	void
	attach_collection(const never_ptr<instance_collection_base> i);

	never_ptr<instance_collection_base>
	get_inst_base(void);

	never_ptr<const instance_collection_base>
	get_inst_base(void) const;

	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;

	void
	unroll(unroll_context& ) const;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

	LIST_VECTOR_POOL_ESSENTIAL_FRIENDS
	LIST_VECTOR_POOL_DEFAULT_STATIC_DECLARATIONS
// private:
};	// end class pint_instantiation_statement
#endif	// USE_INST_STMT_TEMPLATE

//=============================================================================
#if !USE_INST_STMT_TEMPLATE
/**
	Process instantiation statement.
 */
class process_instantiation_statement : public instantiation_statement_base {
private:
	typedef	process_instantiation_statement	this_type;
	typedef	instantiation_statement_base		parent_type;
public:
	typedef	process_instance_collection	collection_type;
	typedef	count_ptr<const process_type_reference>	type_ptr_type;
protected:
	const type_ptr_type			type;
	never_ptr<collection_type>		inst_base;
private:
	process_instantiation_statement();
public:
	process_instantiation_statement(const type_ptr_type& t, 
		const index_collection_item_ptr_type& i);

	~process_instantiation_statement();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	void
	attach_collection(const never_ptr<instance_collection_base> i);

	never_ptr<instance_collection_base>
	get_inst_base(void);

	never_ptr<const instance_collection_base>
	get_inst_base(void) const;

	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;

	void
	unroll(unroll_context& ) const;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

};	// end class process_instantiation_statement

//=============================================================================
/**
	Channel instantiation statement.
 */
class channel_instantiation_statement : public instantiation_statement_base {
private:
	typedef	channel_instantiation_statement	this_type;
	typedef	instantiation_statement_base		parent_type;
public:
	typedef	channel_instance_collection	collection_type;
	typedef	count_ptr<const channel_type_reference>	type_ptr_type;
protected:
	const type_ptr_type			type;
	never_ptr<collection_type>		inst_base;
private:
	channel_instantiation_statement();
public:
	channel_instantiation_statement(const type_ptr_type& t, 
		const index_collection_item_ptr_type& i);

	~channel_instantiation_statement();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	void
	attach_collection(const never_ptr<instance_collection_base> i);

	never_ptr<instance_collection_base>
	get_inst_base(void);

	never_ptr<const instance_collection_base>
	get_inst_base(void) const;

	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

};	// end class channel_instantiation_statement
#endif	// USE_INST_STMT_TEMPLATE

//=============================================================================
#if !USE_INST_STMT_TEMPLATE
/**
	Data-type instantiation statement.
 */
class data_instantiation_statement : public instantiation_statement_base {
private:
	typedef	instantiation_statement_base		parent_type;
	typedef	data_instantiation_statement	this_type;
public:
	typedef	datatype_instance_collection	collection_type;
	typedef	count_ptr<const data_type_reference>	type_ptr_type;
protected:
	/**
		This should really be const, 
		but allocation requires assignability.  
	 */
	type_ptr_type				type;
	never_ptr<collection_type>		inst_base;
private:
	data_instantiation_statement();
public:
	data_instantiation_statement(const type_ptr_type& t, 
		const index_collection_item_ptr_type& i);

	~data_instantiation_statement();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	void
	attach_collection(const never_ptr<instance_collection_base> i);

	never_ptr<instance_collection_base>
	get_inst_base(void);

	never_ptr<const instance_collection_base>
	get_inst_base(void) const;

	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;

#if 1
	// almost ready to unveil...
	void
	unroll(unroll_context& ) const;
#endif

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

public:
	LIST_VECTOR_POOL_ESSENTIAL_FRIENDS
	LIST_VECTOR_POOL_DEFAULT_STATIC_DECLARATIONS
// private:

};	// end class data_instantiation_statement
#endif	// USE_INST_STMT_TEMPLATE

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INST_STMT_H__

