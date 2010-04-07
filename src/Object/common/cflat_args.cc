/**
	\file "Object/common/cflat_args.cc"
	$Id: cflat_args.cc,v 1.3 2010/04/07 00:12:34 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include <functional>			// for bind2nd, greater_equal
#include <iterator>
#include "Object/common/cflat_args.tcc"
#include "Object/inst/alias_actuals.h"
#include "Object/inst/alias_empty.h"
#include "util/copy_if.h"
#include "util/iterator_more.h"		// for set_inserter
#include "util/swap_saver.h"


namespace HAC {
namespace entity {
// using std::ostringstream;
using std::set;		// for caching alias sets
using std::copy;
USING_COPY_IF
using std::ostream_iterator;
using std::bind2nd;
using util::swap_saver;
using util::set_inserter;

//=============================================================================
// helper functions
/**
	Specialization for process_tag.
 */
template <>
bool
__accept_deep_alias(const instance_alias_info<process_tag>& a,
		const footprint&) {
	return !a.is_port_alias();
}

//=============================================================================
// class cflat_aliases_arg_type method definitions

cflat_aliases_arg_type::cflat_aliases_arg_type(
		const footprint_frame& _fpf,
		const global_offset& g,
		const string& _p) :
		global_entry_context(_fpf, g),
		local_proc_graph(), 
		ordered_lpids(),
		local_proc_aliases(),
//		local_bool_aliases(),
		prefix(_p) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cflat_aliases_arg_type::~cflat_aliases_arg_type() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Traversal: by unique or by alias?  By alias would be best.
	Key: visit every alias exactly once.
	Should follow global_entry_context::visit_recursive.
 */
void
cflat_aliases_arg_type::visit(const footprint& f) {
	STACKTRACE_VERBOSE;
// TODO: try-catch exception handling?
	// save these on the stack, to auto-restore them, clearing them out
	// swap_savers are extremely efficient
	const swap_saver<vector<footprint_frame> > __ffs__(fframes);
	const swap_saver<vector<global_offset> > __off__(offsets);
	const swap_saver<graph_type> __gr__(local_proc_graph);
	const swap_saver<ordered_list_type> __ol__(ordered_lpids);
	const swap_saver<alias_set_type> __lpa__(local_proc_aliases);
	prepare(f);
	visit_footprint(f);	// pure virtual
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursive, visits the i'th node in the graph.
 */
void
cflat_aliases_arg_type::__topological_sort_visit(const graph_type& G,
		const size_t i, marks_type& V, ordered_list_type& o) {
	if (!V[i]) {
		V[i] = true;
		set<size_t>::const_iterator j(G[i].begin()), k(G[i].end());
		for ( ; j!=k; ++j) {
			__topological_sort_visit(G, *j, V, o);
		}
		o.push_back(i);	// really want push_front
	}
}

/**
	\pre graph must be acyclic, this algorithm does not detect cycles.
 */
void
cflat_aliases_arg_type::topological_sort(const graph_type& G,
		ordered_list_type& o) {
	ordered_list_type rev;
	o.clear();
	const size_t s = G.size();	// number of nodes
	marks_type V(s, false);		// no nodes visited yet
	size_t i = 0;
	for ( ; i<s; ++i) {
		__topological_sort_visit(G, i, V, rev);
	}
	copy(rev.rbegin(), rev.rend(), back_inserter(o));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prepare some data structures for use.
 */
void
cflat_aliases_arg_type::prepare(const footprint& f) {
	STACKTRACE_VERBOSE;
	typedef	process_tag		Tag;	// meta-type to recurse on
	const port_alias_tracker& pt(f.get_scope_alias_tracker());
	const state_instance<Tag>::pool_type&
		lpp(f.get_instance_pool<Tag>());
	const bool is_top = at_top();
	NEVER_NULL(parent_offset);
	global_offset sgo(*parent_offset, f, add_local_private_tag());
	footprint_frame lff;
if (is_top) {
	lff.construct_top_global_context(f, *parent_offset);
	sgo = global_offset(*parent_offset, f, add_all_local_tag());
} else {
	lff.construct_global_context(f, *fpf, *parent_offset);
}
#if ENABLE_STACKTRACE
	lff.dump_frame(STACKTRACE_STREAM << "local context:") << endl;
#endif
	// copy and increment with each local process
	const size_t pe = lpp.local_entries();
	// but for the top-level only, we start with ports (process too?)
	const size_t pb = is_top ? 0 : lpp.port_entries();
// first pass: 
// a) collect local aliases of processes owned by this footprint
// b) construct and cache the footprint frames, as we use them multiple times
// caching offsets is not really necessary.
// c) construct graph used to perform topological sort
	const size_t ps = pe+1;
	local_proc_aliases.resize(ps);
	INVARIANT(ordered_lpids.empty());
	fframes.resize(ps);
	offsets.resize(ps);
	local_proc_graph.resize(ps);
	size_t pi;
for (pi=pb; pi<pe; ++pi) {
	const size_t lpid = pi +1;
	const state_instance<Tag>& sp(lpp[pi]);
	const footprint_frame& spf(sp._frame);
	const footprint& sfp(*spf._footprint);
	// construct actuals footprint frame to be used as context
	footprint_frame af(spf, lff);     // context
	fframes[lpid].swap(af);
	offsets[lpid] = sgo;
	// construct local-process port graph's adjacency lists
	const footprint_frame_map_type& ppts(spf.get_frame_map<Tag>());
	copy(ppts.begin(), ppts.end(), set_inserter(local_proc_graph[lpid]));
	// collect local aliases
	const alias_reference_set<Tag>&
		ars(pt.get_id_map<Tag>().find(lpid)->second);
	alias_reference_set<Tag>::const_iterator
		i(ars.begin()), e(ars.end());
	for ( ; i!=e; ++i) {
		NEVER_NULL(*i);
		const instance_alias_info<Tag>& a(**i);
		ostringstream oss;
		a.dump_hierarchical_name(oss, dump_flags::no_leading_scope);
		const string& local_name(oss.str());	// base-names
		local_proc_aliases[lpid].insert(local_name);
	}
	// increment global offset
	if (pi >= lpp.port_entries()) {
	sgo += sfp;
	}
}

// topological sort of locally owned processes, 
// some of which are ports of each other!
// The reason this is necessary is to order hierarchical alias propagation
// from, as children may have multiple parents, and all parents must be 
// processed before their children.
{
	ordered_list_type ordered_list;	// result
	topological_sort(local_proc_graph, ordered_list);
	copy_if(ordered_list.begin(), ordered_list.end(),
		back_inserter(ordered_lpids),
		bind2nd(std::greater<size_t>(), pb));	// 1-based indices
}
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("topo-sorted lpids: ");
	copy(ordered_lpids.begin(), ordered_lpids.end(), 
		std::ostream_iterator<size_t>(STACKTRACE_STREAM, ","));
	STACKTRACE_STREAM << endl;
#endif

// second pass: collect member aliases, cross-product with local aliases
// this needs to happen in topological order!
	collect_local_aliases<process_tag>(f, local_proc_aliases);
// also need to see which local bools are members of local processes
}	// end cflat_aliases_arg_type::prepare()

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

