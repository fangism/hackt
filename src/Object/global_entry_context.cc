/**
	\file "Object/global_entry_context.cc"
	$Id: global_entry_context.cc,v 1.14 2011/05/17 21:19:52 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include "Object/global_entry_context.tcc"
#include "Object/module.hh"
#include "Object/global_entry.hh"
#include "Object/global_channel_entry.hh"
#include "Object/global_context_cache.hh"
#include "Object/def/footprint.hh"
#include "Object/def/datatype_definition_base.hh"
#include "Object/expr/expr_dump_context.hh"
#include "Object/inst/state_instance.hh"
#include "Object/inst/instance_pool.hh"
#include "Object/traits/instance_traits.hh"
#include "Object/inst/instance_alias_info.hh"
#include "Object/inst/alias_empty.hh"
#include "Object/inst/alias_actuals.hh"
#include "Object/inst/instance_collection.hh"
#include "Object/inst/general_collection_type_manager.hh"
#include "Object/inst/instance_placeholder.hh"
#include "Object/inst/physical_instance_placeholder.hh"
#include "Object/ref/member_meta_instance_reference.hh"
#include "util/stacktrace.hh"
#include "util/indent.hh"
#include "util/value_saver.hh"

namespace HAC {
namespace entity {
#include "util/using_ostream.hh"
using util::value_saver;
typedef	value_saver<const footprint_frame*>	footprint_frame_setter;

//=============================================================================
// struct global_entry_context_base method definitions

//=============================================================================
// struct global_entry_context method definitions

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
#if FOOTPRINT_OWNS_CONTEXT_CACHE
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
		context_result_type& ret) {
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
		context_result_type& ret) {
	const global_process_context gpc(top);
	const global_entry_context gc(gpc);
	return gc.construct_global_footprint_frame(pr, c, ret);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	1 is OK.  0 is OK too.
	Want to be able to pass down top-level cache pointer.
 */
#define	__CALLER_INITIALIZE_OWNER_SCALAR			1
/**
	1 was broken, is now OK.
	Failed because of temporaries allocated in recursive call.
	Fixed by passing down top owner and swapping arrays.
 */
#define	__CALLER_INITIALIZE_OWNER_AGGREGATE			1
/**
	Publicly convenience-wrapped call.
 */
bool
global_entry_context::construct_global_footprint_frame(
		const meta_instance_reference_base& pr, 
		const unroll_context& uc,
		context_result_type& ret) const {
#if __CALLER_INITIALIZE_OWNER_SCALAR
#if CACHE_REFERENCE_LOOKUP_CONTEXTS
	context_arg_type owner(*topfp);
#else
	global_process_context owner(*topfp);
	owner.offset = *parent_offset;
	owner.construct_top_global_context();
#endif
#else
	global_process_context owner;
#endif
	return construct_global_footprint_frame(owner, ret, pr, uc);
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
		vector<context_result_type>& retf) {
	STACKTRACE("construct_global_footprint_frames(top, pr, retf)");
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
		vector<context_result_type>& retf) {
	STACKTRACE("construct_global_footprint_frames(top, pr, uc, retf)");
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
		vector<context_result_type>& retf) const {
	STACKTRACE("construct_global_footprint_frames(pr, uc, retf)");
	vector<context_arg_type> tmpo;
#if __CALLER_INITIALIZE_OWNER_AGGREGATE
#if CACHE_REFERENCE_LOOKUP_CONTEXTS
	context_arg_type owner(*topfp);
	tmpo.push_back(owner);
#else
	global_process_context owner(*topfp);
	tmpo.push_back(owner);
	global_process_context& first(tmpo.front());
	first.offset = *parent_offset;
	first.construct_top_global_context();
#endif
	INVARIANT(tmpo.size() == 1);
#endif
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
		context_arg_type& owner,
		context_result_type& ret,
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
#if ENABLE_STACKTRACE
		mpr->dump(STACKTRACE_INDENT_PRINT("back to member-ref: "), 
			expr_dump_context::default_value) << endl;
		// const size_t ppid = ret.gpid;
//		STACKTRACE_INDENT_PRINT("ppid = " << ppid << endl);
		STACKTRACE_INDENT_PRINT("owner.offset: " << owner.get_offset() << endl);
		STACKTRACE_INDENT_PRINT("ret.offset: " << ret.get_offset() << endl);
		ret.get_frame().dump_frame(
			STACKTRACE_INDENT_PRINT("ret.frame:")) << endl;
#endif
		// want footprint of the member-owner, not the context passed in
		const footprint_frame& rfpf(ret.get_frame());
		const footprint& rfp(*rfpf._footprint);
		const unroll_context tc(&rfp, topfp);
		// yes, use base-class method, not virtual override
		const size_t lpid =	// is 1-based index
			mpr->simple_ref::lookup_locally_allocated_index(tc);
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
			owner = ret;	// frame and offset
#if CACHE_REFERENCE_LOOKUP_CONTEXTS
			ret.descend(lpid, *topfp);
#else
			ret.descend_frame(lpid, false);
#endif
		} else {
			STACKTRACE_INDENT_PRINT("public port" << endl);
			// then is public port, only change ret frame
			// keep same owner frame
			// no change in global offset
#if CACHE_REFERENCE_LOOKUP_CONTEXTS
			ret.descend(lpid, *topfp);	// FIXME?
#else
			ret.descend_port(lpid);
#endif
		}	// is public vs. private
#if ENABLE_STACKTRACE
		ret.get_frame().dump_frame(
			STACKTRACE_INDENT_PRINT("ret.frame:")) << endl;
		STACKTRACE_INDENT_PRINT("ret.offset: " << ret.get_offset() << endl);
#endif
	} else {
		STACKTRACE_INDENT_PRINT("at top-level: " << endl);
		// we're at top level
#if __CALLER_INITIALIZE_OWNER_SCALAR
		// owner should already point to top context
#else
		owner.offset = *parent_offset;
		owner.frame.construct_top_global_context(*topfp, owner.offset);
#endif
#if ENABLE_STACKTRACE
		owner.get_frame().dump_frame(
			STACKTRACE_STREAM << "owner.frame:") << endl;
		STACKTRACE_INDENT_PRINT("owner.offset: " << owner.get_offset() << endl);
#endif
		const size_t gpid = spr->lookup_locally_allocated_index(uc);
		STACKTRACE_INDENT_PRINT("gpid = " << gpid << endl);
		if (!gpid) {
			return true;	// have error message already
		}
#if CACHE_REFERENCE_LOOKUP_CONTEXTS
		ret.descend(owner.subcache, gpid, *topfp);
#else
		ret.descend_frame(owner, gpid, true);
#endif
#if ENABLE_STACKTRACE
		ret.get_frame().dump_frame(
			STACKTRACE_INDENT_PRINT("ret.frame:")) << endl;
		STACKTRACE_INDENT_PRINT("ret.offset: " << ret.get_offset() << endl);
#endif
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
	Also directly called from member_instance_reference::lookup_locally_allocated_index.
 */
bool
global_entry_context::construct_global_footprint_frames(
		vector<context_arg_type>& owners,
		vector<context_result_type>& rets,
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
		vector<context_arg_type> p_owners;
		vector<context_result_type> p_rets;
		// pass-down top-level owner
		if (construct_global_footprint_frames(owners, p_rets,
				*mpr->get_base_ref(), uc)) {
			// have some error
			return true;
		}
		owners.swap(p_owners);
		owners.clear();
#if ENABLE_STACKTRACE
		mpr->dump(STACKTRACE_INDENT_PRINT("back to member-ref: "), 
			expr_dump_context::default_value) << endl;
#endif
		// const size_t ppid = ret.gpid;
		INVARIANT(p_owners.size() == p_rets.size());
		vector<context_arg_type>::iterator
			gpci(p_owners.begin()), gpce(p_owners.end());
		vector<context_result_type>::iterator
			gpii(p_rets.begin());
		for ( ; gpci!=gpce; ++gpci, ++gpii) {
		context_arg_type& p_owner(*gpci);
		context_result_type& p_ret(*gpii);
#if ENABLE_STACKTRACE
//		STACKTRACE_INDENT_PRINT("ppid = " << ppid << endl);
		STACKTRACE_INDENT_PRINT("owner.offset: " << p_owner.offset << endl);
		STACKTRACE_INDENT_PRINT("ret.offset: " << p_ret.offset << endl);
		p_ret.dump_frame(STACKTRACE_INDENT_PRINT("ret.frame:")) << endl;
#endif
		// want footprint of the member-owner, not the context passed in
		const unroll_context tc(p_ret.get_frame()._footprint, topfp);
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
		const footprint& rfp(*p_ret.get_frame()._footprint);
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
		context_arg_type& owner(owners.back());
		context_result_type& ret(rets.back());
		if (lpid > ports) {	// b/c 1-based index
			// then is local private
			// change both ret frame and owner, and global offset
			STACKTRACE_INDENT_PRINT("private local" << endl);
			owner = ret;	// frame and offset
#if CACHE_REFERENCE_LOOKUP_CONTEXTS
			ret.descend(lpid, *topfp);
#else
			ret.descend_frame(lpid, false);
#endif
		} else {
			STACKTRACE_INDENT_PRINT("public port" << endl);
#if CACHE_REFERENCE_LOOKUP_CONTEXTS
			ret.descend(lpid, *topfp);
#else
			ret.descend_port(lpid);
#endif
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
#if __CALLER_INITIALIZE_OWNER_AGGREGATE
		INVARIANT(owners.size() == 1);
		const context_arg_type& common(owners.front());
#else
		context_arg_type common(*topfp);	// common owner
		common.offset = *parent_offset;
		common.construct_top_global_context();
#endif
#if ENABLE_STACKTRACE
		common.dump_frame(STACKTRACE_STREAM << "common.frame:") << endl;
		STACKTRACE_INDENT_PRINT("common.offset: " << common.offset << endl);
#endif
		vector<size_t> gpids;
		if (!spr->lookup_locally_allocated_indices(uc, gpids).good) {
			return true;	// have error message already
		}
#if ENABLE_STACKTRACE
		STACKTRACE_INDENT_PRINT("locals: ");
		copy(gpids.begin(), gpids.end(), std::ostream_iterator<size_t>(cerr, ","));
		cerr << endl;
#endif
#if __CALLER_INITIALIZE_OWNER_AGGREGATE
		// reference safety: allocate aside and swap
		vector<context_arg_type> owner_array;
		owner_array.resize(gpids.size());
		std::fill(owner_array.begin(), owner_array.end(), common);
		owners.swap(owner_array);
		const context_arg_type& com(owner_array.front());
#else
		owners.resize(gpids.size());	// causes realloc!
		std::fill(owners.begin(), owners.end(), common);
		// copy into array
		const context_arg_type& com(common);
#endif
		rets.resize(gpids.size());
		vector<size_t>::const_iterator
			gpii(gpids.begin()), gpie(gpids.end());
		vector<context_result_type>::iterator gpci(rets.begin());
		for ( ; gpii!=gpie; ++gpii, ++gpci) {
		const size_t gpid = *gpii;
		context_result_type& ret(*gpci);
#if CACHE_REFERENCE_LOOKUP_CONTEXTS
		ret.descend(com.subcache, gpid, *topfp);
#else
		ret.descend_frame(com, gpid, true);
#endif
		}	// end for
	}
	return false;
}	// end construct_global_footprint_frames
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

