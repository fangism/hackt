/**
	\file "Object/nonmeta_channel_manipulator.cc"
	$Id: nonmeta_channel_manipulator.cc,v 1.2 2007/01/21 05:58:25 fang Exp $
 */

#include "Object/nonmeta_channel_manipulator.h"
#include "Object/nonmeta_context.h"
#include "Object/expr/data_expr.h"
#include "Object/ref/data_nonmeta_instance_reference.h"
#include "Object/type/canonical_generic_datatype.h"
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
// class channel_data_dumper method definitions

/**
	TODO: wrap this into an ostream overload interface.
 */
void
channel_data_dumper::operator () (const canonical_generic_datatype& d) {
	// by now datatype is one of the core types: bool, int, enum
	// so we can dispense with the template arguments
	// (unless we want the integer width for masking...)
if (!first) {
	out << ", ";	// leaves last one trailing, ARG...
}
	const never_ptr<const datatype_definition_base>
		b(d.get_base_def());
	NEVER_NULL(b);
	b->dump_channel_field_iterate(out, *this);
	// callee is responsible for advancing the right iterator
	// maybe use some state bit to suppress the first one...
	first = false;
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

