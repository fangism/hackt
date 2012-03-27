/**
	\file "Object/hierarchical_alias_visitor.cc"
	$Id: $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include <sstream>
#include <iterator>
#include "Object/hierarchical_alias_visitor.h"
#include "Object/def/footprint.h"
#include "Object/global_entry.h"
#include "Object/global_channel_entry.h"
#include "Object/inst/port_alias_tracker.h"
#include "Object/inst/alias_actuals.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/state_instance.h"
#include "Object/inst/instance_pool.h"
#include "Object/traits/instance_traits.h"
#include "Object/common/dump_flags.h"
#include "util/value_saver.h"
#include "util/stack_saver.h"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using std::ostream_iterator;
using std::ostringstream;
using util::value_saver;
using util::back_stack_saver;
typedef	value_saver<const footprint_frame*>	footprint_frame_setter;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
hierarchical_alias_visitor::hierarchical_alias_visitor(
		const footprint_frame& ff, const global_offset& g) :
		parent_type(ff, g), scope_stack(), id(0) {
	// FIXME: this just avoids dangling references as a result
	// vector::push_back, but only up to a pre-determined limit,
	// should really make this more robust, by resetting iterators.
	scope_stack.reserve(64);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
hierarchical_alias_visitor::~hierarchical_alias_visitor() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is for terminal (non-recursive) types.
	Yes, always include ports, so no bool parameter.  
 */
template <class Tag>
void
hierarchical_alias_visitor::visit_ports_and_local(const footprint& f) {
	STACKTRACE_VERBOSE;
	const value_saver<size_t> _ids_(id);
	const back_stack_saver<scope_stack_type> _ss_(scope_stack);
	string& local_name(scope_stack.back());
	const typename state_instance<Tag>::pool_type&
		_pool(f.template get_instance_pool<Tag>());
	const port_alias_tracker& pat(f.get_scope_alias_tracker());
	typedef typename port_alias_tracker_base<Tag>::map_type  pmap_type;
	const pmap_type& ppa(pat.get_id_map<Tag>());
	// construct context (footprint_frame)
	footprint_frame lff;
	global_offset sgo(*parent_offset, f, add_local_private_tag());
	NEVER_NULL(parent_offset);
	lff.construct_global_context(f, *fpf, *parent_offset);
	// print local, non-port entries, since ports belong to 'parent'
	// for processes duplicate work computing global offsets twice
	const value_saver<global_offset*> __gs__(g_offset, &sgo);
	const footprint_frame_setter __ffs__(fpf, &lff);        // local actuals
	const footprint_frame_transformer fft(*fpf, Tag());
//	const size_t p = _pool.port_entries();
	const size_t l = _pool.local_entries();
	// but for the top-level only, we want start with ports
	size_t i = 0;
	// or iterate over alias_reference_sets?
	for ( ; i<l; ++i) {
		// global and local index can be deduced
		const typename pmap_type::const_iterator ai(ppa.find(i+1));
	if (ai != ppa.end()) {
		STACKTRACE_INDENT_PRINT("index: " << i+1 << endl);
		id = fft(i+1);
		STACKTRACE_INDENT_PRINT("gid: " << id << endl);
		const state_instance<Tag>& inst(_pool[i]);
		const alias_reference_set<Tag>& pars(ai->second);
		typename alias_reference_set<Tag>::const_iterator
			pai(pars.begin()), pae(pars.end());
		for ( ; pai!=pae; ++pai) {
			NEVER_NULL(*pai);
			if (!(*pai)->is_subinstance()) {
				// push local name onto stack
				ostringstream oss;
				(*pai)->dump_hierarchical_name(oss, 
					dump_flags::no_owners);
				local_name = oss.str();
				STACKTRACE_INDENT_PRINT("  local: "
					<< local_name << endl);
				inst.accept(*this);
			} else {
#if ENABLE_STACKTRACE
				(*pai)->dump_hierarchical_name(
					STACKTRACE_INDENT_PRINT("  skip : "), 
					dump_flags::no_owners) << endl;
#endif
			}
		}
	}
		// visitor will increment g_offset
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	By default, this just visits bools.
	Can be overridden to visit more meta-types.
 */
void
hierarchical_alias_visitor::visit(const footprint& f) {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	f.dump_type(STACKTRACE_INDENT_PRINT("type: ")) << endl;
#endif
	// visit terminal types
	visit_ports_and_local<bool_tag>(f);
	visit_ports_and_local<int_tag>(f);
	visit_ports_and_local<enum_tag>(f);
	visit_ports_and_local<channel_tag>(f);
	visit_ports_and_local<process_tag>(f);
	visit_recursive(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visits every *alias*.
	Does this really need _gpid tracked?
 */
void
hierarchical_alias_visitor::visit_recursive(const footprint& f) {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("in process id " << _gpid << ", type: ");
	f.dump_type(STACKTRACE_STREAM) << endl;
	STACKTRACE_INDENT_PRINT("offset: " << *parent_offset << endl);
	fpf->dump_frame(STACKTRACE_STREAM << "frame:") << endl;
	copy(scope_stack.begin(), scope_stack.end(), 
		ostream_iterator<string>(
			STACKTRACE_INDENT_PRINT("scope: "), "."));
	STACKTRACE_STREAM << endl;
#endif
	// CAUTION!!! recusion could invalidate the vector reference!!!
	const back_stack_saver<scope_stack_type> _ss_(scope_stack);
	string& local_name(scope_stack.back());
	// recurse through processes and print
	const state_instance<process_tag>::pool_type&
		lpp(f.get_instance_pool<process_tag>());
	const port_alias_tracker& pat(f.get_scope_alias_tracker());
	typedef port_alias_tracker_base<process_tag>::map_type  pmap_type;
	const pmap_type& ppa(pat.get_id_map<process_tag>());
	// here we want to include processes that are in ports
	NEVER_NULL(parent_offset);
	global_offset sgo(*parent_offset, f, add_local_private_tag());
	footprint_frame lff;
	lff.construct_global_context(f, *fpf, *parent_offset);
	const value_saver<const global_offset*> __gs__(parent_offset, &sgo);
	// copy and increment with each local process
	// first, loop over ports
	const size_t lpe = lpp.port_entries();
	size_t pi = 0;		// isn't the first entry NULL?
	STACKTRACE_INDENT_PRINT("iterating over " << lpe << " ports" << endl);
	for ( ; pi < lpe; ++pi) {
#if 0 && GLOBAL_CONTEXT_GPID
		STACKTRACE_INDENT_PRINT("local pi = " << pi <<
			", owner gpid = " << _gpid << endl);
		// keep _gpid the same, same parent
#endif
		const pmap_type::const_iterator ai(ppa.find(pi+1));
	if (ai != ppa.end()) {
		const state_instance<process_tag>& sp(lpp[pi]);
		const footprint_frame& spf(sp._frame);
		const footprint& sfp(*spf._footprint);
		const footprint_frame af(spf, lff);     // context
		const footprint_frame_setter ffs(fpf, &af);
		STACKTRACE_INDENT_PRINT("index: " << pi+1 << endl);
		const alias_reference_set<process_tag>& pars(ai->second);
		alias_reference_set<process_tag>::const_iterator
			pai(pars.begin()), pae(pars.end());
		for ( ; pai!=pae; ++pai) {
			NEVER_NULL(*pai);
			// but we want public ports of parents
			if (!(*pai)->is_subinstance()) {
				ostringstream oss;
				(*pai)->dump_hierarchical_name(oss, 
					dump_flags::no_owners);
				local_name = oss.str();
				STACKTRACE_INDENT_PRINT("visit: " << local_name << endl);
#if ENABLE_STACKTRACE
				sfp.dump_type(STACKTRACE_INDENT_PRINT("type: ")) << endl;
#endif
				sfp.accept(*this);
			} else {
#if ENABLE_STACKTRACE
				(*pai)->dump_hierarchical_name(
					STACKTRACE_INDENT_PRINT("skip : "), 
					dump_flags::no_owners) << endl;
#endif
			}
		}
	}
//              sgo += sfp;	// no offset change, still same owner
	}
//	STACKTRACE_INDENT_PRINT("pi = " << pi << endl);
#if 0 && GLOBAL_CONTEXT_GPID
	size_t ppo = parent_offset->get_offset<process_tag>() +1;
#endif  
	const size_t pe = lpp.local_entries();
	STACKTRACE_INDENT_PRINT("iterating over " << pe-lpe << " locals" << endl);
	INVARIANT(pi == lpe);
//	const value_saver<const global_offset*> __gs__(parent_offset, &sgo);
	for ( ; pi<pe; ++pi) {	// ++ppo;
#if 0 && GLOBAL_CONTEXT_GPID
		STACKTRACE_INDENT_PRINT("local pi = " << pi <<
			", owner gpid = " << ppo << endl);
		const value_saver<size_t> __save_gpid(_gpid, ppo);
#endif
		const state_instance<process_tag>& sp(lpp[pi]);
		const footprint_frame& spf(sp._frame);
		const footprint& sfp(*spf._footprint);
		const footprint_frame af(spf, lff);     // context
		const footprint_frame_setter ffs(fpf, &af); 
		const pmap_type::const_iterator
			ai(ppa.find(pi+1));
		if (ai != ppa.end()) {
		STACKTRACE_INDENT_PRINT("index: " << pi+1 << endl);
		const alias_reference_set<process_tag>& pars(ai->second);
		alias_reference_set<process_tag>::const_iterator
			pai(pars.begin()), pae(pars.end());
		for ( ; pai!=pae; ++pai) {
			NEVER_NULL(*pai);
			if (!(*pai)->is_subinstance()) {
				ostringstream oss;
				(*pai)->dump_hierarchical_name(oss, 
					dump_flags::no_owners);
				local_name = oss.str();
				STACKTRACE_INDENT_PRINT("visit: " << local_name << endl);
#if ENABLE_STACKTRACE
				sfp.dump_type(STACKTRACE_INDENT_PRINT("type: ")) << endl;
#endif
				sfp.accept(*this);
			} else {
#if ENABLE_STACKTRACE
				(*pai)->dump_hierarchical_name(
					STACKTRACE_INDENT_PRINT("skip : "), 
					dump_flags::no_owners) << endl;
#endif
			}
		}
		}
		sgo += sfp;
	}
}	// end visit_recursive

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

}	// end namespace entity
}	// end namespace HAC

