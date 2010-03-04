/**
	\file "Object/inst/alias_printer.cc"
	$Id: alias_printer.cc,v 1.8.24.11 2010/03/04 02:53:25 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include "Object/inst/alias_printer.h"
#include "Object/traits/bool_traits.h"
#if BUILTIN_CHANNEL_FOOTPRINTS
#include "Object/global_channel_entry.h"
#endif
#include "Object/traits/chan_traits.h"
#include "Object/traits/proc_traits.h"
#include "Object/state_manager.h"
#include "Object/common/dump_flags.h"
#include "Object/common/alias_string_cache.h"
#include "Object/common/cflat_args.tcc"
#include "main/cflat.h"
#include "main/cflat_options.h"
#include "util/swap_saver.h"


namespace HAC {
namespace entity {
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
using util::value_saver;
using util::swap_saver;
#endif

//=============================================================================
static
void
cflat_print_alias(ostream&, const string&, const string&,
		const cflat_options&);

//=============================================================================
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
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
	accept(alias_printer& c, const GLOBAL_ENTRY<Tag>& e, const size_t gi) {
		typedef class_traits<Tag>		traits_type;
		STACKTRACE_VERBOSE;
	if (traits_type::print_cflat_leaf) {
		ostringstream os;
		e.dump_canonical_name(os, c.topfp, c.sm);
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
	accept(alias_printer& c, const GLOBAL_ENTRY<Tag>& e, const size_t) {
	STACKTRACE_VERBOSE;
	// saves away current footprint frame on stack, and restores on return
	const alias_printer::save_frame save(c, &e._frame);
	NEVER_NULL(c.fpf);
	NEVER_NULL(c.fpf->_footprint);
	c.fpf->_footprint->accept(c);
	}

};	// end struct alias_printer_recursion_policy
#endif	// MEMORY_MAPPED_GLOBAL_ALLOCATION

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
		const string& _p) :
		cflat_aliases_arg_type(
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
			_fpf, g, _p
#else
			_sm, _f, _fpf
#endif
			), 
			local_bool_aliases(), 
			o(_o),
			cf(_cf),
			wires(_w) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
/**
	Save additional structures first.
	Parent call to visit() will call virtual visit_footprint();
 */
void
alias_printer::visit(const footprint& f) {
	STACKTRACE_VERBOSE;
	const swap_saver<alias_set_type> __bpa__(local_bool_aliases);
	cflat_aliases_arg_type::visit(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Traversal: by unique or by alias?  By alias would be best.
	Key: visit every alias exactly once.
	Should follow global_entry_context::visit_recursive.
 */
void
alias_printer::visit_footprint(const footprint& f) {
	STACKTRACE_VERBOSE;
	local_bool_aliases.resize(
		f.get_instance_pool<bool_tag>().local_entries() +1);
		   // allows 1-based index
	STACKTRACE_INDENT_PRINT("resized local-aliases to: " <<
		local_bool_aliases.size() << endl);
	collect_local_aliases<bool_tag>(f, local_bool_aliases);
	visit_local<bool_tag>(f, true);
	visit_recursive(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	General algorithm stolen from global_entry_context::visit_recursive().
	Push this into global_entry_context as visit_recursive_aliases.
 */
void
alias_printer::visit_recursive(const footprint& f) {
	STACKTRACE_VERBOSE;
	typedef	value_saver<const footprint_frame*>	footprint_frame_setter;
	typedef	process_tag				Tag;
#if ENABLE_STACKTRACE
	f.dump_type(STACKTRACE_INDENT_PRINT("type: ")) << endl;
	dump_context(STACKTRACE_INDENT_PRINT("current:\n"));
#endif
	const state_instance<Tag>::pool_type&
		lpp(f.get_instance_pool<Tag>());
	const bool is_top = at_top();
	// copy and increment with each local process
	const size_t pe = lpp.local_entries();
	// but for the top-level only, we start with ports (process too?)
	const size_t pb = is_top ? 0 : lpp.port_entries();
//	const value_saver<const global_offset*> __gs__(parent_offset, &sgo);

// TODO: cache alias sets per footprint! (easy speed-up)
//	we can't because alias sets are context-sensitive
// TODO: factor out this procedure for collecting aliases

// indexed 1-based, by local process index
// this map accumulates aliases of local process, their process ports
// AND private internal aliases to their local ports.  
// set<strings> will keep it unique.
	size_t pi;
// final pass: recursion
for (pi=pb; pi<pe; ++pi) {
	STACKTRACE("iterating local unique process");
	const size_t lpid = pi +1;
	const footprint_frame& af(fframes[lpid]);     // context
	const footprint& sfp(*af._footprint);
	const value_saver<const global_offset*>
		_g_(parent_offset, &offsets[lpid]);
	const footprint_frame_setter ffs(fpf, &af);
	// repeat for each alias as prefix!
#if ENABLE_STACKTRACE
	const size_t gpid = global_entry_context::
		lookup_global_id<Tag>(lpid);
	STACKTRACE_INDENT_PRINT("lpid = " << lpid << endl);
	STACKTRACE_INDENT_PRINT("gpid = " << gpid << endl);
#endif
	set<string>::const_iterator
		i(local_proc_aliases[lpid].begin()),
		e(local_proc_aliases[lpid].end());
	// FIXME: set needs to be appended with local private aliases!
	// TODO: collect set of private internal aliases
	// then iterate over whole set
	// find which processes are subordinate of other processes
	for ( ; i!=e; ++i) {
		const save_prefix save(*this);
		const string& local_name(*i);
		prefix += local_name + ".";
		STACKTRACE_INDENT_PRINT("prefix = " << prefix << endl);
		sfp.accept(AS_A(global_entry_context&, *this));
	}
}
	// invariant checks on sgo, consistent with local instance_pools
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
	INVARIANT(bi < local_bool_aliases.size());
	set<string>& aliases(local_bool_aliases[bi]);

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
	// exclude publicly reachable aliases, as those will be covered
	// by parent/owner process.
	// exclude local bool private aliases that are aliased to port
	if (// accept_deep_alias(a, f)
		!a.get_supermost_collection()->get_placeholder_base()->is_port_formal()
		&& (!((a.instance_index <=
			f.get_instance_pool<bool_tag>().port_entries())
			&& !a.is_port_alias())
		|| any_hierarchical_parent_is_aliased_to_port(a, f)
		)
		) {
		ostringstream ass;
		a.dump_hierarchical_name(ass, dump_flags::no_leading_scope);
		STACKTRACE_INDENT_PRINT("local-alias = " << ass.str() << endl);
		aliases.insert(ass.str());	// unique
	}
	}

	set<string>::const_iterator
		ai(aliases.begin()), ae(aliases.end());
	for ( ; ai!=ae; ++ai) {
		const string alias(prefix +*ai);
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
#endif	// MEMORY_MAPPED_GLOBAL_ALLOCATION

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
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
	a.dump_hierarchical_name(os, dump_flags::no_leading_scope);
	const string& local_name(os.str());
	STACKTRACE_INDENT_PRINT("local " << traits_type::tag_name <<
		" name[" << a.instance_index << "] = " << local_name << endl);
	// construct new prefix from os
	const alias_printer::save_prefix save(*this);
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
	this->prefix += local_name;
	STACKTRACE_INDENT_PRINT("prefix = " << prefix << endl);
	alias_printer_recursion_policy<traits_type::has_substructure>::accept(
		*this, e, gindex);
	// recursion or termination
}	// end if a.valid()
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
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
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
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

