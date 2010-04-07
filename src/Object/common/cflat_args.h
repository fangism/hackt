/**
	"Object/common/cflat_args.h"
	Common aggregate argument types for various cflat methods.  
	$Id: cflat_args.h,v 1.9 2010/04/07 00:12:34 fang Exp $
 */

#ifndef	__HAC_OBJECT_COMMON_CFLAT_ARGS_H__
#define	__HAC_OBJECT_COMMON_CFLAT_ARGS_H__

#include <iosfwd>
#include <string>
#include <vector>
#include <set>
#include "util/member_saver.h"
// #include "Object/devel_switches.h"
#include "Object/global_entry.h"
#include "Object/global_entry_context.h"

namespace HAC {
class cflat_options;
namespace entity {
using std::string;
using std::ostream;
using std::vector;
using std::set;
class footprint;
class footprint_frame;
template <class> class instance_alias_info;

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
typedef	vector<set<string> >	alias_set_type;
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
	/**
		Cumulative hierarchical name (top-down).  
		The prefix grows with each level of instance hierarchy.  
	 */
	string				prefix;
public:
	cflat_aliases_arg_type(
			const footprint_frame& _fpf,
			const global_offset& g,
			const string& _p = string());
	// default copy-constructor
virtual	~cflat_aliases_arg_type();

	typedef	util::member_saver<cflat_aliases_arg_type, string, 
			&cflat_aliases_arg_type::prefix>
				save_prefix;

protected:
	using global_entry_context::visit;

virtual	void
	visit(const footprint&);

private:
virtual	void
	visit_footprint(const footprint&) = 0;

private:
	static
	void
	__topological_sort_visit(const graph_type&, const size_t, 
		marks_type&, ordered_list_type&);
public:
	static
	void
	topological_sort(const graph_type&, ordered_list_type&);

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

