/**
	\file "guile/hackt-config.hh"
	$Id: hackt-config.hh,v 1.2 2007/04/20 18:25:56 fang Exp $
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
