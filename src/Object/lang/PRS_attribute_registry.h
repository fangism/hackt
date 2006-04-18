/**
	\file "Object/lang/PRS_attribute_registry.h"
	$Id: PRS_attribute_registry.h,v 1.4.2.1 2006/04/18 05:57:22 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_ATTRIBUTE_REGISTRY_H__
#define	__HAC_OBJECT_LANG_PRS_ATTRIBUTE_REGISTRY_H__

#include <string>
#include "util/NULL.h"
#include "util/size_t.h"
#include "util/boolean_types.h"
#include "util/qmap.h"
#include "Object/lang/PRS_fwd.h"

namespace HAC {
namespace entity {
namespace PRS {
using std::string;
using util::good_bool;
class cflat_prs_printer;

//=============================================================================
/**
	Do we need main_before, main_after?
	Or just assume that attributes are printed (cflat)
	as prefixes to rules?
 */
template <class VisitorType>
class attribute_visitor_entry {
public:
	typedef	VisitorType			visitor_type;
	typedef	rule_attribute_values_type	values_type;
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
typedef	attribute_visitor_entry<cflat_prs_printer>
					cflat_attribute_definition_entry;
/**
	The global map type for attributes.
 */
typedef	util::qmap<string, cflat_attribute_definition_entry>
						cflat_attribute_registry_type;

/**
	The global PRS attribute registry.  
	This is globally/statically initialized in this corresponding .cc file. 
 */
extern const cflat_attribute_registry_type	cflat_attribute_registry;

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_PRS_ATTRIBUTE_REGISTRY_H__

