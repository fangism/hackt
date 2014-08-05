/**
	\file "guile/libhackt-wrap.cc"
	$Id: libhackt-wrap.cc,v 1.11 2010/08/24 18:08:40 fang Exp $
	TODO: consider replacing or supplementing print functions 
		with to-string functions, in case we want to process 
		the strings.
	Prior history, file was "main/libhackt-wrap.cc"
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include <sstream>
#include <set>
#include "Object/module.hh"
#include "Object/global_entry.hh"
#include "Object/global_channel_entry.hh"
#include "Object/traits/instance_traits.hh"
#include "Object/inst/instance_pool.hh"
#include "Object/inst/state_instance.hh"
#include "Object/global_entry_context.hh"
#include "Object/common/dump_flags.hh"
#include "parser/instref.hh"
#include "guile/devel_switches.hh"
#include "guile/libhackt-wrap.hh"
#include "guile/hackt-config.hh"
#include "guile/hackt-documentation.hh"
#include "guile/scm_reference.hh"
#include "main/main_funcs.hh"		// for load_module
#include "util/tokenize.hh"
#include "util/memory/excl_malloc_ptr.hh"
#include "util/guile_STL.hh"
#include "util/stacktrace.hh"
#include "util/for_all.hh"
#include "util/caller.hh"

//=============================================================================
// smob wrapped structures

//=============================================================================
// convention: all function names shall begin with 'wrap_'

//=============================================================================
// implementations

namespace HAC {
namespace guile_wrap {
#include "util/using_ostream.hh"
using std::string;
using std::transform;
using std::ostringstream;
using entity::module;
using std::set;
using entity::global_entry_context;
using entity::global_offset;
using entity::footprint_frame;
using entity::footprint;
using util::memory::excl_malloc_ptr;
using entity::global_indexed_reference;
using entity::meta_type_map;
using util::string_list;
using entity::class_traits;
using entity::bool_tag;
using entity::int_tag;
using entity::enum_tag;
using entity::channel_tag;
using entity::process_tag;
using entity::dump_flags;
using util::guile::make_scm;
using util::guile::extract_scm;
#ifndef	HAVE_SCM_IS_PAIR
using util::guile::scm_is_pair;
#endif
#ifndef	HAVE_SCM_IS_STRING
using util::guile::scm_is_string;
#endif
using util::guile::scm_assert_pair;
using util::guile::scm_assert_string;
USING_SCM_FROM_LOCALE_SYMBOL

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// static global initialization

count_ptr<module>	obj_module(NULL);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
std::vector<scm_init_func_type>		hackt_primitives_registry;

/**
	Don't use line-continuation right before ARGLIST, triggers cpp bug?
 */
#define	HAC_GUILE_DEFINE(FNAME, PRIMNAME, REQ, OPT, VAR, ARGLIST, DOCSTRING) \
HAC_GUILE_DEFINE_PUBLIC(FNAME, PRIMNAME, REQ, OPT, 			\
	VAR, ARGLIST, hackt_primitives_registry, DOCSTRING)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unique SCM symbols 'bool, 'int...
	To be initialized...
	Should be publicly read-only.
 */
SCM
scm_type_symbols[HAC::entity::META_TYPES_ALL] = { NULL };


/**
	This map performs reverse lookup, using the pointer
	to a permanent symbol as the key.  
	This is populated during procedure-loading initialization, below.  
 */
scm_symbol_to_enum_map_type		scm_symbol_to_enum_map;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if HAVE_ATEXIT
/**
	Since guile isn't kind enough to return and clean up the stack upon
	exit, we employ ::atexit to assist us in cleaning up.  
	NOTE: atexit is called *after* main, but before any other
	global destructors are called.  
 */
static
void
release_libhackt_wrap_resources_at_exit(void) {
	STACKTRACE_VERBOSE;	// would REQUIRE_STATIC_STACKTRACE_INIT?
	if (obj_module) {
		obj_module = count_ptr<module>(NULL);
		// deallocate please!
		// there shouldn't be any other references to it
		// should we assert?
	}
}
#endif	// HAVE_ATEXIT

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapped call to 'load_module'.
	\return nothing
 */
#define	FUNC_NAME "load-object"
HAC_GUILE_DEFINE(wrap_load_object, FUNC_NAME, 1, 0, 0, (SCM objname),
"Same result as running standalone `hacobjdump', also dumped to stdout.") {
	scm_assert_string(objname, FUNC_NAME, 1);
	if (obj_module) {
		scm_misc_error(FUNC_NAME, 
		"Error: only one HAC object file may be loaded per session.",
			SCM_EOL);
	}
	const char* peek = NULL;
	extract_scm(objname, peek);
	obj_module = load_module(peek);
	if (!obj_module) {
		scm_misc_error(FUNC_NAME, 
			"Error: unable to load HAC object file.",
			SCM_EOL);
	}
	// this was needed to initialize context-cache
	if (!obj_module->allocate_unique().good) {
		scm_misc_error(FUNC_NAME, 
			"Error: creating/expanding top-level.",
			SCM_EOL);
	}
	return SCM_UNSPECIFIED;
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	FUNC_NAME "have-object?"
HAC_GUILE_DEFINE(wrap_have_object, FUNC_NAME, 0, 0, 0, (void),
"Return #t if object has been loaded, else #f.") {
	return make_scm<bool>(obj_module);
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapped call to 'objdump'.
	NOTE: this uses cout instead of an ostringstream.  
	TODO: support options to dump different sections
	\return nothing
 */
#define	FUNC_NAME "objdump"
HAC_GUILE_DEFINE(wrap_objdump, FUNC_NAME, 0, 0, 0, (void),
"Same result as running standalone `hacobjdump', also dumped to stdout.") {
	NEVER_NULL(obj_module);
	obj_module->dump(cout);
	return SCM_UNSPECIFIED;
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapped call to parse_global_reference.
	\return pair (cons type index)
 */
#define	FUNC_NAME "parse-reference"
HAC_GUILE_DEFINE(wrap_parse_global_reference, FUNC_NAME, 1, 0, 0, 
	(SCM sref), 
"Parses a reference string @var{sref} as it might appear in HAC source, "
"type-checks it, and returns a (type . index) pair.") {
	STACKTRACE_VERBOSE;
	scm_assert_string(sref, FUNC_NAME, 1);	// redundant
	const char* peek = NULL;
	extract_scm(sref, peek);
	// alternately string_to_locale_stringbuf
	const module& mod(*obj_module);
	const global_indexed_reference
		ref(parser::parse_global_reference(peek, mod.get_footprint()));
	if (ref.first && ref.second) {
		// would like to translate first (type)
		// into a scm quoted tag, like 'bool-tag
		INVARIANT(ref.first < HAC::entity::META_TYPES_ALL);
		INVARIANT(scm_type_symbols[ref.first]);
		return scm_cons(
			scm_type_symbols[ref.first],
			make_scm(ref.second));
	} else {
	// PARANOIA: in case C unwinding fails to clean up stack?
		scm_misc_error(FUNC_NAME, 
			"Error: invalid scalar instance reference.",
			SCM_EOL);
		// will unwinding clean up the stack C++ style?
		return SCM_UNSPECIFIED;
	}
}	// end wrap_parse_global_reference
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Collect all aliases of a name.  
	\return list of strings for all equivalent aliases.  
 */
#define	FUNC_NAME "lookup-reference-aliases"
HAC_GUILE_DEFINE(wrap_lookup_reference_aliases, FUNC_NAME, 1, 0, 0, (SCM rpair),
"Returns a list of all aliases (strings) of the instance named @var{rpair}.") {
	STACKTRACE_VERBOSE;
	scm_assert_pair(rpair, FUNC_NAME, 1);	// redundant?
	size_t index;
#define	EXTRACT_TYPE_ENUM(rpair, type)					\
	{								\
		string s;						\
		extract_scm(scm_symbol_to_string(SCM_CAR(rpair)), s);	\
		const scm_symbol_to_enum_map_type::const_iterator	\
			f(scm_symbol_to_enum_map.find(s));		\
		INVARIANT(f != scm_symbol_to_enum_map.end());		\
		type = f->second;					\
	}
	extract_scm(SCM_CDR(rpair), index);	// already error-handled
	set<string> aliases;
	size_t type;
	EXTRACT_TYPE_ENUM(rpair, type);		// already error-handled
	const module& mod(*obj_module);
	const footprint& topfp(mod.get_footprint());
	switch (type) {
#define	CASE_TYPE(Tag)							\
	case class_traits<Tag>::type_tag_enum_value: {			\
		topfp.collect_aliases_recursive<Tag>(index-1, 		\
			dump_flags::no_owners, aliases);		\
		break;							\
	}
	CASE_TYPE(bool_tag)
	CASE_TYPE(int_tag)
	CASE_TYPE(enum_tag)
	CASE_TYPE(channel_tag)
	CASE_TYPE(process_tag)
#undef	CASE_TYPE
	default:
		scm_misc_error(FUNC_NAME, 
			"Error: invalid meta-type enum.", SCM_EOL);
		return SCM_UNSPECIFIED;
	}	// end switch
	// construct scm_list from std::list
	return make_scm(aliases);
}	// wrap_lookup_reference_aliases
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prett-prints a type-index pair using the meta-type tag name.
	We may have to use a guile print-port instead of cout/cerr.
	\return SCM string.
 */
#define	FUNC_NAME	"reference-index->string"
HAC_GUILE_DEFINE(wrap_reference_index_to_string, FUNC_NAME, 1, 0, 0, 
	(SCM rpair),
"Pretty-prints a type-index pair @var{rpair} using the meta-type tag name, and "
"the globally allocated index assigned to the reference, returns a string.") {
	scm_assert_pair(rpair, FUNC_NAME, 1);	// redundant
	size_t index;
	extract_scm(SCM_CDR(rpair), index);	// already error-handled
	size_t type;
	EXTRACT_TYPE_ENUM(rpair, type);		// already error-handled
	ostringstream oss;
	switch (type) {
#define	CASE_TYPE(Tag)							\
	case class_traits<Tag>::type_tag_enum_value:			\
		oss << class_traits<Tag>::tag_name;			\
		break;
	CASE_TYPE(bool_tag)
	CASE_TYPE(int_tag)
	CASE_TYPE(enum_tag)
	CASE_TYPE(channel_tag)
	CASE_TYPE(process_tag)
#undef	CASE_TYPE
	default:
		scm_misc_error(FUNC_NAME, 
			"Error: invalid meta-type enum.", SCM_EOL);
	}	// end switch
	oss << '[' << index << ']';	// << endl;
	return make_scm(oss.str());
}	// end wrap_reference_index_to_string
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param rpair a [type,index] pair (global_indexed_reference)
	\return string representation of named reference.
 */
#define	FUNC_NAME	"canonical-reference->string"
HAC_GUILE_DEFINE(wrap_canonical_reference_to_string, FUNC_NAME, 1, 0, 0, 
	(SCM rpair),
"Returns the canonical name of the instance referenced by @var{rpair}, "
"a type-index pair.") {
	scm_assert_pair(rpair, FUNC_NAME, 1);	// redundant
	size_t index;
	extract_scm(SCM_CDR(rpair), index);	// already error-handled
	size_t type;
	EXTRACT_TYPE_ENUM(rpair, type);		// already error-handled
	const module& mod(*obj_module);
	const footprint& topfp(mod.get_footprint());
	if (!index) {
		scm_misc_error(FUNC_NAME, "Error: invalid index.", SCM_EOL);
		// TODO: check upper bound of index too
		return SCM_UNSPECIFIED;
	}
	const dump_flags& df(dump_flags::no_owners);
	ostringstream oss;
	switch (type) {
#define	CASE_TYPE(Tag)							\
	case class_traits<Tag>::type_tag_enum_value:			\
		topfp.dump_canonical_name<Tag>(oss, index -1, df);	\
		break;
	CASE_TYPE(bool_tag)
	CASE_TYPE(int_tag)
	CASE_TYPE(enum_tag)
	CASE_TYPE(channel_tag)
	CASE_TYPE(process_tag)
#undef	CASE_TYPE
	default:
		scm_misc_error(FUNC_NAME, 
			"Error: invalid meta-type enum.", SCM_EOL);
	}	// end switch
	return make_scm(oss.str());
}	// end wrap_canonical_reference_to_string
#undef	FUNC_NAME

#undef	EXTRACT_TYPE_ENUM

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
static
size_t
__get_pool_size(void) {
	NEVER_NULL(obj_module);
	return AS_A(const module&, *obj_module).get_footprint()
		.template get_instance_pool<Tag>().total_entries();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: also for other meta types... channel, bool, int...
	\param ind global process ID is 1-based index, but
	0 is a valid process id, referring to the top-level process.
	Other meta-types do NOT have 0 as a valid index, 
	prcesses are the exception.
 */
#define	FUNC_NAME	"valid-process-id?"
HAC_GUILE_DEFINE(wrap_valid_process_id_p, FUNC_NAME, 1, 0, 0, (SCM ind), 
	"Return true if process-index is valid.  [Is 0 valid?]") {
	size_t index;
	extract_scm(ind, index);
	return make_scm(index <= __get_pool_size<process_tag>());
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace guile_wrap
}	// end namespace HAC

#undef	HAC_GUILE_DEFINE
//=============================================================================

BEGIN_C_DECLS
using namespace HAC::guile_wrap;
// using util::guile::scm_gsubr_type;
using util::guile::scm_c_define_exported;
// using util::guile::scm_c_define_gsubr_exported;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Register wrapped functions.  
	Safe to call this multiple times.  
	\pre guile is already booted, and obj_module pointer is set.
	\post obj_module pointer remains unchanged.
 */
static
void
__libhackt_guile_init(void* unused) {
	raw_reference_smob_init();
	// TODO: raw-reference?
	// define some global constants
	// like scm_gc_protect without unprotect
#define	REGISTER_PERMANENT_SYMBOL(Tag)					\
{									\
	const SCM sym = scm_permanent_object(				\
		scm_from_locale_symbol(class_traits<Tag>::tag_name));	\
	scm_type_symbols[class_traits<Tag>::type_tag_enum_value] = sym;	\
	scm_symbol_to_enum_map[class_traits<Tag>::tag_name] = 		\
		class_traits<Tag>::type_tag_enum_value;			\
}
	REGISTER_PERMANENT_SYMBOL(bool_tag)		// makes 'bool
	REGISTER_PERMANENT_SYMBOL(int_tag)		// makes 'int
	REGISTER_PERMANENT_SYMBOL(enum_tag)		// makes 'enum
	REGISTER_PERMANENT_SYMBOL(channel_tag)		// makes 'channel
	REGISTER_PERMANENT_SYMBOL(process_tag)		// makes 'process
#undef	REGISTER_PERMANENT_SYMBOL

	init_documentation();		// from "guile/hackt-documentation.h"
	load_raw_reference_functions();	// from "guile/scm_reference.cc"
	hackt_config_init();		// from "guile/hackt-config.h"
	util::for_all(hackt_primitives_registry, util::caller());
	// ugh, function pointer reinterpret_cast...
#if HAVE_ATEXIT
	const int x = atexit(release_libhackt_wrap_resources_at_exit);
	INVARIANT(!x);	// must have succeeded!
#endif
}	// end libhackt_guile_init

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Call this when loading as an extension after startup.
	e.g. for (load-extension ...)
 */
void
libhackt_guile_init(void) {
	__libhackt_guile_init(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Call this when booting from the executable.
	The executable (main) is responsible for loading the object file
	in advance.
 */
void
scm_init_hackt_libhackt_primitives_module(void) {
	NEVER_NULL(obj_module);
	scm_c_define_module("hackt hackt-primitives", 
		__libhackt_guile_init, NULL);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

END_C_DECLS


