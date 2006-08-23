/**
	\file "AST/sizes.h"
	$Id: sizes.h,v 1.1 2006/08/23 20:57:14 fang Exp $
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

