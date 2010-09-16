/**
	\file "Object/inst/alias_printer.cc"
	$Id: alias_printer.cc,v 1.13 2010/09/16 06:31:44 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include "Object/inst/alias_printer.h"
#include "Object/traits/bool_traits.h"
#if BUILTIN_CHANNEL_FOOTPRINTS
#include "Object/global_channel_entry.h"
#endif
#include "Object/traits/chan_traits.h"
#include "Object/traits/proc_traits.h"
#include "Object/common/dump_flags.h"
#include "Object/common/alias_string_cache.h"
#include "Object/common/cflat_args.tcc"
#include "main/cflat.h"
#include "main/cflat_options.h"
#include "util/swap_saver.h"


namespace HAC {
namespace entity {
using util::value_saver;
using util::swap_saver;

//=============================================================================
static
void
cflat_print_alias(ostream&, const string&, const string&,
		const cflat_options&);

//=============================================================================
// class alias_printer method definitions

alias_printer::alias_printer(ostream& _o,
		const footprint_frame& _fpf,
		const global_offset& g,
		const cflat_options& _cf,
		wire_alias_set& _w,
		const string& _p) :
			// pass same alt_dump_flags to skip alternate names
			cflat_aliases_arg_type(_fpf, g, _cf.__dump_flags,
				(_cf.emit_alt_names() ? _cf.alt_dump_flags
					: _cf.__dump_flags),
				_p), 
			alt_prefix(),
			local_bool_aliases(), 
			o(_o),
			cf(_cf),
			wires(_w) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
	// copy and increment with each local process
	const size_t pe = lpp.local_entries();
	// but for the top-level only, we start with ports (process too?)
	const size_t pb = lpp.port_entries();
//	const value_saver<const global_offset*> __gs__(parent_offset, &sgo);

// TODO: cache alias sets per footprint! (easy speed-up)
//	we can't because alias sets are context-sensitive
// TODO: factor out this procedure for collecting aliases

// indexed 1-based, by local process index
// this map accumulates aliases of local process, their process ports
// AND private internal aliases to their local ports.  
// alias_group_type will keep it unique.
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
	alias_group_type::const_iterator
		i(local_proc_aliases[lpid].begin()),
		e(local_proc_aliases[lpid].end());
	// FIXME: set needs to be appended with local private aliases!
	// TODO: collect set of private internal aliases
	// then iterate over whole set
	// find which processes are subordinate of other processes
	for ( ; i!=e; ++i) {
		const value_saver<string> save(prefix);
		const value_saver<string> save_alt(alt_prefix);
#if USE_ALT_ALIAS_PAIRS
		const string& local_name(i->first);
#else
		const string& local_name(*i);
#endif
		prefix += local_name +cf.__dump_flags.process_member_separator;
		if (cf.emit_alt_names()) {
			alt_prefix += i->second		// alternate name
				+cf.alt_dump_flags.process_member_separator;
		}
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
	const bool is_top = at_top();
	STACKTRACE_INDENT_PRINT("gbid = " << gi << endl);
	ostringstream oss;
	topfp->dump_canonical_name<Tag>(oss, gi -1, 
		cf.__dump_flags);	// 0-based
	const string& canonical(oss.str());
	STACKTRACE_INDENT_PRINT("canonical = " << canonical << endl);
	INVARIANT(bi < local_bool_aliases.size());
	alias_group_type& aliases(local_bool_aliases[bi]);

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
		is_top || 
		!a.get_supermost_collection()->get_placeholder_base()->is_port_formal()
		&& (!((a.instance_index <=
			f.get_instance_pool<bool_tag>().port_entries())
				&& !a.is_port_alias())
			|| any_hierarchical_parent_is_aliased_to_port(a, f))
		) {
		ostringstream ass;
		a.dump_hierarchical_name(ass, cf.__dump_flags);
		STACKTRACE_INDENT_PRINT("local-alias = " << ass.str() << endl);
#if USE_ALT_ALIAS_PAIRS
		string& alt_s(aliases[ass.str()]);
		if (cf.emit_alt_names()) {
			ostringstream xs;
			a.dump_hierarchical_name(xs, cf.alt_dump_flags);
			alt_s = xs.str();
		}
#else
		aliases.insert(ass.str());
#endif
		// insertion is unique
	}
	}

	alias_group_type::const_iterator
		ai(aliases.begin()), ae(aliases.end());
	for ( ; ai!=ae; ++ai) {
#if USE_ALT_ALIAS_PAIRS
		const string& suffix(ai->first);
#else
		const string& suffix(*ai);
#endif
		const string alias(prefix +suffix);
		STACKTRACE_INDENT_PRINT("alias = " << alias << endl);
#if USE_ALT_ALIAS_PAIRS
		if (cf.emit_alt_names()) {
			const string& alt_suffix(ai->second);
			// const string alt_alias(alt_prefix +alt_suffix);
			o << "# ALT[" << cf.alt_tool_name << "]: " <<
				cf.alt_name_prefix << 
				alt_prefix << alt_suffix << " = " <<
				alias << endl;
		}
#endif
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
}	// end namespace entity
}	// end namespace HAC

