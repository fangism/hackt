# "funcs.m4.ac"
#	$Id: builtin_funcs.m4,v 1.2 2007/03/12 07:38:11 fang Exp $

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
	[int main(void) { return __builtin_popcountl(0xf00df00dUL) != 14; }],
	[fang_cv___builtin_popcountl=yes],
	[fang_cv___builtin_popcountl=no]
)
])
if test "$fang_cv___builtin_popcountl" = "yes"
then
	AC_DEFINE(HAVE_BUILTIN_POPCOUNTL, [1],
		[Define to 1 if __builtin_popcountl is available])
fi

AC_CACHE_CHECK([for __builtin_popcountll()],
	[fang_cv___builtin_popcountll], 
[AC_LINK_IFELSE(
	[int main(void) { return __builtin_popcountll(0xf00df00df00dULL) != 21; }],
	[fang_cv___builtin_popcountll=yes],
	[fang_cv___builtin_popcountll=no]
)
])
if test "$fang_cv___builtin_popcountll" = "yes"
then
	AC_DEFINE(HAVE_BUILTIN_POPCOUNTLL, [1],
		[Define to 1 if __builtin_popcountll is available])
fi
])dnl

dnl
dnl Macro: FANG_FUNC_PARITY
dnl Defines HAVE_BUILTIN_PARITY if __builtin_parity is available.  
dnl parity is the number of set bits in an binary integer.  
dnl
AC_DEFUN([FANG_FUNC_PARITY], 
[AC_CACHE_CHECK([for __builtin_parity()],
	[fang_cv___builtin_parity], 
[AC_LINK_IFELSE(
	[int main(void) { return __builtin_parity(0xf00d) != 1; }],
	[fang_cv___builtin_parity=yes],
	[fang_cv___builtin_parity=no]
)
])
if test "$fang_cv___builtin_parity" = "yes"
then
	AC_DEFINE(HAVE_BUILTIN_PARITY, [1],
		[Define to 1 if __builtin_parity is available])
fi

AC_CACHE_CHECK([for __builtin_parityl()],
	[fang_cv___builtin_parityl], 
[AC_LINK_IFELSE(
	[int main(void) { return __builtin_parityl(0xf00df00dUL) != 0; }],
	[fang_cv___builtin_parityl=yes],
	[fang_cv___builtin_parityl=no]
)
])
if test "$fang_cv___builtin_parityl" = "yes"
then
	AC_DEFINE(HAVE_BUILTIN_PARITYL, [1],
		[Define to 1 if __builtin_parityl is available])
fi

AC_CACHE_CHECK([for __builtin_parityll()],
	[fang_cv___builtin_parityll], 
[AC_LINK_IFELSE(
	[int main(void) { return __builtin_parityll(0xf00df00df00dULL) != 1; }],
	[fang_cv___builtin_parityll=yes],
	[fang_cv___builtin_parityll=no]
)
])
if test "$fang_cv___builtin_parityll" = "yes"
then
	AC_DEFINE(HAVE_BUILTIN_PARITYLL, [1],
		[Define to 1 if __builtin_parityll is available])
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

AC_CACHE_CHECK([for __builtin_ffsll()],
	[fang_cv___builtin_ffsll], 
[AC_LINK_IFELSE(
	[int main(void) { return __builtin_ffsll(0xdeadf000f000ULL) != 13; }],
	[fang_cv___builtin_ffsll=yes],
	[fang_cv___builtin_ffsll=no]
)
])
if test "$fang_cv___builtin_ffsll" = "yes"
then
	AC_DEFINE(HAVE_BUILTIN_FFSLL, [1],
		[Define to 1 if __builtin_ffsll is available])
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

AC_CACHE_CHECK([for __builtin_clzll()],
	[fang_cv___builtin_clzll], 
[AC_LINK_IFELSE(
	[int main(void) { return __builtin_clzll(0xf000ULL) != 48; }],
	[fang_cv___builtin_clzll=yes],
	[fang_cv___builtin_clzll=no]
)
])
if test "$fang_cv___builtin_clzll" = "yes"
then
	AC_DEFINE(HAVE_BUILTIN_CLZLL, [1],
		[Define to 1 if __builtin_clzll is available])
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

AC_CACHE_CHECK([for __builtin_ctzll()],
	[fang_cv___builtin_ctzll], 
[AC_LINK_IFELSE(
	[int main(void) { return __builtin_ctzll(0xbeeff000f000ULL) != 12; }],
	[fang_cv___builtin_ctzll=yes],
	[fang_cv___builtin_ctzll=no]
)
])
if test "$fang_cv___builtin_ctzll" = "yes"
then
	AC_DEFINE(HAVE_BUILTIN_CTZLL, [1],
		[Define to 1 if __builtin_ctzll is available])
fi
])dnl

dnl
dnl Macro: FANG_FUNC_BUILTIN_BITOPS
dnl One macro to call all of the above.
dnl
AC_DEFUN([FANG_FUNC_BUILTIN_BITOPS], [
AC_REQUIRE([FANG_FUNC_POPCOUNT])
AC_REQUIRE([FANG_FUNC_PARITY])
AC_REQUIRE([FANG_FUNC_FINDFIRSTSET])
AC_REQUIRE([FANG_FUNC_COUNTLEADINGZEROS])
AC_REQUIRE([FANG_FUNC_COUNTTRAILINGZEROS])
])

