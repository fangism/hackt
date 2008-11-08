/**
	\file "parser/instref.cc"
	$Id: instref.cc,v 1.12 2008/11/08 04:25:55 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>

#include "config.h"
#include "AST/AST.h"
#include "parser/instref.h"
#include "parser/instref-prefix.h"
#include "parser/instref-parse-real.h"
#include "parser/instref-parse-options.h"
#include "lexer/flex_lexer_state.h"
#include "util/stacktrace.h"

// the rest of this file came from an old "sim/prsim/Reference.cc"

#include <iterator>
#include <algorithm>
#include <cstdio>
#include <string>
#include "AST/parse_context.h"
#include "Object/module.h"
#include "Object/common/namespace.h"
#include "Object/unroll/unroll_context.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/proc_traits.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/ref/meta_reference_union.h"
#include "Object/traits/type_tag_enum.h"
#include "Object/entry_collection.h"
#include "common/TODO.h"
#include "util/tokenize_fwd.h"		// for string_list
#include "util/memory/excl_ptr.h"
#include "util/packed_array.h"		// for alias_collection_type

extern
int
instref_parse(void*, YYSTYPE&, flex::lexer_state&);

namespace HAC {
namespace parser {
using entity::bool_tag;
using entity::state_manager;
using entity::unroll_context;
using entity::expr_dump_context;
using entity::module;
using entity::footprint;
using entity::simple_bool_meta_instance_reference;
using entity::simple_process_meta_instance_reference;
using entity::substructure_alias;
using entity::entry_collection;
using entity::index_set_type;
using entity::global_indexed_reference;
using entity::META_TYPE_NONE;
using std::vector;
using std::copy;
using std::string;
using std::ostream_iterator;
using util::string_list;
using util::memory::excl_ptr;
using util::memory::never_ptr;
#include "util/using_ostream.h"
//=============================================================================
/**
	Parses an instance-reference string, and returns an AST slice.
	\throw general exception if unable to write temporary file.  
	\return AST of reference, else NULL upon error.  
 */
excl_ptr<parser::inst_ref_expr>
parse_reference(const char* s) {
	typedef	excl_ptr<parser::inst_ref_expr>	return_type;
	STACKTRACE_VERBOSE;
	NEVER_NULL(s);
	YYSTYPE lval;
	try {
		flex::lexer_state f(s);	// can now pass string to parse!
		instref_parse(NULL, lval, f);
	} catch (...) {
		cerr << "Error parsing instance name: " << s << endl;
		return return_type(NULL);;
	}
	// cerr << "parsed node name successfully... " << endl;
	// here is our mini-parse-tree:
	return return_type(lval._inst_ref_expr);
}

//=============================================================================
/**
	NOTE: parse::context can only accept a modifiable module&
		as a ctor argument, but we REQUIRE that the 
		module we pass be unscathed (const).
		To help enforce constness in this case, 
		we restrict the constructed context to be const
		which promises not to modify its internal 
		reference to the module.  Thus from this point
		we are restricted to using the context const-ly.  
	\return resolved IR of reference.  
 */
entity::meta_reference_union
check_reference(const parser::inst_ref_expr& ref_tree,
		const entity::module& m) {
	typedef	entity::meta_reference_union		return_type;
	STACKTRACE_VERBOSE;
/***
	And now for a slice of compiler pie:
	Oh by the way, once we enable non-const expressions
	in indices, this will automatically support 
	meta-expression evaluation using the values present in
	the module.  Yeah, baby.  
	Passing "true" as the 2nd arg says we want all names 
	publicly visible, see AST::parser::context::view_all_publicly.
***/
	const context c(m, parse_options(), true);
	return_type r;
	try {
		// NOTE: this checks for PUBLIC members only
		// but we should allow PRIVATE references too!
		// perhaps flag through context?
		r = ref_tree.check_meta_reference(c);
	} catch (...) {
		// temporary have shitty error-handling...
		// already have type-check error message
		return return_type();
	}
	if (!r) {
		// don't expect this message to ever happen...
		cerr << "Some other error type-checking..." << endl;
		return return_type();
	}
	return r;
}

//=============================================================================
/** 
	Composition of parse_reference and check_reference.  
	Some error message already given.  
 */
entity::meta_reference_union
parse_and_check_reference(const char* s, const module& m) {
	typedef	entity::meta_reference_union		return_type;
	typedef	excl_ptr<parser::inst_ref_expr>		lval_ptr_type;
	STACKTRACE_VERBOSE;
	const lval_ptr_type ref_tree = parse_reference(s);
	if (!ref_tree) {
		return return_type();
	}
	return check_reference(*ref_tree, m);
}

//=============================================================================
/**
	Finds the allocated index for a bool (node).  
	TODO: be able to cache already checked references with a hash.  
		Could be memory expensive?
	\param n the string that names the instance reference
	\param m the compiled module with the top-level namespace
		and allocated state_manager.  
 */
size_t
parse_node_to_index(const string& n, const module& m) {
	typedef	inst_ref_expr::meta_return_type		checked_ref_type;
#ifndef	INVALID_NODE_INDEX
#define	INVALID_NODE_INDEX	0
#endif
	STACKTRACE_VERBOSE;
	const checked_ref_type r(parse_and_check_reference(n.c_str(), m));
	if (!r) {
		return INVALID_NODE_INDEX;
	}
	typedef	simple_bool_meta_instance_reference	bool_ref_type;
	const count_ptr<const bool_ref_type>
		b(r.inst_ref().is_a<const bool_ref_type>());
	if (!b) {
		// later: write another procedure
		// to print *collections* of bools
		// by prefix matching.
		cerr << "Error: " << n << " does not reference a bool (node)."
			<< endl;
		return INVALID_NODE_INDEX;
	}
	// reminder: this is a packed_array_generic
	// this code uses the allocation information from the 
	// alloc phase to find the canonical ID number.  
	const state_manager& sm(m.get_state_manager());
	const footprint& top(m.get_footprint());
	const size_t ret = b->lookup_globally_allocated_index(sm, top);
#if 0
	cerr << "index = " << ret << endl;
#endif
	return ret;
}

//=============================================================================
/**
	\return globally allocated index of a named process, 
		which can be 0 to reference the top-level process.
 */
size_t
parse_process_to_index(const string& n, const module& m) {
	typedef	inst_ref_expr::meta_return_type		checked_ref_type;
	STACKTRACE_VERBOSE;
if (n == ".") {
	// refers to the top-level process
	return 0;
} else {
#define	INVALID_PROCESS_INDEX			size_t(-1)
	const checked_ref_type r(parse_and_check_reference(n.c_str(), m));
	if (!r) {
		return INVALID_PROCESS_INDEX;
	}
	typedef	simple_process_meta_instance_reference	proc_ref_type;
	const count_ptr<const proc_ref_type>
		b(r.inst_ref().is_a<const proc_ref_type>());
	if (!b) {
		cerr << "Error: " << n << " does not reference a process."
			<< endl;
		return INVALID_PROCESS_INDEX;
	}
	// reminder: this is a packed_array_generic
	// this code uses the allocation information from the 
	// alloc phase to find the canonical ID number.  
	const state_manager& sm(m.get_state_manager());
	const footprint& top(m.get_footprint());
	const size_t ret = b->lookup_globally_allocated_index(sm, top);
	return ret;
}
#undef	INVALID_PROCESS_INDEX
}

//=============================================================================
/**
	\returns a (type, index)-pair that references the globally
	allocated index.  
	TODO: handle meta value references?
 */
global_indexed_reference
parse_global_reference(const string& n, const module& m) {
	typedef	inst_ref_expr::meta_return_type		checked_ref_type;
	STACKTRACE_VERBOSE;
	const checked_ref_type r(parse_and_check_reference(n.c_str(), m));
	if (!r.inst_ref()) {
		return global_indexed_reference(META_TYPE_NONE, 
			INVALID_NODE_INDEX);
	}
	const state_manager& sm(m.get_state_manager());
	const footprint& top(m.get_footprint());
	// is a meta_instance_reference_base
	return r.inst_ref()->lookup_top_level_reference(sm, top);
}

//=============================================================================
/**
	Prints reference identity information. 
	TODO: check non-instance-references:
		namespaces, definitions, typedefs, value-references.
	\return 0 upon success, 1 upon error.  
 */
int
parse_name_to_what(ostream& o, const string& n, const module& m) {
	typedef	inst_ref_expr::meta_return_type		checked_ref_type;
	STACKTRACE_VERBOSE;
	const checked_ref_type r(parse_and_check_reference(n.c_str(), m));
	if (!r) {
		return 1;
	} else {
		o << n << " refers to ";
		r.inst_ref()->what(o) << " ";
		r.inst_ref()->dump_type_size(o) << endl;
		return 0;
	}
}

//=============================================================================
/**
	Prints list of members of the definition of the referenced type.
	TODO: currently uses pre-instantiated definition's members, 
	which may include conditionally instantiated members.  
	\return 0 upon success, 1 upon error.  
 */
int
parse_name_to_members(ostream& o, const string& n, const module& m) {
	typedef	inst_ref_expr::meta_return_type		checked_ref_type;
	STACKTRACE_VERBOSE;
	// scopespace::const_map_iterator i, e;
if (n == ".") {
	// special designator for top-level
	o << "top-level instances: " << endl;
	m.get_global_namespace()->dump_for_definitions(o);
} else {
	const checked_ref_type r(parse_and_check_reference(n.c_str(), m));
	if (!r || !r.inst_ref()) {
		return 1;
	} else if (r.inst_ref()->dimensions()) {
		o << "Error: referenced instance must be a single (scalar)."
			<< endl;
		return 1;
	} else {
		o << n << " (type: ";
		r.inst_ref()->dump_type_size(o) << ") has members: " << endl;
		const never_ptr<const definition_base>
			def(r.inst_ref()->get_base_def());
		NEVER_NULL(def);
		const never_ptr<const scopespace>
			mscope(def->get_scopespace());
		// mscope->dump_instance_members(o) << endl;
		mscope->dump_for_definitions(o);
	}
}
#if 0
	for ( ; i!=e; ++i) {
	const never_ptr<const physical_instance_placeholder>
		p(i->second.is_a<const physical_instance_placeholder>());
	if (p) {
		o << '\t' << i->first << endl;
		// get_unresolved_type_ref
		// get_base_def
	}
	}
#endif
	return 0;
}

//=============================================================================
/**
	Accumlates a sequence of sub-nodes reachable from instance.  
	\return 0 upon success, 1 upon error.  
 */
int
parse_name_to_get_subnodes(ostream& o, const string& n, const module& m, 
		vector<size_t>& v) {
	typedef	inst_ref_expr::meta_return_type		checked_ref_type;
	STACKTRACE_VERBOSE;
	const checked_ref_type r(parse_and_check_reference(n.c_str(), m));
	if (!r || !r.inst_ref()) {
		return 1;
#if 0
	} else if (r.inst_ref()->dimensions()) {
		o << "Error: referenced instance must be a single (scalar)."
			<< endl;
		return 1;
#endif
	// TODO: allow non-scalar collections, sloppy arrays, etc...
	} else {
		entry_collection e;
		r.inst_ref()->collect_subentries(m, e);
		const index_set_type& b(e.get_index_set<bool_tag>());
		v.resize(b.size());
		copy(b.begin(), b.end(), v.begin());
		return 0;
	}
}

//=============================================================================
/**
	Prints reference identity information. 
	TODO: check non-instance-references:
		namespaces, definitions, typedefs, value-references.
	\param list separator
	\return 0 upon success, 1 upon error.  
 */
int
parse_name_to_aliases(ostream& o, const string& n, const module& m, 
		const char* _sep) {
	typedef	inst_ref_expr::meta_return_type		checked_ref_type;
	STACKTRACE_VERBOSE;
	const char* sep = _sep ? _sep : " ";
	NEVER_NULL(sep);
	const checked_ref_type r(parse_and_check_reference(n.c_str(), m));
	if (!r || !r.inst_ref()) {
		return 1;
	} else if (r.inst_ref()->dimensions()) {
		o << "Error: referenced instance must be a single (scalar)."
			<< endl;
		return 1;
	} else {
		string_list aliases;
		r.inst_ref()->collect_aliases(m, aliases);
		ostream_iterator<string> osi(o, sep);
		copy(aliases.begin(), aliases.end(), osi);
		return 0;
	}
}

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

