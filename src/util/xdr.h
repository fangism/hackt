/**
	\file "util/xdr.h"
	Configured wrapper around xdr headers.  
	$Id: xdr.h,v 1.1.2.1 2006/02/24 10:55:41 fang Exp $
 */

#ifndef	__UTIL_XDR_H__
#define	__UTIL_XDR_H__

#include "config.h"

#ifdef HAVE_RPC_TYPES_H
#include <rpc/types.h>
#endif

#ifdef HAVE_RPC_XDR_H
#include <rpc/xdr.h>
#endif


#endif	/* __UTIL_XDR_H__ */

