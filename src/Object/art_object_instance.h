/**
	\file "art_object_instance.h"
	Instance collection classes for ART.  
	$Id: art_object_instance.h,v 1.33 2005/01/13 05:28:31 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_H__
#define	__ART_OBJECT_INSTANCE_H__

#include "art_object_instance_base.h"
#include "memory/pointer_classes.h"

#include "multikey_fwd.h"

namespace ART {
namespace entity {
//=============================================================================
USING_LIST
using namespace util::memory;	// for experimental pointer classes
using MULTIKEY_NAMESPACE::multikey_base;

//=============================================================================
// class instance_collection_base declared in "art_object_instance_base.h"

//=============================================================================
/**
	Process instantiation.  
	Type information is now in the instance_collection_list.
 */
class process_instance_collection : public instance_collection_base {
private:
	typedef	instance_collection_base	parent_type;
public:
	typedef never_ptr<proc_instance_alias>	instance_ptr_type;
	typedef multikey_base<int>		unroll_index_type;

protected:
	// reserve these for connections between instance_references
	// list of template actuals
	// list of port actuals

protected:
	/// Private empty constructor.  
	explicit
	process_instance_collection(const size_t d) : parent_type(d) { }
public:
	process_instance_collection(const scopespace& o, const string& n, 
		const size_t d);

virtual	~process_instance_collection();

virtual	ostream&
	what(ostream& o) const = 0;

//	ostream& dump(ostream& o) const;

virtual	bool
	is_partially_unrolled(void) const = 0;

	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;

	count_ptr<instance_reference_base>
	make_instance_reference(void) const;

	count_ptr<member_instance_reference_base>
	make_member_instance_reference(
		const count_ptr<const simple_instance_reference>& b) const;

virtual void
	instantiate_indices(const index_collection_item_ptr_type& i) = 0;

virtual instance_ptr_type
	lookup_instance(const unroll_index_type& i) const = 0;

virtual bool
	lookup_instance_collection(list<instance_ptr_type>& l,
		const const_range_list& r) const = 0;

virtual const_index_list
	resolve_indices(const const_index_list& l) const = 0;


public:
//	PERSISTENT_METHODS

	static
	process_instance_collection*
	make_proc_array(const scopespace& o, const string& n, const size_t d);

	static
	persistent*
	construct_empty(const int);

	void
	collect_transient_info(persistent_object_manager& m) const;

protected:
	void
	write_object_base(const persistent_object_manager& m, ostream& ) const;

	void
	load_object_base(persistent_object_manager& m, istream& );

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
	explicit
	datatype_instance_collection(const size_t d) : parent_type(d) { }
public:
	datatype_instance_collection(const scopespace& o, const string& n, 
		const size_t d);

virtual	~datatype_instance_collection();

virtual	ostream&
	what(ostream& o) const = 0;

	/** returns the type of the first instantiation statement */
	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;

virtual bool
	is_partially_unrolled(void) const = 0;

virtual ostream&
	dump_unrolled_instances(ostream& o) const = 0;

// methods for connection and aliasing?

// need to do this for real... using object not parse tree
//	bool equals_template_formal(const template_formal_decl& tf) const;
virtual	count_ptr<instance_reference_base>
	make_instance_reference(void) const = 0;

	count_ptr<member_instance_reference_base>
	make_member_instance_reference(
		const count_ptr<const simple_instance_reference>& b) const;

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
	typedef	instance_collection_base	parent_type;
public:
	typedef never_ptr<chan_instance_alias>	instance_ptr_type;
	typedef multikey_base<int>		unroll_index_type;

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

//	ostream& dump(ostream& o) const;

virtual	bool
	is_partially_unrolled(void) const = 0;

	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;

	count_ptr<instance_reference_base>
	make_instance_reference(void) const;

	count_ptr<member_instance_reference_base>
	make_member_instance_reference(
		const count_ptr<const simple_instance_reference>& b) const;

virtual void
	instantiate_indices(const index_collection_item_ptr_type& i) = 0;

virtual instance_ptr_type
	lookup_instance(const unroll_index_type& i) const = 0;

virtual bool
	lookup_instance_collection(list<instance_ptr_type>& l,
		const const_range_list& r) const = 0;

virtual const_index_list
	resolve_indices(const const_index_list& l) const = 0;


public:
//	PERSISTENT_METHODS

	static
	channel_instance_collection*
	make_chan_array(const scopespace& o, const string& n, const size_t d);

	static
	persistent*
	construct_empty(const int);

	void
	collect_transient_info(persistent_object_manager& m) const;

protected:
	void
	write_object_base(const persistent_object_manager& m, ostream& ) const;

	void
	load_object_base(persistent_object_manager& m, istream& );

};	// end class channel_instance_collection

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_H__

