/**
	\file "AST/parse_options.cc"
	$Id: parse_options.cc,v 1.1 2010/08/05 18:25:22 fang Exp $
 */

#include <iostream>
#include "AST/parse_options.hh"
#include "util/IO_utils.tcc"

namespace HAC {
namespace parser {
using util::write_value;
using util::read_value;

//=============================================================================
void
parse_options::write_object(ostream& o) const {
	write_value(o, export_all);
	write_value(o, namespace_instances);
	write_value(o, array_internal_nodes);
	write_value(o, case_collision_policy);
	write_value(o, unknown_spec_policy);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
parse_options::load_object(istream& i) {
	read_value(i, export_all);
	read_value(i, namespace_instances);
	read_value(i, array_internal_nodes);
	read_value(i, case_collision_policy);
	read_value(i, unknown_spec_policy);
}

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

