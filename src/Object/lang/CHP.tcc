/**
	\file "Object/lang/CHP.tcc"
	Template method definitions for CHP classes.
	$Id: CHP.tcc,v 1.1.2.1 2005/07/23 01:05:57 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_CHP_TCC__
#define	__OBJECT_ART_OBJECT_CHP_TCC__

#include <iostream>
#include <iterator>
#include "util/memory/count_ptr.tcc"
#include "Object/lang/CHP.h"
#include "Object/inst/channel_instance_collection.h"
#include "Object/type/data_type_reference.h"
#include "Object/type/builtin_channel_type_reference.h"
#include "Object/ref/simple_nonmeta_instance_reference.h"
#include "Object/ref/nonmeta_instance_reference_subtypes.h"

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
				"expressions to a channel-send that expects "
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
			// TODO: consider using a predicated copy_if functor?
			NEVER_NULL(*ei);
			const count_ptr<const data_type_reference>
				etype((*ei)->get_data_type_ref());
			if (!etype) {
				cerr << "Error resolving expression " << i <<
					" in send expression list.  " << endl;
				return good_bool(false);
			}
			if (!(*ti)->may_be_connectibly_type_equivalent(*etype)) {
				cerr << "Type mismatch in expression " << i <<
					" of send expression list.  " << endl;
				etype->dump(cerr << "\tgot: ") << endl;
				(*ti)->dump(cerr << "\texpected: ") << endl;
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
// class channel_receive method definitions

/**
	\param L the list type, can be deduced.  
	\param l the list of expressions to (type-check) and add.
 */
template <class L>
good_bool
channel_receive::add_references(const L& l) {
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
				"referenecs to a channel-receive that expects "
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
			// TODO: consider using a predicated copy_if functor?
			NEVER_NULL(*ei);
#if 0
			const count_ptr<simple_datatype_nonmeta_value_reference>
				sdir(ei->template is_a<simple_datatype_nonmeta_value_reference>());
			if (!sdir) {
				cerr << "Sorry, channel-receive is currently "
					"limited to simple, datatype nonmeta "
					"instance_references.  " << endl;
				(*ei)->what(cerr << "\tgot: ") << endl;
				return good_bool(false);
			}
#else
			const count_ptr<simple_datatype_nonmeta_value_reference>
				sdir(*ei);
#endif
			const count_ptr<const data_type_reference>
				etype(sdir->get_data_type_ref());
			if (!etype) {
				cerr << "Error resolving reference " << i <<
					" in receive list.  " << endl;
				return good_bool(false);
			}
			if (!(*ti)->may_be_connectibly_type_equivalent(*etype)) {
				cerr << "Type mismatch in reference " << i <<
					" of receive list.  " << endl;
				etype->dump(cerr << "\tgot: ") << endl;
				(*ti)->dump(cerr << "\texpected: ") << endl;
				return good_bool(false);
			}
			// temporary measure:
			if (sdir->dimensions()) {
				cerr << "Sorry, non-scalar instance references "
					"are not yet supported in "
					"nonmeta data references.  " << endl;
				cerr << "See 2005-06-11 entry of NOTES."
					<< endl;
				return good_bool(false);
			}
			insts.push_back(sdir);
		}
		INVARIANT(ei == l.end());
		return good_bool(true);
	} else {
		// not able to resolve, probably because of template
		// parameter dependence.  Will resolve in meta-expansion
		// phase.  Assume it's good for now, do not reject.
		copy(l.begin(), l.end(), back_inserter(insts));
		return good_bool(true); 
	}
}

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_CHP_TCC__

