// "art_object_module.cc"

#include <iostream>
#include "art_object_module.h"
#include "art_object_IO.tcc"

namespace ART {
namespace entity {
using namespace std;
//=============================================================================
// class module method definitions

/**
	Private empty constructor.
 */
module::module() : sequential_scope(), object(), name(""), 
		global_namespace(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
module::module(const string& s) : sequential_scope(), object(), name(s), 
		global_namespace(new name_space("")) {
	assert(global_namespace);
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
	Default empty constructor.  
	Not really used, because module will be stack allocated.  
 */
object*
module::construct_empty(void) {
	return new module();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
module::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, MODULE_TYPE)) {
	global_namespace->collect_transient_info(m);
	// the list itself is a statically allocated member
	collect_object_pointer_list(m);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
module::write_object(persistent_object_manager& m) const {
	ostream& f = m.lookup_write_buffer(this);
	assert(f.good());
	write_string(f, name);
	m.write_pointer(f, global_namespace);
	write_object_pointer_list(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
module::load_object(persistent_object_manager& m) {
if (!m.flag_visit(this)) {
	istream& f = m.lookup_read_buffer(this);
	assert(f.good());
	read_string(f, name);
	m.read_pointer(f, global_namespace);
//	global_namespace->load_object(m);	// not necessary
	load_object_pointer_list(m);
}
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

