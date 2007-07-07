/**
	\file "Object/unroll/template_type_completion.tcc"
	$Id: template_type_completion.tcc,v 1.1.2.1 2007/07/07 21:12:37 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_TEMPLATE_TYPE_COMPLETION_TCC__
#define	__HAC_OBJECT_UNROLL_TEMPLATE_TYPE_COMPLETION_TCC__

#include <iostream>
#include "Object/unroll/template_type_completion.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/expr/dynamic_param_expr_list.h"
#include "Object/expr/expr_dump_context.h"

#include "common/TODO.h"
#include "util/persistent_object_manager.h"
#include "util/stacktrace.h"
#include "util/what.tcc"

namespace HAC {
namespace entity {
using util::persistent_traits;

//=============================================================================
// class template_type_completion method definitions

template <class Tag>
template_type_completion<Tag>::template_type_completion() :
		ref(), relaxed_args() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param r is the instance reference
	\param a is the list of template arguments to attach
 */
template <class Tag>
template_type_completion<Tag>::template_type_completion(
		const reference_ptr_type& r, 
		const const_relaxed_args_type& a) :
		ref(r), relaxed_args(a) {
	NEVER_NULL(ref);
	NEVER_NULL(relaxed_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
template_type_completion<Tag>::~template_type_completion() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
ostream&
template_type_completion<Tag>::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
ostream&
template_type_completion<Tag>::dump(ostream& o,
		const expr_dump_context& c) const {
	ref->dump(o, c);
	return relaxed_args->dump(o << '<', c) << '>';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
good_bool
template_type_completion<Tag>::unroll(const unroll_context& c) const {
	FINISH_ME(Fang);
	return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
template_type_completion<Tag>::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this,
		persistent_traits<this_type>::type_key)) {
	ref->collect_transient_info(m);
	relaxed_args->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
template_type_completion<Tag>::write_object(const persistent_object_manager& m,
		ostream& o) const {
	m.write_pointer(o, ref);
	m.write_pointer(o, relaxed_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
template_type_completion<Tag>::load_object(const persistent_object_manager& m,
		istream& i) {
	m.read_pointer(i, ref);
	m.read_pointer(i, relaxed_args);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_TEMPLATE_TYPE_COMPLETION_TCC__

