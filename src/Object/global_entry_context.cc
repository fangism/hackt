/**
	\file "Object/global_entry_context.cc"
	$Id: global_entry_context.cc,v 1.4.46.9 2010/03/06 00:32:52 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include "Object/global_entry_context.tcc"
#include "Object/module.h"
#include "Object/global_entry.h"
#include "Object/traits/proc_traits.h"	// for process_tag
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
#include <iostream>
#include "Object/global_channel_entry.h"
#include "Object/def/footprint.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/type/canonical_fundamental_chan_type.h"
#include "Object/inst/state_instance.h"
#include "Object/inst/instance_pool.h"
#include "Object/traits/instance_traits.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/alias_actuals.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/general_collection_type_manager.h"
#include "Object/inst/instance_placeholder.h"
#include "Object/inst/physical_instance_placeholder.h"
#include "Object/ref/member_meta_instance_reference.h"
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

#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
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
#endif

//=============================================================================
// class global_entry_context method definitions

#if MEMORY_MAPPED_GLOBAL_ALLOCATION
global_entry_context::global_entry_context(const footprint_frame& ff, 
		const global_offset& g) :
		global_entry_context_base(*ff._footprint), 
		fpf(&ff), parent_offset(&g) {
	NEVER_NULL(topfp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
global_entry_context::~global_entry_context() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const footprint&
global_entry_context::get_current_footprint(void) const {
	if (fpf) {
		NEVER_NULL(fpf->_footprint);
		return *fpf->_footprint;
	} else	return *topfp;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if current context is top-most level.
 */
bool
global_entry_context::at_top(void) const {
	return (topfp == fpf->_footprint);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
global_entry_context::dump_context(ostream& o) const {
	o << "frame: ";
	if (fpf) {
		fpf->dump_frame(o) << endl;
	} else {
		o << "NULL" << endl;
	}
	o << "offset: ";
	if (parent_offset) {
		o << *parent_offset << endl;
	} else {
		o << "NULL" << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Similar traversal to footprint::get_instance<>().
	\param _gpid global process index 1-based.  
		pass 0 to indicate top-level.
	NOTE: parent_offset and fpf are only used once in this impl.
 */
void
global_entry_context::construct_global_footprint_frame(footprint_frame& ret, 
		global_offset& g, size_t gpid) const {
	STACKTRACE_VERBOSE;
	typedef	process_tag				Tag;
	typedef	state_instance<Tag>::pool_type		pool_type;
//	ret = *fpf;		// initialize to top-level (scratch space)
	ret.construct_top_global_context(*topfp, g);
	g = *parent_offset;
#if ENABLE_STACKTRACE
	ret.dump_frame(STACKTRACE_INDENT_PRINT("top:")) << endl;
#endif
if (gpid) {
	// iterative instead of recursive implementation, hence pointers
	const footprint* cf = fpf->_footprint;
	const pool_type* p = &cf->get_instance_pool<Tag>();
	size_t ports = 0;			// at_top
	size_t local = p->local_entries();	// at_top
	STACKTRACE_INDENT_PRINT("ports = " << ports << endl);
	STACKTRACE_INDENT_PRINT("local = " << local << endl);
	STACKTRACE_INDENT_PRINT("gpid = " << gpid << endl);
	STACKTRACE_INDENT_PRINT("offset = " << g << endl);
	while (gpid > local) {
		STACKTRACE_INDENT_PRINT("descending..." << endl);
		const size_t si = gpid -local;	// 1-based index
		if (cf == topfp) {
			g = global_offset(g, *cf, add_all_local_tag());
		} else {
			g = global_offset(g, *cf, add_local_private_tag());
		}
		const pool_private_map_entry_type&
			e(p->locate_private_entry(si));
		const size_t lpid = e.first;
		global_offset delta;
		cf->set_global_offset_by_process(delta, lpid);
		delta += g;
		const state_instance<Tag>& sp((*p)[lpid -1]);
		const footprint_frame& sff(sp._frame);
		cf = sff._footprint;
		gpid = si -e.second;
		footprint_frame lff(sff, ret);
		lff.extend_frame(g, delta);
	//	lff.construct_global_context(*cf, ret, g);
#if ENABLE_STACKTRACE
		lff.dump_frame(STACKTRACE_INDENT_PRINT("frame:")) << endl;
#endif
		lff.swap(ret);
		g = delta;
		p = &cf->get_instance_pool<Tag>();
		ports = p->port_entries();
		local = p->local_private_entries();
		STACKTRACE_INDENT_PRINT("ports = " << ports << endl);
		STACKTRACE_INDENT_PRINT("local = " << local << endl);
		STACKTRACE_INDENT_PRINT("gpid = " << gpid << endl);
		STACKTRACE_INDENT_PRINT("offset = " << g << endl);
	}
	const size_t lpid = gpid;
	STACKTRACE_INDENT_PRINT("lpid = " << lpid << endl);
	p = &cf->get_instance_pool<Tag>();
	const state_instance<Tag>& sp((*p)[lpid -1]);
	const footprint_frame& sff(sp._frame);
#if ENABLE_STACKTRACE
	sff.dump_frame(STACKTRACE_INDENT_PRINT("sff:")) << endl;
	ret.dump_frame(STACKTRACE_INDENT_PRINT("actuals:")) << endl;
#endif
	footprint_frame lff(sff, ret);
	global_offset delta;
	cf->set_global_offset_by_process(delta, lpid);
	delta += g;
	lff.extend_frame(g, delta);
#if ENABLE_STACKTRACE
	lff.dump_frame(STACKTRACE_INDENT_PRINT("frame:")) << endl;
#endif
	lff.swap(ret);
	// keep g
}
	// else refers to top-level
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursive implementation due to member instance reference structuring.
	\return local pid of returned process frame, 0 to signal an error.
 */
size_t
global_entry_context::construct_global_footprint_frame(
		footprint_frame& owner, 
		footprint_frame& ret, global_offset& g, 
		const meta_instance_reference_base& pr) const {
	STACKTRACE_VERBOSE;
	typedef	process_tag			Tag;
	typedef	simple_meta_instance_reference<Tag>	simple_ref;
	typedef	member_meta_instance_reference<Tag>	member_ref;
	const meta_instance_reference_base* prp = &pr;
	const simple_ref* spr = IS_A(const simple_ref*, prp);
	if (!spr) {
		cerr << "Parent is not a process, I give up!" << endl;
		// TODO: more informative error message
		return true;
	}
	// check for scalar reference
	if (spr->dimensions()) {
		cerr << "Parent reference must be scalar." << endl;
		return true;
	}
	const member_ref* mpr = IS_A(const member_ref*, spr);
	if (mpr) {
#if ENABLE_STACKTRACE
		mpr->dump(STACKTRACE_INDENT_PRINT("have a member-ref: "), 
			expr_dump_context::default_value) << endl;
#endif
		// in a sub-process, sets ret, owner and g
		const size_t ppid =
			construct_global_footprint_frame(owner, ret, g,
				*mpr->get_base_ref());
		if (!ppid) {
			// have some error
			return 0;
		}
#if ENABLE_STACKTRACE
		mpr->dump(STACKTRACE_INDENT_PRINT("back to member-ref: "), 
			expr_dump_context::default_value) << endl;
#endif
		const unroll_context uc(ret._footprint, topfp);
		// yes, use base-class method, not virtual override
		const size_t lpid = mpr->simple_ref::lookup_locally_allocated_index(uc);
		STACKTRACE_INDENT_PRINT("ppid = " << ppid << endl);
		const footprint& rfp(*ret._footprint);
		const footprint& ofp(*owner._footprint);
		const state_instance<Tag>::pool_type&
			pp(rfp.get_instance_pool<Tag>());
		const size_t ports = pp.port_entries();
		STACKTRACE_INDENT_PRINT("lpid = " << lpid << " (" << ports
			<< ") ports" << endl);
		const state_instance<Tag>& sp(pp[lpid -1]);
		if (lpid >= ports) {
			STACKTRACE_INDENT_PRINT("private local" << endl);
			// then is local private
			// change both ret frame and owner, and global offset
			global_offset delta;
			ofp.set_global_offset_by_process(delta, ppid);
			STACKTRACE_INDENT_PRINT("delta: " << delta << endl);
			global_offset sgo;
			if (&ofp == topfp) {
				sgo = global_offset(g, ofp, add_all_local_tag());
			} else {
				sgo = global_offset(g, ofp, add_local_private_tag());
			}
			STACKTRACE_INDENT_PRINT("sgo: " << sgo << endl);
			delta += sgo;
			owner.construct_global_context(*sp._frame._footprint, 
				ret, delta);
			g = delta;
			ret = owner;
#if ENABLE_STACKTRACE
			sp._frame.dump_frame(STACKTRACE_INDENT_PRINT("sp.frame:")) << endl;
			owner.dump_frame(STACKTRACE_INDENT_PRINT("new owner:")) << endl;
			STACKTRACE_INDENT_PRINT("offset: " << g << endl);
			ret.dump_frame(STACKTRACE_INDENT_PRINT("ret:")) << endl;
#endif
			return lpid;
		} else {
			STACKTRACE_INDENT_PRINT("public port" << endl);
			// then is public port, only change ret frame
			// no change in global offset
			footprint_frame lff(sp._frame, owner);
#if ENABLE_STACKTRACE
			lff.dump_frame(STACKTRACE_STREAM) << endl;
#endif
			ret.swap(lff);
			return sp._frame.get_frame_map<Tag>()[lpid -1];
		}
	} else {
		STACKTRACE_INDENT_PRINT("at top-level: ");
		// we're at top level
		g = *parent_offset;
		owner.construct_top_global_context(*topfp, g);
#if ENABLE_STACKTRACE
		owner.dump_frame(STACKTRACE_STREAM << "owner:") << endl;
#endif
		const unroll_context uc(topfp, topfp);
		const size_t gpid = spr->lookup_locally_allocated_index(uc);
		STACKTRACE_INDENT_PRINT("gpid = " << gpid << endl);
		construct_global_footprint_frame(ret, g, gpid);
#if ENABLE_STACKTRACE
		ret.dump_frame(STACKTRACE_STREAM << "process:") << endl;
#endif
		return gpid;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This visits only private-local instances, skipping the ports, 
	*except* for the top-level footprint, which includes the ports.
	The default top-level has no ports, but user-overridden top-types
	will almost certainly have ports.  
	\param include_ports usually equal to at_top(), which is when
		one typically wants to traverse ports (for uniqueness).
 */
template <class Tag>
void
global_entry_context::visit_local(const footprint& f,
		const bool include_ports) {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	f.dump_type(STACKTRACE_STREAM << "type: ") << endl;
#endif
	const typename state_instance<Tag>::pool_type&
		_pool(f.get_instance_pool<Tag>());
	const bool is_top = at_top();
	// construct context (footprint_frame)
	footprint_frame lff;
	global_offset sgo(*parent_offset, f, add_local_private_tag());
	NEVER_NULL(parent_offset);
// this setup is done twice, also for visit_recursive
// possible to re-factor and setup once?
// something different for the top-level with ports...
if (is_top) {
	lff.construct_top_global_context(f, *parent_offset);
	sgo = global_offset(*parent_offset, f, add_all_local_tag());
} else {
	lff.construct_global_context(f, *fpf, *parent_offset);
}
	// print local, non-port entries, since ports belong to 'parent'
	// for processes duplicate work computing global offsets twice
	const value_saver<global_offset*> __gs__(g_offset, &sgo);
	const footprint_frame_setter __ffs__(fpf, &lff);	// local actuals
	const size_t l = _pool.local_entries();
	// but for the top-level only, we want start with ports
	size_t i = include_ports ? 0 : _pool.port_entries();
	for ( ; i<l; ++i) {
		// global and local index can be deduced
		const state_instance<Tag>& inst(_pool[i]);
		inst.accept(*this);
		// visitor will increment g_offset
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Hierarchical alias traversal.
 */
void
global_entry_context::visit_aliases(const footprint& f) {
	f.accept(IS_A(alias_visitor&, *this));	// dynamic_cast
}
#endif

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
	TODO: parameter for include_ports.
 */
void
global_entry_context::visit_recursive(const footprint& f) {
        STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("in process type: ");
	f.dump_type(STACKTRACE_STREAM) << endl;
	STACKTRACE_INDENT_PRINT("offset: " << *parent_offset << endl);
	fpf->dump_frame(STACKTRACE_STREAM << "frame:\n") << endl;
#endif
	// recurse through processes and print
	const state_instance<process_tag>::pool_type&
		lpp(f.get_instance_pool<process_tag>());
	const bool is_top = at_top();
	// TODO: alternatively, construct lookup table of lpid->gpid ahead
	// must skip processes that are in ports
	NEVER_NULL(parent_offset);
	global_offset sgo(*parent_offset, f, add_local_private_tag());
	footprint_frame lff;
if (is_top) {
	lff.construct_top_global_context(f, *parent_offset);
	sgo = global_offset(*parent_offset, f, add_all_local_tag());
} else {
	lff.construct_global_context(f, *fpf, *parent_offset);
}
	// copy and increment with each local process
	const size_t pe = lpp.local_entries();
	// but for the top-level only, we want start with ports (process?)
	size_t pi = is_top ? 0 : lpp.port_entries();
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
#endif	// MEMORY_MAPPED_GLOBAL_ALLOCATION

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
#endif	// MEMORY_MAPPED_GLOBAL_ALLOCATION

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

