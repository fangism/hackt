/**
	\file "Object/global_entry_dumper.cc"
	$Id: global_entry_dumper.cc,v 1.1 2010/04/07 00:12:29 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include "Object/global_entry_dumper.h"
#include "Object/module.h"
#include "Object/global_entry.h"
#include "Object/global_channel_entry.h"
#include "Object/def/footprint.h"
#include "Object/type/canonical_fundamental_chan_type.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/inst/state_instance.h"
#include "Object/inst/instance_pool.h"
#include "Object/traits/instance_traits.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/alias_actuals.h"
#include "util/stacktrace.h"
#include "util/indent.h"
#include "util/value_saver.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using util::value_saver;

//=============================================================================
const char
global_entry_dumper::table_header[] =
"globID\tsuper\t\tlocalID\tcanonical\tfootprint-frame";

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
	// this takes care of the case where we include top-level ports
	const size_t global_index = lookup_global_id<Tag>(local_offset);
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
/**
	TODO: refactor the procedure to build the extended frame
 */
void
global_entry_dumper::visit(const state_instance<process_tag>& inst) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(g_offset);
	const global_offset& sgo(*g_offset);
	const footprint_frame& spf(*inst.get_frame());
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
	// this should be equivalent to footprint_frame::extend_frame
#if 0
	af.extend_frame(sgo, b);
#endif
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
	const typename state_instance<Tag>::pool_type&
		_pool(f.get_instance_pool<Tag>());
	const bool is_top = at_top();
if (_pool.total_private_entries() || (is_top && _pool.port_entries())) {
	visit_local<Tag>(f, is_top);
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

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

