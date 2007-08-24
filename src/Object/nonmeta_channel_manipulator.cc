/**
	\file "Object/nonmeta_channel_manipulator.cc"
	$Id: nonmeta_channel_manipulator.cc,v 1.2.28.1 2007/08/24 03:48:02 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#if ENABLE_STACKTRACE
#include <iostream>
#include "Object/expr/expr_dump_context.h"
#endif

#include "Object/nonmeta_channel_manipulator.h"
#include "Object/nonmeta_context.h"
#include "Object/expr/data_expr.h"
#include "Object/ref/data_nonmeta_instance_reference.h"
#include "Object/type/canonical_generic_datatype.h"
#include "util/memory/count_ptr.tcc"
#include "util/macros.h"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
//=============================================================================
template <class Tag>
channel_data_writer_base<Tag>::~channel_data_writer_base() {
	INVARIANT(iter <= &data.member_fields[data.member_fields.size()]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
channel_data_reader_base<Tag>::~channel_data_reader_base() {
	INVARIANT(iter <= &data.member_fields[data.member_fields.size()]);
}

//=============================================================================
// class nonmeta_expr_evaluator_channel_writer method defintions

/**
	Writes evaluated nonmeta data expressions to packed channel fields.  
	Inline me?
 */
void
nonmeta_expr_evaluator_channel_writer::operator () (
		const count_ptr<const data_expr>& d) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(d);
#if ENABLE_STACKTRACE
	d->dump(std::cout << "writer: ",
		expr_dump_context::default_value) << std::endl;
#endif
	d->evaluate_write(context, writer, d);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Should we allow NULL lvalue references?
 */
void
nonmeta_reference_lookup_channel_reader::operator() (
		const count_ptr<const data_nonmeta_instance_reference>& r) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(r);
	r->direct_assign(context, updates, reader);
}

//=============================================================================
channel_data_writer::~channel_data_writer() { }

channel_data_reader::~channel_data_reader() { }

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

