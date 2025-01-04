/**
	\file "Object/common/cflat_args.tcc"
	$Id: cflat_args.tcc,v 1.5 2010/05/11 00:18:05 fang Exp $
 */

#ifndef	__HAC_OBJECT_COMMON_CFLAT_ARGS_TCC__
#define	__HAC_OBJECT_COMMON_CFLAT_ARGS_TCC__

#include <algorithm>
#include "Object/common/cflat_args.hh"
#include "Object/inst/instance_alias_info.hh"
#include "Object/inst/alias_actuals.hh"
#include "Object/inst/alias_empty.hh"
#include "Object/def/footprint.hh"
#include "Object/global_entry.tcc"
#include "Object/common/dump_flags.hh"
#include "Object/inst/physical_instance_collection.hh"
#include "Object/inst/physical_instance_placeholder.hh"
#include "common/ICE.hh"
#include "common/TODO.hh"
#include "util/sstream.hh"
#include "util/macros.h"
#include "util/stacktrace.hh"


namespace HAC {
namespace entity {
using std::ostringstream;
using std::set;		// for caching alias sets
using std::copy;

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
		sp(ss.template is_a<const process_alias>());
	NEVER_NULL(sp);		// else parent is not a process!
	if (sp->instance_index <=
		f.template get_instance_pool<ParentTag>().port_entries()) {
		return true;
	}
	ac = sp->get_container_base();
	ss = ac->get_super_instance();
}
	// what if has no parent, is terminal?
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// private to Object/common/cflat_args.cc
template <class Tag>
bool
accept_deep_alias(const instance_alias_info<Tag>& a, const footprint& f);

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
#if ENABLE_STACKTRACE
	typedef	class_traits<SubTag>		traits_type;
	static const char* tag_name = traits_type::tag_name;
#endif
	const state_instance<Tag>::pool_type&
		lpp(f.template get_instance_pool<Tag>());
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
		ppa(spt.template get_id_map<SubTag>());
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
		alias_group_type mem_aliases;
		for ( ; pmi!=pme; ++pmi) {
			const instance_alias_info<SubTag>& a(**pmi);
			// process ports: if it is a public port, skip it
			// bool ports: if supermost collection of the 
			//	hierarchical alias is not publicly reachable
		if (accept_deep_alias(a, sfp)) {
			ostringstream malias;
			a.dump_hierarchical_name(malias, df);
			STACKTRACE_INDENT_PRINT(tag_name << "-member alias: "
				<< malias.str() << endl);
#if USE_ALT_ALIAS_PAIRS
			string& alt_m(mem_aliases[malias.str()]);
			if (use_alt_name()) {
				ostringstream aa;
				a.dump_hierarchical_name(aa, alt_df);
				alt_m = aa.str();
			}
#else
			mem_aliases.insert(malias.str());
#endif
			// missing parent name
		}
		}
		// evaluate cross-product sets of parent x child
		alias_group_type::const_iterator
			j(local_proc_aliases[lpid].begin()),
			k(local_proc_aliases[lpid].end());
		for ( ; j!=k; ++j) {
			alias_group_type::const_iterator
				p(mem_aliases.begin()), q(mem_aliases.end());
		for ( ; p!=q; ++p) {
#if USE_ALT_ALIAS_PAIRS
			const string c(j->first
				+df.process_member_separator +p->first);
			string& alt(local_aliases[app][c]);
			if (use_alt_name()) {
				alt = j->second +alt_df.process_member_separator
					+p->second;
			}
#else
			const string c(*j +df.process_member_separator +*p);
			STACKTRACE_INDENT_PRINT(tag_name << 
				"-alias[" << app << "]: " << c << endl);
			local_aliases[app].insert(c);
			// will be inserted uniquely
#endif
		}
		}
	}
}
}	// end cflat_aliases_arg_type::collect_local_aliases()

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_COMMON_CFLAT_ARGS_TCC__
