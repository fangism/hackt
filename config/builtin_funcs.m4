# "funcs.m4.ac"
#	$Id: builtin_funcs.m4,v 1.1 2007/02/21 17:00:20 fang Exp $

dnl NOTE: recommend wrapping all of the below calls with AC_LANG_PUSH/POP(C++)
dnl
dnl Q: should we convert link-tests to run-tests?
dnl consequence: will fail cross-compile tests.  

dnl
dnl Macro: FANG_FUNC_POPCOUNT
dnl Defines HAVE_BUILTIN_POPCOUNT if __builtin_popcount is available.  
dnl popcount is the number of set bits in an binary integer.  
dnl
AC_DEFUN([FANG_FUNC_POPCOUNT], 
[AC_CACHE_CHECK([for __builtin_popcount()],
	[fang_cv___builtin_popcount], 
[AC_LINK_IFELSE(
	[int main(void) { return __builtin_popcount(0xf00d) != 7; }],
	[fang_cv___builtin_popcount=yes],
	[fang_cv___builtin_popcount=no]
)
])
if test "$fang_cv___builtin_popcount" = "yes"
then
	AC_DEFINE(HAVE_BUILTIN_POPCOUNT, [1],
		[Define to 1 if __builtin_popcount is available])
fi

AC_CACHE_CHECK([for __builtin_popcountl()],
	[fang_cv___builtin_popcountl], 
[AC_LINK_IFELSE(
	[int main(void) { return __builtin_popcountl(0xf00df00d) != 14; }],
	[fang_cv___builtin_popcountl=yes],
	[fang_cv___builtin_popcountl=no]
)
])
if test "$fang_cv___builtin_popcountl" = "yes"
then
	AC_DEFINE(HAVE_BUILTIN_POPCOUNTL, [1],
		[Define to 1 if __builtin_popcountl is available])
fi
])dnl

dnl
dnl Macro: FANG_FUNC_FINDFIRSTSET
dnl Defines HAVE_BUILTIN_FFS if __builtin_ffs is available.  
dnl ffs finds the index of the first set bit, indexed at 1, 0 returns 0.
dnl
AC_DEFUN([FANG_FUNC_FINDFIRSTSET], 
[AC_CACHE_CHECK([for __builtin_ffs()],
	[fang_cv___builtin_ffs], 
[AC_LINK_IFELSE(
	[int main(void) { return __builtin_ffs(0xf000) != 13; }],
	[fang_cv___builtin_ffs=yes],
	[fang_cv___builtin_ffs=no]
)
])
if test "$fang_cv___builtin_ffs" = "yes"
then
	AC_DEFINE(HAVE_BUILTIN_FFS, [1],
		[Define to 1 if __builtin_ffs is available])
fi

AC_CACHE_CHECK([for __builtin_ffsl()],
	[fang_cv___builtin_ffsl], 
[AC_LINK_IFELSE(
	[int main(void) { return __builtin_ffsl(0xf000f000) != 13; }],
	[fang_cv___builtin_ffsl=yes],
	[fang_cv___builtin_ffsl=no]
)
])
if test "$fang_cv___builtin_ffsl" = "yes"
then
	AC_DEFINE(HAVE_BUILTIN_FFSL, [1],
		[Define to 1 if __builtin_ffsl is available])
fi

])dnl

dnl
dnl Macro: FANG_FUNC_COUNTLEADINGZEROS
dnl Defines HAVE_BUILTIN_CLZ if __builtin_clz is available.  
dnl clz finds the number of leading zeros in an integer, undefined if all 0s
dnl TODO: not sure if run-test is correct
dnl
AC_DEFUN([FANG_FUNC_COUNTLEADINGZEROS], 
[AC_CACHE_CHECK([for __builtin_clz()],
	[fang_cv___builtin_clz], 
[AC_LINK_IFELSE(
	[int main(void) { return __builtin_clz(0xf000) != 16; }],
	[fang_cv___builtin_clz=yes],
	[fang_cv___builtin_clz=no]
)
])
if test "$fang_cv___builtin_clz" = "yes"
then
	AC_DEFINE(HAVE_BUILTIN_CLZ, [1],
		[Define to 1 if __builtin_clz is available])
fi

AC_CACHE_CHECK([for __builtin_clzl()],
	[fang_cv___builtin_clzl], 
[AC_LINK_IFELSE(
	[int main(void) { return __builtin_clzl(0xf000) != 16; }],
	[fang_cv___builtin_clzl=yes],
	[fang_cv___builtin_clzl=no]
)
])
if test "$fang_cv___builtin_clzl" = "yes"
then
	AC_DEFINE(HAVE_BUILTIN_CLZL, [1],
		[Define to 1 if __builtin_clzl is available])
fi
])dnl


dnl
dnl Macro: FANG_FUNC_COUNTTRAILINGZEROS
dnl Defines HAVE_BUILTIN_CTZ if __builtin_ctz is available.  
dnl ctz finds the number of trailing zeros in an integer, undefined if all 0s
dnl
AC_DEFUN([FANG_FUNC_COUNTTRAILINGZEROS], 
[AC_CACHE_CHECK([for __builtin_ctz()],
	[fang_cv___builtin_ctz], 
[AC_LINK_IFELSE(
	[int main(void) { return __builtin_ctz(0xf000) != 12; }],
	[fang_cv___builtin_ctz=yes],
	[fang_cv___builtin_ctz=no]
)
])
if test "$fang_cv___builtin_ctz" = "yes"
then
	AC_DEFINE(HAVE_BUILTIN_CTZ, [1],
		[Define to 1 if __builtin_ctz is available])
fi

AC_CACHE_CHECK([for __builtin_ctzl()],
	[fang_cv___builtin_ctzl], 
[AC_LINK_IFELSE(
	[int main(void) { return __builtin_ctzl(0xf000f000) != 12; }],
	[fang_cv___builtin_ctzl=yes],
	[fang_cv___builtin_ctzl=no]
)
])
if test "$fang_cv___builtin_ctzl" = "yes"
then
	AC_DEFINE(HAVE_BUILTIN_CTZL, [1],
		[Define to 1 if __builtin_ctzl is available])
fi
])dnl

dnl
dnl Macro: FANG_FUNC_BUILTIN_BITOPS
dnl One macro to call all of the above.
dnl
AC_DEFUN([FANG_FUNC_BUILTIN_BITOPS], [
AC_REQUIRE([FANG_FUNC_POPCOUNT])
AC_REQUIRE([FANG_FUNC_FINDFIRSTSET])
AC_REQUIRE([FANG_FUNC_COUNTLEADINGZEROS])
AC_REQUIRE([FANG_FUNC_COUNTTRAILINGZEROS])
])

