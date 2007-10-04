/**
	\file "Object/lang/bool_literal.cc"
	$Id: bool_literal.cc,v 1.6.68.2 2007/10/04 05:52:20 fang Exp $
 */

#include "Object/lang/bool_literal.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/traits/bool_traits.h"
#include "Object/ref/simple_meta_instance_reference.h"
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
	NEVER_NULL(var);
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
	NEVER_NULL(var);
	if (var->unroll_references_packed(c, bc).bad) {
		return 0;		// INVALID_NODE_INDEX
	}
	INVARIANT(!bc.dimensions());    // must be scalar, checked earlier
	const instance_alias_info<bool_tag>& bi(*bc.front());
	const size_t node_index = bi.instance_index;
	INVARIANT(node_index);
	return node_index;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unroll resolves a collection of bool references (group) into
	an array/set.  
	\param g the resulting array/set.  
	\return bad to signal an error.
 */
good_bool
bool_literal::unroll_group(const unroll_context& c, group_type& g) const {
	typedef bool_literal_base_ptr_type::element_type::alias_collection_type
			bool_instance_alias_collection_type;
	STACKTRACE_VERBOSE;
	bool_instance_alias_collection_type bc;
	NEVER_NULL(var);
	if (var->unroll_references_packed(c, bc).bad) {
		return good_bool(false);
	}
	typedef	bool_instance_alias_collection_type::const_iterator
					const_iterator;
	const_iterator i(bc.begin()), e(bc.end());
	// could reserve...
	for ( ; i!=e; ++i) {
		const instance_alias_info<bool_tag>& bi(**i);
		const size_t node_index = bi.instance_index;
		INVARIANT(node_index);
		g.push_back(node_index);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_literal::collect_transient_info_base(persistent_object_manager& m) const {
#if PRS_INTERNAL_NODES
	if (var)
#endif
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

