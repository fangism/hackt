/**
	\file "art_object_module.cc"
	Method definitions for module class.  
 	$Id: art_object_module.cc,v 1.16 2005/03/04 06:19:58 fang Exp $
 */

#ifndef	__ART_OBJECT_MODULE_CC__
#define	__ART_OBJECT_MODULE_CC__

// code debugging switches
#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_DESTRUCTORS		0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS		0 && ENABLE_STACKTRACE

#include <iostream>
#include "art_object_module.h"
#include "art_object_namespace.h"
#include "art_object_unroll_context.h"
#include "persistent_object_manager.tcc"
#include "art_object_type_hash.h"
#include "stacktrace.h"

// conditional defines, after including "stacktrace.h"
#if STACKTRACE_DESTRUCTORS
	#define	STACKTRACE_DTOR(x)		STACKTRACE(x)
#else
	#define	STACKTRACE_DTOR(x)
#endif

#if STACKTRACE_PERSISTENTS
	#define	STACKTRACE_PERSISTENT(x)	STACKTRACE(x)
#else
	#define	STACKTRACE_PERSISTENT(x)
#endif

namespace util {
#if HAVE_PERSISTENT_CONSTRUCT_EMPTY
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::module, MODULE_TYPE_KEY)
#else
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::module, MODULE_TYPE_KEY, 0)
#endif
}	// end namespace util

namespace ART {
namespace entity {
#include "using_ostream.h"
using std::istream;
using util::write_value;
using util::read_value;
using util::write_string;
using util::read_string;
USING_STACKTRACE
using util::persistent_traits;

//=============================================================================
// class module method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
module::module() :
#if 0
		object(), 
#endif
		persistent(), sequential_scope(),
		name(""), global_namespace(NULL), unrolled(false) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
module::module(const string& s) :
#if 0
		object(), 
#endif
		persistent(), sequential_scope(),
		name(s), global_namespace(new name_space("")), unrolled(false) {
	NEVER_NULL(global_namespace);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
module::~module() {
	STACKTRACE_DTOR("~module()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<name_space>
module::get_global_namespace(void) const {
	return global_namespace;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
module::set_global_namespace(excl_ptr<name_space>& n) {
	// automatically memory-managed
	global_namespace = n;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
module::what(ostream& o) const {
	return o << "module";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
module::dump(ostream& o) const {
	o << "In module created from: " << name;
	if (unrolled)
		o << " (unrolled)";
	o << endl;

	global_namespace->dump(o) << endl;

	if (!unrolled) {
		o << "Sequential instance management (to unroll): " << endl;
		return sequential_scope::dump(o);
	} else	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't just call sequential_scope::unroll, this makes sure
	entire module is not already unrolled.  
 */
void
module::unroll_module(void) {
	STACKTRACE("module::unroll_module()");
	if (!unrolled) {
		STACKTRACE("not already unrolled, unrolling...");
		// start with blank context
		unroll_context c;
		sequential_scope::unroll(c);
		unrolled = true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if HAVE_PERSISTENT_CONSTRUCT_EMPTY
/**
	Default empty constructor.  
	Not really used, because module will be stack allocated.  
 */
persistent*
module::construct_empty(const int i) {
	return new module();
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
module::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	STACKTRACE_PERSISTENT("module::collect_transient_info()");
	global_namespace->collect_transient_info(m);
	// the list itself is a statically allocated member
	sequential_scope::collect_transient_info_base(m);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
module::write_object(const persistent_object_manager& m, ostream& f) const {
	STACKTRACE_PERSISTENT("module::write_object()");
	write_string(f, name);
	m.write_pointer(f, global_namespace);
	write_value(f, unrolled);
	sequential_scope::write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
module::load_object(const persistent_object_manager& m, istream& f) {
	STACKTRACE_PERSISTENT("module::load_object()");
	read_string(f, name);
	m.read_pointer(f, global_namespace);
	read_value(f, unrolled);
//	global_namespace->load_object(m);	// not necessary
	sequential_scope::load_object_base(m, f);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

// clean up macros used in this module
#undef	ENABLE_STACKTRACE
#undef	STACKTRACE_DESTRUCTORS
#undef	STACKTRACE_DTOR
#undef	STACKTRACE_PERSISTENTS
#undef	STACKTRACE_PERSISTENT

#endif	// __ART_OBJECT_MODULE_CC__

