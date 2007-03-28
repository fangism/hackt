/**
	\file "guile/hackt-config.h"
	$Id: hackt-config.h,v 1.1.2.1 2007/03/28 06:11:51 fang Exp $
 */

#ifndef	__HAC_GUILE_HACKT_CONFIG_H__
#define	__HAC_GUILE_HACKT_CONFIG_H__

#include "util/c_decl.h"

namespace HAC {
namespace guile_wrap {

}	// end namespace guile_wrap
}	// end namespace HAC

//=============================================================================
BEGIN_C_DECLS

extern
void
hackt_config_init(void);

END_C_DECLS

#endif	// __HAC_GUILE_HACKT_CONFIG_H__
