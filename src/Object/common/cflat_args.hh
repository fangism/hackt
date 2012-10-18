/**
	"Object/common/cflat_args.hh"
	Common aggregate argument types for various cflat methods.  
	$Id: cflat_args.hh,v 1.10 2010/05/11 00:18:05 fang Exp $
 */

#ifndef	__HAC_OBJECT_COMMON_CFLAT_ARGS_H__
#define	__HAC_OBJECT_COMMON_CFLAT_ARGS_H__

/**
	Define to 1 to keep around a mapping to mangled names.  
 */
#define	USE_ALT_ALIAS_PAIRS			1

#include <iosfwd>
#include <string>
#include <vector>
#if USE_ALT_ALIAS_PAIRS
#include <map>
#endif
#include <set>
// #include "Object/devel_switches.h"
#include "Object/global_entry.hh"
#include "Object/global_entry_context.hh"

namespace HAC {
struct cflat_options;
namespace entity {
using std::string;
using std::ostream;
using std::vector;
#if USE_ALT_ALIAS_PAIRS
using std::map;
#endif
using std::set;
class footprint;
class footprint_frame;
template <class> class instance_alias_info;
struct dump_flags;

//=============================================================================
/**
	Argument type for cflat_aliases methods, merely for convenience.  
	Reference members never change.  
 */
struct cflat_aliases_arg_type :
		public global_entry_context
{
// graph structures for topological sorting
typedef	vector<size_t>		ordered_list_type;
typedef	vector<set<size_t> >	graph_type;
typedef	vector<bool>		marks_type;
#if USE_ALT_ALIAS_PAIRS
// mapped_type can be a transformed/mangled string for other tools
typedef	map<string, string>	alias_group_type;
#else
typedef	set<string>		alias_group_type;
#endif
typedef	vector<alias_group_type>	alias_set_type;
protected:
	vector<footprint_frame>		fframes;
	vector<global_offset>		offsets;
	/**
		Directed acyclic graph of local processes containing
		other local processes, used for partial ordering, 
		and topological sort.
	 */
	graph_type			local_proc_graph;
	ordered_list_type		ordered_lpids;
	/**
		Alias sets also include private aliases of local processes.
	 */
	alias_set_type			local_proc_aliases;
//	alias_set_type			local_bool_aliases;

	const dump_flags&		df;
#if USE_ALT_ALIAS_PAIRS
	const dump_flags&		alt_df;
#endif
	/**
		Cumulative hierarchical name (top-down).  
		The prefix grows with each level of instance hierarchy.  
	 */
	string				prefix;
public:
	cflat_aliases_arg_type(
			const global_process_context&,
			const dump_flags&,
#if USE_ALT_ALIAS_PAIRS
			const dump_flags&,
#endif
			const string& _p = string());
	// default copy-constructor
virtual	~cflat_aliases_arg_type();

protected:
	using global_entry_context::visit;

virtual	void
	visit(const footprint&);

private:
virtual	void
	visit_footprint(const footprint&) = 0;

// TODO: push this into a util graph library
private:
	static
	void
	__topological_sort_visit(const graph_type&, const size_t, 
		marks_type&, ordered_list_type&);
public:
	static
	void
	topological_sort(const graph_type&, ordered_list_type&);

#if USE_ALT_ALIAS_PAIRS
	/// if the dump_flags are the same, don't bother
	bool
	use_alt_name(void) const {
		return &df != &alt_df;
	}
#endif

protected:
//	virtual ?
	void
	prepare(const footprint&);

	template <class SubTag>
	void
	collect_local_aliases(const footprint&, alias_set_type&) const;

};	// end struct cflat_aliases_arg_type

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// helper functions

template <class Tag>
bool
accept_deep_alias(const instance_alias_info<Tag>&, const footprint&);

template <class Tag>
bool
any_hierarchical_parent_is_aliased_to_port(const instance_alias_info<Tag>&,
	const footprint&);

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_COMMON_CFLAT_ARGS_H__

