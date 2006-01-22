/**
	\file "util/c_decl.h"
	Macro for wrapping C declarations.  
	$Id: c_decl.h,v 1.2 2006/01/22 06:53:32 fang Exp $
 */

#ifndef	__UTIL_C_DECL_H__
#define	__UTIL_C_DECL_H__

#if	defined(BEGIN_C_DECLS) || defined(END_C_DECLS)
#error	"tried to redefined BEGIN_C_DECLS or END_C_DECLS"
#else

#ifdef	__cplusplus
#define	BEGIN_C_DECLS			extern "C" {
#define	END_C_DECLS			}
#else	/* already in C mode */
#define	BEGIN_C_DECLS
#define	END_C_DECLS
#endif

#endif

#endif	/* __UTIL_C_DECL_H__ */

