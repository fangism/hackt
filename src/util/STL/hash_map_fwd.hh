/**
	\file "util/STL/hash_map_fwd.hh"
	Header-wrapper for gcc-version-specific placement of <hash_map>.
	\todo Make this configuration dependent on ac_cxx_ext_hash_map.
	$Id: hash_map_fwd.hh,v 1.12 2008/11/25 21:52:47 fang Exp $
 */

#ifndef	__UTIL_STL_HASH_MAP_FWD_HH__
#define	__UTIL_STL_HASH_MAP_FWD_HH__

#include "config.h"
#include "util/STL/allocator_fwd.hh"


#if defined(HASH_MAP_NAMESPACE)
#error	"HASH_MAP_NAMESPACE is already defined, but I need to set it."
#endif

// condition under which we force use of unordered_map over hash_map
#define	USING_UNORDERED_MAP	defined(HASH_MAP_DEPRECATED)

// compiler-version dependent location of hash_map
#if	USING_UNORDERED_MAP
#if	defined(HAVE_UNORDERED_MAP)
#define	HASH_MAP_NAMESPACE	std
#define	hash_map		unordered_map
// yes, i know this is dirty...
#elif	defined(HAVE_TR1_UNORDERED_MAP)
#define	HASH_MAP_NAMESPACE	std::tr1
#define	hash_map		unordered_map
// at least for transitional gcc-4.3, prefer tr1::unordered_map over hash_map
#else
#error	"hash_map is deprecated, but I cannot find unordered_map!"
#endif
#else
#if	defined(HASH_MAP_IN___GNU_CXX)
#define	HASH_MAP_NAMESPACE	__gnu_cxx
#elif	defined(HASH_MAP_IN_STD)
#define	HASH_MAP_NAMESPACE	std
// for pre-standard: gcc-2.95
#else	// __GNUC__
// your guess is as good as mine
#error	"If you know where hash_map is for your library, add it here."
#endif	// __GNUC__
#endif	// USING_ORDERED_MAP

// because namespace foo::bar { ... } is not yet part of the standard :-/
#if	USING_UNORDERED_MAP
#if defined(HAVE_UNORDERED_MAP)
#define	BEGIN_HASH_MAP_NS	BEGIN_NAMESPACE_STD
#define	END_HASH_MAP_NS		END_NAMESPACE_STD
#elif defined(HAVE_TR1_UNORDERED_MAP)
#define	BEGIN_HASH_MAP_NS	namespace std { namespace tr1 {
#define	END_HASH_MAP_NS		} }
#endif
#else
#define	BEGIN_HASH_MAP_NS	namespace HASH_MAP_NAMESPACE {
#define	END_HASH_MAP_NS		}
#endif

BEGIN_NAMESPACE_STD
// forward declaration of default comparator
template <class T>
struct equal_to;

#if	defined(HASH_MAP_INTEL_STYLE)
template <class K>
struct less;

template <class K, class C>
struct hash_compare;
#endif

END_NAMESPACE_STD

BEGIN_HASH_MAP_NS

template <class K>
struct hash;

#if	USING_UNORDERED_MAP
#define	HASH_MAP_TEMPLATE_SIGNATURE					\
template <class K, class T, class H, class E, class A>
#define	HASH_MAP_CLASS			unordered_map<K, T, H, E, A>
#elif	defined(HASH_MAP_SGI_STYLE)
/**
	Separate hash and compare functors.  
 */
#define	HASH_MAP_TEMPLATE_SIGNATURE					\
template <class K, class T, class H, class E, class A>
#define	HASH_MAP_CLASS			hash_map<K, T, H, E, A>
#elif	defined(HASH_MAP_INTEL_STYLE)
/**
	Combined hash_compare binary functor.  
 */
#define	HASH_MAP_TEMPLATE_SIGNATURE					\
template <class K, class T, class HC, class A>
#define	HASH_MAP_CLASS			hash_map<K, T, HC, A>
#else
#error	"Your hash_map is neither SGI nor Intel style, I need help."
#endif

#define	DEFAULT_HASH_MAP_TEMPLATE_SIGNATURE	template <class K, class T>

HASH_MAP_TEMPLATE_SIGNATURE
class hash_map;

/**
	Default hash_map typedef template.  
 */
DEFAULT_HASH_MAP_TEMPLATE_SIGNATURE
struct default_hash_map {
#if	USING_UNORDERED_MAP
	typedef	unordered_map<K, T, hash<K>, std::equal_to<K>,
		std::allocator<T> >
#elif	defined(HASH_MAP_SGI_STYLE)
	typedef	hash_map<K, T, hash<K>, std::equal_to<K>, std::allocator<T> >
#elif	defined(HASH_MAP_INTEL_STYLE)
	typedef	hash_map<K, T,
		hash_compare<K, std::less<K> >, std::allocator<T> >
#else
	typedef	someone_kill_me
#endif
					type;
	template <class K2, class T2>
	struct rebind : public default_hash_map<K2, T2> { };
};	// end struct default_hash_map

END_HASH_MAP_NS	// end namespace HASH_MAP_NAMESPACE

#define	USING_HASH	using HASH_MAP_NAMESPACE::hash;
#define	USING_HASH_MAP	using HASH_MAP_NAMESPACE::hash_map;


#endif	// __UTIL_STL_HASH_MAP_FWD_HH__

