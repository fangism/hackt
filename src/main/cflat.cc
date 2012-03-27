/**
	\file "main/cflat.cc"
	cflat backwards compability module.  

	$Id: cflat.cc,v 1.35 2011/05/02 21:27:19 fang Exp $
 */

#define	ENABLE_STACKTRACE		0
#define	ENABLE_STATIC_TRACE		0

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <iostream>
#include <string>

#include "main/cflat.h"
#include "main/cflat_options.h"
#include "main/program_registry.h"
#include "main/main_funcs.h"
#if HFLAT_USE_OPTPARSE
#include "util/optparse.tcc"
#else
#include "main/options_modifier.tcc"
#endif
#include "main/global_options.h"
#include "common/config.h"

#include "Object/type/process_type_reference.h"
#include "common/TODO.h"

#include "util/getopt_mapped.tcc"
#include "util/stacktrace.h"
#include "util/persistent_object_manager.h"

namespace HAC {
using std::string;
using util::persistent;
using util::persistent_object_manager;
#include "util/using_ostream.h"
using entity::process_type_reference;


//=============================================================================
// explicit early class instantiation for proper static initializer ordering
// this guarantees the registry map is initialized before anything is registered

#if HFLAT_USE_OPTPARSE
typedef	util::options_map_impl<cflat_options>		options_map_impl_type;
typedef	options_map_impl_type::opt_map_type		opt_map_type;
static	options_map_impl_type				options_map_wrapper;
#else
// preferred by g++-3.3, but rejected by g++-4
// template class cflat::options_modifier_policy_type;
// accepted by both :S (hate having to copy exact name...)
template class options_modifier_policy<cflat_options>;
#endif

//=============================================================================
// class cflat static initializers

cflat::master_options_map_type
cflat::master_options;

const int
cflat::master_options_initialized = cflat::initialize_master_options_map();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char
cflat::name[] = "cflat";

const char
cflat::brief_str[] =
	"Flattens instantiations CAST-style (backward compatibility)";

STATIC_TRACE_HERE("before cflat registry")

#ifndef	WITH_MAIN
const size_t
cflat::program_id = register_hackt_program_class<cflat>();
#endif

//=============================================================================
#if HFLAT_USE_OPTPARSE
static const string default_options_brief("(CAST cflat preset)");

class cflat::register_options_modifier {
	typedef	options_map_impl_type::opt_entry	opt_entry;
	typedef	options_map_impl_type::opt_func		modifier_type;
	const opt_entry&				receipt;

public:
	register_options_modifier(const string& Mode, 
			const modifier_type COM, 
			const string& h = default_options_brief) :
		receipt(options_map_wrapper.options_map[Mode] =
			opt_entry(COM, NULL, NULL, h)) {
	}

	register_options_modifier(const string& Mode, 
			const modifier_type COM, 
			const char* h) :
		receipt(options_map_wrapper.options_map[Mode] =
			opt_entry(COM, NULL, NULL, h)) {
	}
} __ATTRIBUTE_UNUSED__ ;
#else
static const char default_options_brief[] = "(CAST cflat preset)";

/**
	CFLAT mode registration mechanism.  
 */
class cflat::register_options_modifier :
	public options_modifier_policy_type::register_options_modifier_base {
	typedef	options_modifier_policy_type::register_options_modifier_base
						base_type;
public:
	register_options_modifier(const string& Mode,
			const modifier_type COM, 
			const char* b = default_options_brief) :
			base_type(Mode, COM, b) {
	}

} __ATTRIBUTE_UNUSED__ ;	// end class register_options_modifier
#endif

//=============================================================================
// cflat::options_modifier declarations and definitions

/***
	Only the primary modes set the primary_tool field, 
	all other mode modifiers leave it untouched.  
***/

#if HFLAT_USE_OPTPARSE
#define	OPTARG			const util::option_value& v, 
#define	OPTARG_UNUSED		const util::option_value&, 
#define	OPTARG_FWD		v, 
typedef	bool			optfun_return_type;
#define	OPTFUN_RETURN		return false;
#else
#define	OPTARG
#define	OPTARG_UNUSED
#define	OPTARG_FWD
typedef	void			optfun_return_type;
#define	OPTFUN_RETURN
#endif

/// Does absolutely nothing.  
static
optfun_return_type
__cflat_default(OPTARG_UNUSED cflat::options&) {
	OPTFUN_RETURN
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// preset modes
static
optfun_return_type
__cflat_prsim(OPTARG_UNUSED cflat::options& cf) {
	cf.primary_tool = cflat::options::TOOL_PRSIM;
	cf.tool_options = cflat::options::TOOL_OPTIONS_DEFAULT;
	cf.connect_style = cflat::options::CONNECT_STYLE_EQUAL;
	cf.include_prs = true;
	cf.dump_self_connect = false;
	cf.enquote_names = true;
	cf.dump_non_bools = false;
	cf.namespace_policy = cflat::options::NAMESPACE_POLICY_NONE;
	cf.check_prs = false;
	cf.wire_mode = false;
	cf.csim_style_prs = false;
	cf.dsim_prs = false;
	cf.compute_conductances = false;
	cf.show_precharges = false;
	cf.show_supply_nodes = false;
	cf.show_hierarchy = false;
	cf.show_channel_terminals = false;
	cf.size_prs = false;
	cf.use_referenced_type_instead_of_top_level = false;
	OPTFUN_RETURN
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
optfun_return_type
__cflat_prlint(OPTARG_UNUSED cflat::options& cf) {
	cf.primary_tool = cflat::options::TOOL_PRLINT;
	cf.tool_options = cflat::options::TOOL_OPTIONS_DEFAULT;
	cf.connect_style = cflat::options::CONNECT_STYLE_NONE;
	cf.include_prs = true;
	cf.dump_self_connect = false;
	cf.enquote_names = true;
	cf.dump_non_bools = false;
	cf.namespace_policy = cflat::options::NAMESPACE_POLICY_NONE;
	cf.check_prs = false;
	cf.wire_mode = false;
	cf.csim_style_prs = false;
	cf.dsim_prs = false;
	cf.size_prs = false;
	cf.compute_conductances = false;
	cf.show_precharges = false;
	cf.show_supply_nodes = false;
	cf.show_hierarchy = false;
	cf.show_channel_terminals = false;
	cf.use_referenced_type_instead_of_top_level = false;
	OPTFUN_RETURN
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
optfun_return_type
__cflat_connect(OPTARG_UNUSED cflat::options& cf) {
	// cf.primary_tool = ?
	// cf.tool_options = cflat::options::TOOL_OPTIONS_DEFAULT;
	cf.connect_style = cflat::options::CONNECT_STYLE_CONNECT;
	cf.include_prs = false;
	cf.dump_self_connect = true;
	cf.enquote_names = true;
	cf.dump_non_bools = false;
	cf.namespace_policy = cflat::options::NAMESPACE_POLICY_NONE;
	cf.wire_mode = false;
	cf.csim_style_prs = false;
	cf.check_prs = false;
	cf.wire_mode = false;
	cf.csim_style_prs = false;
	cf.dsim_prs = false;
	cf.size_prs = false;
	cf.compute_conductances = false;
	cf.show_precharges = false;
	cf.show_supply_nodes = false;
	cf.show_hierarchy = false;
	cf.show_channel_terminals = false;
	cf.use_referenced_type_instead_of_top_level = false;
	OPTFUN_RETURN
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Also works for:
	ergen, prs2tau, LVS (what's the difference?), 
	alint
 */
static
optfun_return_type
__cflat_lvs(OPTARG_UNUSED cflat::options& cf) {
	cf.primary_tool = cflat::options::TOOL_LVS;
	cf.tool_options = cflat::options::TOOL_OPTIONS_DEFAULT;
	cf.connect_style = cflat::options::CONNECT_STYLE_CONNECT;
	cf.include_prs = true;
	cf.dump_self_connect = false;
	cf.enquote_names = true;
	cf.dump_non_bools = false;
	cf.namespace_policy = cflat::options::NAMESPACE_POLICY_NONE;
	cf.check_prs = false;
	cf.wire_mode = false;
	cf.csim_style_prs = false;
	cf.dsim_prs = false;
	cf.size_prs = false;
	cf.compute_conductances = false;
	cf.show_precharges = false;	// maybe true?
	cf.show_supply_nodes = false;
	cf.show_hierarchy = false;
	cf.show_channel_terminals = false;
	cf.use_referenced_type_instead_of_top_level = false;
	OPTFUN_RETURN
}

/**
	For the java-lvs flavor, some variations.  
 */
static
optfun_return_type
__cflat_java_lvs(OPTARG cflat::options& cf) {
	__cflat_lvs(OPTARG_FWD cf);
	cf.node_attributes = true;
	cf.split_instance_attributes = true;
	cf.literal_attributes = false;	// new, tool may not be ready for yet
	cf.expand_pass_gates = false;
	OPTFUN_RETURN
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Also works for:
	aspice, Aspice
 */
static
optfun_return_type
__cflat_wire(OPTARG_UNUSED cflat::options& cf) {
	// cf.primary_tool = ?
	cf.connect_style = cflat::options::CONNECT_STYLE_WIRE;
	cf.include_prs = false;
	cf.dump_self_connect = false;
	cf.enquote_names = false;
	cf.dump_non_bools = false;
	cf.namespace_policy = cflat::options::NAMESPACE_POLICY_NONE;
	cf.check_prs = false;
	cf.wire_mode = true;
	cf.csim_style_prs = false;
	cf.dsim_prs = false;
	cf.size_prs = false;
	cf.compute_conductances = false;
	cf.use_referenced_type_instead_of_top_level = false;
	OPTFUN_RETURN
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
optfun_return_type
__cflat_ADspice(OPTARG_UNUSED cflat::options& cf) {
	// cf.primary_tool = ?
	// cf.tool_options = cflat::options::TOOL_OPTIONS_DEFAULT;
	cf.connect_style = cflat::options::CONNECT_STYLE_WIRE;
	cf.include_prs = true;
	cf.dump_self_connect = false;
	cf.enquote_names = false;
	cf.dump_non_bools = false;
	cf.namespace_policy = cflat::options::NAMESPACE_POLICY_NONE;
	cf.check_prs = false;
	cf.wire_mode = true;
	cf.csim_style_prs = false;
	cf.dsim_prs = true;
	cf.size_prs = false;
	cf.compute_conductances = false;
	cf.show_precharges = false;
	cf.show_supply_nodes = false;
	cf.show_hierarchy = false;
	cf.show_channel_terminals = false;
	cf.use_referenced_type_instead_of_top_level = false;
	OPTFUN_RETURN
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
optfun_return_type
__cflat_vcd(OPTARG_UNUSED cflat::options& cf) {
	cf.primary_tool = cflat::options::TOOL_VCD;
	cf.tool_options = cflat::options::TOOL_OPTIONS_DEFAULT;
	cf.connect_style = cflat::options::CONNECT_STYLE_HIERARCHICAL;
	cf.include_prs = false;
	cf.dump_self_connect = false;
	cf.enquote_names = false;
	cf.dump_non_bools = false;
	cf.namespace_policy = cflat::options::NAMESPACE_POLICY_NONE;
	cf.check_prs = false;
	cf.wire_mode = false;
	cf.csim_style_prs = false;
	cf.dsim_prs = false;
	cf.compute_conductances = false;
	cf.show_precharges = false;
	cf.show_supply_nodes = false;
	cf.show_hierarchy = false;
	cf.show_channel_terminals = false;
	cf.size_prs = false;
	cf.use_referenced_type_instead_of_top_level = false;
	OPTFUN_RETURN
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just check, don't print.
 */
static
optfun_return_type
__cflat_check(OPTARG_UNUSED cflat::options& cf) {
	// cf.primary_tool = ?
	// cf.tool_options = cflat::options::TOOL_OPTIONS_DEFAULT;
	cf.connect_style = cflat::options::CONNECT_STYLE_NONE;
	cf.include_prs = true;
	cf.check_prs = true;
	cf.wire_mode = false;
	cf.csim_style_prs = false;
	cf.dsim_prs = false;
	cf.size_prs = false;
	cf.use_referenced_type_instead_of_top_level = false;
	OPTFUN_RETURN
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
optfun_return_type
__cflat_ipple(OPTARG_UNUSED cflat::options& cf) {
	cf.primary_tool = cflat::options::TOOL_LAYOUT;
	cf.tool_options = cflat::options::TOOL_OPTIONS_DEFAULT;
	cf.connect_style = cflat::options::CONNECT_STYLE_NONE;
	cf.include_prs = false;
	cf.dump_self_connect = false;
	cf.enquote_names = false;
	cf.dump_non_bools = true;
	cf.namespace_policy = cflat::options::NAMESPACE_POLICY_NONE;
	cf.check_prs = false;
	cf.wire_mode = false;
	cf.csim_style_prs = false;
	cf.dsim_prs = false;
	cf.compute_conductances = false;
	cf.show_precharges = false;
	cf.show_supply_nodes = false;
	cf.show_hierarchy = true;
	cf.show_channel_terminals = true;
//	cf.use_referenced_type_instead_of_top_level = false;
	OPTFUN_RETURN
}

// end of primary modes
//=============================================================================
STATIC_TRACE_HERE("before cflat option modifiers")

static const cflat::register_options_modifier
/***
@texinfo cflat/mode-prsim.texi
@defvr {@t{cflat} option} prsim
@command{prsim} output mode.
@end defvr
@end texinfo
***/
	cflat_opt_mod_prsim("prsim", &__cflat_prsim),

/***
@texinfo cflat/mode-lvs.texi
@defvr {@t{cflat} option} lvs
@defvrx {@t{cflat} option} LVS
@defvrx {@t{cflat} option} java-lvs
LVS output mode.
The java-lvs option is a slight variant from the traditional lvs.  
@end defvr
@end texinfo
***/
	cflat_opt_mod_lvs("lvs", &__cflat_lvs),
	cflat_opt_mod_LVS("LVS", &__cflat_lvs),		// re-use lvs
	cflat_opt_mod_java_lvs("java-lvs", &__cflat_java_lvs),

/***
@texinfo cflat/mode-ergen.texi
@defvr {@t{cflat} option} ergen
@command{ergen} output mode.
@end defvr
@end texinfo
***/
	cflat_opt_mod_ergen("ergen", &__cflat_lvs),		// re-use lvs

/***
@texinfo cflat/mode-alint.texi
@defvr {@t{cflat} option} alint
@command{alint} output mode.
@end defvr
@end texinfo
***/
	cflat_opt_mod_alint("alint", &__cflat_lvs),		// re-use lvs

/***
@texinfo cflat/mode-prlint.texi
@defvr {@t{cflat} option} prlint
@command{prlint} output mode.
@end defvr
@end texinfo
***/
	cflat_opt_mod_prlint("prlint", &__cflat_prlint),

/***
@texinfo cflat/mode-prs2tau.texi
@defvr {@t{cflat} option} prs2tau
@command{prs2tau} output mode.
@end defvr
@end texinfo
***/
	cflat_opt_mod_prs2tau("prs2tau", &__cflat_lvs),	// re-use lvs

/***
@texinfo cflat/mode-connect.texi
@defvr {@t{cflat} option} connect
@command{connect} output mode.
@end defvr
@end texinfo
***/
	cflat_opt_mod_connect("connect", &__cflat_connect),

/***
@texinfo cflat/mode-vcd.texi
@defvr {@t{cflat} option} vcd
@command{vcd} (standard trace file) header output mode.
@end defvr
@end texinfo
***/
	cflat_opt_mod_vcd("vcd", &__cflat_vcd),

/***
@texinfo cflat/mode-check.texi
@defvr {@t{cflat} option} check
@command{check} output mode.
@end defvr
@end texinfo
***/
	cflat_opt_mod_check("check", &__cflat_check),

/***
@texinfo cflat/mode-wire.texi
@defvr {@t{cflat} option} wire
@command{wire} output mode.
@end defvr
@end texinfo
***/
	cflat_opt_mod_wire("wire", &__cflat_wire),

/***
@texinfo cflat/mode-aspice.texi
@defvr {@t{cflat} option} aspice
@defvrx {@t{cflat} option} Aspice
@command{aspice} output mode.
@end defvr
@end texinfo
***/
	cflat_opt_mod_aspice("aspice", &__cflat_wire),	// re-use wire
	cflat_opt_mod_Aspice("Aspice", &__cflat_wire),	// re-use wire

/***
@texinfo cflat/mode-ADspice.texi
@defvr {@t{cflat} option} ADspice
@command{ADspice} output mode.
@end defvr
@end texinfo
***/
	cflat_opt_mod_ADspice("ADspice", &__cflat_ADspice),

/***
@texinfo cflat/mode-ipple.texi
@defvr {@t{cflat} option} ipple
@command{ipple} output mode.
Prints out hierarchical processes and source/destination terminals of channels.
@end defvr
@end texinfo
***/
	cflat_opt_mod_ipple("ipple", &__cflat_ipple),
/***
@texinfo cflat/mode-default.texi
@defvr {@t{cflat} option} default
@command{default} output mode.
@end defvr
@end texinfo
***/
	cflat_opt_mod_default("default", &__cflat_default);

STATIC_TRACE_HERE("after cflat option modifiers")

//-----------------------------------------------------------------------------
// for setting/unsetting individual flags

// TODO: reperitive code should be templated or macro'd
// define for common patterns to reduce clutter.

/***
@texinfo cflat/opt-connect-none.texi
@defvr {@t{cflat -f} option} no-connect
@defvrx {@t{cflat -f} option} connect-none
Suppress printing of aliases.  
@end defvr
@end texinfo
***/
#if HFLAT_USE_OPTPARSE
#define	SET_CONNECT_STYLE(k)	options_map_impl_type::set_member_constant<unsigned char, &cflat_options::connect_style, cflat::options::k>
#define	__cflat_connect_none	SET_CONNECT_STYLE(CONNECT_STYLE_NONE)
#define	__cflat_connect_equal	SET_CONNECT_STYLE(CONNECT_STYLE_EQUAL)
#define	__cflat_connect_connect	SET_CONNECT_STYLE(CONNECT_STYLE_CONNECT)
#define	__cflat_connect_wire	SET_CONNECT_STYLE(CONNECT_STYLE_WIRE)
#else
static
optfun_return_type
__cflat_connect_none(OPTARG_UNUSED cflat::options& cf) {
	cf.connect_style = cflat::options::CONNECT_STYLE_NONE;
	OPTFUN_RETURN
}
#endif
static const cflat::register_options_modifier
	cflat_opt_mod_connect_none("connect-none", &__cflat_connect_none, 
		"suppresses all printing of aliases"), 
	cflat_opt_mod_no_connect("no-connect", &__cflat_connect_none, 
		"(connect-none)");

/***
@texinfo cflat/opt-connect-equal.texi
@defvr {@t{cflat -f} option} connect-equal
Print aliases with style: @samp{= x y}.
@end defvr
@end texinfo
***/
#if !HFLAT_USE_OPTPARSE
static
optfun_return_type
__cflat_connect_equal(OPTARG_UNUSED cflat::options& cf) {
	cf.connect_style = cflat::options::CONNECT_STYLE_EQUAL;
	OPTFUN_RETURN
}
#endif
static const cflat::register_options_modifier
	cflat_opt_mod_connect_equal("connect-equal", &__cflat_connect_equal, 
		"alias-style: = x y");

/***
@texinfo cflat/opt-connect-connect.texi
@defvr {@t{cflat -f} option} connect-connect
Print aliases with style: @samp{connect x y}.
@end defvr
@end texinfo
***/
#if !HFLAT_USE_OPTPARSE
static
optfun_return_type
__cflat_connect_connect(OPTARG_UNUSED cflat::options& cf) {
	cf.connect_style = cflat::options::CONNECT_STYLE_CONNECT;
	OPTFUN_RETURN
}
#endif
static const cflat::register_options_modifier
	cflat_opt_mod_connect_connect("connect-connect", &__cflat_connect_connect,
		"alias-style: connect x y");

/***
@texinfo cflat/opt-connect-wire.texi
@defvr {@t{cflat -f} option} connect-wire
Print aliases with style: @samp{wire x y}.
@end defvr
@end texinfo
***/
#if !HFLAT_USE_OPTPARSE
static
optfun_return_type
__cflat_connect_wire(OPTARG_UNUSED cflat::options& cf) {
	cf.connect_style = cflat::options::CONNECT_STYLE_WIRE;
	OPTFUN_RETURN
}
#endif
static const cflat::register_options_modifier
	cflat_opt_mod_connect_wire("connect-wire", &__cflat_connect_wire,
		"alias-style: wire x y");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if HFLAT_USE_OPTPARSE
#define	SET_BOOL_OPT(m,v)	options_map_impl_type::set_member_constant<bool, &cflat_options::m, v>
#endif

/***
@texinfo cflat/opt-include-prs.texi
@defvr {@t{cflat -f} option} include-prs
@defvrx {@t{cflat -f} option} exclude-prs
@defvrx {@t{cflat -f} option} no-include-prs
@defvrx {@t{cflat -f} option} no-exclude-prs
Include or exclude production rules from output.  
@end defvr
@end texinfo
***/
#if HFLAT_USE_OPTPARSE
#define	__cflat_include_prs	SET_BOOL_OPT(include_prs, true)
#define	__cflat_exclude_prs	SET_BOOL_OPT(include_prs, false)
#else
static
optfun_return_type
__cflat_include_prs(OPTARG_UNUSED cflat::options& cf) {
	cf.include_prs = true;
	OPTFUN_RETURN
}
static
optfun_return_type
__cflat_exclude_prs(OPTARG_UNUSED cflat::options& cf) {
	cf.include_prs = false;
	OPTFUN_RETURN
}
#endif

static const cflat::register_options_modifier
	cflat_opt_mod_include_prs("include-prs", &__cflat_include_prs, 
		"includes prs in output"),
	cflat_opt_mod_no_exclude_prs("no-exclude-prs", &__cflat_include_prs, 
		"(include-prs)"),
	cflat_opt_mod_exclude_prs("exclude-prs", &__cflat_exclude_prs,
		"excludes prs from output"),
	cflat_opt_mod_no_include_prs("no-include-prs", &__cflat_exclude_prs, 
		"(exclude-prs)");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if HFLAT_USE_OPTPARSE
#define	DEFINE_BOOL_OPTION_PAIR(mem, key, truestr, falsestr)		\
static const cflat::register_options_modifier				\
	cflat_opt_mod_ ## mem(key, &SET_BOOL_OPT(mem, true), truestr),	\
	cflat_opt_mod_no_## mem("no-" key, &SET_BOOL_OPT(mem, false),	\
		falsestr);
#else
#define	DEFINE_BOOL_OPTION_PAIR(mem, key, truestr, falsestr)		\
static									\
optfun_return_type							\
__cflat_ ## mem(OPTARG_UNUSED cflat::options& cf) {			\
	cf.mem = true;							\
	OPTFUN_RETURN							\
}									\
static									\
optfun_return_type							\
__cflat_no_ ## mem(OPTARG_UNUSED cflat::options& cf) {			\
	cf.mem = false;							\
	OPTFUN_RETURN							\
}									\
static const cflat::register_options_modifier				\
	cflat_opt_mod_ ## mem(key, &__cflat_ ## mem, truestr),		\
	cflat_opt_mod_no_## mem("no-" key, &__cflat_no_ ## mem, falsestr);
#endif

/***
@texinfo cflat/opt-self-aliases.texi
@defvr {@t{cflat -f} option} self-aliases
@defvrx {@t{cflat -f} option} no-self-aliases
Includes or exclude aliases @samp{x = x}.
@end defvr
@end texinfo
***/
DEFINE_BOOL_OPTION_PAIR(dump_self_connect, "self-aliases",
	"includes aliases x = x",
	"excludes aliases x = x")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-precharges.texi
@defvr {@t{cflat -f} option} precharges
@defvrx {@t{cflat -f} option} no-precharges
Print or hide precharge expressions.
@end defvr
@end texinfo
***/
DEFINE_BOOL_OPTION_PAIR(show_precharges, "precharges",
	"show precharge expressions",
	"hide precharge expressions")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-supply-nodes.texi
@defvr {@t{cflat -f} option} supply-nodes
@defvrx {@t{cflat -f} option} no-supply-nodes
Print or hide supply-nodes associated with each rule.
This is mostly useful for checking circuits involving 
multiple voltage domains.
@end defvr
@end texinfo
***/
DEFINE_BOOL_OPTION_PAIR(show_supply_nodes, "supply-nodes",
	"show voltage supply nodes per rule",
	"hide voltage supply nodes per rule")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-process-hierarchy.texi
@defvr {@t{cflat -f} option} process-hierarchy
@defvrx {@t{cflat -f} option} no-process-hierarchy
When enabled, rules are encapsulated by the names of the process
to which they belong, in nested fashion.  
@end defvr
@end texinfo
***/
DEFINE_BOOL_OPTION_PAIR(show_hierarchy, "process-hierarchy",
	"show process hierarchy in rules",
	"hide process hierarchy in rules")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-channel-terminals.texi
@defvr {@t{cflat -f} option} channel-terminals
@defvrx {@t{cflat -f} option} no-channel-terminals
When enabled, channels' sources and destinations are printed
out as the hierarchy is traversed.
Recommend @option{process-hierarchy} with this option.
@end defvr
@end texinfo
***/
DEFINE_BOOL_OPTION_PAIR(show_channel_terminals, "channel-terminals",
	"show channel terminals in hierarchical processes",
	"hide channel terminals in hierarchical processes")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-mangled-vcd-ids.texi
@defvr {@t{cflat -f} option} mangled-vcd-ids
@defvrx {@t{cflat -f} option} no-mangled-vcd-ids
For the vcd output mode,
when enabled, print out base-94 ASCII characters for unique identifiers, 
otherwise print out human-readable <integer> values for identifiers.  
Default: true (mangled)
@end defvr
@end texinfo
***/
DEFINE_BOOL_OPTION_PAIR(mangled_vcd_ids, "mangled-vcd-ids",
	"use base-94 ASCII unique identifiers for vcd",
	"use integer unique identifiers for vcd")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-quote-names.texi
@defvr {@t{cflat -f} option} quote-names
@defvrx {@t{cflat -f} option} no-quote-names
Wrap all node names in ``quotes''.  
@end defvr
@end texinfo
***/
DEFINE_BOOL_OPTION_PAIR(enquote_names, "quote-names",
	"wraps all node names in \"quotes\"",
	"no quote around node names")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-node-attributes.texi
@defvr {@t{cflat -f} option} node-attributes
@defvrx {@t{cflat -f} option} no-node-attributes
Whether or not to print node attributes.  
@end defvr
@end texinfo
***/
DEFINE_BOOL_OPTION_PAIR(node_attributes, "node-attributes",
	"print node attributes",
	"suppress node attributes")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-split-instance-attributes.texi
@defvr {@t{cflat -f} option} split-instance-attributes
@defvrx {@t{cflat -f} option} join-instance-attributes
Determines whether to print instance attributes (including nodes)
on a single line like:
@example
@@ "node" attr1 attr2 attr3 ...
@end example
or one attribute per line:
@example
@@ "node" attr1
@@ "node" attr2
@@ "node" attr3
...
@end example
@end defvr
@end texinfo
***/
DEFINE_BOOL_OPTION_PAIR(split_instance_attributes, "join-instance-attributes",
	"print one attribute per line",
	"group instance attributes in a single line")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-literal-attributes.texi
@defvr {@t{cflat -f} option} literal-attributes
@defvrx {@t{cflat -f} option} no-literal-attributes
Whether or not to print node literal attributes within rules.  
@end defvr
@end texinfo
***/
DEFINE_BOOL_OPTION_PAIR(literal_attributes, "literal-attributes",
	"print rule literal attributes",
	"suppress rule literal attributes")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-expand-pass-gates.texi
@defvr {@t{cflat -f} option} expand-pass-gates
@defvrx {@t{cflat -f} option} no-expand-pass-gates
Whether or not to expand pass gates to their equivalent
unidirectional production rules.  
@end defvr
@end texinfo
***/
DEFINE_BOOL_OPTION_PAIR(expand_pass_gates, "expand-pass-gates",
	"expand pass gates to production rules",
	"do not expand pass gates as production rules")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-SEU.texi
@defvr {@t{cflat -f} option} SEU
@defvrx {@t{cflat -f} option} no-SEU
Enable single-event-upset mode for selected tool.  
@end defvr
@end texinfo
***/
// TODO: define a bitfield setter in util::optparse
static
optfun_return_type
__cflat_SEU(OPTARG_UNUSED cflat::options& cf) {
	cf.tool_options |= cflat::options::TOOL_OPTIONS_SEU;
	OPTFUN_RETURN
}
static const cflat::register_options_modifier
	cflat_opt_mod_SEU("SEU", &__cflat_SEU,
		"enable single-event-upset mode for tool");

static
optfun_return_type
__cflat_no_SEU(OPTARG_UNUSED cflat::options& cf) {
	cf.tool_options &= ~cflat::options::TOOL_OPTIONS_SEU;
	OPTFUN_RETURN
}
static const cflat::register_options_modifier
	cflat_opt_mod_no_SEU("no-SEU", &__cflat_no_SEU, 
		"disable single-event-upset mode");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	cf.dump_non_bools = false;
//	cf.namespace_policy = cflat::options::NAMESPACE_POLICY_NONE;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-check-mode.texi
@defvr {@t{cflat -f} option} check-mode
@defvrx {@t{cflat -f} option} no-check-mode
Silences @command{cflat} output while traversing hierarchy.  
Useful only as a diagnostic tool for debugging.  
@end defvr
@end texinfo
***/
DEFINE_BOOL_OPTION_PAIR(check_prs, "check-mode",
	"silences cflat output while traversing hierarchy",
	"cflat output enabled")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-wire-mode.texi
@defvr {@t{cflat -f} option} wire-mode
@defvrx {@t{cflat -f} option} no-wire-mode
Accumulate aliases in the form: @samp{wire (x,y,...)}
@end defvr
@end texinfo
***/
DEFINE_BOOL_OPTION_PAIR(wire_mode, "wire-mode",
	"accumulate aliases in the form: wire (x,y,...)",
	"use one of the connect-* modes of printing aliases")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-dsim-prs.texi
@defvr {@t{cflat -f} option} dsim-prs
@defvrx {@t{cflat -f} option} no-dsim-prs
Wraps prs in: @samp{dsim @{ ... @}}
@end defvr
@end texinfo
***/
DEFINE_BOOL_OPTION_PAIR(dsim_prs, "dsim-prs",
	"wraps prs in: dsim { ... }",
	"not (dsim-prs)")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-sizes.texi
@defvr {@t{cflat -f} option} sizes
@defvrx {@t{cflat -f} option} no-sizes
Prints rule literals with @t{<size>} specifications.  
@end defvr
@end texinfo
***/
DEFINE_BOOL_OPTION_PAIR(size_prs, "sizes",
	"prints rule literals with <size> specifications",
	"not (size-prs)")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-strengths.texi
@defvr {@t{cflat -f} option} strengths
@defvrx {@t{cflat -f} option} no-strengths
Prints min/max drive strengths for each rule.
Strengths are computed as such:
the maximum strength assumes that every guard subexpression is true
and conducting to the supply;
the minimum strength assumes that a single weakest path is conducting, 
and 'one' value for the stronge single path.
CMOS implementability of rules is ignored.  
The strength unit is relative to a 1W/1L drive strength, 
and is not normalized with respect to NFET/PFET mobility. 
The default width is 5, and default length is 2.  
@end defvr
@end texinfo
***/
DEFINE_BOOL_OPTION_PAIR(compute_conductances, "strengths",
	"prints min/max strengths for each rule",
	"suppresses min/max strengths for each rule")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	cf.csim_style_prs = false;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-mangle.texi
@defvr {@t{cflat -f} option} process_member_separator=SEP
By default, '@t{.}' is used to separate process hierarchy.
@var{SEP} can be any string that doesn't contain a comma.
@end defvr
@defvr {@t{cflat -f} option} struct_member_separator=SEP
By default, '@t{.}' is used to denote channel or structure members.
@var{SEP} can be any string that doesn't contain a comma.
@end defvr
@end texinfo
***/
static
optfun_return_type
__cflat_process_member_separator(OPTARG cflat_options& cf) {
	cf.__dump_flags.process_member_separator = v.values.front();
	// ignore excess values
	OPTFUN_RETURN
}
static
optfun_return_type
__cflat_struct_member_separator(OPTARG cflat_options& cf) {
	cf.__dump_flags.struct_member_separator = v.values.front();
	// ignore excess values
	OPTFUN_RETURN
}

static const cflat::register_options_modifier
	cflat_opt_mod_process_member_separator(
		"process_member_separator",
		&__cflat_process_member_separator, 
		"override the default process hierarchy delimiter"),
	cflat_opt_mod_struct_member_separator(
		"struct_member_separator",
		&__cflat_struct_member_separator, 
		"override the default struct member delimiter");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-alternate-names.texi
@defvr {@t{cflat -f} option} alt_tool_name=name
By setting an alternate tool name (just a string), this enables the printing
of an additional name map between the baseline names and 
alternatively mangled names.  
For example, you may wish to generate a map between equivalent spice
names and hflat names.  
@end defvr

The following @t{alt-} options control the mangling for names
emitted for the @emph{alternate} tool.

@defvr {@t{cflat -f} option} alt_name_prefix=STR
This specifies a string to prefix in front of all tool names. 
For example, @option{alt_name_prefix=TOP.} will turn a hierarchical
name @t{x.y.z} into @t{TOP.x.y.z}.
@end defvr

@defvr {@t{cflat -f} option} alt_process_member_separator=SEP
By default, '@t{.}' is used to separate process hierarchy.
@var{SEP} can be any string that doesn't contain a comma.
@end defvr

@defvr {@t{cflat -f} option} alt_struct_member_separator=SEP
By default, '@t{.}' is used to denote channel or structure members.
@var{SEP} can be any string that doesn't contain a comma.
@end defvr
@end texinfo
***/
static
optfun_return_type
__cflat_alt_process_member_separator(OPTARG cflat_options& cf) {
	cf.alt_dump_flags.process_member_separator = v.values.front();
	// ignore excess values
	OPTFUN_RETURN
}
static
optfun_return_type
__cflat_alt_struct_member_separator(OPTARG cflat_options& cf) {
	cf.alt_dump_flags.struct_member_separator = v.values.front();
	// ignore excess values
	OPTFUN_RETURN
}

static const cflat::register_options_modifier
#if HFLAT_USE_OPTPARSE
	cflat_opt_mod_alt_tool_name(
		"alt_tool_name",
		&options_map_impl_type::set_member_single_string<
			&cflat_options::alt_tool_name>, 
		"enable alternate tool name mapping"),
	cflat_opt_mod_alt_name_prefix(
		"alt_name_prefix",
		&options_map_impl_type::set_member_single_string<
			&cflat_options::alt_name_prefix>, 
		"common prefix for alternate tool names"),
#endif
	cflat_opt_mod_alt_process_member_separator(
		"alt_process_member_separator",
		&__cflat_alt_process_member_separator, 
		"set the alternate tool process hierarchy delimiter"),
	cflat_opt_mod_alt_struct_member_separator(
		"alt_struct_member_separator",
		&__cflat_alt_struct_member_separator, 
		"set the alternate tool struct member delimiter");

//=============================================================================
cflat::cflat() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	argv[1] is the cflat mode.
	argv[2] is the name of the input object file.
	Q: any need to parse command line options? later...
 */
int
cflat::main(const int argc, char* argv[], const global_options&) {
	cflat_options cf;
	// cerr << "argc = " << argc << endl;
	if (argc < 3) {
		// cerr << "Insufficient arguments." << endl;
		usage();
		return 1;
	}
	// cflat mode
	const char* const mode = argv[1];
#if HFLAT_USE_OPTPARSE
	typedef	opt_map_type::const_iterator	options_modifier_map_iterator;
	const opt_map_type&
		options_modifier_map(options_map_wrapper.options_map);
#endif
	const options_modifier_map_iterator
		mi(options_modifier_map.find(mode));
	if (mi == options_modifier_map.end()
#if !HFLAT_USE_OPTPARSE
			|| !mi->second
#endif
			) {
		cerr << "Invalid mode: " << mode << endl;
		usage();
		return 1;
	} else {
#if HFLAT_USE_OPTPARSE
		(mi->second.func)(util::option_value(), cf);
#else
		(mi->second)(cf);
#endif
	}

	// additional mode modifiers
	// shift arguments by one to pass over mode
	if (parse_command_options(argc-1, argv+1, cf)) {
		cerr << "Error in command invocation." << endl;
		usage();
		return 1;
	}
	// cerr << "optind = " << optind << endl;
	if (optind+1 >= argc || optind+2 < argc) {
		cerr << "Exactly one non-option argument <objfile> required."
			<< endl;
		usage();
		return 1;
	}
	const char* const ofn = argv[optind+1];
	static const char alloc_errstr[] = 
		"ERROR in allocating global state.  Aborting.";
	count_ptr<module> the_module;
if (cf.comp_opt.compile_input) {
	the_module = parse_and_check(ofn, cf.comp_opt);
} else {
	if (!check_object_loadable(ofn).good)
		return 1;
	the_module = load_module(ofn);
	// load_module_debug(ofn);
}
	if (!the_module)
		return 1;
	// normally, this would only be called if using the original top-level
	// workaround: (for missing global param bug)
	// just create the entire object hierarchy first
	// that way global params area already processed
	if (!the_module->allocate_unique().good) {
		cerr << alloc_errstr << endl;
		return 1;
	}
	count_ptr<module> top_module;	// use this module to cflat
if (cf.use_referenced_type_instead_of_top_level) {
	const count_ptr<const process_type_reference>
		rpt(parse_and_create_complete_process_type(
			cf.named_process_type.c_str(), *the_module));
	if (!rpt) {
		return 1;		// already have error message
	}
	top_module = count_ptr<module>(new module("<auxiliary>"));
	NEVER_NULL(top_module);
	if (!top_module->allocate_unique_process_type(*rpt, *the_module).good) {
		cerr << alloc_errstr << endl;
		return 1;
	}
} else {
	top_module = the_module;
}	// end if use_referenced_type_instead_of_top_level
	// based on mode, set the options to pass into the module.
	const count_ptr<const module> m(top_module);	// const-ify
	if (!module::cflat(m->get_footprint(), cout, cf).good) {
		cerr << "Error during cflat." << endl;
		return 1;
	}
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
cflat::parse_command_options(const int argc, char* argv[], options& cf) {
	// using simple template function
	return master_options(argc, argv, cf);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Parses -f <mode> options.
 */
void
cflat::getopt_f_options(options& opt, const char* optarg) {
#if HFLAT_USE_OPTPARSE
	typedef	opt_map_type::const_iterator			const_iterator;
	const opt_map_type&
		options_modifier_map(options_map_wrapper.options_map);
	const util::option_value ov(util::optparse(optarg));
	const const_iterator mi(options_modifier_map.find(ov.key));
#else
	typedef options_modifier_map_type::mapped_type		mapped_type;
	typedef options_modifier_map_type::const_iterator	const_iterator;
	const const_iterator mi(options_modifier_map.find(optarg));
#endif
	if (mi == options_modifier_map.end()
#if !HFLAT_USE_OPTPARSE
			|| !mi->second
#endif
			) {
		// cerr << "Invalid mode: " << optarg << endl;
		string err("Invalid -f option: ");
#if HFLAT_USE_OPTPARSE
		err += ov.key;
#else
		err += optarg;
#endif
		throw util::getopt_exception(1, err);
	} else {
#if HFLAT_USE_OPTPARSE
		(mi->second.func)(ov, opt);	// check return value?
#else
		const mapped_type& om(mi->second);
		om(opt);	// process option
#endif
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sets cflat mode to use single anonymous instance of type, 
	ignoring top-level instance hierarchy.  
 */
void
cflat::getopt_cflat_type_only(options& opt, const char* optarg) {
if (opt.use_referenced_type_instead_of_top_level) {
	throw util::getopt_exception(1, "Cannot specify more than one type.");
} else {
	opt.use_referenced_type_instead_of_top_level = true;
	opt.named_process_type = optarg;	// strcpy
	// cerr << "optarg = " << opt.named_process_type << endl;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-c.texi
@defopt -c
Indicate that input file is source, as opposed to an object file,
and needs to be compiled.
@end defopt
@end texinfo
***/
static
void
__cflat_getopt_c(cflat_options& opt) {
	parse_create_flag('c', opt.comp_opt);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-C-upper.texi
@defopt -C opts
When compiling input source, forward options @var{opts} to the compiler driver.
@end defopt
@end texinfo
***/
static
void
__cflat_getopt_compile_flags(cflat_options& opt, const char* optarg) {
	parse_create_flag('C', opt.comp_opt);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-h.texi
@defopt -h
Print command-line help and exit.
@end defopt
@end texinfo
***/
static
void
__cflat_help(cflat_options& opt) {
	cflat::usage();
	exit(0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-v.texi
@defopt -v
Print version and build information and exit.
@end defopt
@end texinfo
***/
static
void
__cflat_version(cflat_options& opt) {
	config::dump_all(cout);
	exit(0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
cflat::initialize_master_options_map(void) {
	master_options.add_option('c', &__cflat_getopt_c);
	master_options.add_option('C', &__cflat_getopt_compile_flags);
	master_options.add_option('h', &__cflat_help);
	master_options.add_option('f', &getopt_f_options);
	master_options.add_option('t', &getopt_cflat_type_only);
//	master_options.add_option('p', &getopt_cflat_type_only);
//	*someone* else uses '-p' instead...
	master_options.add_option('v', &__cflat_version);
	return 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: automate help.  
 */
void
cflat::usage(void) {
	cerr << "usage: " << name << " <mode> [options] <hackt-obj-infile>"
		<< endl;
	cerr << "options: " << endl;
	cerr << "\t-c : input file is a source, to be compiled\n";
	cerr << "\t-C <opts> : flags to forward to compiler\n";
	cerr << "\t-h : print this command-line help and exit\n";
	cerr << "\t-t \"type\" : perform flattening on an instance of the named type,\n"
		"\t\tignoring top-level instances (quotes recommended)." << endl;
	cerr << "\t-v : print version and build information and exit\n";
	cerr << "\t-f <mode> : applies mode-preset or individual flag modifier"
		" (repeatable)" << endl;
	// list modes
#if HFLAT_USE_OPTPARSE
	const opt_map_type&
		options_modifier_map(options_map_wrapper.options_map);
#endif
	const size_t modes = options_modifier_map.size();
if (modes) {
	cerr << "Modes and modifier-flags (" << modes << " total):" << endl;
#if HFLAT_USE_OPTPARSE
	options_map_wrapper.help(cerr, false, false);
#else
	dump_options_briefs(cerr);
#endif
}
	// additional options summary
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// cflat::print_alias relocated to "Object/inst/alias_printer.cc"

//=============================================================================
}	// end namespace HAC

#ifdef	WITH_MAIN
/**
	Assumes no global hackt options.  
 */
int
main(const int argc, char* argv[]) {
	const HAC::global_options g;
	return HAC::cflat::main(argc, argv, g);
}
#endif	// WITH_MAIN

DEFAULT_STATIC_TRACE_END

