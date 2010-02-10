/**
	\file "Object/global_entry_context.cc"
	$Id: global_entry_context.cc,v 1.4.46.2 2010/02/10 06:42:59 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include "Object/global_entry_context.h"
#include "Object/module.h"
#include "Object/global_entry.h"
#include "Object/traits/proc_traits.h"	// for process_tag
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
#include <iostream>
#include "Object/global_channel_entry.h"
#include "Object/def/footprint.h"
#include "Object/type/canonical_fundamental_chan_type.h"
#include "Object/inst/state_instance.h"
#include "Object/inst/instance_pool.h"
#include "Object/traits/instance_traits.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/alias_actuals.h"
#include "util/stacktrace.h"
#include "util/indent.h"
#include "util/value_saver.h"
#endif

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using util::value_saver;
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
typedef	util::value_saver<const footprint_frame*>	footprint_frame_setter;
#endif

//=============================================================================
// class global_entry_context_base method definitions

//-----------------------------------------------------------------------------
// class global_entry_context_base::module_setter method definitions

global_entry_context_base::module_setter::module_setter(
		global_entry_context_base& _ccb, 
		const module& m) :
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		manager_saver_type(_ccb, &m.get_state_manager()),
#endif
		footprint_saver_type(_ccb, &m.get_footprint()) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
global_entry_context_base::module_setter::~module_setter() { }

//=============================================================================
// class global_entry_context method definitions

#if MEMORY_MAPPED_GLOBAL_ALLOCATION
global_entry_context::~global_entry_context() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
global_entry_context::visit_local(const footprint& f) {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	f.dump_type(STACKTRACE_STREAM << "type: ") << endl;
#endif
	const typename state_instance<Tag>::pool_type&
		_pool(f.get_instance_pool<Tag>());
	// construct context (footprint_frame)
	footprint_frame lff;
	NEVER_NULL(parent_offset);
	lff.construct_global_context(f, *fpf, *parent_offset);
	// print local, non-port entries, since ports belong to 'parent'
	// for processes duplicate work computing global offsets twice
	global_offset sgo(*parent_offset, f, add_local_private_tag());
	const value_saver<global_offset*> __gs__(g_offset, &sgo);
	const footprint_frame_setter __ffs__(fpf, &lff);	// local actuals
	const size_t l = _pool.local_entries();
	size_t i = _pool.port_entries();
	for ( ; i<l; ++i) {
		// global and local index can be deduced
		const state_instance<Tag>& inst(_pool[i]);
		// __visit(inst);
		inst.accept(*this);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// default visitors do nothing, override in subclasses
void
global_entry_context::visit(const state_instance<process_tag>& p) { }

void
global_entry_context::visit(const state_instance<channel_tag>& p) { }

void
global_entry_context::visit(const state_instance<enum_tag>& p) { }

void
global_entry_context::visit(const state_instance<int_tag>& p) { }

void
global_entry_context::visit(const state_instance<bool_tag>& p) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively traverses processes, setting up footprint frame context 
	at each level.
 */
void
global_entry_context::visit_recursive(const footprint& f) {
        STACKTRACE_VERBOSE;
	// recurse through processes and print
	const state_instance<process_tag>::pool_type&
		lpp(f.get_instance_pool<process_tag>());
	// TODO: alternatively, construct lookup table of lpid->gpid ahead
	// must skip processes that are in ports
	NEVER_NULL(parent_offset);
	global_offset sgo(*parent_offset, f, add_local_private_tag());
	footprint_frame lff;
	lff.construct_global_context(f, *fpf, *parent_offset);
	// copy and increment with each local process
	size_t pi = lpp.port_entries();
	const size_t pe = lpp.local_entries();
	const value_saver<const global_offset*> __gs__(parent_offset, &sgo);
	for ( ; pi<pe; ++pi) {
		const state_instance<process_tag>& sp(lpp[pi]);
		const footprint_frame& spf(sp._frame);
		const footprint& sfp(*spf._footprint);
		const footprint_frame af(spf, lff);     // context
		const footprint_frame_setter ffs(fpf, &af);
		// really wants to be conditional, but would depend on <Tag>
#if 0
		if (sfp.get_instance_pool<Tag>().total_private_entries()) {
			sfp.__dump_local_map_by_process<Tag>(o, topfp,
				af, next_ppid, sgo);
		}
#else
		sfp.accept(*this);
#endif
		sgo += sfp;
	}
	// invariant checks on sgo, consistent with local instance_pools
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	By default, visit locally before visiting recursively.
	This doesn't do very much, other than walk the hierarchy.
	This doesn't visit any local instances.  
	TODO: provide a unified visit routine.
 */
void
global_entry_context::visit(const footprint& f) {
	STACKTRACE_VERBOSE;
//	visit_local<Tag>(f);		// no Tag available in this context
	visit_recursive(f);
}
#endif

//=============================================================================
// class global_entry_context::footprint_frame_setter method definitions

#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
global_entry_context::footprint_frame_setter::footprint_frame_setter(
		global_entry_context& _cc, const footprint_frame& _ff) :
		frame_saver_type(_cc, &_ff) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param pid a global process index, corresponding to the 
		processes, whose footprint-frame should be referenced.
 */
global_entry_context::footprint_frame_setter::footprint_frame_setter(
		global_entry_context& _cc, const size_t pid) :
		frame_saver_type(_cc, 
			pid ? &_cc.get_state_manager()->
				get_pool<process_tag>()[pid]._frame
			: NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
global_entry_context::footprint_frame_setter::~footprint_frame_setter() { }
#endif

//=============================================================================
const char
global_entry_dumper::table_header[] =
"globID\tsuper\t\tlocalID\tcanonical\tfootprint-frame";

#if MEMORY_MAPPED_GLOBAL_ALLOCATION
global_entry_dumper::~global_entry_dumper() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
global_entry_dumper::visit(const footprint& f) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("PID = " << pid << endl);
{
	const value_saver<size_t> __ppid__(pid);
	NEVER_NULL(parent_offset);
	pid = parent_offset->global_offset_base<process_tag>::offset +1;
	global_entry_context::visit(f);
}
	++pid;
	STACKTRACE_INDENT_PRINT("++PID = " << pid << endl);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
global_entry_dumper::__default_visit(const state_instance<Tag>& p) {
	STACKTRACE_VERBOSE;
	// ripped from global_entry<Tag>::dump_base()
	const size_t local_offset = p.get_back_ref()->instance_index;
	NEVER_NULL(parent_offset);
	const size_t toffset = parent_offset->global_offset_base<Tag>::offset;
	// need old offset
	const size_t p_offset =
		fpf->_footprint->get_instance_pool<Tag>().port_entries();
	// already 1-based
	const size_t global_index = toffset +local_offset -p_offset;
	NEVER_NULL(global_index);
	os << global_index << '\t';
	if (pid) {
		os << "process\t" << pid << '\t';
	} else {
		os << "(top)\t-\t";
	}
	os << local_offset << '\t';
	const size_t gid = global_index -1;	// 0-based
	topfp->dump_canonical_name<Tag>(os, gid) << '\t';
	p.get_back_ref()->dump_attributes(os);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
global_entry_dumper::visit(const state_instance<process_tag>& inst) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(g_offset);
	const global_offset& sgo(*g_offset);
	const footprint_frame& spf = *inst.get_frame();
	const footprint& sfp(*spf._footprint);
	const global_offset
		b(sgo, sfp, add_local_private_tag()),
		c(sgo, sfp, add_total_private_tag());
	STACKTRACE_INDENT_PRINT('\t' << sgo << b << c << endl);
	__default_visit(inst);
	// context, but extended to include locals
	const footprint_frame af(spf, *fpf);    // context
	// type: strict/relaxed distinction
#if 0
	// sfp.dump_type(o);            // type
	sfp.dump_footprint<process_tag>(*this);
#else
	typedef	instance_alias_info<process_tag>	alias_type;
	alias_type::dump_complete_type(*inst.get_back_ref(),
		os, inst._frame._footprint);
#endif
	const util::indent __tab__(os, "\t");
	af.dump_extended_frame(os, sgo, b, c) << endl;
	*g_offset = c;		// increment global offset with each process
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
global_entry_dumper::visit(const state_instance<channel_tag>& p) {
	__default_visit(p);
	p.channel_type->dump(os);
	os << endl;
}

void
global_entry_dumper::visit(const state_instance<enum_tag>& p) {
	__default_visit(p);
	os << endl;
}

void
global_entry_dumper::visit(const state_instance<int_tag>& p) {
	__default_visit(p);
	os << endl;
}

void
global_entry_dumper::visit(const state_instance<bool_tag>& p) {
	__default_visit(p);
	os << endl;
}

//-----------------------------------------------------------------------------
// class global_allocation_dumper method definitions

/**
	Only visit one type at a time.  
 */
template <class Tag>
void
global_allocation_dumper<Tag>::visit(const footprint& f) {
	STACKTRACE_VERBOSE;
if (f.get_instance_pool<Tag>().total_private_entries()) {
	visit_local<Tag>(f);
	global_entry_dumper::visit(f);
} else {
	++pid;		// still need to increment local pid counter
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// explicit template instantiations
template class global_allocation_dumper<process_tag>;
template class global_allocation_dumper<channel_tag>;
template class global_allocation_dumper<enum_tag>;
template class global_allocation_dumper<int_tag>;
template class global_allocation_dumper<bool_tag>;
#endif	// MEMORY_MAPPED_GLOBAL_ALLOCATION

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

