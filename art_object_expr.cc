// "art_object_expr.cc"

#include "art_parser.h"
#include "art_object.h"
#include "art_object_expr.h"

namespace ART {
namespace entity {
//=============================================================================
// class param_literal method definitions

param_literal::param_literal(const param_instantiation* v) :
		param_expr(), var(v) {
	assert(var);
}

param_literal::~param_literal() {
}

//=============================================================================
// class 


//=============================================================================
};	// end namepace entity
};	// end namepace ART

