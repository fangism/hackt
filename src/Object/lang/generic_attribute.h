/**
	\file "Object/lang/generic_attribute.h"
	Basic attribute tuple.  
	$Id: generic_attribute.h,v 1.1 2008/10/05 23:00:17 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_GENERIC_ATTRIBUTE_H__
#define	__HAC_OBJECT_LANG_GENERIC_ATTRIBUTE_H__

#include <string>
#include <vector>
#include "Object/lang/generic_attribute_fwd.h"
#include "util/persistent_fwd.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
class unroll_context;
class param_expr;
class dynamic_param_expr_list;
class const_param_expr_list;
struct expr_dump_context;
using std::ostream;
using std::istream;
using std::string;
using util::persistent_object_manager;
using util::memory::count_ptr;

//=============================================================================
/**
	Generic key-values pair.  
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
	string					key;
	count_ptr<values_type>			values;
public:
	generic_attribute();

	explicit
	generic_attribute(const string&);

	~generic_attribute();

	operator bool () const;

	// arg is equiv to const_reference
	void
	push_back(const value_type&);

	const string&
	get_key(void) const { return key; }

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
class generic_attribute_list_type : public std::vector<generic_attribute> {
public:
	ostream&
	dump(ostream&, const expr_dump_context&) const;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class generic_attribute_list_type

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_GENERIC_ATTRIBUTE_H__

