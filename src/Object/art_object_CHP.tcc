/**
	\file "Object/art_object_CHP.tcc"
	Template method definitions for CHP classes.
	$Id: art_object_CHP.tcc,v 1.1.4.3 2005/06/14 18:16:26 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_CHP_TCC__
#define	__OBJECT_ART_OBJECT_CHP_TCC__

#include <iostream>
#include <iterator>
#include "util/memory/count_ptr.tcc"
#include "Object/art_object_CHP.h"
#include "Object/art_object_instance_chan.h"
#include "Object/art_object_type_ref.h"
#include "Object/art_object_nonmeta_inst_ref.h"
#include "Object/art_object_nonmeta_inst_ref_subtypes.h"

namespace ART {
namespace entity {
namespace CHP {
using std::copy;
using std::back_inserter;
using util::memory::never_ptr;
#include "util/using_ostream.h"

//=============================================================================
// class channel_send method definitions

/**
	\param L the list type, can be deduced.  
	\param l the list of expressions to (type-check) and add.
 */
template <class L>
good_bool
channel_send::add_expressions(const L& l) {
	// get chan's type, in canonical form
	const never_ptr<const channel_instance_collection>
		inst_base(chan->get_inst_base_subtype());
	const count_ptr<const channel_type_reference_base>
		type_ref(inst_base->get_type_ref()
			.template is_a<const channel_type_reference_base>());
	// critical that this next pointer only exists locally 
	// see channel_type_reference::resolve_builtin_channel_type
	NEVER_NULL(type_ref);
	const never_ptr<const builtin_channel_type_reference>
		bctr(type_ref->resolve_builtin_channel_type());
	if (bctr) {
		const size_t l_size = l.size();
		const size_t max = bctr->num_datatypes();
		if (l_size != max) {
			cerr << "You doofus, you passed the wrong number of "
				"expressions to channel-send, which requires "
				<< max << " arguments." << endl;
			// somewhere need to catch insufficient...
			return good_bool(false);
		}
		// was able to resolve built-in channel type
		typedef	builtin_channel_type_reference::datatype_list_type
							datatype_list_type;
		const builtin_channel_type_reference::datatype_list_type&
			types(bctr->get_datatype_list());
		datatype_list_type::const_iterator ti(types.begin());
		const datatype_list_type::const_iterator te(types.end());
		typename L::const_iterator ei(l.begin());
		size_t i = 1;
		for ( ; ti!=te; ti++, ei++, i++) {
			const count_ptr<const data_type_reference>
				etype((*ei)->get_data_type_ref());
			if (!etype) {
				cerr << "Error resolving expression " << i <<
					" in send expression list.  " << endl;
				return good_bool(false);
			}
			if (!(*ti)->may_be_type_equivalent(*etype)) {
				cerr << "Type mismatch in expression " << i <<
					" of send expression list.  " << endl;
				return good_bool(false);
			}
			// temporary measure:
			if ((*ei)->dimensions()) {
				cerr << "Sorry, non-scalar instance references "
					"are not yet supported in "
					"data expressions.  " << endl;
				cerr << "See 2005-06-11 entry of NOTES."
					<< endl;
				return good_bool(false);
			}
			exprs.push_back(*ei);
		}
		INVARIANT(ei == l.end());
		return good_bool(true);
	} else {
		// not able to resolve, probably because of template
		// parameter dependence.  Will resolve in meta-expansion
		// phase.  Assume it's good for now, do not reject.
		copy(l.begin(), l.end(), back_inserter(exprs));
		return good_bool(true); 
	}
}

//=============================================================================

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_CHP_TCC__

