/**
	\file "guile/scm_reference.cc"
	$Id: scm_reference.cc,v 1.1.2.5 2007/04/06 03:52:37 fang Exp $
	TODO: consider replacing or supplementing print functions 
		with to-string functions, in case we want to process 
		the strings.
 */

#define	ENABLE_STACKTRACE			0

#include "Object/module.h"
#include "guile/scm_reference.h"
#include <iostream>
#include <sstream>
#include <memory>			// for auto_ptr
#include "util/guile_STL.h"
#include "Object/traits/instance_traits.h"
#include "Object/ref/reference_set.h"
#include "Object/entry_collection.h"
#include "parser/instref.h"
#include "guile/hackt-documentation.h"
#include "guile/libhackt-wrap.h"
#include "util/for_all.h"
#include "util/caller.h"

//=============================================================================
// smob wrapped structures

namespace HAC {
namespace guile_wrap {
using HAC::entity::meta_reference_union;
using std::ostringstream;
using entity::module;
using entity::class_traits;
using entity::bool_tag;
using entity::int_tag;
using entity::enum_tag;
using entity::channel_tag;
using entity::process_tag;
using entity::global_references_set;
using entity::entry_collection;
using util::guile::make_scm;

/**
	Tag identifier initialized by raw_reference_smob_init().
	This tag corresponds to entity::meta_reference_union* (naked pointer).  
	This will be set by the first call to raw_reference_smob_init().
 */
static
scm_t_bits __raw_reference_tag;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Public read-only alias, valid after global static initialization.
 */
const
scm_t_bits& raw_reference_tag(__raw_reference_tag);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Extracts pointer to raw-reference from smob.  
 */
const scm_reference_union*
scm_smob_to_raw_reference_ptr(const SCM& sref) {
	scm_assert_smob_type(raw_reference_tag, sref);
	return reinterpret_cast<const meta_reference_union*>
			(SCM_SMOB_DATA(sref));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Nothing to recursively mark.  
 */
static
SCM
mark_raw_reference(SCM obj) {
	return SCM_UNSPECIFIED;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Not bothering to use the scm_gc_malloc/free.  
	\return 0 always.
 */
static
size_t
free_raw_reference(SCM obj) {
	// pointer must be heap-allocated
//	std::cerr << "freeing raw-reference." << std::endl;
	const meta_reference_union* ptr = scm_smob_to_raw_reference_ptr(obj);
	if (ptr) {
		delete ptr;
		ptr = NULL;	// and STAY dead!
	}
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return non-zero to indicate success.
 */
static
int
print_raw_reference(SCM obj, SCM port, scm_print_state* p) {
	scm_puts("#<raw-reference ", port);
	const meta_reference_union* ptr = scm_smob_to_raw_reference_ptr(obj);
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
static
SCM
equalp_raw_reference(SCM o1, SCM o2) {
}
#endif

//=============================================================================
/**
	Registers our own raw-reference type as a smob to guile.  
	Direct use of this type should be avoided where possible.  
	This registers the type exactly once.
 */
void
raw_reference_smob_init(void) {
//	std::cout << raw_reference_tag << std::endl;	// is 0
if (!raw_reference_tag) {	// first time reads 0-initialized
	__raw_reference_tag =
		scm_make_smob_type("raw-reference",
			sizeof(meta_reference_union));
#if 0
	// experiment: what happens?  VERY VERY BAD
	std::cout << raw_reference_tag << std::endl;
	raw_reference_tag = scm_make_smob_type("raw-reference",
		sizeof(HAC::entity::meta_reference_union));
	std::cout << raw_reference_tag << std::endl;
#endif
//	scm_set_smob_mark(raw_reference_tag, mark_raw_reference);
	scm_set_smob_free(raw_reference_tag, free_raw_reference);
	scm_set_smob_print(raw_reference_tag, print_raw_reference);
//	scm_set_smob_equalp(raw_reference_tag, equalp_raw_reference);
}
}	// end raw_reference_smob_init

//=============================================================================
/**
	Aggregated during static initialization, called by
	load_raw_reference_functions().
 */
static
std::vector<scm_init_func_type>		local_registry;

#define HAC_GUILE_DEFINE(FNAME, PRIMNAME, REQ, OPT, VAR, ARGLIST, DOCSTRING) \
HAC_GUILE_DEFINE_PUBLIC(FNAME, PRIMNAME, REQ, OPT,			\
	VAR, ARGLIST, local_registry, DOCSTRING)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor for generating pair from indices.  
	TODO: attribute visibility hidden
 */
struct type_bound_global_reference_maker {
	const size_t				type;

	explicit
	type_bound_global_reference_maker(const size_t t) : type(t) {
		INVARIANT(type < HAC::entity::META_TYPES_ALL);
	}

	SCM
	operator () (const size_t i) const {
		return scm_cons(
			scm_type_symbols[type],
			make_scm(i));
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
	\return a string with type-size info.
	TODO: return as SCM objects instead of strings.  
 */
#define	FUNC_NAME "reference-type->string"
HAC_GUILE_DEFINE(wrap_reference_type_to_string, FUNC_NAME, 1, 0, 0, (SCM sref),
"Extracts the type and size information of a (smob) raw-reference @var{sref} "
"as a string, returned.") {
	scm_assert_smob_type(raw_reference_tag, sref);
	const meta_reference_union* ptr = scm_smob_to_raw_reference_ptr(sref);
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
	const meta_reference_union* ptr = scm_smob_to_raw_reference_ptr(sref);
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

#undef	HAC_GUILE_DEFINE
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Load functions from this unit into the caller's current module.
 */
void
load_raw_reference_functions(void) {
	INVARIANT(raw_reference_tag);
	util::for_all(local_registry, util::caller());
}

//=============================================================================
}	// end namespace guile_wrap
}	// end namespace HAC

