/**
	\file "AST/globals.hh"
	Shared AST objects.  
	$Id: globals.hh,v 1.3 2010/07/01 20:20:24 fang Exp $
 */

#include "AST/AST_fwd.hh"
#include "util/memory/count_ptr.hh"

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern
const count_ptr<const type_completion_statement>
get_GND_attributes(void);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern
const count_ptr<const type_completion_statement>
get_Vdd_attributes(void);

//=============================================================================
}	// end namespace parser
}	// end namespace HAC


