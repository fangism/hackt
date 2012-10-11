/**
	\file "parser/instref.cc"
	$Id: instref.cc,v 1.29 2011/03/14 20:33:21 fang Exp $
 */

#define	DEBUGGING_SHIT			0
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
#include "Object/global_channel_entry.h"
#include "Object/global_entry_context.h"
// #include "Object/common/dump_flags.h"
#include "Object/common/namespace.h"
#include "Object/unroll/unroll_context.h"
#include "Object/traits/instance_traits.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/alias_actuals.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/instance_placeholder_base.h"
#if DEBUGGING_SHIT
#include "Object/inst/datatype_instance_placeholder.h" // DEBUG ONLY
#include "Object/inst/instance_placeholder.h" // DEBUG ONLY
#include "util/memory/count_ptr.tcc"
#endif
#include "Object/inst/bool_port_collector.tcc"
#include "Object/inst/state_instance.h"
#include "Object/inst/instance_pool.h"
#include "Object/ref/meta_reference_union.h"
#include "Object/traits/type_tag_enum.h"
#include "Object/entry_collection.h"
#include "common/TODO.h"
#include "util/tokenize_fwd.h"		// for string_list
#include "util/value_saver.h"
#include "util/directory.h"
#include "util/memory/count_ptr.tcc"
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
using entity::int_tag;
using entity::enum_tag;
using entity::channel_tag;
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
using entity::global_reference_array_type;
using entity::META_TYPE_NONE;
using entity::META_TYPE_PROCESS;
using entity::META_TYPE_INT;
using entity::META_TYPE_ENUM;
using entity::META_TYPE_CHANNEL;
using entity::META_TYPE_BOOL;
using entity::footprint_frame;
using entity::global_entry_context;
using entity::global_offset;
using entity::global_offset_base;
using entity::global_process_context;
using std::set;
using std::vector;
using std::copy;
using std::string;
using std::ostream_iterator;
using std::bind1st;
using util::set_inserter;
using util::value_saver;
using util::string_list;
using util::memory::excl_ptr;
using util::memory::never_ptr;
using util::directory_stack;
#include "util/using_ostream.h"

typedef	inst_ref_expr::meta_return_type		checked_ref_type;
//=============================================================================
// class typed_indexed_reference method definitions

#ifndef	INVALID_NODE_INDEX
#define	INVALID_NODE_INDEX	0
#endif

template <class Tag>
typed_indexed_reference<Tag>::typed_indexed_reference(
		const string& n, const entity::module& m) {
	STACKTRACE_VERBOSE;
	typedef	entity::class_traits<Tag>		traits_type;
	const global_indexed_reference gref(parse_global_reference(n, m));
	if (gref.first != traits_type::type_tag_enum_value) {
#if 1
		cerr << "Error: " << n << " does not reference a " <<
			traits_type::tag_name << "." << endl;
#else
		// silent error, caller catches
#endif
		index = INVALID_NODE_INDEX;	// 0
	} else {
		index = gref.second;
	}
}

template struct typed_indexed_reference<bool_tag>;
template struct typed_indexed_reference<int_tag>;
template struct typed_indexed_reference<enum_tag>;
template struct typed_indexed_reference<channel_tag>;
template struct typed_indexed_reference<process_tag>;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	If the result indices array is empty, that signals an error.
 */
template <class Tag>
typed_indexed_references<Tag>::typed_indexed_references(
		const string& n, const entity::module& m) {
	STACKTRACE_VERBOSE;
	typedef	entity::class_traits<Tag>		traits_type;
#if AGGREGATE_PARENT_REFS
	global_reference_array_type temp;
	if (parse_global_references(n, m, temp)) {
		cerr << "Error parsing reference(s): " << n << endl;
		return;
	}
	if (temp.front().first != traits_type::type_tag_enum_value) {
		cerr << "Error: " << n << " does not reference a " <<
			traits_type::tag_name << "." << endl;
		return;
	}
	// TODO: copy indices
	indices.resize(temp.size());
	transform(temp.begin(), temp.end(), indices.begin(), 
		util::member_select_ref(&global_indexed_reference::second));
#else
	expanded_global_references_type temp;
	if (expand_global_references(n, m, temp))
		return;
	indices.reserve(temp.size());
	expanded_global_references_type::const_iterator
		i(temp.begin()), e(temp.end());
for ( ; i!=e; ++i) {
	const global_indexed_reference& gref(i->second);
	if (gref.first != traits_type::type_tag_enum_value) {
#if 1
		// really should use expanded name
		cerr << "Error: " << n << " does not reference a " <<
			traits_type::tag_name << "." << endl;
#else
		// silent error, caller catches
#endif
		indices.clear();
		return;
	}
	INVARIANT(gref.second);
	indices.push_back(gref.second);
}	// end for each reference
#endif
}

template struct typed_indexed_references<bool_tag>;
template struct typed_indexed_references<int_tag>;
template struct typed_indexed_references<enum_tag>;
template struct typed_indexed_references<channel_tag>;
template struct typed_indexed_references<process_tag>;

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
count_ptr<inst_ref_expr>
parse_reference(const char* s) {
	typedef	count_ptr<inst_ref_expr>	return_type;
	STACKTRACE_VERBOSE;
	NEVER_NULL(s);
	STACKTRACE_INDENT_PRINT("Parsing: " << s << endl);
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

//-----------------------------------------------------------------------------
/**
	Expands array and ranged references.  
 */
extern
int
expand_reference(const count_ptr<const inst_ref_expr>& r, 
	reference_array_type& a) {
	NEVER_NULL(r);
	return r->expand_const_reference(r, a);
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
meta_reference_union
check_reference(const inst_ref_expr& ref_tree,
		const entity::module& m) {
	typedef	meta_reference_union		return_type;
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
meta_reference_union
parse_and_check_reference(const char* s, const module& m) {
	typedef	meta_reference_union		return_type;
	typedef	count_ptr<inst_ref_expr>	lval_ptr_type;
	STACKTRACE_VERBOSE;
	const lval_ptr_type ref_tree = parse_reference(s);
	if (!ref_tree) {
		return return_type();
	}
	return check_reference(*ref_tree, m);
}

//=============================================================================
/**
	Expands to an array of checked references.
	TODO: don't flatten into massive array of individual references,
	leverage repeated structure to minimize calls. 
	Implement interfaces using aggregate references.  
	\return true on error.
 */
bool
expand_global_references(const string& _base, const module& m, 
		expanded_global_references_type& ret) {
	STACKTRACE_VERBOSE;
	// we have to expand this the hard way because we need
	// the original expanded reference names.
	const AST_reference_ptr r(parse_reference(_base.c_str()));
	if (!r) {
		cerr << "Error in instance reference." << endl;
		return true;
	}
	reference_array_type st_refs;
	if (expand_reference(r, st_refs)) {
		cerr << "Error in expanding instance reference." << endl;
		return true;
	}
	ret.reserve(st_refs.size());
	reference_array_type::const_iterator
		ri(st_refs.begin()), re(st_refs.end());
for ( ; ri!=re; ++ri) {
	NEVER_NULL(*ri);
	const meta_reference_union cr(check_reference(**ri, m));
	if (!cr) {
		cerr << "Error in instance reference." << endl;
		return true;
	}
	const global_indexed_reference
		g(parse_global_reference(cr, m));
	ret.push_back(expanded_global_reference(*ri, g));
}	// end for all expanded references
	return false;
}	// end expand_global_references

//=============================================================================
/**
	Finds the allocated index for a bool (node).  
	TODO: be able to cache already checked references with a hash.  
		Could be memory expensive?
	\param n the string that names the instance reference
	\param m the compiled module with the top-level namespace
		and allocated state_manager.  
	\return 1-based global bool index, 0 if not found.
 */
bool_index
parse_node_to_index(const string& n, const module& m) {
	STACKTRACE_VERBOSE;
	return bool_index(n, m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true on error
 */
bool
parse_nodes_to_indices(const string& n, const module& m,
		vector<size_t>& b) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("Parsing node(s): " << n << endl);
	bool_indices tmp(n, m);
	tmp.indices.swap(b);
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("bool-ids: ");
	copy(b.begin(), b.end(), std::ostream_iterator<size_t>(cerr, ","));
	cerr << endl;
#endif
	return b.empty();
}

//=============================================================================
/**
	\return globally allocated index of a named process, 
		which can be 0 to reference the top-level process.
		-1 signals an error.
 */
process_index
parse_process_to_index(const string& n, const module& m) {
	STACKTRACE_VERBOSE;
if (n == ".") {
	// refers to the top-level process
	return process_index(0);
} else {
	const process_index r(n, m);
	if (!r.index)	// error
		return process_index();	// -1
	// because 0 indicates top-level process
	else	return r;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true on error
 */
bool
parse_processes_to_indices(const string& n, const module& m, 
		vector<size_t>& p) {
	STACKTRACE_VERBOSE;
if (n == ".") {
	// refers to the top-level process
	p.push_back(0);
	return false;
} else {
	process_indices tmp(n, m);
	tmp.indices.swap(p);
	return p.empty();
}
}

//=============================================================================
static
bool
must_be_scalar_inst(const checked_ref_type& r, ostream* o) {
	if (!r.inst_ref()) {
		return false;
	} else if (r.inst_ref()->dimensions()) {
	if (o) {
		*o << "Error: referenced instance must be a single (scalar)."
			<< endl;
	}
		return false;
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\returns a (type, index)-pair that references the globally
	allocated index.  
	Optional error stream allows suppression of diagnostics.  
	TODO: handle meta value references?
 */
global_indexed_reference
parse_global_reference(const string& n, const module& m, ostream* o) {
	STACKTRACE_VERBOSE;
	static const global_indexed_reference
		err(META_TYPE_NONE, INVALID_NODE_INDEX);
	const checked_ref_type r(parse_and_check_reference(n.c_str(), m));
	if (!must_be_scalar_inst(r, o)) { return err; }
	return parse_global_reference(r, m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default to passing cerr stream.
 */
global_indexed_reference
parse_global_reference(const string& n, const module& m) {
	return parse_global_reference(n, m, &std::cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
global_indexed_reference
parse_global_reference(const meta_reference_union& r, const module& m) {
	INVARIANT(r.inst_ref());
	const footprint& topfp(m.get_footprint());
	global_process_context gpc(topfp);
	gpc.construct_top_global_context();
	const global_entry_context gc(gpc);
	return r.inst_ref()->lookup_top_level_reference(gc);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
parse_global_references(const string& n, 
		const module& m, global_reference_array_type& a) {
	STACKTRACE_VERBOSE;
	const checked_ref_type r(parse_and_check_reference(n.c_str(), m));
	if (!r.inst_ref()) {
		return 1;
	}
	// allow array references
	return parse_global_references(r, m, a);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Parses an aggregate reference into a collection.
	\return non-zero on error
 */
int
parse_global_references(const meta_reference_union& r, 
		const module& m, global_reference_array_type& a) {
	STACKTRACE_VERBOSE;
	INVARIANT(r.inst_ref());
	const footprint& topfp(m.get_footprint());
	global_process_context gpc(topfp);
	gpc.construct_top_global_context();
	const global_entry_context gc(gpc);
	const good_bool b(r.inst_ref()->lookup_top_level_references(gc, a));
	return (b.good && a.size()) ? 0 : 1;
}

//=============================================================================
/**
	Convenience function.
	Maybe make this public...
	\param pid 1-based global process index, 0 for top-level.
 */
static
const footprint*
get_process_footprint(const size_t pid, const module& m) {
	typedef	process_tag			Tag;
	const footprint& topfp(m.get_footprint());
if (pid) {
	return topfp.get_instance<Tag>(pid -1)._frame._footprint;
} else {
	// top-level
	return &topfp;
}
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
	// TODO: re-write to avoid double-parsing
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
		STACKTRACE_INDENT_PRINT("gpid = " << gref.second << endl);
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
	STACKTRACE_VERBOSE;
	// scopespace::const_map_iterator i, e;
if (n == ".") {
	// special designator for top-level
	o << "top-level instances: " << endl;
	m.get_global_namespace()->dump_for_definitions(o);
	// TODO: use module's top_footprint
} else {
	const checked_ref_type r(parse_and_check_reference(n.c_str(), m));
	if (!must_be_scalar_inst(r, &cerr)) { return 1; }
	const global_indexed_reference
		gref(parse_global_reference(r, m));
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
	STACKTRACE_VERBOSE;
	entry_collection e;
	const int ret = parse_name_to_get_subinstances(n, m, e);
	if (ret) {
		return ret;
	}
	index_set_type& bools(e.get_index_set<bool_tag>());
	v.reserve(bools.size());
	copy(bools.begin(), bools.end(), back_inserter(v));
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Collects set of all subinstances, bools, channels, etc...
	\return 0 upon success, 1 upon error.  
 */
int
parse_name_to_get_subinstances(const string& n, const module& m, 
		entry_collection& e) {
	STACKTRACE_VERBOSE;
	const footprint& topfp(m.get_footprint());
if (n == ".") {
	// no lookup necessary, just copy all integers!
#define PMAX(Tag)	topfp.get_instance_pool<Tag>().total_entries() +1

#define	ADD_EVERYTHING(Tag)						\
{									\
	const size_t bmax = PMAX(Tag);					\
	size_t i = INVALID_NODE_INDEX +1;				\
	index_set_type& v(e.get_index_set<Tag>());			\
	for ( ; i<bmax; ++i) {						\
		v.insert(i);						\
	}								\
}
	ADD_EVERYTHING(bool_tag)
	ADD_EVERYTHING(int_tag)
	ADD_EVERYTHING(enum_tag)
	ADD_EVERYTHING(channel_tag)
	ADD_EVERYTHING(process_tag)
#undef	ADD_EVERYTHING
#undef	PMAX
	return 0;
} else {
	const checked_ref_type r(parse_and_check_reference(n.c_str(), m));
if (!r || !r.inst_ref()) {
	return 1;
} else if (r.inst_ref()->dimensions()) {
#if 0
// TODO: allow non-scalar collections, sloppy arrays, etc...
	cerr << "Error: referenced instance must be a single (scalar)."
		<< endl;
	return 1;
#else
// TODO: refactor this to make re-usable
	entity::global_reference_array_type tmp;
//	const footprint& topfp(m.get_footprint());
	const global_process_context gpc(m.get_footprint());
	const global_entry_context gc(gpc);
	if (!r.inst_ref()->lookup_top_level_references(gc, tmp).good) {
		cerr << "Error expanding reference array: ";
		r.inst_ref()->dump(cerr, expr_dump_context::default_value);
		cerr << endl;
		return 1;
	}
	// note: is waste to reconstruct top-context everytime, oh well...
	entity::global_reference_array_type::const_iterator
		ti(tmp.begin()), te(tmp.end());
	for ( ; ti!=te; ++ti) {
		// should never error out, really
		if (parse_name_to_get_subinstances(*ti, m, e))
			return 1;
	}
	return 0;
#endif
} else {
	// wasteful double-parsing... TODO: rewrite
	// much easier with continuous ranges in memory mapping
	const global_indexed_reference
		gref(parse_global_reference(n, m));
	if (!gref.second) {
		// there was an error
		cerr << "ERROR: bad instance reference: ";
		r.inst_ref()->dump(cerr, expr_dump_context::default_value);
//		cerr << n;
		cerr << endl;
		return 1;
	}
	return parse_name_to_get_subinstances(gref, m, e);
}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
parse_name_to_get_subinstances(const global_indexed_reference& gref,
		const module& m, entry_collection& e) {
	STACKTRACE_VERBOSE;
//	const footprint& topfp(m.get_footprint());
	switch (gref.first) {
	case META_TYPE_PROCESS: {
		// TODO: factor this out somewhere else for reuse?
		index_set_type worklist;
		// coverage of process ports
		index_set_type& covered(e.get_index_set<process_tag>());
		worklist.insert(gref.second);
	do {
		const index_set_type::iterator next_pi(worklist.begin());
		const size_t gpid = *next_pi;
		worklist.erase(next_pi);
		const std::pair<index_set_type::const_iterator, bool>
			pip(covered.insert(gpid));
	if (pip.second) {
		STACKTRACE_INDENT_PRINT("gpid = " << gpid << endl);
		const global_process_context pc(m, gpid);
		const footprint_frame& tmpf(pc.frame);
		const global_offset& tmpg(pc.offset);
	{
		// visit public process ports separately through worklist
		const footprint_frame_map_type&
			ppf(tmpf.get_frame_map<process_tag>());
		const entity::state_instance<process_tag>::pool_type&
			pp(tmpf._footprint->get_instance_pool<process_tag>());
		copy(ppf.begin(), ppf.begin() +pp.port_entries(), 
			set_inserter(worklist));
	}
		// deep private enumeration just continues past
		// end-of-local indices
		// what if there are no locals!
		// adjust to 1-indexed global index
#define	COLLECT_SUBINSTANCES(Tag)					\
	{								\
		index_set_type& sorted(e.get_index_set<Tag>());		\
		const footprint_frame_map_type&				\
			pbf(tmpf.get_frame_map<Tag>());			\
		copy(pbf.begin(), pbf.end(), set_inserter(sorted));	\
		const entity::state_instance<Tag>::pool_type&		\
			bp(tmpf._footprint->get_instance_pool<Tag>());	\
		const size_t priv = bp.non_local_private_entries();	\
		const size_t locp = bp.local_private_entries();		\
		const size_t b_off =					\
			tmpg.entity::global_offset_base<Tag>::offset +1; \
		STACKTRACE_INDENT_PRINT("b_off = " << b_off << endl);	\
		STACKTRACE_INDENT_PRINT("locp = " << locp << endl);	\
		STACKTRACE_INDENT_PRINT("priv. entries = " << priv << endl); \
		const size_t p_off = b_off +locp;			\
		size_t i = 0;						\
		for ( ; i<priv; ++i) {					\
			sorted.insert(i+p_off);				\
		}							\
	}
	COLLECT_SUBINSTANCES(bool_tag)
	COLLECT_SUBINSTANCES(int_tag)
	COLLECT_SUBINSTANCES(enum_tag)
	COLLECT_SUBINSTANCES(channel_tag)
#undef	COLLECT_SUBINSTANCES
	}	// else was already covered, skip it
	} while (!worklist.empty());
		break;
	}	// end case META_TYPE_PROCESS
	// for everything else, just return self
	case META_TYPE_BOOL:
		e.get_index_set<bool_tag>().insert(gref.second);
		break;
	case META_TYPE_INT:
		e.get_index_set<int_tag>().insert(gref.second);
		break;
	case META_TYPE_ENUM:
		e.get_index_set<enum_tag>().insert(gref.second);
		break;
	case META_TYPE_CHANNEL:
		e.get_index_set<channel_tag>().insert(gref.second);
		break;
	default:
		return 1;
	}	// end switch
	return 0;
}	// end parse_name_to_get_subinstances

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Same as parse_name_to_get_subnodes, but non-recursive, 
	only returns bools reachable through public ports.  
	Referenced instance must be a process.  
	\return 0 upon success, 1 upon error.  
 */
int
parse_name_to_get_subnodes_local(
		const process_index& p,
//		const string& n,
		const module& m, 
		vector<size_t>& v) {
	STACKTRACE_VERBOSE;
	const size_t& pid(p.index);
	if (!p.valid()) {
		return 1;
	}
	const global_process_context pc(m, pid);
	const footprint_frame& tmpf(pc.frame);
//	const global_offset& tmpg(pc.offset);
	const footprint_frame_map_type&
		pbf(tmpf.get_frame_map<bool_tag>());
	// unique sort it
	std::set<size_t> s;
	copy(pbf.begin(), pbf.end(), set_inserter(s));
	copy(s.begin(), s.end(), back_inserter(v));
	return 0;
}	// end parse_name_to_get_subnodes_local

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns the subset of local_subnodes that are port aliases.
	Caller may not use the port-name however, due to choice of
	canonical names.  
	TODO?: rewrite this once footprints have back-references
		to their base definitions, and can access their
		port formals.  
	\param pred (optional) array of predicates that is used to 
		filter out the set of nodes.  
	\param v the return set of global bool IDs of the referenced ports
	\return 0 upon success, 1 upon error.  
 */
int
parse_name_to_get_ports(const process_index& p, 
//		const string& n,
		const module& m, 
		vector<size_t>& v, const vector<bool>* pred) {
	STACKTRACE_VERBOSE;
	const size_t& pid(p.index);
	if (!p.valid()) {
		return 1;
	} else if (!pid) {
		// top-level process has no ports!
		return 0;
		// even when using user-specified top-type?
	}
	const global_process_context pc(m, pid);
	const footprint_frame& tmpf(pc.frame);
//	const global_offset& tmpg(pc.offset);
	const footprint_frame_map_type&
		pbf(tmpf.get_frame_map<bool_tag>());
	const size_t ps = tmpf._footprint->get_instance_pool<bool_tag>()
		.port_entries();
	std::set<size_t> s;
	footprint_frame_map_type::const_iterator
		pi(pbf.begin()), pe(pbf.begin() +ps);
if (pred) {
	// mask out ... want to use something like a mask_array/valarray
	vector<bool>::const_iterator mi(pred->begin()), me(pred->end());
	INVARIANT(pred->size() >= ps);
	for ( ; pi!=pe; ++pi, ++mi) {
	if (*mi) {
		s.insert(*pi);
	}
	}
} else {
	// unpredicated, just take all of them
	copy(pi, pe, set_inserter(s));
}
	copy(s.begin(), s.end(), back_inserter(v));
	return 0;
}	// end parse_name_to_get_ports

//=============================================================================
/**
	Construct set of aliases.
	\param n the name of the instance, whose aliases are to be sought.
	\param m top-level module.
	\return 0 upon success, 1 upon error.  
 */
int
parse_name_to_aliases(string_set& aliases, const string& n, const module& m,
		const dump_flags& df) {
	STACKTRACE_VERBOSE;
	const footprint& topfp(m.get_footprint());
	const global_indexed_reference
		gref(parse_global_reference(n, m));
	STACKTRACE_INDENT_PRINT("gref.index = " << gref.second << endl);
	if (gref.first && gref.second) {
		topfp.collect_aliases_recursive(gref, df, aliases);
	} else {
		return 1;
	}
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints reference identity information. 
	TODO: check non-instance-references:
		namespaces, definitions, typedefs, value-references.
	This is called by the 'who' command in various simulators.  
	\param n the name of the instance, whose aliases are to be sought.
	\param m top-level module.
	\param sep list separator, like space or newline.
	\return 0 upon success, 1 upon error.  
 */
int
parse_name_to_aliases(ostream& o, const string& n, const module& m, 
		const dump_flags& df, 
		const char* _sep) {
	STACKTRACE_VERBOSE;
	const char* sep = _sep ? _sep : " ";
	NEVER_NULL(sep);
	string_set aliases;
	if (parse_name_to_aliases(aliases, n, m, df)) {
		return 1;
	}
	ostream_iterator<string> osi(o, sep);
	copy(aliases.begin(), aliases.end(), osi);
	return 0;
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
	vector<string> temp;
{
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
		// parse the parent to get context
		const global_indexed_reference
			gref(parse_global_reference(parent, m, NULL));
		// silence bad references diagnostics
		if (!gref.second) { return; }
		if (gref.first != entity::META_TYPE_PROCESS) { return; }
		// until non-process types have subinstances...
		f = get_process_footprint(gref.second, m);
	}
	f->export_instance_names(temp);
}
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

