/**
	\file "Object/lang/generic_attribute.tcc"
	$Id: generic_attribute.tcc,v 1.1.2.2 2009/09/02 22:09:27 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_GENERIC_ATTRIBUTE_TCC__
#define	__HAC_OBJECT_LANG_GENERIC_ATTRIBUTE_TCC__

#include <iostream>
#include "Object/lang/generic_attribute.h"
#include "util/memory/count_ptr.tcc"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"

//=============================================================================

/**
	Resolves a set of generic attributes.
	\param r is the result list of unrolled attributes.
	\param m is the attribute map to use to look up.
	\return good if no errors.  Unknown attributes are currently
		retported as errors.  
 */
template <class AttrMapType>
good_bool
unroll_check_attributes(const generic_attribute_list_type& attributes,
		resolved_attribute_list_type& r, 
		const unroll_context& c, 
		const AttrMapType& m) {
	STACKTRACE_VERBOSE;
	typedef	generic_attribute_list_type::const_iterator	const_iterator;
	const_iterator i(attributes.begin());
	const const_iterator e(attributes.end());
	for ( ; i!=e; ++i) {
		const string& key(i->get_key());
		// check whether or not named attribute is registered
		// NOTE: every directive should at least be registered
		// as a cflat directive, the master set of all directives.  
		const typename AttrMapType::const_iterator f(m.find(key));
		if (f == m.end()) {
			cerr << "Error: unrecognized attribute \'" << key <<
				"\'." << endl;
			return good_bool(false);
		}
		const typename AttrMapType::mapped_type& att(f->second);
		if (i->get_values()) {
			const count_ptr<const const_param_expr_list>
				att_vals(i->unroll_values(c));
			if (!att_vals) {
				// catch error?
				// allow value-less attributes
				const const_param_expr_list empty;
				if (!att.check_values(empty).good)
					return good_bool(false);
			} else
			if (!att.check_values(*att_vals).good) {
				// already have error message
				return good_bool(false);
			}
			r.push_back(resolved_attribute(key, att_vals));
		} else {
			// can have no values
			r.push_back(resolved_attribute(key));
		}
	}
	return good_bool(true);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_GENERIC_ATTRIBUTE_TCC__

