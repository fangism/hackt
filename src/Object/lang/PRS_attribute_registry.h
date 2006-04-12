/**
	\file "Object/lang/PRS_attribute_registry.h"
	$Id: PRS_attribute_registry.h,v 1.3 2006/04/12 08:53:15 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_ATTRIBUTE_REGISTRY_H__
#define	__HAC_OBJECT_LANG_PRS_ATTRIBUTE_REGISTRY_H__

#include <string>
#include "util/NULL.h"
#include "util/size_t.h"
#include "util/boolean_types.h"
#include "util/qmap.h"

namespace HAC {
namespace entity {
class const_param_expr_list;
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
class attribute_definition_entry {
public:
	typedef	const_param_expr_list		values_type;
	typedef void (main_type)(cflat_prs_printer&, const values_type&);
	typedef	main_type*			main_ptr_type;
	typedef	good_bool (check_values_type)(const values_type&);
	typedef	check_values_type*		check_values_ptr_type;
private:
	string					_key;
	main_ptr_type				_main;
	check_values_ptr_type			_check_values;
public:
	attribute_definition_entry() : _key(), _main(NULL),
		_check_values(NULL) { }

	attribute_definition_entry(const string& k, const main_ptr_type m,
			const check_values_ptr_type c = NULL) :
			_key(k), _main(m), _check_values(c) { }

	operator bool () const { return _main; }

	void
	main(cflat_prs_printer&, const values_type&) const;

	good_bool
	check_values(const values_type&) const;

};	// end attribute_definition_registry

//=============================================================================
/**
	The global map type for attributes.
 */
typedef	util::qmap<string, attribute_definition_entry>	attribute_registry_type;

/**
	The global PRS attribute registry.  
 */
extern const attribute_registry_type			attribute_registry;

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_PRS_ATTRIBUTE_REGISTRY_H__

