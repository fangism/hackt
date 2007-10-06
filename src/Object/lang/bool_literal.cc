/**
	\file "Object/lang/bool_literal.cc"
	$Id: bool_literal.cc,v 1.6.68.6 2007/10/06 21:14:23 fang Exp $
 */

#include "Object/lang/bool_literal.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/traits/bool_traits.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/lang/PRS.h"	// for PRS::literal, PRS::expr_dump_context
#include "Object/expr/expr_dump_context.h"
#include "Object/traits/node_traits.h"
#include "Object/ref/simple_meta_dummy_reference.h"
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/packed_array.h"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
using util::write_value;
using util::read_value;

//=============================================================================
// class bool_literal method definitions

bool_literal::bool_literal() : var(NULL), int_node(NULL), negated(false)
	{ }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_literal::bool_literal(const bool_literal_base_ptr_type& l) :
		var(l), int_node(NULL), negated(false) {
	NEVER_NULL(var);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_literal::bool_literal(const node_literal_ptr_type& l) :
		var(), int_node(l), negated(false) {
	NEVER_NULL(int_node);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This seems redundant... gimme delegating ctors!
 */
bool_literal::bool_literal(const count_ptr<const PRS::literal>& l) :
		var(l->get_bool_var()), 
		int_node(l->internal_node()), negated(false)
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_literal::bool_literal(const count_ptr<PRS::literal>& l) :
		var(l->get_bool_var()), 
		int_node(l->internal_node()), negated(false)
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_literal::bool_literal(const bool_literal& b) :
		var(b.var), int_node(b.int_node), negated(b.negated) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_literal&
bool_literal::operator = (const bool_literal& b) {
	var = b.var;
	int_node = b.int_node;
	negated = b.negated;
	return *this;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
bool_literal::operator == (const bool_literal& r) const {
	return (var == r.var) && (int_node == r.int_node)
		&& (negated == r.negated);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_literal::~bool_literal() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
bool_literal::dump(ostream& o, const expr_dump_context& c) const {
if (var) {
	return var->dump(o, c);
} else {
	NEVER_NULL(int_node);
	if (negated) o << '~';
	return int_node->dump(o << '@', c);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
bool_literal::dump(ostream& o, const PRS::expr_dump_context& c) const {
	return dump(o, expr_dump_context(c));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This does NOT print any error message.  
	\pre this refers to bool variable, not an internal node.
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
	Negatedness is irrelevant here.
 */
node_literal_ptr_type
bool_literal::unroll_node_reference(const unroll_context& c) const {
	// substitute parameters for constants
	NEVER_NULL(int_node);
	return int_node->__unroll_resolve_copy(c, int_node);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Negatedness is irrelevant here.
	Diagnostic on failure?
 */
bool_literal
bool_literal::unroll_reference(const unroll_context& c) const {
if (var) {
	const count_ptr<const simple_bool_meta_instance_reference>	
		p(var->__unroll_resolve_copy(c, var));
	return bool_literal(p);
} else {
	return bool_literal(unroll_node_reference(c));
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unroll resolves a collection of bool references (group) into
	an array/set.  
	\param g the resulting array/set.  
	\pre this refers to bool variable, not an internal node.
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
if (var) {
	var->collect_transient_info(m);
} else {
	NEVER_NULL(int_node);
	int_node->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reminder: var is exclusive with int_node.
 */
void
bool_literal::write_object_base(const persistent_object_manager& m,
		ostream& o) const {
	m.write_pointer(o, var);
	if (!var) {
		m.write_pointer(o, int_node);
		write_value(o, negated);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_literal::load_object_base(const persistent_object_manager& m,
		istream& i) {
	m.read_pointer(i, var);
	if (!var) {
		m.read_pointer(i, int_node);
		read_value(i, negated);
	}
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

