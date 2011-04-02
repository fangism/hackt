/**
	\file "Object/unroll/meta_conditional_base.cc"
	Base for guarded bodies in meta language.  
	$Id: meta_conditional_base.cc,v 1.7 2011/04/02 01:46:14 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include "Object/unroll/meta_conditional_base.h"
#include "Object/expr/pbool_expr.h"

// #include "common/TODO.h"
#include "util/persistent_object_manager.tcc"
// #include "util/IO_utils.h"
#include "util/memory/count_ptr.tcc"
// #include "util/stacktrace.h"

//=============================================================================
namespace HAC {
namespace entity {

//=============================================================================
// class meta_conditional_base method definitions

meta_conditional_base::meta_conditional_base() : guards() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
meta_conditional_base::~meta_conditional_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
meta_conditional_base::collect_transient_info_base(
		persistent_object_manager& m) const {
	m.collect_pointer_list(guards);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
meta_conditional_base::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer_list(o, guards);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
meta_conditional_base::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer_list(i, guards);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

