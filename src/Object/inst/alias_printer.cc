/**
	\file "Object/inst/alias_printer.cc"
	$Id: alias_printer.cc,v 1.8.24.4 2010/02/23 22:34:03 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include "Object/inst/alias_printer.h"
#include "Object/devel_switches.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_actuals.h"
#include "Object/inst/alias_empty.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/traits/enum_traits.h"
#if ENABLE_DATASTRUCTS
#include "Object/traits/struct_traits.h"
#endif
#if BUILTIN_CHANNEL_FOOTPRINTS
#include "Object/global_channel_entry.h"
#endif
#include "Object/traits/chan_traits.h"
#include "Object/traits/proc_traits.h"
#include "Object/def/footprint.h"
#include "Object/global_entry.tcc"
#include "Object/state_manager.h"
#include "Object/common/dump_flags.h"
#include "Object/common/alias_string_cache.h"
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
#include "Object/inst/physical_instance_placeholder.h"
#endif
#include "common/ICE.h"
#include "common/TODO.h"
#include "main/cflat.h"
#include "main/cflat_options.h"
#include "util/sstream.h"
#include "util/macros.h"
#include "util/stacktrace.h"


namespace HAC {
namespace entity {
using std::ostringstream;
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
using util::value_saver;
#endif

//=============================================================================
static
void
cflat_print_alias(ostream&, const string&, const string&,
		const cflat_options&);

//=============================================================================
/**
	Defined by specialization only.  
	\param HasSubstructure whether or not the meta-class type
		has substructure.  
 */
template <bool HasSubstructure>
struct alias_printer_recursion_policy;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct alias_printer_recursion_policy<false> {
	template <class Tag>
	static
	void
	accept(alias_printer& c, const GLOBAL_ENTRY<Tag>& e
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
//			, const footprint&
#else
			, const size_t gi
#endif
			) {
		typedef class_traits<Tag>		traits_type;
		STACKTRACE_VERBOSE;
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	const size_t gi = c.lookup_global_id<Tag>(
		e.get_back_ref()->instance_index);
#endif
	if (traits_type::print_cflat_leaf) {
		ostringstream os;
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
		c.get_top_footprint().template dump_canonical_name<Tag>(os, gi -1);
#else
		e.dump_canonical_name(os, c.topfp, c.sm);
#endif
		const string& canonical(os.str());
		STACKTRACE_INDENT_PRINT("canonical = " << canonical << endl);
	if (!c.cf.check_prs) {
		if (!c.cf.wire_mode) {
			cflat_print_alias(c.o, canonical, c.prefix, c.cf);
		} else if (canonical != c.prefix) {
			// this should only be done for bool_tag!
			INVARIANT(gi < c.wires.size());
			c.wires[gi].strings.push_back(c.prefix);
		}
	}
	}
	}
};	// end struct alias_printer_recursion_policy

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct alias_printer_recursion_policy<true> {
	template <class Tag>
	static
	void
	accept(alias_printer& c, const GLOBAL_ENTRY<Tag>& e
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
//		, const footprint& f
#else
		, const size_t
#endif
		) {
	STACKTRACE_VERBOSE;
	// saves away current footprint frame on stack, and restores on return
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	const alias_printer::save_prefix save(c);
	c.prefix += '.';
	const footprint& f(*c.fpf->_footprint);
#if 0
	const typename state_instance<Tag>::pool_type&
		lp(f.get_instance_pool<Tag>());
#endif
	const instance_alias_info<Tag>& local_alias = *e.get_back_ref();
	const size_t lpid = local_alias.instance_index;
	STACKTRACE_INDENT_PRINT("lpid = " << lpid << endl);
	// b/c visitor only visits immediate locals, not their substructures
// Q: is this process a public port or private local?
	const bool is_private = !local_alias.get_container_base()
		->get_placeholder_base()->is_port_formal();
//		(lpid > f.get_instance_pool<Tag>().port_entries());
	STACKTRACE_INDENT_PRINT("is " << (is_private ? "private" : "public") << endl);
#if 0
// visit substructure's immediate public aliases
	const substructure_alias& sub(local_alias);
	sub.accept(c);
#endif
if (is_private) {
//	const footprint& f(*c.fpf->_footprint);
	INVARIANT(!local_alias.get_container_base()->get_super_instance());
#if 0
	const size_t nports = lp.port_entries();
	INVARIANT(lpid > nports);	// not true, could be aliased to port
#endif
#if ENABLE_STACKTRACE
	const size_t gpid = c.lookup_global_id<Tag>(lpid);
	STACKTRACE_INDENT_PRINT("gpid = " << gpid << endl);
#endif
	// see global_entry_dumper::visit(const state_instance<process_tag>&)
	// the construction of the extended frame
	// TODO: refactor this code!
	STACKTRACE_INDENT_PRINT("parent offset = " << *c.parent_offset << endl);
	global_offset sgo(*c.parent_offset);	// copy
	global_offset delta, local;
	if (c.at_top()) {
		local = global_offset(local, f, add_all_local_tag());
	} else {
		local = global_offset(local, f, add_local_private_tag());
	}
	STACKTRACE_INDENT_PRINT("local         = " << local << endl);
	sgo += local;
#if ENABLE_STACKTRACE
	f.dump(STACKTRACE_STREAM);		// print enumeration map
#endif
	f.set_global_offset_by_process(delta, lpid);	// conditional?
	STACKTRACE_INDENT_PRINT("delta         = " << delta << endl);
	sgo += delta;
	const footprint_frame& spf(e._frame);
	const footprint& sfp(*spf._footprint);
	const footprint_frame af(spf, *c.fpf);
	const value_saver<const footprint_frame*> _f_(c.fpf, &af);
	const value_saver<const global_offset*> _g_(c.parent_offset, &sgo);
	STACKTRACE_INDENT_PRINT("child  offset = " << sgo << endl);
#if ENABLE_STACKTRACE
	af.dump_frame(STACKTRACE_STREAM << "actual context =") << endl;
#endif
	sfp.accept(AS_A(global_entry_context&, c));
	// or just c.visit(*spf._footprint);
} else {
	// is public (process) port
	// transformed frame
	// but offset is already out of scope/range...
	FINISH_ME(Fang);
}
#else
	const alias_printer::save_frame save(c, &e._frame);
	NEVER_NULL(c.fpf);
	NEVER_NULL(c.fpf->_footprint);
	c.fpf->_footprint->accept(c);
#endif
	}

};	// end struct alias_printer_recursion_policy

//=============================================================================
// class alias_printer method definitions

alias_printer::alias_printer(ostream& _o,
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
		const footprint_frame& _fpf,
		const global_offset& g,
#else
		const state_manager& _sm,
		const footprint& _f,
		const footprint_frame* const _fpf,
#endif
		const cflat_options& _cf,
		wire_alias_set& _w,
		const string& _p
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
		, const this_type* __p
#endif
		) :
		cflat_aliases_arg_type(_o,
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
			_fpf, g,
#else
			_sm, _f, _fpf,
#endif
			_cf, _w, _p)
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
			, parent(__p)
#endif
			{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
/**
	Traversal: by unique or by alias?  By alias would be best.
	Key: visit every alias exactly once.
	Should follow global_entry_context::visit_recursive.
 */
void
alias_printer::visit(const footprint& f) {
	STACKTRACE_VERBOSE;
#if 0
#if ENABLE_STACKTRACE
	f.dump_type(STACKTRACE_INDENT_PRINT("type: ")) << endl;
	dump_context(STACKTRACE_INDENT_PRINT("current:\n"));
	owner_context().dump_context(STACKTRACE_INDENT_PRINT("owner:\n"));
#endif
	footprint_frame lff(f);	// lff(f);
//	global_offset sgo(*parent_offset, f, add_local_private_tag());
if (at_top()) {
	lff.construct_top_global_context(f, *parent_offset);
#if 0
	global_offset sgo(*parent_offset);
	const global_offset b(sgo, f, add_local_private_tag());
	lff.extend_frame(sgo, b);
//	sgo = global_offset(*parent_offset, f, add_all_local_tag());
#endif
	// can't alter global_offset here because members may be
	// public or private
#if ENABLE_STACKTRACE
	lff.dump_frame(STACKTRACE_INDENT_PRINT("TOP CONTEXT =")) << endl;
//	STACKTRACE_INDENT_PRINT("sgo =           " << sgo << endl);
#endif
	const value_saver<const footprint_frame*> _f_(fpf, &lff);
	f.accept(AS_A(alias_visitor&, *this));
} else {
//	lff.construct_global_context(f, *cfpf, *parent_offset);
	f.accept(AS_A(alias_visitor&, *this));
}
#else
	visit_local<bool_tag>(f, true);
	visit_recursive(f);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 1
// kept for reference for now
/**
	General algorithm stolen from global_entry_context::visit_recursive().
	Push this into global_entry_context as visit_recursive_aliases.
 */
void
alias_printer::visit_recursive(const footprint& f) {
	STACKTRACE_VERBOSE;
	typedef	process_tag		Tag;	// meta-type to recurse on
	typedef	value_saver<const footprint_frame*>	footprint_frame_setter;
#if ENABLE_STACKTRACE
	f.dump_type(STACKTRACE_INDENT_PRINT("type: ")) << endl;
	dump_context(STACKTRACE_INDENT_PRINT("current:\n"));
#endif
	const port_alias_tracker& pt(f.get_scope_alias_tracker());
	const state_instance<Tag>::pool_type&
		lpp(f.get_instance_pool<Tag>());
	const bool is_top = at_top();
	NEVER_NULL(parent_offset);
	global_offset sgo(*parent_offset, f, add_local_private_tag());
	footprint_frame lff;
if (is_top) {
	lff.construct_top_global_context(f, *parent_offset);
	sgo = global_offset(*parent_offset, f, add_all_local_tag());
} else {
	lff.construct_global_context(f, *fpf, *parent_offset);
}
#if ENABLE_STACKTRACE
	lff.dump_frame(STACKTRACE_STREAM << "local context:") << endl;
#endif
	// copy and increment with each local process
	const size_t pe = lpp.local_entries();
	// but for the top-level only, we want start with ports (process?)
#if 1
	size_t pi = is_top ? 0 : lpp.port_entries();
#else
	size_t pi = 0;
#endif
	const value_saver<const global_offset*> __gs__(parent_offset, &sgo);
	for ( ; pi<pe; ++pi) {
		const state_instance<Tag>& sp(lpp[pi]);
		const footprint_frame& spf(sp._frame);
		const footprint& sfp(*spf._footprint);
		const footprint_frame af(spf, lff);     // context
		const footprint_frame_setter ffs(fpf, &af);
		// repeat for each alias as prefix!
		const size_t lpid = pi +1;
		const alias_reference_set<Tag>&
			ars(pt.get_id_map<Tag>().find(lpid)->second);
		// 1-based
#if ENABLE_STACKTRACE
		const size_t gpid = global_entry_context::
			lookup_global_id<Tag>(lpid);
		STACKTRACE_INDENT_PRINT("lpid = " << lpid << endl);
		STACKTRACE_INDENT_PRINT("gpid = " << gpid << endl);
#endif
		alias_reference_set<Tag>::const_iterator
			i(ars.begin()), e(ars.end());
		for ( ; i!=e; ++i) {
			NEVER_NULL(*i);
			const instance_alias_info<Tag>& a(**i);
		// really want direct ports, and not port-substructures
//		if (!a.get_container_base()->get_super_instance()) {
			ostringstream oss;
			a.dump_hierarchical_name(oss,
				dump_flags::no_leading_scope);
			const string& local_name(oss.str());
			const save_prefix save(*this);
			prefix += local_name + ".";
			STACKTRACE_INDENT_PRINT("prefix = " << prefix << endl);
			sfp.accept(AS_A(global_entry_context&, *this));
//		}
		}
		if (pi >= lpp.port_entries()) {
		sgo += sfp;
		}
	}
	// invariant checks on sgo, consistent with local instance_pools
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Goal: visit each unique process once, but repeat for aliases
	Based on global_entry_dumper::visit(const process_instance&)
 */
void
alias_printer::visit(const state_instance<process_tag>& p) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(g_offset);
	const global_offset& sgo(*g_offset);
	const footprint_frame& spf(*p.get_frame());
	const footprint& sfp(*spf._footprint);
	const global_offset
		b(sgo, sfp, add_local_private_tag()),
		c(sgo, sfp, add_total_private_tag());
//	STACKTRACE_INDENT_PRINT('\t' << sgo << b << c << endl);
//	__default_visit(p);
	// context, but extended to include locals
	const footprint_frame af(spf, *fpf);    // context
#if 0
	af.extend_frame(sgo, b);
	af.dump_extended_frame(os, sgo, b, c) << endl;
#endif
{
	typedef	process_tag			Tag;
	const size_t lpi = p.get_back_ref()->instance_index;	// 1-based
#if ENABLE_STACKTRACE
	const size_t gi = lookup_global_id<Tag>(lpi);	// 1-based
	STACKTRACE_INDENT_PRINT("process[" << gi << "] = ");
	topfp->dump_canonical_name<Tag>(STACKTRACE_STREAM, gi -1) << endl;
#endif
	// don't need canonical name, just set of aliases
	const footprint& f(get_current_footprint());
	const port_alias_tracker& pt(f.get_scope_alias_tracker());
	const alias_reference_set<Tag>&
		ars(pt.get_id_map<Tag>().find(lpi)->second);
	alias_reference_set<Tag>::const_iterator i(ars.begin()), e(ars.end());
	const value_saver<const footprint_frame*> _f_(fpf, &af);
	const value_saver<const global_offset*> _g_(parent_offset, &b);
	for (; i!=e; ++i) {
		NEVER_NULL(*i);
		const instance_alias_info<Tag>& a(**i);
		// exclude some member references (x.y), already covered
	if (!a.get_container_base()->get_super_instance()) {
		ostringstream oss;
		a.dump_hierarchical_name(oss,
			dump_flags::no_leading_scope);
		const string& local_name(oss.str());
		const save_prefix save(*this);
		prefix += local_name + ".";
		STACKTRACE_INDENT_PRINT("prefix = " << prefix << endl);
		sfp.accept(AS_A(global_entry_context&, *this));
	}
	}
}
	*g_offset = c;          // increment global offset with each process
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 1
/**
	TODO: factor this into alias_visitor
 */
void
alias_printer::visit(const state_instance<bool_tag>& b) {
	STACKTRACE_VERBOSE;
	typedef	bool_tag			Tag;
	typedef	class_traits<Tag>		traits_type;
if (traits_type::print_cflat_leaf) {
if (!cf.check_prs) {
	// visit all aliases!
	const size_t bi = b.get_back_ref()->instance_index;	// 1-based
	const size_t gi = lookup_global_id<Tag>(bi);	// 1-based
	STACKTRACE_INDENT_PRINT("gbid = " << gi << endl);
	ostringstream oss;
	topfp->dump_canonical_name<Tag>(oss, gi -1);	// 0-based
	const string& canonical(oss.str());
	STACKTRACE_INDENT_PRINT("canonical = " << canonical << endl);
	const footprint& f(get_current_footprint());
	const port_alias_tracker& pt(f.get_scope_alias_tracker());
	const alias_reference_set<Tag>&
		ars(pt.get_id_map<Tag>().find(bi)->second);
	alias_reference_set<Tag>::const_iterator
		i(ars.begin()), e(ars.end());
	for (; i!=e; ++i) {
		NEVER_NULL(*i);
		const instance_alias_info<Tag>& a(**i);
		// exclude some bool member references (x.y), already covered
	if (!a.get_container_base()->get_super_instance()) {
		ostringstream ass;
		a.dump_hierarchical_name(ass, dump_flags::no_leading_scope);
		const string alias(prefix +ass.str());
		STACKTRACE_INDENT_PRINT("alias = " << alias << endl);
		if (!cf.wire_mode) {
			cflat_print_alias(o, canonical, alias, cf);
		} else if (canonical != alias) {
			// this should only be done for bool_tag!
			INVARIANT(gi < wires.size());
			wires[gi].strings.push_back(alias);
		}
	}
	}
}
}
}
#endif	// TRAVERSE_BY_ALIAS
#endif	// MEMORY_MAPPED_GLOBAL_ALLOCATION

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Rather than take a footprint argument, passed by the collection, 
	we get the footprint each time because collections with
	relaxed types may have different types per element.  
	This executes in a top-down traversal of the hierarchy.  
	NOTE: this code was relocated from instance_alias_info::cflat_aliases.
 */
template <class Tag>
void
alias_printer::__visit(const instance_alias_info<Tag>& a) {
	typedef class_traits<Tag>		traits_type;
	STACKTRACE_VERBOSE;
	// possibly uninstantiated because of conditionals
if (a.valid()) {
	ostringstream os;
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	a.dump_leaf_name(os);
#else
	a.dump_hierarchical_name(os, dump_flags::no_leading_scope);
#endif
	const string& local_name(os.str());
	STACKTRACE_INDENT_PRINT("local " << traits_type::tag_name <<
		" name[" << a.instance_index << "] = " << local_name << endl);
	// construct new prefix from os
	const alias_printer::save_prefix save(*this);
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	const footprint& lf(*fpf->_footprint);
	const typename state_instance<Tag>::pool_type&
		gp(lf.template get_instance_pool<Tag>());
	const state_instance<Tag>& e(gp[a.instance_index -1]);
#if ENABLE_STACKTRACE
	const size_t gindex = lookup_global_id<Tag>(a.instance_index);
	STACKTRACE_INDENT_PRINT("global " << traits_type::tag_name <<
		" name[" << gindex << "] = ");
	topfp->dump_canonical_name<Tag>(STACKTRACE_STREAM, gindex -1) << endl;
#endif
#if 0
	if (!at_top())
		this->prefix += ".";
#endif
#else
	const global_entry_pool<Tag>& gp(this->sm.template get_pool<Tag>());
	size_t gindex;
if (this->fpf) {
	this->prefix += ".";
	// this is not a top-level instance (from recursion)
	const footprint_frame_transformer ft(*this->fpf, Tag());
	gindex = ft(a.instance_index);
} else {
	// footprint_frame is null, this is a top-level instance
	// the instance_index can be used directly as the offset into
	// the state_manager's member arrays
	BOUNDS_CHECK(a.instance_index && a.instance_index < gp.size());
	gindex = a.instance_index;
}
	const global_entry<Tag>& e(gp[gindex]);
#endif
	this->prefix += local_name;
	STACKTRACE_INDENT_PRINT("prefix = " << prefix << endl);
	alias_printer_recursion_policy<traits_type::has_substructure>::accept(
		*this, e
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
//		, *fpf->_footprint
#else
		, gindex
#endif
		);
	// recursion or termination
}	// end if a.valid()
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
#if 0
/**
	Recursively looks up parent's to resolve global id.
	\param 1-based local instance index.
	\return 1-based global instance index.
 */
template <class Tag>
size_t
alias_printer::lookup_global_id(const size_t li) const {
	STACKTRACE_VERBOSE;
	return lookup_local_id<Tag>(li);
//	+this->owner_context().parent_offset->global_offset_base<Tag>::offset;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively looks up local ID.
	\param 1-based local instance index.
	\return 1-based local instance index (relative to owner).
 */
template <class Tag>
size_t
alias_printer::lookup_local_id(const size_t li) const {
	STACKTRACE_VERBOSE;
	const this_type* c = this;
	size_t gi = li;
	while (c) {
		const footprint_frame_map_type&
			ffm(c->fpf->get_frame_map<Tag>());
		INVARIANT(gi <= ffm.size());
		gi = ffm[gi -1];
		STACKTRACE_INDENT_PRINT("index = " << gi << endl);
		c = c->parent;
	}
	return gi;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Find parent context.
 */
const global_entry_context&
alias_printer::owner_context(void) const {
	const this_type* c = this;
	while (c->parent) {
		c = c->parent;
	}
	return *c;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
ostream&
alias_printer::dump_canonical_name(ostream& os, const size_t gi) const {
	if (parent)
		return parent->dump_canonical_name<Tag>(os, gi);
	else	return topfp->dump_canonical_name<Tag>(os, gi);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Choose action based on whether or not this alias is a 
	direct public port (not whether it is aliased to port).  
	Don't forget that alias is visited *locally* through footprint, 
	not publicly as a member alias.
 */
void
alias_printer::visit(const instance_alias_info<process_tag>& a) {
	typedef	process_tag			Tag;
	typedef	value_saver<const footprint_frame*>	footprint_frame_setter;
	STACKTRACE_VERBOSE;
	ostringstream oss;
	a.dump_leaf_name(oss);
	const string& local_name(oss.str());
	const save_prefix save(*this);
	prefix += local_name + '.';
	STACKTRACE_INDENT_PRINT("local  = " << local_name << endl);
	const bool is_private = !a.get_container_base()
		->get_placeholder_base()->is_port_formal();
	STACKTRACE_INDENT_PRINT("is " << (is_private ? "private" : "public") << endl);
	STACKTRACE_INDENT_PRINT("prefix = " << prefix << endl);
	const size_t lpid = a.instance_index;	// local to immediate parent
if (is_private) {
	// lpid needs to be relative to owner parent
//	const size_t lpid = mpid;
#if ENABLE_STACKTRACE
	const size_t gpid = lookup_global_id<Tag>(lpid);
	STACKTRACE_INDENT_PRINT("lpid = " << lpid << ", gpid = " << gpid << endl);
	STACKTRACE_INDENT_PRINT("canonical = ");
	dump_canonical_name<Tag>(STACKTRACE_STREAM, gpid -1) << endl;
#endif
	// lookup unique instance to get footprint
	// context footprint can be parent or auxiliary
	const footprint& parent_fp(*fpf->_footprint);
	const state_instance<Tag>&
		unique_p(parent_fp.get_instance_pool<Tag>()[lpid -1]);
	const footprint_frame& sub_frame(unique_p._frame);
	const footprint& sub_fp(*sub_frame._footprint);
	const footprint& owner_fp(parent_fp);
	STACKTRACE_INDENT_PRINT("parent offset = " << *parent_offset << endl);
	global_offset sgo(*parent_offset);	// copy
	global_offset delta, local;
	if (at_top()) {
		local = global_offset(local, owner_fp, add_all_local_tag());
	} else {
		local = global_offset(local, owner_fp, add_local_private_tag());
	}
	STACKTRACE_INDENT_PRINT("local         = " << local << endl);
	sgo += local;
	owner_fp.set_global_offset_by_process(delta, lpid);	// conditional?
	STACKTRACE_INDENT_PRINT("delta         = " << delta << endl);
	sgo += delta;
	STACKTRACE_INDENT_PRINT("child offset  = " << sgo << endl);
	// might need separate offset to distinguish between public/private...
	const value_saver<const global_offset*> _g_(parent_offset, &sgo);
	footprint_frame aux_ff(sub_frame, *fpf);
	// can't be at top: there's no alias for top!
	// aux_ff.construct_global_context(sub_fp, *cfpf, sgo);
	const global_offset b(sgo, sub_fp, add_local_private_tag());
	aux_ff.extend_frame(sgo, b);
	const footprint_frame_setter _fpf_(fpf, &aux_ff);
#if ENABLE_STACKTRACE
	aux_ff.dump_frame(STACKTRACE_INDENT_PRINT("aux frame =")) << endl;
#endif
//	sub_fp.accept(AS_A(alias_visitor&, *this));
	visit(sub_fp);
} else {
	STACKTRACE_INDENT_PRINT("public process port" << endl);
	// chain printers...
	STACKTRACE_INDENT_PRINT("lpid = " << lpid << endl);
	const footprint& parent_fp(*fpf->_footprint);
#if ENABLE_STACKTRACE
	parent_fp.dump_type(STACKTRACE_INDENT_PRINT("parent.type: ")) << endl;
#endif
	const state_instance<Tag>&
		unique_p(parent_fp.get_instance_pool<Tag>()[lpid -1]);
	const footprint_frame& sub_frame(unique_p._frame);
#if ENABLE_STACKTRACE
	sub_frame.dump_frame(STACKTRACE_INDENT_PRINT("sub frame: ")) << endl;
#endif
#if 0
//	const size_t mpid = lookup_local_id<Tag>(lpid);
	const size_t mpid = fpf->get_frame_map<Tag>()[lpid -1];
	STACKTRACE_INDENT_PRINT("mpid  = " << mpid << endl);
#endif
	const global_entry_context& owner(owner_context());
	const footprint& owner_fp(owner.get_top_footprint());
	const size_t gpid = lookup_global_id<Tag>(lpid);
	STACKTRACE_INDENT_PRINT("gpid = " << gpid << endl);
	STACKTRACE_INDENT_PRINT("parent offset = " << *parent_offset << endl);
	const footprint& sub_fp(*sub_frame._footprint);
	global_offset sgo(*parent_offset);
	global_offset delta;	//, local;
#if 0
	if (at_top()) {
		local = global_offset(local, owner_fp, add_all_local_tag());
	} else {
		local = global_offset(local, owner_fp, add_local_private_tag());
	}
	STACKTRACE_INDENT_PRINT("local         = " << local << endl);
	sgo += local;
#endif
	owner_fp.set_global_offset_by_process(delta, gpid);	// conditional?
	STACKTRACE_INDENT_PRINT("delta         = " << delta << endl);
	sgo += delta;
	STACKTRACE_INDENT_PRINT("child offset  = " << sgo << endl);
#if 1
//	const footprint_frame dummy(sub_fp);
	this_type child(o, sub_frame, sgo, cf, wires, prefix, this);
//	child.visit(sub_fp);
//	visit(...);
	sub_fp.accept(AS_A(global_entry_context&, child));
//	visit(sub_fp);
	// shallow recursion, visit top-locals only
#else
	FINISH_ME(Fang);
#endif
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Choose action based on whether or not this alias is a 
	direct public port (not whether it is aliased to port).  
	Don't forget that alias is visited *locally* through footprint, 
	not publicly as a member alias.
 */
void
alias_printer::visit(const instance_alias_info<bool_tag>& a) {
	typedef	bool_tag			Tag;
	STACKTRACE_VERBOSE;
	ostringstream oss;
	a.dump_leaf_name(oss);
	const string& local_name(oss.str());
	const string alias(prefix +local_name);
	STACKTRACE_INDENT_PRINT("local = " << local_name << endl);
	STACKTRACE_INDENT_PRINT("alias = " << alias << endl);
#if ENABLE_STACKTRACE
	const bool is_private = !a.get_container_base()
		->get_placeholder_base()->is_port_formal();
	STACKTRACE_INDENT_PRINT("is " << (is_private ? "private" : "public") << endl);
#endif
	// find canonical name from gpid
	const size_t lbid = a.instance_index;
	const size_t gbid = lookup_global_id<Tag>(lbid);
	STACKTRACE_INDENT_PRINT("lbid = " << lbid << ", gbid = " << gbid << endl);
	// same for private and public?
	ostringstream c;
//	topfp->dump_canonical_name<Tag>(c, gbid -1);
	dump_canonical_name<Tag>(c, gbid -1);
	cflat_print_alias(o, c.str(), alias, cf);
}
#endif	// MEMORY_MAPPED_GLOBAL_ALLOCATION

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints an alias as specified by the flags.  
	Should be publicly accessible.  
 */
void
cflat_print_alias(ostream& o, const string& canonical, const string& alias,
		const cflat_options& cf) {
if (cf.dump_self_connect || alias != canonical) {
	switch (cf.connect_style) {
		case cflat_options::CONNECT_STYLE_CONNECT:
			o << "connect ";
			break;  
		case cflat_options::CONNECT_STYLE_EQUAL:
			o << "= ";
			break;  
		case cflat_options::CONNECT_STYLE_WIRE:
			o << "wire ";
			break;  
		default:
			o << "alias ";
			break;  
	}       
	if (cf.enquote_names) {
		o << '\"' << canonical << "\" \"" << alias << '\"';
	} else {
		o << canonical << ' ' << alias;
	}       
	o << endl;
}       
}

//=============================================================================
#define	DEFINE_INSTANCE_ALIAS_INFO_VISITOR(Tag)				\
void									\
alias_printer::visit(const instance_alias_info<Tag>& a) {		\
	__visit(a);							\
}

#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
DEFINE_INSTANCE_ALIAS_INFO_VISITOR(bool_tag)
#endif
DEFINE_INSTANCE_ALIAS_INFO_VISITOR(int_tag)
DEFINE_INSTANCE_ALIAS_INFO_VISITOR(enum_tag)
#if ENABLE_DATASTRUCTS
DEFINE_INSTANCE_ALIAS_INFO_VISITOR(datastruct_tag)
#endif
DEFINE_INSTANCE_ALIAS_INFO_VISITOR(channel_tag)
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
DEFINE_INSTANCE_ALIAS_INFO_VISITOR(process_tag)
#endif

#undef	DEFINE_INSTANCE_ALIAS_INFO_VISITOR

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

