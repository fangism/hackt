/**
	\file "Object/nonmeta_context.tcc"
	$Id: nonmeta_context.tcc,v 1.2 2007/01/21 05:58:29 fang Exp $
 */

#ifndef	__HAC_OBJECT_NONMETA_CONTEXT_TCC__
#define	__HAC_OBJECT_NONMETA_CONTEXT_TCC__

#include <iostream>
#include "Object/nonmeta_context.h"
// #include "Object/ref/simple_nonmeta_value_reference.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/global_entry_context.tcc"
#include "Object/expr/const_index_list.h"
#include "Object/expr/nonmeta_index_list.h"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"

//=============================================================================
// class nonemta_context_base method definitions

/**
	Looks up a nonmeta instance/value reference's global index.  
	\param r the nonmeta reference to lookup.
	\return global index corresponding to reference or 0 if failed.  
 */
template <class reference_type>
size_t
nonmeta_context_base::lookup_nonmeta_reference_global_index(
		const reference_type& r) const {
	typedef	typename reference_type::traits_type	traits_type;
	typedef	typename traits_type::tag_type		Tag;
	typedef	typename traits_type::instance_collection_generic_type
					instance_collection_generic_type;
	typedef	simple_meta_instance_reference<Tag>	meta_reference_type;

	STACKTRACE_VERBOSE;
	const never_ptr<const nonmeta_index_list> r_ind(r.get_indices());
	meta_reference_type cr(r.get_inst_base_subtype());
	if (r_ind) {
		const count_ptr<const const_index_list>
			cil(r_ind->nonmeta_resolve_copy(*this));
		if (!cil) {
			cerr << "Run-time error resolving nonmeta indices."
				<< endl;
			return 0;
		}
		cr.attach_indices(cil);
	}
	// else is scalar
	return this->lookup_meta_reference_global_index(cr);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_NONMETA_CONTEXT_TCC__

