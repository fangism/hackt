/**
	\file "Object/inst/alias_printer.h"
	$Id: alias_printer.h,v 1.3.16.5 2010/02/25 02:48:43 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_ALIAS_PRINTER_H__
#define	__HAC_OBJECT_INST_ALIAS_PRINTER_H__

#include <set>
#include <vector>
#include "Object/inst/alias_visitor.h"
#include "Object/common/cflat_args.h"

/**
	Define to 1 to visit by aliases.
	Define to 0 to visit by unique instances.  
	Visiting by aliases is easier to guarantee coverage.
	Visiting by unique instances is easier to maintain footprint frames.
	Goal: 0
 */
// #define	TRAVERSE_BY_ALIAS		1

namespace HAC {
namespace entity {
struct alias_printer;
using std::set;
using std::vector;
//=============================================================================
/**
	Alias-printing visitor.  
 */
struct alias_printer : 
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		public alias_visitor, 
#endif
		public cflat_aliases_arg_type {
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
// graph structures for topological sorting
typedef	vector<size_t>		ordered_list_type;
typedef	vector<set<size_t> >	graph_type;
typedef	vector<bool>		marks_type;
typedef	vector<set<string> >	alias_set_type;
private:
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
	alias_set_type			local_bool_aliases;
#endif
#if 0 && MEMORY_MAPPED_GLOBAL_ALLOCATION
	typedef	alias_printer			this_type;
	/**
		Auxiliary footprint frame, this is updated with every
		public and private recursion, whereas the parent 'fpf'
		is only updated with private recursion.
	 */
//	const footprint_frame*			aux_fpf;
	const this_type*			parent;
#endif
public:
	alias_printer(ostream& _o, 
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
			const footprint_frame& _fpf, 
			const global_offset& g,
#else
			const state_manager& _sm,
			const footprint& _f,
			const footprint_frame* const _fpf,
#endif
			const cflat_options& _cf,
			wire_alias_set& _w,
			const string& _p = string()
#if 0 && MEMORY_MAPPED_GLOBAL_ALLOCATION
			, const this_type* __p = NULL
#endif
			);

	// default dtor
	// default copy-ctor

	// may not need these... depending on traversal algorithm
	VISIT_INSTANCE_ALIAS_INFO_PROTOS()

#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	void
	visit(const footprint&);

	void
	visit_recursive(const footprint&);

#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	using alias_visitor::visit;
#endif
	using cflat_aliases_arg_type::visit;

#if 0
	template <class Tag>
	size_t
	lookup_local_id(const size_t) const;

	template <class Tag>
	size_t
	lookup_global_id(const size_t) const;

	const global_entry_context&
	owner_context(void) const;

	template <class Tag>
	ostream&
	dump_canonical_name(ostream&, const size_t) const;
#endif

	void
	visit(const state_instance<bool_tag>&);

private:
	static
	void
	__topological_sort_visit(const graph_type&, const size_t, 
		marks_type&, ordered_list_type&);
public:
	static
	void
	topological_sort(const graph_type&, ordered_list_type&);
#endif

private:
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	void
	prepare(const footprint&);

	template <class SubTag>
	void
	collect_local_aliases(const footprint&, alias_set_type&) const;
#else
	// helper functions here
	template <class Tag>
	void
	__visit(const instance_alias_info<Tag>&);
#endif

	// non-copyable
	explicit
	alias_printer(const alias_printer&);

};	// end class alias_printer

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_ALIAS_PRINTER_H__

