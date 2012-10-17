/**
	\file "Object/lang/attribute_visitor_entry.hh"
	Attribute function structure.  
	$Id: attribute_visitor_entry.hh,v 1.1 2008/10/07 03:22:25 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_ATTRIBUTE_VISITOR_ENTRY_H__
#define	__HAC_OBJECT_LANG_ATTRIBUTE_VISITOR_ENTRY_H__

#include <string>
#include "util/NULL.h"
#include "util/boolean_types.hh"
#include "Object/lang/PRS_fwd.hh"

namespace HAC {
namespace entity {
using std::string;
using util::good_bool;

//=============================================================================
/**
	Structure with name of attribute and a function pointer for
	checking value types and constraints, and another for applying
	the attribute to whatever.  
	Do we need main_before, main_after?
	Or just assume that attributes are printed (cflat)
	as prefixes to rules?
 */
template <class VisitorType>
class attribute_visitor_entry {
public:
	typedef	VisitorType			visitor_type;
	typedef	resolved_attribute_values_type	values_type;
	typedef void (main_type)(visitor_type&, const values_type&);
	typedef	main_type*			main_ptr_type;
	typedef	good_bool (check_values_type)(const values_type&);
	typedef	check_values_type*		check_values_ptr_type;
private:
	string					_key;
	main_ptr_type				_main;
	check_values_ptr_type			_check_values;
public:
	attribute_visitor_entry() : _key(), _main(NULL),
		_check_values(NULL) { }

	attribute_visitor_entry(const string& k, const main_ptr_type m,
			const check_values_ptr_type c = NULL) :
			_key(k), _main(m), _check_values(c) { }

	operator bool () const { return _main; }

	void
	main(visitor_type& p, const values_type& v) const {
		if (this->_main) {
			(*this->_main)(p, v);
		}
	}

	good_bool
	check_values(const values_type& v) const {
		if (this->_check_values) {
			return (*this->_check_values)(v);
		} else	return good_bool(true);
	}

};	// end attribute_visitor_entry

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_ATTRIBUTE_VISITOR_ENTRY_H__

