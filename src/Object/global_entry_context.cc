/**
	\file "Object/global_entry_context.cc"
	$Id: global_entry_context.cc,v 1.14 2011/05/17 21:19:52 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include "Object/global_entry_context.tcc"
#include "Object/module.h"
#include "Object/global_entry.h"
#include <iostream>
#include "Object/global_channel_entry.h"
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
#include "util/tree_cache.tcc"
#include "util/stacktrace.h"
#include "util/indent.h"
#include "util/value_saver.h"

// explicit template instantiation
namespace util {
using HAC::entity::global_entry_context;
template class tree_cache<size_t, global_entry_context::cache_entry_type>;
}

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
global_entry_context::set_global_context(const cache_entry_type& c) {
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
/**
	Similar traversal to footprint::get_instance<>().
	\param gpid global process index 1-based.  
		pass 0 to indicate top-level.
	NOTE: parent_offset and fpf are only used once in this impl.
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
	STACKTRACE_INDENT_PRINT("gpid(1) = " << gpid << endl);
	STACKTRACE_INDENT_PRINT("offset = " << g << endl);
	while (gpid > local) {
		STACKTRACE_INDENT_PRINT("descending..." << endl);
		const size_t si = gpid -local;	// 1-based index
		// subtract 1 for self index?
		// TODO: refactor out following code
		STACKTRACE_INDENT_PRINT("rem(1) = " << si << endl);
		if (cf == topfp) {
			g = global_offset(g, *cf, add_all_local_tag());
		} else {
			g = global_offset(g, *cf, add_local_private_tag());
		}
		const pool_private_map_entry_type&
			e(p->locate_private_entry(si -1));	// need 0-base!
		const size_t lpid = e.first;
		STACKTRACE_INDENT_PRINT("lpid(1) = " << lpid << endl);
		global_offset delta;
		cf->set_global_offset_by_process(delta, lpid);
		delta += g;
		const state_instance<Tag>& sp((*p)[lpid -1]);
		const footprint_frame& sff(sp._frame);
		cf = sff._footprint;
		gpid = si -e.second;		// still 1-based
		footprint_frame lff(sff, ret);
		ret.construct_global_context(*cf, lff, delta);
#if ENABLE_STACKTRACE
		ret.dump_frame(STACKTRACE_INDENT_PRINT("frame:")) << endl;
#endif
		g = delta;
		p = &cf->get_instance_pool<Tag>();
		ports = p->port_entries();
		local = p->local_private_entries();
		STACKTRACE_INDENT_PRINT("ports = " << ports << endl);
		STACKTRACE_INDENT_PRINT("local = " << local << endl);
		STACKTRACE_INDENT_PRINT("gpid(1) = " << gpid << endl);
		STACKTRACE_INDENT_PRINT("offset = " << g << endl);
	}
	STACKTRACE_INDENT_PRINT("found owner scope" << endl);
	const size_t lpid = gpid +ports;
	STACKTRACE_INDENT_PRINT("lpid(1) = " << lpid << endl);
	STACKTRACE_INDENT_PRINT("offset = " << g << endl);
	g = global_offset(g, *cf, add_local_private_tag());
	STACKTRACE_INDENT_PRINT("offset+fp = " << g << endl);
	p = &cf->get_instance_pool<Tag>();
	const state_instance<Tag>& sp((*p)[lpid -1]);
	const footprint_frame& sff(sp._frame);
	footprint_frame lff(sff, ret);
#if ENABLE_STACKTRACE
	sff.dump_frame(STACKTRACE_INDENT_PRINT("sff:")) << endl;
	ret.dump_frame(STACKTRACE_INDENT_PRINT("actuals:")) << endl;
	lff.dump_frame(STACKTRACE_INDENT_PRINT("pre-frame:")) << endl;
#endif
	global_offset delta;
	cf->set_global_offset_by_process(delta, lpid);
	STACKTRACE_INDENT_PRINT("delta = " << delta << endl);
	delta += g;
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("delta+g = " << delta << endl);
#endif
	cf = sff._footprint;
	ret.construct_global_context(*cf, lff, delta);
#if ENABLE_STACKTRACE
	ret.dump_frame(STACKTRACE_INDENT_PRINT("ret-frame:")) << endl;
#endif
	g = delta;
}
	// else refers to top-level
}	// end global_entry_context::construct_global_footprint_frame

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Follows exact same flow as construct_global_footprint_frame(), above.
	This variation returns a referenced to a cache-managed footprint
	frame, which could either exist from before, or be generated 
	on the fly.
	Performance-critical back-ends should use this.
 */
const global_entry_context::cache_entry_type&
global_entry_context::lookup_global_footprint_frame_cache(size_t gpid, 
		index_frame_cache_type* cache) const {
	STACKTRACE_VERBOSE;
	typedef	process_tag				Tag;
	typedef	state_instance<Tag>::pool_type		pool_type;
	NEVER_NULL(cache);
	// the top footprint frame is always cached, and pre-constructed
	cache_entry_type* ret = &cache->value;
	// entry type contains both frame and offset (pair)
if (gpid) {
	// iterative instead of recursive implementation, hence pointers
	const footprint* cf = ret->frame._footprint;	// topfp->footprint
	const pool_type* p = &cf->get_instance_pool<Tag>();
	size_t ports = 0;			// at_top
	size_t local = p->local_entries();	// at_top
	while (gpid > local) {
		const size_t si = gpid -local;	// 1-based index
		const pool_private_map_entry_type&
			e(p->locate_private_entry(si -1));	// need 0-base!
		const size_t lpid = e.first;
		gpid = si -e.second;		// still 1-based
		const std::pair<index_frame_cache_type::child_iterator, bool>
			cp(cache->insert_find(lpid));
	if (cp.second) {
		// was a cache miss: re-compute
		global_offset g(ret->offset);
		if (cf == topfp) {
			g = global_offset(g, *cf, add_all_local_tag());
		} else {
			g = global_offset(g, *cf, add_local_private_tag());
		}
		global_offset delta;
		cf->set_global_offset_by_process(delta, lpid);
		delta += g;
		const state_instance<Tag>& sp((*p)[lpid -1]);
		const footprint_frame& sff(sp._frame);
		cf = sff._footprint;
		const footprint_frame lff(sff, ret->frame);
		cache = &const_cast<index_frame_cache_type&>(*cp.first); // descend
		ret = &cache->value;
		ret->frame.construct_global_context(*cf, lff, delta);
		ret->offset = delta;		// g = delta;
		INVARIANT(cf == ret->frame._footprint);
	} else {
		// else was a cache hit
		cache = &const_cast<index_frame_cache_type&>(*cp.first); // descend
		ret = &cache->value;
		cf = ret->frame._footprint;
	}
		p = &cf->get_instance_pool<Tag>();
		ports = p->port_entries();
		local = p->local_private_entries();
	}	// end while
	const size_t lpid = gpid +ports;
	const std::pair<index_frame_cache_type::child_iterator, bool>
		cp(cache->insert_find(lpid));
if (cp.second) {	// cache miss
	global_offset g(ret->offset, *cf, add_local_private_tag());
	p = &cf->get_instance_pool<Tag>();
	const state_instance<Tag>& sp((*p)[lpid -1]);
	const footprint_frame& sff(sp._frame);
	footprint_frame lff(sff, ret->frame);
	global_offset delta;
	cf->set_global_offset_by_process(delta, lpid);
	delta += g;
	cf = sff._footprint;
	cache = &const_cast<index_frame_cache_type&>(*cp.first); // descend
	ret = &cache->value;
	ret->frame.construct_global_context(*cf, lff, delta);
	ret->offset = delta;
} else {	// cache hit
	cache = &const_cast<index_frame_cache_type&>(*cp.first); // descend
	ret = &cache->value;
}
	return *ret;
} else {
	return *ret;
}
	// else refers to top-level
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapped call.
 */
size_t
global_entry_context::construct_global_footprint_frame(
		const footprint& top,
		const meta_instance_reference_base& pr, 
		global_process_context& ret) {
	const unroll_context lookup_c(&top, &top);      // any loop variables?
	return construct_global_footprint_frame(top, pr, lookup_c, ret);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapped call.
	unroll_context is pased in manually, in case of override.
 */
size_t
global_entry_context::construct_global_footprint_frame(
		const footprint& top,
		const meta_instance_reference_base& pr, 
		const unroll_context& c,
		global_process_context& ret) {
	const global_process_context gpc(top);
	const global_entry_context gc(gpc);
	return gc.construct_global_footprint_frame(pr, c, ret);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Publicly convenience-wrapped call.
 */
size_t
global_entry_context::construct_global_footprint_frame(
		const meta_instance_reference_base& pr, 
		const unroll_context& uc,
//		footprint_frame& ret
		global_process_context& ret) const {
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
		vector<size_t>& gpids,
		vector<footprint_frame>& retf) {
	const unroll_context lookup_c(&top, &top);      // any loop variables?
	return construct_global_footprint_frames(
		top, pr, lookup_c, gpids, retf);
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
		vector<size_t>& gpids,
		vector<footprint_frame>& retf) {
	const footprint_frame tff(top);
	const global_offset g;
	const global_entry_context gc(tff, g);
	return gc.construct_global_footprint_frames(pr, c, gpids, retf);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Publicly convenience-wrapped call.
 */
bool
global_entry_context::construct_global_footprint_frames(
		const meta_instance_reference_base& pr, 
		const unroll_context& uc,
		vector<size_t>& gpids,
		vector<footprint_frame>& retf) const {
        global_offset go, tmpg;
        footprint_frame tmpo;
	return construct_global_footprint_frames(
		tmpo, retf, go, tmpg, pr, uc, gpids);
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
 */
size_t
global_entry_context::construct_global_footprint_frame(
		global_process_context& owner,
		global_process_context& ret,
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
		return 0;
	}
	// check for scalar reference
	if (spr->dimensions()) {
		cerr << "Parent reference must be scalar." << endl;
		return 0;
	}
	const member_ref* mpr = IS_A(const member_ref*, spr);
	if (mpr) {
#if ENABLE_STACKTRACE
		mpr->dump(STACKTRACE_INDENT_PRINT("have a member-ref: "), 
			expr_dump_context::default_value) << endl;
#endif
		// in a sub-process, sets ret and owner
		const size_t ppid =
			construct_global_footprint_frame(owner, ret,
				*mpr->get_base_ref(), uc);
		if (!ppid) {
			// have some error
			return 0;
		}
#if ENABLE_STACKTRACE
		mpr->dump(STACKTRACE_INDENT_PRINT("back to member-ref: "), 
			expr_dump_context::default_value) << endl;
		STACKTRACE_INDENT_PRINT("ppid = " << ppid << endl);
		STACKTRACE_INDENT_PRINT("owner.offset: " << owner.offset << endl);
		STACKTRACE_INDENT_PRINT("ret.offset: " << ret.offset << endl);
		ret.frame.dump_frame(STACKTRACE_INDENT_PRINT("ret.frame:")) << endl;
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
			return 0;
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
			owner.frame.dump_frame(
				STACKTRACE_INDENT_PRINT("new owner.frame:")) << endl;
			STACKTRACE_INDENT_PRINT("owner.offset: " << owner.offset << endl);
			ret.frame.dump_frame(STACKTRACE_INDENT_PRINT("ret.frame:")) << endl;
#endif
			return lpid;
		} else {
			STACKTRACE_INDENT_PRINT("public port" << endl);
			// then is public port, only change ret frame
			// keep same owner frame
			// no change in global offset
			const state_instance<Tag>& sp(pp[lpid -1]);
			footprint_frame rff(sp._frame, ret.frame);
#if ENABLE_STACKTRACE
			sp._frame.dump_frame(STACKTRACE_INDENT_PRINT("sp.frame formal:")) << endl;
			owner.frame.dump_frame(STACKTRACE_INDENT_PRINT("owner.frame:")) << endl;
			ret.frame.dump_frame(STACKTRACE_INDENT_PRINT("parent.frame:")) << endl;
			rff.dump_frame(STACKTRACE_INDENT_PRINT("rff:")) << endl;
//			lff.dump_frame(STACKTRACE_INDENT_PRINT("lff:")) << endl;
#endif
			ret.frame.construct_global_context(
				*sp._frame._footprint, rff, ret.offset);
			// ret.offset = ...;	// ?
#if ENABLE_STACKTRACE
			ret.frame.dump_frame(STACKTRACE_INDENT_PRINT("ret.frame:")) << endl;
#endif
			const size_t lret = lpid;
			STACKTRACE_INDENT_PRINT("ret-id = " << lret << endl);
			return lret;
			// leave owner.offset alone
			// leave owner.frame alone
		}	// is public vs. private
	} else {
		STACKTRACE_INDENT_PRINT("at top-level: ");
		// we're at top level
		owner.offset = *parent_offset;
		owner.frame.construct_top_global_context(*topfp, owner.offset);
#if ENABLE_STACKTRACE
		owner.frame.dump_frame(STACKTRACE_STREAM << "owner.frame:") << endl;
#endif
		const size_t gpid = spr->lookup_locally_allocated_index(uc);
		STACKTRACE_INDENT_PRINT("gpid = " << gpid << endl);
		if (!gpid) {
			return 0;	// have error message already
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
		ret.frame.dump_frame(STACKTRACE_STREAM << "top sub-process:") << endl;
#endif
		return gpid;
	}
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
		footprint_frame& owner, 
		vector<footprint_frame>& retf,
		global_offset& owner_g,
		vector<global_offset>& ret_gs,
		const meta_instance_reference_base& pr, 
		const unroll_context& uc, 
		vector<size_t>& ind) const {
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
	// parent reference can now be array
	const member_ref* mpr = IS_A(const member_ref*, spr);
if (mpr) {
	// in a sub-process, sets ret, owner and owner_g
	vector<size_t> ppids;
	vector<footprint_frame> pfs;
	vector<global_offset> pgs;
	// recursive to parents
	if (construct_global_footprint_frames(owner, pfs, owner_g, pgs,
			*mpr->get_base_ref(), uc, ppids)) {
		// have some error
		return true;
	}
	INVARIANT(ppids.size() == pfs.size());
	INVARIANT(ppids.size() == pgs.size());
	vector<size_t>::const_iterator ppi(ppids.begin());
	vector<footprint_frame>::const_iterator
		pfi(pfs.begin()), pfe(pfs.end());
	vector<global_offset>::const_iterator rgi(pgs.begin());
	// each member of array may have different type (relaxed template)
	// TODO: optimize when reference is to strict type
	// or detect when all members are the same type, 
	// and hoist ::lookup_locally_allocated_indices out of the loop
	// and take the cross-product.
	// OR cache sets of local indices belonging to the same type
for ( ; pfi!=pfe; ++pfi, ++ppi, ++pgi) {
	// want footprint of the member-owner, not the context passed in
	const footprint_frame& pf(*pfi);
	const global_offset& pg(*pgi);
	const footprint& rfp(*pf._footprint);
	const state_instance<Tag>::pool_type& pp(rfp.get_instance_pool<Tag>());
	const size_t ports = pp.port_entries();
	const size_t locals = pp.local_entries();
	const unroll_context tc(rfp, topfp);
	vector<size_t> lpids;
	// yes, use base-class method, not virtual override
	if (mpr->simple_ref::lookup_locally_allocated_indices(tc, lpids)) {
		return true;
	}
	const size_t PN = ppids.size() *lpids.size();
	ind.reserve(PN);
	ret_gs.reserve(PN);
	retf.reserve(PN);
	vector<size_t>::const_iterator lpi(lpids.begin()), lpe(lpids.end());
	for ( ; lpi!=lpi; ++lpi) {
	const size_t& lpid(*lpi);	// is 1-based index
	if (!lpid) {
		return true;
	}
	INVARIANT(lpid <= locals);
		retf.push_back(footprint_frame());
	if (lpid > ports) {	// b/c 1-based index
		// then is local private
		// change both ret frame and owner, and global offset
		const state_instance<Tag>& sp(pp[lpid -1]);
		owner_g = pg;
		global_offset pdelta;
	{
		// compute offset relative to immediate parent
		global_offset z;
		z = global_offset(z, rfp, add_local_private_tag());
		rfp.set_global_offset_by_process(pdelta, lpid);
		pdelta += z;
		pdelta += owner_g;
	}
		owner = pf;
		footprint_frame lff(sp._frame, pf);
		retf.back().construct_global_context(
			*sp._frame._footprint, lff, pdelta);
		ret_gs.push_back(pdelta);
		ind.push_back(lpid);
	} else {
		// then is public port, only change ret frame
		// keep same owner frame
		// no change in global offset
		const state_instance<Tag>& sp(pp[lpid -1]);
		footprint_frame rff(sp._frame, pf);
		retf.back().construct_global_context(
			*sp._frame._footprint, rff, ret_g);
		// ret_g = ...;	// ?
		const size_t lret = lpid;
		ind.push_back(lret);
		// leave owner_g offset alone
		// leave owner frame alone
		ret_gs.push_back(owner_g);
	}	// is public vs. private
	}	// end for
}	// end for
} else {
	// we're at top level
	owner_g = *parent_offset;
	owner.construct_top_global_context(*topfp, owner_g);
	if (spr->lookup_locally_allocated_indices(uc, ind)) {
		return true;	// have error message already
	}
	const footprint& ofp(*owner._footprint);
	const state_instance<Tag>::pool_type&
		pp(ofp.get_instance_pool<Tag>());
	const global_offset sgo(owner_g, ofp, add_all_local_tag());
	retf.resize(ind.size());
	ret_gs.resize(ind.size());
	vector<size_t>::const_iterator gpi(ind.begin()), gpe(ind.end());
	vector<footprint_frame>::iterator rfi(retf.begin());
	vector<global_offset>::iterator rgi(ret_gs.begin());
	for ( ; gpi!=gpe; ++gpi, ++rfi, ++rgi) {
		const size_t& gpid(*gpi);
		footprint_frame& ret(*rfi);
		global_offset& ret_g(*rgi);
		const state_instance<Tag>& sp(pp[gpid -1]);
		global_offset delta;
		ofp.set_global_offset_by_process(delta, gpid);
		delta += sgo;
		footprint_frame lff(sp._frame, owner);
		ret.construct_global_context(*sp._frame._footprint, lff, delta);
		ret_g = delta;
	//	return gpid;
	}	// end for
}
	return false;
}	// end global_entry_context::construct_global_footprint_frames
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

