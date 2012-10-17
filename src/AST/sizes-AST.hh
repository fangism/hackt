/**
	\file "AST/sizes-AST.hh"
	$Id: sizes-AST.hh,v 1.1 2007/02/27 02:27:53 fang Exp $
 */

#ifndef	__HAC_AST_SIZES_H__
#define	__HAC_AST_SIZES_H__

#include <iosfwd>

namespace HAC {
namespace parser {

// extern
std::ostream&
dump_class_sizes(std::ostream&);

}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_SIZES_H__

