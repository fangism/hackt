/**
	\file "util/template.hh"
	Configurable template-related language features.  
	Every macro definition in this file should be 
	configure-dependent, resulting from compiler characteristic
	tests via autoconf.  
	$Id: template.hh,v 1.2 2006/01/22 06:53:37 fang Exp $
 */
#ifndef	__UTIL_TEMPLATE_H__
#define	__UTIL_TEMPLATE_H__

#include "config.h"

// things to check:

// template typedefs

// support for explicit template instantiation of typedef

// requirement of template keyword in member function references

// extern template -- should be disabled as a GNU extension

// repository (-frepo)

// how to explicitly instantiation nested class/typedef

#endif	// __UTIL_TEMPLATE_H__

