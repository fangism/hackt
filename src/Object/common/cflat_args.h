/**
	"Object/common/cflat_args.h"
	Common aggregate argument types for various cflat methods.  
	$Id: cflat_args.h,v 1.7.124.3 2010/03/04 02:53:22 fang Exp $
 */

#ifndef	__HAC_OBJECT_COMMON_CFLAT_ARGS_H__
#define	__HAC_OBJECT_COMMON_CFLAT_ARGS_H__

#include <iosfwd>
#include <string>
#include <vector>
#include <set>
#include "util/member_saver.h"
#include "Object/devel_switches.h"
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
#include "Object/global_entry.h"
#include "Object/global_entry_context.h"
#endif

namespace HAC {
class cflat_options;
namespace entity {
using std::string;
using std::ostream;
using std::vector;
using std::set;
class footprint;
class footprint_frame;
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
template <class> class instance_alias_info;
#else
class state_manager;
struct wire_alias_set;	// defined in "Object/common/alias_string_cache.h"
#endif

//=============================================================================
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
// typedef	global_entry_context			cflat_args_base;
#else
/**
	The base structure for traversals of the name object (alias)
	hierarchy.  
	Walkers should be based on this...
 */
struct cflat_args_base {
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	/**
		The state manager contains the information about the
		globally allocated unique instances, 
		including footprint frames.  
	 */
	const state_manager&		sm;
#endif
	/**
		Should be the top-level footprint belonging to the module, 
		needed for dumping correct canonical name.  
	 */
	const footprint&		topfp;
	/**
		If this is NULL, then we are at top-level.  
		Is a never-delete pointer.  
		The footprint frame maps the local aliases to globally
		allocated unique instances.  
	 */
	const footprint_frame*		fpf;
public:
	cflat_args_base(
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
			const state_manager& _sm, 
#endif
			const footprint& _f,
			const footprint_frame* const _fpf) : 
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
			sm(_sm),
#endif
			topfp(_f), fpf(_fpf) { }

	typedef	util::member_saver<cflat_args_base, const footprint_frame*, 
				&cflat_args_base::fpf>
					save_frame;

};	// end struct cflat_args_base
#endif

//=============================================================================
/**
	Argument type for cflat_aliases methods, merely for convenience.  
	Reference members never change.  
 */
struct cflat_aliases_arg_type :
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
		public global_entry_context
#else
		public cflat_args_base
#endif
{
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
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
#else
	ostream&			o;
	/**
		cflat mode and style flags.  
	 */
	const cflat_options&		cf;
	/**
		\pre is already sized properly to accomodate total
			number of allocated bool nodes.  
	 */
	wire_alias_set&			wires;
#endif
	/**
		Cumulative hierarchical name (top-down).  
		The prefix grows with each level of instance hierarchy.  
	 */
	string				prefix;
public:
	cflat_aliases_arg_type(
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
			const footprint_frame& _fpf,
			const global_offset& g,
#else
			ostream& _o,
			const state_manager& _sm, 
			const footprint& _f, 
			const footprint_frame* const _fpf, 
			const cflat_options& _cf,
			wire_alias_set& _w,
#endif
			const string& _p = string())
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
			;
#else
			:
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
			global_entry_context(_fpf, g),
#else
			cflat_args_base(_sm, _f, _fpf), 
			o(_o),
			cf(_cf),
			wires(_w), 
#endif
			prefix(_p) {
	}
#endif
	// default copy-constructor
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
virtual	~cflat_aliases_arg_type();
#else
	// default destructor
#endif

	typedef	util::member_saver<cflat_aliases_arg_type, string, 
			&cflat_aliases_arg_type::prefix>
				save_prefix;

#if MEMORY_MAPPED_GLOBAL_ALLOCATION
protected:
	using global_entry_context::visit;

	void
	visit(const footprint&);

private:
virtual	void
	visit_footprint(const footprint&) = 0;
#endif
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
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
//	virtual ?
	void
	prepare(const footprint&);

	template <class SubTag>
	void
	collect_local_aliases(const footprint&, alias_set_type&) const;
#endif

};	// end struct cflat_aliases_arg_type

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
// helper functions

template <class Tag>
bool
accept_deep_alias(const instance_alias_info<Tag>&, const footprint&);

template <class Tag>
bool
any_hierarchical_parent_is_aliased_to_port(const instance_alias_info<Tag>&,
	const footprint&);
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_COMMON_CFLAT_ARGS_H__

