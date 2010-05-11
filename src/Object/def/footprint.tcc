/**
	\file "Object/def/footprint.tcc"
	Exported template implementation of footprint base class. 
	$Id: footprint.tcc,v 1.7 2010/05/11 00:18:07 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEF_FOOTPRINT_TCC__
#define	__HAC_OBJECT_DEF_FOOTPRINT_TCC__

#include <iostream>
#include <iterator>
#include "Object/def/footprint.h"
#include "Object/inst/instance_pool.h"	// for pool_private_map_entry_type
#include "Object/inst/state_instance.h"
#include "Object/inst/alias_actuals.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/traits/proc_traits.h"
#include "Object/common/dump_flags.h"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using std::ostream_iterator;

//=============================================================================
// class footprint_base method definitions

#if 0
// may not be needed after the last branch merge
template <class Tag>
footprint_base<Tag>::footprint_base() :
		_pool(class_traits<Tag>::instance_pool_chunk_size >> 1) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
footprint_base<Tag>::~footprint_base() { }
#endif


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This lookup could be critical?
	Return a reference to a unique object in the hierarchy
	that represents the type information.  
	\param Tag is the meta-type tag.
	\param gi is the global index being referenced.  0-based.
	\param is_top is true if is top-level, should be compile-time param.
	Implementation follows footprint::dump_canonical_name().
	TODO: re-write non-recursively
 */
template <class Tag>
const state_instance<Tag>&
footprint::get_instance(const size_t gi, const bool is_top) const {
	typedef	typename state_instance<Tag>::pool_type	pool_type;
//	STACKTRACE_VERBOSE;
	const pool_type& p(get_instance_pool<Tag>());
	const size_t ports = is_top ? 0 : p.port_entries();
	const size_t local = is_top ? p.local_entries()
		: p.local_private_entries();	// skip ports
	if (gi < local) {
		return p[gi +ports];
	} else {
		const size_t si = gi -local;
		const pool_private_map_entry_type&
			e(p.locate_private_entry(si));
		// e.first is the local process index (1-indexed)
		const state_instance<process_tag>::pool_type&
			ppool(get_instance_pool<process_tag>());
		const size_t m = ppool.local_entries();
		INVARIANT(e.first <= m);
		const state_instance<process_tag>& sp(ppool[e.first -1]);
		// e.second is the offset to subtract
		return sp._frame._footprint->get_instance<Tag>(
			si -e.second, false);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: re-write non-recursively
	\param Tag is the meta-type tag.
	\param gi is the global index being referenced.  0-based.
	\param is_top is true if is top-level, should be compile-time param.
 */
template <class Tag>
ostream&
footprint::dump_canonical_name(ostream& o, const size_t gi, 
		const dump_flags& df, 
		const bool is_top) const {
//	STACKTRACE_VERBOSE;
#if 0 && ENABLE_STACKTRACE
	dump_type(o << "type:") << endl;
#endif
	typedef	typename state_instance<Tag>::pool_type	pool_type;
	const pool_type& p(get_instance_pool<Tag>());
	const size_t ports = is_top ? 0 : p.port_entries();
	const size_t local = is_top ? p.local_entries()
		: p.local_private_entries();	// skip ports
//	STACKTRACE_INDENT_PRINT("<gi=" << gi << '/' << local << '>' << endl);
	if (gi < local) {
		// enumeration skips over ports
		// TODO: what if top-level has ports!?!?
		p[gi +ports].get_back_ref()->dump_hierarchical_name(o, 
			dump_flags::no_definition_owner);
	} else {
		const size_t si = gi -local;
		// si is 0-based residue index
//		STACKTRACE_INDENT_PRINT("<si=" << si << '>' << endl);
		const pool_private_map_entry_type&
			e(p.locate_private_entry(si));
		// e.first is the local process index (1-indexed)
		//	that 'owns' this referenced instance
		const state_instance<process_tag>::pool_type&
			ppool(get_instance_pool<process_tag>());
		const size_t m = ppool.local_entries();
//		STACKTRACE_INDENT_PRINT("<e.first=" << e.first << '/' << m << '>' << endl);
//		STACKTRACE_INDENT_PRINT("<e.second=" << e.second << '>' << endl);
		INVARIANT(e.first <= m);
		const state_instance<process_tag>& sp(ppool[e.first -1]);
		sp.get_back_ref()->dump_hierarchical_name(o, 
			df
			// dump_flags::no_definition_owner
			)
				<< df.process_member_separator;
		// TODO: pass in dump_flags to honor hierarchical separator
		// e.second is the offset to subtract
		sp._frame._footprint->dump_canonical_name<Tag>(
			o, si -e.second, df, false);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively collects aliases, public, private local, etc...
	\param index is 0-based global index.
	\aliases set of aliases to return.
 */
template <class Tag>
void
footprint::collect_aliases_recursive(const size_t index,
		set<string>& aliases, const bool is_top) const {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("rel-id(0) = " << index << endl);
	// based on current index, decide how to recurse
	// visit a single state_instance<bool_tag> if is local
	// visit all processes that contain local id
	const typename state_instance<Tag>::pool_type&
		lp(get_instance_pool<Tag>());
	const size_t ports = is_top ? 0 : lp.port_entries();
	const size_t local = is_top ? lp.local_entries()
		: lp.local_private_entries();
	STACKTRACE_INDENT_PRINT("local entries: " << local << endl);
if (index < local) { 
	STACKTRACE_INDENT_PRINT("local reference" << endl);
	// is locally owned or covered by port, 
	// traverse all local processes in which it is found
	this->collect_port_aliases<Tag>(index +ports, aliases);
} else {
	STACKTRACE_INDENT_PRINT("private reference" << endl);
	// is privately owned by subprocess
	// identify which subprocess owns it
	const size_t si = index -local;
	STACKTRACE_INDENT_PRINT("si = " << si << endl);
		// si is 0-based residue index
	const pool_private_map_entry_type&
		e(lp.locate_private_entry(si));
	const state_instance<process_tag>::pool_type&
		ppool(get_instance_pool<process_tag>());
	const size_t m = ppool.local_entries();
	const size_t& lpid(e.first);
	INVARIANT(lpid <= m);	// e.first is lpid (1-based)
	STACKTRACE_INDENT_PRINT("found in lpid(1) " << lpid << endl);
	set<string> private_aliases;
	// recurse first, then prepend resulting aliases
	ppool[lpid -1]._frame._footprint
		->collect_aliases_recursive<Tag>(si -e.second,
			private_aliases, false);
#if ENABLE_STACKTRACE
	copy(private_aliases.begin(), private_aliases.end(),
		ostream_iterator<string>(
			STACKTRACE_INDENT_PRINT("private-aliases: "), " "));
	STACKTRACE_STREAM << endl;
#endif
	// collect all aliases of that process
	set<string> local_aliases;
	scope_aliases.template get_id_map<process_tag>().find(lpid)->second
		.export_alias_strings(local_aliases);
	// to form cross-product of aliases
	set<string>::const_iterator
		pi(local_aliases.begin()), pe(local_aliases.end());
#if ENABLE_STACKTRACE
	copy(pi, pe, ostream_iterator<string>(
		STACKTRACE_INDENT_PRINT("local-aliases: "), " "));
	STACKTRACE_STREAM << endl;
#endif
	INVARIANT(pi != pe);            // must be at least one name!
	for ( ; pi!=pe; ++pi) {
		set<string>::const_iterator
			ai(private_aliases.begin()), ae(private_aliases.end());
		for ( ; ai!=ae; ++ai) {
			aliases.insert(*pi +'.' +*ai);
		}
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Finds all aliases of a port.
	This deeply recurses into subprocesses that contain the port.
	Implementation uses local ids only, sweet!
	\param ltid local indexed 0-based, must be in range of ports.
	\param aliases set in which to return aliases found.
 */
template <class Tag>
void
footprint::collect_port_aliases(const size_t ltid, set<string>& aliases) const {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("ltid(0) = " << ltid << endl);
#if ENABLE_STACKTRACE
	static const char* tag_name = class_traits<Tag>::tag_name;
#endif
{
	// first collect local aliases
	INVARIANT(ltid < get_instance_pool<Tag>().local_entries());
	const typename port_alias_tracker_base<Tag>::map_type&
		lpa(scope_aliases.template get_id_map<Tag>());
	const alias_reference_set<Tag>& lrs(lpa.find(ltid+1)->second);
	lrs.export_alias_strings(aliases);
#if ENABLE_STACKTRACE
	copy(aliases.begin(), aliases.end(), 
		ostream_iterator<string>(
			STACKTRACE_INDENT_PRINT("local: "), " "));
	STACKTRACE_STREAM << endl;
#endif
}
	const port_alias_tracker_base<process_tag>::map_type&
		lpps(scope_aliases.template get_id_map<process_tag>());
	const state_instance<process_tag>::pool_type&
		lpp(get_instance_pool<process_tag>());
	const size_t lpm = lpp.local_entries();
	size_t lpid = 1;                // 1-based index
for (; lpid <= lpm; ++lpid) {
	// gather local process aliases
	STACKTRACE_INDENT_PRINT("in lpid " << lpid << endl);
	const alias_reference_set<process_tag>& ppr(lpps.find(lpid)->second);
	set<string> parent_aliases;
	ppr.export_alias_strings(parent_aliases);
#if ENABLE_STACKTRACE
	copy(parent_aliases.begin(), parent_aliases.end(), 
		ostream_iterator<string>(
			STACKTRACE_INDENT_PRINT("parents: "), " "));
	STACKTRACE_STREAM << endl;
#endif

	// now iterate over spf's process ports
	// query footprint for private (and public) internal aliases.
	const state_instance<process_tag>& sp(lpp[lpid-1]);
	const footprint_frame& spf(sp._frame);
	const footprint& sfp(*spf._footprint);
	const footprint_frame_map_type&
		ppts(spf.template get_frame_map<Tag>());
	const size_t pps = ppts.size();
	size_t ppi = 0;
	set<string> mem_aliases;
	for ( ; ppi<pps; ++ppi) {
		// ppi is the position within the ports frame
		// ppts[ppi] is the local index mapped to that port
		const size_t app = ppts[ppi];	// 1-indexed
	if (app == ltid+1) {
		// only care about one local id, skip all others
		STACKTRACE_INDENT_PRINT(tag_name << "-port["
			<< ppi+1 << "] -> local-" << tag_name
			<< " " << app << endl);
		sfp.collect_port_aliases<Tag>(ppi, mem_aliases);
		// recursion!
	}
	}
#if ENABLE_STACKTRACE
	copy(mem_aliases.begin(), mem_aliases.end(), 
		ostream_iterator<string>(
			STACKTRACE_INDENT_PRINT("children: "), " "));
	STACKTRACE_STREAM << endl;
#endif
	// evaluate cross-product sets of parent x child
	set<string>::const_iterator
		j(parent_aliases.begin()), k(parent_aliases.end());
	for ( ; j!=k; ++j) {
		set<string>::const_iterator
			p(mem_aliases.begin()), q(mem_aliases.end());
	for ( ; p!=q; ++p) {
		const string c(*j + '.' + *p);
		aliases.insert(c);
		// will be inserted uniquely
	}
	}
}
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_DEF_FOOTPRINT_TCC__

