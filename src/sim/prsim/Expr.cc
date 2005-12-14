/**
	\file "sim/prsim/Expr.cc"
	Expression node implementation.  
	$Id: Expr.cc,v 1.1.2.2 2005/12/14 07:16:09 fang Exp $
 */

#include "sim/prsim/Expr.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
//=============================================================================
// class Expr method definitions

Expr::Expr() : size(0) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Expr::Expr(const count_type s) : size(s) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Expr::~Expr() { }

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

