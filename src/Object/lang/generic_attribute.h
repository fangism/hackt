/**
	\file "Object/lang/generic_attribute.h"
	Basic attribute tuple.  
	$Id: generic_attribute.h,v 1.1.18.2 2009/09/02 00:22:58 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_GENERIC_ATTRIBUTE_H__
#define	__HAC_OBJECT_LANG_GENERIC_ATTRIBUTE_H__

#include <string>
#include <vector>
#include "Object/lang/generic_attribute_fwd.h"
#include "util/boolean_types.h"
#include "util/persistent_fwd.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
class unroll_context;
class param_expr;
class dynamic_param_expr_list;
class const_param_expr_list;
struct expr_dump_context;
struct resolved_attribute;
using std::vector;
using std::ostream;
using std::istream;
using std::string;
using util::persistent_object_manager;
using util::memory::count_ptr;
using util::good_bool;
typedef	vector<resolved_attribute>	resolved_attribute_list_base;
struct resolved_attribute_list_type;

//=============================================================================
/**
	Generic key-values pair, where value is yet unresolved.  
	Consideration: for efficient copy-constructing, 
	use a count_ptr<vector<...> > instead of a vector.  
	String are already efficiently copied internally.  
 */
class generic_attribute {
public:
	typedef	dynamic_param_expr_list		values_type;
	typedef	count_ptr<const param_expr>	value_type;
	typedef	const value_type&		const_reference;
	typedef	value_type&			reference;
private:
	// TODO: replace string with symbol (shared memory!)
	string					key;
	count_ptr<values_type>			values;
public:
	generic_attribute();

	explicit
	generic_attribute(const string&);

	generic_attribute(const resolved_attribute&);

	~generic_attribute();

	operator bool () const;

	bool
	operator == (const generic_attribute&) const;

	// arg is equiv to const_reference
	void
	push_back(const value_type&);

	const string&
	get_key(void) const { return key; }

	count_ptr<const values_type>
	get_values(void) const;

	ostream&
	dump(ostream&, const expr_dump_context& c) const;

	count_ptr<const const_param_expr_list>
	unroll_values(const unroll_context&) const;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);

};	// end class generic_attribute

//-----------------------------------------------------------------------------
/**
	Just a list/vector of attributes.  
	We define some convenient methods here.  
 */
class generic_attribute_list_type : public vector<generic_attribute> {
	typedef	vector<generic_attribute>		parent_type;
public:
	typedef	parent_type::value_type			value_type;
	typedef	parent_type::const_reference		const_reference;
	typedef	parent_type::const_iterator		const_iterator;
public:
	generic_attribute_list_type() : parent_type() { }

	generic_attribute_list_type(const resolved_attribute_list_type&);

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class generic_attribute_list_type

//-----------------------------------------------------------------------------
/**
	Unroll-resolved attributes.  
	Generated from PRS::attribute.
	Probably should use a pointer yet to keep persistence easy.  
 */
struct resolved_attribute {
	typedef	const_param_expr_list		values_type;
	typedef	count_ptr<const values_type>	values_ptr_type;
	string					key;
	values_ptr_type				values;

	resolved_attribute();

	explicit
	resolved_attribute(const string&);
	resolved_attribute(const string&, const values_ptr_type&);

	~resolved_attribute();

	ostream&
	dump(ostream&) const;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);

};	// end struct resolved_attribute

//-----------------------------------------------------------------------------
class resolved_attribute_list_type : public resolved_attribute_list_base {
	typedef	resolved_attribute_list_base		parent_type;
public:
	typedef	parent_type::value_type			value_type;
	typedef	parent_type::const_reference		const_reference;
	typedef	parent_type::const_iterator		const_iterator;

	ostream&
	dump(ostream&) const;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);
};	// end class resolved_attribute_list_type

//-----------------------------------------------------------------------------

/**
	Resolves a set of generic attributes.
 */
template <class AttrMapType>
good_bool
unroll_check_attributes(const generic_attribute_list_type&,
		resolved_attribute_list_type&, 
		const unroll_context&, 
		const AttrMapType&);

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_GENERIC_ATTRIBUTE_H__

