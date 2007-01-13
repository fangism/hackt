/**
	\file "Object/nonmeta_channel_manipulator.cc"
	$Id: nonmeta_channel_manipulator.cc,v 1.1.2.1 2007/01/13 02:07:58 fang Exp $
 */

#include "Object/nonmeta_channel_manipulator.h"
#include "Object/nonmeta_context.h"
#include "Object/expr/data_expr.h"
#include "Object/ref/data_nonmeta_instance_reference.h"
#include "util/memory/count_ptr.tcc"

namespace HAC {
namespace entity {
//=============================================================================
// class nonmeta_expr_evaluator_channel_writer method defintions

/**
	Writes evaluated nonmeta data expressions to packed channel fields.  
	Inline me?
 */
void
nonmeta_expr_evaluator_channel_writer::operator () (
		const count_ptr<const data_expr>& d) {
	NEVER_NULL(d);
	d->evaluate_write(context, writer, d);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Should we allow NULL lvalue references?
 */
void
nonmeta_reference_lookup_channel_reader::operator() (
		const count_ptr<const data_nonmeta_instance_reference>& r) {
	NEVER_NULL(r);
	r->direct_assign(context, updates, reader);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

