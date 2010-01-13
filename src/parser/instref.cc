/**
	\file "parser/instref.cc"
	$Id: instref.cc,v 1.19.2.2 2010/01/13 17:43:42 fang Exp $
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
#include "Object/global_entry.h"
#include "Object/global_entry_context.h"
// #include "Object/common/dump_flags.h"
#include "Object/common/namespace.h"
#include "Object/unroll/unroll_context.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/proc_traits.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/alias_actuals.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/instance_placeholder_base.h"
#include "Object/inst/bool_port_collector.tcc"
#include "Object/ref/meta_reference_union.h"
#include "Object/traits/type_tag_enum.h"
#include "Object/entry_collection.h"
#include "common/TODO.h"
#include "util/tokenize_fwd.h"		// for string_list
#include "util/value_saver.h"
#include "util/directory.h"
#include "util/memory/excl_ptr.h"
#include "util/packed_array.h"		// for alias_collection_type
#include "util/member_select.h"
#include "util/copy_if.h"		// for transform_if algo
#include "util/iterator_more.h"		// for set_inserter

extern
int
instref_parse(void*, YYSTYPE&, flex::lexer_state&);

namespace HAC {
namespace parser {
using entity::bool_tag;
using entity::process_tag;
using entity::global_entry_pool;
using entity::footprint_frame_map_type;
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
using std::bind1st;
using util::value_saver;
using util::string_list;
using util::memory::excl_ptr;
using util::memory::never_ptr;
using util::directory_stack;
#include "util/using_ostream.h"
//=============================================================================
#if 0
/**
	Global variable used by parse_reference.
	When set, preprocess the reference string using the directory stack
	before parsing.
 */
const directory_stack* dir_stack = NULL;
#endif

//=============================================================================
/**
	Parses an instance-reference string, and returns an AST slice.
	\throw general exception if unable to write temporary file.  
	\return AST of reference, else NULL upon error.  
	TODO: let this handle directory-like references, for the sake of
		directory tab-completion.
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
		// this only checks that reference is syntactically well-formed
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
	const footprint& top(m.get_footprint());
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	const size_t ret = b->lookup_globally_allocated_index(top);
#else
	const state_manager& sm(m.get_state_manager());
	const size_t ret = b->lookup_globally_allocated_index(sm, top);
#endif
#if 0
	cerr << "index = " << ret << endl;
#endif
	return ret;
}

//=============================================================================
/**
	\return globally allocated index of a named process, 
		which can be 0 to reference the top-level process.
		-1 signals an error.
 */
size_t
parse_process_to_index(const string& n, const module& m) {
	typedef	inst_ref_expr::meta_return_type		checked_ref_type;
	STACKTRACE_VERBOSE;
if (n == ".") {
	// refers to the top-level process
	return 0;
} else {
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
	const footprint& top(m.get_footprint());
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	const size_t ret = b->lookup_globally_allocated_index(top);
#else
	const state_manager& sm(m.get_state_manager());
	const size_t ret = b->lookup_globally_allocated_index(sm, top);
#endif
	return ret;
}
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
	const footprint& top(m.get_footprint());
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	return r.inst_ref()->lookup_top_level_reference(top);
#else
	const state_manager& sm(m.get_state_manager());
	// r.inst_ref() is a meta_instance_reference_base
	return r.inst_ref()->lookup_top_level_reference(sm, top);
#endif
}

//=============================================================================
/**
	Convenience function.
	Maybe make this public...
 */
static
const footprint*
get_process_footprint(const size_t pid, const module& m) {
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	FINISH_ME_EXIT(Fang);
	// this should be easy to implement with hierarchy!
	return NULL;
#else
	const state_manager& sm(m.get_state_manager());
	const global_entry_pool<process_tag>&
		proc_pool(sm.get_pool<process_tag>());
	// get process_instance_alias
	const entity::global_entry<process_tag>& pe(proc_pool[pid]);
	const footprint* f = pe._frame._footprint;
	NEVER_NULL(f);
	return f;
#endif
}

//=============================================================================
/**
	Prints reference identity information. 
	Currently only works for instance references.
	TODO: check non-instance-references:
		namespaces, definitions, typedefs, value-references.
	TODO: print template parameters of complete type
	Currently footprint lacks direct back-reference to base definition.
	\return 0 upon success, 1 upon error.  
 */
int
parse_name_to_what(ostream& o, const string& n, const module& m) {
	typedef	inst_ref_expr::meta_return_type		checked_ref_type;
	STACKTRACE_VERBOSE;
	const checked_ref_type r(parse_and_check_reference(n.c_str(), m));
	const count_ptr<const entity::meta_instance_reference_base>&
		mr(r.inst_ref());
	if (!r || !mr) {
		return 1;
	}
	const size_t dim = mr->dimensions();
	// handle arrays first
	if (dim) {
		o << n << " refers to ";
		// r.inst_ref() is a meta_instance_reference_base
		mr->what(o) << " ";
		mr->dump_type_size(o) << endl;
		return 0;
	}
	// else is scalar
	const global_indexed_reference gref(parse_global_reference(n, m));
	// wasteful to parse again, I know...
	if (!gref.second) {
		o << "Error resolving instance reference: " << n << endl;
		return 1;
	}
	o << n << " refers to ";
	mr->what(o) << " ";
	switch (gref.first) {
	// for now, only processes have footprints
	case entity::META_TYPE_PROCESS: {
		get_process_footprint(gref.second, m)->dump_type(o) << endl;
		break;
	}
	default:
		mr->dump_type_size(o) << endl;
	}
	return 0;
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
	// TODO: use module's top_footprint
} else {
	const checked_ref_type r(parse_and_check_reference(n.c_str(), m));
	if (!r || !r.inst_ref()) {
		return 1;
	} else if (r.inst_ref()->dimensions()) {
		o << "Error: referenced instance must be a single (scalar)."
			<< endl;
		return 1;
	} else {
		// check for valid reference first
		const global_indexed_reference
			gref(r.inst_ref()->lookup_top_level_reference(
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
				m.get_state_manager(), 
#endif
				m.get_footprint()));
		if (!gref.second) {
			o << "Error resolving instance reference: "
				<< n << endl;
			return 1;
		}
		const footprint* f = NULL;
		o << n << " (type: ";
		switch (gref.first) {
		case entity::META_TYPE_PROCESS:
			f = get_process_footprint(gref.second, m);
			f->dump_type(o);
			break;
		default:
			r.inst_ref()->dump_type_size(o);
		}
		o << ") has members: " << endl;
	if (f) {
		f->dump_member_list(o);
	} else {
		const never_ptr<const definition_base>
			def(r.inst_ref()->get_base_def());
		NEVER_NULL(def);
		const never_ptr<const scopespace>
			mscope(def->get_scopespace());
		// mscope->dump_instance_members(o) << endl;
		mscope->dump_for_definitions(o);
	}
	}
}
	return 0;
}

//=============================================================================
/**
	Accumlates a sequence of sub-nodes reachable from instance.  
	\return 0 upon success, 1 upon error.  
	TODO: can't we just work-list traverse processes and 
	their bool members from their footprint frames?
 */
int
parse_name_to_get_subnodes(const string& n, const module& m, 
		vector<size_t>& v) {
	typedef	inst_ref_expr::meta_return_type		checked_ref_type;
	STACKTRACE_VERBOSE;
if (n == ".") {
	// no lookup necessary, just copy all integers!
	const size_t bmax =
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
		m.get_footprint().get_instance_pool<bool_tag>().total_entries();
#else
		m.get_state_manager().get_pool<bool_tag>().size();
#endif
	size_t i = INVALID_NODE_INDEX +1;
	v.reserve(bmax -1);
	for ( ; i<bmax; ++i) {
		v.push_back(i);
	}
	return 0;
} else {
	const checked_ref_type r(parse_and_check_reference(n.c_str(), m));
	if (!r || !r.inst_ref()) {
		return 1;
#if 0
	} else if (r.inst_ref()->dimensions()) {
		cerr << "Error: referenced instance must be a single (scalar)."
			<< endl;
		return 1;
#endif
	// TODO: allow non-scalar collections, sloppy arrays, etc...
	} else {
		entry_collection e;
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
		// much easier with continuous ranges in memory mapping
		const global_indexed_reference
			gref(parse_global_reference(n, m));
		if (!gref.first) {
			return 1;
		}
		if (!m.get_footprint().collect_subentries(gref, e).good) {
			return 1;
		}
#else
		if (!r.inst_ref()->collect_subentries(m, e).good) {
			return 1;
		}
#endif
		const index_set_type& b(e.get_index_set<bool_tag>());
		v.resize(b.size());
		copy(b.begin(), b.end(), v.begin());
		return 0;
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Same as parse_name_to_get_subnodes, but non-recursive, 
	only returns bools reachable through public ports.  
	Referenced instance must be a process.  
	\return 0 upon success, 1 upon error.  
 */
int
parse_name_to_get_subnodes_local(const string& n, const module& m, 
		vector<size_t>& v) {
	typedef	inst_ref_expr::meta_return_type		checked_ref_type;
	STACKTRACE_VERBOSE;
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	FINISH_ME_EXIT(Fang);
	return 1;
#else
	const size_t pid = parse_process_to_index(n, m);
	if (pid == INVALID_PROCESS_INDEX) {
		return 1;
	}
	const footprint_frame_map_type&
		pbf(m.get_state_manager().get_bool_frame_map(pid));
	// unique sort it
	std::set<size_t> s;
	if (pid) {
		copy(pbf.begin(), pbf.end(), util::set_inserter(s));
	} else {	// top-level process needs transformation, yuck...
		// FIXME: this is very BUG prone
		copy(++pbf.begin(), pbf.end(), util::set_inserter(s));
		// skip the 0th entry
	}
	copy(s.begin(), s.end(), back_inserter(v));
	return 0;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns the subset of local_subnodes that are port aliases.
	Caller may not use the port-name however, due to choice of
	canonical names.  
	TODO?: rewrite this once footprints have back-references
		to their base definitions, and can access their
		port formals.  
	\return 0 upon success, 1 upon error.  
 */
int
parse_name_to_get_ports(const string& n, const module& m, 
		vector<size_t>& v) {
	typedef	inst_ref_expr::meta_return_type		checked_ref_type;
	STACKTRACE_VERBOSE;
	const size_t pid = parse_process_to_index(n, m);
	if (pid == INVALID_PROCESS_INDEX) {
		return 1;
	} else if (!pid) {
		// top-level process has no ports!
		return 0;
	}
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	FINISH_ME_EXIT(Fang);
	return 1;
#else
	const state_manager& sm(m.get_state_manager());
	const footprint& topfp(m.get_footprint());
	const entity::global_entry_context_base gec(sm, topfp);
	const global_entry_pool<process_tag>&
		proc_pool(sm.get_pool<process_tag>());
	// get process_instance_alias
	const entity::global_entry<process_tag>& pe(proc_pool[pid]);
	const entity::state_instance<process_tag>&
		p(pe.get_canonical_instance(gec));
	const entity::instance_alias_info<process_tag>&
		pi(*p.get_back_ref());
	// get subinstances, and their local indices
	entity::bool_port_collector<std::set<size_t> > C;
	pi.accept(C);
#if 0
	pi.dump_ports(cout, entity::dump_flags::default_value) << endl;
	copy(C.bool_indices.begin(), C.bool_indices.end(), 
		ostream_iterator<size_t>(cout, ","));
	cout << endl;
#endif
	if (pe.parent_tag_value == entity::META_TYPE_NONE) {
		// then parent is top-level no transformation necessary
		copy(C.bool_indices.begin(), C.bool_indices.end(),
			back_inserter(v));
	} else {
		// we need to translate local to global indices
		INVARIANT(pe.parent_tag_value == entity::META_TYPE_PROCESS);
		const footprint_frame_map_type&
			pbf(sm.get_bool_frame_map(pe.parent_id));
		transform(C.bool_indices.begin(), C.bool_indices.end(), 
			back_inserter(v),
			entity::footprint_frame_transformer(pbf));
	}
	// can't handle non-processes yet :S
#if 0
	copy(v.begin(), v.end(), ostream_iterator<size_t>(cout, ","));
	cout << endl;
#endif
#endif
	return 0;
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
#define	DEBUG_COMPLETION		0
static
size_t
last_separator(const string& orig) {
	const size_t ld = orig.find_last_of('.');
	const size_t ls = orig.find_last_of('/');
	size_t cut;	// position of last separator
	if (ld == string::npos) {
		cut = ls;
	} else if (ls != string::npos) {
		cut = std::max(ld, ls);
	} else {
		cut = ld;
	}
#if DEBUG_COMPLETION
	cout << "<ld:" << ld << ",ls:" << ls << ",cut:" << cut << '>';
#endif
	return cut;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param _text is the partial token of text to be completed,
		this text should be preserved on the line in the 
		returned strings.
	\param matches is the return array, should start empty, but need not be
	\returns an array of candidate strings for instances.
	Now returns only names of instances that were instantiated!
 */
void
complete_instance_names(const char* _text, const module& m, 
		const directory_stack* d, 
		vector<string>& matches) {
	typedef	scopespace::const_map_iterator	const_iterator;
	STACKTRACE_VERBOSE;
	// to alter parse_reference
	const string orig(_text);	// unaltered text
	const size_t cut = last_separator(orig);
	string root, child;
	if (cut != string::npos) {
		root = orig.substr(0, cut +1);	// text to preserve
		child = orig.substr(cut +1);
	} else {	// else leave blank
		child = orig;
	}
#if DEBUG_COMPLETION
	cout << "<root:" << root << ",child:" << child << '>';
#endif
	// equivalent logical canonical path, after directory transformation
	const string text(d ? d->transform(orig) : orig);
#if DEBUG_COMPLETION
	cout << "<orig:" << orig << ",equiv:" << text << '>';
#endif
	const footprint* f = NULL;
	// extract parent from text
	// if text is blank, return items from "ls ." (top-level)
	const string parent(d ? d->transform(root) : root);
#if DEBUG_COMPLETION
	cout << "<parent:" << parent << '>';
#endif
// does canonical string contain a '.'?  If so, cut text into two parts.
// else need to do partial parsing for context
	if (!parent.length()) {
		f = &m.get_footprint();
	} else {			// split up string
		typedef	inst_ref_expr::meta_return_type	checked_ref_type;
		// parse the parent to get context
		const checked_ref_type
			r(parse_and_check_reference(parent.c_str(), m));
		if (!r || !r.inst_ref()) { return; }
			// no error message
		if (r.inst_ref()->dimensions()) { return; }
			// no error message
		const global_indexed_reference
			gref(r.inst_ref()->lookup_top_level_reference(
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
				m.get_state_manager(), 
#endif
				m.get_footprint()));
		if (!gref.second) { return; }
		if (gref.first != entity::META_TYPE_PROCESS) { return; }
		// until non-process types have subinstances...
		f = get_process_footprint(gref.second, m);
	}
	vector<string> temp;
	f->export_instance_names(temp);
	// filter out with child string
	// TODO: handle array indices
	// may need to prepend parent '.' to matched names...
	
//	const string root(parent.size() ? parent + '.' : "");
	// caller function will take care of custom display
	// function to omit the common prefix in the match display.
	if (child.size()) {
		// use child string to bound range
		// INVARIANT: temp is already sorted because it
		// came from a sorted structure
		string child2(child);
		++child2[child2.length() -1];
		const vector<string>::const_iterator
			f(lower_bound(temp.begin(), temp.end(), child)),
			l(lower_bound(temp.begin(), temp.end(), child2));
		transform(f, l, back_inserter(matches), 
			bind1st(std::plus<string>(), root));
	} else {
		// all matches
		transform(temp.begin(), temp.end(), back_inserter(matches), 
			bind1st(std::plus<string>(), root));
	}
#if DEBUG_COMPLETION
	cout << "<MATCHES:";
	copy(matches.begin(), matches.end(),
		std::ostream_iterator<string>(cout, "|"));
	cout << '>';
#endif
}

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

