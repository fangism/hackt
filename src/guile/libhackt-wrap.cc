/**
	\file "guile/libhackt-wrap.cc"
	$Id: libhackt-wrap.cc,v 1.3.2.5 2007/03/27 22:00:39 fang Exp $
	TODO: consider replacing or supplementing print functions 
		with to-string functions, in case we want to process 
		the strings.
	TODO: some of these functions are better in guile/scm_reference.cc.
	Prior history, file was "main/libhackt-wrap.cc"
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include <sstream>
#include <memory>
#include "Object/module.h"
#include "Object/global_entry.h"
#include "Object/global_channel_entry.h"
#include "Object/ref/meta_reference_union.h"
#include "Object/ref/reference_set.h"
#include "Object/traits/instance_traits.h"
#include "Object/entry_collection.h"
#include "parser/instref.h"
#include "guile/libhackt-wrap.h"
#include "guile/hackt-documentation.h"
#include "guile/scm_reference.h"
#include "util/libguile.h"
#include "util/guile_STL.h"
#include "util/tokenize.h"
#include "util/memory/excl_malloc_ptr.h"
#include "util/stacktrace.h"
#include "util/for_all.h"
#include "util/caller.h"

//=============================================================================
// smob wrapped structures

//=============================================================================
// convention: all function names shall begin with 'wrap_'

//=============================================================================
// implementations

namespace HAC {
namespace guile_wrap {
#include "util/using_ostream.h"
using std::transform;
using std::ostringstream;
using entity::module;
using entity::state_manager;
using entity::footprint;
using util::memory::excl_malloc_ptr;
using entity::global_indexed_reference;
using entity::global_references_set;
using entity::entry_collection;
using entity::meta_type_map;
using entity::class_traits;
using util::string_list;
using entity::class_traits;
using entity::bool_tag;
using entity::int_tag;
using entity::enum_tag;
using entity::channel_tag;
using entity::process_tag;
using entity::meta_reference_union;
using util::guile::make_scm;
using util::guile::extract_scm;
#ifndef	HAVE_SCM_IS_PAIR
using util::guile::scm_irpair;
#endif
#ifndef	HAVE_SCM_IS_STRING
using util::guile::scm_is_string;
#endif
using util::guile::scm_assert_pair;
using util::guile::scm_assert_string;

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
	Functor for generating pair from indices.  
	TODO: attribute visibility hidden
 */
struct type_bound_global_reference_maker {
	const size_t				type;

	explicit
	type_bound_global_reference_maker(const size_t t) : type(t) { }

	SCM
	operator () (const size_t i) const {
		return scm_cons(make_scm(type), make_scm(i));
	}
};	// end struct type_bound_global_reference_maker

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Convert a set of references into list of pairs suitable for scheme.  
 */
static
SCM
global_references_set_export_scm_refs(const global_references_set& s) {
	util::guile::scm_list_insert_iterator ret(SCM_LIST0);
#define	TRANSFORM(Tag)							\
{									\
	const size_t tag_num = class_traits<Tag>::type_tag_enum_value;	\
	const global_references_set::ref_bin_type& b(s.ref_bin[tag_num]); \
	const type_bound_global_reference_maker t(tag_num);		\
	ret = transform(b.begin(), b.end(), ret, t);			\
}
	TRANSFORM(bool_tag)
	TRANSFORM(int_tag)
	TRANSFORM(enum_tag)
	TRANSFORM(channel_tag)
	TRANSFORM(process_tag)
#undef	TRANSFORM
	return ret.list;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapped call to 'objdump'.
	NOTE: this uses cout instead of an ostringstream.  
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
	Parses and returns a raw-reference (unchecked).  
 */
#define	FUNC_NAME "parse-raw-reference"
HAC_GUILE_DEFINE(wrap_parse_raw_reference, FUNC_NAME, 1, 0, 0, (SCM sref),
"Parses a reference string @var{sref} as it might appear in HAC source, and "
"returns an internal representation (smob:raw-reference) of the reference.") {
	const char* peek = scm_to_locale_string(sref);	// 1.8
	// alternately string_to_locale_stringbuf
	const module& mod(*obj_module);
	// alert: heap-allocating though naked pointer, copy-constructing
	std::auto_ptr<meta_reference_union>
		ref(new meta_reference_union(
			parser::parse_and_check_reference(peek, mod)));
	SCM ret_smob;
	SCM_NEWSMOB(ret_smob, raw_reference_tag, ref.release());
	return ret_smob;
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
	scm_assert_string(sref, FUNC_NAME, 1);
	const char* peek = scm_to_locale_string(sref);	// 1.8
	// alternately string_to_locale_stringbuf
	const module& mod(*obj_module);
	const global_indexed_reference
		ref(parser::parse_global_reference(peek, mod));
	if (ref.first && ref.second) {
		// would like to translate first (type)
		// into a scm quoted tag, like 'bool-tag
		return scm_cons(make_scm(ref.first), 
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
	extract_scm(SCM_CDR(rpair), index);	// already error-handled
	string_list aliases;
	size_t type;
	extract_scm(SCM_CAR(rpair), type);	// already error-handled
	const module& mod(*obj_module);
	switch (type) {
#define	CASE_TYPE(Tag)							\
	case class_traits<Tag>::type_tag_enum_value:			\
		mod.match_aliases<Tag>(aliases, index);			\
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
		return SCM_UNSPECIFIED;
	}	// end switch
	// construct scm_list from std::list
	return make_scm(aliases);
}	// wrap_lookup_reference_aliases
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return a string with type-size info.
	TODO: return as SCM objects instead of strings.  
 */
#define	FUNC_NAME "reference-type->string"
HAC_GUILE_DEFINE(wrap_reference_type_to_string, FUNC_NAME, 1, 0, 0, (SCM sref),
"Extracts the type and size information of a (smob) raw-reference @var{sref} "
"as a string, returned.") {
	scm_assert_smob_type(raw_reference_tag, sref);
	const meta_reference_union* ptr = 
		reinterpret_cast<const meta_reference_union*>
			(SCM_SMOB_DATA(sref));
	if (ptr && ptr->inst_ref()) {
		ostringstream oss;
		ptr->inst_ref()->what(oss) << " ";
		ptr->inst_ref()->dump_type_size(oss);	// << endl;
		// scm_puts(oss.str().c_str(), scm_current_output_port());
		return make_scm(oss.str());
	} else {
		// scm_puts("(null)", scm_current_output_port());
		scm_misc_error(FUNC_NAME, "Error: raw-reference.", SCM_EOL);
		return SCM_UNSPECIFIED;
	}
	// return SCM_UNSPECIFIED;
}
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
	extract_scm(SCM_CAR(rpair), type);	// already error-handled
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
	extract_scm(SCM_CAR(rpair), type);	// already error-handled
	const module& mod(*obj_module);
	const state_manager& sm(mod.get_state_manager());
	const footprint& topfp(mod.get_footprint());
	if (!index) {
		scm_misc_error(FUNC_NAME, "Error: invalid index.", SCM_EOL);
		// TODO: check upper bound of index too
		return SCM_UNSPECIFIED;
	}
	ostringstream oss;
	switch (type) {
#define	CASE_TYPE(Tag)							\
	case class_traits<Tag>::type_tag_enum_value:			\
		sm.get_pool<Tag>()[index]				\
			.dump_canonical_name(oss, topfp, sm);		\
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param sref a raw_reference smob.  
	\return list of references (type,index)-pairs that are subinstances, 
		reachable aliases.  
 */
#define	FUNC_NAME "collect-reference-subinstances"
HAC_GUILE_DEFINE(wrap_collect_reference_subinstances, FUNC_NAME, 1, 0, 0,
	(SCM sref),
"Returns a list of unique (type-index) references that are reachable "
"subinstances of @var{sref}, usually a process.") {
	scm_assert_smob_type(raw_reference_tag, sref);
	const meta_reference_union* ptr = 
		reinterpret_cast<const meta_reference_union*>
			(SCM_SMOB_DATA(sref));
	if (ptr && ptr->inst_ref()) {
		entry_collection e;
		ptr->inst_ref()->collect_subentries(*obj_module, e);
		global_references_set s;
		s.import_entry_collection(e);
		return global_references_set_export_scm_refs(s);
	} else {
		return SCM_EOL;
	}
}	// wrap_collect_reference_subinstances
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace guile_wrap
}	// end namespace HAC

#undef	HAC_GUILE_DEFINE
//=============================================================================

BEGIN_C_DECLS
using namespace HAC::guile_wrap;
using util::guile::scm_gsubr_type;
using util::guile::scm_c_define_exported;
using util::guile::scm_c_define_gsubr_exported;


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
	NEVER_NULL(obj_module);
	raw_reference_smob_init();
	// TODO: raw-reference?
	// define some global constants
	scm_c_define_exported("bool-tag",
		make_scm<int>(class_traits<bool_tag>::type_tag_enum_value));
	scm_c_define_exported("int-tag",
		make_scm<int>(class_traits<int_tag>::type_tag_enum_value));
	scm_c_define_exported("enum-tag",
		make_scm<int>(class_traits<enum_tag>::type_tag_enum_value));
	scm_c_define_exported("channel-tag",
		make_scm<int>(class_traits<channel_tag>::type_tag_enum_value));
	scm_c_define_exported("process-tag",
		make_scm<int>(class_traits<process_tag>::type_tag_enum_value));

	init_documentation();
	util::for_all(hackt_primitives_registry, util::caller());
	// ugh, function pointer reinterpret_cast...
#if HAVE_ATEXIT
	const int x = atexit(release_libhackt_wrap_resources_at_exit);
	INVARIANT(!x);	// must have succeeded!
#endif
}	// end libhackt_guile_init

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
libhackt_guile_init(void) {
	__libhackt_guile_init(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
scm_init_hackt_libhackt_primitives_module(void) {
	scm_c_define_module("hackt hackt-primitives", 
		__libhackt_guile_init, NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

END_C_DECLS


