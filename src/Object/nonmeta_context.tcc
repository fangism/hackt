/**
	\file "Object/nonmeta_context.tcc"
	$Id: nonmeta_context.tcc,v 1.3 2011/02/25 23:19:29 fang Exp $
 */

#ifndef	__HAC_OBJECT_NONMETA_CONTEXT_TCC__
#define	__HAC_OBJECT_NONMETA_CONTEXT_TCC__

#include <iostream>
#include <vector>
#include "Object/nonmeta_context.hh"
// #include "Object/ref/simple_nonmeta_value_reference.hh"
#include "Object/ref/simple_meta_instance_reference.hh"
#include "Object/global_entry_context.tcc"
#include "Object/expr/const_index_list.hh"
#include "Object/expr/nonmeta_index_list.hh"
#include "util/stacktrace.hh"

namespace HAC {
namespace entity {
#include "util/using_ostream.hh"

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
	// FIXME: this assumes is not a member reference!
	STACKTRACE_VERBOSE;
#if !NONMETA_MEMBER_REFERENCES
	typedef	simple_meta_instance_reference<Tag>
					meta_reference_type;
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
#else
	typedef	typename reference_type::meta_reference_type
					meta_reference_type;
	const count_ptr<meta_reference_type>
		tmp(r.resolve_meta_reference(*this));
	NEVER_NULL(tmp);		// TODO: error handle
	const meta_reference_type&
		cr(IS_A(const meta_reference_type&, *tmp));
#endif
	return this->lookup_meta_reference_global_index(cr);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_NONMETA_CONTEXT_TCC__

