/**
	\file "art_object_inst_stmt.h"
	Instance statement classes for ART.  
	$Id: art_object_inst_stmt.h,v 1.8.4.1.4.1 2005/01/24 22:28:39 fang Exp $
 */

#ifndef	__ART_OBJECT_INST_STMT_H__
#define	__ART_OBJECT_INST_STMT_H__

#include "art_object_instance_management_base.h"
#include "art_object_inst_stmt_base.h"
#include "memory/count_ptr.h"
#include "memory/list_vector_pool_fwd.h"

namespace ART {
namespace entity {
//=============================================================================
USING_LIST
using namespace util::memory;	// for experimental pointer classes
using namespace MULTIKEY_NAMESPACE;
using namespace MULTIKEY_MAP_NAMESPACE;

//=============================================================================
/**
	Abstract base class for built-in parameter type instantiations.  
 */
class param_instantiation_statement : public instantiation_statement {
private:
	typedef	instantiation_statement		parent_type;
protected:
	param_instantiation_statement() : instantiation_statement() { }
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
/**
	Boolean parameter instantiation statement.
 */
class pbool_instantiation_statement : public object, 
		public param_instantiation_statement {
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
	unroll(void) const;

public:
	PERSISTENT_METHODS

	friend class list_vector_pool<this_type>;
	LIST_VECTOR_POOL_STATIC_DECLARATIONS
// private:
};	// end class pbool_instantiation_statement

//-----------------------------------------------------------------------------
/**
	Integer parameter instantiation statement.
 */
class pint_instantiation_statement : public object, 
		public param_instantiation_statement {
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
	unroll(void) const;

public:
	PERSISTENT_METHODS

	friend class list_vector_pool<this_type>;
	LIST_VECTOR_POOL_STATIC_DECLARATIONS
// private:
};	// end class pint_instantiation_statement

//=============================================================================
/**
	Process instantiation statement.
 */
class process_instantiation_statement : public object, 
		public instantiation_statement {
private:
	typedef	instantiation_statement		parent_type;
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

public:
	PERSISTENT_METHODS

};	// end class process_instantiation_statement

//=============================================================================
/**
	Channel instantiation statement.
 */
class channel_instantiation_statement : public object, 
		public instantiation_statement {
private:
	typedef	instantiation_statement		parent_type;
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
	PERSISTENT_METHODS

};	// end class channel_instantiation_statement

//=============================================================================
/**
	Data-type instantiation statement.
 */
class data_instantiation_statement : public object, 
		public instantiation_statement {
private:
	typedef	instantiation_statement		parent_type;
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
	unroll(void) const;
#endif

public:
	PERSISTENT_METHODS


public:
	friend class list_vector_pool<this_type>;
	LIST_VECTOR_POOL_STATIC_DECLARATIONS
// private:

};	// end class data_instantiation_statement

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INST_STMT_H__

