/**
	\file "libchpfn/numeric.hh"
	$Id: numeric.hh,v 1.1 2010/10/14 17:33:45 fang Exp $
 */

#ifndef	__HAC_LIBCHPFN_NUMERIC_H__
#define	__HAC_LIBCHPFN_NUMERIC_H__

#include "Object/expr/types.hh"

namespace HAC {
namespace CHP {
using entity::bool_value_type;
using entity::int_value_type;

extern
int_value_type
sign_extend(const int_value_type);

extern
bool_value_type
extract_bit(const int_value_type, const int_value_type);

extern
int_value_type
extract_bits(const int_value_type, const int_value_type, const int_value_type);

}	// end namespace CHP
}	// end namespace HAC

#endif	// __HAC_LIBCHPFN_NUMERIC_H__

