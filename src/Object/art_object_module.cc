/**
	\file "art_object_module.cc"
	Method definitions for module class.  
 	$Id: art_object_module.cc,v 1.13 2005/01/13 18:59:45 fang Exp $
 */

#ifndef	__ART_OBJECT_MODULE_CC__
#define	__ART_OBJECT_MODULE_CC__

#include <iostream>
#include "art_object_module.h"
#include "art_object_namespace.h"
#include "persistent_object_manager.tcc"
#include "art_object_type_hash.h"

namespace ART {
namespace entity {
#include "using_ostream.h"
using std::istream;
using util::write_value;
using util::read_value;
using util::write_string;
using util::read_string;

//=============================================================================
// class module method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(module, MODULE_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
module::module() :
		object(), persistent(), sequential_scope(),
		name(""), global_namespace(NULL), unrolled(false) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
module::module(const string& s) :
		object(), persistent(), sequential_scope(),
		name(s), global_namespace(new name_space("")), unrolled(false) {
	NEVER_NULL(global_namespace);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
module::~module() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<name_space>
module::get_global_namespace(void) const {
	return global_namespace;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
module::set_global_namespace(excl_ptr<name_space> n) {
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
	o << "In module created from: " << name << endl;
	global_namespace->dump(o) << endl;
	o << "Sequential instance management: " << endl;
	return sequential_scope::dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't just call sequential_scope::unroll, this makes sure
	entire module is not already unrolled.  
 */
void
module::unroll_module(void) {
	if (!unrolled) {
		sequential_scope::unroll();
		unrolled = true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default empty constructor.  
	Not really used, because module will be stack allocated.  
 */
persistent*
module::construct_empty(const int i) {
	return new module();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
module::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, MODULE_TYPE_KEY)) {
	global_namespace->collect_transient_info(m);
	// the list itself is a statically allocated member
	sequential_scope::collect_transient_info_base(m);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
module::write_object(const persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	INVARIANT(f.good());
	WRITE_POINTER_INDEX(f, m);
	write_string(f, name);
	m.write_pointer(f, global_namespace);
	write_value(f, unrolled);
	sequential_scope::write_object_base(m, f);
	WRITE_OBJECT_FOOTER(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
module::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	INVARIANT(f.good());
	STRIP_POINTER_INDEX(f, m);
	read_string(f, name);
	m.read_pointer(f, global_namespace);
	read_value(f, unrolled);
//	global_namespace->load_object(m);	// not necessary
	sequential_scope::load_object_base(m, f);
	STRIP_OBJECT_FOOTER(f);
}
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_MODULE_CC__

