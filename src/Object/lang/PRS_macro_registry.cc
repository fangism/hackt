/**
	\file "Object/lang/PRS_macro_registry.cc"
	Macro definitions belong here.  
	$Id: PRS_macro_registry.cc,v 1.1.2.1 2006/01/23 06:17:56 fang Exp $
 */

#include <iostream>
#include "Object/lang/PRS_macro_registry.h"
#include "util/qmap.tcc"

namespace HAC {
namespace entity {

namespace PRS {
using util::qmap;
#include "util/using_ostream.h"

//=============================================================================
// explicit template instantiations
#if 1
template class qmap<string, macro_definition_entry>;
// template macro_definition_entry
//	qmap<string, macro_definition_entry>::operator[](const string&) const;
#else
template macro_registry_type;
#endif

//=============================================================================
/**
	Global static initialization.
	This must appear first before any registrations take place in this 
	translation unit.
 */
const macro_registry_type
macro_registry;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Local static modifiable reference to use with registration.  
 */
static
macro_registry_type&
__macro_registry(const_cast<macro_registry_type&>(macro_registry));

//=============================================================================
// class macro_definition_entry method definitions

good_bool
macro_definition_entry::check_num_args(const size_t i) const {
	if (_check_num_args) {
		return (*_check_num_args)(i);
	} else	return good_bool(true);
}

//=============================================================================
/**
	Macro class registration function.  
 */
template <class T>
static
size_t
register_macro_class(void) {
	typedef	macro_registry_type::iterator		iterator;
	typedef	macro_registry_type::mapped_type	mapped_type;
	const string k(T::name);
	mapped_type& m = __macro_registry[k];
	if (m) {
		cerr << "Error: PRS macro by the name \'" << k <<
			"\' has already been registered!" << endl;
		THROW_EXIT;
	}
	m = macro_definition_entry(k, &T::main, &T::check_num_args);
	// oddly, this is needed to force instantiation of the [] const operator
	const mapped_type& n = macro_registry[k];
	INVARIANT(n);
	return macro_registry.size();
}

//=============================================================================
/**
	Convenient home for user-defined macro classes.  
	Could make this namespace anonymous...
 */
namespace __macros__ {

/**
	Convenient macro for declaring macro classes.
	TODO: consider adding a description string.  
 */
#define	DECLARE_PRS_MACRO_CLASS(class_name, macro_name)			\
struct class_name {							\
public:									\
	static const char			name[];			\
	static void main(const state_manager&);				\
	static good_bool check_num_args(const size_t);			\
private:								\
	static const size_t			id;			\
};									\
const char class_name::name[] = macro_name;				\
const size_t class_name::id = register_macro_class<class_name>();

/**
	Default check for number of arguments.  
 */
#define	DEFINE_DEFAULT_PRS_MACRO_CLASS_CHECK_NUM_ARGS(class_name)	\
good_bool								\
class_name::check_num_args(const size_t) {				\
	return good_bool(true);						\
}

//-----------------------------------------------------------------------------
/***
	IDEAS:
	assert_disjoint() -- node ID must NOT match
	assert_alias() -- node IDs must match
	order()
	stat()
	unstat()
	exclhi()
	excllo()
	passn()
	passp()
***/

//-----------------------------------------------------------------------------
DECLARE_PRS_MACRO_CLASS(Echo, "echo")

/**
	TODO: print out each canonical node name.  
 */
void
Echo::main(const state_manager&) {
	cout << "Hello, world!" << endl;
}

/// any number of arguments
good_bool
Echo::check_num_args(const size_t) {
	return good_bool(true);
}

//-----------------------------------------------------------------------------
#undef	DECLARE_PRS_MACRO_CLASS
}	// end namespace __macros__

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

