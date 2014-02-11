/**
	\file "Object/lang/bool_literal.cc"
	$Id: bool_literal.cc,v 1.10 2010/09/02 00:34:39 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include "Object/lang/bool_literal.hh"
#include "Object/inst/alias_empty.hh"
#include "Object/inst/instance_alias_info.hh"
#include "Object/traits/bool_traits.hh"
#include "Object/ref/simple_meta_instance_reference.hh"
#include "Object/ref/meta_instance_reference_subtypes.hh"
#include "Object/lang/PRS.hh"	// for PRS::literal, PRS::expr_dump_context
#include "Object/expr/expr_dump_context.hh"
#include "Object/traits/node_traits.hh"
#include "Object/ref/simple_meta_dummy_reference.hh"
#include "Object/global_entry.hh"
#include "Object/global_entry_context.hh"
#include "Object/unroll/unroll_context.hh"
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/packed_array.hh"
#include "util/stacktrace.hh"

namespace HAC {
namespace entity {
using util::write_value;
using util::read_value;

//=============================================================================
// from "Object/lang/SPEC.cc"
extern
bool
allow_private_member_references;

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
bool
bool_literal::valid(void) const {
	return var || int_node;
}

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
#if ENABLE_STACKTRACE
	var->dump(STACKTRACE_INDENT_PRINT(""), PRS::expr_dump_context())
		<< std::endl;
#endif
	if (var->unroll_references_packed(c, bc).bad) {
		return 0;		// INVALID_NODE_INDEX
	}
	INVARIANT(!bc.dimensions());    // must be scalar, checked earlier
	const instance_alias_info<tag_type>& bi(*bc.front());
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
	typedef	simple_meta_instance_reference<tag_type>	reference_type;
	typedef reference_type::alias_collection_type
					bool_instance_alias_collection_type;
	typedef reference_type::subindex_collection_type
					bool_subindex_collection_type;
	STACKTRACE_VERBOSE;
	NEVER_NULL(var);
if (allow_private_member_references) {
	// direct translation to indices, allowing references to 
	// private sub-members in the hierarchy.
	bool_subindex_collection_type pbi;
	const global_process_context gpc(c.get_target_footprint());
	const global_entry_context gc(gpc);
	if (var->unroll_subindices_packed(gc, c, pbi).bad) {
		return good_bool(false);
	}
	copy(pbi.begin(), pbi.end(), back_inserter(g));
} else {
	bool_instance_alias_collection_type bc;
	if (var->unroll_references_packed(c, bc).bad) {
		return good_bool(false);
	}
	transform(bc.begin(), bc.end(), back_inserter(g), 
		instance_index_extractor());
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

