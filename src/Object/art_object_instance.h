/**
	\file "art_object_instance.h"
	Instance collection classes for ART.  
	$Id: art_object_instance.h,v 1.34.2.5.2.3 2005/02/22 03:00:56 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_H__
#define	__ART_OBJECT_INSTANCE_H__

#include "art_object_instance_base.h"
#include "art_object_index.h"
#include "memory/pointer_classes.h"
#include "packed_array_fwd.h"


namespace ART {
namespace entity {
//=============================================================================
USING_LIST
using namespace util::memory;	// for experimental pointer classes
using util::packed_array_generic;

//=============================================================================
// class instance_collection_base declared in "art_object_instance_base.h"

//=============================================================================
/**
	Process instantiation.  
	Type information is now in the instance_collection_list.
 */
class process_instance_collection : public instance_collection_base {
private:
	typedef	process_instance_collection	this_type;
	typedef	instance_collection_base	parent_type;
public:
	typedef	proc_instance_alias		instance_alias_type;
	typedef	process_alias_connection	alias_connection_type;
	typedef never_ptr<instance_alias_type>	instance_ptr_type;
	typedef	packed_array_generic<pint_value_type, instance_ptr_type>
						alias_collection_type;
	typedef	parent_type::inst_ref_ptr_type	inst_ref_ptr_type;
	typedef	parent_type::member_inst_ref_ptr_type	
						member_inst_ref_ptr_type;
	typedef	process_type_reference		type_reference_type;
	typedef	count_ptr<const type_reference_type>
						final_ptr_type;
protected:
	// reserve these for connections between instance_references

	/**
		This is the final type established during unrolling.  
		Q: can cycles form in instance-type heirarchy?
	 */
	final_ptr_type				proc_type;

	// list of port actuals

protected:
	/// Private empty constructor.  
	explicit
	process_instance_collection(const size_t d);
public:
	process_instance_collection(const scopespace& o, const string& n, 
		const size_t d);

virtual	~process_instance_collection();

virtual	ostream&
	what(ostream& o) const = 0;

	ostream&
	dump(ostream& o) const;

	ostream&
	type_dump(ostream& o) const;

virtual ostream&
	dump_unrolled_instances(ostream& o) const = 0;

	bool
	is_partially_unrolled(void) const { return proc_type; }

	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;

	count_ptr<instance_reference_base>
	make_instance_reference(void) const;

	member_inst_ref_ptr_type
	make_member_instance_reference(const inst_ref_ptr_type& b) const;

	bool
	commit_type(const final_ptr_type& );

virtual void
	instantiate_indices(const index_collection_item_ptr_type& i) = 0;

virtual instance_ptr_type
	lookup_instance(const multikey_index_type& i) const = 0;

virtual bool
	lookup_instance_collection(list<instance_ptr_type>& l,
		const const_range_list& r) const = 0;

virtual const_index_list
	resolve_indices(const const_index_list& l) const = 0;

virtual	bool
	unroll_aliases(const multikey_index_type&, const multikey_index_type&, 
		alias_collection_type&) const = 0;

public:
	static
	process_instance_collection*
	make_proc_array(const scopespace& o, const string& n, const size_t d);

	static
	persistent*
	construct_empty(const int);

protected:
	void
	collect_transient_info_base(persistent_object_manager& m) const;

	void
	write_object_base(const persistent_object_manager& m, ostream& ) const;

	void
	load_object_base(const persistent_object_manager& m, istream& );

};	// end class process_instance_collection

//=============================================================================
/**
	Base class for instantiation of a data type, 
	either inside or outside definition.  
 */
class datatype_instance_collection : public instance_collection_base {
private:
	typedef	instance_collection_base	parent_type;
protected:
	typedef	parent_type::inst_ref_ptr_type	inst_ref_ptr_type;
	typedef	parent_type::member_inst_ref_ptr_type	
						member_inst_ref_ptr_type;
	typedef	count_ptr<const data_type_reference>	type_ref_ptr_type;
protected:
	explicit
	datatype_instance_collection(const size_t d) : parent_type(d) { }

	datatype_instance_collection(const scopespace& o, const string& n, 
		const size_t d);

public:

virtual	~datatype_instance_collection();

virtual	ostream&
	what(ostream& o) const = 0;

	ostream&
	dump(ostream& o) const;

	/** returns the type of the first instantiation statement */
	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;

virtual bool
	is_partially_unrolled(void) const = 0;

virtual ostream&
	dump_unrolled_instances(ostream& o) const = 0;

	// a better return type?
virtual	bool
	commit_type(const type_ref_ptr_type& ) = 0;

// methods for connection and aliasing?

// need to do this for real... using object not parse tree
//	bool equals_template_formal(const template_formal_decl& tf) const;
virtual	count_ptr<instance_reference_base>
	make_instance_reference(void) const = 0;

virtual void
	instantiate_indices(const index_collection_item_ptr_type& i) = 0;

virtual	never_ptr<const const_param_expr_list>
	get_actual_param_list(void) const;	// = 0;
	
protected:	// propagate to children
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class datatype_instance_collection

//=============================================================================
/**
	Channel instantiation.  
	Type information is now in the instance_collection_list.
 */
class channel_instance_collection : public instance_collection_base {
private:
	typedef	channel_instance_collection	this_type;
	typedef	instance_collection_base	parent_type;
public:
	typedef chan_instance_alias		instance_alias_type;
	typedef	channel_alias_connection	alias_connection_type;
	typedef never_ptr<chan_instance_alias>	instance_ptr_type;
	typedef	packed_array_generic<pint_value_type, instance_ptr_type>
						alias_collection_type;
	typedef	parent_type::inst_ref_ptr_type	inst_ref_ptr_type;
	typedef	parent_type::member_inst_ref_ptr_type	
						member_inst_ref_ptr_type;
protected:
	// reserve these for connections between instance_references
	// list of template actuals
	// list of port actuals

protected:
	/// Private empty constructor.  
	explicit
	channel_instance_collection(const size_t d) : parent_type(d) { }
public:
	channel_instance_collection(const scopespace& o, const string& n, 
		const size_t d);

virtual	~channel_instance_collection();

virtual	ostream&
	what(ostream& o) const = 0;

	ostream&
	type_dump(ostream& o) const;

//	ostream& dump(ostream& o) const;

virtual	bool
	is_partially_unrolled(void) const = 0;

	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;

	count_ptr<instance_reference_base>
	make_instance_reference(void) const;

	member_inst_ref_ptr_type
	make_member_instance_reference(const inst_ref_ptr_type& b) const;

virtual void
	instantiate_indices(const index_collection_item_ptr_type& i) = 0;

virtual instance_ptr_type
	lookup_instance(const multikey_index_type& i) const = 0;

virtual bool
	lookup_instance_collection(list<instance_ptr_type>& l,
		const const_range_list& r) const = 0;

virtual const_index_list
	resolve_indices(const const_index_list& l) const = 0;

virtual	bool
	unroll_aliases(const multikey_index_type&, const multikey_index_type&, 
		alias_collection_type&) const = 0;


public:
	static
	channel_instance_collection*
	make_chan_array(const scopespace& o, const string& n, const size_t d);

	static
	persistent*
	construct_empty(const int);

	// later array will contain aliases, which contains pointers...
	void
	collect_transient_info(persistent_object_manager& m) const;

protected:
	void
	write_object_base(const persistent_object_manager& m, ostream& ) const;

	void
	load_object_base(const persistent_object_manager& m, istream& );

};	// end class channel_instance_collection

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_H__

