/**
	\file "main/libhackt-wrap.cc"
	$Id: libhackt-wrap.cc,v 1.4 2007/03/15 06:11:05 fang Exp $
	TODO: consider replacing or supplementing print functions 
		with to-string functions, in case we want to process 
		the strings.
 */

#define	ENABLE_STACKTRACE			0

#include "main/libhackt-wrap.h"
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
#include "util/libguile.h"
#include "util/guile_STL.h"
#include "util/tokenize.h"
#include "util/memory/excl_malloc_ptr.h"
#include "util/stacktrace.h"

//=============================================================================
// smob wrapped structures

namespace HAC {
namespace guile_wrap {
using HAC::entity::meta_reference_union;
using std::ostringstream;

/**
	Tag identifier initialized by raw_reference_smob_init().
	This tag corresponds to entity::meta_reference_union* (naked pointer).  
 */
static
scm_t_bits raw_reference_tag;

/**
	Nothing to recursively mark.  
 */
static
SCM
mark_raw_reference(SCM obj) {
	return SCM_UNSPECIFIED;
}

/**
	Not bothering to use the scm_gc_malloc/free.  
	\return 0 always.
 */
static
size_t
free_raw_reference(SCM obj) {
	// pointer must be heap-allocated
//	std::cerr << "freeing raw-reference." << std::endl;
	meta_reference_union* ptr = 
		reinterpret_cast<meta_reference_union*>(SCM_SMOB_DATA(obj));
	if (ptr) {
		delete ptr;
		ptr = NULL;	// and STAY dead!
	}
	return 0;
}

/**
	\return non-zero to indicate success.
 */
static
int
print_raw_reference(SCM obj, SCM port, scm_print_state* p) {
	scm_puts("#<raw-reference ", port);
	const meta_reference_union* ptr = 
		reinterpret_cast<const meta_reference_union*>
			(SCM_SMOB_DATA(obj));
	if (ptr && ptr->inst_ref()) {
		ostringstream oss;
		ptr->inst_ref()->what(oss) << " ";
		ptr->inst_ref()->dump_type_size(oss);
		scm_puts(oss.str().c_str(), port);
	} else {
		scm_puts("null", port);
	}
	scm_puts(" >", port);
	return 1;
}

#if 0
static
SCM
equalp_raw_reference(SCM o1, SCM o2) {
}
#endif

/**
	Registers our own raw-reference type as a smob to guile.  
	Direct use of this type should be avoided where possible.  
 */
static
void
raw_reference_smob_init(void) {
	raw_reference_tag = scm_make_smob_type("raw-reference",
		sizeof(HAC::entity::meta_reference_union));
	scm_set_smob_mark(raw_reference_tag, mark_raw_reference);
	scm_set_smob_free(raw_reference_tag, free_raw_reference);
	scm_set_smob_print(raw_reference_tag, print_raw_reference);
//	scm_set_smob_equalp(raw_reference_tag, equalp_raw_reference);
}

}	// end namespace guile_wrap
}	// end namespace HAC

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
using std::transform;
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// static global initialization

excl_ptr<module>	obj_module(NULL);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor for generating pair from indices.  
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
	Parses and returns a raw-reference (unchecked).  
 */
static
SCM
wrap_parse_raw_reference(SCM s_str) {
#define	FUNC_NAME "parse-raw-reference"
	const char* peek = scm_to_locale_string(s_str);	// 1.8
	// alternately string_to_locale_stringbuf
	const module& mod(*obj_module);
	// alert: heap-allocating though naked pointer, copy-constructing
	std::auto_ptr<meta_reference_union>
		ref(new meta_reference_union(
			parser::parse_and_check_reference(peek, mod)));
	SCM ret_smob;
	SCM_NEWSMOB(ret_smob, raw_reference_tag, ref.release());
	return ret_smob;
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
	scm_assert_string(s_str, FUNC_NAME, 1);
	const char* peek = scm_to_locale_string(s_str);	// 1.8
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
#undef	FUNC_NAME
}	// end wrap_parse_global_reference

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Collect all aliases of a name.  
	\return list of strings for all equivalent aliases.  
 */
static
SCM
wrap_lookup_reference_aliases(SCM s_pair) {
#define	FUNC_NAME "lookup-reference-aliases"
	STACKTRACE_VERBOSE;
	scm_assert_pair(s_pair, FUNC_NAME, 1);
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
		return SCM_UNSPECIFIED;
	}	// end switch
	// construct scm_list from std::list
	return make_scm(aliases);
#undef	FUNC_NAME
}	// wrap_lookup_reference_aliases

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
SCM
wrap_print_reference_type(SCM s_ref) {
#define	FUNC_NAME "print-reference-type"
	scm_assert_smob_type(raw_reference_tag, s_ref);
	const meta_reference_union* ptr = 
		reinterpret_cast<const meta_reference_union*>
			(SCM_SMOB_DATA(s_ref));
	if (ptr && ptr->inst_ref()) {
		ostringstream oss;
		ptr->inst_ref()->what(oss) << " ";
		ptr->inst_ref()->dump_type_size(oss) << endl;
		scm_puts(oss.str().c_str(), scm_current_output_port());
	} else {
		scm_puts("(null)", scm_current_output_port());
	}
	return SCM_UNSPECIFIED;
#undef	FUNC_NAME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prett-prints a type-index pair using the meta-type tag name.
	We may have to use a guile print-port instead of cout/cerr.
 */
static
SCM
wrap_print_reference_index(SCM s_pair) {
#define	FUNC_NAME	"print-reference-index"
	scm_assert_pair(s_pair, FUNC_NAME, 1);
	size_t index;
	extract_scm(SCM_CDR(s_pair), index);	// already error-handled
	size_t type;
	extract_scm(SCM_CAR(s_pair), type);	// already error-handled
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
	oss << '[' << index << ']' << endl;
	scm_puts(oss.str().c_str(), scm_current_output_port());
	return SCM_UNSPECIFIED;
#undef	FUNC_NAME
}	// end wrap_print_reference_index

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param s_pair a [type,index] pair (global_indexed_reference)
 */
static
SCM
wrap_print_canonical_reference(SCM s_pair) {
#define	FUNC_NAME	"print-canonical-reference"
	scm_assert_pair(s_pair, FUNC_NAME, 1);
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
	scm_puts(oss.str().c_str(), scm_current_output_port());
	return SCM_UNSPECIFIED;
#undef	FUNC_NAME
}	// end wrap_print_canonical_reference

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param s_ref a raw_reference smob.  
	\return list of references (type,index)-pairs that are subinstances, 
		reachable aliases.  
 */
static
SCM
wrap_collect_reference_subinstances(SCM s_ref) {
#define	FUNC_NAME "collect-reference-subinstances"
	scm_assert_smob_type(raw_reference_tag, s_ref);
	const meta_reference_union* ptr = 
		reinterpret_cast<const meta_reference_union*>
			(SCM_SMOB_DATA(s_ref));
	if (ptr && ptr->inst_ref()) {
		entry_collection e;
		ptr->inst_ref()->collect_subentries(*obj_module, e);
		global_references_set s;
		s.import_entry_collection(e);
		return global_references_set_export_scm_refs(s);
	} else {
		return SCM_EOL;
	}
#undef	FUNC_NAME
}	// wrap_collect_reference_subinstances

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace guile_wrap
}	// end namespace HAC

//=============================================================================

BEGIN_C_DECLS
using namespace HAC::guile_wrap;
using util::guile::scm_gsubr_type;


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Register wrapped functions.  
	\pre guile is already booted.
 */
void
libhackt_guile_init(void) {
	raw_reference_smob_init();
	// ugh, function pointer reinterpret_cast...
	scm_c_define_gsubr("objdump", 0, 0, 0, wrap_objdump);
	scm_c_define_gsubr("parse-reference", 1, 0, 0,
		(scm_gsubr_type) wrap_parse_global_reference);
	scm_c_define_gsubr("parse-raw-reference", 1, 0, 0,
		(scm_gsubr_type) wrap_parse_raw_reference);
	scm_c_define_gsubr("print-reference-type", 1, 0, 0,
		(scm_gsubr_type) wrap_print_reference_type);
	scm_c_define_gsubr("print-reference-index", 1, 0, 0,
		(scm_gsubr_type) wrap_print_reference_index);
	scm_c_define_gsubr("print-canonical-reference", 1, 0, 0,
		(scm_gsubr_type) wrap_print_canonical_reference);
	scm_c_define_gsubr("lookup-reference-aliases", 1, 0, 0,
		(scm_gsubr_type) wrap_lookup_reference_aliases);
	scm_c_define_gsubr("collect-reference-subinstances", 1, 0, 0,
		(scm_gsubr_type) wrap_collect_reference_subinstances);
}	// end libhackt_guile_init

END_C_DECLS


