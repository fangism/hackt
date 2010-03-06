/**
	\file "Object/common/cflat_args.tcc"
	$Id: cflat_args.tcc,v 1.1.2.2 2010/03/06 00:32:56 fang Exp $
 */

#ifndef	__HAC_OBJECT_COMMON_CFLAT_ARGS_TCC__
#define	__HAC_OBJECT_COMMON_CFLAT_ARGS_TCC__

#include <algorithm>
#include "Object/common/cflat_args.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_actuals.h"
#include "Object/inst/alias_empty.h"
#include "Object/def/footprint.h"
#include "Object/global_entry.tcc"
#include "Object/common/dump_flags.h"
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
#include "Object/inst/physical_instance_collection.h"
#include "Object/inst/physical_instance_placeholder.h"
#endif
#include "common/ICE.h"
#include "common/TODO.h"
#include "util/sstream.h"
#include "util/macros.h"
#include "util/stacktrace.h"


namespace HAC {
namespace entity {
using std::ostringstream;
using std::set;		// for caching alias sets
using std::copy;

#if MEMORY_MAPPED_GLOBAL_ALLOCATION
//=============================================================================
// class cflat_aliases_arg_type method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Predicates under which a subordinate alias should be considered.
 */
template <class Tag>
bool
any_hierarchical_parent_is_aliased_to_port(const instance_alias_info<Tag>& a, 
		const footprint& f) {
	typedef	process_tag		ParentTag;
	// NO hierarchical parents are already aliased to process port
	never_ptr<const physical_instance_collection>
		ac(a.get_container_base());
	never_ptr<const substructure_alias>
		ss(ac->get_super_instance());
while (ss) {
	typedef	instance_alias_info<ParentTag>	process_alias;
	const never_ptr<const process_alias>
		sp(ss.is_a<const process_alias>());
	NEVER_NULL(sp);		// else parent is not a process!
	if (sp->instance_index <=
			f.get_instance_pool<ParentTag>().port_entries()) {
		return true;
	}
	ac = sp->get_container_base();
	ss = ac->get_super_instance();
}
	// what if has no parent, is terminal?
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	If a bool is reachable through public port alias (hierarchical),
	exclude it.  
	Also applies to any terminal type (basically non-process).
	if the supermost (process) collection of a bool is not aliased 
	to a process-port, also exclude it.
 */
template <class Tag>
static
inline
bool
__accept_deep_alias(const instance_alias_info<Tag>& a, const footprint& f) {
	typedef	class_traits<Tag>		traits_type;
#if ENABLE_STACKTRACE
	static const char* tag_name = traits_type::tag_name;
#endif
	const bool reachable = a.get_supermost_collection()
		->get_placeholder_base()->is_port_formal();
	STACKTRACE_INDENT_PRINT(tag_name << (reachable ? " is" : " is not")
		<< " reachable." << endl);
	if (reachable)
		return false;
	if (a.is_port_alias())
		return false;
	// and NO hierarchical parents are already aliased to process port
	return !any_hierarchical_parent_is_aliased_to_port(a, f);
}

// Specialization for process_tag, defined in .cc unit.
template <>
bool
__accept_deep_alias(const instance_alias_info<process_tag>&, const footprint&);

template <class Tag>
bool
accept_deep_alias(const instance_alias_info<Tag>& a, const footprint& f) {
	return __accept_deep_alias(a, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Collect all local scope aliases, *including* private aliases
	inside subprocesses that are also aliased to their ports.  
	Re-use: would be useful to just search for one 
 */
template <class SubTag>
void
cflat_aliases_arg_type::collect_local_aliases(const footprint& f,
		alias_set_type& local_aliases) const {
	STACKTRACE_VERBOSE;
	typedef	process_tag			Tag;
	typedef	class_traits<SubTag>		traits_type;
#if ENABLE_STACKTRACE
	static const char* tag_name = traits_type::tag_name;
#endif
	const state_instance<Tag>::pool_type& lpp(f.get_instance_pool<Tag>());
//	local_aliases.resize(lpp.local_entries() +1);	// allow 1-based index
//	STACKTRACE_INDENT_PRINT("resized local-aliases to: " << local_aliases.size() << endl);
	ordered_list_type::const_iterator
		pidi(ordered_lpids.begin()), pide(ordered_lpids.end());
for (; pidi!=pide; ++pidi) {
	// HACK ALERT: to cover internal aliases
	// now iterate over spf's process ports
	// query footprint for private (and public) internal aliases.
	const size_t lpid = *pidi;
	// const size_t lpid = pi +1;
	const state_instance<Tag>& sp(lpp[lpid-1]);
	const footprint_frame& spf(sp._frame);
	const footprint& sfp(*spf._footprint);
	const port_alias_tracker& spt(sfp.get_scope_alias_tracker());
	const typename port_alias_tracker_base<SubTag>::map_type&
		ppa(spt.get_id_map<SubTag>());
	const footprint_frame_map_type& ppts(spf.get_frame_map<SubTag>());
	const size_t pps = ppts.size();
	size_t ppi = 0;
	for ( ; ppi<pps; ++ppi) {
		// ppi is the position within the ports frame
		// ppts[ppi] is the local index mapped to that port
		const size_t app = ppts[ppi];
//		INVARIANT(app != lpid);	// process cannot contain itself!
		STACKTRACE_INDENT_PRINT(tag_name << "-port["
			<< ppi+1 << "] -> local-" << tag_name
			<< " " << app << endl);
		// iterate over everything but the port aliases,
		// which were already covered by the above pass.
		const alias_reference_set<SubTag>& par(ppa.find(ppi+1)->second);
		typename alias_reference_set<SubTag>::const_iterator
			pmi(par.begin()), pme(par.end());
		set<string> mem_aliases;
		for ( ; pmi!=pme; ++pmi) {
			const instance_alias_info<SubTag>& a(**pmi);
			// process ports: if it is a public port, skip it
			// bool ports: if supermost collection of the 
			//	hierarchical alias is not publicly reachable
		if (accept_deep_alias(a, sfp)) {
			ostringstream malias;
			a.dump_hierarchical_name(malias,
				dump_flags::no_leading_scope);
			STACKTRACE_INDENT_PRINT(tag_name << "-member alias: "
				<< malias.str() << endl);
			mem_aliases.insert(malias.str());
			// missing parent name
		}
		}
		// evaluate cross-product sets of parent x child
		set<string>::const_iterator
			j(local_proc_aliases[lpid].begin()),
			k(local_proc_aliases[lpid].end());
		for ( ; j!=k; ++j) {
			set<string>::const_iterator
				p(mem_aliases.begin()), q(mem_aliases.end());
		for ( ; p!=q; ++p) {
			const string c(*j + '.' + *p);
			STACKTRACE_INDENT_PRINT(tag_name << 
				"-alias[" << app << "]: " << c << endl);
			local_aliases[app].insert(c);
			// will be inserted uniquely
		}
		}
	}
}
}	// end cflat_aliases_arg_type::collect_local_aliases()

#endif	// MEMORY_MAPPED_GLOBAL_ALLOCATION

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_COMMON_CFLAT_ARGS_TCC__
