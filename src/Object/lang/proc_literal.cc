/**
	\file "Object/lang/proc_literal.cc"
	$Id: proc_literal.cc,v 1.2 2010/08/24 21:05:47 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include "Object/lang/proc_literal.hh"
#include "Object/inst/alias_actuals.hh"
#include "Object/inst/instance_alias_info.hh"
#include "Object/traits/proc_traits.hh"
#include "Object/ref/simple_meta_instance_reference.hh"
#include "Object/ref/meta_instance_reference_subtypes.hh"
#include "Object/expr/expr_dump_context.hh"
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
#include "util/using_ostream.hh"		// debug only

//=============================================================================
// global variables
// from "Object/lang/SPEC.cc"
/**
	Allow private member references in hierarchical references.
	Should be allowed only in certain contexts.  
 */
extern
bool
allow_private_member_references;

//=============================================================================
// class proc_literal method definitions

proc_literal::proc_literal() : var(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
proc_literal::proc_literal(const proc_literal_base_ptr_type& l) :
		var(l) {
	NEVER_NULL(var);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
proc_literal::proc_literal(const proc_literal& b) : var(b.var) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
proc_literal&
proc_literal::operator = (const proc_literal& b) {
	var = b.var;
	return *this;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
proc_literal::operator == (const proc_literal& r) const {
	return (var == r.var);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
proc_literal::~proc_literal() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
proc_literal::dump(ostream& o, const expr_dump_context& c) const {
	return var->dump(o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
proc_literal::dump(ostream& o, const PRS::expr_dump_context& c) const {
	return dump(o, expr_dump_context(c));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This does NOT print any error message.  
	\pre this refers to bool variable, not an internal node.
	\return ID number of the referenced scalar bool, or 0 if not found.  
 */
size_t
proc_literal::unroll_base(const unroll_context& c) const {
	typedef proc_literal_base_ptr_type::element_type::alias_collection_type
			proc_instance_alias_collection_type;
	STACKTRACE_VERBOSE;
	proc_instance_alias_collection_type bc;
	NEVER_NULL(var);
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
	Diagnostic on failure?
 */
proc_literal
proc_literal::unroll_reference(const unroll_context& c) const {
	const count_ptr<const simple_process_meta_instance_reference>	
		p(var->__unroll_resolve_copy(c, var));
	return proc_literal(p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unroll resolves a collection of proc references (group) into
	an array/set.  
	TODO: allow collection to contain different types!
	\param g the resulting array/set.  
	\pre this refers to bool variable, not an internal node.
	\return bad to signal an error.
 */
good_bool
proc_literal::unroll_group(const unroll_context& c, group_type& g) const {
	typedef	simple_meta_instance_reference<tag_type>	reference_type;
	typedef reference_type::alias_collection_type
					proc_instance_alias_collection_type;
	typedef reference_type::subindex_collection_type
					proc_subindex_collection_type;
	STACKTRACE_VERBOSE;
	NEVER_NULL(var);
if (allow_private_member_references) {
	// direct translation to indices, allowing references to 
	// private sub-members in the hierarchy.
	proc_subindex_collection_type pbi;
	const global_process_context gpc(c.get_target_footprint());
	const global_entry_context gc(gpc);
	if (var->unroll_subindices_packed(gc, c, pbi).bad) {
		return good_bool(false);
	}
	copy(pbi.begin(), pbi.end(), back_inserter(g));
} else {
	proc_instance_alias_collection_type bc;
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
proc_literal::collect_transient_info_base(persistent_object_manager& m) const {
	var->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
proc_literal::write_object_base(const persistent_object_manager& m,
		ostream& o) const {
	m.write_pointer(o, var);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
proc_literal::load_object_base(const persistent_object_manager& m,
		istream& i) {
	m.read_pointer(i, var);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

