/**
	\file "Object/lang/directive_base.cc"
	$Id: directive_base.cc,v 1.1.2.1 2006/02/09 07:06:52 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include "Object/lang/directive_base.h"
#include "Object/expr/const_param.h"
#include "util/IO_utils.h"
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using util::write_value;
using util::read_value;

//=============================================================================
// class SPEC::directive_base method definitions

directive_base::directive_base() : name(), params(), nodes() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
directive_base::directive_base(const string& k) :
		name(k), params(), nodes() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
directive_base::~directive_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return 1-indexed offset of first error if found, else 0.  
 */
size_t
directive_base::first_param_error(void) const {
	// TODO: use std::find! and std::distance.
	const size_t s = params.size();
	if (s) {
		size_t i = 0;
		for ( ; i<s; i++) {
			if (!params[i]) {
				cerr << "Error resolving expression " << i <<
					"." << endl;
				return i+1;
			}
		}
	}
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return 1-indexed offset of first error if found, else 0.  
 */
size_t
directive_base::first_node_error(void) const {
	// TODO: use std::find! and std::distance.
	const size_t s = nodes.size();
	if (s) {
		size_t i = 0;
		for ( ; i<s; i++) {
			if (!nodes[i]) {
				cerr << "Error resolving literal " << i <<
					"." << endl;
				return i+1;
			}
		}
	}
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directive_base::collect_transient_info_base(
		persistent_object_manager& m) const {
	m.collect_pointer_list(params);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directive_base::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	INVARIANT(name.length());
	write_value(o, name);
	m.write_pointer_list(o, params);
	util::write_sequence(o, nodes);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directive_base::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	read_value(i, name);
	INVARIANT(name.length());
	m.read_pointer_list(i, params);
	util::read_sequence_resize(i, nodes);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

