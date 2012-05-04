/**
	\file "Object/global_entry_context.cc"
	$Id: global_entry_context.cc,v 1.14 2011/05/17 21:19:52 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include "Object/global_entry_context.tcc"
#include "Object/module.h"
#include "Object/global_entry.h"
#include "Object/global_channel_entry.h"
#include "Object/global_context_cache.h"
#include "Object/def/footprint.h"
#include "Object/def/datatype_definition_base.h"
#include "Object/expr/expr_dump_context.h"
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

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using util::value_saver;
typedef	value_saver<const footprint_frame*>	footprint_frame_setter;

//=============================================================================
// class global_entry_context_base method definitions

//=============================================================================
// class global_entry_context method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
global_entry_context::global_entry_context(const global_process_context& p) :
		global_entry_context_base(*p.frame._footprint), 
		fpf(&p.frame), parent_offset(&p.offset), 
		g_offset(NULL)
#if GLOBAL_CONTEXT_GPID
		, _gpid(0)
#endif
{
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
void
global_entry_context::set_global_context(const global_process_context& c) {
	fpf = &c.frame;
	parent_offset = &c.offset;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
global_entry_context::dump_context(ostream& o) const {
	o << "frame: ";
	if (fpf) {
		fpf->dump_frame(o) << endl;
		fpf->_footprint->dump_type(o << "type: ") << endl;
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
#if MODULE_OWNS_CONTEXT_CACHE
// call global_context_cache::get_global_context() instead
#else
/**
	Similar traversal to footprint::get_instance<>().
	Only called by global_process_context::global_process_context(
		const module&, const size_t)
	\param gpid global process index 1-based.  
		pass 0 to indicate top-level.
	NOTE: parent_offset and fpf are only used once in this impl.
		g_offset member is never used here.
 */
void
global_entry_context::construct_global_footprint_frame(
		global_process_context& gpc,
		size_t gpid) const {
	STACKTRACE_VERBOSE;
	typedef	process_tag				Tag;
	typedef	state_instance<Tag>::pool_type		pool_type;
	footprint_frame& ret(gpc.frame);
	global_offset& g(gpc.offset);
//	ret = *fpf;		// initialize to top-level (scratch space)
	ret.construct_top_global_context(*topfp, g);
	g = *parent_offset;
#if ENABLE_STACKTRACE
	gpc.dump_frame(STACKTRACE_INDENT_PRINT("top:")) << endl;
#endif
if (gpid) {
	// iterative instead of recursive implementation, hence pointers
	const footprint* cf = fpf->_footprint;
	const pool_type* p = &cf->get_instance_pool<Tag>();
	size_t local = p->local_entries();	// at_top
	STACKTRACE_INDENT_PRINT("local = " << local << endl);
	STACKTRACE_INDENT_PRINT("gpid(1) = " << gpid << endl);
//	STACKTRACE_INDENT_PRINT("offset = " << g << endl);
	while (gpid > local) {
		STACKTRACE_INDENT_PRINT("descending..." << endl);
		const size_t si = gpid -local;	// 1-based index
		STACKTRACE_INDENT_PRINT("rem(1) = " << si << endl);
		const pool_private_map_entry_type&
			e(p->locate_private_entry(si -1));	// need 0-base!
		const size_t lpid = e.first;
		STACKTRACE_INDENT_PRINT("lpid(1) = " << lpid << endl);
		gpid = si -e.second;		// still 1-based
		gpc.descend_frame(lpid, cf == topfp);
		cf = gpc.frame._footprint;
		p = &cf->get_instance_pool<Tag>();
		local = p->local_private_entries();
		STACKTRACE_INDENT_PRINT("local = " << local << endl);
		STACKTRACE_INDENT_PRINT("gpid(1) = " << gpid << endl);
	}
	STACKTRACE_INDENT_PRINT("found owner scope" << endl);
	const size_t ports = p->port_entries();
	STACKTRACE_INDENT_PRINT("ports = " << ports << endl);
	const size_t lpid = gpid +ports;
	STACKTRACE_INDENT_PRINT("lpid(1) = " << lpid << endl);
//	STACKTRACE_INDENT_PRINT("offset = " << g << endl);
	gpc.descend_frame(lpid, cf == topfp);
}
	// else refers to top-level
}	// end global_entry_context::construct_global_footprint_frame
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapped call.
 */
bool
global_entry_context::construct_global_footprint_frame(
		const footprint& top,
		const meta_instance_reference_base& pr, 
		global_process_context_id& ret) {
	const unroll_context lookup_c(&top, &top);      // any loop variables?
	return construct_global_footprint_frame(top, pr, lookup_c, ret);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapped call.
	unroll_context is pased in manually, in case of override.
 */
bool
global_entry_context::construct_global_footprint_frame(
		const footprint& top,
		const meta_instance_reference_base& pr, 
		const unroll_context& c,
		global_process_context_id& ret) {
	const global_process_context gpc(top);
	const global_entry_context gc(gpc);
	return gc.construct_global_footprint_frame(pr, c, ret);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Publicly convenience-wrapped call.
 */
bool
global_entry_context::construct_global_footprint_frame(
		const meta_instance_reference_base& pr, 
		const unroll_context& uc,
		global_process_context_id& ret) const {
	global_process_context tmp;
	return construct_global_footprint_frame(tmp, ret, pr, uc);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if AGGREGATE_PARENT_REFS
/**
	Wrapped call.
	\return true on error
 */
bool
global_entry_context::construct_global_footprint_frames(
		const footprint& top,
		const meta_instance_reference_base& pr, 
		vector<global_process_context_id>& retf) {
	const unroll_context lookup_c(&top, &top);      // any loop variables?
	return construct_global_footprint_frames(
		top, pr, lookup_c, retf);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapped call.
	unroll_context is pased in manually, in case of override.
 */
bool
global_entry_context::construct_global_footprint_frames(
		const footprint& top,
		const meta_instance_reference_base& pr, 
		const unroll_context& c,
		vector<global_process_context_id>& retf) {
	const global_process_context gpc(top);
	const global_entry_context gc(gpc);
	return gc.construct_global_footprint_frames(pr, c, retf);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Publicly convenience-wrapped call.
 */
bool
global_entry_context::construct_global_footprint_frames(
		const meta_instance_reference_base& pr, 
		const unroll_context& uc,
		vector<global_process_context_id>& retf) const {
	vector<global_process_context> tmpo;
	return construct_global_footprint_frames(
		tmpo, retf, pr, uc);
}
#endif	// AGGREGATE_PARENT_REFS

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursive implementation due to member instance reference structuring.
	\param pr must be a scalar process reference (can be hierarchical).
	\return local pid of returned process frame, 0 to signal an error.
		owner is the frame and offset of the current owner frame
		ret is the frame and offset of the current return frame
		(both returned by reference, not used when passed)
		Caller should choose which offset is used.
	\return true on error.
 */
bool
global_entry_context::construct_global_footprint_frame(
		global_process_context& owner,
		global_process_context_id& ret,
		const meta_instance_reference_base& pr, 
		const unroll_context& uc) const {
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
		// in a sub-process, sets ret and owner
		if (construct_global_footprint_frame(owner, ret,
				*mpr->get_base_ref(), uc)) {
			// have some error
			return true;
		}
		// const size_t ppid = ret.gpid;
#if ENABLE_STACKTRACE
		mpr->dump(STACKTRACE_INDENT_PRINT("back to member-ref: "), 
			expr_dump_context::default_value) << endl;
//		STACKTRACE_INDENT_PRINT("ppid = " << ppid << endl);
		STACKTRACE_INDENT_PRINT("owner.offset: " << owner.offset << endl);
		STACKTRACE_INDENT_PRINT("ret.offset: " << ret.offset << endl);
		ret.dump_frame(STACKTRACE_INDENT_PRINT("ret.frame:")) << endl;
#endif
		// want footprint of the member-owner, not the context passed in
		const unroll_context tc(ret.frame._footprint, topfp);
		// yes, use base-class method, not virtual override
		const size_t lpid =	// is 1-based index
			mpr->simple_ref::lookup_locally_allocated_index(tc);
		const footprint& rfp(*ret.frame._footprint);
		const state_instance<Tag>::pool_type&
			pp(rfp.get_instance_pool<Tag>());
		const size_t ports = pp.port_entries();
		const size_t locals = pp.local_entries();
		STACKTRACE_INDENT_PRINT("lpid = " << lpid << " (" << ports
			<< " ports)" << endl);
		if (!lpid) {
			// have error
			return true;
		}
		INVARIANT(lpid <= locals);
		if (lpid > ports) {	// b/c 1-based index
			// then is local private
			// change both ret frame and owner, and global offset
			STACKTRACE_INDENT_PRINT("private local" << endl);
			const state_instance<Tag>& sp(pp[lpid -1]);
			owner.offset = ret.offset;
			STACKTRACE_INDENT_PRINT("owner.offset <- ret.offset: "
				<< owner.offset << endl);
			global_offset pdelta;
		{
			// compute offset relative to immediate parent
			global_offset z;
			z = global_offset(z, rfp, add_local_private_tag());
			STACKTRACE_INDENT_PRINT("r-delta: " << z << endl);
			rfp.set_global_offset_by_process(pdelta, lpid);
			STACKTRACE_INDENT_PRINT("pdelta: " << pdelta << endl);
			pdelta += z;
			STACKTRACE_INDENT_PRINT("pdelta+z: " << pdelta << endl);
			pdelta += owner.offset;
			STACKTRACE_INDENT_PRINT("pdelta+z+o: " << pdelta << endl);
		}
			owner.frame = ret.frame;
			footprint_frame lff(sp._frame, ret.frame);
			ret.frame.construct_global_context(*sp._frame._footprint, lff, pdelta);
			ret.offset = pdelta;
#if ENABLE_STACKTRACE
			sp._frame.dump_frame(STACKTRACE_INDENT_PRINT("sp.frame:")) << endl;
			owner.dump_frame(
				STACKTRACE_INDENT_PRINT("new owner.frame:")) << endl;
			STACKTRACE_INDENT_PRINT("owner.offset: " << owner.offset << endl);
			ret.dump_frame(STACKTRACE_INDENT_PRINT("ret.frame:")) << endl;
#endif
			ret.gpid = lpid;
		} else {
			STACKTRACE_INDENT_PRINT("public port" << endl);
			// then is public port, only change ret frame
			// keep same owner frame
			// no change in global offset
			const state_instance<Tag>& sp(pp[lpid -1]);
			footprint_frame rff(sp._frame, ret.frame);
#if ENABLE_STACKTRACE
			sp._frame.dump_frame(STACKTRACE_INDENT_PRINT("sp.frame formal:")) << endl;
			owner.dump_frame(STACKTRACE_INDENT_PRINT("owner.frame:")) << endl;
			ret.dump_frame(STACKTRACE_INDENT_PRINT("parent.frame:")) << endl;
			rff.dump_frame(STACKTRACE_INDENT_PRINT("rff:")) << endl;
//			lff.dump_frame(STACKTRACE_INDENT_PRINT("lff:")) << endl;
#endif
			ret.frame.construct_global_context(
				*sp._frame._footprint, rff, ret.offset);
			// ret.offset = ...;	// ?
#if ENABLE_STACKTRACE
			ret.dump_frame(STACKTRACE_INDENT_PRINT("ret.frame:")) << endl;
#endif
			const size_t lret = lpid;
			STACKTRACE_INDENT_PRINT("ret-id = " << lret << endl);
			ret.gpid = lret;
			// leave owner.offset alone
			// leave owner.frame alone
		}	// is public vs. private
	} else {
		STACKTRACE_INDENT_PRINT("at top-level: " << endl);
		// we're at top level
		owner.offset = *parent_offset;
		owner.frame.construct_top_global_context(*topfp, owner.offset);
#if ENABLE_STACKTRACE
		owner.dump_frame(STACKTRACE_STREAM << "owner.frame:") << endl;
#endif
		const size_t gpid = spr->lookup_locally_allocated_index(uc);
		STACKTRACE_INDENT_PRINT("gpid = " << gpid << endl);
		if (!gpid) {
			return true;	// have error message already
		}
		const footprint& ofp(*owner.frame._footprint);
		const state_instance<Tag>::pool_type&
			pp(ofp.get_instance_pool<Tag>());
		const state_instance<Tag>& sp(pp[gpid -1]);
		const global_offset sgo(owner.offset, ofp, add_all_local_tag());
		global_offset delta;
		ofp.set_global_offset_by_process(delta, gpid);
		STACKTRACE_INDENT_PRINT("offset: " << delta << endl);
		delta += sgo;
#if ENABLE_STACKTRACE
		STACKTRACE_INDENT_PRINT("sgo: " << sgo << endl);
		STACKTRACE_INDENT_PRINT("offset+sgo: " << delta << endl);
		sp._frame.dump_frame(STACKTRACE_STREAM << "sp.frame:") << endl;
#endif
		footprint_frame lff(sp._frame, owner.frame);
		ret.frame.construct_global_context(*sp._frame._footprint, lff, delta);
		// INVARIANT(sub-process is not a top-process-port!);
		ret.offset = delta;	// NEW
#if ENABLE_STACKTRACE
		STACKTRACE_INDENT_PRINT("owner.offset: " << owner.offset << endl);
		STACKTRACE_INDENT_PRINT("ret.offset: " << ret.offset << endl);
		ret.dump_frame(STACKTRACE_STREAM << "top sub-process:") << endl;
#endif
		ret.gpid = gpid;
	}
	return false;
}	// end global_entry_context::construct_global_footprint_frame

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if AGGREGATE_PARENT_REFS
/**
	Recursive implementation due to member instance reference structuring.
	\param pr can be an aggregate process reference (can be hierarchical).
	\return true to signal error.
		local pid of returned process frame
		owner_g is the offset of the current owner frame
		ret_g is the offset of the current return frame
		(both returned by reference, not used when passed)
		Caller should choose which offset is used.
 */
bool
global_entry_context::construct_global_footprint_frames(
		vector<global_process_context>& owners,
		vector<global_process_context_id>& rets,
		const meta_instance_reference_base& pr, 
		const unroll_context& uc) const {
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
	const member_ref* mpr = IS_A(const member_ref*, spr);
	if (mpr) {
#if ENABLE_STACKTRACE
		mpr->dump(STACKTRACE_INDENT_PRINT("have a member-ref: "), 
			expr_dump_context::default_value) << endl;
#endif
		// in a sub-process, sets ret and owner
		vector<global_process_context> p_owners;
		vector<global_process_context_id> p_rets;
		if (construct_global_footprint_frames(p_owners, p_rets,
				*mpr->get_base_ref(), uc)) {
			// have some error
			return true;
		}
#if ENABLE_STACKTRACE
		mpr->dump(STACKTRACE_INDENT_PRINT("back to member-ref: "), 
			expr_dump_context::default_value) << endl;
#endif
		// const size_t ppid = ret.gpid;
		INVARIANT(p_owners.size() == p_rets.size());
		vector<global_process_context>::iterator
			gpci(p_owners.begin()), gpce(p_owners.end());
		vector<global_process_context_id>::iterator
			gpii(p_rets.begin());
		for ( ; gpci!=gpce; ++gpci, ++gpii) {
		global_process_context& p_owner(*gpci);
		global_process_context_id& p_ret(*gpii);
#if ENABLE_STACKTRACE
//		STACKTRACE_INDENT_PRINT("ppid = " << ppid << endl);
		STACKTRACE_INDENT_PRINT("owner.offset: " << p_owner.offset << endl);
		STACKTRACE_INDENT_PRINT("ret.offset: " << p_ret.offset << endl);
		p_ret.dump_frame(STACKTRACE_INDENT_PRINT("ret.frame:")) << endl;
#endif
		// want footprint of the member-owner, not the context passed in
		const unroll_context tc(p_ret.frame._footprint, topfp);
		// yes, use base-class method, not virtual override
		vector<size_t> lpids;
		if (!mpr->simple_ref::lookup_locally_allocated_indices(tc, lpids).good) {
			// have error
			return true;
		}
#if ENABLE_STACKTRACE
		STACKTRACE_INDENT_PRINT("locals: ");
		copy(lpids.begin(), lpids.end(), std::ostream_iterator<size_t>(cerr, ","));
		cerr << endl;
#endif
		const footprint& rfp(*p_ret.frame._footprint);
		const state_instance<Tag>::pool_type&
			pp(rfp.get_instance_pool<Tag>());
		const size_t ports = pp.port_entries();
		const size_t locals = pp.local_entries();

		const size_t ps = p_owners.size() *lpids.size();
		owners.reserve(ps);
		rets.reserve(ps);

		vector<size_t>::const_iterator
			lpii(lpids.begin()), lpie(lpids.end());
		for ( ; lpii!=lpie; ++lpii) {
		const size_t lpid = *lpii;	// is 1-based index
		STACKTRACE_INDENT_PRINT("lpid = " << lpid << " (" << ports
			<< " ports)" << endl);
		INVARIANT(lpid);	// should error out earlier
		INVARIANT(lpid <= locals);
		owners.push_back(p_owner);	// copy first
		rets.push_back(p_ret);		// copy first
		global_process_context& owner(owners.back());
		global_process_context_id& ret(rets.back());
		if (lpid > ports) {	// b/c 1-based index
			// then is local private
			// change both ret frame and owner, and global offset
			STACKTRACE_INDENT_PRINT("private local" << endl);
			const state_instance<Tag>& sp(pp[lpid -1]);
			owner.offset = ret.offset;
			STACKTRACE_INDENT_PRINT("owner.offset <- ret.offset: "
				<< owner.offset << endl);
			global_offset pdelta;
		{
			// compute offset relative to immediate parent
			global_offset z;
			z = global_offset(z, rfp, add_local_private_tag());
			STACKTRACE_INDENT_PRINT("r-delta: " << z << endl);
			rfp.set_global_offset_by_process(pdelta, lpid);
			STACKTRACE_INDENT_PRINT("pdelta: " << pdelta << endl);
			pdelta += z;
			STACKTRACE_INDENT_PRINT("pdelta+z: " << pdelta << endl);
			pdelta += owner.offset;
			STACKTRACE_INDENT_PRINT("pdelta+z+o: " << pdelta << endl);
		}
			owner.frame = ret.frame;
			footprint_frame lff(sp._frame, ret.frame);
			ret.frame.construct_global_context(*sp._frame._footprint, lff, pdelta);
			ret.offset = pdelta;
#if ENABLE_STACKTRACE
			sp._frame.dump_frame(STACKTRACE_INDENT_PRINT("sp.frame:")) << endl;
			owner.dump_frame(
				STACKTRACE_INDENT_PRINT("new owner.frame:")) << endl;
			STACKTRACE_INDENT_PRINT("owner.offset: " << owner.offset << endl);
			ret.dump_frame(STACKTRACE_INDENT_PRINT("ret.frame:")) << endl;
#endif
			ret.gpid = lpid;
		} else {
			STACKTRACE_INDENT_PRINT("public port" << endl);
			// then is public port, only change ret frame
			// keep same owner frame
			// no change in global offset
			const state_instance<Tag>& sp(pp[lpid -1]);
			footprint_frame rff(sp._frame, ret.frame);
#if ENABLE_STACKTRACE
			sp._frame.dump_frame(STACKTRACE_INDENT_PRINT("sp.frame formal:")) << endl;
			owner.dump_frame(STACKTRACE_INDENT_PRINT("owner.frame:")) << endl;
			ret.dump_frame(STACKTRACE_INDENT_PRINT("parent.frame:")) << endl;
			rff.dump_frame(STACKTRACE_INDENT_PRINT("rff:")) << endl;
//			lff.dump_frame(STACKTRACE_INDENT_PRINT("lff:")) << endl;
#endif
			ret.frame.construct_global_context(
				*sp._frame._footprint, rff, ret.offset);
			// ret.offset = ...;	// ?
#if ENABLE_STACKTRACE
			ret.dump_frame(STACKTRACE_INDENT_PRINT("ret.frame:")) << endl;
#endif
			const size_t lret = lpid;
			STACKTRACE_INDENT_PRINT("ret-id = " << lret << endl);
			ret.gpid = lret;
			// leave owner.offset alone
			// leave owner.frame alone
		}	// is public vs. private
		}	// end for
		}	// end for
	} else {
		STACKTRACE_INDENT_PRINT("at top-level: " << endl);
#if ENABLE_STACKTRACE
		spr->dump(STACKTRACE_INDENT_PRINT("top-ref: "), 
			expr_dump_context::default_value) << endl;
#endif
		// we're at top level
		global_process_context common;	// common owner
		common.offset = *parent_offset;
		common.frame.construct_top_global_context(*topfp, common.offset);
#if ENABLE_STACKTRACE
		common.dump_frame(STACKTRACE_STREAM << "owner.frame:") << endl;
#endif
		const footprint& ofp(*common.frame._footprint);
		const state_instance<Tag>::pool_type&
			pp(ofp.get_instance_pool<Tag>());
		const global_offset
			sgo(common.offset, ofp, add_all_local_tag());
		STACKTRACE_INDENT_PRINT("sgo: " << sgo << endl);
		STACKTRACE_INDENT_PRINT("common.offset: " << common.offset << endl);
		vector<size_t> gpids;
		if (!spr->lookup_locally_allocated_indices(uc, gpids).good) {
			return true;	// have error message already
		}
#if ENABLE_STACKTRACE
		STACKTRACE_INDENT_PRINT("locals: ");
		copy(gpids.begin(), gpids.end(), std::ostream_iterator<size_t>(cerr, ","));
		cerr << endl;
#endif
		owners.resize(gpids.size());
		std::fill(owners.begin(), owners.end(), common);
		rets.resize(gpids.size());
		vector<size_t>::const_iterator
			gpii(gpids.begin()), gpie(gpids.end());
		vector<global_process_context_id>::iterator gpci(rets.begin());
		for ( ; gpii!=gpie; ++gpii, ++gpci) {
		const size_t gpid = *gpii;
		global_process_context_id& ret(*gpci);
		STACKTRACE_INDENT_PRINT("gpid = " << gpid << endl);
		INVARIANT(gpid);	// else should have errored out earlier
		const state_instance<Tag>& sp(pp[gpid -1]);
		global_offset delta;
		ofp.set_global_offset_by_process(delta, gpid);
		STACKTRACE_INDENT_PRINT("offset: " << delta << endl);
		delta += sgo;
#if ENABLE_STACKTRACE
		STACKTRACE_INDENT_PRINT("offset+sgo: " << delta << endl);
		sp._frame.dump_frame(STACKTRACE_STREAM << "sp.frame:") << endl;
#endif
		footprint_frame lff(sp._frame, common.frame);
		ret.frame.construct_global_context(*sp._frame._footprint, lff, delta);
		// INVARIANT(sub-process is not a top-process-port!);
		ret.offset = delta;	// NEW
#if ENABLE_STACKTRACE
		STACKTRACE_INDENT_PRINT("ret.offset: " << ret.offset << endl);
		ret.dump_frame(STACKTRACE_STREAM << "top sub-process:") << endl;
#endif
		ret.gpid = gpid;
		}	// end for
	}
	return false;
}
#endif	// AGGREGATE_PARENT_REFS

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
		_pool(f.template get_instance_pool<Tag>());
	// construct context (footprint_frame)
	footprint_frame lff;
	global_offset sgo(*parent_offset, f, add_local_private_tag());
	NEVER_NULL(parent_offset);
// this setup is done twice, also for visit_recursive
// possible to re-factor and setup once?
// something different for the top-level with ports...
	lff.construct_global_context(f, *fpf, *parent_offset);
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
}	// end global_entry_context::visit_local

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This visits only public port instances, skipping the locals.
	Copy-modified from visit_local(), above.
 */
template <class Tag>
void
global_entry_context::visit_ports(const footprint& f) {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	f.dump_type(STACKTRACE_STREAM << "type: ") << endl;
#endif
	const typename state_instance<Tag>::pool_type&
		_pool(f.template get_instance_pool<Tag>());
	// construct context (footprint_frame)
	footprint_frame lff;
	global_offset sgo(*parent_offset, f, add_local_private_tag());
	NEVER_NULL(parent_offset);
// this setup is done twice, also for visit_recursive
// possible to re-factor and setup once?
// something different for the top-level with ports...
	lff.construct_global_context(f, *fpf, *parent_offset);
	// print local, non-port entries, since ports belong to 'parent'
	// for processes duplicate work computing global offsets twice
	const value_saver<global_offset*> __gs__(g_offset, &sgo);
	const footprint_frame_setter __ffs__(fpf, &lff);	// local actuals
	const size_t l = _pool.port_entries();
	// but for the top-level only, we want start with ports
	size_t i = 0;
	for ( ; i<l; ++i) {
		// global and local index can be deduced
		const state_instance<Tag>& inst(_pool[i]);
		inst.accept(*this);
		// visitor will increment g_offset
	}
}	// end global_entry_context::visit_ports

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
	This traversal visits each unique process instance *once*.
	If you wish to visit each unique process alias, then override this.
	TODO: parameter for include_ports.  not needed?
 */
void
global_entry_context::visit_recursive_unique(const footprint& f) {
        STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("in process id " << _gpid << ", type: ");
	f.dump_type(STACKTRACE_STREAM) << endl;
	STACKTRACE_INDENT_PRINT("offset: " << *parent_offset << endl);
	fpf->dump_frame(STACKTRACE_STREAM << "frame:") << endl;
#endif
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
	const size_t pe = lpp.local_entries();
	// but for the top-level only, we want start with ports (process?)
	size_t pi = lpp.port_entries();
#if GLOBAL_CONTEXT_GPID
	size_t ppo = parent_offset->get_offset<process_tag>() +1;
#endif
	const value_saver<const global_offset*> __gs__(parent_offset, &sgo);
	for ( ; pi<pe; ++pi, ++ppo) {
#if GLOBAL_CONTEXT_GPID
		STACKTRACE_INDENT_PRINT("for pi = " << pi <<
			", gpid = " << ppo << endl);
		const value_saver<size_t> __save_gpid(_gpid, ppo);
#endif
		const state_instance<process_tag>& sp(lpp[pi]);
		const footprint_frame& spf(sp._frame);
		const footprint& sfp(*spf._footprint);
		const footprint_frame af(spf, lff);     // context
		const footprint_frame_setter ffs(fpf, &af);
		// really wants to be conditional, but would depend on <Tag>
		sfp.accept(*this);
		sgo += sfp;
	}
	// invariant checks on sgo, consistent with local instance_pools
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
global_entry_context::visit_recursive(const footprint& f) {
	visit_recursive_unique(f);	// default
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively traverses processes *without* actual context, 
	for the purposes of visiting all types post-order.
	Since only types are visited, the frames and offsets
	remain unused in this pass.
 */
void
global_entry_context::visit_types(const footprint& f) {
        STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("in process type: ");
	f.dump_type(STACKTRACE_STREAM) << endl;
#endif
	// recurse through processes and print
	const state_instance<process_tag>::pool_type&
		lpp(f.get_instance_pool<process_tag>());
	// copy and increment with each local process
	size_t pi = 0;
	const size_t pe = lpp.local_entries();
	for ( ; pi<pe; ++pi) {
		STACKTRACE_INDENT_PRINT("lpid = " << pi << endl);
		const state_instance<process_tag>& sp(lpp[pi]);
		const footprint_frame& spf(sp._frame);
		NEVER_NULL(spf._footprint);
		const footprint& sfp(*spf._footprint);
		const footprint_frame_setter ffs(fpf, &spf);
		sfp.accept(*this);
	}
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Call this to report error.
	Caller is responsible for throw-ing or re-throwing exceptions.
 */
void
global_entry_context::report_instantiation_error(ostream& o) const {
	if (at_top()) {
		cerr << "Instantiated from -TOP-: " << endl;
	} else {
		get_current_footprint().dump_type(
			cerr << "Instantiated from type: ") << endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// explicit template instantiations
template void global_entry_context::visit_ports<process_tag>(const footprint&);
template void global_entry_context::visit_ports<channel_tag>(const footprint&);
template void global_entry_context::visit_ports<enum_tag>(const footprint&);
template void global_entry_context::visit_ports<int_tag>(const footprint&);
template void global_entry_context::visit_ports<bool_tag>(const footprint&);

template void global_entry_context::visit_local<process_tag>(const footprint&, const bool);
template void global_entry_context::visit_local<channel_tag>(const footprint&, const bool);
template void global_entry_context::visit_local<enum_tag>(const footprint&, const bool);
template void global_entry_context::visit_local<int_tag>(const footprint&, const bool);
template void global_entry_context::visit_local<bool_tag>(const footprint&, const bool);

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

