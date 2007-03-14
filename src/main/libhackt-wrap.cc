/**
	\file "main/libhackt-wrap.cc"
	$Id: libhackt-wrap.cc,v 1.3 2007/03/14 04:06:23 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include "main/libhackt-wrap.h"
#include "util/libguile.h"
#include <iostream>
#include "Object/module.h"
#include "Object/global_entry.h"
#include "Object/global_channel_entry.h"
#include "Object/traits/instance_traits.h"
#include "parser/instref.h"
#include "util/guile_gh.h"
#include "util/guile_STL.h"
#include "util/tokenize.h"
#include "util/memory/excl_malloc_ptr.h"
#include "util/stacktrace.h"

//=============================================================================
// convention: all function names shall begin with 'wrap_'

#if 0
namespace HAC {
namespace guile_wrap {
static
SCM
wrap_objdump(void);
}	// end namespace guile
namespace entity {

}	// end namespace entity
//=============================================================================
}	// end namespace HAC
#endif

//=============================================================================
// implementations

namespace HAC {
namespace guile_wrap {
#include "util/using_ostream.h"
using entity::module;
using entity::state_manager;
using entity::footprint;
using util::memory::excl_malloc_ptr;
using entity::global_indexed_reference;
using entity::meta_type_map;
using entity::class_traits;
using util::string_list;
using entity::bool_tag;
using entity::int_tag;
using entity::enum_tag;
using entity::channel_tag;
using entity::process_tag;
using util::guile::make_scm;
using util::guile::extract_scm;
#ifndef	HAVE_SCM_IS_PAIR
using util::guile::scm_is_pair;
#endif
#ifndef	HAVE_SCM_IS_STRING
using util::guile::scm_is_string;
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// static global initialization

excl_ptr<module>	obj_module(NULL);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapped call to 'objdump'.
 */
static
SCM
wrap_objdump(void) {
#define	FUNC_NAME "objdump"
	NEVER_NULL(obj_module);
	obj_module->dump(cout);
	return SCM_UNSPECIFIED;
#undef	FUNC_NAME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapped call to parse_global_reference.
	\return pair (cons type index)
 */
static
SCM
wrap_parse_global_reference(SCM s_str) {
#define	FUNC_NAME "parse-reference"
	STACKTRACE_VERBOSE;
	if (scm_is_string(s_str)) {
		// auto-free
#if 0
		typedef	excl_malloc_ptr<char>::type		str_ptr_type;
		const str_ptr_type buf(SCM_STRING_CHARS(s_str));
#else
		const char* peek = scm_to_locale_string(s_str);	// 1.8
#endif
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
		// PARANOIA: in case C unwinding fails to clean up stack
			// buf.~str_ptr_type();
			scm_misc_error(FUNC_NAME, 
				"Error: invalid scalar instance reference.",
				SCM_EOL);
			// will unwinding clean up the stack C++ style?
			return SCM_BOOL_F;
		}
	} else {
		cerr << "Error: expect string argument." << endl;
		scm_wrong_type_arg(FUNC_NAME, 1, s_str);
		return SCM_BOOL_F;
	}
#undef	FUNC_NAME
}	// end wrap_parse_global_reference

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Collect all aliases of a name.  
	Consider alternatives: gh_scm2ulong vs. scm_num2ulong.  
	\return list of strings
 */
static
SCM
wrap_lookup_reference_aliases(SCM s_pair) {
#define	FUNC_NAME "lookup-reference-aliases"
	STACKTRACE_VERBOSE;
if (scm_is_pair(s_pair)) {
	size_t index;
	extract_scm(SCM_CDR(s_pair), index);	// already error-handled
	string_list aliases;
	size_t type;
	extract_scm(SCM_CAR(s_pair), type);	// already error-handled
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
		return SCM_BOOL_F;
	}	// end switch
	// construct scm_list from std::list
	return make_scm(aliases);
} else {
	cerr << "Error: expecting pair(type, index) argument." << endl;
	scm_wrong_type_arg(FUNC_NAME, 1, s_pair);
	return SCM_BOOL_F;
}
#undef	FUNC_NAME
}	// wrap_lookup_reference_aliases

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prett-prints a type-index pair using the meta-type tag name.
	We may have to use a guile print-port instead of cout/cerr.
 */
static
SCM
wrap_print_reference_index(SCM s_pair) {
#define	FUNC_NAME	"print-reference-index"
if (scm_is_pair(s_pair)) {
	size_t index;
	extract_scm(SCM_CDR(s_pair), index);	// already error-handled
	size_t type;
	extract_scm(SCM_CAR(s_pair), type);	// already error-handled
	switch (type) {
#define	CASE_TYPE(Tag)							\
	case class_traits<Tag>::type_tag_enum_value:			\
		cout << class_traits<Tag>::tag_name;			\
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
	cout << '[' << index << ']' << endl;
} else {
	cerr << "Error: expecting pair(type, index) argument." << endl;
	scm_wrong_type_arg(FUNC_NAME, 1, s_pair);
}
	return SCM_UNSPECIFIED;
#undef	FUNC_NAME
}	// end wrap_print_reference_index

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
SCM
wrap_print_canonical_reference(SCM s_pair) {
#define	FUNC_NAME	"print-canonical-reference"
if (scm_is_pair(s_pair)) {
	size_t index;
	extract_scm(SCM_CDR(s_pair), index);	// already error-handled
	size_t type;
	extract_scm(SCM_CAR(s_pair), type);	// already error-handled
	const module& mod(*obj_module);
	const state_manager& sm(mod.get_state_manager());
	const footprint& topfp(mod.get_footprint());
	if (!index) {
		scm_misc_error(FUNC_NAME, "Error: invalid index.", SCM_EOL);
		// TODO: check upper bound of index too
		return SCM_UNSPECIFIED;
	}
	switch (type) {
#define	CASE_TYPE(Tag)							\
	case class_traits<Tag>::type_tag_enum_value:			\
		sm.get_pool<Tag>()[index]				\
			.dump_canonical_name(cout, topfp, sm);		\
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
} else {
	cerr << "Error: expecting pair(type, index) argument." << endl;
	scm_wrong_type_arg(FUNC_NAME, 1, s_pair);
}
	return SCM_UNSPECIFIED;
#undef	FUNC_NAME
}	// end wrap_print_canonical_reference

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace guile_wrap
}	// end namespace HAC

//=============================================================================

BEGIN_C_DECLS
using namespace HAC::guile_wrap;

/**
	Register wrapped functions.  
	\pre guile is already booted.
 */
void
libhackt_guile_init(void) {
	typedef	SCM (*scm_gsubr_type)();
	// ugh, function pointer reinterpret_cast...
	scm_c_define_gsubr("objdump", 0, 0, 0, wrap_objdump);
#if 0
	scm_c_define_gsubr("hackt-parse-to-aliases", 0, 0, 0,
		(scm_gsubr_type) wrap_parse_name_to_aliases);
#endif
	scm_c_define_gsubr("parse-reference", 1, 0, 0,
		(scm_gsubr_type) wrap_parse_global_reference);
	scm_c_define_gsubr("print-reference-index", 1, 0, 0,
		(scm_gsubr_type) wrap_print_reference_index);
	scm_c_define_gsubr("print-canonical-reference", 1, 0, 0,
		(scm_gsubr_type) wrap_print_canonical_reference);
	scm_c_define_gsubr("lookup-reference-aliases", 1, 0, 0,
		(scm_gsubr_type) wrap_lookup_reference_aliases);
}	// end libhackt_guile_init

END_C_DECLS


