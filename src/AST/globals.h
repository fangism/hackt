/**
	\file "AST/globals.h"
	Shared AST objects.  
	$Id: globals.h,v 1.1.2.1 2009/09/18 18:12:15 fang Exp $
 */

#include "AST/AST_fwd.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace parser {
using util::memory::count_ptr;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern
const count_ptr<const port_formal_decl>&
get_implicit_ports(void);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern
const count_ptr<const instance_declaration>&
get_implicit_globals(void);

//=============================================================================
}	// end namespace parser
}	// end namespace HAC


