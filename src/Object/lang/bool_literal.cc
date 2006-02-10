/**
	\file "Object/lang/bool_literal.cc"
	$Id: bool_literal.cc,v 1.3 2006/02/10 21:50:40 fang Exp $
 */

#include "Object/lang/bool_literal.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/traits/bool_traits.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/ref/simple_datatype_meta_instance_reference_base.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/lang/PRS.h"	// for PRS::literal, PRS::expr_dump_context
#include "Object/expr/expr_dump_context.h"
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/packed_array.h"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
//=============================================================================
// class bool_literal method definitions

bool_literal::bool_literal() : var(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_literal::bool_literal(const bool_literal_base_ptr_type& l) : var(l) {
	NEVER_NULL(var);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_literal::bool_literal(const count_ptr<const PRS::literal>& l) :
		var(l->get_bool_var()) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_literal::bool_literal(const count_ptr<PRS::literal>& l) :
		var(l->get_bool_var()) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_literal::~bool_literal() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
bool_literal::dump(ostream& o, const expr_dump_context& c) const {
	return var->dump(o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
bool_literal::dump(ostream& o, const PRS::expr_dump_context& c) const {
	return dump(o, expr_dump_context(c));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This does NOT print any error message.  
	\return ID number of the referenced scalar bool, or 0 if not found.  
 */
size_t
bool_literal::unroll_base(const unroll_context& c) const {
	typedef bool_literal_base_ptr_type::element_type::alias_collection_type
			bool_instance_alias_collection_type;
	STACKTRACE_VERBOSE;
	bool_instance_alias_collection_type bc;
	if (var->unroll_references(c, bc).bad) {
		return 0;		// INVALID_NODE_INDEX
	}
	INVARIANT(!bc.dimensions());    // must be scalar, checked earlier
	const instance_alias_info<bool_tag>& bi(*bc.front());
	const size_t node_index = bi.instance_index;
	INVARIANT(node_index);
	return node_index;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_literal::collect_transient_info_base(persistent_object_manager& m) const {
	var->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_literal::write_object_base(const persistent_object_manager& m,
		ostream& o) const {
	m.write_pointer(o, var);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_literal::load_object_base(const persistent_object_manager& m,
		istream& i) {
	m.read_pointer(i, var);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

