// This file is part of the uSTL library, an STL implementation.
//
// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#define __STDC_LIMIT_MACROS	// For WCHAR_MIN and WCHAR_MAX in stdint.
#define __STDC_CONSTANT_MACROS	// For UINT??_C macros to avoid using L and UL suffixes on constants.
#include "config.h"
#ifndef STDC_HEADERS
    #error "This library requires standard C and C++ headers to compile."
#endif
#ifndef STDUNIX_HEADERS
    #error "This library requires standard UNIX headers to compile."
#endif
#if HAVE_STDINT_H
    #include <stdint.h>
#elif HAVE_INTTYPES_H
    #include <inttypes.h>
#else
    #error "Need standard integer types definitions, usually in stdint.h"
#endif
#if HAVE_SYS_TYPES_H
    #include <sys/types.h>
#endif
#include <stddef.h>		// For ptrdiff_t, size_t
#include <stdarg.h>
#include <limits.h>
#include <float.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#if HAVE_ALLOCA_H
    #include <alloca.h>
#endif
#ifndef WITHOUT_LIBSTDCPP
    #include <exception>
    #include <typeinfo>
    #include <new>
#endif
#ifndef SIZE_MAX
    #define SIZE_MAX		UINT_MAX
#endif
#if sun || __sun		// Solaris defines UINTPTR_MAX as empty.
    #undef UINTPTR_MAX
    #define UINTPTR_MAX		ULONG_MAX
#endif
#ifndef WCHAR_MAX
    #ifdef __WCHAR_MAX__
	#define WCHAR_MAX	__WCHAR_MAX__
    #else
	#define WCHAR_MAX	CHAR_MAX
    #endif
#endif
#if HAVE_LONG_LONG
    #ifndef LLONG_MAX
	#define ULLONG_MAX	UINT64_C(0xFFFFFFFFFFFFFFFF)
	#define LLONG_MAX	INT64_C(0x7FFFFFFFFFFFFFFF)
	#define LLONG_MIN	ULLONG_MAX
    #endif
#endif
#ifndef BYTE_ORDER
    #define LITTLE_ENDIAN	USTL_LITTLE_ENDIAN
    #define BIG_ENDIAN		USTL_BIG_ENDIAN
    #define BYTE_ORDER		USTL_BYTE_ORDER
#endif

typedef size_t		uoff_t;		///< A type for storing offsets into blocks measured by size_t.
typedef uint32_t	hashvalue_t;	///< Value type returned by the hash functions.
typedef size_t		streamsize;	///< Size of stream data
typedef uoff_t		streamoff;	///< Offset into a stream

#if !defined(UINTPTR_MAX) || !defined(UINT32_C)
    #error "If you include stdint.h before ustl.h, define __STDC_LIMIT_MACROS and __STDC_CONSTANT_MACROS first"
#endif
#if WANT_ALWAYS_INLINE
    #define inline INLINE inline
#endif

//----------------------------------------------------------------------
// Functors and general utilities.
//----------------------------------------------------------------------

/// Evaluates to itself
#define ITSELF(x)		x

/// Concatenates \p a and \p b
#define PASTE(a,b)	a##b

//----------------------------------------------------------------------
// Lists and other iterators
//----------------------------------------------------------------------

/// The maximum number of elements in REPEAT, LIST, and COMMA_LIST
#define METAMAC_MAXN	9

/// Simple list with no separators. Repeats x N times.
/// @{
#define REPEAT_1(x)	x(1)
#define REPEAT_2(x)	REPEAT_1(x) x(2)
#define REPEAT_3(x)	REPEAT_2(x) x(3)
#define REPEAT_4(x)	REPEAT_3(x) x(4)
#define REPEAT_5(x)	REPEAT_4(x) x(5)
#define REPEAT_6(x)	REPEAT_5(x) x(6)
#define REPEAT_7(x)	REPEAT_6(x) x(7)
#define REPEAT_8(x)	REPEAT_7(x) x(8)
#define REPEAT_9(x)	REPEAT_8(x) x(9)
#define REPEAT(N,x)	PASTE(REPEAT_,N)(x)
/// @}

/// Simple separated list. Repeats x N times with sep in between.
/// @{
#define LIST_1(x,sep)	x(1)
#define LIST_2(x,sep)	LIST_1(x,sep) sep x(2)
#define LIST_3(x,sep)	LIST_2(x,sep) sep x(3)
#define LIST_4(x,sep)	LIST_3(x,sep) sep x(4)
#define LIST_5(x,sep)	LIST_4(x,sep) sep x(5)
#define LIST_6(x,sep)	LIST_5(x,sep) sep x(6)
#define LIST_7(x,sep)	LIST_6(x,sep) sep x(7)
#define LIST_8(x,sep)	LIST_7(x,sep) sep x(8)
#define LIST_9(x,sep)	LIST_8(x,sep) sep x(9)
#define LIST(N,x,sep)	PASTE(LIST_,N)(x,sep)
/// @}

/// Comma separated list. A special case of LIST needed because the preprocessor can't substitute commas.
/// @{
#define COMMA_LIST_1(x)	x(1)
#define COMMA_LIST_2(x)	COMMA_LIST_1(x), x(2)
#define COMMA_LIST_3(x)	COMMA_LIST_2(x), x(3)
#define COMMA_LIST_4(x)	COMMA_LIST_3(x), x(4)
#define COMMA_LIST_5(x)	COMMA_LIST_4(x), x(5)
#define COMMA_LIST_6(x)	COMMA_LIST_5(x), x(6)
#define COMMA_LIST_7(x)	COMMA_LIST_6(x), x(7)
#define COMMA_LIST_8(x)	COMMA_LIST_7(x), x(8)
#define COMMA_LIST_9(x)	COMMA_LIST_8(x), x(9)
#define COMMA_LIST(N,x)	PASTE(COMMA_LIST_,N)(x)
/// @}

//----------------------------------------------------------------------
// Macros for defining LIST arguments.
//----------------------------------------------------------------------

/// Ignores N, producing lists of identically named arguments.
#define LARG_NONE(name,N)	name

/// Appends N to name.
#define LARG_NUMBER(name,N)	name##N

/// name is a reference type.
#define LARG_REF(name,N)	name##N&

/// Sequential parameter passed by value with sequential types.
#define LARG_MT_PARAM_BY_VALUE(type,name,N)	type##N name##N

/// Sequential parameter passed by reference with sequential types.
#define LARG_MT_PARAM_BY_REF(type,name,N)	type##N& name##N

//----------------------------------------------------------------------

namespace ustl {
/// Template metaprogramming tools
namespace tm {

/// An empty type useful as a placeholder.
class NullType { };

/// Converts an integer to a type.
template <int v> struct Int2Type { enum { value = v }; };

/// Converts an type to a unique empty type.
template <typename T> struct Type2Type { typedef T OriginalType; };

/// Selects type Result = flag ? T : U
template <bool flag, typename T, typename U>
struct Select { typedef T Result; };
template <typename T, typename U>
struct Select<false, T, U> { typedef U Result; };

/// IsSameType<T,U>::value is true when T=U
template <typename T, typename U>
struct IsSameType { enum { value = false }; };
template <typename T>
struct IsSameType<T,T> { enum { value = true }; };

/// \brief Checks for conversion possibilities between T and U
/// Conversion<T,U>::exists is true if T is convertible to U
/// Conversion<T,U>::sameType is true if U is T
template <typename T, typename U>
struct Conversion {
private:
    typedef char UT;
    typedef short TT;
    static UT Test (U);
    static TT Test (...);
    static T MakeT (void);
public:
    enum {
	exists = sizeof(UT) == sizeof(Test(MakeT())),
	sameType = false
    };
};
template <typename T>
struct Conversion<T, T> { enum { exists = true, sameType = true }; };
template <typename T>
struct Conversion<void, T> { enum { exists = false, sameType = false }; };
template <typename T>
struct Conversion<T, void> { enum { exists = false, sameType = false }; };
template <>
struct Conversion<void, void> { enum { exists = true, sameType = true }; };

/// SuperSubclass<T,U>::value is true when U is derived from T, or when U is T
template <typename T, typename U>
struct SuperSubclass {
    enum { value = (::ustl::tm::Conversion<const volatile U*, const volatile T*>::exists &&
		    !::ustl::tm::Conversion<const volatile T*, const volatile void*>::sameType) };
    enum { dontUseWithIncompleteTypes = sizeof(T)==sizeof(U) };	// Dummy enum to make sure that both classes are fully defined.
};
template <>
struct SuperSubclass<void, void> { enum { value = false }; };
template <typename U>
struct SuperSubclass<void, U> {
    enum { value = false };
    enum { dontUseWithIncompleteTypes = 0==sizeof(U) };
};
template <typename T>
struct SuperSubclass<T, void> {
    enum { value = false };
    enum { dontUseWithIncompleteTypes = 0==sizeof(T) };
};

/// SuperSubclassStrict<T,U>::value is true when U is derived from T
template <typename T, typename U>
struct SuperSubclassStrict {
    enum { value = SuperSubclass<T,U>::value &&
		    !::ustl::tm::Conversion<const volatile T*, const volatile U*>::sameType };
};

#if !HAVE_CPP11
// static assert support
template <bool> struct CompileTimeError;
template <> struct CompileTimeError<true> {};
#define static_assert(cond,msg)	{ ::ustl::tm::CompileTimeError<!!(cond)> ERROR_##msg; (void) ERROR_##msg; }
#endif

} // namespace tm
} // namespace ustl

namespace ustl {
namespace tm {

/// The building block of typelists. Use it throught the Seq templates.
template <typename T, typename U>
struct Typelist {
    typedef T Head;
    typedef U Tail;
};

/// Namespace containing typelist-related functionality.
namespace tl {

//----------------------------------------------------------------------
// Seq template definitions. The macros expand to a spec per arg count
//
#define TL_MAX_SEQ_TYPES		9
#define TL_MAX_SEQ_SPECS		8
#define TL_SEQ_TYPE(n)			T##n
#define TL_SEQ_TYPENAME(n)		typename TL_SEQ_TYPE(n)
#define TL_SEQ_NULLTYPE_DEFAULT(n)	TL_SEQ_TYPENAME(n)=NullType
#define TL_SEQ_TL_END(n)		> 
#define TL_SEQ_ONE_TYPELIST(n)		Typelist<TL_SEQ_TYPE(n)

/// Creates a typelist from a sequence of types
template <COMMA_LIST(TL_MAX_SEQ_TYPES,TL_SEQ_NULLTYPE_DEFAULT)>
struct Seq {
    typedef COMMA_LIST(TL_MAX_SEQ_TYPES,TL_SEQ_ONE_TYPELIST),
	NullType REPEAT(TL_MAX_SEQ_TYPES,TL_SEQ_TL_END) Type;
};

#define TL_SEQ_SPEC(n)	\
template <COMMA_LIST (n, TL_SEQ_TYPENAME)>	\
struct Seq<COMMA_LIST (n, TL_SEQ_TYPE)> {	\
    typedef COMMA_LIST(n,TL_SEQ_ONE_TYPELIST),	\
	NullType REPEAT(n,TL_SEQ_TL_END) Type;	\
}
LIST(TL_MAX_SEQ_SPECS,TL_SEQ_SPEC, ;);

#undef TL_SEQ_SPEC
#undef TL_SEQ_TL_END
#undef TL_SEQ_ONE_TYPELIST
#undef TL_SEQ_NULLTYPE_DEFAULT
#undef TL_SEQ_TYPE
#undef TL_MAX_SEQ_SPECS

//----------------------------------------------------------------------
// Various utility functions follow.

/// Length<List>::value is the number of types in the typelist.
template <typename List> struct Length { };
template <> struct Length<NullType> { enum { value = 0 }; };
template <typename T, typename U>
struct Length<Typelist<T, U> > { enum { value = 1 + Length<U>::value }; };

/// TypeAt<List, i>::Result is the ith type in List
template <typename List, unsigned index> struct TypeAt { };
template <class Head, class Tail>
struct TypeAt<Typelist<Head, Tail>, 0> {
    typedef Head Result;
};
template <class Head, class Tail, unsigned index>
struct TypeAt<Typelist<Head, Tail>, index> {
    typedef typename TypeAt<Tail, index-1>::Result Result;
};

/// TypeAtNonStrict<List,i,DefaultType>::Result is List[i] or DefaultType if out of range.
template <typename List, unsigned index, typename DefaultType = NullType>
struct TypeAtNonStrict {
    typedef DefaultType Result;
};
template <typename Head, typename Tail, typename DefaultType>
struct TypeAtNonStrict<Typelist<Head, Tail>, 0, DefaultType> {
    typedef Head Result;
};
template <typename Head, typename Tail, unsigned index, typename DefaultType>
struct TypeAtNonStrict<Typelist<Head, Tail>, index, DefaultType> {
    typedef typename TypeAtNonStrict<Tail, index-1, DefaultType>::Result Result;
};

/// IndexOf<List,T>::value is the position of T in List, or -1 if not found.
template <typename List, typename T> struct IndexOf;
template <typename T>
struct IndexOf<NullType, T> { enum { value = -1 }; };
template <typename T, typename Tail>
struct IndexOf<Typelist<T, Tail>, T> { enum { value = 0 }; };
template <typename Head, typename Tail, typename T>
struct IndexOf<Typelist<Head, Tail>, T> {
private:
    enum { iintail = IndexOf<Tail, T>::value };
public:
    enum { value = (iintail == -1 ? -1 : 1+iintail) };
};

/// Appends a type or a typelist to another in Append<TList, T>::Result
template <typename List, typename T> struct Append;
template <> struct Append<NullType, NullType> { typedef NullType Result; };
template <typename T> struct Append<NullType, T> {
    typedef Typelist<T,NullType> Result;
};
template <typename Head, typename Tail>
struct Append<NullType, Typelist<Head, Tail> > {
    typedef Typelist<Head, Tail> Result;
};
template <typename Head, typename Tail, typename T>
struct Append<Typelist<Head, Tail>, T> {
    typedef Typelist<Head, typename Append<Tail, T>::Result> Result;
};
        
// Erase<List, T>::Result contains List without the first T.
template <typename TList, typename T> struct Erase;
template <typename T>
struct Erase<NullType, T> { typedef NullType Result; };
template <typename T, typename Tail>
struct Erase<Typelist<T, Tail>, T> { typedef Tail Result; };
template <typename Head, typename Tail, typename T>
struct Erase<Typelist<Head, Tail>, T> {
    typedef Typelist<Head, typename Erase<Tail, T>::Result> Result;
};

// EraseAll<List, T>::Result contains List without any T.
template <typename List, typename T> struct EraseAll;
template <typename T>
struct EraseAll<NullType, T> { typedef NullType Result; };
template <typename T, typename Tail>
struct EraseAll<Typelist<T, Tail>, T> {
    typedef typename EraseAll<Tail, T>::Result Result;
};
template <typename Head, typename Tail, typename T>
struct EraseAll<Typelist<Head, Tail>, T> {
    typedef Typelist<Head, typename EraseAll<Tail, T>::Result> Result;
};

/// Removes all duplicate types in a typelist
template <typename List> struct NoDuplicates;
template <> struct NoDuplicates<NullType> { typedef NullType Result; };
template <typename Head, typename Tail>
struct NoDuplicates< Typelist<Head, Tail> > {
private:
    typedef typename NoDuplicates<Tail>::Result L1;
    typedef typename Erase<L1, Head>::Result L2;
public:
    typedef Typelist<Head, L2> Result;
};

// Replaces the first occurence of a type in a typelist, with another type
template <typename List, typename T, typename U> struct Replace;
template <typename T, typename U>
struct Replace<NullType, T, U> { typedef NullType Result; };
template <typename T, typename Tail, typename U>
struct Replace<Typelist<T, Tail>, T, U> {
    typedef Typelist<U, Tail> Result;
};
template <typename Head, typename Tail, typename T, typename U>
struct Replace<Typelist<Head, Tail>, T, U> {
    typedef Typelist<Head, typename Replace<Tail, T, U>::Result> Result;
};

// Replaces all occurences of a type in a typelist, with another type
template <typename List, typename T, typename U> struct ReplaceAll;
template <typename T, typename U>
struct ReplaceAll<NullType, T, U> { typedef NullType Result; };
template <typename T, typename Tail, typename U>
struct ReplaceAll<Typelist<T, Tail>, T, U> {
    typedef Typelist<U, typename ReplaceAll<Tail, T, U>::Result> Result;
};
template <typename Head, typename Tail, typename T, typename U>
struct ReplaceAll<Typelist<Head, Tail>, T, U> {
    typedef Typelist<Head, typename ReplaceAll<Tail, T, U>::Result> Result;
};

// Reverses a typelist
template <typename List> struct Reverse;
template <> struct Reverse<NullType> { typedef NullType Result; };
template <typename Head, typename Tail>
struct Reverse< Typelist<Head, Tail> > {
    typedef typename Append<typename Reverse<Tail>::Result, Head>::Result Result;
};

// Finds the type in a typelist that is the most derived from a given type
template <typename List, typename T> struct MostDerived;
template <typename T> struct MostDerived<NullType, T> { typedef T Result; };
template <typename Head, typename Tail, typename T>
struct MostDerived<Typelist<Head, Tail>, T> {
private:
    typedef typename MostDerived<Tail, T>::Result Candidate;
public:
    typedef typename Select<SuperSubclass<Candidate,Head>::value, Head, Candidate>::Result Result;
};

// Arranges the types in a typelist so that the most derived types appear first
template <typename List> struct DerivedToFront;
template <> struct DerivedToFront<NullType> { typedef NullType Result; };
template <typename Head, typename Tail>
struct DerivedToFront< Typelist<Head, Tail> > {
private:
    typedef typename MostDerived<Tail, Head>::Result TheMostDerived;
    typedef typename Replace<Tail, TheMostDerived, Head>::Result Temp;
    typedef typename DerivedToFront<Temp>::Result L;
public:
    typedef Typelist<TheMostDerived, L> Result;
};

//----------------------------------------------------------------------

} // namespace tl
} // namespace tm
} // namespace ustl

namespace ustl {
namespace tm {

//----------------------------------------------------------------------
// Type classes and type modifiers
//----------------------------------------------------------------------

typedef tl::Seq<unsigned char, unsigned short, unsigned, unsigned long>::Type
							StdUnsignedInts;
typedef tl::Seq<signed char, short, int, long>::Type	StdSignedInts;
typedef tl::Seq<bool, char, wchar_t>::Type		StdOtherInts;
typedef tl::Seq<float, double>::Type			StdFloats;

template <typename U> struct Identity			{ typedef U Result; };
template <typename U> struct AddPointer			{ typedef U* Result; };
template <typename U> struct AddPointer<U&>		{ typedef U* Result; };
template <typename U> struct AddReference		{ typedef U& Result; };
template <typename U> struct AddReference<U&>		{ typedef U& Result; };
template <>           struct AddReference<void>		{ typedef NullType Result; };
template <typename U> struct AddParameterType		{ typedef const U& Result; };
template <typename U> struct AddParameterType<U&>	{ typedef U& Result; };
template <>           struct AddParameterType<void>	{ typedef NullType Result; };
template <typename U> struct RemoveReference		{ typedef U Result; };
template <typename U> struct RemoveReference<U&>	{ typedef U Result; };
template <bool, typename T> struct EnableIf		{ typedef void Result; };
template <typename T> struct EnableIf<true, T>		{ typedef T Result; };


//----------------------------------------------------------------------
// Function pointer testers
//----------------------------------------------------------------------
// Macros expand to numerous parameters

template <typename T>
struct IsFunctionPointerRaw { enum { result = false}; };
template <typename T>
struct IsMemberFunctionPointerRaw { enum { result = false}; };

#define TM_FPR_MAXN		9
#define TM_FPR_TYPE(n)		PASTE(T,n)
#define TM_FPR_TYPENAME(n)	typename TM_FPR_TYPE(n)

// First specialize for regular functions
template <typename T>
struct IsFunctionPointerRaw<T(*)(void)> 
{enum {result = true};};

#define TM_FPR_SPEC(n)		\
template <typename T, COMMA_LIST(n, TM_FPR_TYPENAME)>		\
struct IsFunctionPointerRaw<T(*)(COMMA_LIST(n, TM_FPR_TYPE))>	\
{ enum { result = true }; }

LIST (TM_FPR_MAXN, TM_FPR_SPEC, ;);

// Then for those with an ellipsis argument
template <typename T>
struct IsFunctionPointerRaw<T(*)(...)> 
{enum {result = true};};

#define TM_FPR_SPEC_ELLIPSIS(n)	\
template <typename T, COMMA_LIST(n, TM_FPR_TYPENAME)>			\
struct IsFunctionPointerRaw<T(*)(COMMA_LIST(n, TM_FPR_TYPE), ...)>	\
{ enum { result = true }; }

LIST (TM_FPR_MAXN, TM_FPR_SPEC_ELLIPSIS, ;);

// Then for member function pointers
template <typename T, typename S>
struct IsMemberFunctionPointerRaw<T (S::*)(void)> 
{ enum { result = true }; };

#define TM_MFPR_SPEC(n)		\
template <typename T, typename S, COMMA_LIST(n, TM_FPR_TYPENAME)>	\
struct IsMemberFunctionPointerRaw<T (S::*)(COMMA_LIST(n, TM_FPR_TYPE))>	\
{ enum { result = true };};

LIST (TM_FPR_MAXN, TM_MFPR_SPEC, ;);

// Then for member function pointers with an ellipsis argument
template <typename T, typename S>
struct IsMemberFunctionPointerRaw<T (S::*)(...)> 
{ enum { result = true }; };

#define TM_MFPR_SPEC_ELLIPSIS(n)		\
template <typename T, typename S, COMMA_LIST(n, TM_FPR_TYPENAME)>	\
struct IsMemberFunctionPointerRaw<T (S::*)(COMMA_LIST(n, TM_FPR_TYPE), ...)> \
{ enum { result = true }; };

LIST (TM_FPR_MAXN, TM_MFPR_SPEC_ELLIPSIS, ;);

// Then for const member function pointers (getting tired yet?)
template <typename T, typename S>
struct IsMemberFunctionPointerRaw<T (S::*)(void) const> 
{ enum { result = true }; };

#define TM_CMFPR_SPEC(n)	\
template <typename T, typename S, COMMA_LIST(n, TM_FPR_TYPENAME)>	\
struct IsMemberFunctionPointerRaw<T (S::*)(COMMA_LIST(n, TM_FPR_TYPE)) const>	\
{ enum { result = true };};

LIST (TM_FPR_MAXN, TM_CMFPR_SPEC, ;);

// Finally for const member function pointers with an ellipsis argument (whew!)
template <typename T, typename S>
struct IsMemberFunctionPointerRaw<T (S::*)(...) const> 
{ enum { result = true }; };

#define TM_CMFPR_SPEC_ELLIPSIS(n)		\
template <typename T, typename S, COMMA_LIST(n, TM_FPR_TYPENAME)>	\
struct IsMemberFunctionPointerRaw<T (S::*)(COMMA_LIST(n, TM_FPR_TYPE), ...) const> \
{ enum { result = true }; };

LIST (TM_FPR_MAXN, TM_CMFPR_SPEC_ELLIPSIS, ;);

#undef TM_FPR_SPEC
#undef TM_FPR_SPEC_ELLIPSIS
#undef TM_MFPR_SPEC
#undef TM_MFPR_SPEC_ELLIPSIS
#undef TM_CMFPR_SPEC
#undef TM_CMFPR_SPEC_ELLIPSIS
#undef TM_FPR_TYPENAME
#undef TM_FPR_TYPE
#undef TM_FPR_MAXN

//----------------------------------------------------------------------
// Type traits template
//----------------------------------------------------------------------

/// Figures out at compile time various properties of any given type
/// Invocations (T is a type, TypeTraits<T>::Propertie):
///
/// - isPointer       : returns true if T is a pointer type
/// - PointeeType     : returns the type to which T points if T is a pointer 
///                     type, NullType otherwise
/// - isReference     : returns true if T is a reference type
/// - isLValue        : returns true if T is an lvalue
/// - isRValue        : returns true if T is an rvalue
/// - ReferredType    : returns the type to which T refers if T is a reference 
///                     type, NullType otherwise
/// - isMemberPointer : returns true if T is a pointer to member type
/// - isStdUnsignedInt: returns true if T is a standard unsigned integral type
/// - isStdSignedInt  : returns true if T is a standard signed integral type
/// - isStdIntegral   : returns true if T is a standard integral type
/// - isStdFloat      : returns true if T is a standard floating-point type
/// - isStdArith      : returns true if T is a standard arithmetic type
/// - isStdFundamental: returns true if T is a standard fundamental type
/// - isUnsignedInt   : returns true if T is a unsigned integral type
/// - isSignedInt     : returns true if T is a signed integral type
/// - isIntegral      : returns true if T is a integral type
/// - isFloat         : returns true if T is a floating-point type
/// - isArith         : returns true if T is a arithmetic type
/// - isFundamental   : returns true if T is a fundamental type
/// - ParameterType   : returns the optimal type to be used as a parameter for 
///                     functions that take Ts
/// - isConst         : returns true if T is a const-qualified type
/// - NonConstType    : Type with removed 'const' qualifier from T, if any
/// - isVolatile      : returns true if T is a volatile-qualified type
/// - NonVolatileType : Type with removed 'volatile' qualifier from T, if any
/// - UnqualifiedType : Type with removed 'const' and 'volatile' qualifiers from 
///                     T, if any
/// - ConstParameterType: returns the optimal type to be used as a parameter 
///                       for functions that take 'const T's
///
template <typename T>
class TypeTraits {
private:
    #define TMTT1	template <typename U> struct
    #define TMTT2	template <typename U, typename V> struct
    TMTT1 ReferenceTraits	{ enum { result = false, lvalue = true, rvalue = false }; typedef U ReferredType; };
    TMTT1 ReferenceTraits<U&>	{ enum { result = true,  lvalue = true, rvalue = false }; typedef U ReferredType; };
    TMTT1 PointerTraits		{ enum { result = false }; typedef NullType PointeeType; };
    TMTT1 PointerTraits<U*>	{ enum { result = true  }; typedef U PointeeType; };
    TMTT1 PointerTraits<U*&>	{ enum { result = true  }; typedef U PointeeType; };
    TMTT1 PToMTraits		{ enum { result = false }; };
    TMTT2 PToMTraits<U V::*>	{ enum { result = true  }; };
    TMTT2 PToMTraits<U V::*&>	{ enum { result = true  }; };
    TMTT1 FunctionPointerTraits	{ enum { result = IsFunctionPointerRaw<U>::result }; };
    TMTT1 PToMFunctionTraits	{ enum { result = IsMemberFunctionPointerRaw<U>::result }; };
    TMTT1 UnConst		{ typedef U Result;  enum { isConst = false }; };
    TMTT1 UnConst<const U>	{ typedef U Result;  enum { isConst = true  }; };
    TMTT1 UnConst<const U&>	{ typedef U& Result; enum { isConst = true  }; };
    TMTT1 UnVolatile		{ typedef U Result;  enum { isVolatile = false }; };
    TMTT1 UnVolatile<volatile U>{ typedef U Result;  enum { isVolatile = true  }; };
    TMTT1 UnVolatile<volatile U&> {typedef U& Result;enum { isVolatile = true  }; };
#if HAVE_CPP11
    TMTT1 ReferenceTraits<U&&>	{ enum { result = true,  lvalue = false, rvalue = true }; typedef U ReferredType; };
    TMTT1 PointerTraits<U*&&>	{ enum { result = true  }; typedef U PointeeType; };
    TMTT2 PToMTraits<U V::*&&>	{ enum { result = true  }; };
    TMTT1 UnConst<const U&&>	{ typedef U&& Result; enum { isConst = true  }; };
    TMTT1 UnVolatile<volatile U&&> {typedef U&& Result;enum { isVolatile = true  }; };
#endif
    #undef TMTT2
    #undef TMTT1
public:
    typedef typename UnConst<T>::Result 
	NonConstType;
    typedef typename UnVolatile<T>::Result 
	NonVolatileType;
    typedef typename UnVolatile<typename UnConst<T>::Result>::Result 
	UnqualifiedType;
    typedef typename PointerTraits<UnqualifiedType>::PointeeType 
	PointeeType;
    typedef typename ReferenceTraits<T>::ReferredType 
	ReferredType;

    enum { isConst		= UnConst<T>::isConst };
    enum { isVolatile		= UnVolatile<T>::isVolatile };
    enum { isReference		= ReferenceTraits<UnqualifiedType>::result };
    enum { isLValue		= ReferenceTraits<UnqualifiedType>::lvalue };
    enum { isRValue		= ReferenceTraits<UnqualifiedType>::rvalue };
    enum { isFunction		= FunctionPointerTraits<typename AddPointer<T>::Result >::result };
    enum { isFunctionPointer	= FunctionPointerTraits<
				    typename ReferenceTraits<UnqualifiedType>::ReferredType >::result };
    enum { isMemberFunctionPointer= PToMFunctionTraits<
				    typename ReferenceTraits<UnqualifiedType>::ReferredType >::result };
    enum { isMemberPointer	= PToMTraits<
				    typename ReferenceTraits<UnqualifiedType>::ReferredType >::result ||
				    isMemberFunctionPointer };
    enum { isPointer		= PointerTraits<
				    typename ReferenceTraits<UnqualifiedType>::ReferredType >::result ||
				    isFunctionPointer };
    enum { isStdUnsignedInt	= tl::IndexOf<StdUnsignedInts, UnqualifiedType>::value >= 0 ||
				    tl::IndexOf<StdUnsignedInts,
					typename ReferenceTraits<UnqualifiedType>::ReferredType>::value >= 0};
    enum { isStdSignedInt	= tl::IndexOf<StdSignedInts, UnqualifiedType>::value >= 0 ||
				    tl::IndexOf<StdSignedInts,
					typename ReferenceTraits<UnqualifiedType>::ReferredType>::value >= 0};
    enum { isStdIntegral	= isStdUnsignedInt || isStdSignedInt ||
				    tl::IndexOf<StdOtherInts, UnqualifiedType>::value >= 0 ||
				    tl::IndexOf<StdOtherInts,
					typename ReferenceTraits<UnqualifiedType>::ReferredType>::value >= 0};
    enum { isStdFloat		= tl::IndexOf<StdFloats, UnqualifiedType>::value >= 0 ||
				    tl::IndexOf<StdFloats,
					typename ReferenceTraits<UnqualifiedType>::ReferredType>::value >= 0};
    enum { isStdArith		= isStdIntegral || isStdFloat };
    enum { isStdFundamental	= isStdArith || isStdFloat || Conversion<T, void>::sameType };
	
    enum { isUnsignedInt	= isStdUnsignedInt };
    enum { isUnsigned		= isUnsignedInt || isPointer };
    enum { isSignedInt		= isStdSignedInt };
    enum { isIntegral		= isStdIntegral || isUnsignedInt || isSignedInt };
    enum { isFloat		= isStdFloat };
    enum { isSigned		= isSignedInt || isFloat };
    enum { isArith		= isIntegral || isFloat };
    enum { isFundamental	= isStdFundamental || isArith };
    
    typedef typename Select<isStdArith || isPointer || isMemberPointer, T, 
	    typename AddParameterType<T>::Result>::Result 
	ParameterType;
};

} // namespace tm
} // namespace ustl

#if HAVE_CPP11
#if HAVE_CPP11
namespace ustl {

//{{{ Helper templates and specs ---------------------------------------

/// true or false templatized constant for metaprogramming
template <typename T, T v>
struct integral_constant {
    using value_type = T;
    using type = integral_constant<value_type,v>;
    static constexpr const value_type value = v;
    constexpr operator value_type() const	{ return value; }
    constexpr value_type operator()() const	{ return value; }
};

using true_type = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;

/// Selects type = flag ? T : U
template <bool flag, typename T, typename U> struct conditional { using type = T; };
template <typename T, typename U> struct conditional<false, T, U> { using type = U; };
template <bool flag, typename T, typename U> using conditional_t = typename conditional<flag,T,U>::type;

/// Selects bool = flag ? T : U
template <bool flag, bool T, bool U> struct t_if : public integral_constant<bool, T> {};
template <bool T, bool U> struct t_if<false, T, U> : public integral_constant<bool, U> {};

template <bool, typename T = void> struct enable_if { };
template <typename T> struct enable_if<true, T> { using type = T; };
template <bool flag, typename T> using enable_if_t = typename enable_if<flag,T>::type;

#define UNARY_TRAIT_DEFB(name,condition)	\
template <typename T> struct name : public integral_constant<bool, condition> {}
#define UNARY_TRAIT_DEFN(name)	\
template <typename T> struct name : public false_type {}
#define UNARY_TRAIT_TRUE(name,type)	\
template <> struct name<type> : public true_type {}

//}}}-------------------------------------------------------------------
//{{{ Type modifications

template <typename T> struct remove_const		{ using type = T; };
template <typename T> struct remove_const<T const>	{ using type = T; };
template <typename T> using remove_const_t = typename remove_const<T>::type;

template <typename T> struct remove_volatile		{ using type = T; };
template <typename T> struct remove_volatile<T volatile>{ using type = T; };
template <typename T> using remove_volatile_t = typename remove_volatile<T>::type;

template <typename T> struct remove_cv			{ using type = remove_volatile_t<remove_const_t<T>>; };
template <typename T> using remove_cv_t = typename remove_cv<T>::type;

template <typename T> struct add_const			{ using type = T const; };
template <typename T> struct add_const<const T>		{ using type = T const; };
template <typename T> using add_const_t = typename add_const<T>::type;

template <typename T> struct add_volatile		{ using type = T volatile; };
template <typename T> struct add_volatile<volatile T>	{ using type = T volatile; };
template <typename T> using add_volatile_t = typename add_volatile<T>::type;

template <typename T> struct add_cv			{ using type = add_volatile_t<add_const_t<T>>; };
template <typename T> using add_cv_t = typename add_cv<T>::type;

template <typename T> struct remove_reference		{ using type = T; };
template <typename T> struct remove_reference<T&>	{ using type = T; };
template <typename T> struct remove_reference<T&&>	{ using type = T; };
template <typename T> using remove_reference_t = typename remove_reference<T>::type;

template <typename T> struct remove_pointer		{ using type = T; };
template <typename T> struct remove_pointer<T*>		{ using type = T; };
template <typename T> using remove_pointer_t = typename remove_pointer<T>::type;

template <typename T> struct add_pointer		{ using type = T*; };
template <typename T> struct add_pointer<T*>		{ using type = T*; };
template <typename T> using add_pointer_t = typename add_pointer<T>::type;

template <typename T> struct remove_extent			{ using type = T; };
template <typename T> struct remove_extent<T[]>			{ using type = T; };
template <typename T, size_t N> struct remove_extent<T[N]>	{ using type = T; };
template <typename T> using remove_extent_t = typename remove_extent<T>::type;

template <typename T> struct remove_all_extents			{ using type = T; };
template <typename T> struct remove_all_extents<T[]>		{ using type = typename remove_all_extents<T>::type; };
template <typename T, size_t N> struct remove_all_extents<T[N]>	{ using type = typename remove_all_extents<T>::type; };
template <typename T> using remove_all_extents_t = typename remove_all_extents<T>::type;

template <typename T> struct underlying_type	{ using type = __underlying_type(T); };
template <typename T> using underlying_type_t = typename underlying_type<T>::type;

template <typename T> struct make_signed	{ using type = T; };
template <> struct make_signed<char>		{ using type = signed char; };
template <> struct make_signed<unsigned char>	{ using type = signed char; };
template <> struct make_signed<unsigned short>	{ using type = signed short; };
template <> struct make_signed<unsigned int>	{ using type = signed int; };
template <> struct make_signed<unsigned long>	{ using type = signed long; };
#if HAVE_LONG_LONG
template <> struct make_signed<unsigned long long> { using type = signed long long; };
#endif
template <typename T> using make_signed_t = typename make_signed<T>::type;

template <typename T> struct make_unsigned	{ using type = T; };
template <> struct make_unsigned<char>		{ using type = unsigned char; };
template <> struct make_unsigned<signed char>	{ using type = unsigned char; };
template <> struct make_unsigned<short>		{ using type = unsigned short; };
template <> struct make_unsigned<int>		{ using type = unsigned int; };
template <> struct make_unsigned<long>		{ using type = unsigned long; };
#if HAVE_LONG_LONG
template <> struct make_unsigned<long long>	{ using type = unsigned long long; };
#endif
template <typename T> using make_unsigned_t = typename make_unsigned<T>::type;

//}}}-------------------------------------------------------------------
//{{{ Primary type categories

#if __clang__	// clang already has these __is_ helpers as builtins

UNARY_TRAIT_DEFB (is_void, __is_void(remove_cv_t<T>));
UNARY_TRAIT_DEFB (is_integral, __is_integral(remove_cv_t<T>));
UNARY_TRAIT_DEFB (is_signed, __is_signed(remove_cv_t<T>));
UNARY_TRAIT_DEFB (is_floating_point, __is_floating_point(remove_cv_t<T>));
UNARY_TRAIT_DEFB (is_pointer, __is_pointer(remove_cv_t<T>));
UNARY_TRAIT_DEFB (is_member_pointer, __is_member_pointer(remove_cv_t<T>));
UNARY_TRAIT_DEFB (is_member_function_pointer, __is_member_function_pointer(remove_cv_t<T>));

#else

UNARY_TRAIT_DEFN (__is_void);
UNARY_TRAIT_TRUE (__is_void, void);
UNARY_TRAIT_DEFB (is_void, __is_void<remove_cv_t<T>>::value);

UNARY_TRAIT_DEFN (__is_integral);
UNARY_TRAIT_TRUE (__is_integral, char);
#if HAVE_THREE_CHAR_TYPES
UNARY_TRAIT_TRUE (__is_integral, signed char);
#endif
UNARY_TRAIT_TRUE (__is_integral, short);
UNARY_TRAIT_TRUE (__is_integral, int);
UNARY_TRAIT_TRUE (__is_integral, long);
UNARY_TRAIT_TRUE (__is_integral, unsigned char);
UNARY_TRAIT_TRUE (__is_integral, unsigned short);
UNARY_TRAIT_TRUE (__is_integral, unsigned int);
UNARY_TRAIT_TRUE (__is_integral, unsigned long);
#if HAVE_LONG_LONG
UNARY_TRAIT_TRUE (__is_integral, long long);
UNARY_TRAIT_TRUE (__is_integral, unsigned long long);
#endif
UNARY_TRAIT_TRUE (__is_integral, wchar_t);
UNARY_TRAIT_TRUE (__is_integral, bool);
UNARY_TRAIT_DEFB (is_integral, __is_integral<remove_cv_t<T>>::value);

UNARY_TRAIT_DEFN (__is_signed);
UNARY_TRAIT_TRUE (__is_signed, char);
UNARY_TRAIT_TRUE (__is_signed, wchar_t);
UNARY_TRAIT_TRUE (__is_signed, short);
UNARY_TRAIT_TRUE (__is_signed, int);
UNARY_TRAIT_TRUE (__is_signed, long);
UNARY_TRAIT_TRUE (__is_signed, long long);
UNARY_TRAIT_DEFB (is_signed, __is_signed<remove_cv_t<T>>::value);

UNARY_TRAIT_DEFN (__is_floating_point);
UNARY_TRAIT_TRUE (__is_floating_point, float);
UNARY_TRAIT_TRUE (__is_floating_point, double);
UNARY_TRAIT_TRUE (__is_floating_point, long double);
UNARY_TRAIT_DEFB (is_floating_point, __is_floating_point<remove_cv_t<T>>::value);

template <typename T> struct __is_pointer : public false_type {};
template <typename T> struct __is_pointer<T*> : public true_type {};
template <typename T> struct is_pointer : public __is_pointer<remove_cv_t<T>> {};

UNARY_TRAIT_DEFN (__is_member_pointer);
template <typename T, typename U> struct __is_member_pointer<U T::*> : public true_type {};
UNARY_TRAIT_DEFB (is_member_pointer, __is_member_pointer<remove_cv_t<T>>::value);

UNARY_TRAIT_DEFN (__is_member_function_pointer);
template <typename T, typename R> struct __is_member_function_pointer<R (T::*)(void)> : public true_type {};
template <typename T, typename R> struct __is_member_function_pointer<R (T::*)(...)> : public true_type {};
template <typename T, typename R, typename... Args>
struct __is_member_function_pointer<R (T::*)(Args...)> : public true_type {};
template <typename T, typename R, typename... Args>
struct __is_member_function_pointer<R (T::*)(Args..., ...)> : public true_type {};
UNARY_TRAIT_DEFB (is_member_function_pointer, __is_member_function_pointer<remove_cv_t<T>>::value);

#endif	// __clang__

UNARY_TRAIT_DEFB (is_unsigned, !is_signed<T>::value);
UNARY_TRAIT_DEFB (is_member_object_pointer, is_member_pointer<T>::value && !is_member_function_pointer<T>::value);

UNARY_TRAIT_DEFN (is_array);
UNARY_TRAIT_DEFN (is_lvalue_reference);
template <typename T> struct is_lvalue_reference<T&> : public true_type {};
UNARY_TRAIT_DEFN (is_rvalue_reference);
template <typename T> struct is_rvalue_reference<T&&> : public true_type {};

UNARY_TRAIT_DEFB (is_reference,	is_lvalue_reference<T>::value || is_rvalue_reference<T>::value);

template <typename T> struct is_array<T[]> : public true_type {};
template <typename T, size_t N> struct is_array<T[N]> : public true_type {};

UNARY_TRAIT_DEFB (is_union,	__is_union(T));
UNARY_TRAIT_DEFB (is_class,	__is_class(T));
UNARY_TRAIT_DEFB (is_enum,	__is_enum(T));

UNARY_TRAIT_DEFN (is_function);
template <typename R, typename... Args> struct is_function<R(Args...)> : public true_type { };
template <typename R, typename... Args> struct is_function<R(Args...) &> : public true_type { };
template <typename R, typename... Args> struct is_function<R(Args...) &&> : public true_type { };
template <typename R, typename... Args> struct is_function<R(Args...,...)> : public true_type { };
template <typename R, typename... Args> struct is_function<R(Args...,...) &> : public true_type { };
template <typename R, typename... Args> struct is_function<R(Args...,...) &&> : public true_type { };
template <typename R, typename... Args> struct is_function<R(Args...) const> : public true_type { };
template <typename R, typename... Args> struct is_function<R(Args...) const &> : public true_type { };
template <typename R, typename... Args> struct is_function<R(Args...) const &&> : public true_type { };
template <typename R, typename... Args> struct is_function<R(Args...,...) const> : public true_type { };
template <typename R, typename... Args> struct is_function<R(Args...,...) const &> : public true_type { };
template <typename R, typename... Args> struct is_function<R(Args...,...) const &&> : public true_type { };
template <typename R, typename... Args> struct is_function<R(Args...) volatile> : public true_type { };
template <typename R, typename... Args> struct is_function<R(Args...) volatile &> : public true_type { };
template <typename R, typename... Args> struct is_function<R(Args...) volatile &&> : public true_type { };
template <typename R, typename... Args> struct is_function<R(Args...,...) volatile> : public true_type { };
template <typename R, typename... Args> struct is_function<R(Args...,...) volatile &> : public true_type { };
template <typename R, typename... Args> struct is_function<R(Args...,...) volatile &&> : public true_type { };
template <typename R, typename... Args> struct is_function<R(Args...) const volatile> : public true_type { };
template <typename R, typename... Args> struct is_function<R(Args...) const volatile &> : public true_type { };
template <typename R, typename... Args> struct is_function<R(Args...) const volatile &&> : public true_type { };
template <typename R, typename... Args> struct is_function<R(Args...,...) const volatile> : public true_type { };
template <typename R, typename... Args> struct is_function<R(Args...,...) const volatile &> : public true_type { };
template <typename R, typename... Args> struct is_function<R(Args...,...) const volatile &&> : public true_type { };

UNARY_TRAIT_DEFB (is_object, !is_reference<T>::value && !is_void<T>::value && !is_function<T>::value);
UNARY_TRAIT_DEFB (__is_referenceable, is_reference<T>::value || is_object<T>::value);
template <typename R, typename... Args>
struct __is_referenceable<R(Args...)> : public true_type {};
template <typename R, typename... Args>
struct __is_referenceable<R(Args...,...)> : public true_type {};

//}}}-------------------------------------------------------------------
//{{{ Composite type categories

UNARY_TRAIT_DEFB (is_arithmetic,	is_integral<T>::value || is_floating_point<T>::value);
UNARY_TRAIT_DEFB (is_fundamental,	is_arithmetic<T>::value || is_void<T>::value);
UNARY_TRAIT_DEFB (is_scalar,		is_arithmetic<T>::value || is_enum<T>::value || is_pointer<T>::value || is_member_pointer<T>::value);
UNARY_TRAIT_DEFB (is_compound,		!is_fundamental<T>::value);

template <typename T, bool = __is_referenceable<T>::value> struct __add_lvalue_reference { using type = T; };
template <typename T> struct __add_lvalue_reference<T, true> { using type = T&; };
template <typename T> struct add_lvalue_reference : public __add_lvalue_reference<T> {};
template <typename T> using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

template <typename T, bool = __is_referenceable<T>::value> struct __add_rvalue_reference { using type = T; };
template <typename T> struct __add_rvalue_reference<T, true> { using type = T&&; };
template <typename T> struct add_rvalue_reference : public __add_rvalue_reference<T> {};
template <typename T> using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

/// Unusable default value for T. Use with decltype.
template <typename T> add_rvalue_reference_t<T> declval (void) noexcept;

//}}}-------------------------------------------------------------------
//{{{ Type properties

UNARY_TRAIT_DEFN (is_const);
template <typename T> struct is_const<T const> : public true_type {};
UNARY_TRAIT_DEFN (is_volatile);
template <typename T> struct is_volatile<T volatile> : public true_type {};

UNARY_TRAIT_DEFB (is_empty,		__is_empty(T));
UNARY_TRAIT_DEFB (is_abstract,		__is_abstract(T));
UNARY_TRAIT_DEFB (is_literal_type,	__is_literal_type(T));
UNARY_TRAIT_DEFB (is_polymorphic,	__is_polymorphic(T));
#if HAVE_CPP14
UNARY_TRAIT_DEFB (is_final,		__is_final(T));
#endif
UNARY_TRAIT_DEFB (is_standard_layout,	__is_standard_layout(T));
UNARY_TRAIT_DEFB (is_pod,		__is_pod(T) || is_scalar<T>::value || (is_array<T>::value && is_scalar<remove_all_extents_t<T>>::value));
UNARY_TRAIT_DEFB (is_trivial,		is_pod<T>::value || __is_trivial(T));
UNARY_TRAIT_DEFB (has_trivial_copy,	is_pod<T>::value || __has_trivial_copy(T));
UNARY_TRAIT_DEFB (has_trivial_assign,	is_pod<T>::value || __has_trivial_assign(T));
UNARY_TRAIT_DEFB (has_trivial_constructor,	is_pod<T>::value || __has_trivial_constructor(T));
UNARY_TRAIT_DEFB (has_trivial_destructor,	is_pod<T>::value || __has_trivial_destructor(T));
UNARY_TRAIT_DEFB (has_virtual_destructor,	__has_virtual_destructor(T));
UNARY_TRAIT_DEFB (has_nothrow_assign,		__has_nothrow_assign(T));
UNARY_TRAIT_DEFB (has_nothrow_copy,		__has_nothrow_copy(T));
UNARY_TRAIT_DEFB (has_nothrow_constructor,	__has_nothrow_constructor(T));

template <typename T> struct alignment_of : public integral_constant<size_t, alignof(T)> {};

template <typename T> struct rank		: public integral_constant<size_t, 0> {};
template <typename T> struct rank<T[]>		: public integral_constant<size_t, 1 + rank<T>::value> {};
template <typename T, size_t N> struct rank<T[N]> : public integral_constant<size_t, 1 + rank<T>::value> {};

template <typename T, unsigned I = 0> struct extent		{ static constexpr const size_t value = 0; };
template <typename T, unsigned I> struct extent<T[],I>		{ static constexpr const size_t value = I ? extent<T,I-1>::value : 0; };
template <typename T, unsigned I, size_t N> struct extent<T[N],I> { static constexpr const size_t value = I ? extent<T,I-1>::value : N; };

template <typename T, bool IsArray = is_array<T>::value, bool IsFunction = is_function<T>::value> struct __decay;
template <typename T> struct __decay<T, false, false>	{ using type = remove_cv_t<T>; };
template <typename T> struct __decay<T, true, false>	{ using type = remove_extent_t<T>*; };
template <typename T> struct __decay<T, false, true>	{ using type = add_pointer_t<T>; };
template <typename T> struct decay : public __decay<remove_reference_t<T>> {};
template <typename T> using decay_t = typename decay<T>::type;

//}}}-------------------------------------------------------------------
//{{{ Constructability and destructability

// All these use the standard SFINAE technique
struct __is_destructible {
    template <typename T, typename = decltype(declval<T&>().~T())> static true_type test (int);
    template <typename T> static false_type test (...);
};
template <typename T> struct is_destructible : public decltype(__is_destructible::test<T>(0)) {};

struct __is_nothrow_destructible {
    template <typename T> static integral_constant<bool, noexcept(declval<T&>().~T())> test (int);
    template<typename> static false_type test (...);
};
template<typename T>
struct is_nothrow_destructible : public decltype(__is_nothrow_destructible::test<T>(0)) {};

struct __is_default_constructible {
    template <typename T, typename = decltype(T())> static true_type test (int);
    template <typename T> static false_type test (...);
};
template <typename T> struct is_default_constructible : public decltype(__is_default_constructible::test<T>(0)) {};

struct __is_nothrow_default_constructible {
    template <typename T, typename = decltype(T())> static integral_constant<bool, noexcept(T())> test (int);
    template <typename T> static false_type test (...);
};
template <typename T> struct is_nothrow_default_constructible : public decltype(__is_nothrow_default_constructible::test<T>(0)) {};

template <typename T> struct is_constructible : public is_default_constructible<T> {};
template <typename T> struct is_nothrow_constructible : public is_nothrow_default_constructible<T> {};

struct __is_copy_constructible {
    template <typename T, typename = decltype(T(declval<T&>()))> static true_type test (int);
    template <typename T> static false_type test (...);
};
template <typename T> struct is_copy_constructible : public decltype(__is_copy_constructible::test<T>(0)) {};

struct __is_move_constructible {
    template <typename T, typename = decltype(T(declval<T&&>()))> static true_type test (int);
    template <typename T> static false_type test (...);
};
template <typename T> struct is_move_constructible : public decltype(__is_move_constructible::test<T>(0)) {};

struct __is_assignable {
    template <typename T, typename U, typename = decltype(declval<T>() = declval<U>())> static true_type test (int);
    template <typename T, typename U> static false_type test (...);
};
template <typename T, typename U> struct is_assignable : public decltype(__is_assignable::test<T,U>(0)) {};

template <typename T> struct is_copy_assignable : public is_assignable<T&, const T&> {};
template <typename T> struct is_move_assignable : public is_assignable<T&, T&&> {};

// TODO: later
template <typename T> struct is_nothrow_copy_constructible : public false_type {};
template <typename T> struct is_nothrow_move_constructible : public false_type {};
template <typename T, typename U> struct is_nothrow_assignable : public false_type {};
template <typename T> struct is_nothrow_copy_assignable : public false_type {};
template <typename T> struct is_nothrow_move_assignable : public false_type {};

#if __GNUC__ >= 5
UNARY_TRAIT_DEFB (is_trivially_copyable,	__is_trivially_copyable(T));
template <typename T, typename... Args>
struct is_trivially_constructible : public integral_constant<bool, __is_trivially_constructible(T, Args...)> {};

template<typename T>
struct is_trivially_copy_constructible : public
     integral_constant<bool, is_copy_constructible<T>::value
				 && __is_trivially_constructible(T, const T&)> {};

template<typename T>
struct is_trivially_move_constructible : public
     integral_constant<bool, is_move_constructible<T>::value
				 && __is_trivially_constructible(T, T&&)> {};

template<typename T, typename U>
struct is_trivially_assignable : public integral_constant<bool, __is_trivially_assignable(T, U)> {};

UNARY_TRAIT_DEFB (is_trivially_default_constructible,	__is_trivially_constructible(T));
UNARY_TRAIT_DEFB (is_trivially_copy_assignable,	__is_trivially_assignable(T,const T&));
UNARY_TRAIT_DEFB (is_trivially_move_assignable,	__is_trivially_assignable(T,T&&));
#else
UNARY_TRAIT_DEFB (is_trivially_copyable,		__has_trivial_copy(T));
UNARY_TRAIT_DEFB (is_trivially_default_constructible,	__has_trivial_constructor(T));
UNARY_TRAIT_DEFB (is_trivially_copy_assignable,		__has_trivial_assign(T));
UNARY_TRAIT_DEFB (is_trivially_move_assignable,		false);
#endif

UNARY_TRAIT_DEFB (is_trivially_destructible,	__has_trivial_destructor(T));
UNARY_TRAIT_DEFB (has_trivial_copy_constructor,	__has_trivial_copy(T));
UNARY_TRAIT_DEFB (has_trivial_copy_assign,	__has_trivial_assign(T));

//}}}-------------------------------------------------------------------
//{{{ Type relations

template <typename T, typename U> struct is_same : public false_type {};
template <typename T> struct is_same<T,T> : public true_type {};

#if __clang__	// clang has __is_convertible builtin

template <typename F, typename T>
struct is_convertible : public integral_constant<bool, __is_convertible(F, T)> {};

#else

template <typename F, typename T, bool = is_void<F>::value || is_function<T>::value || is_array<T>::value>
class __is_convertible : public integral_constant<bool, is_void<T>::value> {};
template <typename F, typename T>
class __is_convertible<F, T, false> {
    template <typename TT> static void __test_aux(TT);
    template <typename FF, typename TT, typename = decltype(__test_aux<TT>(declval<FF>()))>
					static true_type __test(int);
    template <typename FF, typename TT> static false_type __test(...);
public:
    using type = decltype(__test<F, T>(0));
};
template <typename F, typename T>
struct is_convertible : public __is_convertible<F, T>::type {};

#endif

/// Defines a has_member_function_name template where has_member_function_name<O>::value is true when O::name exists
/// Example: HAS_MEMBER_FUNCTION(read, void (O::*)(istream&)); has_member_function_read<vector<int>>::value == true
#define HAS_MEMBER_FUNCTION(name, signature)	\
template <typename T>				\
class __has_member_function_##name {		\
    template <typename O, signature> struct test_for_##name {};\
    template <typename O> static true_type found (test_for_##name<O,&O::name>*);\
    template <typename O> static false_type found (...);\
public:						\
    using type = decltype(found<T>(nullptr));	\
};						\
template <typename T>				\
struct has_member_function_##name : public __has_member_function_##name<T>::type {}

/// Defines a has_static_member_variable template where has_static_member_variable_name<O>::value is true when O::name exists
/// Example: HAS_STATIC_MEMBER_VARIABLE(int, _val); has_static_member_variable__val<A>::value == true
#define HAS_STATIC_MEMBER_VARIABLE(varT,name)	\
template <typename T>				\
class __has_static_member_variable_##name {	\
    template <typename O, add_pointer_t<decay_t<varT>> V> struct test_for_##name {};\
    template <typename O> static true_type found (test_for_##name<O,&O::name>*);\
    template <typename O> static false_type found (...);\
public:						\
    using type = decltype(found<T>(nullptr));	\
};						\
template <typename T>				\
struct has_static_member_variable_##name : public __has_static_member_variable_##name<T>::type {}

template <typename T, typename U> struct is_base_of {
    static constexpr const bool value = __is_base_of(T,U);
};
template <typename T> struct is_base_of<T,T> : public false_type {};
template <typename T> struct is_base_of<void,T> : public false_type {};

template <size_t Size, size_t Grain> struct aligned_storage
    { struct type { alignas(Grain) unsigned char _data[Size]; }; };

//}}}-------------------------------------------------------------------
//{{{ Helper templates and specs
#undef UNARY_TRAIT_DEFN
#undef UNARY_TRAIT_DEFB
#define POD_CLASS(T)			namespace ustl { UNARY_TRAIT_TRUE(is_pod,T); }
//}}}

} // namespace ustl
#endif // HAVE_CPP11
#endif

namespace ustl {

namespace {
template <typename T> struct __limits_digits { enum { value = sizeof(T)*8 };};
template <typename T> struct __limits_digits10 { enum { value = sizeof(T)*8*643/2136+1 };};
}

/// \class numeric_limits ulimits.h ustl.h
/// \brief Defines numeric limits for a type.
///
template <typename T> 
struct numeric_limits {
    static inline constexpr T min (void)	{ return T(0); }	// Returns the minimum value for type T.
    static inline constexpr T max (void)	{ return T(0); }	// Returns the minimum value for type T.
    static const bool is_signed = tm::TypeTraits<T>::isSigned;		///< True if the type is signed.
    static const bool is_integer = tm::TypeTraits<T>::isIntegral;	///< True if stores an exact value.
    static const bool is_integral = tm::TypeTraits<T>::isFundamental;	///< True if fixed size and cast-copyable.
    static const unsigned digits = __limits_digits<T>::value;		///< Number of bits in T
    static const unsigned digits10 = __limits_digits10<T>::value;	///< Maximum number of decimal digits in printed version of T
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS

template <typename T>
struct numeric_limits<T*> {
    static inline constexpr T* min (void)	{ return nullptr; }
    static inline constexpr T* max (void)	{ return reinterpret_cast<T*>(UINTPTR_MAX); }
    static const bool is_signed = false;
    static const bool is_integer = true;
    static const bool is_integral = true;
    static const unsigned digits = __limits_digits<T*>::value;
    static const unsigned digits10 = __limits_digits10<T*>::value;
};

#define _NUMERIC_LIMITS(type, minVal, maxVal, bSigned, bInteger, bIntegral)	\
template <>								\
struct numeric_limits<type> {						\
    static inline constexpr type min (void)	{ return minVal; }	\
    static inline constexpr type max (void)	{ return maxVal; }	\
    static const bool is_signed = bSigned;				\
    static const bool is_integer = bInteger;				\
    static const bool is_integral = bIntegral;				\
    static const unsigned digits = __limits_digits<type>::value;	\
    static const unsigned digits10 = __limits_digits10<type>::value;	\
}

//--------------------------------------------------------------------------------------
//		type		min		max		signed	integer	integral
//--------------------------------------------------------------------------------------
_NUMERIC_LIMITS (bool,		false,		true,		false,	true,	true);
_NUMERIC_LIMITS (char,		CHAR_MIN,	CHAR_MAX,	true,	true,	true);
_NUMERIC_LIMITS (int,		INT_MIN,	INT_MAX,	true,	true,	true);
_NUMERIC_LIMITS (short,		SHRT_MIN,	SHRT_MAX,	true,	true,	true);
_NUMERIC_LIMITS (long,		LONG_MIN,	LONG_MAX,	true,	true,	true);
#if HAVE_THREE_CHAR_TYPES
_NUMERIC_LIMITS (signed char,	SCHAR_MIN,	SCHAR_MAX,	true,	true,	true);
#endif
_NUMERIC_LIMITS (unsigned char,	0,		UCHAR_MAX,	false,	true,	true);
_NUMERIC_LIMITS (unsigned int,	0,		UINT_MAX,	false,	true,	true);
_NUMERIC_LIMITS (unsigned short,0,		USHRT_MAX,	false,	true,	true);
_NUMERIC_LIMITS (unsigned long,	0,		ULONG_MAX,	false,	true,	true);
_NUMERIC_LIMITS (wchar_t,	0,		WCHAR_MAX,	false,	true,	true);
_NUMERIC_LIMITS (float,		FLT_MIN,	FLT_MAX,	true,	false,	true);
_NUMERIC_LIMITS (double,	DBL_MIN,	DBL_MAX,	true,	false,	true);
_NUMERIC_LIMITS (long double,	LDBL_MIN,	LDBL_MAX,	true,	false,	true);
#if HAVE_LONG_LONG
_NUMERIC_LIMITS (long long,	LLONG_MIN,	LLONG_MAX,	true,	true,	true);
_NUMERIC_LIMITS (unsigned long long,	0,	ULLONG_MAX,	false,	true,	true);
#endif
//--------------------------------------------------------------------------------------

#endif // DOXYGEN_SHOULD_SKIP_THIS

/// Macro for defining numeric_limits specializations
#define NUMERIC_LIMITS(type, minVal, maxVal, bSigned, bInteger, bIntegral)	\
namespace ustl { _NUMERIC_LIMITS (type, minVal, maxVal, bSigned, bInteger, bIntegral); }

} // namespace ustl

namespace ustl {

#if HAVE_CPP11
    using nullptr_t	= decltype(nullptr);
#endif

#if __GNUC__
    /// Returns the number of elements in a static vector
    #define VectorSize(v)	(sizeof(v) / sizeof(*v))
#else
    // Old compilers will not be able to evaluate *v on an empty vector.
    // The tradeoff here is that VectorSize will not be able to measure arrays of local structs.
    #define VectorSize(v)	(sizeof(v) / ustl::size_of_elements(1, v))
#endif

/// Returns the end() for a static vector
template <typename T, size_t N> inline constexpr T* VectorEnd (T(&a)[N]) { return &a[N]; }

/// Expands into a ptr,size expression for the given static vector; useful as link arguments.
#define VectorBlock(v)	&(v)[0], VectorSize(v)
/// Expands into a begin,end expression for the given static vector; useful for algorithm arguments.
#define VectorRange(v)	&(v)[0], VectorEnd(v)

/// Returns the number of bits in the given type
#define BitsInType(t)	(sizeof(t) * CHAR_BIT)

/// Returns the mask of type \p t with the lowest \p n bits set.
#define BitMask(t,n)	(t(~t(0)) >> (BitsInType(t) - (n)))

/// Argument that is used only in debug builds (as in an assert)
#ifndef NDEBUG
    #define DebugArg(x)	x
#else
    #define DebugArg(x)
#endif

/// Shorthand for container iteration.
#define foreach(type,i,ctr)	for (type i = (ctr).begin(); i != (ctr).end(); ++ i)
/// Shorthand for container reverse iteration.
#define eachfor(type,i,ctr)	for (type i = (ctr).rbegin(); i != (ctr).rend(); ++ i)

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Macro for passing template types as macro arguments.
// These are deprecated. Use metamac macros instead. Will remove by next release.
#define TEMPLATE_FULL_DECL1(d1,t1)		template <d1 t1>
#define TEMPLATE_FULL_DECL2(d1,t1,d2,t2)	template <d1 t1, d2 t2>
#define TEMPLATE_FULL_DECL3(d1,t1,d2,t2,d3,t3)	template <d1 t1, d2 t2, d3 t3>
#define TEMPLATE_DECL1(t1)		TEMPLATE_FULL_DECL1(typename,t1)
#define TEMPLATE_DECL2(t1,t2)		TEMPLATE_FULL_DECL2(typename,t1,typename,t2)
#define TEMPLATE_DECL3(t1,t2,t3)	TEMPLATE_FULL_DECL3(typename,t1,typename,t2,typename,t3)
#define TEMPLATE_TYPE1(type,a1)		type<a1>
#define TEMPLATE_TYPE2(type,a1,a2)	type<a1,a2>
#define TEMPLATE_TYPE3(type,a1,a2,a3)	type<a1,a2,a3>
#endif

/// Returns the minimum of \p a and \p b
template <typename T1, typename T2>
inline constexpr T1 min (const T1& a, const T2& b)
{
    return a < b ? a : b;
}

/// Returns the maximum of \p a and \p b
template <typename T1, typename T2>
inline constexpr T1 max (const T1& a, const T2& b)
{
    return b < a ? a : b;
}

/// Indexes into a static array with bounds limit
template <typename T, size_t N>
inline constexpr T& VectorElement (T(&v)[N], size_t i) { return v[min(i,N-1)]; }

/// The alignment performed by default.
const size_t c_DefaultAlignment = __alignof__(void*);

/// \brief Rounds \p n up to be divisible by \p grain
template <typename T>
inline constexpr T AlignDown (T n, size_t grain = c_DefaultAlignment)
    { return n - n % grain; }

/// \brief Rounds \p n up to be divisible by \p grain
template <typename T>
inline constexpr T Align (T n, size_t grain = c_DefaultAlignment)
    { return AlignDown (n + grain - 1, grain); }

/// Returns a nullptr pointer cast to T.
template <typename T>
inline constexpr T* NullPointer (void)
    { return nullptr; }

/// \brief Returns a non-dereferentiable value reference.
/// This is useful for passing to stream_align_of or the like which need a value but
/// don't need to actually use it.
template <typename T>
inline constexpr T& NullValue (void)
    { return *NullPointer<T>(); }

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Offsets a pointer
template <typename T>
inline T advance_ptr (T i, ptrdiff_t offset)
    { return i + offset; }

// Offsets a void pointer
template <>
inline const void* advance_ptr (const void* p, ptrdiff_t offset)
{
    assert (p || !offset);
    return reinterpret_cast<const uint8_t*>(p) + offset;
}

// Offsets a void pointer
template <>
inline void* advance_ptr (void* p, ptrdiff_t offset)
{
    assert (p || !offset);
    return reinterpret_cast<uint8_t*>(p) + offset;
}
#endif

/// Offsets an iterator
template <typename T, typename Distance>
inline T advance (T i, Distance offset)
    { return advance_ptr (i, offset); }

/// Returns the difference \p p1 - \p p2
template <typename T1, typename T2>
inline constexpr ptrdiff_t distance (T1 i1, T2 i2)
{
    return i2 - i1;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define UNVOID_DISTANCE(T1const,T2const)   \
template <> inline constexpr ptrdiff_t distance (T1const void* p1, T2const void* p2) \
{ return (T2const uint8_t*)(p2) - (T1const uint8_t*)(p1); }
UNVOID_DISTANCE(,)
UNVOID_DISTANCE(const,const)
UNVOID_DISTANCE(,const)
UNVOID_DISTANCE(const,)
#undef UNVOID_DISTANCE
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// The compiler issues a warning if an unsigned type is compared to 0.
template <typename T, bool IsSigned> struct __is_negative { inline constexpr bool operator()(const T& v) const { return v < 0; } };
template <typename T> struct __is_negative<T,false> { inline constexpr bool operator()(const T&) const { return false; } };
/// Warning-free way to check if \p v is negative, even if for unsigned types.
template <typename T> inline constexpr bool is_negative (const T& v) { return __is_negative<T,numeric_limits<T>::is_signed>()(v); }
#endif

/// \brief Returns the absolute value of \p v
/// Unlike the stdlib functions, this is inline and works with all types.
template <typename T>
inline constexpr T absv (T v)
{
    return is_negative(v) ? -v : v;
}

/// \brief Returns -1 for negative values, 1 for positive, and 0 for 0
template <typename T>
inline constexpr T sign (T v)
{
    return (0 < v) - is_negative(v);
}

/// Returns the absolute value of the distance i1 and i2
template <typename T1, typename T2>
inline constexpr size_t abs_distance (T1 i1, T2 i2)
{
    return absv (distance(i1, i2));
}

/// Returns the size of \p n elements of size \p T
template <typename T>
inline constexpr size_t size_of_elements (size_t n, const T*)
{
    return n * sizeof(T);
}

// Defined in byteswap.h, which is usually unusable.
#undef bswap_16
#undef bswap_32
#undef bswap_64

inline uint16_t bswap_16 (uint16_t v)
{
#if CPU_HAS_CMPXCHG8	// If it has that, it has bswap.
    if (!__builtin_constant_p(v)) asm ("rorw $8, %0":"+r"(v)); else
#endif
	v = v << 8 | v >> 8;
    return v;
}
inline uint32_t bswap_32 (uint32_t v)
{
#if CPU_HAS_CMPXCHG8
    if (!__builtin_constant_p(v)) asm ("bswap %0":"+r"(v)); else
#endif
	v = v << 24 | (v & 0xFF00) << 8 | ((v >> 8) & 0xFF00) | v >> 24;
    return v;
}
#if HAVE_INT64_T
inline uint64_t bswap_64 (uint64_t v)
{
#if x86_64
    if (!__builtin_constant_p(v)) asm ("bswap %0":"+r"(v)); else
#endif
	v = (uint64_t(bswap_32(v)) << 32) | bswap_32(v >> 32);
    return v;
}
#endif

/// \brief Swaps the byteorder of \p v.
template <typename T>
inline T bswap (const T& v)
{
    switch (BitsInType(T)) {
	default:	return v;
	case 16:	return T (bswap_16 (uint16_t (v)));
	case 32:	return T (bswap_32 (uint32_t (v)));
#if HAVE_INT64_T
	case 64:	return T (bswap_64 (uint64_t (v)));
#endif
    }
}

#if BYTE_ORDER == BIG_ENDIAN
template <typename T> inline T le_to_native (const T& v) { return bswap (v); }
template <typename T> inline T be_to_native (const T& v) { return v; }
template <typename T> inline T native_to_le (const T& v) { return bswap (v); }
template <typename T> inline T native_to_be (const T& v) { return v; }
#elif BYTE_ORDER == LITTLE_ENDIAN
template <typename T> inline T le_to_native (const T& v) { return v; }
template <typename T> inline T be_to_native (const T& v) { return bswap (v); }
template <typename T> inline T native_to_le (const T& v) { return v; }
template <typename T> inline T native_to_be (const T& v) { return bswap (v); }
#endif // BYTE_ORDER

/// Deletes \p p and sets it to nullptr
template <typename T>
inline void Delete (T*& p)
{
    delete p;
    p = nullptr;
}

/// Deletes \p p as an array and sets it to nullptr
template <typename T>
inline void DeleteVector (T*& p)
{
    delete [] p;
    p = nullptr;
}

/// Template of making != from ! and ==
template <typename T>
inline constexpr bool operator!= (const T& x, const T& y)
    { return !(x == y); }

/// Template of making > from <
template <typename T>
inline constexpr bool operator> (const T& x, const T& y)
    { return y < x; }

/// Template of making <= from < and ==
template <typename T>
inline constexpr bool operator<= (const T& x, const T& y)
    { return !(y < x); }

/// Template of making >= from < and ==
template <typename T>
inline constexpr bool operator>= (const T& x, const T& y)
    { return !(x < y); }

/// Packs \p s multiple times into \p b. Useful for loop unrolling.
template <typename TSmall, typename TBig>
inline void pack_type (TSmall s, TBig& b)
{
    b = s;
    for (unsigned h = BitsInType(TSmall); h < BitsInType(TBig); h *= 2)
	b = (b << h)|b;
}

/// \brief Divides \p n1 by \p n2 and rounds the result up.
/// This is in contrast to regular division, which rounds down.
template <typename T1, typename T2>
inline T1 DivRU (T1 n1, T2 n2)
{
    T2 adj = n2 - 1;
    if (is_negative (n1))
	adj = -adj;
    return (n1 + adj) / n2;
}

/// Sets the contents of \p pm to 1 and returns true if the previous value was 0.
inline bool TestAndSet (int* pm)
{
#if CPU_HAS_CMPXCHG8
    bool rv;
    int oldVal (1);
    asm volatile ( // cmpxchg compares to %eax and swaps if equal
	"cmpxchgl %3, %1\n\t"
	"sete %0"
	: "=a" (rv), "=m" (*pm), "=r" (oldVal)
	: "2" (oldVal), "a" (0)
	: "memory");
    return rv;
#elif __i386__ || __x86_64__
    int oldVal (1);
    asm volatile ("xchgl %0, %1" : "=r"(oldVal), "=m"(*pm) : "0"(oldVal), "m"(*pm) : "memory");
    return !oldVal;
#elif __sparc32__	// This has not been tested
    int rv;
    asm volatile ("ldstub %1, %0" : "=r"(rv), "=m"(*pm) : "m"(pm));
    return !rv;
#else
    const int oldVal (*pm);
    *pm = 1;
    return !oldVal;
#endif
}

/// Returns the index of the first set bit in \p v or \p nbv if none.
inline uoff_t FirstBit (uint32_t v, uoff_t nbv)
{
    uoff_t n = nbv;
#if __i386__ || __x86_64__
    if (!__builtin_constant_p(v)) asm ("bsr\t%1, %k0":"+r,r"(n):"r,m"(v)); else
#endif
#if __GNUC__
    if (v) n = 31 - __builtin_clz(v);
#else
    if (v) for (uint32_t m = uint32_t(1)<<(n=31); !(v & m); m >>= 1) --n;
#endif
    return n;
}
/// Returns the index of the first set bit in \p v or \p nbv if none.
inline uoff_t FirstBit (uint64_t v, uoff_t nbv)
{
    uoff_t n = nbv;
#if __x86_64__
    if (!__builtin_constant_p(v)) asm ("bsr\t%1, %0":"+r,r"(n):"r,m"(v)); else
#endif
#if __GNUC__ && SIZE_OF_LONG >= 8
    if (v) n = 63 - __builtin_clzl(v);
#elif __GNUC__ && HAVE_LONG_LONG && SIZE_OF_LONG_LONG >= 8
    if (v) n = 63 - __builtin_clzll(v);
#else
    if (v) for (uint64_t m = uint64_t(1)<<(n=63); !(v & m); m >>= 1) --n;
#endif
    return n;
}

/// Returns the next power of 2 >= \p v.
/// Values larger than UINT32_MAX/2 will return 2^0
inline uint32_t NextPow2 (uint32_t v)
{
    uint32_t r = v-1;
#if __i386__ || __x86_64__
    if (!__builtin_constant_p(r)) asm("bsr\t%0, %0":"+r"(r)); else
#endif
    { r = FirstBit(r,r); if (r >= BitsInType(r)-1) r = uint32_t(-1); }
    return 1<<(1+r);
}

/// Bitwise rotate value left
template <typename T>
inline T Rol (T v, size_t n)
{
#if __i386__ || __x86_64__
    if (!(__builtin_constant_p(v) && __builtin_constant_p(n))) asm("rol\t%b1, %0":"+r,r"(v):"i,c"(n)); else
#endif
    v = (v << n) | (v >> (BitsInType(T)-n));
    return v;
}

/// Bitwise rotate value right
template <typename T>
inline T Ror (T v, size_t n)
{
#if __i386__ || __x86_64__
    if (!(__builtin_constant_p(v) && __builtin_constant_p(n))) asm("ror\t%b1, %0":"+r,r"(v):"i,c"(n)); else
#endif
    v = (v >> n) | (v << (BitsInType(T)-n));
    return v;
}

/// \brief This template is to be used for dereferencing a type-punned pointer without a warning.
///
/// When casting a local variable to an unrelated type through a pointer (for
/// example, casting a float to a uint32_t without conversion), the resulting
/// memory location can be accessed through either pointer, which violates the
/// strict aliasing rule. While -fno-strict-aliasing option can be given to
/// the compiler, eliminating this warning, inefficient code may result in
/// some instances, because aliasing inhibits some optimizations. By using
/// this template, and by ensuring the memory is accessed in one way only,
/// efficient code can be produced without the warning. For gcc 4.1.0+.
///
template <typename DEST, typename SRC>
inline DEST noalias (const DEST&, SRC* s)
{
    asm("":"+g"(s)::"memory");
    union UPun { SRC s; DEST d; };
    return ((UPun*)(s))->d;
}

template <typename DEST, typename SRC>
inline DEST noalias_cast (SRC s)
{
    asm("":"+g"(s)::"memory");
    union { SRC s; DEST d; } u = {s};
    return u.d;
}

namespace simd {
    /// Call after you are done using SIMD algorithms for 64 bit tuples.
    #define ALL_MMX_REGS_CHANGELIST "mm0","mm1","mm2","mm3","mm4","mm5","mm6","mm7","st","st(1)","st(2)","st(3)","st(4)","st(5)","st(6)","st(7)"
#if CPU_HAS_3DNOW
    inline void reset_mmx (void) { asm ("femms":::ALL_MMX_REGS_CHANGELIST); }
#elif CPU_HAS_MMX
    inline void reset_mmx (void) { asm ("emms":::ALL_MMX_REGS_CHANGELIST); }
#else
    inline void reset_mmx (void) {}
#endif
} // namespace simd
} // namespace ustl

namespace ustl {

#if HAVE_CPP11

template <typename T>
inline constexpr typename tm::RemoveReference<T>::Result&& move (T&& v) noexcept
    { return static_cast<typename tm::RemoveReference<T>::Result&&>(v); }

template <typename T>
inline constexpr T&& forward (typename tm::RemoveReference<T>::Result& v) noexcept
    { return static_cast<T&&>(v); }

template <typename T>
inline constexpr T&& forward (typename tm::RemoveReference<T>::Result&& v) noexcept
    { return static_cast<T&&>(v); }

template <typename Assignable> 
inline void swap (Assignable& a, Assignable& b)
{
    Assignable tmp = move(a);
    a = move(b);
    b = move(tmp);
}

#else

/// Assigns the contents of a to b and the contents of b to a.
/// This is used as a primitive operation by many other algorithms. 
/// \ingroup SwapAlgorithms
///
template <typename Assignable> 
inline void swap (Assignable& a, Assignable& b)
{
    Assignable tmp = a;
    a = b;
    b = tmp;
}

#endif

/// Equivalent to swap (*a, *b)
/// \ingroup SwapAlgorithms
///
template <typename Iterator> 
inline void iter_swap (Iterator a, Iterator b)
{
    swap (*a, *b);
}

/// Copy copies elements from the range [first, last) to the range
/// [result, result + (last - first)). That is, it performs the assignments
/// *result = *first, *(result + 1) = *(first + 1), and so on. [1] Generally,
/// for every integer n from 0 to last - first, copy performs the assignment
/// *(result + n) = *(first + n). Assignments are performed in forward order,
/// i.e. in order of increasing n. 
/// \ingroup MutatingAlgorithms
///
template <typename InputIterator, typename OutputIterator>
inline OutputIterator copy (InputIterator first, InputIterator last, OutputIterator result)
{
    for (; first != last; ++result, ++first)
	*result = *first;
    return result;
}

/// Copy_n copies elements from the range [first, first + n) to the range
/// [result, result + n). That is, it performs the assignments
/// *result = *first, *(result + 1) = *(first + 1), and so on. Generally,
/// for every integer i from 0 up to (but not including) n, copy_n performs
/// the assignment *(result + i) = *(first + i). Assignments are performed
/// in forward order, i.e. in order of increasing n.
/// \ingroup MutatingAlgorithms
///
template <typename InputIterator, typename OutputIterator>
inline OutputIterator copy_n (InputIterator first, size_t count, OutputIterator result)
{
    for (; count; --count, ++result, ++first)
	*result = *first;
    return result;
}

/// \brief Copy copies elements from the range (last, first] to result.
/// \ingroup MutatingAlgorithms
/// Copies elements starting at last, decrementing both last and result.
///
template <typename InputIterator, typename OutputIterator>
inline OutputIterator copy_backward (InputIterator first, InputIterator last, OutputIterator result)
{
    while (first != last)
	*--result = *--last;
    return result;
}

/// For_each applies the function object f to each element in the range
/// [first, last); f's return value, if any, is ignored. Applications are
/// performed in forward order, i.e. from first to last. For_each returns
/// the function object after it has been applied to each element.
/// \ingroup MutatingAlgorithms
///
template <typename InputIterator, typename UnaryFunction>
inline UnaryFunction for_each (InputIterator first, InputIterator last, UnaryFunction f)
{
    for (; first != last; ++first)
	f (*first);
    return f;
}

/// Fill assigns the value value to every element in the range [first, last).
/// That is, for every iterator i in [first, last),
/// it performs the assignment *i = value.
/// \ingroup GeneratorAlgorithms
///
template <typename ForwardIterator, typename T>
inline void fill (ForwardIterator first, ForwardIterator last, const T& value)
{
    for (; first != last; ++first)
	*first = value;
}

/// Fill_n assigns the value value to every element in the range
/// [first, first+count). That is, for every iterator i in [first, first+count),
/// it performs the assignment *i = value. The return value is first + count.
/// \ingroup GeneratorAlgorithms
///
template <typename OutputIterator, typename T>
inline OutputIterator fill_n (OutputIterator first, size_t count, const T& value)
{
    for (; count; --count, ++first)
	*first = value;
    return first;
}

#if CPU_HAS_MMX
extern "C" void copy_n_fast (const void* src, size_t count, void* dest) noexcept;
#else
inline void copy_n_fast (const void* src, size_t count, void* dest) noexcept
    { memcpy (dest, src, count); }
#endif
#if __i386__ || __x86_64__
extern "C" void copy_backward_fast (const void* first, const void* last, void* result) noexcept;
#else
inline void copy_backward_fast (const void* first, const void* last, void* result) noexcept
{
    const size_t nBytes (distance (first, last));
    memmove (advance (result, -nBytes), first, nBytes);
}
#endif
extern "C" void fill_n8_fast (uint8_t* dest, size_t count, uint8_t v) noexcept;
extern "C" void fill_n16_fast (uint16_t* dest, size_t count, uint16_t v) noexcept;
extern "C" void fill_n32_fast (uint32_t* dest, size_t count, uint32_t v) noexcept;
extern "C" void rotate_fast (void* first, void* middle, void* last) noexcept;

#if __GNUC__ >= 4
/// \brief Computes the number of 1 bits in a number.
/// \ingroup ConditionAlgorithms
inline size_t popcount (uint32_t v)	{ return __builtin_popcount (v); }
#if HAVE_INT64_T
inline size_t popcount (uint64_t v)	{ return __builtin_popcountll (v); }
#endif
#else
size_t popcount (uint32_t v) noexcept;
#if HAVE_INT64_T
size_t popcount (uint64_t v) noexcept;
#endif	// HAVE_INT64_T
#endif	// __GNUC__

//----------------------------------------------------------------------
// Optimized versions for standard types
//----------------------------------------------------------------------

#if WANT_UNROLLED_COPY

template <typename T>
inline T* unrolled_copy (const T* first, size_t count, T* result)
{
    copy_n_fast (first, count * sizeof(T), result);
    return advance (result, count);
}

template <>
inline uint8_t* copy_backward (const uint8_t* first, const uint8_t* last, uint8_t* result)
{
    copy_backward_fast (first, last, result);
    return result;
}

template <typename T>
inline T* unrolled_fill (T* result, size_t count, T value)
{
    for (; count; --count, ++result)
	*result = value;
    return result;
}
template <> inline uint8_t* unrolled_fill (uint8_t* result, size_t count, uint8_t value)
    { fill_n8_fast (result, count, value); return advance (result, count); }
template <> inline uint16_t* unrolled_fill (uint16_t* result, size_t count, uint16_t value)
    { fill_n16_fast (result, count, value); return advance (result, count); }
template <> inline uint32_t* unrolled_fill (uint32_t* result, size_t count, uint32_t value)
    { fill_n32_fast (result, count, value); return advance (result, count); }
template <> inline float* unrolled_fill (float* result, size_t count, float value)
    { fill_n32_fast ((uint32_t*) result, count, *noalias_cast<uint32_t*>(&value)); return advance (result, count); }

#if CPU_HAS_MMX
#define UNROLLED_COPY_SPECIALIZATION(type)						\
template <> inline type* copy (const type* first, const type* last, type* result)	\
{ return unrolled_copy (first, distance (first, last), result); }			\
template <> inline type* copy_n (const type* first, size_t count, type* result)		\
{ return unrolled_copy (first, count, result); }
#define UNROLLED_FILL_SPECIALIZATION(type)						\
template <> inline void fill (type* first, type* last, const type& value)		\
{ unrolled_fill (first, distance (first, last), value); }				\
template <> inline type* fill_n (type* first, size_t count, const type& value)		\
{ return unrolled_fill (first, count, value); }
UNROLLED_COPY_SPECIALIZATION(uint8_t)
UNROLLED_FILL_SPECIALIZATION(uint8_t)
UNROLLED_COPY_SPECIALIZATION(uint16_t)
UNROLLED_FILL_SPECIALIZATION(uint16_t)
UNROLLED_COPY_SPECIALIZATION(uint32_t)
UNROLLED_FILL_SPECIALIZATION(uint32_t)
UNROLLED_COPY_SPECIALIZATION(float)
UNROLLED_FILL_SPECIALIZATION(float)
#undef UNROLLED_FILL_SPECIALIZATION
#undef UNROLLED_COPY_SPECIALIZATION
#endif // WANT_UNROLLED_COPY
#endif // CPU_HAS_MMX

// Specializations for void* and char*, aliasing the above optimized versions.
//
// All these need duplication with const and non-const arguments, since
// otherwise the compiler will default to the unoptimized version for
// pointers not const in the caller's context, such as local variables.
// These are all inline, but they sure slow down compilation... :(
//
#define COPY_ALIAS_FUNC(ctype, type, alias_type)			\
template <> inline type* copy (ctype* first, ctype* last, type* result)	\
{ return (type*) copy ((const alias_type*) first, (const alias_type*) last, (alias_type*) result); }
#if WANT_UNROLLED_COPY
#if HAVE_THREE_CHAR_TYPES
COPY_ALIAS_FUNC(const char, char, uint8_t)
COPY_ALIAS_FUNC(char, char, uint8_t)
#endif
COPY_ALIAS_FUNC(const int8_t, int8_t, uint8_t)
COPY_ALIAS_FUNC(int8_t, int8_t, uint8_t)
COPY_ALIAS_FUNC(uint8_t, uint8_t, uint8_t)
COPY_ALIAS_FUNC(const int16_t, int16_t, uint16_t)
COPY_ALIAS_FUNC(int16_t, int16_t, uint16_t)
COPY_ALIAS_FUNC(uint16_t, uint16_t, uint16_t)
#if CPU_HAS_MMX || (SIZE_OF_LONG > 4)
COPY_ALIAS_FUNC(const int32_t, int32_t, uint32_t)
COPY_ALIAS_FUNC(int32_t, int32_t, uint32_t)
COPY_ALIAS_FUNC(uint32_t, uint32_t, uint32_t)
#endif
#endif
COPY_ALIAS_FUNC(const void, void, uint8_t)
COPY_ALIAS_FUNC(void, void, uint8_t)
#undef COPY_ALIAS_FUNC
#define COPY_BACKWARD_ALIAS_FUNC(ctype, type, alias_type)				\
template <> inline type* copy_backward (ctype* first, ctype* last, type* result)	\
{ return (type*) copy_backward ((const alias_type*) first, (const alias_type*) last, (alias_type*) result); }
#if WANT_UNROLLED_COPY
#if HAVE_THREE_CHAR_TYPES
COPY_BACKWARD_ALIAS_FUNC(char, char, uint8_t)
#endif
COPY_BACKWARD_ALIAS_FUNC(uint8_t, uint8_t, uint8_t)
COPY_BACKWARD_ALIAS_FUNC(int8_t, int8_t, uint8_t)
COPY_BACKWARD_ALIAS_FUNC(uint16_t, uint16_t, uint8_t)
COPY_BACKWARD_ALIAS_FUNC(const uint16_t, uint16_t, uint8_t)
COPY_BACKWARD_ALIAS_FUNC(int16_t, int16_t, uint8_t)
COPY_BACKWARD_ALIAS_FUNC(const int16_t, int16_t, uint8_t)
#endif
COPY_BACKWARD_ALIAS_FUNC(void, void, uint8_t)
COPY_BACKWARD_ALIAS_FUNC(const void, void, uint8_t)
#undef COPY_BACKWARD_ALIAS_FUNC
#define FILL_ALIAS_FUNC(type, alias_type, v_type)				\
template <> inline void fill (type* first, type* last, const v_type& value)	\
{ fill ((alias_type*) first, (alias_type*) last, (const alias_type) value); }
FILL_ALIAS_FUNC(void, uint8_t, char)
FILL_ALIAS_FUNC(void, uint8_t, uint8_t)
#if WANT_UNROLLED_COPY
#if HAVE_THREE_CHAR_TYPES
FILL_ALIAS_FUNC(char, uint8_t, char)
FILL_ALIAS_FUNC(char, uint8_t, uint8_t)
#endif
FILL_ALIAS_FUNC(int8_t, uint8_t, int8_t)
FILL_ALIAS_FUNC(int16_t, uint16_t, int16_t)
#if CPU_HAS_MMX || (SIZE_OF_LONG > 4)
FILL_ALIAS_FUNC(int32_t, uint32_t, int32_t)
#endif
#endif
#undef FILL_ALIAS_FUNC
#define COPY_N_ALIAS_FUNC(ctype, type, alias_type)					\
template <> inline type* copy_n (ctype* first, size_t count, type* result)	\
{ return (type*) copy_n ((const alias_type*) first, count, (alias_type*) result); }
COPY_N_ALIAS_FUNC(const void, void, uint8_t)
COPY_N_ALIAS_FUNC(void, void, uint8_t)
#if WANT_UNROLLED_COPY
#if HAVE_THREE_CHAR_TYPES
COPY_N_ALIAS_FUNC(const char, char, uint8_t)
COPY_N_ALIAS_FUNC(char, char, uint8_t)
#endif
COPY_N_ALIAS_FUNC(int8_t, int8_t, uint8_t)
COPY_N_ALIAS_FUNC(uint8_t, uint8_t, uint8_t)
COPY_N_ALIAS_FUNC(const int8_t, int8_t, uint8_t)
COPY_N_ALIAS_FUNC(int16_t, int16_t, uint16_t)
COPY_N_ALIAS_FUNC(uint16_t, uint16_t, uint16_t)
COPY_N_ALIAS_FUNC(const int16_t, int16_t, uint16_t)
#if CPU_HAS_MMX || (SIZE_OF_LONG > 4)
COPY_N_ALIAS_FUNC(int32_t, int32_t, uint32_t)
COPY_N_ALIAS_FUNC(uint32_t, uint32_t, uint32_t)
COPY_N_ALIAS_FUNC(const int32_t, int32_t, uint32_t)
#endif
#endif
#undef COPY_N_ALIAS_FUNC
#define FILL_N_ALIAS_FUNC(type, alias_type, v_type)				\
template <> inline type* fill_n (type* first, size_t n, const v_type& value)	\
{ return (type*) fill_n ((alias_type*) first, n, (const alias_type) value); }
FILL_N_ALIAS_FUNC(void, uint8_t, char)
FILL_N_ALIAS_FUNC(void, uint8_t, uint8_t)
#if WANT_UNROLLED_COPY
#if HAVE_THREE_CHAR_TYPES
FILL_N_ALIAS_FUNC(char, uint8_t, char)
FILL_N_ALIAS_FUNC(char, uint8_t, uint8_t)
#endif
FILL_N_ALIAS_FUNC(int8_t, uint8_t, int8_t)
FILL_N_ALIAS_FUNC(int16_t, uint16_t, int16_t)
#if CPU_HAS_MMX || (SIZE_OF_LONG > 4)
FILL_N_ALIAS_FUNC(int32_t, uint32_t, int32_t)
#endif
#endif
#undef FILL_N_ALIAS_FUNC

extern const char _FmtPrtChr[2][8];

} // namespace ustl

/// The ustl namespace contains all ustl classes and algorithms.
namespace ustl {

class istream;
class ostream;
class ostringstream;

/// \class cmemlink cmemlink.h ustl.h
/// \ingroup MemoryManagement
///
/// \brief A read-only pointer to a sized block of memory.
///
/// Use this class the way you would a const pointer to an allocated unstructured block.
/// The pointer and block size are available through member functions and cast operator.
///
/// Example usage:
///
/// \code
///     void* p = malloc (46721);
///     cmemlink a, b;
///     a.link (p, 46721);
///     assert (a.size() == 46721));
///     b = a;
///     assert (b.size() == 46721));
///     assert (b.DataAt(34) == a.DataAt(34));
///     assert (0 == memcmp (a, b, 12));
/// \endcode
///
class cmemlink {
public:
    typedef char		value_type;
    typedef const value_type*	pointer;
    typedef const value_type*	const_pointer;
    typedef value_type		reference;
    typedef value_type		const_reference;
    typedef size_t		size_type;
    typedef uint32_t		written_size_type;
    typedef ptrdiff_t		difference_type;
    typedef const_pointer	const_iterator;
    typedef const_iterator	iterator;
    typedef const cmemlink&	rcself_t;
public:
    inline		cmemlink (void)				: _data (nullptr), _size (0) { }
    inline		cmemlink (const void* p, size_type n)	: _data (const_pointer(p)), _size (n) { assert (p || !n); }
    inline		cmemlink (const cmemlink& l)		: _data (l._data), _size (l._size) {}
    inline virtual     ~cmemlink (void) noexcept			{}
    void		link (const void* p, size_type n);
    inline void		link (const cmemlink& l)	{ link (l.begin(), l.size()); }
    inline void		link (const void* first, const void* last)	{ link (first, distance (first, last)); }
    inline void		relink (const void* p, size_type n);
    virtual void	unlink (void) noexcept		{ _data = nullptr; _size = 0; }
    inline rcself_t	operator= (const cmemlink& l)	{ link (l); return *this; }
    bool		operator== (const cmemlink& l) const noexcept;
    inline void		swap (cmemlink& l)		{ ::ustl::swap (_data, l._data); ::ustl::swap (_size, l._size); }
    inline size_type	size (void) const		{ return _size; }
    inline size_type	max_size (void) const		{ return size(); }
    inline size_type	readable_size (void) const	{ return size(); }
    inline bool		empty (void) const		{ return !size(); }
   inline const_pointer	data (void) const		{ return _data; }
   inline const_pointer	cdata (void) const		{ return _data; }
    inline iterator	begin (void) const		{ return iterator (cdata()); }
    inline iterator	iat (size_type i) const		{ assert (i <= size()); return begin() + i; }
    inline iterator	end (void) const		{ return iat (size()); }
    inline void		resize (size_type n)		{ _size = n; }
    inline void		read (istream&)			{ assert (!"ustl::cmemlink is a read-only object."); }
    void		write (ostream& os) const;
    size_type		stream_size (void) const noexcept;
    void		text_write (ostringstream& os) const;
    void		write_file (const char* filename, int mode = 0644) const;
private:
    const_pointer	_data;		///< Pointer to the data block (const)
    size_type		_size;		///< size of the data block
};

//----------------------------------------------------------------------

/// A fast alternative to link which can be used when relinking to the same block (i.e. when it is resized)
inline void cmemlink::relink (const void* p, size_type n)
{
    _data = reinterpret_cast<const_pointer>(p);
    _size = n;
}

//----------------------------------------------------------------------

/// Use with cmemlink-derived classes to link to a static array
#define static_link(v)	link (VectorBlock(v))

} // namespace ustl

namespace ustl {

/// \class pair upair.h ustl.h
/// \ingroup AssociativeContainers
///
/// \brief Container for two values.
///
template <typename T1, typename T2>
class pair {
public:
    typedef T1		first_type;
    typedef T2		second_type;
public:
    /// Default constructor.
    inline constexpr	pair (void)			: first (T1()), second (T2()) {}
    /// Initializes members with \p a, and \p b.
    inline		pair (const T1& a, const T2& b)	: first (a), second (b) {}
    template <typename T3, typename T4>
    inline		pair (const pair<T3,T4>& p2)	: first (p2.first), second (p2.second) {}
    inline pair&	operator= (const pair& p2)	{ first = p2.first; second = p2.second; return *this; }
    template <typename T3, typename T4>
    inline pair&	operator= (const pair<T3,T4>& p2) { first = p2.first; second = p2.second; return *this; }
    inline bool		operator== (const pair& v)const	{ return first == v.first && second == v.second; }
    inline bool		operator< (const pair& v) const	{ return first < v.first || (first == v.first && second < v.second); }
    inline void		swap (pair& v)			{ swap(first,v.first); swap(second,v.second); }
#if HAVE_CPP11
			pair (const pair&) = default;
			pair (pair&&) = default;
    template <typename T3, typename T4>
    inline		pair (T3&& a, T4&& b)		: first (forward<T3>(a)), second (forward<T4>(b)) {}
    template <typename T3, typename T4>
    inline		pair (pair<T3,T4>&& p2)		: first (forward<T3>(p2.first)), second (forward<T4>(p2.second)) {}
    inline pair&	operator= (pair&& p2)		{ first = move(p2.first); second = move(p2.second); return *this; }
    template <typename T3, typename T4>
    inline pair&	operator= (pair<T3,T4>&& p2)	{ first = forward<T3>(p2.first); second = forward<T4>(p2.second); return *this; }
#endif
public:
    first_type		first;
    second_type		second;
};

#if HAVE_CPP11

/// Returns a pair object with (a,b)
template <typename T1, typename T2>
inline pair<T1,T2> make_pair (T1&& a, T2&& b)
    { return pair<T1,T2> (forward<T1>(a), forward<T2>(b)); }

#endif

/// Returns a pair object with (a,b)
template <typename T1, typename T2>
inline pair<T1,T2> make_pair (const T1& a, const T2& b)
    { return pair<T1,T2> (a, b); }

} // namespace ustl

namespace ustl {

//----------------------------------------------------------------------
// Standard functors
//----------------------------------------------------------------------

/// \brief void-returning function abstract interface.
/// \ingroup FunctorObjects
template <typename Result>
struct void_function {
    typedef Result	result_type;
};

/// \brief \p Result f (\p Arg) function abstract interface.
/// \ingroup FunctorObjects
template <typename Arg, typename Result>
struct unary_function {
    typedef Arg		argument_type;
    typedef Result	result_type;
};

/// \brief \p Result f (\p Arg1, \p Arg2) function abstract interface.
/// \ingroup FunctorObjects
template <typename Arg1, typename Arg2, typename Result>
struct binary_function {
    typedef Arg1	first_argument_type;
    typedef Arg2	second_argument_type;
    typedef Result	result_type;
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#define STD_BINARY_FUNCTOR(name, rv, func)	\
template <class T> struct name : public binary_function<T,T,rv> \
{ inline rv operator()(const T& a, const T& b) const { return func; } };
#define STD_UNARY_FUNCTOR(name, rv, func)	\
template <class T> struct name : public unary_function<T,rv> \
{ inline rv operator()(const T& a) const { return func; } };
#define STD_CONVERSION_FUNCTOR(name, func)	\
template <class S, class D> struct name : public unary_function<S,D> \
{ inline D operator()(const S& a) const { return func; } };

STD_BINARY_FUNCTOR (plus,		T,	(a + b))
STD_BINARY_FUNCTOR (minus,		T,	(a - b))
STD_BINARY_FUNCTOR (divides,		T,	(a / b))
STD_BINARY_FUNCTOR (modulus,		T,	(a % b))
STD_BINARY_FUNCTOR (multiplies,		T,	(a * b))
STD_BINARY_FUNCTOR (logical_and,	T,	(a && b))
STD_BINARY_FUNCTOR (logical_or,		T,	(a || b))
STD_UNARY_FUNCTOR  (logical_not,	T,	(!a))
STD_BINARY_FUNCTOR (bitwise_or,		T,	(a | b))
STD_BINARY_FUNCTOR (bitwise_and,	T,	(a & b))
STD_BINARY_FUNCTOR (bitwise_xor,	T,	(a ^ b))
STD_UNARY_FUNCTOR  (bitwise_not,	T,	(~a))
STD_UNARY_FUNCTOR  (negate,		T,	(-a))
STD_BINARY_FUNCTOR (equal_to,		bool,	(a == b))
STD_BINARY_FUNCTOR (not_equal_to,	bool,	(!(a == b)))
STD_BINARY_FUNCTOR (greater,		bool,	(b < a))
STD_BINARY_FUNCTOR (less,		bool,	(a < b))
STD_BINARY_FUNCTOR (greater_equal,	bool,	(!(a < b)))
STD_BINARY_FUNCTOR (less_equal,		bool,	(!(b < a)))
STD_BINARY_FUNCTOR (compare,		int,	(a < b ? -1 : (b < a)))
STD_UNARY_FUNCTOR  (identity,		T,	(a))

#endif // DOXYGEN_SHOULD_SKIP_THIS

/// \brief Selects and returns the first argument.
/// \ingroup FunctorObjects
template <class T1, class T2> struct project1st	: public binary_function<T1,T2,T1>    { inline const T1& operator()(const T1& a, const T2&) const { return a; } };
/// \brief Selects and returns the second argument.
/// \ingroup FunctorObjects
template <class T1, class T2> struct project2nd	: public binary_function<T1,T2,T2>    { inline const T2& operator()(const T1&, const T2& a) const { return a; } };

//----------------------------------------------------------------------
// Generic function to functor converters.
//----------------------------------------------------------------------

/// \brief Wrapper object for unary function pointers.
/// Use the ptr_fun accessor to create this object.
/// \ingroup FunctorObjects
template <typename Arg, typename Result>
class pointer_to_unary_function : public unary_function<Arg,Result> {
public:
    typedef Arg		argument_type;
    typedef Result	result_type;
    typedef Result	(*pfunc_t)(Arg);
public:
    explicit inline	pointer_to_unary_function (pfunc_t pfn) : _pfn (pfn) {}
    inline result_type	operator() (argument_type v) const { return _pfn(v); }
private:
    pfunc_t		_pfn;	///< Pointer to the wrapped function.
};

/// \brief Wrapper object for binary function pointers.
/// Use the ptr_fun accessor to create this object.
/// \ingroup FunctorObjects
template <typename Arg1, typename Arg2, typename Result>
class pointer_to_binary_function : public binary_function<Arg1,Arg2,Result> {
public:
    typedef Arg1	first_argument_type;
    typedef Arg2	second_argument_type;
    typedef Result	result_type;
    typedef Result	(*pfunc_t)(Arg1, Arg2);
public:
    explicit inline	pointer_to_binary_function (pfunc_t pfn) : _pfn (pfn) {}
    inline result_type	operator() (first_argument_type v1, second_argument_type v2) const { return _pfn(v1, v2); }
private:
    pfunc_t		_pfn;	///< Pointer to the wrapped function.
};

/// ptr_fun(pfn) wraps function pointer pfn into a functor class that calls it.
/// \ingroup FunctorAccessors
template <typename Arg, typename Result>
inline pointer_to_unary_function<Arg,Result> ptr_fun (Result (*pfn)(Arg))
{
    return pointer_to_unary_function<Arg,Result> (pfn);
}

/// ptr_fun(pfn) wraps function pointer pfn into a functor class that calls it.
/// \ingroup FunctorAccessors
template <typename Arg1, typename Arg2, typename Result>
inline pointer_to_binary_function<Arg1,Arg2,Result> ptr_fun (Result (*pfn)(Arg1,Arg2))
{
    return pointer_to_binary_function<Arg1,Arg2,Result> (pfn);
}

//----------------------------------------------------------------------
// Negators.
//----------------------------------------------------------------------

/// \brief Wraps a unary function to return its logical negative.
/// Use the unary_negator accessor to create this object.
/// \ingroup FunctorObjects
template <class UnaryFunction>
class unary_negate : public unary_function<typename UnaryFunction::argument_type,
					   typename UnaryFunction::result_type> {
public:
    typedef typename UnaryFunction::argument_type	argument_type;
    typedef typename UnaryFunction::result_type		result_type;
public:
    explicit inline unary_negate (UnaryFunction pfn) : _pfn (pfn) {}
    inline result_type operator() (argument_type v) const { return !_pfn(v); }
private:
    UnaryFunction	_pfn;
};

/// Returns the functor that negates the result of *pfn().
/// \ingroup FunctorAccessors
template <class UnaryFunction>
inline unary_negate<UnaryFunction> unary_negator (UnaryFunction pfn)
{
    return unary_negate<UnaryFunction>(pfn);
}

//----------------------------------------------------------------------
// Argument binders
//----------------------------------------------------------------------

/// \brief Converts a binary function to a unary function
/// by binding a constant value to the first argument.
/// Use the bind1st accessor to create this object.
/// \ingroup FunctorObjects
template <class BinaryFunction> 
class binder1st : public unary_function<typename BinaryFunction::second_argument_type,
					typename BinaryFunction::result_type> {
public:
    typedef typename BinaryFunction::first_argument_type	arg1_t;
    typedef typename BinaryFunction::second_argument_type	arg2_t;
    typedef typename BinaryFunction::result_type		result_t;
public:
    inline binder1st (const BinaryFunction& pfn, const arg1_t& v) : _pfn (pfn), _v(v) {}
    inline result_t operator()(arg2_t v2) const { return _pfn (_v, v2); }
protected:
    BinaryFunction	_pfn;
    arg1_t		_v;
};

/// \brief Converts a binary function to a unary function
/// by binding a constant value to the second argument.
/// Use the bind2nd accessor to create this object.
/// \ingroup FunctorObjects
template <class BinaryFunction> 
class binder2nd : public unary_function<typename BinaryFunction::first_argument_type,
					typename BinaryFunction::result_type> {
public:
    typedef typename BinaryFunction::first_argument_type	arg1_t;
    typedef typename BinaryFunction::second_argument_type	arg2_t;
    typedef typename BinaryFunction::result_type		result_t;
public:
    inline binder2nd (const BinaryFunction& pfn, const arg2_t& v) : _pfn (pfn), _v(v) {}
    inline result_t operator()(arg1_t v1) const { return _pfn (v1, _v); }
protected:
    BinaryFunction	_pfn;
    arg2_t		_v;
};

/// Converts \p pfn into a unary function by binding the first argument to \p v.
/// \ingroup FunctorAccessors
template <typename BinaryFunction>
inline binder1st<BinaryFunction>
bind1st (BinaryFunction pfn, typename BinaryFunction::first_argument_type v) 
{
    return binder1st<BinaryFunction> (pfn, v);
}

/// Converts \p pfn into a unary function by binding the second argument to \p v.
/// \ingroup FunctorAccessors
template <typename BinaryFunction>
inline binder2nd<BinaryFunction>
bind2nd (BinaryFunction pfn, typename BinaryFunction::second_argument_type v) 
{
    return binder2nd<BinaryFunction> (pfn, v);
}

//----------------------------------------------------------------------
// Composition adapters
//----------------------------------------------------------------------

/// \brief Chains two unary functions together.
///
/// When f(x) and g(x) are composed, the result is function c(x)=f(g(x)).
/// Use the \ref compose1 accessor to create this object.
/// This template is an extension, implemented by SGI STL and uSTL.
/// \ingroup FunctorObjects
///
template <typename Operation1, typename Operation2>
class unary_compose : public unary_function<typename Operation2::argument_type,
					    typename Operation1::result_type> {
public:
    typedef typename Operation2::argument_type	arg_t;
    typedef const arg_t&			rcarg_t;
    typedef typename Operation1::result_type	result_t;
public:
    inline unary_compose (const Operation1& f, const Operation2& g) : _f(f), _g(g) {}
    inline result_t operator() (rcarg_t x) const { return _f(_g(x)); }
protected:
    Operation1	_f;	///< f(x), if c(x) = f(g(x))
    Operation2	_g;	///< g(x), if c(x) = f(g(x))
};

/// Creates a \ref unary_compose object whose function c(x)=f(g(x))
/// \ingroup FunctorAccessors
template <typename Operation1, typename Operation2>
inline unary_compose<Operation1, Operation2>
compose1 (const Operation1& f, const Operation2& g)
{ return unary_compose<Operation1,Operation2>(f, g); }

/// \brief Chains two unary functions through a binary function.
///
/// When f(x,y), g(x), and h(x) are composed, the result is function
/// c(x)=f(g(x),h(x)). Use the \ref compose2 accessor to create this
/// object. This template is an extension, implemented by SGI STL and uSTL.
/// \ingroup FunctorObjects
///
template <typename Operation1, typename Operation2, typename Operation3>
class binary_compose : public unary_function<typename Operation2::argument_type,
					    typename Operation1::result_type> {
public:
    typedef typename Operation2::argument_type	arg_t;
    typedef const arg_t&			rcarg_t;
    typedef typename Operation1::result_type	result_t;
public:
    inline binary_compose (const Operation1& f, const Operation2& g, const Operation3& h) : _f(f), _g(g), _h(h) {}
    inline result_t operator() (rcarg_t x) const { return _f(_g(x), _h(x)); }
protected:
    Operation1	_f;	///< f(x,y), if c(x) = f(g(x),h(x))
    Operation2	_g;	///< g(x), if c(x) = f(g(x),h(x))
    Operation3	_h;	///< h(x), if c(x) = f(g(x),h(x))
};

/// Creates a \ref binary_compose object whose function c(x)=f(g(x),h(x))
/// \ingroup FunctorAccessors
template <typename Operation1, typename Operation2, typename Operation3>
inline binary_compose<Operation1, Operation2, Operation3>
compose2 (const Operation1& f, const Operation2& g, const Operation3& h)
{ return binary_compose<Operation1, Operation2, Operation3> (f, g, h); }

//----------------------------------------------------------------------
// Member function adaptors
//----------------------------------------------------------------------

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#define MEM_FUN_T(WrapperName, ClassName, ArgType, FuncType, CallType)				\
    template <typename Ret, class T>								\
    class ClassName : public unary_function<ArgType,Ret> {					\
    public:											\
	typedef Ret (T::*func_t) FuncType;							\
    public:											\
	explicit inline	ClassName (func_t pf) : _pf (pf) {}					\
	inline Ret	operator() (ArgType p) const { return (p CallType _pf)(); }		\
    private:											\
	func_t	_pf;										\
    };	\
	\
    template <class Ret, typename T>		\
    inline ClassName<Ret,T> WrapperName (Ret (T::*pf) FuncType)	\
    {						\
	return ClassName<Ret,T> (pf);		\
    }

MEM_FUN_T(mem_fun,	mem_fun_t, 		T*,		(void),		->*)
MEM_FUN_T(mem_fun,	const_mem_fun_t, 	const T*,	(void) const,	->*)
MEM_FUN_T(mem_fun_ref,	mem_fun_ref_t,		T&,		(void),		.*)
MEM_FUN_T(mem_fun_ref,	const_mem_fun_ref_t, 	const T&,	(void) const,	.*)

#define EXT_MEM_FUN_T(ClassName, HostType, FuncType) \
    template <class T, typename Ret, typename V> \
    class ClassName : public unary_function<V,void> { \
    public: \
	typedef Ret (T::*func_t)(V) FuncType; \
    public: \
	inline		ClassName (HostType t, func_t pf) : _t (t), _pf (pf) {} \
	inline Ret	operator() (V v) const { return (_t->*_pf)(v); } \
    private: \
	HostType	_t; \
	func_t		_pf; \
    };	\
	\
    template <class T, typename Ret, typename V>					\
    inline ClassName<T,Ret,V> mem_fun (HostType p, Ret (T::*pf)(V) FuncType)	\
    {											\
	return ClassName<T,Ret,V> (p, pf);						\
    }

EXT_MEM_FUN_T(ext_mem_fun_t,		T*,		)
EXT_MEM_FUN_T(const_ext_mem_fun_t,	const T*,	const)

#endif // DOXYGEN_SHOULD_SKIP_THIS

//----------------------------------------------------------------------
// Member variable adaptors (uSTL extension)
//----------------------------------------------------------------------

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#define MEM_VAR_T(FunctorName, ArgType, VarType, BaseClass, CallImpl)			\
    template <typename Function, class T, typename VT>					\
    class FunctorName##_t : public BaseClass {						\
    public:										\
	typedef ArgType				argument_type;				\
	typedef typename Function::result_type	result_type;				\
	typedef VarType				mem_var_ptr_t;				\
    public:										\
	inline FunctorName##_t (mem_var_ptr_t pv, Function pfn) : _pv(pv), _pfn(pfn) {}	\
	inline result_type operator() CallImpl						\
    private:										\
	mem_var_ptr_t	_pv;								\
	Function	_pfn;								\
    };											\
											\
    template <typename Function, class T, typename VT>					\
    inline FunctorName##_t<Function, T, VT>						\
    FunctorName (VT T::*mvp, Function pfn)						\
    {											\
	return FunctorName##_t<Function,T,VT> (mvp, pfn);				\
    }

#define FUNCTOR_UNARY_BASE(ArgType)	unary_function<ArgType, typename Function::result_type>
#define FUNCTOR_BINARY_BASE(ArgType)	binary_function<ArgType, ArgType, typename Function::result_type>

#define MEM_VAR_UNARY_ARGS		(argument_type p) const \
					{ return _pfn(p.*_pv); }
#define MEM_VAR_BINARY_ARGS		(argument_type p1, argument_type p2) const \
					{ return _pfn(p1.*_pv, p2.*_pv); }

MEM_VAR_T(mem_var1,		T&, VT T::*,		FUNCTOR_UNARY_BASE(T&),  MEM_VAR_UNARY_ARGS)
MEM_VAR_T(const_mem_var1, const T&, const VT T::*,	FUNCTOR_UNARY_BASE(T&),  MEM_VAR_UNARY_ARGS)
MEM_VAR_T(mem_var2,		T&, VT T::*,		FUNCTOR_BINARY_BASE(T&), MEM_VAR_BINARY_ARGS)
MEM_VAR_T(const_mem_var2, const T&, const VT T::*,	FUNCTOR_BINARY_BASE(T&), MEM_VAR_BINARY_ARGS)

#undef MEM_VAR_UNARY_ARGS
#undef MEM_VAR_BINARY_ARGS

#endif // DOXYGEN_SHOULD_SKIP_THIS

/// Returned functor passes member variable \p mvp reference of given object to equal\<VT\>.
/// \ingroup FunctorAccessors
template <class T, typename VT>
inline const_mem_var1_t<binder2nd<equal_to<VT> >, T, VT>
mem_var_equal_to (const VT T::*mvp, const VT& v)
{
    return const_mem_var1_t<binder2nd<equal_to<VT> >,T,VT> (mvp, bind2nd(equal_to<VT>(), v));
}

/// Returned functor passes member variable \p mvp reference of given object to less\<VT\>.
/// \ingroup FunctorAccessors
template <class T, typename VT>
inline const_mem_var1_t<binder2nd<less<VT> >, T, VT>
mem_var_less (const VT T::*mvp, const VT& v)
{
    return const_mem_var1_t<binder2nd<less<VT> >,T,VT> (mvp, bind2nd(less<VT>(), v));
}

/// Returned functor passes member variable \p mvp reference of given object to equal\<VT\>.
/// \ingroup FunctorAccessors
template <class T, typename VT>
inline const_mem_var2_t<equal_to<VT>, T, VT>
mem_var_equal_to (const VT T::*mvp)
{
    return const_mem_var2_t<equal_to<VT>,T,VT> (mvp, equal_to<VT>());
}

/// Returned functor passes member variable \p mvp reference of given object to less\<VT\>.
/// \ingroup FunctorAccessors
template <class T, typename VT>
inline const_mem_var2_t<less<VT>, T, VT>
mem_var_less (const VT T::*mvp)
{
    return const_mem_var2_t<less<VT>,T,VT> (mvp, less<VT>());
}

//----------------------------------------------------------------------
// Dereference adaptors (uSTL extension)
//----------------------------------------------------------------------

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#define DEREFERENCER_T(ClassName, ArgType, BaseClass, CallImpl, FunctorKey)	\
    template <typename T, typename Function>					\
    class ClassName : public BaseClass {					\
    public:									\
	typedef ArgType*			argument_type;			\
	typedef typename Function::result_type	result_type;			\
    public:									\
	inline			ClassName (Function pfn) : _pfn (pfn) {}	\
	inline result_type	operator() CallImpl				\
    private:									\
	Function		_pfn;						\
    };										\
										\
    template <typename T, typename Function>					\
    inline ClassName<T,Function> _dereference (Function pfn, FunctorKey)	\
    {										\
	return ClassName<T,Function> (pfn);					\
    }

#define DEREF_UNARY_ARGS		(argument_type p) const \
					{ return _pfn(*p); }
#define DEREF_BINARY_ARGS		(argument_type p1, argument_type p2) const \
					{ return _pfn(*p1, *p2); }

DEREFERENCER_T(deref1_t,	T, 		FUNCTOR_UNARY_BASE(T*),		DEREF_UNARY_ARGS,	FUNCTOR_UNARY_BASE(T))
DEREFERENCER_T(const_deref1_t,	const T, 	FUNCTOR_UNARY_BASE(const T*),	DEREF_UNARY_ARGS,	FUNCTOR_UNARY_BASE(const T))
DEREFERENCER_T(deref2_t,	T, 		FUNCTOR_BINARY_BASE(T*),	DEREF_BINARY_ARGS,	FUNCTOR_BINARY_BASE(T))
DEREFERENCER_T(const_deref2_t,	const T, 	FUNCTOR_BINARY_BASE(const T*),	DEREF_BINARY_ARGS,	FUNCTOR_BINARY_BASE(const T))

#define dereference(f) _dereference(f,f)

#undef DEREF_UNARY_ARGS
#undef DEREF_BINARY_ARGS

#endif

} // namespace ustl

namespace ustl {

class ostringstream;
class istream;
class ostream;

/// \class CBacktrace bktrace.h ustl.h
///
/// \brief Stores the backtrace from the point of construction.
///
/// The backtrace, or callstack, is the listing of functions called to
/// reach the construction of this object. This is useful for debugging,
/// to print the location of an error. To get meaningful output you'll
/// need to use a debug build with symbols and with frame pointers. For
/// GNU ld you will also need to link with the -rdynamic option to see
/// actual function names instead of __gxx_personality0+0xF4800.
///
class CBacktrace {
public:
			CBacktrace (void) noexcept;
			CBacktrace (const CBacktrace& v) noexcept;
    inline		~CBacktrace (void) noexcept	{ if (_symbols) free (_symbols); }
    const CBacktrace&	operator= (const CBacktrace& v) noexcept;
    void		text_write (ostringstream& os) const;
    void		read (istream& is);
    void		write (ostream& os) const;
    size_t		stream_size (void) const;
private:
    void		GetSymbols (void) noexcept DLL_LOCAL;
private:
    void*		_addresses [64];	///< Addresses of each function on the stack.
    char*		_symbols;		///< Symbols corresponding to each address.
    uint32_t		_nFrames;		///< Number of addresses in _addresses.
    uint32_t		_symbolsSize;		///< Size of _symbols.
};

} // namespace ustl

#if WITHOUT_LIBSTDCPP
namespace std {
/// If you write a replacement terminate handler, it must be of this type.
typedef void (*terminate_handler) (void);
/// If you write a replacement unexpected handler, it must be of this type.
typedef void (*unexpected_handler) (void);
/// Takes a new handler function as an argument, returns the old function.
terminate_handler set_terminate (terminate_handler pHandler) noexcept;
/// The runtime will call this function if exception handling must be
/// abandoned for any reason.  It can also be called by the user.
void terminate (void) noexcept __attribute__ ((__noreturn__));
/// Takes a new handler function as an argument, returns the old function.
unexpected_handler set_unexpected (unexpected_handler pHandler) noexcept;
/// The runtime will call this function if an exception is thrown which
/// violates the function's exception specification.
void unexpected (void) __attribute__ ((__noreturn__));
/// Returns true when the caught exception violates the throw specification.
bool uncaught_exception() noexcept;
} // namespace std
#endif

namespace ustl {

class string;

typedef uint32_t	xfmt_t;

enum {
    xfmt_Exception,
    xfmt_BadAlloc
};

/// \class exception uexception.h ustl.h
/// \ingroup Exceptions
///
/// \brief Base class for exceptions, equivalent to std::exception.
///
#if WITHOUT_LIBSTDCPP
class exception {
#else
class exception : public std::exception {
#endif
public:
    typedef const CBacktrace& rcbktrace_t;
public:
    inline		exception (void) noexcept : _format (xfmt_Exception) {}
    inline virtual	~exception (void) noexcept {}
    inline virtual const char* what (void) const noexcept { return "error"; }
    virtual void	info (string& msgbuf, const char* fmt = nullptr) const noexcept;
    virtual void	read (istream& is);
    virtual void	write (ostream& os) const;
    void		text_write (ostringstream& os) const noexcept;
    inline virtual size_t stream_size (void) const noexcept { return sizeof(_format) + sizeof(uint32_t) + _backtrace.stream_size(); }
    /// Format of the exception is used to lookup exception::info format string.
    /// Another common use is the instantiation of serialized exceptions, used
    /// by the error handler node chain to troubleshoot specific errors.
    inline xfmt_t	format (void) const	{ return _format; }
    inline rcbktrace_t	backtrace (void) const	{ return _backtrace; }
protected:
    inline void		set_format (xfmt_t fmt) { _format = fmt; }
private:
    CBacktrace		_backtrace;	///< Backtrace of the throw point.
    xfmt_t		_format;	///< Format of the exception's data.
};

/// \class bad_cast uexception.h ustl.h
/// \ingroup Exceptions
///
/// \brief Thrown to indicate a bad dynamic_cast usage.
///
class bad_cast : public exception {
public:
    inline 			bad_cast (void) noexcept		: exception() {}
    inline virtual const char*	what (void) const noexcept override	{ return "bad cast"; }
};

class bad_typeid : public exception {
public:
    inline			bad_typeid (void) noexcept		{ }
    inline virtual const char*	what (void) const noexcept override	{ return "bad typeid"; }
};

const char* demangle_type_name (char* buf, size_t bufSize, size_t* pdmSize = nullptr) noexcept;

//----------------------------------------------------------------------

/// \class bad_alloc uexception.h ustl.h
/// \ingroup Exceptions
///
/// \brief Exception thrown on memory allocation failure by memblock::reserve.
///
#if WITHOUT_LIBSTDCPP
} // namespace ustl
namespace std {
class bad_alloc : public ::ustl::exception {
#else

class bad_alloc : public std::bad_alloc, public exception {
#endif
public:
    explicit		bad_alloc (size_t nBytes = 0) noexcept;
    inline virtual const char*	what (void) const noexcept override { return "memory allocation failed"; }
    virtual void	info (ustl::string& msgbuf, const char* fmt = nullptr) const noexcept override;
    virtual void	read (ustl::istream& is) override;
    virtual void	write (ustl::ostream& os) const override;
    virtual size_t	stream_size (void) const noexcept override;
protected:
    size_t		_bytesRequested;	///< Number of bytes requested by the failed allocation.
};

#if WITHOUT_LIBSTDCPP
} // namespace std
namespace ustl {
    typedef std::bad_alloc bad_alloc;
#endif
} // namespace ustl

/// Just like malloc, but throws on failure.
extern "C" void* tmalloc (size_t n) __attribute__((malloc));
/// Just like free, but doesn't crash when given a nullptr.
extern "C" void nfree (void* p) noexcept;

#if WITHOUT_LIBSTDCPP

//
// These are replaceable signatures:
//  - normal single new and delete (no arguments, throw @c bad_alloc on error)
//  - normal array new and delete (same)
//  - @c nothrow single new and delete (take a @c nothrow argument, return
//    @c nullptr on error)
//  - @c nothrow array new and delete (same)
//
//  Placement new and delete signatures (take a memory address argument,
//  does nothing) may not be replaced by a user's program.
//
void* operator new (size_t n);
void* operator new[] (size_t n);
void  operator delete (void* p) noexcept;
void  operator delete[] (void* p) noexcept;
#if HAVE_CPP14
void  operator delete (void* p, size_t) noexcept;
void  operator delete[] (void* p, size_t) noexcept;
#endif

// Default placement versions of operator new.
inline void* operator new (size_t, void* p) noexcept	{ return p; }
inline void* operator new[] (size_t, void* p) noexcept	{ return p; }

// Default placement versions of operator delete.
inline void  operator delete  (void*, void*) noexcept	{ }
inline void  operator delete[](void*, void*) noexcept	{ }

#endif	// WITHOUT_LIBSTDCPP

namespace ustl {

//----------------------------------------------------------------------

template <typename Category, typename T, typename Distance = ptrdiff_t, typename Pointer = T*, typename Reference = T&>
struct iterator {
    typedef T		value_type;
    typedef Distance	difference_type;
    typedef Pointer	pointer;
    typedef Reference	reference;
    typedef Category	iterator_category;
};

//----------------------------------------------------------------------

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag {};
struct bidirectional_iterator_tag {};
struct random_access_iterator_tag {};

/// \struct iterator_traits uiterator.h ustl.h
/// \brief Contains the type traits of \p Iterator
///
template <typename Iterator>
struct iterator_traits {
    typedef typename Iterator::value_type	value_type;
    typedef typename Iterator::difference_type	difference_type;
    typedef typename Iterator::pointer		pointer;
    typedef typename Iterator::reference	reference;
    typedef typename Iterator::iterator_category iterator_category;
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS

template <typename T>
struct iterator_traits<T*> {
    typedef T		value_type;
    typedef ptrdiff_t	difference_type;
    typedef const T*	const_pointer;
    typedef T*		pointer;
    typedef const T&	const_reference;
    typedef T&		reference;
    typedef random_access_iterator_tag	iterator_category;
};

template <typename T>
struct iterator_traits<const T*> {
    typedef T		value_type;
    typedef ptrdiff_t	difference_type;
    typedef const T*	const_pointer;
    typedef const T*	pointer;
    typedef const T&	const_reference;
    typedef const T&	reference;
    typedef random_access_iterator_tag	iterator_category;
};

template <>
struct iterator_traits<void*> {
    typedef uint8_t		value_type;
    typedef ptrdiff_t		difference_type;
    typedef const void*		const_pointer;
    typedef void*		pointer;
    typedef const value_type&	const_reference;
    typedef value_type&		reference;
    typedef random_access_iterator_tag	iterator_category;
};

template <>
struct iterator_traits<const void*> {
    typedef uint8_t		value_type;
    typedef ptrdiff_t		difference_type;
    typedef const void*		const_pointer;
    typedef const void*		pointer;
    typedef const value_type&	const_reference;
    typedef const value_type&	reference;
    typedef random_access_iterator_tag	iterator_category;
};

#endif

//----------------------------------------------------------------------

/// \class reverse_iterator uiterator.h ustl.h
/// \ingroup IteratorAdaptors
/// \brief Wraps \p Iterator to behave in an exactly opposite manner.
///
template <class Iterator>
class reverse_iterator {
public:
    typedef typename iterator_traits<Iterator>::value_type	value_type;
    typedef typename iterator_traits<Iterator>::difference_type	difference_type;
    typedef typename iterator_traits<Iterator>::pointer		pointer;
    typedef typename iterator_traits<Iterator>::reference	reference;
    typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
public:
				reverse_iterator (void) : _i() {}
    explicit			reverse_iterator (Iterator iter) : _i (iter) {}
    inline bool			operator== (const reverse_iterator& iter) const { return _i == iter._i; }
    inline bool			operator< (const reverse_iterator& iter) const { return iter._i < _i; }
    inline Iterator		base (void) const { return _i; }
    inline reference		operator* (void) const { Iterator prev (_i); --prev; return *prev; }
    inline pointer		operator-> (void) const { return &(operator*()); }
    inline reverse_iterator&	operator++ (void) { -- _i; return *this; }
    inline reverse_iterator&	operator-- (void) { ++ _i; return *this; }
    inline reverse_iterator	operator++ (int) { reverse_iterator prev (*this); -- _i; return prev; }
    inline reverse_iterator	operator-- (int) { reverse_iterator prev (*this); ++ _i; return prev; }
    inline reverse_iterator&	operator+= (size_t n) { _i -= n; return *this; }
    inline reverse_iterator&	operator-= (size_t n) { _i += n; return *this; }
    inline reverse_iterator	operator+ (size_t n) const { return reverse_iterator (_i - n); }
    inline reverse_iterator	operator- (size_t n) const { return reverse_iterator (_i + n); }
    inline reference		operator[] (uoff_t n) const { return *(*this + n); }
    inline difference_type	operator- (const reverse_iterator& i) const { return distance (_i, i._i); }
protected:
    Iterator			_i;
};

//----------------------------------------------------------------------

/// \class insert_iterator uiterator.h ustl.h
/// \ingroup IteratorAdaptors
/// \brief Calls insert on bound container for each assignment.
///
template <class Container>
class insert_iterator {
public:
    typedef typename Container::value_type	value_type;
    typedef typename Container::difference_type	difference_type;
    typedef typename Container::pointer		pointer;
    typedef typename Container::reference	reference;
    typedef typename Container::iterator	iterator;
    typedef output_iterator_tag			iterator_category;
public:
    explicit			insert_iterator (Container& ctr, iterator ip) : _rctr (ctr), _ip (ip) {}
    inline insert_iterator&	operator= (typename Container::const_reference v)
				    { _ip = _rctr.insert (_ip, v); return *this; }
    inline insert_iterator&	operator* (void)  { return *this; }
    inline insert_iterator&	operator++ (void) { ++ _ip; return *this; }
    inline insert_iterator	operator++ (int)  { insert_iterator prev (*this); ++_ip; return prev; }
protected:
    Container&			_rctr;
    iterator			_ip;
};

/// Returns the insert_iterator for \p ctr.
template <class Container>
inline insert_iterator<Container> inserter (Container& ctr, typename Container::iterator ip)
{
    return insert_iterator<Container> (ctr, ip);
}

//----------------------------------------------------------------------

/// \class back_insert_iterator uiterator.h ustl.h
/// \ingroup IteratorAdaptors
/// \brief Calls push_back on bound container for each assignment.
///
template <class Container>
class back_insert_iterator {
public:
    typedef typename Container::value_type	value_type;
    typedef typename Container::difference_type	difference_type;
    typedef typename Container::pointer		pointer;
    typedef typename Container::reference	reference;
    typedef output_iterator_tag			iterator_category;
public:
    explicit				back_insert_iterator (Container& ctr) : _rctr (ctr) {}
    inline back_insert_iterator&	operator= (typename Container::const_reference v)
					    { _rctr.push_back (v); return *this; }
    inline back_insert_iterator&	operator* (void)  { return *this; }
    inline back_insert_iterator&	operator++ (void) { return *this; }
    inline back_insert_iterator		operator++ (int)  { return *this; }
protected:
    Container&		_rctr;
};

/// Returns the back_insert_iterator for \p ctr.
template <class Container>
inline back_insert_iterator<Container> back_inserter (Container& ctr)
{
    return back_insert_iterator<Container> (ctr);
}

//----------------------------------------------------------------------

/// \class index_iterate uiterator.h ustl.h
/// \ingroup IteratorAdaptors
///
/// \brief Allows iteration through an index container.
///
/// Converts an iterator into a container of uoff_t indexes to an
/// iterator of iterators into another container.
///
template <typename RandomAccessIterator, typename IndexIterator>
class index_iterate {
public:
    typedef RandomAccessIterator	value_type;
    typedef ptrdiff_t			difference_type;
    typedef RandomAccessIterator*	pointer;
    typedef RandomAccessIterator	reference;
    typedef random_access_iterator_tag	iterator_category;
public:
				index_iterate (void) : _base(), _i() {}
				index_iterate (RandomAccessIterator ibase, IndexIterator iindex) : _base (ibase), _i (iindex) {}
    inline bool			operator== (const index_iterate& i) const { return _i == i._i; }
    inline bool			operator< (const index_iterate& i) const { return _i < i._i; }
    inline bool			operator== (const RandomAccessIterator& i) const { return _base == i; }
    inline bool			operator< (const RandomAccessIterator& i) const { return _base < i; }
    inline IndexIterator	base (void) const { return _i; }
    inline reference		operator* (void) const { return advance(_base, *_i); }
    inline pointer		operator-> (void) const { return &(operator*()); }
    inline index_iterate&	operator++ (void) { ++_i; return *this; }
    inline index_iterate&	operator-- (void) { --_i; return *this; }
    inline index_iterate	operator++ (int) { index_iterate prev (*this); ++_i; return prev; }
    inline index_iterate	operator-- (int) { index_iterate prev (*this); --_i; return prev; }
    inline index_iterate&	operator+= (size_t n) { _i += n; return *this; }
    inline index_iterate&	operator-= (size_t n) { _i -= n; return *this; }
    inline index_iterate	operator+ (size_t n) const { return index_iterate (_base, _i + n); }
    inline index_iterate	operator- (size_t n) const { return index_iterate (_base, _i - n); }
    inline reference		operator[] (uoff_t n) const { return *(*this + n); }
    inline difference_type	operator- (const index_iterate& i) const { return distance (_i, i._i); }
private:
    RandomAccessIterator	_base;
    IndexIterator		_i;
};

/// Returns an index_iterate for \p ibase over \p iindex.
template <typename RandomAccessIterator, typename IndexIterator>
inline index_iterate<RandomAccessIterator, IndexIterator> index_iterator (RandomAccessIterator ibase, IndexIterator iindex)
{
    return index_iterate<RandomAccessIterator, IndexIterator> (ibase, iindex);
}

/// Converts the indexes in \p xc to iterators in \p ic of base \p ibase.
template <typename IndexContainer, typename IteratorContainer>
inline void indexv_to_iteratorv (typename IteratorContainer::value_type ibase, const IndexContainer& xc, IteratorContainer& ic)
{
    ic.resize (xc.size());
    copy_n (index_iterator (ibase, xc.begin()), xc.size(), ic.begin());
}

//----------------------------------------------------------------------

/// Converts the given const_iterator into an iterator.
///
template <typename Container>
inline typename Container::iterator unconst (typename Container::const_iterator i, Container&)
    { return const_cast<typename Container::iterator>(i); }

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#define IBYI(Iter1, Iter2, Ctr1, Ctr2)	\
template <typename Container1, typename Container2>	\
inline typename Container2::Iter2 ibyi (typename Container1::Iter1 idx, Ctr1& ctr1, Ctr2& ctr2)	\
{						\
    assert (ctr1.size() == ctr2.size());	\
    return ctr2.begin() + (idx - ctr1.begin());	\
}

IBYI(const_iterator, const_iterator, const Container1, const Container2)
IBYI(iterator, iterator, Container1, Container2)
IBYI(const_iterator, iterator, const Container1, Container2)
IBYI(iterator, const_iterator, Container1, const Container2)

#else // DOXYGEN

#error "This declaration is for doxygen only; it is not compiled."

/// Converts a const_iterator in one container into a const_iterator in another container.
template <typename Container1, typename Container2>
inline typename Container2::iterator ibyi (typename Container1::iterator idx, Container1& ctr1, Container2& ctr2) {}

#endif // DOXYGEN

//----------------------------------------------------------------------

} // namespace ustl

namespace ustl {

//{{{ auto_ptr -------------------------------------------------------

/// \class auto_ptr umemory.h ustl.h
/// \ingroup MemoryManagement
///
/// \brief A smart pointer.
///
/// Calls delete in the destructor; assignment transfers ownership.
/// This class does not work with void pointers due to the absence
/// of the required dereference operator. auto_ptr is deprecated in
/// c++11; use unique_ptr instead.
///
template <typename T>
class auto_ptr {
public:
    typedef T		value_type;
    typedef T*		pointer;
    typedef T&		reference;
public:
    /// Takes ownership of \p p.
    inline explicit	auto_ptr (pointer p = nullptr)	: _p (p) {}
    /// Takes ownership of pointer in \p p. \p p relinquishes ownership.
    inline		auto_ptr (auto_ptr<T>& p)	: _p (p.release()) {}
    /// Deletes the owned pointer.
    inline		~auto_ptr (void)		{ delete _p; }
    /// Returns the pointer without relinquishing ownership.
    inline pointer	get (void) const		{ return _p; }
    /// Returns the pointer and gives up ownership.
    inline pointer	release (void)			{ pointer rv (_p); _p = nullptr; return rv; }
    /// Deletes the pointer and sets it equal to \p p.
    inline void		reset (pointer p)		{ if (p != _p) { delete _p; _p = p; } }
    /// Takes ownership of \p p.
    inline auto_ptr<T>&	operator= (pointer p)		{ reset (p); return *this; }
    /// Takes ownership of pointer in \p p. \p p relinquishes ownership.
    inline auto_ptr<T>&	operator= (auto_ptr<T>& p)	{ reset (p.release()); return *this; }
    inline reference	operator* (void) const		{ return *_p; }
    inline pointer	operator-> (void) const		{ return _p; }
    inline bool		operator== (const pointer p) const	{ return _p == p; }
    inline bool		operator== (const auto_ptr<T>& p) const	{ return _p == p._p; }
    inline bool		operator< (const auto_ptr<T>& p) const	{ return p._p < _p; }
private:
    pointer		_p;
};

//}}}-------------------------------------------------------------------
//{{{ unique_ptr
#if HAVE_CPP11

/// \class unique_ptr memory.h stl.h
/// \ingroup MemoryManagement
/// \brief A smart pointer.
/// Calls delete in the destructor; assignment transfers ownership.
/// This class does not work with void pointers due to the absence
/// of the required dereference operator.
template <typename T>
class unique_ptr {
public:
    using element_type		= T;
    using pointer		= element_type*;
    using reference		= element_type&;
public:
    inline constexpr		unique_ptr (void)		: _p (nullptr) {}
    inline constexpr explicit	unique_ptr (pointer p)		: _p (p) {}
    inline			unique_ptr (unique_ptr&& p)	: _p (p.release()) {}
				unique_ptr (const unique_ptr&) = delete;
    inline			~unique_ptr (void)		{ delete _p; }
    inline constexpr pointer	get (void) const		{ return _p; }
    inline pointer		release (void)			{ auto rv (_p); _p = nullptr; return rv; }
    inline void			reset (pointer p = nullptr)	{ assert (p != _p || !p); auto ov (_p); _p = p; delete ov; }
    inline void			swap (unique_ptr& v)		{ swap (_p, v._p); }
    inline constexpr explicit	operator bool (void) const	{ return _p != nullptr; }
    inline unique_ptr&		operator= (pointer p)		{ reset (p); return *this; }
    inline unique_ptr&		operator= (unique_ptr&& p)	{ reset (p.release()); return *this; }
    unique_ptr&			operator=(const unique_ptr&) = delete;
    inline constexpr reference	operator* (void) const		{ return *get(); }
    inline constexpr pointer	operator-> (void) const		{ return get(); }
    inline constexpr reference	operator[] (size_t i) const	{ return get()[i]; }
    inline constexpr bool	operator== (const pointer p) const	{ return _p == p; }
    inline constexpr bool	operator== (const unique_ptr& p) const	{ return _p == p._p; }
    inline constexpr bool	operator< (const unique_ptr& p) const	{ return _p < p._p; }
private:
    pointer			_p;
};

// array version
template<typename T>
class unique_ptr<T[]> {
public:
    using element_type		= T;
    using pointer		= element_type*;
    using reference		= element_type&;
public:
    inline constexpr		unique_ptr (void)		: _p (nullptr) {}
    inline constexpr explicit	unique_ptr (pointer p)		: _p (p) {}
    inline			unique_ptr (unique_ptr&& p)	: _p (p.release()) {}
				unique_ptr(const unique_ptr&) = delete;
    inline			~unique_ptr (void)		{ delete [] _p; }
    inline constexpr pointer	get (void) const		{ return _p; }
    inline pointer			release (void)			{ auto rv (_p); _p = nullptr; return rv; }
    inline void			reset (pointer p)		{ assert (p != _p); auto ov (_p); _p = p; delete [] ov; }
    inline void			swap (unique_ptr& v)		{ swap (_p, v._p); }
    inline constexpr explicit	operator bool (void) const	{ return _p != nullptr; }
    inline unique_ptr&		operator= (pointer p)		{ reset (p); return *this; }
    inline unique_ptr&		operator= (unique_ptr&& p)	{ reset (p.release()); return *this; }
    unique_ptr&			operator=(const unique_ptr&) = delete;
    inline constexpr reference	operator* (void) const		{ return *_p; }
    inline constexpr pointer	operator-> (void) const		{ return _p; }
    inline constexpr reference	operator[] (size_t i) const	{ return _p[i]; }
    inline constexpr bool	operator== (const pointer p) const	{ return _p == p; }
    inline constexpr bool	operator== (const unique_ptr& p) const	{ return _p == p._p; }
    inline constexpr bool	operator< (const unique_ptr& p) const	{ return _p < p._p; }
private:
    pointer			_p;
};

#if HAVE_CPP14

template <typename T> struct __make_unique { using __single_object = unique_ptr<T>; };
template <typename T> struct __make_unique<T[]> { using __array = unique_ptr<T[]>; };
template <typename T, size_t N> struct __make_unique<T[N]> { struct __invalid_type {}; };

template <typename T, typename... Args>
inline typename __make_unique<T>::__single_object
    make_unique (Args&&... args) { return unique_ptr<T> (new T (forward<Args>(args)...)); }

template <typename T>
inline typename __make_unique<T>::__array
    make_unique (size_t n) { return unique_ptr<T> (new remove_extent_t<T>[n]()); }

template <typename T, typename... Args>
inline typename __make_unique<T>::__invalid_type
    make_unique (Args&&...) = delete;

#endif // HAVE_CPP14
#endif // HAVE_CPP11

//}}}-------------------------------------------------------------------
//{{{ shared_ptr

#if HAVE_CPP11

/// \class shared_ptr memory.h stl.h
/// \ingroup MemoryManagement
/// \brief A smart pointer.
/// Calls delete in the destructor; assignment shares ownership.
template <typename T>
class shared_ptr {
public:
    using element_type		= T;
    using pointer		= element_type*;
    using reference		= element_type&;
private:
    struct container {
	pointer	p;
	size_t	refs;
	inline constexpr explicit container (pointer np) : p(np),refs(1) {}
	inline	~container (void) noexcept { assert (!refs); delete p; }
    };
public:
    inline constexpr		shared_ptr (void)		: _p (nullptr) {}
    inline explicit		shared_ptr (pointer p)		: _p (new container (p)) {}
    inline			shared_ptr (shared_ptr&& p)	: _p (p._p) { p._p = nullptr; }
    inline			shared_ptr (const shared_ptr& p): _p (p._p) { if (_p) ++_p->refs; }
    inline			~shared_ptr (void)		{ reset(); }
    inline constexpr size_t	use_count (void) const		{ return _p ? _p->refs : 0; }
    inline constexpr bool	unique (void) const		{ return use_count() == 1; }
    inline constexpr pointer	get (void) const		{ return _p ? _p->p : nullptr; }
    void			reset (pointer p = nullptr) {
				    assert (p != get() || !p);
				    auto ov = _p;
				    _p = p ? new container(p) : nullptr;
				    if (ov && !--ov->refs)
					delete ov;
				}
    inline void			swap (shared_ptr& v)		{ swap (_p, v._p); }
    inline constexpr explicit	operator bool (void) const	{ return get(); }
    inline shared_ptr&		operator= (pointer p)		{ reset (p); return *this; }
    inline shared_ptr&		operator= (shared_ptr&& p)	{ swap (p); return *this; }
    inline shared_ptr&		operator= (const shared_ptr& p)	{ reset(); _p = p; if (_p) ++_p->refs; return *this; }
    inline constexpr reference	operator* (void) const		{ return *get(); }
    inline constexpr pointer	operator-> (void) const		{ return get(); }
    inline constexpr reference	operator[] (size_t i) const	{ return get()[i]; }
    inline constexpr bool	operator== (const pointer p) const	{ return get() == p; }
    inline constexpr bool	operator== (const shared_ptr& p) const	{ return get() == p.get(); }
    inline constexpr bool	operator< (const shared_ptr& p) const	{ return get() < p.get(); }
private:
    container*			_p;
};

#if HAVE_CPP14

template <typename T, typename... Args>
inline auto make_shared (Args&&... args)
    { return shared_ptr<T> (new T (forward<Args>(args)...)); }

#endif // HAVE_CPP14

//}}}-------------------------------------------------------------------
//{{{ scope_exit

template <typename F>
class scope_exit {
public:
    inline explicit	scope_exit (F&& f) noexcept		: _f(move(f)),_enabled(true) {}
    inline		scope_exit (scope_exit&& f) noexcept	: _f(move(f._f)),_enabled(f._enabled) { f.release(); }
    inline void		release (void) noexcept			{ _enabled = false; }
    inline		~scope_exit (void) noexcept (noexcept (declval<F>()))	{ if (_enabled) _f(); }
			scope_exit (const scope_exit&) = delete;
    scope_exit&		operator= (const scope_exit&) = delete;
    scope_exit&		operator= (scope_exit&&) = delete;
private:
    F		_f;
    bool	_enabled;
};

#if HAVE_CPP14
template <typename F>
auto make_scope_exit (F&& f) noexcept
    { return scope_exit<remove_reference_t<F>>(forward<F>(f)); }
#endif // HAVE_CPP14

//}}}-------------------------------------------------------------------
//{{{ unique_resource

template <typename R, typename D>
class unique_resource {
public:
    inline explicit	unique_resource (R&& resource, D&& deleter, bool enabled = true) noexcept
			    : _resource(move(resource)), _deleter(move(deleter)),_enabled(enabled) {}
    inline		unique_resource (unique_resource&& r) noexcept
			    : _resource(move(r._resource)),_deleter(move(r._deleter)),_enabled(r._enabled) { r.release(); }
			unique_resource (const unique_resource&) = delete;
    inline		~unique_resource() noexcept(noexcept(declval<unique_resource<R,D>>().reset()))
			    { reset(); }
    inline const D&	get_deleter (void) const noexcept	{ return _deleter; }
    inline R const&	get (void) const noexcept		{ return _resource; }
    inline R const&	release (void) noexcept			{ _enabled = false; return get(); }
    inline void		reset (void) noexcept (noexcept(declval<D>())) {
			    if (_enabled) {
				_enabled = false;
				get_deleter()(_resource);
			    }
			}
    inline void		reset (R&& r) noexcept (noexcept(reset())) {
			    reset();
			    _resource = move(r);
			    _enabled = true;
			}
    unique_resource&	operator= (const unique_resource&) = delete;
    unique_resource&	operator= (unique_resource &&r) noexcept(noexcept(reset())) {
			    reset();
			    _deleter = move(r._deleter);
			    _resource = move(r._resource);
			    _enabled = r._enabled;
			    r.release();
			    return *this;
			}
    inline		operator R const& (void) const noexcept	{ return get(); }
    inline R		operator-> (void) const noexcept	{ return _resource; }
    inline add_lvalue_reference_t<remove_pointer_t<R>>
			operator* (void) const	{ return *_resource; }
private:
    R			_resource;
    D			_deleter;
    bool		_enabled;
};

#if HAVE_CPP14

template <typename R,typename D>
auto make_unique_resource (R&& r, D&& d) noexcept
    { return unique_resource<R,remove_reference_t<D>>(move(r), forward<remove_reference_t<D>>(d), true); }

template <typename R,typename D>
auto make_unique_resource_checked (R r, R invalid, D d) noexcept
{
    bool shouldrun = !(r == invalid);
    return unique_resource<R,D>(move(r), move(d), shouldrun);
}

#endif // HAVE_CPP14
#endif // HAVE_CPP11

//}}}-------------------------------------------------------------------
//{{{ construct and destroy

/// Calls the placement new on \p p.
/// \ingroup RawStorageAlgorithms
///
template <typename T>
inline void construct (T* p)
{
    new (p) T;
}

/// Calls the placement new on \p p.
/// \ingroup RawStorageAlgorithms
///
template <typename ForwardIterator>
inline void construct (ForwardIterator first, ForwardIterator last)
{
    typedef typename iterator_traits<ForwardIterator>::value_type value_type;
#if HAVE_CPP11
    if (is_pod<value_type>::value)
#else
    if (numeric_limits<value_type>::is_integral)
#endif
	memset ((void*) first, 0, max(distance(first,last),0)*sizeof(value_type));
    else
	for (--last; intptr_t(first) <= intptr_t(last); ++first)
	    construct (&*first);
}

/// Calls the placement new on \p p.
/// \ingroup RawStorageAlgorithms
///
template <typename T>
inline void construct (T* p, const T& value)
{
    new (p) T (value);
}

/// Calls the destructor of \p p without calling delete.
/// \ingroup RawStorageAlgorithms
///
template <typename T>
inline void destroy (T* p) noexcept
{
    p->~T();
}

// Helper templates to not instantiate anything for integral types.
template <typename T>
void dtors (T first, T last) noexcept
    { for (--last; intptr_t(first) <= intptr_t(last); ++first) destroy (&*first); }
template <typename T, bool bIntegral>
struct Sdtorsr {
    inline void operator()(T first, T last) noexcept { dtors (first, last); }
};
template <typename T>
struct Sdtorsr<T,true> {
    inline void operator()(T, T) noexcept {}
};

/// Calls the destructor on elements in range [first, last) without calling delete.
/// \ingroup RawStorageAlgorithms
///
template <typename ForwardIterator>
inline void destroy (ForwardIterator first, ForwardIterator last) noexcept
{
    typedef typename iterator_traits<ForwardIterator>::value_type value_type;
#if HAVE_CPP11
    Sdtorsr<ForwardIterator,is_pod<value_type>::value>()(first, last);
#else
    Sdtorsr<ForwardIterator,numeric_limits<value_type>::is_integral>()(first, last);
#endif
}

//}}}-------------------------------------------------------------------
//{{{ Raw storage algorithms

/// Casts \p p to the type of the second pointer argument.
template <typename T> inline T* cast_to_type (void* p, const T*) { return (T*) p; }

/// \brief Creates a temporary buffer pair from \p p and \p n
/// This is intended to be used with alloca to create temporary buffers.
/// The size in the returned pair is set to 0 if the allocation is unsuccessful.
/// \ingroup RawStorageAlgorithms
///
template <typename T>
inline pair<T*, ptrdiff_t> make_temporary_buffer (void* p, size_t n, const T* ptype)
{
    return make_pair (cast_to_type(p,ptype), ptrdiff_t(p ? n : 0));
}

#if HAVE_ALLOCA_H
    /// \brief Allocates a temporary buffer, if possible.
    /// \ingroup RawStorageAlgorithms
    #define get_temporary_buffer(size, ptype)	make_temporary_buffer (alloca(size_of_elements(size, ptype)), size, ptype)
    #define return_temporary_buffer(p)
#else
    #define get_temporary_buffer(size, ptype)	make_temporary_buffer (malloc(size_of_elements(size, ptype)), size, ptype)
    #define return_temporary_buffer(p)		if (p) free (p), p = nullptr
#endif

/// Copies [first, last) into result by calling copy constructors in result.
/// \ingroup RawStorageAlgorithms
///
template <typename InputIterator, typename ForwardIterator>
ForwardIterator uninitialized_copy (InputIterator first, InputIterator last, ForwardIterator result)
{
    for (; first < last; ++result, ++first)
	construct (&*result, *first);
    return result;
}

/// Copies [first, first + n) into result by calling copy constructors in result.
/// \ingroup RawStorageAlgorithms
///
template <typename InputIterator, typename ForwardIterator>
ForwardIterator uninitialized_copy_n (InputIterator first, size_t n, ForwardIterator result)
{
    for (++n; --n; ++result, ++first)
	construct (&*result, *first);
    return result;
}

/// Calls construct on all elements in [first, last) with value \p v.
/// \ingroup RawStorageAlgorithms
///
template <typename ForwardIterator, typename T>
void uninitialized_fill (ForwardIterator first, ForwardIterator last, const T& v)
{
    for (; first < last; ++first)
	construct (&*first, v);
}

/// Calls construct on all elements in [first, first + n) with value \p v.
/// \ingroup RawStorageAlgorithms
///
template <typename ForwardIterator, typename T>
ForwardIterator uninitialized_fill_n (ForwardIterator first, size_t n, const T& v)
{
    for (++n; --n; ++first)
	construct (&*first, v);
    return first;
}
    
} // namespace ustl

//}}}-------------------------------------------------------------------
//{{{ initializer_list
#if HAVE_CPP11

namespace std {	// Internal stuff must be in std::

/// Internal class for compiler support of C++11 initializer lists
template <typename T>
class initializer_list {
public:
    typedef T 			value_type;
    typedef size_t 		size_type;
    typedef const T& 		const_reference;
    typedef const_reference	reference;
    typedef const T* 		const_iterator;
    typedef const_iterator	iterator;
private:
    /// This object is only constructed by the compiler when the {1,2,3}
    /// syntax is used, so the constructor must be private
    inline constexpr		initializer_list (const_iterator p, size_type sz) noexcept : _data(p), _size(sz) {}
public:
    inline constexpr		initializer_list (void)noexcept	: _data(nullptr), _size(0) {}
    inline constexpr size_type	size (void) const noexcept	{ return _size; }
    inline constexpr const_iterator begin() const noexcept	{ return _data; }
    inline constexpr const_iterator end() const noexcept	{ return begin()+size(); }
private:
    iterator			_data;
    size_type			_size;
};

template <typename T>
inline constexpr const T* begin (initializer_list<T> il) noexcept { return il.begin(); }
template <typename T>
inline constexpr const T* end (initializer_list<T> il) noexcept { return il.end(); }

} // namespace std

#endif	// HAVE_CPP11
//}}}-------------------------------------------------------------------

namespace ustl {

/// Swaps corresponding elements of [first, last) and [result,)
/// \ingroup SwapAlgorithms
///
template <typename ForwardIterator1, typename ForwardIterator2>
inline ForwardIterator2 swap_ranges (ForwardIterator1 first, ForwardIterator2 last, ForwardIterator2 result)
{
    for (; first != last; ++first, ++result)
	iter_swap (first, result);
    return result;
}

/// Returns the first iterator i in the range [first, last) such that
/// *i == value. Returns last if no such iterator exists. 
/// \ingroup SearchingAlgorithms
///
template <typename InputIterator, typename EqualityComparable>
inline InputIterator find (InputIterator first, InputIterator last, const EqualityComparable& value)
{
    while (first != last && !(*first == value))
	++ first;
    return first;
}

/// Returns the first iterator such that *i == *(i + 1)
/// \ingroup SearchingAlgorithms
///
template <typename ForwardIterator>
ForwardIterator adjacent_find (ForwardIterator first, ForwardIterator last)
{
    if (first != last)
	for (ForwardIterator prev = first; ++first != last; ++ prev)
	    if (*prev == *first)
		return prev;
    return last;
}

/// Returns the pointer to the first pair of unequal elements.
/// \ingroup SearchingAlgorithms
///
template <typename InputIterator>
pair<InputIterator,InputIterator>
mismatch (InputIterator first1, InputIterator last1, InputIterator first2)
{
    while (first1 != last1 && *first1 == *first2)
	++ first1, ++ first2;
    return make_pair (first1, first2);
}

/// \brief Returns true if two ranges are equal.
/// This is an extension, present in uSTL and SGI STL.
/// \ingroup SearchingAlgorithms
///
template <typename InputIterator>
inline bool equal (InputIterator first1, InputIterator last1, InputIterator first2)
{
    return mismatch (first1, last1, first2).first == last1;
}

/// Count finds the number of elements in [first, last) that are equal
/// to value. More precisely, the first version of count returns the
/// number of iterators i in [first, last) such that *i == value.
/// \ingroup SearchingAlgorithms
///
template <typename InputIterator, typename EqualityComparable>
inline size_t count (InputIterator first, InputIterator last, const EqualityComparable& value)
{
    size_t total = 0;
    for (; first != last; ++first)
	if (*first == value)
	    ++ total;
    return total;
}

///
/// The first version of transform performs the operation op(*i) for each
/// iterator i in the range [first, last), and assigns the result of that
/// operation to *o, where o is the corresponding output iterator. That is,
/// for each n such that 0 <= n < last - first, it performs the assignment
/// *(result + n) = op(*(first + n)).
/// The return value is result + (last - first).
/// \ingroup MutatingAlgorithms
/// \ingroup PredicateAlgorithms
///
template <typename InputIterator, typename OutputIterator, typename UnaryFunction>
inline OutputIterator transform (InputIterator first, InputIterator last, OutputIterator result, UnaryFunction op)
{
    for (; first != last; ++result, ++first)
	*result = op (*first);
    return result;
}

///
/// The second version of transform is very similar, except that it uses a
/// Binary Function instead of a Unary Function: it performs the operation
/// op(*i1, *i2) for each iterator i1 in the range [first1, last1) and assigns
/// the result to *o, where i2 is the corresponding iterator in the second
/// input range and where o is the corresponding output iterator. That is,
/// for each n such that 0 <= n < last1 - first1, it performs the assignment
/// *(result + n) = op(*(first1 + n), *(first2 + n).
/// The return value is result + (last1 - first1).
/// \ingroup MutatingAlgorithms
/// \ingroup PredicateAlgorithms
///
template <typename InputIterator1, typename InputIterator2, typename OutputIterator, typename BinaryFunction>
inline OutputIterator transform (InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, OutputIterator result, BinaryFunction op)
{
    for (; first1 != last1; ++result, ++first1, ++first2)
	*result = op (*first1, *first2);
    return result;
}

/// Replace replaces every element in the range [first, last) equal to
/// old_value with new_value. That is: for every iterator i,
/// if *i == old_value then it performs the assignment *i = new_value.
/// \ingroup MutatingAlgorithms
///
template <typename ForwardIterator, typename T>
inline void replace (ForwardIterator first, ForwardIterator last, const T& old_value, const T& new_value)
{
    for (; first != last; ++first)
	if (*first == old_value)
	    *first = new_value;
}

/// Replace_copy copies elements from the range [first, last) to the range
/// [result, result + (last-first)), except that any element equal to old_value
/// is not copied; new_value is copied instead. More precisely, for every
/// integer n such that 0 <= n < last-first, replace_copy performs the
/// assignment *(result+n) = new_value if *(first+n) == old_value, and
/// *(result+n) = *(first+n) otherwise.
/// \ingroup MutatingAlgorithms
///
template <typename InputIterator, typename OutputIterator, typename T>
inline OutputIterator replace_copy (InputIterator first, InputIterator last, OutputIterator result, const T& old_value, const T& new_value)
{
    for (; first != last; ++result, ++first)
        *result = (*first == old_value) ? new_value : *first;
}

/// Generate assigns the result of invoking gen, a function object that
/// takes no arguments, to each element in the range [first, last).
/// \ingroup GeneratorAlgorithms
/// \ingroup PredicateAlgorithms
///
template <typename ForwardIterator, typename Generator>
inline void generate (ForwardIterator first, ForwardIterator last, Generator gen)
{
    for (; first != last; ++first)
	*first = gen();
}

/// Generate_n assigns the result of invoking gen, a function object that
/// takes no arguments, to each element in the range [first, first+n).
/// The return value is first + n.
/// \ingroup GeneratorAlgorithms
/// \ingroup PredicateAlgorithms
///
template <typename OutputIterator, typename Generator>
inline OutputIterator generate_n (OutputIterator first, size_t n, Generator gen)
{
    for (uoff_t i = 0; i != n; ++i, ++first)
	*first = gen();
    return first;
}

/// \brief Reverse reverses a range.
/// That is: for every i such that 0 <= i <= (last - first) / 2),
/// it exchanges *(first + i) and *(last - (i + 1)).
/// \ingroup MutatingAlgorithms
///
template <typename BidirectionalIterator>
inline void reverse (BidirectionalIterator first, BidirectionalIterator last)
{
    for (; distance (first, --last) > 0; ++first)
	iter_swap (first, last);
}

/// \brief Reverses [first,last) and writes it to \p output.
/// \ingroup MutatingAlgorithms
///
template <typename BidirectionalIterator, typename OutputIterator>
inline OutputIterator reverse_copy (BidirectionalIterator first, BidirectionalIterator last, OutputIterator result)
{
    for (; first != last; ++result)
	*result = *--last;
    return result;
}

/// \brief Exchanges ranges [first, middle) and [middle, last)
/// \ingroup MutatingAlgorithms
///
template <typename ForwardIterator>
ForwardIterator rotate (ForwardIterator first, ForwardIterator middle, ForwardIterator last)
{
    if (first == middle || middle == last)
	return first;
    reverse (first, middle);
    reverse (middle, last);
    for (;first != middle && middle != last; ++first)
	iter_swap (first, --last);
    reverse (first, (first == middle ? last : middle));
    return first;
}

/// Specialization for pointers, which can be treated identically.
template <typename T>
inline T* rotate (T* first, T* middle, T* last)
{
    rotate_fast (first, middle, last);
    return first;
}
 

/// \brief Exchanges ranges [first, middle) and [middle, last) into \p result.
/// \ingroup MutatingAlgorithms
///
template <typename ForwardIterator, typename OutputIterator>
inline OutputIterator rotate_copy (ForwardIterator first, ForwardIterator middle, ForwardIterator last, OutputIterator result)
{
    return copy (first, middle, copy (middle, last, result));
}

/// \brief Combines two sorted ranges.
/// \ingroup SortingAlgorithms
///
template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
OutputIterator merge (InputIterator1 first1, InputIterator1 last1,
		      InputIterator2 first2, InputIterator2 last2, OutputIterator result)
{
    for (; first1 != last1 && first2 != last2; ++result) {
	if (*first1 < *first2)
	    *result = *first1++;
	else
	    *result = *first2++;
    }
    if (first1 < last1)
	return copy (first1, last1, result);
    else
	return copy (first2, last2, result);
}

/// Combines two sorted ranges from the same container.
/// \ingroup SortingAlgorithms
///
template <typename InputIterator>
void inplace_merge (InputIterator first, InputIterator middle, InputIterator last)
{
    for (; middle != last; ++first) {
	while (*first < *middle)
	    ++ first;
	reverse (first, middle);
	reverse (first, ++middle);
    }
}

/// Remove_copy copies elements that are not equal to value from the range
/// [first, last) to a range beginning at result. The return value is the
/// end of the resulting range. This operation is stable, meaning that the
/// relative order of the elements that are copied is the same as in the
/// range [first, last).
/// \ingroup MutatingAlgorithms
///
template <typename InputIterator, typename OutputIterator, typename T>
OutputIterator remove_copy (InputIterator first, InputIterator last, OutputIterator result, const T& value)
{
    for (; first != last; ++first) {
	if (!(*first == value)) {
	    *result = *first;
	    ++ result;
	}
    }
    return result;
}

/// Remove_copy copies elements pointed to by iterators in [rfirst, rlast)
/// from the range [first, last) to a range beginning at result. The return
/// value is the end of the resulting range. This operation is stable, meaning
/// that the relative order of the elements that are copied is the same as in the
/// range [first, last). Range [rfirst, rlast) is assumed to be sorted.
/// This algorithm is a uSTL extension.
/// \ingroup MutatingAlgorithms
///
template <typename InputIterator, typename OutputIterator, typename RInputIterator>
OutputIterator remove_copy (InputIterator first, InputIterator last, OutputIterator result, RInputIterator rfirst, RInputIterator rlast)
{
    for (; first != last; ++first) {
	while (rfirst != rlast && *rfirst < first)
	    ++ rfirst;
	if (rfirst == rlast || first != *rfirst) {
	    *result = *first;
	    ++ result;
	}
    }
    return result;
}

/// Remove removes from the range [first, last) all elements that are equal to
/// value. That is, remove returns an iterator new_last such that the range
/// [first, new_last) contains no elements equal to value. [1] The iterators
/// in the range [new_last, last) are all still dereferenceable, but the
/// elements that they point to are unspecified. Remove is stable, meaning
/// that the relative order of elements that are not equal to value is
/// unchanged.
/// \ingroup MutatingAlgorithms
///
template <typename ForwardIterator, typename T>
inline ForwardIterator remove (ForwardIterator first, ForwardIterator last, const T& value)
{
    return remove_copy (first, last, first, value);
}

/// Unique_copy copies elements from the range [first, last) to a range
/// beginning with result, except that in a consecutive group of duplicate
/// elements only the first one is copied. The return value is the end of
/// the range to which the elements are copied. This behavior is similar
/// to the Unix filter uniq.
/// \ingroup MutatingAlgorithms
///
template <typename InputIterator, typename OutputIterator>
OutputIterator unique_copy (InputIterator first, InputIterator last, OutputIterator result)
{
    if (first != last) {
	*result = *first;
	while (++first != last)
	    if (!(*first == *result))
		*++result = *first;
	++ result;
    }
    return result;
}

/// Every time a consecutive group of duplicate elements appears in the range
/// [first, last), the algorithm unique removes all but the first element.
/// That is, unique returns an iterator new_last such that the range [first,
/// new_last) contains no two consecutive elements that are duplicates.
/// The iterators in the range [new_last, last) are all still dereferenceable,
/// but the elements that they point to are unspecified. Unique is stable,
/// meaning that the relative order of elements that are not removed is
/// unchanged.
/// \ingroup MutatingAlgorithms
///
template <typename ForwardIterator>
inline ForwardIterator unique (ForwardIterator first, ForwardIterator last)
{
    return unique_copy (first, last, first);
}

/// Returns the furthermost iterator i in [first, last) such that,
/// for every iterator j in [first, i), *j < value
/// Assumes the range is sorted.
/// \ingroup SearchingAlgorithms
///
template <typename ForwardIterator, typename LessThanComparable>
ForwardIterator lower_bound (ForwardIterator first, ForwardIterator last, const LessThanComparable& value)
{
    ForwardIterator mid;
    while (first != last) {
	mid = advance (first, size_t(distance (first,last)) / 2);
	if (*mid < value)
	    first = mid + 1;
	else
	    last = mid;
    }
    return first;
}

/// Performs a binary search inside the sorted range.
/// \ingroup SearchingAlgorithms
///
template <typename ForwardIterator, typename LessThanComparable>
inline bool binary_search (ForwardIterator first, ForwardIterator last, const LessThanComparable& value)
{
    ForwardIterator found = lower_bound (first, last, value);
    return found != last && !(value < *found);
}

/// Returns the furthermost iterator i in [first,last) such that for
/// every iterator j in [first,i), value < *j is false.
/// \ingroup SearchingAlgorithms
///
template <typename ForwardIterator, typename LessThanComparable>
ForwardIterator upper_bound (ForwardIterator first, ForwardIterator last, const LessThanComparable& value)
{
    ForwardIterator mid;
    while (first != last) {
	mid = advance (first, size_t(distance (first,last)) / 2);
	if (value < *mid)
	    last = mid;
	else
	    first = mid + 1;
    }
    return last;
}

/// Returns pair<lower_bound,upper_bound>
/// \ingroup SearchingAlgorithms
///
template <typename ForwardIterator, typename LessThanComparable>
inline pair<ForwardIterator,ForwardIterator> equal_range (ForwardIterator first, ForwardIterator last, const LessThanComparable& value)
{
    pair<ForwardIterator,ForwardIterator> rv;
    rv.second = rv.first = lower_bound (first, last, value);
    while (rv.second != last && !(value < *(rv.second)))
	++ rv.second;
    return rv;
}

/// Randomly permute the elements of the container.
/// \ingroup GeneratorAlgorithms
///
template <typename RandomAccessIterator>
void random_shuffle (RandomAccessIterator first, RandomAccessIterator last)
{
    for (; first != last; ++ first)
	iter_swap (first, first + (rand() % distance (first, last)));
}

/// \brief Generic compare function adaptor to pass to qsort
/// \ingroup FunctorObjects
template <typename ConstPointer, typename Compare>
int qsort_adapter (const void* p1, const void* p2)
{
    ConstPointer i1 = reinterpret_cast<ConstPointer>(p1);
    ConstPointer i2 = reinterpret_cast<ConstPointer>(p2);
    Compare comp;
    return comp (*i1, *i2) ? -1 : (comp (*i2, *i1) ? 1 : 0);
}

/// Sorts the container
/// \ingroup SortingAlgorithms
/// \ingroup PredicateAlgorithms
///
template <typename RandomAccessIterator, typename Compare>
void sort (RandomAccessIterator first, RandomAccessIterator last, Compare)
{
    typedef typename iterator_traits<RandomAccessIterator>::value_type value_type;
    typedef typename iterator_traits<RandomAccessIterator>::const_pointer const_pointer;
    qsort (first, distance (first, last), sizeof(value_type),
	   &qsort_adapter<const_pointer, Compare>);
}

/// Sorts the container
/// \ingroup SortingAlgorithms
///
template <typename RandomAccessIterator>
inline void sort (RandomAccessIterator first, RandomAccessIterator last)
{
    typedef typename iterator_traits<RandomAccessIterator>::value_type value_type;
    sort (first, last, less<value_type>());
}

/// Sorts the container preserving order of equal elements.
/// \ingroup SortingAlgorithms
/// \ingroup PredicateAlgorithms
///
template <typename RandomAccessIterator, typename Compare>
void stable_sort (RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
    for (RandomAccessIterator j, i = first; ++i < last;) { // Insertion sort
	for (j = i; j-- > first && comp(*i, *j);) ;
	if (++j != i) rotate (j, i, i + 1);
    }
}

/// Sorts the container
/// \ingroup SortingAlgorithms
///
template <typename RandomAccessIterator>
inline void stable_sort (RandomAccessIterator first, RandomAccessIterator last)
{
    typedef typename iterator_traits<RandomAccessIterator>::value_type value_type;
    stable_sort (first, last, less<value_type>());
}

/// \brief Searches for the first subsequence [first2,last2) in [first1,last1)
/// \ingroup SearchingAlgorithms
template <typename ForwardIterator1, typename ForwardIterator2>
inline ForwardIterator1 search (ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2)
{
    typedef typename iterator_traits<ForwardIterator1>::value_type value_type;
    return search (first1, last1, first2, last2, equal_to<value_type>());
}

/// \brief Searches for the last subsequence [first2,last2) in [first1,last1)
/// \ingroup SearchingAlgorithms
template <typename ForwardIterator1, typename ForwardIterator2>
inline ForwardIterator1 find_end (ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2)
{
    typedef typename iterator_traits<ForwardIterator1>::value_type value_type;
    return find_end (first1, last1, first2, last2, equal_to<value_type>());
}

/// \brief Searches for the first occurence of \p count \p values in [first, last)
/// \ingroup SearchingAlgorithms
template <typename Iterator, typename T>
inline Iterator search_n (Iterator first, Iterator last, size_t count, const T& value)
{
    typedef typename iterator_traits<Iterator>::value_type value_type;
    return search_n (first, last, count, value, equal_to<value_type>());
}

/// \brief Searches [first1,last1) for the first occurrence of an element from [first2,last2)
/// \ingroup SearchingAlgorithms
template <typename InputIterator, typename ForwardIterator>
inline InputIterator find_first_of (InputIterator first1, InputIterator last1, ForwardIterator first2, ForwardIterator last2)
{
    typedef typename iterator_traits<InputIterator>::value_type value_type;
    return find_first_of (first1, last1, first2, last2, equal_to<value_type>());
}

/// \brief Returns true if [first2,last2) is a subset of [first1,last1)
/// \ingroup ConditionAlgorithms
/// \ingroup SetAlgorithms
template <typename InputIterator1, typename InputIterator2>
inline bool includes (InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2)
{
    typedef typename iterator_traits<InputIterator1>::value_type value_type;
    return includes (first1, last1, first2, last2, less<value_type>());
}

/// \brief Merges [first1,last1) with [first2,last2)
///
/// Result will contain every element that is in either set. If duplicate
/// elements are present, max(n,m) is placed in the result.
///
/// \ingroup SetAlgorithms
template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
inline OutputIterator set_union (InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result)
{
    typedef typename iterator_traits<InputIterator1>::value_type value_type;
    return set_union (first1, last1, first2, last2, result, less<value_type>());
}

/// \brief Creates a set containing elements shared by the given ranges.
/// \ingroup SetAlgorithms
template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
inline OutputIterator set_intersection (InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result)
{
    typedef typename iterator_traits<InputIterator1>::value_type value_type;
    return set_intersection (first1, last1, first2, last2, result, less<value_type>());
}

/// \brief Removes from [first1,last1) elements present in [first2,last2)
/// \ingroup SetAlgorithms
template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
inline OutputIterator set_difference (InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result)
{
    typedef typename iterator_traits<InputIterator1>::value_type value_type;
    return set_difference (first1, last1, first2, last2, result, less<value_type>());
}

/// \brief Performs union of sets A-B and B-A.
/// \ingroup SetAlgorithms
template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
inline OutputIterator set_symmetric_difference (InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result)
{
    typedef typename iterator_traits<InputIterator1>::value_type value_type;
    return set_symmetric_difference (first1, last1, first2, last2, result, less<value_type>());
}

/// \brief Returns true if the given range is sorted.
/// \ingroup ConditionAlgorithms
template <typename ForwardIterator>
inline bool is_sorted (ForwardIterator first, ForwardIterator last)
{
    typedef typename iterator_traits<ForwardIterator>::value_type value_type;
    return is_sorted (first, last, less<value_type>());
}

/// \brief Compares two given containers like strcmp compares strings.
/// \ingroup ConditionAlgorithms
template <typename InputIterator1, typename InputIterator2>
inline bool lexicographical_compare (InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2)
{
    typedef typename iterator_traits<InputIterator1>::value_type value_type;
    return lexicographical_compare (first1, last1, first2, last2, less<value_type>());
}

/// \brief Creates the next lexicographical permutation of [first,last).
/// Returns false if no further permutations can be created.
/// \ingroup GeneratorAlgorithms
template <typename BidirectionalIterator>
inline bool next_permutation (BidirectionalIterator first, BidirectionalIterator last)
{
    typedef typename iterator_traits<BidirectionalIterator>::value_type value_type;
    return next_permutation (first, last, less<value_type>());
}

/// \brief Creates the previous lexicographical permutation of [first,last).
/// Returns false if no further permutations can be created.
/// \ingroup GeneratorAlgorithms
template <typename BidirectionalIterator>
inline bool prev_permutation (BidirectionalIterator first, BidirectionalIterator last)
{
    typedef typename iterator_traits<BidirectionalIterator>::value_type value_type;
    return prev_permutation (first, last, less<value_type>());
}

/// \brief Returns iterator to the max element in [first,last)
/// \ingroup SearchingAlgorithms
template <typename ForwardIterator>
inline ForwardIterator max_element (ForwardIterator first, ForwardIterator last)
{
    typedef typename iterator_traits<ForwardIterator>::value_type value_type;
    return max_element (first, last, less<value_type>());
}

/// \brief Returns iterator to the min element in [first,last)
/// \ingroup SearchingAlgorithms
template <typename ForwardIterator>
inline ForwardIterator min_element (ForwardIterator first, ForwardIterator last)
{
    typedef typename iterator_traits<ForwardIterator>::value_type value_type;
    return min_element (first, last, less<value_type>());
}

/// \brief Makes [first,middle) a part of the sorted array.
/// Contents of [middle,last) is undefined. This implementation just calls stable_sort.
/// \ingroup SortingAlgorithms
template <typename RandomAccessIterator>
inline void partial_sort (RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last)
{
    typedef typename iterator_traits<RandomAccessIterator>::value_type value_type;
    partial_sort (first, middle, last, less<value_type>());
}

/// \brief Puts \p nth element into its sorted position.
/// In this implementation, the entire array is sorted. I can't think of any
/// use for it where the time gained would be useful.
/// \ingroup SortingAlgorithms
/// \ingroup SearchingAlgorithms
///
template <typename RandomAccessIterator>
inline void nth_element (RandomAccessIterator first, RandomAccessIterator nth, RandomAccessIterator last)
{
    partial_sort (first, nth, last);
}

/// \brief Like partial_sort, but outputs to [result_first,result_last)
/// \ingroup SortingAlgorithms
template <typename InputIterator, typename RandomAccessIterator>
inline RandomAccessIterator partial_sort_copy (InputIterator first, InputIterator last, RandomAccessIterator result_first, RandomAccessIterator result_last)
{
    typedef typename iterator_traits<InputIterator>::value_type value_type;
    return partial_sort_copy (first, last, result_first, result_last, less<value_type>());
}

} // namespace ustl

namespace ustl {

/// \class memlink memlink.h ustl.h
/// \ingroup MemoryManagement
///
/// \brief Wrapper for pointer to block with size.
///
/// Use this class the way you would a pointer to an allocated unstructured block.
/// The pointer and block size are available through member functions and cast operator.
///
/// Example usage:
/// \code
///     void* p = malloc (46721);
///     memlink a, b;
///     a.link (p, 46721);
///     assert (a.size() == 46721));
///     b = a;
///     assert (b.size() == 46721));
///     assert (b.begin() + 34 == a.begin + 34);
///     assert (0 == memcmp (a, b, 12));
///     a.fill (673, b, 42, 67);
///     b.erase (87, 12);
/// \endcode
///
class memlink : public cmemlink {
public:
    typedef value_type*			pointer;
    typedef cmemlink::pointer		const_pointer;
    typedef cmemlink::const_iterator	const_iterator;
    typedef pointer			iterator;
    typedef const memlink&		rcself_t;
public:
    inline		memlink (void)				: cmemlink() {}
    inline		memlink (void* p, size_type n)		: cmemlink (p, n) {}
    inline		memlink (const void* p, size_type n)	: cmemlink (p, n) {}
    inline		memlink (rcself_t l)			: cmemlink (l) {}
    inline explicit	memlink (const cmemlink& l)		: cmemlink (l) {}
    inline pointer	data (void)				{ return const_cast<pointer>(cmemlink::data()); }
   inline const_pointer	data (void) const			{ return cmemlink::data(); }
    inline iterator	begin (void)				{ return iterator (data()); }
    inline iterator	iat (size_type i)			{ assert (i <= size()); return begin() + i; }
    inline iterator	end (void)				{ return iat (size()); }
    inline const_iterator	begin (void) const		{ return cmemlink::begin(); }
    inline const_iterator	end (void) const		{ return cmemlink::end(); }
    inline const_iterator	iat (size_type i) const		{ return cmemlink::iat (i); }
    size_type		writable_size (void) const		{ return size(); }
    inline rcself_t	operator= (const cmemlink& l)		{ cmemlink::operator= (l); return *this; }
    inline rcself_t	operator= (rcself_t l)			{ cmemlink::operator= (l); return *this; }
    inline void		link (const void* p, size_type n)	{ cmemlink::link (p, n); }
    inline void		link (void* p, size_type n)		{ cmemlink::link (p, n); }
    inline void		link (const cmemlink& l)		{ cmemlink::link (l); }
    inline void		link (memlink& l)			{ cmemlink::link (l); }
    inline void		link (const void* first, const void* last)	{ link (first, distance (first, last)); }
    inline void		link (void* first, void* last)		{ link (first, distance (first, last)); }
    inline void		relink (const void* p, size_type n)	{ cmemlink::relink (p, n); }
    inline void		relink (void* p, size_type n)		{ cmemlink::relink (p, n); }
    inline void		swap (memlink& l)			{ cmemlink::swap (l); }
    void		fill (const_iterator start, const void* p, size_type elsize, size_type elCount = 1) noexcept;
    inline void		insert (const_iterator start, size_type size);
    inline void		erase (const_iterator start, size_type size);
    void		read (istream& is);
};

/// Shifts the data in the linked block from \p start to \p start + \p n.
/// The contents of the uncovered bytes is undefined.
inline void memlink::insert (const_iterator cstart, size_type n)
{
    assert (data() || !n);
    assert (cmemlink::begin() || !n);
    assert (cstart >= begin() && cstart + n <= end());
    iterator start = const_cast<iterator>(cstart);
    rotate (start, end() - n, end());
}

/// Shifts the data in the linked block from \p start + \p n to \p start.
/// The contents of the uncovered bytes is undefined.
inline void memlink::erase (const_iterator cstart, size_type n)
{
    assert (data() || !n);
    assert (cmemlink::begin() || !n);
    assert (cstart >= begin() && cstart + n <= end());
    iterator start = const_cast<iterator>(cstart);
    rotate (start, start + n, end());
}

/// Use with memlink-derived classes to allocate and link to stack space.
#define alloca_link(m,n)	(m).link (alloca (n), (n))

} // namespace ustl

namespace ustl {

/// \class memblock memblock.h ustl.h
/// \ingroup MemoryManagement
///
/// \brief Allocated memory block.
///
/// Adds memory management capabilities to memlink. Uses malloc and realloc to
/// maintain the internal pointer, but only if allocated using members of this class,
/// or if linked to using the Manage() member function. Managed memory is automatically
/// freed in the destructor.
///
class memblock : public memlink {
public:
				memblock (void) noexcept;
				memblock (const void* p, size_type n);
    explicit			memblock (size_type n);
    explicit			memblock (const cmemlink& b);
    explicit			memblock (const memlink& b);
				memblock (const memblock& b);
    virtual			~memblock (void) noexcept;
    virtual void		unlink (void) noexcept override;
    inline void			assign (const cmemlink& l)	{ assign (l.cdata(), l.readable_size()); }
    inline const memblock&	operator= (const cmemlink& l)	{ assign (l); return *this; }
    inline const memblock&	operator= (const memlink& l)	{ assign (l); return *this; }
    inline const memblock&	operator= (const memblock& l)	{ assign (l); return *this; }
    inline void			swap (memblock& l) noexcept	{ memlink::swap (l); ::ustl::swap (_capacity, l._capacity); }
    void			assign (const void* p, size_type n);
    void			reserve (size_type newSize, bool bExact = false);
    void			resize (size_type newSize, bool bExact = true);
    iterator			insert (const_iterator start, size_type size);
    iterator			erase (const_iterator start, size_type size);
    inline void			clear (void) noexcept		{ resize (0); }
    inline size_type		capacity (void) const		{ return _capacity; }
    inline bool			is_linked (void) const		{ return !capacity(); }
    inline size_type		max_size (void) const		{ return is_linked() ? memlink::max_size() : SIZE_MAX; }
    inline void			manage (memlink& l)		{ manage (l.begin(), l.size()); }
    void			deallocate (void) noexcept;
    void			shrink_to_fit (void);
    void			manage (void* p, size_type n) noexcept;
    void			copy_link (void);
    void			read (istream& is);
    void			read_file (const char* filename);
#if HAVE_CPP11
    inline			memblock (memblock&& b)		: memlink(), _capacity(0) { swap (b); }
    inline memblock&		operator= (memblock&& b)	{ swap (b); return *this; }
#endif
protected:
    virtual size_type		minimumFreeCapacity (void) const noexcept __attribute__((const));
private:
    size_type			_capacity;	///< Number of bytes allocated by Resize.
};

} // namespace ustl

namespace ustl {

/// Copy_if copies elements from the range [first, last) to the range
/// [result, result + (last - first)) if pred(*i) returns true.
/// \ingroup MutatingAlgorithms
/// \ingroup PredicateAlgorithms
///
template <typename InputIterator, typename OutputIterator, typename Predicate>
inline OutputIterator copy_if (InputIterator first, InputIterator last, OutputIterator result, Predicate pred)
{
    for (; first != last; ++first) {
	if (pred(*first)) {
	    *result = *first;
	    ++ result;
	}
    }
    return result;
}

/// Returns the first iterator i in the range [first, last) such that
/// pred(*i) is true. Returns last if no such iterator exists.
/// \ingroup SearchingAlgorithms
/// \ingroup PredicateAlgorithms
///
template <typename InputIterator, typename Predicate>
inline InputIterator find_if (InputIterator first, InputIterator last, Predicate pred)
{
    while (first != last && !pred (*first))
	++ first;
    return first;
}

/// Returns the first iterator such that p(*i, *(i + 1)) == true.
/// \ingroup SearchingAlgorithms
/// \ingroup PredicateAlgorithms
///
template <typename ForwardIterator, typename BinaryPredicate>
inline ForwardIterator adjacent_find (ForwardIterator first, ForwardIterator last, BinaryPredicate p)
{
    if (first != last)
	for (ForwardIterator prev = first; ++first != last; ++ prev)
	    if (p (*prev, *first))
		return prev;
    return last;
}

/// Returns the pointer to the first pair of unequal elements.
/// \ingroup SearchingAlgorithms
/// \ingroup PredicateAlgorithms
///
template <typename InputIterator, typename BinaryPredicate>
inline pair<InputIterator,InputIterator>
mismatch (InputIterator first1, InputIterator last1, InputIterator first2, BinaryPredicate comp)
{
    while (first1 != last1 && comp(*first1, *first2))
	++ first1, ++ first2;
    return make_pair (first1, first2);
}

/// Returns true if two ranges are equal.
/// This is an extension, present in uSTL and SGI STL.
/// \ingroup ConditionAlgorithms
/// \ingroup PredicateAlgorithms
///
template <typename InputIterator, typename BinaryPredicate>
inline bool equal (InputIterator first1, InputIterator last1, InputIterator first2, BinaryPredicate comp)
{
    return mismatch (first1, last1, first2, comp).first == last1;
}

/// Count_if finds the number of elements in [first, last) that satisfy the
/// predicate pred. More precisely, the first version of count_if returns the
/// number of iterators i in [first, last) such that pred(*i) is true.
/// \ingroup ConditionAlgorithms
/// \ingroup PredicateAlgorithms
///
template <typename InputIterator, typename Predicate>
inline size_t count_if (InputIterator first, InputIterator last, Predicate pred)
{
    size_t total = 0;
    for (; first != last; ++first)
	if (pred (*first))
	    ++ total;
    return total;
}

/// Replace_if replaces every element in the range [first, last) for which
/// pred returns true with new_value. That is: for every iterator i, if
/// pred(*i) is true then it performs the assignment *i = new_value.
/// \ingroup MutatingAlgorithms
/// \ingroup PredicateAlgorithms
///
template <typename ForwardIterator, typename Predicate, typename T>
inline void replace_if (ForwardIterator first, ForwardIterator last, Predicate pred, const T& new_value)
{
    for (; first != last; ++first)
	if (pred (*first))
	    *first = new_value;
}

/// Replace_copy_if copies elements from the range [first, last) to the range
/// [result, result + (last-first)), except that any element for which pred is
/// true is not copied; new_value is copied instead. More precisely, for every
/// integer n such that 0 <= n < last-first, replace_copy_if performs the
/// assignment *(result+n) = new_value if pred(*(first+n)),
/// and *(result+n) = *(first+n) otherwise.
/// \ingroup MutatingAlgorithms
/// \ingroup PredicateAlgorithms
///
template <typename InputIterator, typename OutputIterator, typename Predicate, typename T>
inline OutputIterator replace_copy_if (InputIterator first, InputIterator last, OutputIterator result, Predicate pred, const T& new_value) 
{
    for (; first != last; ++result, ++first)
        *result = pred(*first) ? new_value : *first;
}

/// Remove_copy_if copies elements from the range [first, last) to a range
/// beginning at result, except that elements for which pred is true are not
/// copied. The return value is the end of the resulting range. This operation
/// is stable, meaning that the relative order of the elements that are copied
/// is the same as in the range [first, last).
/// \ingroup MutatingAlgorithms
/// \ingroup PredicateAlgorithms
///
template <typename InputIterator, typename OutputIterator, typename Predicate>
inline OutputIterator remove_copy_if (InputIterator first, InputIterator last, OutputIterator result, Predicate pred)
{
    for (; first != last; ++first)
	if (pred (*first))
	    *result++ = *first;
    return result;
}

/// Remove_if removes from the range [first, last) every element x such that
/// pred(x) is true. That is, remove_if returns an iterator new_last such that
/// the range [first, new_last) contains no elements for which pred is true.
/// The iterators in the range [new_last, last) are all still dereferenceable,
/// but the elements that they point to are unspecified. Remove_if is stable,
/// meaning that the relative order of elements that are not removed is
/// unchanged.
/// \ingroup MutatingAlgorithms
/// \ingroup PredicateAlgorithms
///
template <typename ForwardIterator, typename Predicate>
inline ForwardIterator remove_if (ForwardIterator first, ForwardIterator last, Predicate pred)
{
    return remove_copy_if (first, last, first, pred);
}

/// The reason there are two different versions of unique_copy is that there
/// are two different definitions of what it means for a consecutive group of
/// elements to be duplicates. In the first version, the test is simple
/// equality: the elements in a range [f, l) are duplicates if, for every
/// iterator i in the range, either i == f or else *i == *(i-1). In the second,
/// the test is an arbitrary Binary Predicate binary_pred: the elements in
/// [f, l) are duplicates if, for every iterator i in the range, either
/// i == f or else binary_pred(*i, *(i-1)) is true.
/// \ingroup MutatingAlgorithms
/// \ingroup PredicateAlgorithms
///
template <typename InputIterator, typename OutputIterator, typename BinaryPredicate>
OutputIterator unique_copy (InputIterator first, InputIterator last, OutputIterator result, BinaryPredicate binary_pred)
{
    if (first != last) {
	*result = *first;
	while (++first != last)
	    if (!binary_pred (*first, *result))
		*++result = *first;
	++ result;
    }
    return result;
}

/// Every time a consecutive group of duplicate elements appears in the range
/// [first, last), the algorithm unique removes all but the first element.
/// That is, unique returns an iterator new_last such that the range [first,
/// new_last) contains no two consecutive elements that are duplicates.
/// The iterators in the range [new_last, last) are all still dereferenceable,
/// but the elements that they point to are unspecified. Unique is stable,
/// meaning that the relative order of elements that are not removed is
/// unchanged.
/// \ingroup MutatingAlgorithms
/// \ingroup PredicateAlgorithms
///
template <typename ForwardIterator, typename BinaryPredicate>
inline ForwardIterator unique (ForwardIterator first, ForwardIterator last, BinaryPredicate binary_pred)
{
    return unique_copy (first, last, first, binary_pred);
}

/// Returns the furthermost iterator i in [first, last) such that,
/// for every iterator j in [first, i), comp(*j, value) is true.
/// Assumes the range is sorted.
/// \ingroup SearchingAlgorithms
/// \ingroup PredicateAlgorithms
///
template <typename ForwardIterator, typename T, typename StrictWeakOrdering>
ForwardIterator lower_bound (ForwardIterator first, ForwardIterator last, const T& value, StrictWeakOrdering comp)
{
    ForwardIterator mid;
    while (first != last) {
	mid = advance (first, size_t(distance (first,last)) / 2);
	if (comp (*mid, value))
	    first = mid + 1;
	else
	    last = mid;
    }
    return first;
}

/// Performs a binary search inside the sorted range.
/// \ingroup SearchingAlgorithms
/// \ingroup PredicateAlgorithms
///
template <typename ForwardIterator, typename T, typename StrictWeakOrdering>
inline bool binary_search (ForwardIterator first, ForwardIterator last, const T& value, StrictWeakOrdering comp)
{
    ForwardIterator found = lower_bound (first, last, value, comp);
    return found != last && !comp(*found, value);
}

/// Returns the furthermost iterator i in [first,last) such that for
/// every iterator j in [first,i), comp(value,*j) is false.
/// \ingroup SearchingAlgorithms
/// \ingroup PredicateAlgorithms
///
template <typename ForwardIterator, typename T, typename StrictWeakOrdering>
ForwardIterator upper_bound (ForwardIterator first, ForwardIterator last, const T& value, StrictWeakOrdering comp)
{
    ForwardIterator mid;
    while (first != last) {
	mid = advance (first, size_t(distance (first,last)) / 2);
	if (comp (value, *mid))
	    last = mid;
	else
	    first = mid + 1;
    }
    return last;
}

/// Returns pair<lower_bound,upper_bound>
/// \ingroup SearchingAlgorithms
/// \ingroup PredicateAlgorithms
///
template <typename ForwardIterator, typename T, typename StrictWeakOrdering>
inline pair<ForwardIterator,ForwardIterator> equal_range (ForwardIterator first, ForwardIterator last, const T& value, StrictWeakOrdering comp)
{
    pair<ForwardIterator,ForwardIterator> rv;
    rv.second = rv.first = lower_bound (first, last, value, comp);
    while (rv.second != last && !comp(value, *(rv.second)))
	++ rv.second;
    return rv;
}

/// \brief Puts \p nth element into its sorted position.
/// In this implementation, the entire array is sorted. The performance difference is
/// so small and the function use is so rare, there is no need to have code for it.
/// \ingroup SortingAlgorithms
/// \ingroup SearchingAlgorithms
/// \ingroup PredicateAlgorithms
///
template <typename RandomAccessIterator, typename Compare>
inline void nth_element (RandomAccessIterator first, RandomAccessIterator, RandomAccessIterator last, Compare comp)
{
    sort (first, last, comp);
}

/// \brief Searches for the first subsequence [first2,last2) in [first1,last1)
/// \ingroup SearchingAlgorithms
/// \ingroup PredicateAlgorithms
template <typename ForwardIterator1, typename ForwardIterator2, typename BinaryPredicate>
ForwardIterator1 search (ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2, BinaryPredicate comp)
{
    const ForwardIterator1 slast = last1 - distance(first2, last2) + 1;
    for (; first1 < slast; ++first1) {
	ForwardIterator2 i = first2;
	ForwardIterator1 j = first1;
	for (; i != last2 && comp(*j, *i); ++i, ++j) ;
	if (i == last2)
	    return first1;
    }
    return last1;
}

/// \brief Searches for the last subsequence [first2,last2) in [first1,last1)
/// \ingroup SearchingAlgorithms
/// \ingroup PredicateAlgorithms
template <typename ForwardIterator1, typename ForwardIterator2, typename BinaryPredicate>
ForwardIterator1 find_end (ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2, BinaryPredicate comp)
{
    ForwardIterator1 s = last1 - distance(first2, last2);
    for (; first1 < s; --s) {
	ForwardIterator2 i = first2, j = s;
	for (; i != last2 && comp(*j, *i); ++i, ++j) ;
	if (i == last2)
	    return s;
    }
    return last1;
}

/// \brief Searches for the first occurence of \p count \p values in [first, last)
/// \ingroup SearchingAlgorithms
/// \ingroup PredicateAlgorithms
template <typename Iterator, typename T, typename BinaryPredicate>
Iterator search_n (Iterator first, Iterator last, size_t count, const T& value, BinaryPredicate comp)
{
    size_t n = 0;
    for (; first != last; ++first) {
	if (!comp (*first, value))
	    n = 0;
	else if (++n == count)
	    return first - --n;
    }
    return last;
}

/// \brief Searches [first1,last1) for the first occurrence of an element from [first2,last2)
/// \ingroup SearchingAlgorithms
/// \ingroup PredicateAlgorithms
template <typename InputIterator, typename ForwardIterator, typename BinaryPredicate>
InputIterator find_first_of (InputIterator first1, InputIterator last1, ForwardIterator first2, ForwardIterator last2, BinaryPredicate comp)
{
    for (; first1 != last1; ++first1)
	for (ForwardIterator i = first2; i != last2; ++i)
	    if (comp (*first1, *i))
		return first1;
    return first1;
}

/// \brief Returns true if [first2,last2) is a subset of [first1,last1)
/// \ingroup ConditionAlgorithms
/// \ingroup SetAlgorithms
/// \ingroup PredicateAlgorithms
template <typename InputIterator1, typename InputIterator2, typename StrictWeakOrdering>
bool includes (InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, StrictWeakOrdering comp)
{
    for (; (first1 != last1) & (first2 != last2); ++first1) {
	if (comp (*first2, *first1))
	    return false;
	first2 += !comp (*first1, *first2);
    }
    return first2 == last2;
}

/// \brief Merges [first1,last1) with [first2,last2)
///
/// Result will contain every element that is in either set. If duplicate
/// elements are present, max(n,m) is placed in the result.
///
/// \ingroup SetAlgorithms
/// \ingroup PredicateAlgorithms
template <typename InputIterator1, typename InputIterator2, typename OutputIterator, typename StrictWeakOrdering>
OutputIterator set_union (InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result, StrictWeakOrdering comp)
{
    for (; (first1 != last1) & (first2 != last2); ++result) {
	if (comp (*first2, *first1))
	    *result = *first2++;
	else {
	    first2 += !comp (*first1, *first2);
	    *result = *first1++;
	}
    }
    return copy (first2, last2, copy (first1, last1, result));
}

/// \brief Creates a set containing elements shared by the given ranges.
/// \ingroup SetAlgorithms
/// \ingroup PredicateAlgorithms
template <typename InputIterator1, typename InputIterator2, typename OutputIterator, typename StrictWeakOrdering>
OutputIterator set_intersection (InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result, StrictWeakOrdering comp)
{
    while ((first1 != last1) & (first2 != last2)) {
	bool b1ge2 = !comp (*first1, *first2), b2ge1 = !comp (*first2, *first1);
	if (b1ge2 & b2ge1)
	    *result++ = *first1;
	first1 += b2ge1;
	first2 += b1ge2;
    }
    return result;
}

/// \brief Removes from [first1,last1) elements present in [first2,last2)
/// \ingroup SetAlgorithms
/// \ingroup PredicateAlgorithms
template <typename InputIterator1, typename InputIterator2, typename OutputIterator, typename StrictWeakOrdering>
OutputIterator set_difference (InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result, StrictWeakOrdering comp)
{
    while ((first1 != last1) & (first2 != last2)) {
	bool b1ge2 = !comp (*first1, *first2), b2ge1 = !comp (*first2, *first1);
	if (!b1ge2)
	    *result++ = *first1;
	first1 += b2ge1;
	first2 += b1ge2;
    }
    return copy (first1, last1, result);
}

/// \brief Performs union of sets A-B and B-A.
/// \ingroup SetAlgorithms
/// \ingroup PredicateAlgorithms
template <typename InputIterator1, typename InputIterator2, typename OutputIterator, typename StrictWeakOrdering>
OutputIterator set_symmetric_difference (InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result, StrictWeakOrdering comp)
{
    while ((first1 != last1) & (first2 != last2)) {
	bool b1l2 = comp (*first1, *first2), b2l1 = comp (*first2, *first1);
	if (b1l2)
	    *result++ = *first1;
	else if (b2l1)
	    *result++ = *first2;
	first1 += !b2l1;
	first2 += !b1l2;
    }
    return copy (first2, last2, copy (first1, last1, result));
}

/// \brief Returns true if the given range is sorted.
/// \ingroup ConditionAlgorithms
/// \ingroup PredicateAlgorithms
template <typename ForwardIterator, typename StrictWeakOrdering>
bool is_sorted (ForwardIterator first, ForwardIterator last, StrictWeakOrdering comp)
{
    for (ForwardIterator i = first; ++i < last; ++first)
	if (comp (*i, *first))
	    return false;
    return true;
}

/// \brief Compares two given containers like strcmp compares strings.
/// \ingroup ConditionAlgorithms
/// \ingroup PredicateAlgorithms
template <typename InputIterator1, typename InputIterator2, typename BinaryPredicate>
bool lexicographical_compare (InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, BinaryPredicate comp)
{
    for (; (first1 != last1) & (first2 != last2); ++first1, ++first2) {
	if (comp (*first1, *first2))
	    return true;
	if (comp (*first2, *first1))
	    return false;
    }
    return (first1 == last1) & (first2 != last2);
}

/// \brief Creates the next lexicographical permutation of [first,last).
/// Returns false if no further permutations can be created.
/// \ingroup GeneratorAlgorithms
/// \ingroup PredicateAlgorithms
template <typename BidirectionalIterator, typename StrictWeakOrdering>
bool next_permutation (BidirectionalIterator first, BidirectionalIterator last, StrictWeakOrdering comp)
{
    if (distance (first, last) < 2)
	return false;
    BidirectionalIterator i = last;
    for (--i; i != first; ) {
	--i;
	if (comp (i[0], i[1])) {
	    BidirectionalIterator j = last;
	    while (!comp (*i, *--j)) ;
	    iter_swap (i, j);
	    reverse (i + 1, last);
	    return true;
	}
    }
    reverse (first, last);
    return false;
}

/// \brief Creates the previous lexicographical permutation of [first,last).
/// Returns false if no further permutations can be created.
/// \ingroup GeneratorAlgorithms
/// \ingroup PredicateAlgorithms
template <typename BidirectionalIterator, typename StrictWeakOrdering>
bool prev_permutation (BidirectionalIterator first, BidirectionalIterator last, StrictWeakOrdering comp)
{
    if (distance (first, last) < 2)
	return false;
    BidirectionalIterator i = last;
    for (--i; i != first; ) {
	--i;
	if (comp(i[1], i[0])) {
	    BidirectionalIterator j = last;
	    while (!comp (*--j, *i)) ;
	    iter_swap (i, j);
	    reverse (i + 1, last);
	    return true;
	}
    }
    reverse (first, last);
    return false;
}

/// \brief Returns iterator to the max element in [first,last)
/// \ingroup SearchingAlgorithms
/// \ingroup PredicateAlgorithms
template <typename ForwardIterator, typename BinaryPredicate>
inline ForwardIterator max_element (ForwardIterator first, ForwardIterator last, BinaryPredicate comp)
{
    ForwardIterator result = first;
    for (; first != last; ++first)
	if (comp (*result, *first))
	    result = first;
    return result;
}

/// \brief Returns iterator to the min element in [first,last)
/// \ingroup SearchingAlgorithms
/// \ingroup PredicateAlgorithms
template <typename ForwardIterator, typename BinaryPredicate>
inline ForwardIterator min_element (ForwardIterator first, ForwardIterator last, BinaryPredicate comp)
{
    ForwardIterator result = first;
    for (; first != last; ++first)
	if (comp (*first, *result))
	    result = first;
    return result;
}

/// \brief Makes [first,middle) a part of the sorted array.
/// Contents of [middle,last) is undefined. This implementation just calls stable_sort.
/// \ingroup SortingAlgorithms
/// \ingroup PredicateAlgorithms
template <typename RandomAccessIterator, typename StrictWeakOrdering>
inline void partial_sort (RandomAccessIterator first, RandomAccessIterator, RandomAccessIterator last, StrictWeakOrdering comp)
{
    stable_sort (first, last, comp);
}

/// \brief Like partial_sort, but outputs to [result_first,result_last)
/// \ingroup SortingAlgorithms
/// \ingroup PredicateAlgorithms
template <typename InputIterator, typename RandomAccessIterator, typename StrictWeakOrdering>
RandomAccessIterator partial_sort_copy (InputIterator first, InputIterator last, RandomAccessIterator result_first, RandomAccessIterator result_last, StrictWeakOrdering comp)
{
    RandomAccessIterator rend = result_first;
    for (; first != last; ++first) {
	RandomAccessIterator i = result_first;
	for (; i != rend && comp (*i, *first); ++i) ;
	if (i == result_last)
	    continue;
	rend += (rend < result_last);
	copy_backward (i, rend - 1, rend);
	*i = *first;
    }
    return rend;
}

/// \brief Like partition, but preserves equal element order.
/// \ingroup SortingAlgorithms
/// \ingroup PredicateAlgorithms
template <typename ForwardIterator, typename Predicate>
ForwardIterator stable_partition (ForwardIterator first, ForwardIterator last, Predicate pred)
{
    if (first == last)
	return first;
    ForwardIterator l, r, m = advance (first, distance (first, last) / 2);
    if (first == m)
	return pred(*first) ? last : first;
    l = stable_partition (first, m, pred);
    r = stable_partition (m, last, pred);
    rotate (l, m, r);
    return advance (l, distance (m, r));
}

/// \brief Splits [first,last) in two by \p pred.
///
/// Creates two ranges [first,middle) and [middle,last), where every element
/// in the former is less than every element in the latter.
/// The return value is middle.
///
/// \ingroup SortingAlgorithms
/// \ingroup PredicateAlgorithms
template <typename ForwardIterator, typename Predicate>
inline ForwardIterator partition (ForwardIterator first, ForwardIterator last, Predicate pred)
{
    return stable_partition (first, last, pred);
}

} // namespace ustl

namespace ustl {

/// \class vector uvector.h ustl.h
/// \ingroup Sequences
///
/// \brief STL vector equivalent.
///
/// Provides a typed array-like interface to a managed memory block, including
/// element access, iteration, modification, resizing, and serialization. In
/// this design elements frequently undergo bitwise move, so don't put it in
/// here if it doesn't support it. This mostly means having no self-pointers.
///
template <typename T>
class vector {
public:
    typedef T				value_type;
    typedef value_type*			pointer;
    typedef const value_type*		const_pointer;
    typedef value_type&			reference;
    typedef const value_type&		const_reference;
    typedef pointer			iterator;
    typedef const_pointer		const_iterator;
    typedef memblock::size_type		size_type;
    typedef memblock::written_size_type	written_size_type;
    typedef memblock::difference_type	difference_type;
    typedef ::ustl::reverse_iterator<iterator>	reverse_iterator;
    typedef ::ustl::reverse_iterator<const_iterator>	const_reverse_iterator;
public:
    inline			vector (void);
    inline explicit		vector (size_type n);
				vector (size_type n, const T& v);
				vector (const vector& v);
				vector (const_iterator i1, const_iterator i2);
    inline			~vector (void) noexcept;
    inline const vector&	operator= (const vector& v);
    inline bool			operator== (const vector& v) const	{ return _data == v._data; }
    inline			operator cmemlink (void) const	{ return cmemlink (_data); }
    inline			operator cmemlink (void)	{ return cmemlink (_data); }
    inline			operator memlink (void)		{ return memlink (_data); }
    inline void			reserve (size_type n, bool bExact = false);
    inline void			resize (size_type n, bool bExact = true);
    inline size_type		capacity (void) const		{ return _data.capacity() / sizeof(T);	}
    inline size_type		size (void) const		{ return _data.size() / sizeof(T);		}
    inline size_type		max_size (void) const		{ return _data.max_size() / sizeof(T);	}
    inline bool			empty (void) const		{ return _data.empty();			}
    inline iterator		begin (void)			{ return iterator (_data.begin());		}
    inline const_iterator	begin (void) const		{ return const_iterator (_data.begin());	}
    inline iterator		end (void)			{ return iterator (_data.end());		}
    inline const_iterator	end (void) const		{ return const_iterator (_data.end());	}
    inline const_iterator	cbegin (void) const		{ return begin(); }
    inline const_iterator	cend (void) const		{ return end(); }
    inline reverse_iterator	rbegin (void)			{ return reverse_iterator (end());		}
  inline const_reverse_iterator	rbegin (void) const		{ return const_reverse_iterator (end());	}
    inline reverse_iterator	rend (void)			{ return reverse_iterator (begin());		}
  inline const_reverse_iterator	rend (void) const		{ return const_reverse_iterator (begin());	}
  inline const_reverse_iterator	crbegin (void) const		{ return rbegin(); }
  inline const_reverse_iterator	crend (void) const		{ return rend(); }
    inline iterator		data (void)			{ return _data.data(); }
    inline const_iterator	data (void) const		{ return _data.data(); }
    inline const_iterator	cdata (void) const		{ return _data.cdata(); }
    inline iterator		iat (size_type i)		{ assert (i <= size()); return begin() + i; }
    inline const_iterator	iat (size_type i) const		{ assert (i <= size()); return begin() + i; }
    inline reference		at (size_type i)		{ assert (i < size()); return begin()[i]; }
    inline const_reference	at (size_type i) const		{ assert (i < size()); return begin()[i]; }
    inline reference		operator[] (size_type i)	{ return at (i); }
    inline const_reference	operator[] (size_type i) const	{ return at (i); }
    inline reference		front (void)			{ return at(0); }
    inline const_reference	front (void) const		{ return at(0); }
    inline reference		back (void)			{ assert (!empty()); return end()[-1]; }
    inline const_reference	back (void) const		{ assert (!empty()); return end()[-1]; }
    inline void			push_back (const T& v = T());
    inline void			pop_back (void)			{ destroy (end()-1); _data.memlink::resize (_data.size() - sizeof(T)); }
    inline void			clear (void)			{ destroy (begin(), end()); _data.clear(); }
    inline void			shrink_to_fit (void)		{ _data.shrink_to_fit(); }
    inline void			deallocate (void) noexcept;
    inline void			assign (const_iterator i1, const_iterator i2);
    inline void			assign (size_type n, const T& v);
    inline void			swap (vector& v)		{ _data.swap (v._data); }
    inline iterator		insert (const_iterator ip, const T& v);
    inline iterator		insert (const_iterator ip, size_type n, const T& v);
    inline iterator		insert (const_iterator ip, const_iterator i1, const_iterator i2);
    inline iterator		erase (const_iterator ep, size_type n = 1);
    inline iterator		erase (const_iterator ep1, const_iterator ep2);
    inline void			manage (pointer p, size_type n)		{ _data.manage (p, n * sizeof(T)); }
    inline bool			is_linked (void) const			{ return _data.is_linked(); }
    inline void			unlink (void)				{ _data.unlink(); }
    inline void			copy_link (void)			{ _data.copy_link(); }
    inline void			link (const_pointer p, size_type n)	{ _data.link (p, n * sizeof(T)); }
    inline void			link (pointer p, size_type n)		{ _data.link (p, n * sizeof(T)); }
    inline void			link (const vector& v)		{ _data.link (v); }
    inline void			link (vector& v)			{ _data.link (v); }
    inline void			link (const_pointer first, const_pointer last)	{ _data.link (first, last); }
    inline void			link (pointer first, pointer last)		{ _data.link (first, last); }
    inline void			read (istream& is)			{ container_read (is, *this); }
    inline void			write (ostream& os) const		{ container_write (os, *this); }
    inline void			text_write (ostringstream& os) const	{ container_text_write (os, *this); }
    inline size_t		stream_size (void) const		{ return container_stream_size (*this); }
#if HAVE_CPP11
    inline			vector (vector&& v)			: _data(move(v._data)) {}
    inline			vector (std::initializer_list<T> v)	: _data() { uninitialized_copy_n (v.begin(), v.size(), append_hole(v.size())); }
    inline vector&		operator= (vector&& v)			{ swap (v); return *this; }
    template <typename... Args>
    inline iterator		emplace (const_iterator ip, Args&&... args);
    template <typename... Args>
    inline void			emplace_back (Args&&... args);
    inline void			push_back (T&& v)			{ emplace_back (move(v)); }
    inline iterator		insert (const_iterator ip, T&& v)	{ return emplace (ip, move(v)); }
    inline iterator		insert (const_iterator ip, std::initializer_list<T> v)	{ return insert (ip, v.begin(), v.end()); }
#endif
protected:
    inline iterator		insert_space (const_iterator ip, size_type n);
private:
    inline iterator		insert_hole (const_iterator ip, size_type n);
    inline iterator		append_hole (size_type n);
private:
    memblock			_data;	///< Raw element data, consecutively stored.
};

/// Allocates space for at least \p n elements.
template <typename T>
inline void vector<T>::reserve (size_type n, bool bExact)
{
    _data.reserve (n * sizeof(T), bExact);
}

template <typename T>
inline typename vector<T>::iterator vector<T>::append_hole (size_type n)
{
    _data.reserve (_data.size() + n*sizeof(T));
    _data.memlink::resize (_data.size()+n*sizeof(T));
    return end()-n;
}

/// Resizes the vector to contain \p n elements.
template <typename T>
inline void vector<T>::resize (size_type n, bool bExact)
{
    destroy (begin()+n, end());
    const size_type nb = n * sizeof(T);
    if (_data.capacity() < nb)
	reserve (n, bExact);
    construct (end(), end() + (nb - _data.size())/sizeof(T));
    _data.memlink::resize (nb);
}

/// Calls element destructors and frees storage.
template <typename T>
inline void vector<T>::deallocate (void) noexcept
{
    destroy (begin(), end());
    _data.deallocate();
}

/// Initializes empty vector.
template <typename T>
inline vector<T>::vector (void)
:_data()
{
}

/// Initializes a vector of size \p n.
template <typename T>
inline vector<T>::vector (size_type n)
:_data()
{
    resize (n);
}

/// Copies \p n elements from \p v.
template <typename T>
vector<T>::vector (size_type n, const T& v)
:_data()
{
    uninitialized_fill_n (append_hole (n), n, v);
}

/// Copies \p v.
template <typename T>
vector<T>::vector (const vector<T>& v)
:_data()
{
    uninitialized_copy_n (v.begin(), v.size(), append_hole(v.size()));
}

/// Copies range [\p i1, \p i2]
template <typename T>
vector<T>::vector (const_iterator i1, const_iterator i2)
:_data()
{
    uninitialized_copy (i1, i2, append_hole(distance(i1,i2)));
}

/// Destructor
template <typename T>
inline vector<T>::~vector (void) noexcept
{
    destroy (begin(), end());
}

/// Copies the range [\p i1, \p i2]
template <typename T>
inline void vector<T>::assign (const_iterator i1, const_iterator i2)
{
    assert (i1 <= i2);
    resize (distance (i1, i2));
    ::ustl::copy (i1, i2, begin());
}

/// Copies \p n elements with value \p v.
template <typename T>
inline void vector<T>::assign (size_type n, const T& v)
{
    resize (n);
    ::ustl::fill (begin(), end(), v);
}

/// Copies contents of \p v.
template <typename T>
inline const vector<T>& vector<T>::operator= (const vector<T>& v)
{
    assign (v.begin(), v.end());
    return *this;
}

/// Inserts \p n uninitialized elements at \p ip.
template <typename T>
inline typename vector<T>::iterator vector<T>::insert_hole (const_iterator ip, size_type n)
{
    const uoff_t ipmi = distance (_data.begin(), memblock::const_iterator(ip));
    reserve (size() + n);
    return iterator (_data.insert (_data.iat(ipmi), n * sizeof(T)));
}

/// Inserts \p n uninitialized elements at \p ip.
template <typename T>
inline typename vector<T>::iterator vector<T>::insert_space (const_iterator ip, size_type n)
{
    iterator ih = insert_hole (ip, n);
    construct (ih, ih+n);
    return ih;
}

/// Inserts \p n elements with value \p v at offsets \p ip.
template <typename T>
inline typename vector<T>::iterator vector<T>::insert (const_iterator ip, size_type n, const T& v)
{
    iterator d = insert_hole (ip, n);
    uninitialized_fill_n (d, n, v);
    return d;
}

/// Inserts value \p v at offset \p ip.
template <typename T>
inline typename vector<T>::iterator vector<T>::insert (const_iterator ip, const T& v)
{
    iterator d = insert_hole (ip, 1);
    construct (d, v);
    return d;
}

/// Inserts range [\p i1, \p i2] at offset \p ip.
template <typename T>
inline typename vector<T>::iterator vector<T>::insert (const_iterator ip, const_iterator i1, const_iterator i2)
{
    assert (i1 <= i2);
    iterator d = insert_hole (ip, distance (i1, i2));
    uninitialized_copy (i1, i2, d);
    return d;
}

/// Removes \p count elements at offset \p ep.
template <typename T>
inline typename vector<T>::iterator vector<T>::erase (const_iterator ep, size_type n)
{
    iterator d = const_cast<iterator>(ep);
    destroy (d, d+n);
    return iterator (_data.erase (memblock::iterator(d), n * sizeof(T)));
}

/// Removes elements from \p ep1 to \p ep2.
template <typename T>
inline typename vector<T>::iterator vector<T>::erase (const_iterator ep1, const_iterator ep2)
{
    assert (ep1 <= ep2);
    return erase (ep1, distance(ep1, ep2));
}

/// Inserts value \p v at the end of the vector.
template <typename T>
inline void vector<T>::push_back (const T& v)
{
    construct (append_hole(1), v);
}

#if HAVE_CPP11

/// Constructs value at \p ip
template <typename T>
template <typename... Args>
inline typename vector<T>::iterator vector<T>::emplace (const_iterator ip, Args&&... args)
{
    return new (insert_hole(ip,1)) T (forward<Args>(args)...);
}

/// Constructs value at the end of the vector.
template <typename T>
template <typename... Args>
inline void vector<T>::emplace_back (Args&&... args)
{
    new (append_hole(1)) T (forward<Args>(args)...);
}

#endif

/// Use with vector classes to allocate and link to stack space. \p n is in elements.
#define typed_alloca_link(m,T,n)	(m).link ((T*) alloca ((n) * sizeof(T)), (n))

} // namespace ustl

namespace ustl {

//----------------------------------------------------------------------

typedef uint8_t utf8subchar_t;	///< Type for the encoding subcharacters.

//----------------------------------------------------------------------

inline size_t Utf8Bytes (wchar_t v) __attribute__((const));
inline size_t Utf8Bytes (const wchar_t* first, const wchar_t* last) __attribute__((pure));
inline size_t Utf8SequenceBytes (wchar_t c) __attribute__((const));

//----------------------------------------------------------------------

/// Returns the number of bytes required to UTF-8 encode \p v.
inline size_t Utf8Bytes (wchar_t v)
{
    if ((uint32_t) v < 128)
	return 1;
    size_t n;
    #if __i386__ || __x86_64__
        uint32_t r = 0;
	asm ("bsr\t%2, %%eax\n\t"
	    "add\t$4, %0\n\t"
	    "div\t%3":"=a"(n),"+d"(r):"r"(v),"c"(5));
    #else
	static const uint32_t c_Bounds[7] = { 0x0000007F, 0x000007FF, 0x0000FFFF, 0x001FFFFF, 0x03FFFFFF, 0x7FFFFFFF, 0xFFFFFFFF };
	for (n = 0; c_Bounds[n++] < uint32_t(v););
    #endif
    return n;
}

/// Measures the size of a wchar_t array in UTF-8 encoding.
inline size_t Utf8Bytes (const wchar_t* first, const wchar_t* last)
{
    size_t bc = 0;
    for (; first < last; ++first)
	bc += Utf8Bytes(*first);
    return bc;
}

/// Returns the number of bytes in a UTF-8 sequence that starts with \p c.
inline size_t Utf8SequenceBytes (wchar_t c)	// a wchar_t to keep c in a full register
{
    // Count the leading bits. Header bits are 1 * nBytes followed by a 0.
    //	0 - single byte character. Take 7 bits (0xFF >> 1)
    //	1 - error, in the middle of the character. Take 6 bits (0xFF >> 2)
    //	    so you will keep reading invalid entries until you hit the next character.
    //	>2 - multibyte character. Take remaining bits, and get the next bytes.
    // All errors are ignored, since the user can not correct them.
    //
    wchar_t mask = 0x80;
    size_t nBytes = 0;
    for (; c & mask; ++nBytes)
	mask >>= 1;
    return nBytes ? nBytes : 1; // A sequence is always at least 1 byte.
}

//----------------------------------------------------------------------

/// \class utf8in_iterator utf8.h ustl.h
/// \ingroup IteratorAdaptors
///
/// \brief An iterator adaptor to character containers for reading UTF-8 encoded text.
///
/// For example, you can copy from ustl::string to ustl::vector<wchar_t> with
/// copy (utf8in (str.begin()), utf8in (str.end()), back_inserter(wvect));
/// There is no error handling; if the reading frame slips you'll get extra
/// characters, one for every misaligned byte. Although it is possible to skip
/// to the start of the next character, that would result in omitting the
/// misformatted character and the one after it, making it very difficult to
/// detect by the user. It is better to write some strange characters and let
/// the user know his file is corrupted. Another problem is overflow on bad
/// encodings (like a 0xFF on the end of a string). This is checked through
/// the end-of-string nul character, which will always be there as long as
/// you are using the string class.
///
template <typename Iterator, typename WChar = wchar_t>
class utf8in_iterator {
public:
    typedef typename iterator_traits<Iterator>::value_type	value_type;
    typedef typename iterator_traits<Iterator>::difference_type	difference_type;
    typedef typename iterator_traits<Iterator>::pointer		pointer;
    typedef typename iterator_traits<Iterator>::reference	reference;
    typedef input_iterator_tag					iterator_category;
public:
    explicit			utf8in_iterator (const Iterator& is)		: _i (is), _v (0) { Read(); }
				utf8in_iterator (const utf8in_iterator& i)	: _i (i._i), _v (i._v) {}
    inline const utf8in_iterator& operator= (const utf8in_iterator& i)		{ _i = i._i; _v = i._v; return *this; }
    inline Iterator		base (void) const	{ return _i - (Utf8Bytes(_v) - 1); }
    /// Reads and returns the next value.
    inline WChar		operator* (void) const	{ return _v; }
    inline utf8in_iterator&	operator++ (void)	{ ++_i; Read(); return *this; }
    inline utf8in_iterator	operator++ (int)	{ utf8in_iterator old (*this); operator++(); return old; }
    inline utf8in_iterator&	operator+= (uoff_t n)	{ while (n--) operator++(); return *this; }
    inline utf8in_iterator	operator+ (uoff_t n)	{ utf8in_iterator v (*this); return v += n; }
    inline bool			operator== (const utf8in_iterator& i) const	{ return _i == i._i; }
    inline bool			operator< (const utf8in_iterator& i) const	{ return _i < i._i; }
    difference_type		operator- (const utf8in_iterator& i) const;
private:
    void			Read (void);
private:
    Iterator			_i;
    WChar			_v;
};

/// Steps to the next character and updates current returnable value.
template <typename Iterator, typename WChar>
void utf8in_iterator<Iterator,WChar>::Read (void)
{
    const utf8subchar_t c = *_i;
    size_t nBytes = Utf8SequenceBytes (c);
    _v = c & (0xFF >> nBytes);	// First byte contains bits after the header.
    while (--nBytes && *++_i)	// Each subsequent byte has 6 bits.
	_v = (_v << 6) | (*_i & 0x3F);
}

/// Returns the distance in characters (as opposed to the distance in bytes).
template <typename Iterator, typename WChar>
typename utf8in_iterator<Iterator,WChar>::difference_type
utf8in_iterator<Iterator,WChar>::operator- (const utf8in_iterator<Iterator,WChar>& last) const
{
    difference_type dist = 0;
    for (Iterator first (last._i); first < _i; ++dist)
	first = advance (first, Utf8SequenceBytes (*first));
    return dist;
}

//----------------------------------------------------------------------

/// \class utf8out_iterator utf8.h ustl.h
/// \ingroup IteratorAdaptors
///
/// \brief An iterator adaptor to character containers for writing UTF-8 encoded text.
///
template <typename Iterator, typename WChar = wchar_t>
class utf8out_iterator {
public:
    typedef typename iterator_traits<Iterator>::value_type	value_type;
    typedef typename iterator_traits<Iterator>::difference_type	difference_type;
    typedef typename iterator_traits<Iterator>::pointer		pointer;
    typedef typename iterator_traits<Iterator>::reference	reference;
    typedef output_iterator_tag					iterator_category;
public:
    explicit			utf8out_iterator (const Iterator& os) : _i (os) {}
				utf8out_iterator (const utf8out_iterator& i) : _i (i._i) {} 
    inline const Iterator&	base (void) const { return _i; }
    /// Writes \p v into the stream.
    utf8out_iterator&		operator= (WChar v);
    inline utf8out_iterator&	operator* (void) { return *this; }
    inline utf8out_iterator&	operator++ (void) { return *this; }
    inline utf8out_iterator&	operator++ (int) { return *this; }
    inline bool			operator== (const utf8out_iterator& i) const { return _i == i._i; }
    inline bool			operator< (const utf8out_iterator& i) const { return _i < i._i; }
private:
    Iterator			_i;
};

/// Writes \p v into the stream.
template <typename Iterator, typename WChar>
utf8out_iterator<Iterator,WChar>& utf8out_iterator<Iterator,WChar>::operator= (WChar v)
{
    const size_t nBytes = Utf8Bytes (v);
    if (nBytes > 1) {
	// Write the bits 6 bits at a time, except for the first one,
	// which may be less than 6 bits.
	wchar_t shift = nBytes * 6;
	*_i++ = ((v >> (shift -= 6)) & 0x3F) | (0xFF << (8 - nBytes));
	while (shift)
	    *_i++ = ((v >> (shift -= 6)) & 0x3F) | 0x80;
    } else	// If only one byte, there is no header.
	*_i++ = v;
    return *this;
}

//----------------------------------------------------------------------

/// Returns a UTF-8 adaptor writing to \p i. Useful in conjuction with back_insert_iterator.
template <typename Iterator>
inline utf8out_iterator<Iterator> utf8out (Iterator i)
{
    return utf8out_iterator<Iterator> (i);
}

/// Returns a UTF-8 adaptor reading from \p i.
template <typename Iterator>
inline utf8in_iterator<Iterator> utf8in (Iterator i)
{
    return utf8in_iterator<Iterator> (i);
}

//----------------------------------------------------------------------

} // namespace ustl

namespace ustl {

/// \class string ustring.h ustl.h
/// \ingroup Sequences
///
/// \brief STL basic_string&lt;char&gt; equivalent.
///
/// An STL container for text string manipulation.
/// Differences from C++ standard:
///	- string is a class, not a template. Wide characters are assumed to be
///		encoded with utf8 at all times except when rendering or editing,
///		where you would use a utf8 iterator.
/// 	- format member function - you can, of course use an \ref ostringstream,
///		which also have format functions, but most of the time this way
///		is more convenient. Because uSTL does not implement locales,
///		format is the only way to create localized strings.
/// 	- const char* cast operator. It is much clearer to use this than having
/// 		to type .c_str() every time.
/// 	- length returns the number of _characters_, not bytes.
///		This function is O(N), so use wisely.
///
/// An additional note is in order regarding the use of indexes. All indexes
/// passed in as arguments or returned by find are byte offsets, not character
/// offsets. Likewise, sizes are specified in bytes, not characters. The
/// rationale is that there is no way for you to know what is in the string.
/// There is no way for you to know how many characters are needed to express
/// one thing or another. The only thing you can do to a localized string is
/// search for delimiters and modify text between them as opaque blocks. If you
/// do anything else, you are hardcoding yourself into a locale! So stop it!
///
class string : public memblock {
public:
    typedef char		value_type;
    typedef unsigned char	uvalue_type;
    typedef value_type*		pointer;
    typedef const value_type*	const_pointer;
    typedef wchar_t		wvalue_type;
    typedef wvalue_type*	wpointer;
    typedef const wvalue_type*	const_wpointer;
    typedef pointer		iterator;
    typedef const_pointer	const_iterator;
    typedef value_type&		reference;
    typedef value_type		const_reference;
    typedef ::ustl::reverse_iterator<iterator>		reverse_iterator;
    typedef ::ustl::reverse_iterator<const_iterator>	const_reverse_iterator;
    typedef utf8in_iterator<const_iterator>		utf8_iterator;
    typedef size_type		pos_type;
    static constexpr const pos_type npos = INT_MAX;	///< Value that means the end of string.
public:
    inline			string (void) noexcept		: memblock () { relink ("",0); }
				string (const string& s);
    inline			string (const string& s, pos_type o, size_type n = npos);
    inline explicit		string (const cmemlink& l);
				string (const_pointer s) noexcept;
    inline			string (const_pointer s, size_type len);
    inline			string (const_pointer s1, const_pointer s2);
				string (size_type n, value_type c);
    inline			~string (void) noexcept		{ }
    inline pointer		data (void)			{ return string::pointer (memblock::data()); }
    inline const_pointer	data (void) const		{ return string::const_pointer (memblock::data()); }
    inline const_pointer	c_str (void) const		{ return string::const_pointer (memblock::cdata()); }
    inline size_type		max_size (void) const		{ size_type s (memblock::max_size()); return s - !!s; }
    inline size_type		capacity (void) const		{ size_type c (memblock::capacity()); return c - !!c; }
    void			resize (size_type n);
    inline void			resize (size_type n, value_type c);
    inline void			clear (void)			{ resize (0); }
    inline iterator		begin (void)			{ return iterator (memblock::begin()); }
    inline const_iterator	begin (void) const		{ return const_iterator (memblock::begin()); }
    inline const_iterator	cbegin (void) const		{ return begin(); }
    inline iterator		end (void)			{ return iterator (memblock::end()); }
    inline const_iterator	end (void) const		{ return const_iterator (memblock::end()); }
    inline const_iterator	cend (void) const		{ return end(); }
    inline reverse_iterator	rbegin (void)			{ return reverse_iterator (end()); }
  inline const_reverse_iterator	rbegin (void) const		{ return const_reverse_iterator (end()); }
  inline const_reverse_iterator	crbegin (void) const		{ return rbegin(); }
    inline reverse_iterator	rend (void)			{ return reverse_iterator (begin()); }
  inline const_reverse_iterator	rend (void) const		{ return const_reverse_iterator (begin()); }
  inline const_reverse_iterator	crend (void) const		{ return rend(); }
    inline utf8_iterator	utf8_begin (void) const		{ return utf8_iterator (begin()); }
    inline utf8_iterator	utf8_end (void) const		{ return utf8_iterator (end()); }
    inline const_reference	at (pos_type pos) const		{ assert (pos <= size() && begin()); return begin()[pos]; }
    inline reference		at (pos_type pos)		{ assert (pos <= size() && begin()); return begin()[pos]; }
    inline const_iterator	iat (pos_type pos) const	{ return begin() + (__builtin_constant_p(pos) && pos >= npos ? size() : min(size_type(pos),size())); }
    inline iterator		iat (pos_type pos)		{ return const_cast<iterator>(const_cast<const string*>(this)->iat(pos)); }
    const_iterator		wiat (pos_type i) const noexcept;
    inline iterator		wiat (pos_type i)		{ return const_cast<iterator>(const_cast<const string*>(this)->wiat(i)); }
    inline const_reference	front (void) const		{ return at(0); }
    inline reference		front (void)			{ return at(0); }
    inline const_reference	back (void) const		{ return at(size()-1); }
    inline reference		back (void)			{ return at(size()-1); }
    inline size_type		length (void) const		{ return distance (utf8_begin(), utf8_end()); }
    inline string&		append (const_iterator i1, const_iterator i2)	{ return append (i1, distance (i1, i2)); }
    string&	   		append (const_pointer s, size_type len);
    string&	   		append (const_pointer s);
    string&			append (size_type n, value_type c);
    inline string&		append (size_type n, wvalue_type c)		{ insert (size(), n, c); return *this; }
    inline string&		append (const_wpointer s1, const_wpointer s2)	{ insert (size(), s1, s2); return *this; }
    inline string&		append (const_wpointer s)			{ const_wpointer se (s); for (;se&&*se;++se) {} return append (s, se); }
    inline string&		append (const string& s)			{ return append (s.begin(), s.end()); }
    inline string&		append (const string& s,pos_type o,size_type n)	{ return append (s.iat(o), s.iat(o+n)); }
    inline void			push_back (value_type c)			{ resize(size()+1); end()[-1] = c; }
    inline void			push_back (wvalue_type c)			{ append (1, c); }
    inline void			pop_back (void)					{ resize (size()-1); }
    inline string&		assign (const_iterator i1, const_iterator i2)	{ return assign (i1, distance (i1, i2)); }
    string&	    		assign (const_pointer s, size_type len);
    string&	    		assign (const_pointer s);
    inline string&		assign (const_wpointer s1, const_wpointer s2)	{ clear(); return append (s1, s2); }
    inline string&		assign (const_wpointer s1)			{ clear(); return append (s1); }
    inline string&		assign (const string& s)			{ return assign (s.begin(), s.end()); }
    inline string&		assign (const string& s, pos_type o, size_type n)	{ return assign (s.iat(o), s.iat(o+n)); }
    inline string&		assign (size_type n, value_type c)		{ clear(); return append (n, c); }
    inline string&		assign (size_type n, wvalue_type c)		{ clear(); return append (n, c); }
    size_type			copy (pointer p, size_type n, pos_type pos = 0) const noexcept;
    inline size_type		copyto (pointer p, size_type n, pos_type pos = 0) const noexcept { size_type bc = copy(p,n-1,pos); p[bc]=0; return bc; }
    inline int			compare (const string& s) const		{ return compare (begin(), end(), s.begin(), s.end()); }
    inline int			compare (pos_type start, size_type len, const string& s) const	{ return compare (iat(start), iat(start+len), s.begin(), s.end()); }
    inline int			compare (pos_type s1, size_type l1, const string& s, pos_type s2, size_type l2) const	{ return compare (iat(s1), iat(s1+l1), s.iat(s2), s.iat(s2+l2)); }
    inline int			compare (const_pointer s) const		{ return compare (begin(), end(), s, s + strlen(s)); }
    inline int			compare (pos_type s1, size_type l1, const_pointer s, size_type l2) const { return compare (iat(s1), iat(s1+l1), s, s+l2); }
    inline int			compare (pos_type s1, size_type l1, const_pointer s) const { return compare (s1, l1, s, strlen(s)); }
    static int			compare (const_iterator first1, const_iterator last1, const_iterator first2, const_iterator last2) noexcept;
    inline			operator const value_type* (void) const;
    inline			operator value_type* (void);
    inline const string&	operator= (const string& s)		{ return assign (s.begin(), s.end()); }
    inline const string&	operator= (const_reference c)		{ return assign (&c, 1); }
    inline const string&	operator= (const_pointer s)		{ return assign (s); }
    inline const string&	operator= (const_wpointer s)		{ return assign (s); }
    inline const string&	operator+= (const string& s)		{ return append (s.begin(), s.size()); }
    inline const string&	operator+= (value_type c)		{ push_back(c); return *this; }
    inline const string&	operator+= (const_pointer s)		{ return append (s); }
    inline const string&	operator+= (wvalue_type c)		{ return append (1, c); }
    inline const string&	operator+= (uvalue_type c)		{ return operator+= (value_type(c)); }
    inline const string&	operator+= (const_wpointer s)		{ return append (s); }
    inline string		operator+ (const string& s) const;
    inline bool			operator== (const string& s) const	{ return memblock::operator== (s); }
    bool			operator== (const_pointer s) const noexcept;
    inline bool			operator== (value_type c) const		{ return size() == 1 && c == at(0); }
    inline bool			operator== (uvalue_type c) const	{ return operator== (value_type(c)); }
    inline bool			operator!= (const string& s) const	{ return !operator== (s); }
    inline bool			operator!= (const_pointer s) const	{ return !operator== (s); }
    inline bool			operator!= (value_type c) const		{ return !operator== (c); }
    inline bool			operator!= (uvalue_type c) const	{ return !operator== (c); }
    inline bool			operator< (const string& s) const	{ return 0 > compare (s); }
    inline bool			operator< (const_pointer s) const	{ return 0 > compare (s); }
    inline bool			operator< (value_type c) const		{ return 0 > compare (begin(), end(), &c, &c + 1); }
    inline bool			operator< (uvalue_type c) const		{ return operator< (value_type(c)); }
    inline bool			operator> (const_pointer s) const	{ return 0 < compare (s); }
    inline string&		insert (pos_type ip, size_type n, value_type c)				{ insert (iat(ip), n, c); return *this; }
    inline string&		insert (pos_type ip, const_pointer s)					{ insert (iat(ip), s, s + strlen(s)); return *this; }
    inline string&		insert (pos_type ip, const_pointer s, size_type nlen)			{ insert (iat(ip), s, s + nlen); return *this; }
    inline string&		insert (pos_type ip, const string& s)					{ insert (iat(ip), s.c_str(), s.size()); return *this; }
    inline string&		insert (pos_type ip, const string& s, size_type sp, size_type slen)	{ insert (iat(ip), s.iat(sp), s.iat(sp + slen)); return *this; }
    string&			insert (pos_type ip, size_type n, wvalue_type c);
    string&			insert (pos_type ip, const_wpointer first, const_wpointer last, size_type n = 1);
    inline string&		insert (int ip, size_type n, value_type c)				{ insert (pos_type(ip), n, c); return *this; }
    inline string&		insert (int ip, const_pointer s, size_type nlen)			{ insert (pos_type(ip), s, nlen); return *this; }
    iterator			insert (const_iterator start, size_type n, value_type c);
    inline iterator		insert (const_iterator start, value_type c)				{ return insert (start, 1u, c); }
    iterator			insert (const_iterator start, const_pointer s, size_type n);
    iterator			insert (const_iterator start, const_pointer first, const_iterator last, size_type n = 1);
    iterator			erase (const_iterator epo, size_type n = 1);
    string&			erase (pos_type epo = 0, size_type n = npos);
    inline string&		erase (int epo, size_type n = npos)			{ return erase (pos_type(epo), n); }
    inline iterator		erase (const_iterator first, const_iterator last)	{ return erase (first, size_type(distance(first,last))); }
    inline iterator		eraser (pos_type first, pos_type last)			{ return erase (iat(first), iat(last)); }
    string&			replace (const_iterator first, const_iterator last, const_pointer i1, const_pointer i2, size_type n);
    template <typename InputIt>
    string&			replace (const_iterator first, const_iterator last, InputIt first2, InputIt last2)	{ return replace (first, last, first2, last2, 1); }
    inline string&		replace (const_iterator first, const_iterator last, const string& s)			{ return replace (first, last, s.begin(), s.end()); }
    string&			replace (const_iterator first, const_iterator last, const_pointer s);
    inline string&		replace (const_iterator first, const_iterator last, const_pointer s, size_type slen)	{ return replace (first, last, s, s + slen); }
    inline string&		replace (const_iterator first, const_iterator last, size_type n, value_type c)		{ return replace (first, last, &c, &c + 1, n); }
    inline string&		replace (pos_type rp, size_type n, const string& s)					{ return replace (iat(rp), iat(rp + n), s); }
    inline string&		replace (pos_type rp, size_type n, const string& s, uoff_t sp, size_type slen)		{ return replace (iat(rp), iat(rp + n), s.iat(sp), s.iat(sp + slen)); }
    inline string&		replace (pos_type rp, size_type n, const_pointer s, size_type slen)			{ return replace (iat(rp), iat(rp + n), s, s + slen); }
    inline string&		replace (pos_type rp, size_type n, const_pointer s)					{ return replace (iat(rp), iat(rp + n), string(s)); }
    inline string&		replace (pos_type rp, size_type n, size_type count, value_type c)			{ return replace (iat(rp), iat(rp + n), count, c); }
    inline string		substr (pos_type o = 0, size_type n = npos) const	{ return string (*this, o, n); }
    inline void			swap (string& v)					{ memblock::swap (v); }
    pos_type			find (value_type c, pos_type pos = 0) const noexcept;
    pos_type			find (const string& s, pos_type pos = 0) const noexcept;
    inline pos_type		find (uvalue_type c, pos_type pos = 0) const noexcept		{ return find (value_type(c), pos); }
    inline pos_type		find (const_pointer p, pos_type pos, size_type count) const	{ string sp; sp.link (p,count); return find (sp, pos); }
    pos_type			rfind (value_type c, pos_type pos = npos) const noexcept;
    pos_type			rfind (const string& s, pos_type pos = npos) const noexcept;
    inline pos_type		rfind (uvalue_type c, pos_type pos = npos) const noexcept	{ return rfind (value_type(c), pos); }
    inline pos_type		rfind (const_pointer p, pos_type pos, size_type count) const	{ string sp; sp.link (p,count); return rfind (sp, pos); }
    pos_type			find_first_of (const string& s, pos_type pos = 0) const noexcept;
    inline pos_type		find_first_of (value_type c, pos_type pos = 0) const				{ string sp (1, c); return find_first_of(sp,pos); }
    inline pos_type		find_first_of (uvalue_type c, pos_type pos = 0) const				{ return find_first_of (value_type(c), pos); }
    inline pos_type		find_first_of (const_pointer p, pos_type pos, size_type count) const		{ string sp; sp.link (p,count); return find_first_of (sp, pos); }
    pos_type			find_first_not_of (const string& s, pos_type pos = 0) const noexcept;
    inline pos_type		find_first_not_of (value_type c, pos_type pos = 0) const			{ string sp (1, c); return find_first_not_of(sp,pos); }
    inline pos_type		find_first_not_of (uvalue_type c, pos_type pos = 0) const			{ return find_first_not_of (value_type(c), pos); }
    inline pos_type		find_first_not_of (const_pointer p, pos_type pos, size_type count) const	{ string sp; sp.link (p,count); return find_first_not_of (sp, pos); }
    pos_type			find_last_of (const string& s, pos_type pos = npos) const noexcept;
    inline pos_type		find_last_of (value_type c, pos_type pos = npos) const				{ string sp (1, c); return find_last_of(sp,pos); }
    inline pos_type		find_last_of (uvalue_type c, pos_type pos = npos) const				{ return find_last_of (value_type(c), pos); }
    inline pos_type		find_last_of (const_pointer p, pos_type pos, size_type count) const		{ string sp; sp.link (p,count); return find_last_of (sp, pos); }
    pos_type			find_last_not_of (const string& s, pos_type pos = npos) const noexcept;
    inline pos_type		find_last_not_of (value_type c, pos_type pos = npos) const			{ string sp (1, c); return find_last_not_of(sp,pos); }
    inline pos_type		find_last_not_of (uvalue_type c, pos_type pos = npos) const			{ return find_last_not_of (value_type(c), pos); }
    inline pos_type		find_last_not_of (const_pointer p, pos_type pos, size_type count) const		{ string sp; sp.link (p,count); return find_last_not_of (sp, pos); }
    int				vformat (const char* fmt, va_list args);
    int				format (const char* fmt, ...) __attribute__((__format__(__printf__, 2, 3)));
    void			read (istream&);
    void			write (ostream& os) const;
    size_t			stream_size (void) const noexcept;
    static hashvalue_t		hash (const char* f1, const char* l1) noexcept;
#if HAVE_CPP11
    using initlist_t = std::initializer_list<value_type>;
    inline			string (string&& v)		: memblock (move(v)) {}
    inline			string (initlist_t v)		: memblock() { assign (v.begin(), v.size()); }
    inline string&		assign (string&& v)		{ swap (v); return *this; }
    inline string&		assign (initlist_t v)		{ return assign (v.begin(), v.size()); }
    inline string&		append (initlist_t v)		{ return append (v.begin(), v.size()); }
    inline string&		operator+= (initlist_t v)	{ return append (v.begin(), v.size()); }
    inline string&		operator= (string&& v)		{ return assign (move(v)); }
    inline string&		operator= (initlist_t v)	{ return assign (v.begin(), v.size()); }
    inline iterator		insert (const_iterator ip, initlist_t v)	{ return insert (ip, v.begin(), v.end()); }
    inline string&		replace (const_iterator first, const_iterator last, initlist_t v)	{ return replace (first, last, v.begin(), v.end()); }
#endif
private:
    virtual size_type		minimumFreeCapacity (void) const noexcept final override __attribute__((const));
};

//----------------------------------------------------------------------

/// Assigns itself the value of string \p s
inline string::string (const cmemlink& s)
: memblock ()
{
    assign (const_iterator (s.begin()), s.size());
}

/// Assigns itself a [o,o+n) substring of \p s.
inline string::string (const string& s, pos_type o, size_type n)
: memblock()
{
    assign (s, o, n);
}

/// Copies the value of \p s of length \p len into itself.
inline string::string (const_pointer s, size_type len)
: memblock ()
{
    assign (s, len);
}

/// Copies into itself the string data between \p s1 and \p s2
inline string::string (const_pointer s1, const_pointer s2)
: memblock ()
{
    assert (s1 <= s2 && "Negative ranges result in memory allocation errors.");
    assign (s1, s2);
}

/// Returns the pointer to the first character.
inline string::operator const string::value_type* (void) const
{
    assert ((!end() || !*end()) && "This string is linked to data that is not 0-terminated. This may cause serious security problems. Please assign the data instead of linking.");
    return begin();
}

/// Returns the pointer to the first character.
inline string::operator string::value_type* (void)
{
    assert ((end() && !*end()) && "This string is linked to data that is not 0-terminated. This may cause serious security problems. Please assign the data instead of linking.");
    return begin();
}

/// Concatenates itself with \p s
inline string string::operator+ (const string& s) const
{
    string result (*this);
    result += s;
    return result;
}

/// Resize to \p n and fill new entries with \p c
inline void string::resize (size_type n, value_type c)
{
    const size_type oldn = size();
    resize (n);
    fill_n (iat(oldn), max(ssize_t(n-oldn),0), c);
}

//----------------------------------------------------------------------
// Operators needed to avoid comparing pointer to pointer

#define PTR_STRING_CMP(op, impl)	\
inline bool op (const char* s1, const string& s2) { return impl; }
PTR_STRING_CMP (operator==, (s2 == s1))
PTR_STRING_CMP (operator!=, (s2 != s1))
PTR_STRING_CMP (operator<,  (s2 >  s1))
PTR_STRING_CMP (operator<=, (s2 >= s1))
PTR_STRING_CMP (operator>,  (s2 <  s1))
PTR_STRING_CMP (operator>=, (s2 <= s1))
#undef PTR_STRING_CMP

inline string operator+ (const char* cs, const string& ss) { string r; r.reserve (strlen(cs)+ss.size()); r += cs; r += ss; return r; }

//----------------------------------------------------------------------

inline hashvalue_t hash_value (const char* first, const char* last)
{ return string::hash (first, last); }
inline hashvalue_t hash_value (const char* v)
{ return hash_value (v, v + strlen(v)); }

//----------------------------------------------------------------------
// String-number conversions

#define STRING_TO_INT_CONVERTER(name,type,func)	\
inline type name (const string& str, size_t* idx = nullptr, int base = 10) \
{					\
    const char* sp = str.c_str();	\
    char* endp = nullptr;		\
    type r = func (sp, idx ? &endp : nullptr, base);\
    if (idx)				\
	*idx = endp - sp;		\
    return r;				\
}
STRING_TO_INT_CONVERTER(stoi,int,strtol)
STRING_TO_INT_CONVERTER(stol,long,strtol)
STRING_TO_INT_CONVERTER(stoul,unsigned long,strtoul)
#if HAVE_LONG_LONG
STRING_TO_INT_CONVERTER(stoll,long long,strtoll)
STRING_TO_INT_CONVERTER(stoull,unsigned long long,strtoull)
#endif
#undef STRING_TO_INT_CONVERTER

#define STRING_TO_FLOAT_CONVERTER(name,type,func) \
inline type name (const string& str, size_t* idx = nullptr) \
{					\
    const char* sp = str.c_str();	\
    char* endp = nullptr;		\
    type r = func (sp, idx ? &endp : nullptr);\
    if (idx)				\
	*idx = endp - sp;		\
    return r;				\
}
STRING_TO_FLOAT_CONVERTER(stof,float,strtof)
STRING_TO_FLOAT_CONVERTER(stod,double,strtod)
STRING_TO_FLOAT_CONVERTER(stold,long double,strtold)
#undef STRING_TO_FLOAT_CONVERTER

#define NUMBER_TO_STRING_CONVERTER(type,fmts)\
    inline string to_string (type v) { string r; r.format(fmts,v); return r; }
NUMBER_TO_STRING_CONVERTER(int,"%d")
NUMBER_TO_STRING_CONVERTER(long,"%ld")
NUMBER_TO_STRING_CONVERTER(unsigned long,"%lu")
#if HAVE_LONG_LONG
NUMBER_TO_STRING_CONVERTER(long long,"%lld")
NUMBER_TO_STRING_CONVERTER(unsigned long long,"%llu")
#endif
NUMBER_TO_STRING_CONVERTER(float,"%f")
NUMBER_TO_STRING_CONVERTER(double,"%lf")
NUMBER_TO_STRING_CONVERTER(long double,"%Lf")
#undef NUMBER_TO_STRING_CONVERTER

} // namespace ustl

namespace ustl {

/// \class set uset.h ustl.h
/// \ingroup Sequences
///
/// \brief Unique sorted container. Sorted vector with all values unique.
///
template <typename T, typename Comp = less<T> >
class set : public vector<T> {
public:
    typedef const set<T,Comp>&				rcself_t;
    typedef vector<T>					base_class;
    typedef typename base_class::value_type		key_type;
    typedef typename base_class::value_type		data_type;
    typedef typename base_class::value_type		value_type;
    typedef typename base_class::size_type		size_type;
    typedef typename base_class::pointer		pointer;
    typedef typename base_class::const_pointer		const_pointer;
    typedef typename base_class::reference		reference;
    typedef typename base_class::const_reference	const_reference;
    typedef typename base_class::const_iterator		const_iterator;
    typedef typename base_class::iterator		iterator;
    typedef typename base_class::reverse_iterator	reverse_iterator;
    typedef typename base_class::const_reverse_iterator	const_reverse_iterator;
    typedef pair<iterator,bool>				insertrv_t;
    typedef pair<iterator,iterator>			range_t;
    typedef pair<const_iterator,const_iterator>		const_range_t;
public:
    inline			set (void)		: base_class() { }
    explicit inline		set (size_type n)	: base_class (n) { }
    inline			set (rcself_t v)	: base_class (v) { }
    inline			set (const_iterator i1, const_iterator i2) : base_class() { insert (i1, i2); }
    inline rcself_t		operator= (rcself_t v)	{ base_class::operator= (v); return *this; }
    inline size_type		size (void) const	{ return base_class::size(); }
    inline iterator		begin (void)		{ return base_class::begin(); }
    inline const_iterator	begin (void) const	{ return base_class::begin(); }
    inline const_iterator	cbegin (void) const	{ return base_class::cbegin(); }
    inline iterator		end (void)		{ return base_class::end(); }
    inline const_iterator	end (void) const	{ return base_class::end(); }
    inline const_iterator	cend (void) const	{ return base_class::cend(); }
    inline const_iterator	find (const_reference v) const	{ const_iterator i = ::ustl::lower_bound (begin(), end(), v, Comp()); return (i != end() && *i == v) ? i : end(); }
    inline iterator		find (const_reference v)	{ return const_cast<iterator>(const_cast<rcself_t>(*this).find (v)); }
    inline const_iterator	lower_bound (const_reference v) const	{ return ::ustl::lower_bound (begin(), end(), v, Comp()); }
    inline iterator		lower_bound (const_reference v)		{ return const_cast<iterator>(const_cast<rcself_t>(*this).lower_bound (v)); }
    inline const_iterator	upper_bound (const_reference v) const	{ return ::ustl::upper_bound (begin(), end(), v, Comp()); }
    inline iterator		upper_bound (const_reference v)		{ return const_cast<iterator>(const_cast<rcself_t>(*this).upper_bound (v)); }
    inline const_range_t	equal_range (const_reference v) const	{ return ::ustl::equal_range (begin(), end(), v, Comp()); }
    inline range_t		equal_range (const_reference v)		{ return ::ustl::equal_range (begin(), end(), v, Comp()); }
    inline size_type		count (const_reference v) const		{ const_range_t r = equal_range(v); return distance(r.first,r.second); }
    inline Comp			value_comp (void) const			{ return Comp(); }
    inline Comp			key_comp (void) const			{ return value_comp(); }
    inline void			assign (const_iterator i1, const_iterator i2)	{ clear(); insert (i1, i2); }
    inline void			push_back (const_reference v)		{ insert (v); }
    insertrv_t			insert (const_reference v);
    inline iterator		insert (const_iterator, const_reference v)	{ return insert(v).first; }
    inline void			insert (const_iterator i1, const_iterator i2)	{ for (; i1 < i2; ++i1) insert (*i1); }
    inline void			erase (const_reference v)		{ iterator ip = find (v); if (ip != end()) erase (ip); }
    inline iterator		erase (iterator ep)			{ return base_class::erase (ep); }
    inline iterator		erase (iterator ep1, iterator ep2)	{ return base_class::erase (ep1, ep2); }
    inline void			clear (void)				{ base_class::clear(); }
    inline void			swap (set& v)				{ base_class::swap (v); }
#if HAVE_CPP11
    using initlist_t = std::initializer_list<value_type>;
    inline			set (set&& v)				: base_class (move(v)) {}
    inline			set (initlist_t v)			: base_class() { insert (v.begin(), v.end()); }
    inline set&			operator= (set&& v)			{ base_class::operator= (move(v)); return *this; }
    insertrv_t			insert (T&& v);
    inline iterator		insert (const_iterator, T&& v)		{ return insert (move(v)); }
    inline void			insert (initlist_t v)			{ insert (v.begin(), v.end()); }
    template <typename... Args>
    inline insertrv_t		emplace (Args&&... args)		{ return insert (T(forward<Args>(args)...)); }
    template <typename... Args>
    inline iterator		emplace_hint (const_iterator h, Args&&... args)	{ return insert (h, T(forward<Args>(args)...)); }
    template <typename... Args>
    inline insertrv_t		emplace_back (Args&&... args)		{ return insert (T(forward<Args>(args)...)); }
#endif
};

#if HAVE_CPP11

template <typename T, typename Comp>
typename set<T,Comp>::insertrv_t set<T,Comp>::insert (T&& v)
{
    iterator ip = lower_bound (v);
    bool bInserted = (ip == end() || Comp()(v,*ip));
    if (bInserted)
	ip = base_class::insert (ip, move(v));
    return make_pair (ip, bInserted);
}

#endif

/// Inserts \p v into the container, maintaining the sort order.
template <typename T, typename Comp>
typename set<T,Comp>::insertrv_t set<T,Comp>::insert (const_reference v)
{
    iterator ip = lower_bound (v);
    bool bInserted = (ip == end() || Comp()(v,*ip));
    if (bInserted)
	ip = base_class::insert (ip, v);
    return make_pair (ip, bInserted);
}

} // namespace ustl

namespace ustl {

/// \class multiset umultiset.h ustl.h
/// \ingroup AssociativeContainers
///
/// \brief Multiple sorted container.
/// Unlike set, it may contain multiple copies of each element.
///
template <typename T, typename Comp = less<T> >
class multiset : public vector<T> {
public:
    typedef const multiset<T,Comp>&			rcself_t;
    typedef vector<T>					base_class;
    typedef typename base_class::value_type		value_type;
    typedef typename base_class::size_type		size_type;
    typedef typename base_class::pointer		pointer;
    typedef typename base_class::const_pointer		const_pointer;
    typedef typename base_class::reference		reference;
    typedef typename base_class::const_reference	const_reference;
    typedef typename base_class::const_iterator		const_iterator;
    typedef typename base_class::iterator		iterator;
    typedef typename base_class::reverse_iterator	reverse_iterator;
    typedef typename base_class::const_reverse_iterator	const_reverse_iterator;
    typedef pair<iterator,iterator>			range_t;
    typedef pair<const_iterator,const_iterator>		const_range_t;
public:
    inline			multiset (void)				: base_class() {}
    inline explicit		multiset (size_type n)			: base_class (n) {}
    inline explicit		multiset (rcself_t v)			: base_class (v) {}
    inline			multiset (const_iterator i1, const_iterator i2) : base_class() { insert (i1, i2); }
    inline rcself_t		operator= (rcself_t v)			{ base_class::operator= (v); return *this; }
    inline size_type		size (void) const			{ return base_class::size(); }
    inline iterator		begin (void)				{ return base_class::begin(); }
    inline const_iterator	begin (void) const			{ return base_class::begin(); }
    inline const_iterator	cbegin (void) const			{ return base_class::cbegin(); }
    inline iterator		end (void)				{ return base_class::end(); }
    inline const_iterator	end (void) const			{ return base_class::end(); }
    inline const_iterator	cend (void) const			{ return base_class::cend(); }
    inline Comp			value_comp (void) const			{ return Comp(); }
    inline Comp			key_comp (void) const			{ return value_comp(); }
    inline void			assign (const_iterator i1, const_iterator i2)	{ clear(); insert (i1, i2); }
    inline const_iterator	find (const_reference v) const		{ const_iterator i = ::ustl::lower_bound (begin(), end(), v, Comp()); return (i != end() && *i == v) ? i : end(); }
    inline iterator		find (const_reference v)		{ return const_cast<iterator>(const_cast<rcself_t>(*this).find (v)); }
    inline const_iterator	lower_bound (const_reference v) const	{ return ::ustl::lower_bound (begin(), end(), v, Comp()); }
    inline iterator		lower_bound (const_reference v)		{ return const_cast<iterator>(const_cast<rcself_t>(*this).lower_bound (v)); }
    inline const_iterator	upper_bound (const_reference v) const	{ return ::ustl::upper_bound (begin(), end(), v, Comp()); }
    inline iterator		upper_bound (const_reference v)		{ return const_cast<iterator>(const_cast<rcself_t>(*this).upper_bound (v)); }
    inline const_range_t	equal_range (const_reference v) const	{ return ::ustl::equal_range (begin(), end(), v, Comp()); }
    inline range_t		equal_range (const_reference v)		{ return ::ustl::equal_range (begin(), end(), v, Comp()); }
    inline size_type		count (const_reference v) const		{ const_range_t r = equal_range(v); return distance(r.first,r.second); }
    inline void			push_back (const_reference v)		{ insert (v); }
    inline iterator		insert (const_reference v)		{ return base_class::insert (upper_bound(v), v); }
    inline iterator		insert (const_iterator, const_reference v)	{ return insert(v); }
    void			insert (const_iterator i1, const_iterator i2)	{ for (; i1 < i2; ++i1) insert (*i1); }
    inline iterator		erase (const_iterator ep)			{ return base_class::erase (ep); }
    inline iterator		erase (const_iterator ep1, const_iterator ep2)	{ return base_class::erase (ep1, ep2); }
    inline size_type		erase (const_reference v)		{ range_t epr = equal_range (v); erase (epr.first, epr.second); return distance(epr.first, epr.second); }
    inline void			clear (void)				{ base_class::clear(); }
    inline void			swap (multiset& v)			{ base_class::swap (v); }
#if HAVE_CPP11
    using initlist_t = std::initializer_list<value_type>;
    inline explicit		multiset (multiset&& v)			: base_class (move(v)) {}
    inline			multiset (initlist_t v)			: base_class() { insert (v.begin(), v.end()); }
    inline multiset&		operator= (multiset&& v)		{ base_class::operator= (move(v)); return *this; }
    inline iterator		insert (T&& v)				{ return base_class::insert (upper_bound(v), move(v)); }
    inline iterator		insert (const_iterator, T&& v)		{ return insert (move(v)); }
    inline void			insert (initlist_t v)			{ insert (v.begin(), v.end()); }
    template <typename... Args>
    inline iterator		emplace (Args&&... args)		{ return insert (T(forward<Args>(args)...)); }
    template <typename... Args>
    inline iterator		emplace_hint (const_iterator h, Args&&... args)	{ return insert (h, T(forward<Args>(args)...)); }
    template <typename... Args>
    inline iterator		emplace_back (Args&&... args)		{ return insert (T(forward<Args>(args)...)); }
#endif
};

} // namespace ustl

namespace ustl {

typedef uint32_t	bitset_value_type;

void convert_to_bitstring (const bitset_value_type* v, size_t n, string& buf) noexcept;
void convert_from_bitstring (const string& buf, bitset_value_type* v, size_t n) noexcept;

/// \class bitset ubitset.h ustl.h
/// \ingroup Sequences
///
/// \brief bitset is a fixed-size block of memory with addressable bits.
///
/// Normally used for state flags; allows setting and unsetting of individual
/// bits as well as bitwise operations on the entire set. The interface is
/// most like that of unsigned integers, and is intended to be used as such.
/// If you were using begin() and end() functions in STL's bitset, you would
/// not be able to do the same thing here, because those functions return
/// host type iterators, not bits.
///
template <size_t Size>
class bitset {
public:
    typedef bitset_value_type	value_type;
    typedef value_type*		pointer;
    typedef const value_type*	const_pointer;
    typedef pointer		iterator;
    typedef const_pointer	const_iterator;
    typedef size_t		difference_type;
    typedef size_t		size_type;
    typedef const bitset<Size>&	rcself_t;
private:
    static const size_t s_WordBits	= BitsInType (value_type);
    static const size_t	s_nWords	= Size / s_WordBits + ((Size % s_WordBits) != 0);
    static const size_t	s_nBits		= s_nWords * s_WordBits;
private:
    inline value_type&		BitRef (uoff_t n)	{ assert (n < Size); return _bits [n / s_WordBits]; }
    inline value_type		BitRef (uoff_t n) const	{ assert (n < Size); return _bits [n / s_WordBits]; }
    inline value_type		Mask (uoff_t n) const	{ assert (n < Size); return 1 << (n % s_WordBits); }
public:
    inline		bitset (value_type v = 0)	{ fill_n (_bits, s_nWords, 0); _bits[0] = v; }
    inline		bitset (const string& buf)	{ convert_from_bitstring (buf, _bits, s_nWords); }
    inline void		flip (uoff_t n)			{ BitRef(n) ^= Mask(n); }
    inline void		reset (void)			{ fill_n (_bits, s_nWords, 0); }
    inline void		clear (void)			{ fill_n (_bits, s_nWords, 0); }
    inline void		set (void)			{ fill_n (_bits, s_nWords, -1); }
    inline bitset	operator~ (void) const		{ bitset rv (*this); rv.flip(); return rv; }
    inline size_type	size (void) const		{ return Size; }
    inline size_type	capacity (void) const		{ return s_nBits; }
    inline bool		test (uoff_t n) const		{ return BitRef(n) & Mask(n); }
    inline bool		operator[] (uoff_t n) const	{ return test(n); }
  inline const_iterator	begin (void) const		{ return _bits; }
    inline iterator	begin (void)			{ return _bits; }
  inline const_iterator	end (void) const		{ return _bits + s_nWords; }
    inline iterator	end (void)			{ return _bits + s_nWords; }
 			/// Returns the value_type with the equivalent bits. If size() > 1, you'll get only the first BitsInType(value_type) bits.
    inline value_type	to_value (void) const		{ return _bits[0]; }
    			/// Flips all the bits in the set.
    inline void		flip (void) { transform (begin(), end(), begin(), bitwise_not<value_type>()); }
			/// Sets or clears bit \p n.
    inline void		set (uoff_t n, bool val = true)
			{
			    value_type& br (BitRef (n));
			    const value_type mask (Mask (n));
			    const value_type bOn (br | mask), bOff (br & ~mask);
			    br = val ? bOn : bOff;
			}
			// Sets the value of the bitrange \p first through \p last to the equivalent number of bits from \p v.
    inline void		set (uoff_t first, uoff_t DebugArg(last), value_type v)
			{
			    assert (size_t (distance (first, last)) <= s_WordBits && "Bit ranges must be 32 bits or smaller");
			    assert (first / s_WordBits == last / s_WordBits && "Bit ranges can not cross dword (4 byte) boundary");
			    assert ((v & BitMask(value_type,distance(first,last))) == v && "The value is too large to fit in the given bit range");
			    BitRef(first) |= v << (first % s_WordBits);
			}
    			/// Clears the bit \p n.
    inline void		reset (uoff_t n)		{ set (n, false); }
			/// Returns a string with bits MSB "001101001..." LSB.
    inline string	to_string (void) const
			{
			    string rv (Size, '0');
			    convert_to_bitstring (_bits, s_nWords, rv);
			    return rv;
			}
    inline value_type	at (uoff_t n) const		{ return test(n); }
			/// Returns the value in bits \p first through \p last.
    inline value_type	at (uoff_t first, uoff_t last) const
			{
			    assert (size_t (distance (first, last)) <= s_WordBits && "Bit ranges must be 32 bits or smaller");
			    assert (first / s_WordBits == last / s_WordBits && "Bit ranges can not cross dword (4 byte) boundary");
			    return (BitRef(first) >> (first % s_WordBits)) & BitMask(value_type,distance(first, last));
			}
    inline bool		any (void) const	{ value_type sum = 0; foreach (const_iterator, i, *this) sum |= *i; return sum; }
    inline bool		none (void) const	{ return !any(); }
    inline size_t	count (void) const	{ size_t sum = 0; foreach (const_iterator, i, *this) sum += popcount(*i); return sum; }
    inline bool		operator== (rcself_t v) const
			    { return s_nWords == 1 ? (_bits[0] == v._bits[0]) : equal (begin(), end(), v.begin()); }
    inline bitset	operator& (rcself_t v) const
			    { bitset<Size> result; transform (begin(), end(), v.begin(), result.begin(), bitwise_and<value_type>()); return result; }
    inline bitset	operator| (rcself_t v) const
			    { bitset<Size> result; transform (begin(), end(), v.begin(), result.begin(), bitwise_or<value_type>()); return result; }
    inline bitset	operator^ (rcself_t v) const
			    { bitset<Size> result; transform (begin(), end(), v.begin(), result.begin(), bitwise_xor<value_type>()); return result; }
    inline rcself_t	operator&= (rcself_t v)
			    { transform (begin(), end(), v.begin(), begin(), bitwise_and<value_type>()); return *this; }
    inline rcself_t	operator|= (rcself_t v)
			    { transform (begin(), end(), v.begin(), begin(), bitwise_or<value_type>()); return *this; }
    inline rcself_t	operator^= (rcself_t v)
			    { transform (begin(), end(), v.begin(), begin(), bitwise_xor<value_type>()); return *this; }
    inline void		read (istream& is)			{ nr_container_read (is, *this); }
    inline void		write (ostream& os) const		{ nr_container_write (os, *this); }
    inline void		text_write (ostringstream& os) const	{ os << to_string(); }
    inline size_t	stream_size (void) const		{ return sizeof(_bits); }
private:
    value_type		_bits [s_nWords];
};

} // namespace ustl

namespace ustl {

/// \class tuple utuple.h ustl.h
/// \ingroup Sequences
///
/// \brief A fixed-size array of \p N \p Ts.
///
template <size_t N, typename T>
class tuple {
public:
    typedef T						value_type;
    typedef size_t					size_type;
    typedef value_type*					pointer;
    typedef const value_type*				const_pointer;
    typedef value_type&					reference;
    typedef const value_type&				const_reference;
    typedef pointer					iterator;
    typedef const_pointer				const_iterator;
    typedef ::ustl::reverse_iterator<iterator>		reverse_iterator;
    typedef ::ustl::reverse_iterator<const_iterator>	const_reverse_iterator;
    typedef pair<iterator,iterator>			range_t;
    typedef pair<const_iterator,const_iterator>		const_range_t;
public:
    template <typename T2>
    inline			tuple (const tuple<N,T2>& t);
    inline			tuple (const tuple<N,T>& t);
    inline			tuple (const_pointer v);
    inline			tuple (void);
    inline explicit		tuple (const_reference v0);
    inline			tuple (const_reference v0, const_reference v1);
    inline			tuple (const_reference v0, const_reference v1, const_reference v2);
    inline			tuple (const_reference v0, const_reference v1, const_reference v2, const_reference v3);
    inline iterator		begin (void)			{ return _v; }
    inline const_iterator	begin (void) const		{ return _v; }
    inline iterator		end (void)			{ return begin() + N; }
    inline const_iterator	end (void) const		{ return begin() + N; }
    inline size_type		size (void) const		{ return N; }
    inline size_type		max_size (void) const		{ return N; }
    inline bool			empty (void) const		{ return N == 0; }
    inline const_reference	at (size_type i) const		{ return _v[i]; }
    inline reference		at (size_type i)		{ return _v[i]; }
    inline const_reference	operator[] (size_type i) const	{ return _v[i]; }
    inline reference		operator[] (size_type i)	{ return _v[i]; }
    template <typename T2>
    inline tuple&		operator= (const tuple<N,T2>& src);
    inline tuple&		operator= (const tuple<N,T>& src);
    inline tuple&		operator+= (const_reference v)
				    { for (uoff_t i = 0; i < N; ++ i) _v[i] += v; return *this; }
    inline tuple&		operator-= (const_reference v)
				    { for (uoff_t i = 0; i < N; ++ i) _v[i] -= v; return *this; }
    inline tuple&		operator*= (const_reference v)
				    { for (uoff_t i = 0; i < N; ++ i) _v[i] *= v; return *this; }
    inline tuple&		operator/= (const_reference v)
				    { for (uoff_t i = 0; i < N; ++ i) _v[i] /= v; return *this; }
    inline tuple		operator+ (const_reference v) const
				    { tuple result; for (uoff_t i = 0; i < N; ++ i) result[i] = _v[i] + v; return result; }
    inline tuple		operator- (const_reference v) const
				    { tuple result; for (uoff_t i = 0; i < N; ++ i) result[i] = _v[i] - v; return result; }
    inline tuple		operator* (const_reference v) const
				    { tuple result; for (uoff_t i = 0; i < N; ++ i) result[i] = _v[i] * v; return result; }
    inline tuple		operator/ (const_reference v) const
				    { tuple result; for (uoff_t i = 0; i < N; ++ i) result[i] = _v[i] / v; return result; }
    inline void			swap (tuple<N,T>& v)
				    { for (uoff_t i = 0; i < N; ++ i) ::ustl::swap (_v[i], v._v[i]); }
    inline void			read (istream& is)			{ nr_container_read (is, *this); }
    inline void			write (ostream& os) const		{ nr_container_write (os, *this); }
    inline void			text_write (ostringstream& os) const	{ container_text_write (os, *this); }
    inline size_t		stream_size (void) const		{ return nr_container_stream_size (*this); }
#if HAVE_CPP11
    using initlist_t = std::initializer_list<value_type>;
    inline			tuple (initlist_t v)			{ assign (v); }
    inline tuple&		assign (initlist_t v);
    inline tuple&		operator= (initlist_t v)		{ return assign(v); }
    inline tuple&		operator+= (initlist_t v)
				    { for (uoff_t i = 0; i < min(N,v.size()); ++ i) _v[i] += v.begin()[i]; return *this; }
    inline tuple&		operator-= (initlist_t v)
				    { for (uoff_t i = 0; i < min(N,v.size()); ++ i) _v[i] -= v.begin()[i]; return *this; }
    inline tuple&		operator*= (initlist_t v)
				    { for (uoff_t i = 0; i < min(N,v.size()); ++ i) _v[i] *= v.begin()[i]; return *this; }
    inline tuple&		operator/= (initlist_t v)
				    { for (uoff_t i = 0; i < min(N,v.size()); ++ i) _v[i] /= v.begin()[i]; return *this; }
#endif
private:
    T				_v [N];
};

} // namespace ustl

#if HAVE_MATH_H
    #include <math.h>
#endif

namespace ustl {
namespace simd {

//----------------------------------------------------------------------
// Generic algorithms
//----------------------------------------------------------------------

/// Applies \p op to each element in \p op1.
template <typename Ctr, typename UnaryOperation>
inline void packop (Ctr& op1, UnaryOperation op)
{
    foreach (typename Ctr::iterator, i, op1)
	op (*i);
}

/// Applies \p op to each element in \p op1 and \p op2 and stores in \p op2.
template <typename Ctr, typename BinaryOperation>
inline void packop (const Ctr& op1, Ctr& op2, BinaryOperation op)
{
    assert (op2.size() <= op1.size());
    typename Ctr::const_iterator i1 (op1.begin());
    typename Ctr::iterator i2 (op2.begin());
    for (; i2 != op2.end(); ++i1, ++i2)
	*i2 = op (*i2, *i1);
}

/// Applies \p op to corresponding elements in \p op1 and \p op2 and stores in \p result.
template <typename Ctr, typename BinaryOperation>
inline void packop (const Ctr& op1, const Ctr& op2, Ctr& result, BinaryOperation op)
{
    assert (op1.size() <= op2.size() && op1.size() <= result.size());
    passign (op1, result);
    op (op2, result);
}

/// Copies \p op1 into \p result.
template <typename Ctr>
inline void passign (const Ctr& op1, Ctr& result)
{
    assert (op1.size() <= result.size());
    typename Ctr::iterator d (result.begin());
    foreach (typename Ctr::const_iterator, s, op1)
	*d++ = *s;
}

/// Copies \p result.size() elements from \p op1 to \p result.
template <typename Ctr>
inline void ipassign (typename Ctr::const_iterator op1, Ctr& result)
{
    foreach (typename Ctr::iterator, d, result)
	*d = *op1++;
}

template <typename Ctr1, typename Ctr2, typename ConvertFunction>
inline void pconvert (const Ctr1& op1, Ctr2& op2, ConvertFunction f)
{
    assert (op1.size() <= op2.size());
    typename Ctr1::const_iterator i1 (op1.begin());
    typename Ctr2::iterator i2 (op2.begin());
    for (; i1 != op1.end(); ++i1, ++i2)
	*i2 = f (*i1);
}

// Functionoids for SIMD operations, like saturation arithmetic, shifts, etc.
STD_BINARY_FUNCTOR (fpadds, T, ((b > numeric_limits<T>::max() - a) ? numeric_limits<T>::max() : a + b))
STD_BINARY_FUNCTOR (fpsubs, T, ((a < numeric_limits<T>::min() + b) ? numeric_limits<T>::min() : a - b))
STD_BINARY_FUNCTOR (fpshl,  T, (a << b))
STD_BINARY_FUNCTOR (fpshr,  T, (a >> b))
STD_BINARY_FUNCTOR (fpmin,  T, (min (a, b)))
STD_BINARY_FUNCTOR (fpmax,  T, (max (a, b)))
STD_BINARY_FUNCTOR (fpavg,  T, ((a + b + 1) / 2))
STD_CONVERSION_FUNCTOR (fcast, (D(a)))
#if HAVE_MATH_H
STD_UNARY_FUNCTOR (fpreciprocal,T, (1 / a))
STD_UNARY_FUNCTOR (fpsqrt,	T, (reset_mmx(), T (sqrt (a))))
STD_UNARY_FUNCTOR (fprecipsqrt,	T, (reset_mmx(), 1 / T(sqrt (a))))
STD_UNARY_FUNCTOR (fsin,	T, (reset_mmx(), T (sin (a))))
STD_UNARY_FUNCTOR (fcos,	T, (reset_mmx(), T (cos (a))))
STD_UNARY_FUNCTOR (ftan,	T, (reset_mmx(), T (tan (a))))
#if HAVE_RINTF
STD_CONVERSION_FUNCTOR (fround, (reset_mmx(), D(rintf(a))))
#else
STD_CONVERSION_FUNCTOR (fround, (reset_mmx(), D(rint(a))))
#endif
template <> inline int32_t fround<double,int32_t>::operator()(const double& a) const { reset_mmx(); return int32_t(rint(a)); }
#endif
template <> inline float fpavg<float>::operator()(const float& a, const float& b) const { return (a + b) / 2; }
template <> inline double fpavg<double>::operator()(const double& a, const double& b) const { return (a + b) / 2; }

#define SIMD_PACKEDOP1(name, operation)		\
template <typename Ctr>				\
inline void name (Ctr& op1)			\
{						\
    typedef typename Ctr::value_type value_t;	\
    packop (op1, operation<value_t>());		\
}
#define SIMD_PACKEDOP2(name, operation)		\
template <typename Ctr>				\
inline void name (const Ctr& op1, Ctr& op2)	\
{						\
    typedef typename Ctr::value_type value_t;	\
    packop (op1, op2, operation<value_t>());	\
}
#define SIMD_PACKEDOP3(name, operation)			\
template <typename Ctr>					\
inline void name (const Ctr& op1, const Ctr& op2, Ctr& result)	\
{							\
    typedef typename Ctr::value_type value_t;		\
    packop (op1, op2, result, operation<value_t>());	\
}
#define SIMD_SINGLEOP1(name, operation)		\
template <typename T>				\
inline T name (T op)				\
{						\
    operation<T> obj;				\
    return obj(op);				\
}
#define SIMD_CONVERTOP(name, operation)		\
template <typename Ctr1, typename Ctr2>		\
inline void name (const Ctr1& op1, Ctr2& op2)	\
{						\
    typedef typename Ctr1::value_type value1_t;	\
    typedef typename Ctr2::value_type value2_t;	\
    pconvert (op1, op2, operation<value1_t, value2_t>());\
}

SIMD_PACKEDOP2 (padd, plus)
SIMD_PACKEDOP2 (psub, minus)
SIMD_PACKEDOP2 (pmul, multiplies)
SIMD_PACKEDOP2 (pdiv, divides)
SIMD_PACKEDOP2 (pand, bitwise_and)
SIMD_PACKEDOP2 (por, bitwise_or)
SIMD_PACKEDOP2 (pxor, bitwise_xor)
SIMD_PACKEDOP2 (pshl, fpshl)
SIMD_PACKEDOP2 (pshr, fpshr)
SIMD_PACKEDOP2 (psubs, fpsubs)
SIMD_PACKEDOP2 (pmin, fpmin)
SIMD_PACKEDOP2 (pmax, fpmax)
SIMD_PACKEDOP2 (pavg, fpavg)

SIMD_PACKEDOP3 (padd, plus)
SIMD_PACKEDOP3 (psub, minus)
SIMD_PACKEDOP3 (pmul, multiplies)
SIMD_PACKEDOP3 (pdiv, divides)
SIMD_PACKEDOP3 (pand, bitwise_and)
SIMD_PACKEDOP3 (por, bitwise_or)
SIMD_PACKEDOP3 (pxor, bitwise_xor)
SIMD_PACKEDOP3 (pshl, fpshl)
SIMD_PACKEDOP3 (pshr, fpshr)
SIMD_PACKEDOP3 (padds, fpadds)
SIMD_PACKEDOP3 (psubs, fpsubs)
SIMD_PACKEDOP3 (pmin, fpmin)
SIMD_PACKEDOP3 (pmax, fpmax)
SIMD_PACKEDOP3 (pavg, fpavg)

#if HAVE_MATH_H
SIMD_PACKEDOP1 (precip, fpreciprocal)
SIMD_PACKEDOP1 (psqrt, fpsqrt)
SIMD_PACKEDOP1 (precipsqrt, fprecipsqrt)
SIMD_PACKEDOP1 (psin, fsin)
SIMD_PACKEDOP1 (pcos, fcos)
SIMD_PACKEDOP1 (ptan, ftan)

SIMD_SINGLEOP1 (srecip, fpreciprocal)
SIMD_SINGLEOP1 (ssqrt, fpsqrt)
SIMD_SINGLEOP1 (srecipsqrt, fprecipsqrt)
SIMD_SINGLEOP1 (ssin, fsin)
SIMD_SINGLEOP1 (scos, fcos)
SIMD_SINGLEOP1 (stan, ftan)

SIMD_CONVERTOP (pround, fround)

template <typename T> inline int32_t sround (T op) { fround<T,int32_t> obj; return obj (op); }
#endif

#undef SIMD_SINGLEOP1
#undef SIMD_PACKEDOP3
#undef SIMD_PACKEDOP2
#undef SIMD_PACKEDOP1

//----------------------------------------------------------------------
// Vector types to cast tuple data to
//----------------------------------------------------------------------

#if HAVE_VECTOR_EXTENSIONS && __GNUC__ >= 4
#define VECTOR_ATTRIBUTE(mode,vs)	__attribute__((vector_size(vs)))
#else
#define VECTOR_ATTRIBUTE(mode,vs)
#endif
typedef uint8_t v8qi_t VECTOR_ATTRIBUTE (V8QI,8);
typedef uint16_t v4hi_t VECTOR_ATTRIBUTE (V4HI,8);
typedef uint16_t v8hi_t VECTOR_ATTRIBUTE (V8HI,16);
typedef uint32_t v2si_t VECTOR_ATTRIBUTE (V2SI,8);
typedef uint32_t v4si_t VECTOR_ATTRIBUTE (V4SI,16);
#if HAVE_INT64_T
typedef uint64_t v1di_t VECTOR_ATTRIBUTE (V1DI,8);
#endif
typedef float v2sf_t VECTOR_ATTRIBUTE (V2SF,8);
typedef float v4sf_t VECTOR_ATTRIBUTE (V4SF,16);
typedef double v2df_t VECTOR_ATTRIBUTE (V2DF,16);
#undef VECTOR_ATTRIBUTE

#define SIMDA_RI(n)		"m"(oin[n])
#define SIMDA_RO(n)		"m"(oout[n])
#define SIMDA_WI(n)		"=m"(oin[n])
#define SIMDA_WO(n)		"=m"(oout[n])

//----------------------------------------------------------------------
// Hardware accelerated specializations
//----------------------------------------------------------------------

#define SIMD_PKOP2_SPEC(n, type, optype)	\
template <>					\
inline void packop (const tuple<n,type>& oin, tuple<n,type>& oout, optype<type>)
#define SIMD_PASSIGN_SPEC(n, type)		\
template <>					\
inline void passign (const tuple<n,type>& oin, tuple<n,type>& oout)
#define SIMD_IPASSIGN_SPEC(n, type)		\
template <>					\
inline void ipassign (tuple<n,type>::const_iterator oin, tuple<n,type>& oout)
#define SIMD_CONVERT_SPEC(n, type1, type2, optype)	\
template <>					\
inline void pconvert (const tuple<n,type1>& oin, tuple<n,type2>& oout, optype<type1,type2>)

#if CPU_HAS_MMX
#define STD_MMX_ARGS	: "m"(oout[0]), "m"(oin[0]) : "mm0", "st", "memory"
#define DBL_MMX_ARGS	: "m"(oout[0]), "m"(oout[2]), "m"(oin[0]), "m"(oin[2]) : "mm0", "mm1", "st", "st(1)", "memory"
#define MMX_PKOP2_SPEC(n,type,optype,instruction)	\
SIMD_PKOP2_SPEC(n,type,optype)		\
{ asm ("movq %0, %%mm0\n\t" #instruction " %1, %%mm0\n\tmovq %%mm0, %0" : STD_MMX_ARGS); reset_mmx(); }
#define MMX_DBL_PKOP2_SPEC(n,type,optype,instruction)	\
SIMD_PKOP2_SPEC(n,type,optype)		\
{ asm ("movq %0, %%mm0\n\tmovq %1, %%mm1\n\t" #instruction " %2, %%mm0\n\t" #instruction " %3, %%mm1\n\tmovq %%mm0, %0\n\tmovq %%mm1, %1" : DBL_MMX_ARGS); reset_mmx(); }
#define MMX_PASSIGN_SPEC(n,type)	\
SIMD_PASSIGN_SPEC(n,type)		\
{ asm ("movq %1, %%mm0\n\tmovq %%mm0, %0" : STD_MMX_ARGS); reset_mmx(); }
#define MMX_DBL_PASSIGN_SPEC(n,type)	\
SIMD_PASSIGN_SPEC(n,type)		\
{ asm ("movq %2, %%mm0\n\tmovq %3, %%mm1\n\tmovq %%mm0, %0\n\tmovq %%mm1, %1" : DBL_MMX_ARGS); reset_mmx(); }
#define MMX_IPASSIGN_SPEC(n,type)	\
SIMD_IPASSIGN_SPEC(n,type)		\
{ asm ("movq %1, %%mm0\n\tmovq %%mm0, %0" : STD_MMX_ARGS); reset_mmx(); }
#define MMX_DBL_IPASSIGN_SPEC(n,type)	\
SIMD_IPASSIGN_SPEC(n,type)		\
{ asm ("movq %2, %%mm0\n\tmovq %3, %%mm1\n\tmovq %%mm0, %0\n\tmovq %%mm1, %1" : DBL_MMX_ARGS); reset_mmx(); }

MMX_PASSIGN_SPEC(8,uint8_t)
MMX_PKOP2_SPEC(8,uint8_t,plus,paddb)
MMX_PKOP2_SPEC(8,uint8_t,minus,psubb)
MMX_PKOP2_SPEC(8,uint8_t,bitwise_and,pand)
MMX_PKOP2_SPEC(8,uint8_t,bitwise_or,por)
MMX_PKOP2_SPEC(8,uint8_t,bitwise_xor,pxor)
MMX_PKOP2_SPEC(8,uint8_t,fpadds,paddusb)
MMX_PKOP2_SPEC(8,uint8_t,fpsubs,psubusb)

MMX_PASSIGN_SPEC(8,int8_t)
MMX_PKOP2_SPEC(8,int8_t,plus,paddb)
MMX_PKOP2_SPEC(8,int8_t,minus,psubb)
MMX_PKOP2_SPEC(8,int8_t,bitwise_and,pand)
MMX_PKOP2_SPEC(8,int8_t,bitwise_or,por)
MMX_PKOP2_SPEC(8,int8_t,bitwise_xor,pxor)
MMX_PKOP2_SPEC(8,int8_t,fpadds,paddsb)
MMX_PKOP2_SPEC(8,int8_t,fpsubs,psubsb)

MMX_PASSIGN_SPEC(4,uint16_t)
MMX_PKOP2_SPEC(4,uint16_t,plus,paddw)
MMX_PKOP2_SPEC(4,uint16_t,minus,psubw)
MMX_PKOP2_SPEC(4,uint16_t,bitwise_and,pand)
MMX_PKOP2_SPEC(4,uint16_t,bitwise_or,por)
MMX_PKOP2_SPEC(4,uint16_t,bitwise_xor,pxor)
/// \todo psllw does not work like other operations, it uses the first element for shift count.
//MMX_PKOP2_SPEC(4,uint16_t,fpshl,psllw)
//MMX_PKOP2_SPEC(4,uint16_t,fpshr,psrlw)
MMX_PKOP2_SPEC(4,uint16_t,fpadds,paddusw)
MMX_PKOP2_SPEC(4,uint16_t,fpsubs,psubusw)

MMX_PASSIGN_SPEC(4,int16_t)
MMX_PKOP2_SPEC(4,int16_t,plus,paddw)
MMX_PKOP2_SPEC(4,int16_t,minus,psubw)
MMX_PKOP2_SPEC(4,int16_t,bitwise_and,pand)
MMX_PKOP2_SPEC(4,int16_t,bitwise_or,por)
MMX_PKOP2_SPEC(4,int16_t,bitwise_xor,pxor)
//MMX_PKOP2_SPEC(4,int16_t,fpshl,psllw)
//MMX_PKOP2_SPEC(4,int16_t,fpshr,psrlw)
MMX_PKOP2_SPEC(4,int16_t,fpadds,paddsw)
MMX_PKOP2_SPEC(4,int16_t,fpsubs,psubsw)

MMX_PASSIGN_SPEC(2,uint32_t)
MMX_PKOP2_SPEC(2,uint32_t,plus,paddd)
MMX_PKOP2_SPEC(2,uint32_t,minus,psubd)
MMX_PKOP2_SPEC(2,uint32_t,bitwise_and,pand)
MMX_PKOP2_SPEC(2,uint32_t,bitwise_or,por)
MMX_PKOP2_SPEC(2,uint32_t,bitwise_xor,pxor)
//MMX_PKOP2_SPEC(2,uint32_t,fpshl,pslld)
//MMX_PKOP2_SPEC(2,uint32_t,fpshr,psrld)

MMX_PASSIGN_SPEC(2,int32_t)
MMX_PKOP2_SPEC(2,int32_t,plus,paddd)
MMX_PKOP2_SPEC(2,int32_t,minus,psubd)
MMX_PKOP2_SPEC(2,int32_t,bitwise_and,pand)
MMX_PKOP2_SPEC(2,int32_t,bitwise_or,por)
MMX_PKOP2_SPEC(2,int32_t,bitwise_xor,pxor)
//MMX_PKOP2_SPEC(2,int32_t,fpshl,pslld)
//MMX_PKOP2_SPEC(2,int32_t,fpshr,psrld)

MMX_DBL_PKOP2_SPEC(4,uint32_t,plus,paddd)
MMX_DBL_PKOP2_SPEC(4,uint32_t,minus,psubd)
MMX_DBL_PKOP2_SPEC(4,uint32_t,bitwise_and,pand)
MMX_DBL_PKOP2_SPEC(4,uint32_t,bitwise_or,por)
MMX_DBL_PKOP2_SPEC(4,uint32_t,bitwise_xor,pxor)
//MMX_DBL_PKOP2_SPEC(2,uint32_t,fpshl,pslld)
//MMX_DBL_PKOP2_SPEC(2,uint32_t,fpshr,psrld)

MMX_DBL_PKOP2_SPEC(4,int32_t,plus,paddd)
MMX_DBL_PKOP2_SPEC(4,int32_t,minus,psubd)
MMX_DBL_PKOP2_SPEC(4,int32_t,bitwise_and,pand)
MMX_DBL_PKOP2_SPEC(4,int32_t,bitwise_or,por)
MMX_DBL_PKOP2_SPEC(4,int32_t,bitwise_xor,pxor)
//MMX_DBL_PKOP2_SPEC(2,int32_t,fpshl,pslld)
//MMX_DBL_PKOP2_SPEC(2,int32_t,fpshr,psrld)

#if CPU_HAS_SSE || CPU_HAS_3DNOW
MMX_PKOP2_SPEC(8,uint8_t,fpavg,pavgb)
MMX_PKOP2_SPEC(8,int8_t,fpavg,pavgb)
MMX_PKOP2_SPEC(4,uint16_t,fpavg,pavgw)
MMX_PKOP2_SPEC(4,int16_t,fpavg,pavgw)
MMX_PKOP2_SPEC(8,uint8_t,fpmin,pminub)
MMX_PKOP2_SPEC(8,uint8_t,fpmax,pmaxub)
MMX_PKOP2_SPEC(4,int16_t,fpmax,pmaxsw)
MMX_PKOP2_SPEC(4,int16_t,fpmin,pminsw)
#endif // CPU_HAS_SSE || CPU_HAS_3DNOW

#if CPU_HAS_3DNOW
MMX_PASSIGN_SPEC(2,float)
MMX_PKOP2_SPEC(2,float,plus,pfadd)
MMX_PKOP2_SPEC(2,float,minus,pfsub)
MMX_PKOP2_SPEC(2,float,multiplies,pfmul)
MMX_PKOP2_SPEC(2,float,fpmin,pfmin)
MMX_PKOP2_SPEC(2,float,fpmax,pfmax)
#ifndef CPU_HAS_SSE
MMX_DBL_PKOP2_SPEC(4,float,plus,pfadd)
MMX_DBL_PKOP2_SPEC(4,float,minus,pfsub)
MMX_DBL_PKOP2_SPEC(4,float,multiplies,pfmul)
MMX_DBL_PKOP2_SPEC(4,float,fpmin,pfmin)
MMX_DBL_PKOP2_SPEC(4,float,fpmax,pfmax)
#endif
#endif // CPU_HAS_3DNOW

MMX_IPASSIGN_SPEC(8,uint8_t)
MMX_IPASSIGN_SPEC(4,uint16_t)
MMX_IPASSIGN_SPEC(2,uint32_t)
MMX_IPASSIGN_SPEC(2,float)

#ifndef CPU_HAS_SSE
MMX_DBL_PASSIGN_SPEC(4,float)
MMX_DBL_PASSIGN_SPEC(4,uint32_t)
MMX_DBL_PASSIGN_SPEC(4,int32_t)
MMX_DBL_IPASSIGN_SPEC(4,float)
MMX_DBL_IPASSIGN_SPEC(4,uint32_t)
MMX_DBL_IPASSIGN_SPEC(4,int32_t)
#endif

#undef MMX_IPASSIGN_SPEC
#undef MMX_PASSIGN_SPEC
#undef MMX_PKOP2_SPEC
#undef STD_MMX_ARGS
#endif // CPU_HAS_MMX

#if CPU_HAS_SSE
#define STD_SSE_ARGS	: "m"(oout[0]), "m"(oin[0]) : "xmm0", "memory"
#define SSE_PKOP2_SPEC(n,type,optype,instruction)	\
SIMD_PKOP2_SPEC(n,type,optype)		\
{ asm ("movups %0, %%xmm0\n\tmovups %1, %%xmm1\n\t" #instruction " %%xmm1, %%xmm0\n\tmovups %%xmm0, %0" : STD_SSE_ARGS);}
#define SSE_PASSIGN_SPEC(n,type)			\
SIMD_PASSIGN_SPEC(n,type)		\
{ asm ("movups %1, %%xmm0\n\tmovups %%xmm0, %0" : STD_SSE_ARGS);}
#define SSE_IPASSIGN_SPEC(n,type)	\
SIMD_IPASSIGN_SPEC(n,type)		\
{ asm ("movups %1, %%xmm0\n\tmovups %%xmm0, %0" : STD_SSE_ARGS);}
SSE_PASSIGN_SPEC(4,float)
SSE_PASSIGN_SPEC(4,int32_t)
SSE_PASSIGN_SPEC(4,uint32_t)
SSE_PKOP2_SPEC(4,float,plus,addps)
SSE_PKOP2_SPEC(4,float,minus,subps)
SSE_PKOP2_SPEC(4,float,multiplies,mulps)
SSE_PKOP2_SPEC(4,float,divides,divps)
SSE_PKOP2_SPEC(4,float,bitwise_and,andps)
SSE_PKOP2_SPEC(4,float,bitwise_or,orps)
SSE_PKOP2_SPEC(4,float,bitwise_xor,xorps)
SSE_PKOP2_SPEC(4,float,fpmax,maxps)
SSE_PKOP2_SPEC(4,float,fpmin,minps)

SIMD_CONVERT_SPEC(4,float,int32_t,fround) {
    asm ("cvtps2pi %2, %%mm0\n\t"
	 "cvtps2pi %3, %%mm1\n\t"
	 "movq %%mm0, %0\n\t"
	 "movq %%mm1, %1"
	 : DBL_MMX_ARGS);
    reset_mmx();
}
SIMD_CONVERT_SPEC(4,int32_t,float,fround) {
    asm ("cvtpi2ps %2, %%xmm0\n\t"
	 "shufps $0x4E,%%xmm0,%%xmm0\n\t"
	 "cvtpi2ps %1, %%xmm0\n\t"
	 "movups %%xmm0, %0"
	 :: "m"(oout[0]), "m"(oin[0]), "m"(oin[2]) : "xmm0", "memory");
}
template <> inline int32_t fround<float,int32_t>::operator()(const float& a) const {
    int32_t rv;
    asm ("movss %1, %%xmm0\n\t"
	 "cvtss2si %%xmm0, %0"
	 : "=r"(rv) : "m"(a) : "xmm0" );
    return rv;
}
template <> inline uint32_t fround<float,uint32_t>::operator()(const float& a) const {
    uint32_t rv;
    asm ("movss %1, %%xmm0\n\t"
	 "cvtss2si %%xmm0, %0"
	 : "=r"(rv) : "m"(a) : "xmm0" );
    return rv;
}

SSE_IPASSIGN_SPEC(4,float)
SSE_IPASSIGN_SPEC(4,int32_t)
SSE_IPASSIGN_SPEC(4,uint32_t)

#undef SSE_IPASSIGN_SPEC
#undef SSE_PASSIGN_SPEC
#undef SSE_PKOP2_SPEC
#undef STD_SSE_ARGS
#endif // CPU_HAS_SSE

#undef SIMDA_RI
#undef SIMDA_RO
#undef SIMDA_WI
#undef SIMDA_WO
#undef SIMD_PACKEDOP_SPEC

} // namespace simd
} // namespace ustl

namespace ustl {

template <size_t N, typename T>
template <typename T2>
inline tuple<N,T>::tuple (const tuple<N,T2>& t)
    { simd::pconvert (t, *this, simd::fcast<T2,T>()); }

template <size_t N, typename T>
inline tuple<N,T>::tuple (const tuple<N,T>& t)
    { simd::passign (t, *this); }

template <size_t N, typename T>
inline tuple<N,T>::tuple (const_pointer v)
    { simd::ipassign (v, *this); }

template <size_t N, typename T>
inline tuple<N,T>::tuple (void)
    { fill_n (_v, N, T()); }

template <size_t N, typename T>
inline tuple<N,T>::tuple (const_reference v0)
    { fill_n (_v, N, v0); }

template <size_t N, typename T>
inline tuple<N,T>::tuple (const_reference v0, const_reference v1)
{
    _v[0] = v0;
    fill_n (_v+1, N-1, v1);
}

template <size_t N, typename T>
inline tuple<N,T>::tuple (const_reference v0, const_reference v1, const_reference v2)
{
    _v[0] = v0;
    _v[1] = v1;
    fill_n (_v+2, N-2, v2);
}

template <size_t N, typename T>
inline tuple<N,T>::tuple (const_reference v0, const_reference v1, const_reference v2, const_reference v3)
{
    _v[0] = v0;
    _v[1] = v1;
    _v[2] = v2;
    fill_n (_v+3, N-3, v3);
}

#if HAVE_CPP11
template <size_t N, typename T>
inline tuple<N,T>& tuple<N,T>::assign (initlist_t v)
{
    const size_t isz = min (v.size(), N);
    copy_n (v.begin(), isz, begin());
    fill_n (begin()+isz, N-isz, T());
    return *this;
}
#endif

template <size_t N, typename T>
template <typename T2>
inline tuple<N,T>& tuple<N,T>::operator= (const tuple<N,T2>& src)
{ simd::pconvert (src, *this, simd::fcast<T2,T>()); return *this; }

template <size_t N, typename T>
inline tuple<N,T>& tuple<N,T>::operator= (const tuple<N,T>& src)
{ simd::passign (src, *this); return *this; }

template <size_t N, typename T1, typename T2>
inline bool operator== (const tuple<N,T1>& t1, const tuple<N,T2>& t2)
{
    for (uoff_t i = 0; i < N; ++ i)
	if (t1[i] != t2[i])
	    return false;
    return true;
}

template <size_t N, typename T1, typename T2>
inline bool operator< (const tuple<N,T1>& t1, const tuple<N,T2>& t2)
{
    for (uoff_t i = 0; i < N && t1[i] <= t2[i]; ++ i)
	if (t1[i] < t2[i])
	    return true;
    return false;
}

template <size_t N, typename T1, typename T2>
inline tuple<N,T1>& operator+= (tuple<N,T1>& t1, const tuple<N,T2>& t2)
    { for (uoff_t i = 0; i < N; ++ i) t1[i] = T1(t1[i] + t2[i]); return t1; }

template <size_t N, typename T1, typename T2>
inline tuple<N,T1>& operator-= (tuple<N,T1>& t1, const tuple<N,T2>& t2)
    { for (uoff_t i = 0; i < N; ++ i) t1[i] = T1(t1[i] - t2[i]); return t1; }

template <size_t N, typename T1, typename T2>
inline tuple<N,T1>& operator*= (tuple<N,T1>& t1, const tuple<N,T2>& t2)
    { for (uoff_t i = 0; i < N; ++ i) t1[i] = T1(t1[i] * t2[i]); return t1; }

template <size_t N, typename T1, typename T2>
inline tuple<N,T1>& operator/= (tuple<N,T1>& t1, const tuple<N,T2>& t2)
    { for (uoff_t i = 0; i < N; ++ i) t1[i] = T1(t1[i] / t2[i]); return t1; }

template <size_t N, typename T1, typename T2>
inline tuple<N,T1> operator+ (const tuple<N,T1>& t1, const tuple<N,T2>& t2)
{
    tuple<N,T1> result;
    for (uoff_t i = 0; i < N; ++ i) result[i] = T1(t1[i] + t2[i]);
    return result;
}

template <size_t N, typename T1, typename T2>
inline tuple<N,T1> operator- (const tuple<N,T1>& t1, const tuple<N,T2>& t2)
{
    tuple<N,T1> result;
    for (uoff_t i = 0; i < N; ++ i) result[i] = T1(t1[i] - t2[i]);
    return result;
}

template <size_t N, typename T1, typename T2>
inline tuple<N,T1> operator* (const tuple<N,T1>& t1, const tuple<N,T2>& t2)
{
    tuple<N,T1> result;
    for (uoff_t i = 0; i < N; ++ i) result[i] = T1(t1[i] * t2[i]);
    return result;
}

template <size_t N, typename T1, typename T2>
inline tuple<N,T1> operator/ (const tuple<N,T1>& t1, const tuple<N,T2>& t2)
{
    tuple<N,T1> result;
    for (uoff_t i = 0; i < N; ++ i) result[i] = T1(t1[i] / t2[i]);
    return result;
}

//----------------------------------------------------------------------
// Define SIMD specializations for member functions.

#if CPU_HAS_SSE
#define SSE_TUPLE_SPECS(n,type)							\
template <> inline tuple<n,type>::tuple (void)					\
{ asm("xorps %%xmm0, %%xmm0\n\tmovups %%xmm0, %0":"+m"(_v[0])::"xmm0","memory"); } \
template<> inline void tuple<n,type>::swap (tuple<n,type>& v)			\
{										\
    asm ("movups %0,%%xmm0\n\tmovups %1,%%xmm1\n\t"				\
	"movups %%xmm0,%1\n\tmovups %%xmm1,%0"					\
	: "+m"(_v[0]), "+m"(v._v[0]) :: "xmm0","xmm1","memory");		\
}
SSE_TUPLE_SPECS(4,float)
SSE_TUPLE_SPECS(4,int32_t)
SSE_TUPLE_SPECS(4,uint32_t)
#undef SSE_TUPLE_SPECS
#endif
#if SIZE_OF_LONG == 8 && __GNUC__
#define LONG_TUPLE_SPECS(n,type)		\
template <> inline tuple<n,type>::tuple (void)	\
{ asm("":"+m"(_v[0])::"memory");		\
  *noalias_cast<long*>(_v) = 0; }				\
template<> inline void tuple<n,type>::swap (tuple<n,type>& v)	\
{ asm("":"+m"(_v[0]),"+m"(v._v[0])::"memory");			\
  iter_swap (noalias_cast<long*>(_v), noalias_cast<long*>(v._v));	\
  asm("":"+m"(_v[0]),"+m"(v._v[0])::"memory");			\
}
LONG_TUPLE_SPECS(2,float)
LONG_TUPLE_SPECS(4,int16_t)
LONG_TUPLE_SPECS(4,uint16_t)
LONG_TUPLE_SPECS(2,int32_t)
LONG_TUPLE_SPECS(2,uint32_t)
LONG_TUPLE_SPECS(8,int8_t)
LONG_TUPLE_SPECS(8,uint8_t)
#undef LONG_TUPLE_SPECS
#elif CPU_HAS_MMX
#define MMX_TUPLE_SPECS(n,type)		\
template <> inline tuple<n,type>::tuple (void)	\
{  asm ("pxor %%mm0, %%mm0\n\tmovq %%mm0, %0"	\
	:"=m"(_v[0])::"mm0","st","memory"); simd::reset_mmx(); }	\
template<> inline void tuple<n,type>::swap (tuple<n,type>& v)		\
{  asm ("movq %2,%%mm0\n\tmovq %3,%%mm1\n\t"				\
	"movq %%mm0,%1\n\tmovq %%mm1,%0"				\
	:"=m"(_v[0]),"=m"(v._v[0]):"m"(_v[0]),"m"(v._v[0]):"mm0","mm1","st","st(1)","memory"); \
   simd::reset_mmx();							\
}
MMX_TUPLE_SPECS(2,float)
MMX_TUPLE_SPECS(4,int16_t)
MMX_TUPLE_SPECS(4,uint16_t)
MMX_TUPLE_SPECS(2,int32_t)
MMX_TUPLE_SPECS(2,uint32_t)
MMX_TUPLE_SPECS(8,int8_t)
MMX_TUPLE_SPECS(8,uint8_t)
#undef MMX_TUPLE_SPECS
#endif

#if __i386__ || __x86_64__
#define UINT32_TUPLE_SPECS(type,otype)		\
template <> inline tuple<2,type>::tuple (void)	\
{ asm("":"+m"(_v[0]),"+m"(_v[1])::"memory");	\
  *noalias_cast<uint32_t*>(_v) = 0;		\
  asm("":"+m"(_v[0]),"+m"(_v[1])::"memory"); }\
template <> inline tuple<2,type>& tuple<2,type>::operator= (const tuple<2,type>& v)\
{ asm ("mov %3, %0"							\
       :"=m"(*noalias_cast<uint32_t*>(_v)),"=m"(_v[0]),"=m"(_v[1])	\
       :"r"(*noalias_cast<const uint32_t*>(v.begin())),"m"(v[0]),"m"(v[1]):"memory");	\
  return *this; }							\
template <> template <>							\
inline tuple<2,type>& tuple<2,type>::operator= (const tuple<2,otype>& v)\
{ asm ("mov %3, %0"							\
       :"=m"(*noalias_cast<uint32_t*>(_v)),"=m"(_v[0]),"=m"(_v[1])	\
       :"r"(*noalias_cast<const uint32_t*>(v.begin())),"m"(v[0]),"m"(v[1]):"memory");	\
  return *this; }							\
template <> inline tuple<2,type>::tuple (const tuple<2,type>& v)	\
{ operator= (v); }							\
template <> template <>							\
inline tuple<2,type>::tuple (const tuple<2,otype>& v)			\
{ operator= (v); }							\
template<> inline void tuple<2,type>::swap (tuple<2,type>& v)		\
{ asm(""::"m"(_v[0]),"m"(_v[1]),"m"(v._v[0]),"m"(v._v[1]):"memory");\
  iter_swap (noalias_cast<uint32_t*>(_v), noalias_cast<uint32_t*>(v._v));			\
  asm("":"=m"(_v[0]),"=m"(_v[1]),"=m"(v._v[0]),"=m"(v._v[1])::"memory"); }			\
template <> inline tuple<2,type>& operator+= (tuple<2,type>& t1, const tuple<2,type>& t2)	\
    { t1[0] += t2[0]; t1[1] += t2[1]; return t1; }						\
template <> inline tuple<2,type>& operator-= (tuple<2,type>& t1, const tuple<2,type>& t2)	\
    { t1[0] -= t2[0]; t1[1] -= t2[1]; return t1; }						\
template <> inline tuple<2,type> operator+ (const tuple<2,type>& t1, const tuple<2,type>& t2)	\
    { return tuple<2,type> (t1[0] + t2[0], t1[1] + t2[1]); }					\
template <> inline tuple<2,type> operator- (const tuple<2,type>& t1, const tuple<2,type>& t2)	\
    { return tuple<2,type> (t1[0] - t2[0], t1[1] - t2[1]); }
UINT32_TUPLE_SPECS(int16_t,uint16_t)
UINT32_TUPLE_SPECS(uint16_t,int16_t)
#undef UINT32_TUPLE_SPECS
#endif

#undef TUPLEV_R1
#undef TUPLEV_R2
#undef TUPLEV_W1
#undef TUPLEV_W2

#define SIMD_TUPLE_PACKOP(N,T)	\
template <> inline tuple<N,T>& operator+= (tuple<N,T>& t1, const tuple<N,T>& t2)	\
    { simd::padd (t2, t1); return t1; }						\
template <> inline tuple<N,T>& operator-= (tuple<N,T>& t1, const tuple<N,T>& t2)	\
    { simd::psub (t2, t1); return t1; }						\
template <> inline tuple<N,T>& operator*= (tuple<N,T>& t1, const tuple<N,T>& t2)	\
    { simd::pmul (t2, t1); return t1; }						\
template <> inline tuple<N,T>& operator/= (tuple<N,T>& t1, const tuple<N,T>& t2)	\
    { simd::pdiv (t2, t1); return t1; }						\
template <> inline tuple<N,T> operator+ (const tuple<N,T>& t1, const tuple<N,T>& t2)	\
    { tuple<N,T> result (t1); simd::padd (t2, result); return result; }		\
template <> inline tuple<N,T> operator- (const tuple<N,T>& t1, const tuple<N,T>& t2)	\
    { tuple<N,T> result (t1); simd::psub (t2, result); return result; }		\
template <> inline tuple<N,T> operator* (const tuple<N,T>& t1, const tuple<N,T>& t2)	\
    { tuple<N,T> result (t1); simd::pmul (t2, result); return result; }		\
template <> inline tuple<N,T> operator/ (const tuple<N,T>& t1, const tuple<N,T>& t2)	\
    { tuple<N,T> result (t1); simd::pdiv (t2, result); return result; }
SIMD_TUPLE_PACKOP(4,float)
SIMD_TUPLE_PACKOP(2,float)
SIMD_TUPLE_PACKOP(2,double)
SIMD_TUPLE_PACKOP(4,int32_t)
SIMD_TUPLE_PACKOP(4,uint32_t)
SIMD_TUPLE_PACKOP(4,int16_t)
SIMD_TUPLE_PACKOP(4,uint16_t)
SIMD_TUPLE_PACKOP(2,int32_t)
SIMD_TUPLE_PACKOP(2,uint32_t)
SIMD_TUPLE_PACKOP(8,int8_t)
SIMD_TUPLE_PACKOP(8,uint8_t)
#undef SIMD_TUPLE_PACKOP

} // namespace ustl

namespace ustl {

/// \class matrix umatrix.h ustl.h
/// \ingroup Sequences
///
/// \brief A two-dimensional array of NX*NY elements of type T.
///
template <size_t NX, size_t NY, typename T>
class matrix : public tuple<NX*NY,T> {
public:
    typedef tuple<NX,T>					row_type;
    typedef tuple<NY,T>					column_type;
    typedef tuple<NX*NY,T>				tuple_type;
    typedef typename tuple_type::value_type		value_type;
    typedef typename tuple_type::size_type		size_type;
    typedef typename tuple_type::pointer		pointer;
    typedef typename tuple_type::const_pointer		const_pointer;
    typedef typename tuple_type::reference		reference;
    typedef typename tuple_type::const_reference	const_reference;
    typedef typename tuple_type::iterator		iterator;
    typedef typename tuple_type::const_iterator		const_iterator;
    typedef typename tuple_type::range_t		range_t;
    typedef typename tuple_type::const_range_t		const_range_t;
    typedef typename tuple_type::reverse_iterator	reverse_iterator;
    typedef typename tuple_type::const_reverse_iterator	const_reverse_iterator;
public:
    inline			matrix (void)			{ }
    inline size_type		columns (void) const		{ return NX; }
    inline size_type		rows (void) const		{ return NY; }
    inline const_iterator	at (size_type i) const		{ return matrix::begin() + i * NX; }
    inline iterator		at (size_type i)		{ return matrix::begin() + i * NX; }
    inline const_iterator	operator[] (size_type i) const	{ return at (i); }
    inline iterator		operator[] (size_type i)	{ return at (i); }
    inline row_type		row (size_type r) const		{ return row_type (at (r)); }
    inline column_type		column (size_type c) const;
    template <typename T2>
    inline const matrix&	operator= (const matrix<NX,NY,T2>& src)	{ tuple_type::operator= (src); return *this; }
    inline const matrix&	operator= (const matrix<NX,NY,T>& src)	{ tuple_type::operator= (src); return *this; }
    inline const matrix&	operator+= (const_reference v)		{ tuple_type::operator+= (v); return *this; }
    inline const matrix&	operator-= (const_reference v)		{ tuple_type::operator-= (v); return *this; }
    inline const matrix&	operator*= (const_reference v)		{ tuple_type::operator*= (v); return *this; }
    inline const matrix&	operator/= (const_reference v)		{ tuple_type::operator/= (v); return *this; }
    inline const matrix		operator+ (const_reference v) const
				    { matrix result (*this); result += v; return result; }
    inline const matrix		operator- (const_reference v) const
				    { matrix result (*this); result -= v; return result; }
    inline const matrix		operator* (const_reference v) const
				    { matrix result (*this); result *= v; return result; }
    inline const matrix		operator/ (const_reference v) const
				    { matrix result (*this); result /= v; return result; }
};

template <size_t NX, size_t NY, typename T>
inline typename matrix<NX,NY,T>::column_type matrix<NX,NY,T>::column (size_type c) const
{
    column_type result;
    const_iterator src (matrix::begin() + c);
    iterator dest (result.begin());
    for (uoff_t i = 0; i < NY; ++ i, ++ dest, src += NX)
	*dest = *src;
    return result;
}

//----------------------------------------------------------------------
// Define SIMD specializations for member functions.

#if CPU_HAS_SSE
#define MATRIX_R(v)		"m"(v[0]),"m"(v[4]),"m"(v[8]),"m"(v[12])
#define MATRIX_W(v)		"=m"(v[0]),"=m"(v[4]),"=m"(v[8]),"=m"(v[12])
#define SSE_TUPLE_SPECS(n,type)				\
template <> inline tuple<n,type>::tuple (void)		\
{   asm volatile ("xorps %%xmm0, %%xmm0\n\t"		\
	"movups %%xmm0, %0\n\t"				\
	"movups %%xmm0, %1\n\t"				\
	"movups %%xmm0, %2\n\t"				\
	"movups %%xmm0, %3"				\
	: "=m"(_v[0]),"=m"(_v[4]),"=m"(_v[8]),"=m"(_v[12])	\
	::"xmm0","memory");				\
}							\
namespace simd {					\
SIMD_PASSIGN_SPEC(n,type)				\
{  							\
    asm volatile ("movups %2, %%xmm0\n\t"		\
	"movups %3, %%xmm1\n\t"				\
	"movups %%xmm0, %0\n\t"				\
	"movups %%xmm1, %1"				\
	: "=m"(oout[0]),"=m"(oout[4])			\
	: "m"(oin[0]),"m"(oin[4])			\
	: "xmm0", "xmm1", "memory");			\
    asm volatile ("movups %2, %%xmm0\n\t"		\
	"movups %3, %%xmm1\n\t"				\
	"movups %%xmm0, %0\n\t"				\
	"movups %%xmm1, %1"				\
	: "=m"(oout[8]),"=m"(oout[12])			\
	: "m"(oin[8]),"m"(oin[12])			\
	: "xmm0", "xmm1", "memory");			\
}							\
}
SSE_TUPLE_SPECS(16,float)
SSE_TUPLE_SPECS(16,int32_t)
SSE_TUPLE_SPECS(16,uint32_t)
#undef SSE_TUPLE_SPECS
#undef TOUCH_MATRIX_R
#undef TOUCH_MATRIX_W
#undef MATRIX_R
#undef MATRIX_W
#endif

} // namespace ustl

namespace ustl {

/// \brief Creates an identity matrix in \p m
/// \ingroup NumericAlgorithms
template <size_t NX, size_t NY, typename T>
void load_identity (matrix<NX,NY,T>& m)
{
    fill_n (m.begin(), NX * NY, 0);
    for (typename matrix<NX,NY,T>::iterator i = m.begin(); i < m.end(); i += NX + 1)
	*i = 1;
}

/// \brief Multiplies two matrices
/// \ingroup NumericAlgorithms
template <size_t NX, size_t NY, typename T>
matrix<NY,NY,T> operator* (const matrix<NX,NY,T>& m1, const matrix<NY,NX,T>& m2)
{
    matrix<NY,NY,T> mr;
    for (uoff_t ry = 0; ry < NY; ++ ry) {
	for (uoff_t rx = 0; rx < NY; ++ rx) {
	    T dpv (0);
	    for (uoff_t x = 0; x < NX; ++ x)
		dpv += m1[ry][x] * m2[x][rx];
	    mr[ry][rx] = dpv;
	}
    }
    return mr;
}

/// \brief Transforms vector \p t with matrix \p m
/// \ingroup NumericAlgorithms
template <size_t NX, size_t NY, typename T>
tuple<NX,T> operator* (const tuple<NY,T>& t, const matrix<NX,NY,T>& m)
{
    tuple<NX,T> tr;
    for (uoff_t x = 0; x < NX; ++ x) {
	T dpv (0);
	for (uoff_t y = 0; y < NY; ++ y)
	    dpv += t[y] * m[y][x];
	tr[x] = dpv;
    }
    return tr;
}

/// \brief Transposes (exchanges rows and columns) matrix \p m.
/// \ingroup NumericAlgorithms
template <size_t N, typename T>
void transpose (matrix<N,N,T>& m)
{
    for (uoff_t x = 0; x < N; ++ x)
	for (uoff_t y = x; y < N; ++ y)
	    swap (m[x][y], m[y][x]);
}

#if WANT_UNROLLED_COPY

#if CPU_HAS_SSE

#if linux // Non-linux gcc versions (BSD, Solaris) can't handle "x" constraint and provide no alternative.
template <>
inline void load_identity (matrix<4,4,float>& m)
{
    asm (
	"movaps %4, %%xmm1		\n\t"	// 1 0 0 0
	"movups %4, %0			\n\t"	// 1 0 0 0
	"shufps $0xB1,%%xmm1,%%xmm1	\n\t"	// 0 1 0 0
	"movups %%xmm1, %1		\n\t"	// 0 1 0 0
	"shufps $0x4F,%4,%%xmm1		\n\t"	// 0 0 1 0
	"shufps $0x1B,%4,%4		\n\t"	// 0 0 0 1
	"movups %%xmm1, %2		\n\t"	// 0 0 1 0
	"movups %4, %3"				// 0 0 0 1
	: "=m"(m[0][0]), "=m"(m[1][0]), "=m"(m[2][0]), "=m"(m[3][0])
	: "x"(1.0f)
	: "xmm1", "memory"
    );
    asm ("":::"memory");
}
#endif

inline void _sse_load_matrix (const float* m)
{
    asm (
	"movups %0, %%xmm4	\n\t"	// xmm4 = m[1 2 3 4]
	"movups %1, %%xmm5	\n\t"	// xmm5 = m[1 2 3 4]
	"movups %2, %%xmm6	\n\t"	// xmm6 = m[1 2 3 4]
	"movups %3, %%xmm7"		// xmm7 = m[1 2 3 4]
	: : "m"(m[0]), "m"(m[4]), "m"(m[8]), "m"(m[12])
	: "xmm4", "xmm5", "xmm6", "xmm7", "memory"
    );
}

inline void _sse_transform_to_vector (float* result)
{
    asm (
	"movaps %%xmm0, %%xmm1		\n\t" // xmm1 = t[0 1 2 3]
	"movaps %%xmm0, %%xmm2		\n\t" // xmm1 = t[0 1 2 3]
	"movaps %%xmm0, %%xmm3		\n\t" // xmm1 = t[0 1 2 3]
	"shufps $0x00, %%xmm0, %%xmm0	\n\t" // xmm0 = t[0 0 0 0]
	"shufps $0x66, %%xmm1, %%xmm1	\n\t" // xmm1 = t[1 1 1 1]
	"shufps $0xAA, %%xmm2, %%xmm2	\n\t" // xmm2 = t[2 2 2 2]
	"shufps $0xFF, %%xmm3, %%xmm3	\n\t" // xmm3 = t[3 3 3 3]
	"mulps  %%xmm4, %%xmm0		\n\t" // xmm0 = t[0 0 0 0] * m[0 1 2 3]
	"mulps  %%xmm5, %%xmm1		\n\t" // xmm1 = t[1 1 1 1] * m[0 1 2 3]
	"addps  %%xmm1, %%xmm0		\n\t" // xmm0 = xmm0 + xmm1
	"mulps  %%xmm6, %%xmm2		\n\t" // xmm2 = t[2 2 2 2] * m[0 1 2 3]
	"mulps  %%xmm7, %%xmm3		\n\t" // xmm3 = t[3 3 3 3] * m[0 1 2 3]
	"addps  %%xmm3, %%xmm2		\n\t" // xmm2 = xmm2 + xmm3
	"addps  %%xmm2, %%xmm0		\n\t" // xmm0 = result
	"movups %%xmm0, %0"
	: "=m"(result[0]), "=m"(result[1]), "=m"(result[2]), "=m"(result[3]) :
	: "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7", "memory"
    );
}

template <>
inline tuple<4,float> operator* (const tuple<4,float>& t, const matrix<4,4,float>& m)
{
    tuple<4,float> result;
    _sse_load_matrix (m.begin());
    asm ("movups %0, %%xmm0" : : "m"(t[0]), "m"(t[1]), "m"(t[2]), "m"(t[3]) : "xmm0", "memory");
    _sse_transform_to_vector (result.begin());
    return result;
}

template <>
inline matrix<4,4,float> operator* (const matrix<4,4,float>& m1, const matrix<4,4,float>& m2)
{
    matrix<4,4,float> result;
    _sse_load_matrix (m2.begin());
    for (uoff_t r = 0; r < 4; ++ r) { 
	asm ("movups %0, %%xmm0" : : "m"(m1[r][0]), "m"(m1[r][1]), "m"(m1[r][2]), "m"(m1[r][3]) : "xmm0", "memory");
	_sse_transform_to_vector (result[r]);
    }
    return result;
}

#elif CPU_HAS_3DNOW

/// Specialization for 4-component vector transform, the slow part of 3D graphics.
template <>
static tuple<4,float> operator* (const tuple<4,float>& t, const matrix<4,4,float>& m)
{
    tuple<4,float> result;
    // This is taken from "AMD Athlon Code Optimization Guide" from AMD. 18 cycles!
    // If you are writing a 3D engine, you may want to copy it instead of calling it
    // because of the femms instruction at the end, which takes 2 cycles.
    asm (
	"movq	   %2, %%mm0		\n\t"	//            y | x
	"movq	   %3, %%mm1		\n\t"	//            w | z
	"movq	   %%mm0, %%mm2		\n\t"	//            y | x
	"movq	   %4, %%mm3		\n\t"	//      m[0][1] | m[0][0]
	"punpckldq  %%mm0, %%mm0	\n\t"	//            x | x
	"movq	   %6, %%mm4		\n\t"	//      m[1][1] | m[1][0]
	"pfmul	   %%mm0, %%mm3		\n\t"	//    x*m[0][1] | x*m[0][0]
	"punpckhdq  %%mm2, %%mm2	\n\t"	//            y | y
	"pfmul	   %%mm2, %%mm4		\n\t"	//    y*m[1][1] | y*m[1][0]
	"movq	   %5, %%mm5		\n\t"	//      m[0][3] | m[0][2]
	"movq	   %7, %%mm7		\n\t"	//      m[1][3] | m[1][2]
	"movq	   %%mm1, %%mm6		\n\t"	//            w | z
	"pfmul	   %%mm0, %%mm5		\n\t"	//    x*m[0][3] | v0>x*m[0][2]
	"movq	   %8, %%mm0		\n\t"	//      m[2][1] | m[2][0]
	"punpckldq  %%mm1, %%mm1	\n\t"	//            z | z
	"pfmul	   %%mm2, %%mm7		\n\t"	//    y*m[1][3] | y*m[1][2]
	"movq	   %9, %%mm2		\n\t"	//      m[2][3] | m[2][2]
	"pfmul	   %%mm1, %%mm0		\n\t"	//    z*m[2][1] | z*m[2][0]
	"pfadd	   %%mm4, %%mm3		\n\t"	// x*m[0][1]+y*m[1][1] | x*m[0][0]+y*m[1][0]
	"movq	   %10, %%mm4		\n\t"	//      m[3][1] | m[3][0]
	"pfmul	   %%mm1, %%mm2		\n\t"	//    z*m[2][3] | z*m[2][2]
	"pfadd	   %%mm7, %%mm5		\n\t"	// x*m[0][3]+y*m[1][3] | x*m[0][2]+y*m[1][2]
	"movq	   %11, %%mm1		\n\t"	//      m[3][3] | m[3][2]
	"punpckhdq  %%mm6, %%mm6	\n\t"	//            w | w
	"pfadd	   %%mm0, %%mm3		\n\t"	// x*m[0][1]+y*m[1][1]+z*m[2][1] | x*m[0][0]+y*m[1][0]+z*m[2][0]
	"pfmul	   %%mm6, %%mm4		\n\t"	//    w*m[3][1] | w*m[3][0]
	"pfmul	   %%mm6, %%mm1		\n\t"	//    w*m[3][3] | w*m[3][2]
	"pfadd	   %%mm2, %%mm5		\n\t"	// x*m[0][3]+y*m[1][3]+z*m[2][3] | x*m[0][2]+y*m[1][2]+z*m[2][2]
	"pfadd	   %%mm4, %%mm3		\n\t"	// x*m[0][1]+y*m[1][1]+z*m[2][1]+w*m[3][1] | x*m[0][0]+y*m[1][0]+z*m[2][0]+w*m[3][0]
	"movq	   %%mm3, %0		\n\t"	// store result->y | result->x
	"pfadd	   %%mm1, %%mm5		\n\t"	// x*m[0][3]+y*m[1][3]+z*m[2][3]+w*m[3][3] | x*m[0][2]+y*m[1][2]+z*m[2][2]+w*m[3][2]
	"movq	   %%mm5, %1"			// store result->w | result->z
	: "=m"(result[0]), "=m"(result[2])
	: "m"(t[0]), "m"(t[2]),
	  "m"(m[0][0]), "m"(m[0][2]),
	  "m"(m[1][0]), "m"(m[1][2]),
	  "m"(m[2][0]), "m"(m[2][2]),
	  "m"(m[3][0]), "m"(m[3][2])
	: ALL_MMX_REGS_CHANGELIST, "memory"
    );
    asm ("":::"memory");
    simd::reset_mmx();
    return result;
}

#else	// If no processor extensions, just unroll the multiplication

/// Specialization for 4-component vector transform, the slow part of 3D graphics.
template <> inline tuple<4,float> operator* (const tuple<4,float>& t, const matrix<4,4,float>& m)
{
    tuple<4,float> tr;
    for (uoff_t i = 0; i < 4; ++ i)
	tr[i] = t[0] * m[0][i] + t[1] * m[1][i] + t[2] * m[2][i] + t[3] * m[3][i];
    return tr;
}

#endif	// CPU_HAS_3DNOW
#endif	// WANT_UNROLLED_COPY

} // namespace ustl

namespace ustl {

/// For partial specialization of stream_size_of for objects
template <typename T> struct object_stream_size {
    inline streamsize operator()(const T& v) const { return v.stream_size(); }
};
template <typename T> struct integral_object_stream_size {
    inline streamsize operator()(const T& v) const { return sizeof(v); }
};
/// Returns the size of the given object. Overloads for standard types are available.
template <typename T>
inline streamsize stream_size_of (const T& v) {
    typedef typename tm::Select <numeric_limits<T>::is_integral,
	integral_object_stream_size<T>, object_stream_size<T> >::Result stream_sizer_t;
    return stream_sizer_t()(v);
}

/// \brief Returns the recommended stream alignment for type \p T. Override with ALIGNOF.
/// Because this is occasionally called with a null value, do not access the argument!
template <typename T>
inline size_t stream_align_of (const T&)
{
    if (numeric_limits<T>::is_integral)
	return __alignof__(T);
    return 4;
}

#define ALIGNOF(type,grain)	\
namespace ustl {		\
    template <> inline size_t stream_align_of (const type&) { return grain; } }

} // namespace ustl

//
// Extra overloads in this macro are needed because it is the one used for
// marshalling pointers. Passing a pointer to stream_size_of creates a
// conversion ambiguity between converting to const pointer& and converting
// to bool; the compiler always chooses the bool conversion (because it
// requires 1 conversion instead of 2 for the other choice). There is little
// point in adding the overloads to other macros, since they are never used
// for pointers.
//
/// Declares that T is to be written as is into binary streams.
#define INTEGRAL_STREAMABLE(T)	\
    namespace ustl {		\
	inline istream& operator>> (istream& is, T& v)		{ is.iread(v);  return is; }	\
	inline ostream& operator<< (ostream& os, const T& v)	{ os.iwrite(v); return os; }	\
	inline ostream& operator<< (ostream& os, T& v)		{ os.iwrite(v); return os; }	\
	template<> inline streamsize stream_size_of(const T& v)	{ return sizeof(v); }		\
    }

/// Declares that T contains read, write, and stream_size methods. This is no longer needed and is deprecated.
#define STD_STREAMABLE(T)

/// Declares \p T to be writable to text streams. This is no longer needed and is deprecated.
#define TEXT_STREAMABLE(T)

/// Declares that T is to be cast into TSUB for streaming.
#define CAST_STREAMABLE(T,TSUB)	\
    namespace ustl {		\
	inline istream& operator>> (istream& is, T& v)		{ TSUB sv; is >> sv; v = (T)(sv); return is; }	\
	inline ostream& operator<< (ostream& os, const T& v)	{ os << TSUB(v); return os; }			\
	template<> inline streamsize stream_size_of(const T& v)	{ return stream_size_of (TSUB(v)); }		\
    }

/// Placed into a class it declares the methods required by STD_STREAMABLE. Syntactic sugar.
#define DECLARE_STD_STREAMABLE				\
    public:						\
	void		read (istream& is);		\
	void		write (ostream& os) const;	\
	streamsize	stream_size (void) const

/// Specifies that \p T is printed by using it as an index into \p Names string array.
#define LOOKUP_TEXT_STREAMABLE(T,Names,nNames)	\
    namespace ustl {				\
	inline ostringstream& operator<< (ostringstream& os, const T& v) {	\
	    os << Names[min(uoff_t(v),uoff_t(nNames-1))];			\
	    return os;				\
	}					\
    }

namespace ustl {

class file_exception;

/// Defines types and constants used by all stream classes.
class ios_base {
public:
    /// Used to set parameters for stringstreams
    enum fmtflags_bits {
	boolalpha	= (1 << 0),	///< Boolean values printed as text.
	showbase	= (1 << 1),	///< Add 0x or 0 prefixes on hex and octal numbers.
	showpoint	= (1 << 2),	///< Print decimal point.
	showpos		= (1 << 3),
	skipws		= (1 << 4),	///< Skip whitespace when reading.
	unitbuf		= (1 << 5),
	uppercase	= (1 << 6),
	dec		= (1 << 7),	///< Decimal number output.
	oct		= (1 << 8),	///< Octal number output.
	hex		= (1 << 9),	///< Hexadecimal number output.
	fixed		= (1 << 10),	///< Fixed-point float output.
	scientific	= (1 << 11),	///< Scientific float format.
	left		= (1 << 12),	///< Left alignment.
	right		= (1 << 13),	///< Right alignment.
	internal	= (1 << 14),
	basefield	= dec| oct| hex,
	floatfield	= fixed| scientific,
	adjustfield	= left| right| internal
    };
    /// For file-based streams, specifies fd mode.
    enum openmode_bits {
	in	= (1 << 0),
	out	= (1 << 1),
	app	= (1 << 2),
	ate	= (1 << 3),
	binary	= (1 << 4),
	trunc	= (1 << 5),
	#ifndef DOXYGEN_SHOULD_SKIP_THIS
	nonblock= (1 << 6),
	nocreate= (1 << 7),
	noctty	= (1 << 8),
	nombits	= 9
	#endif
    };
    /// Seek directions, equivalent to SEEK_SET, SEEK_CUR, and SEEK_END.
    enum seekdir {
	beg,
	cur,
	end
    };
    /// I/O state bitmasks.
    enum iostate_bits {
	goodbit	= 0,
	badbit	= (1 << 0),
	eofbit	= (1 << 1),
	failbit	= (1 << 2),
	#ifndef DOXYGEN_SHOULD_SKIP_THIS
	nbadbits = 3,
	allbadbits = 0x7
	#endif
    };

    enum { default_stream_buffer_size = 4095 };

    typedef uint32_t		openmode;	///< Holds openmode_bits.
    typedef uint32_t		fmtflags;	///< Holds fmtflags_bits for a string stream.
    typedef uint32_t		iostate;	///< Holds iostate_bits for a file stream.
    typedef file_exception	failure;	///< Thrown by fstream on errors.

    static const char c_DefaultDelimiters [16];	///< Default word delimiters for stringstreams.
public:
    inline		ios_base (void)			: _state (goodbit), _exceptions (allbadbits) {}
    inline iostate	rdstate (void) const		{ return _state; }
    inline bool		bad (void) const		{ return rdstate() & badbit; }
    inline bool		good (void) const		{ return rdstate() == goodbit; }
    inline bool		fail (void) const		{ return rdstate() & (badbit | failbit); }
    inline bool		eof (void) const		{ return rdstate() & eofbit; }
    inline bool		operator! (void) const		{ return fail(); }
    inline		operator bool (void) const	{ return !fail(); }
    inline void		clear (iostate v = goodbit)	{ _state = v; }
    inline void		setstate (iostate v)		{ _state |= v; }
    inline iostate	exceptions (void) const		{ return _exceptions; }
    inline iostate	exceptions (iostate v)		{ return _exceptions = v; }
protected:
    inline bool		set_and_throw (iostate v)	{ setstate(v); return exceptions() & v; }
    void		overrun (const char* op, const char* type, uint32_t n, uint32_t p, uint32_t rem);
private:
    uint16_t		_state;		///< Open state, using ios::iostate_bits.
    uint16_t		_exceptions;	///< Exception flags, using ios::iostate_bits.
};

} // namespace ustl

#if WANT_STREAM_BOUNDS_CHECKING
#if WITHOUT_LIBSTDCPP

namespace __cxxabiv1 { class __class_type_info; }

namespace std {
class type_info {
public:
    inline virtual	~type_info (void)			{ }
    inline const char*	name (void) const			{ return __name[0] == '*' ? __name + 1 : __name; }
    inline bool		before (const type_info& v) const	{ return __name < v.__name; }
    inline bool		operator==(const type_info& v) const	{ return __name == v.__name; }
    inline bool		operator!=(const type_info& v) const	{ return !operator==(v); }
    virtual bool	__is_pointer_p (void) const;
    virtual bool	__is_function_p (void) const;
    virtual bool	__do_catch (const type_info* __thr_type, void** __thr_obj, unsigned __outer) const;
    virtual bool	__do_upcast (const __cxxabiv1::__class_type_info* __target, void** __obj_ptr) const;
    explicit inline	type_info (const char* newname)		: __name(newname) { }
private:
    inline void		operator= (const type_info&)		{ }
    inline		type_info (const type_info&)		{ }
protected:
    const char*		__name;
};
} // namespace std

#endif	// WITHOUT_LIBSTDCPP
#endif

namespace ustl {

class ostream;
class memlink;
class string;

/// \class istream mistream.h ustl.h
/// \ingroup BinaryStreams
///
/// \brief Helper class to read packed binary streams.
/// 
/// This class contains a set of functions to read integral types from an
/// unstructured memory block. Unpacking binary file data can be done this
/// way, for instance. aligning the data is your responsibility, and can
/// be accomplished by proper ordering of reads and by calling the align()
/// function. Unaligned access is usually slower by orders of magnitude and,
/// on some architectures, such as PowerPC, can cause your program to crash.
/// Therefore, all read functions have asserts to check alignment.
/// Overreading the end of the stream will also cause a crash (an assert in
/// debug builds). Oh, and don't be intimidated by the size of the inlines
/// here. In the assembly code the compiler will usually chop everything down
/// to five instructions each.
/// 
/// Alignment rules for your objects:
///	- Assume your writes start off 4-byte aligned.
///	- After completion, \ref istream::align the stream to at least 4.
///	- If data portability between 32bit and 64bit platforms is important
///	(it often is not, in config files and the like), ensure you are always
///	using fixed-size types and are aligning to a fixed grain. Avoid writing
///	8-byte types, and if you do, manually align before doing so.
///	- Non-default alignment is allowed if you plan to frequently write this
///	object in array form and alignment would be costly. For example, an
///	array of uint16_t-sized objects may leave the stream uint16_t aligned
///	as long as you know about it and will default-align the stream after
///	writing the array (note: \ref vector will already do this for you)
/// 
/// Example code:
/// \code
///     memblock b;
///     b.read_file ("test.file");
///     ostream is (b);
///     is >> boolVar >> ios::talign<int>();
///     is >> intVar >> floatVar;
///     is.read (binaryData, binaryDataSize);
///     is.align();
/// \endcode
///
class istream : public cmemlink, public ios_base {
public:
    inline		istream (void)				: cmemlink(), _pos (0) {}
    inline		istream (const void* p, streamsize n)	: cmemlink(p, n), _pos (0) {}
    inline explicit	istream (const cmemlink& source)	: cmemlink (source), _pos (0) {}
    explicit		istream (const ostream& source) noexcept;
    inline iterator	end (void) const			{ return cmemlink::end(); }
    inline void		link (const void* p, streamsize n)	{ cmemlink::link (p, n); }
    inline void		link (const cmemlink& l)		{ cmemlink::link (l.cdata(), l.readable_size()); }
    inline void		link (const void* f, const void* l)	{ cmemlink::link (f, l); }
    inline void		relink (const void* p, streamsize n)	{ cmemlink::relink (p, n); _pos = 0; }
    inline void		relink (const cmemlink& l)		{ relink (l.cdata(), l.readable_size()); }
    virtual void	unlink (void) noexcept override;
    virtual streamsize	underflow (streamsize = 1);
    inline uoff_t	pos (void) const	{ return _pos; }
    inline const_iterator ipos (void) const	{ return begin() + pos(); }
    inline streamsize	remaining (void) const	{ return size() - pos(); }
    inline void		seek (uoff_t newPos);
    inline void		iseek (const_iterator newPos);
    inline void		skip (streamsize nBytes);
    inline bool		aligned (streamsize grain = c_DefaultAlignment) const;
    inline bool		verify_remaining (const char* op, const char* type, streamsize n);
    inline streamsize	align_size (streamsize grain = c_DefaultAlignment) const;
    inline void		align (streamsize grain = c_DefaultAlignment);
    inline void		swap (istream& is);
    inline void		read (void* buffer, streamsize size);
    inline void		read (memlink& buf)	{ read (buf.begin(), buf.writable_size()); }
    void		read_strz (string& str);
    streamsize		readsome (void* s, streamsize n);
    inline void		read (istream&)			{ }
    void		write (ostream& os) const;
    void		text_write (ostringstream& os) const;
    inline streamsize	stream_size (void) const	{ return remaining(); }
    template <typename T>
    inline void		iread (T& v);
    inline void		ungetc (void)		{ seek (pos() - 1); }
    inline off_t	tellg (void) const	{ return pos(); }
    inline void		seekg (off_t p, seekdir d = beg);
private:
    streamoff		_pos;		///< The current read position.
};

//----------------------------------------------------------------------

template <typename T, typename Stream>
inline streamsize required_stream_size (T, const Stream&) { return 1; }
template <typename T>
inline streamsize required_stream_size (T v, const istream&) { return stream_size_of(v); }

template <typename Stream>
inline bool stream_at_eof (const Stream& stm)	{ return stm.eof(); }
template <>
inline bool stream_at_eof (const istream&)	{ return false; }

/// \class istream_iterator
/// \ingroup BinaryStreamIterators
///
/// \brief An iterator over an istream to use with uSTL algorithms.
///
template <typename T, typename Stream = istream>
class istream_iterator {
public:
    typedef T			value_type;
    typedef ptrdiff_t		difference_type;
    typedef const value_type*	pointer;
    typedef const value_type&	reference;
    typedef typename Stream::size_type	size_type;
    typedef input_iterator_tag	iterator_category;
public:
				istream_iterator (void)		: _pis (nullptr), _v() {}
    explicit			istream_iterator (Stream& is)	: _pis (&is), _v() { Read(); }
				istream_iterator (const istream_iterator& i)	: _pis (i._pis), _v (i._v) {}
    /// Reads and returns the next value.
    inline const T&		operator* (void)	{ return _v; }
    inline istream_iterator&	operator++ (void)	{ Read(); return *this; }
    inline istream_iterator&	operator-- (void)	{ _pis->seek (_pis->pos() - 2 * stream_size_of(_v)); return operator++(); }
    inline istream_iterator	operator++ (int)	{ istream_iterator old (*this); operator++(); return old; }
    inline istream_iterator	operator-- (int)	{ istream_iterator old (*this); operator--(); return old; }
    inline istream_iterator&	operator+= (streamsize n)	{ while (n--) operator++(); return *this; }
    inline istream_iterator&	operator-= (streamsize n)	{ _pis->seek (_pis->pos() - (n + 1) * stream_size_of(_v)); return operator++(); }
    inline istream_iterator	operator- (streamoff n) const			{ istream_iterator result (*this); return result -= n; }
    inline difference_type	operator- (const istream_iterator& i) const	{ return distance (i._pis->pos(), _pis->pos()) / stream_size_of(_v); }
    inline bool			operator== (const istream_iterator& i) const	{ return (!_pis && !i._pis) || (_pis && i._pis && _pis->pos() == i._pis->pos()); }
    inline bool			operator< (const istream_iterator& i) const	{ return !i._pis || (_pis && _pis->pos() < i._pis->pos()); }
private:
    void Read (void)
    {
	if (!_pis)
	    return;
	const streamsize rs (required_stream_size (_v, *_pis));
	if (_pis->remaining() < rs && _pis->underflow (rs) < rs) {
	    _pis = nullptr;
	    return;
	}
	*_pis >> _v;
	if (stream_at_eof (*_pis))
	    _pis = nullptr;
    }
private:
    Stream*	_pis;		///< The host stream.
    T		_v;		///< Last read value; cached to be returnable as a const reference.
};

//----------------------------------------------------------------------

/// Checks that \p n bytes are available in the stream, or else throws.
inline bool istream::verify_remaining (const char* op, const char* type, streamsize n)
{
    const streamsize rem = remaining();
    bool enough = n <= rem;
    if (!enough) overrun (op, type, n, pos(), rem);
    return enough;
}

/// Sets the current read position to \p newPos
inline void istream::seek (uoff_t newPos)
{
#if WANT_STREAM_BOUNDS_CHECKING
    if (newPos > size())
	return overrun ("seekg", "byte", newPos, pos(), size());
#else
    assert (newPos <= size());
#endif
    _pos = newPos;
}

/// Sets the current read position to \p newPos
inline void istream::iseek (const_iterator newPos)
{
    seek (distance (begin(), newPos));
}

/// Sets the current write position to \p p based on \p d.
inline void istream::seekg (off_t p, seekdir d)
{
    switch (d) {
	case beg:	seek (p); break;
	case cur:	seek (pos() + p); break;
	case ios_base::end:	seek (size() - p); break;
    }
}

/// Skips \p nBytes without reading them.
inline void istream::skip (streamsize nBytes)
{
    seek (pos() + nBytes);
}

/// Returns the number of bytes to skip to be aligned on \p grain.
inline streamsize istream::align_size (streamsize grain) const
{
    return Align (pos(), grain) - pos();
}

/// Returns \c true if the read position is aligned on \p grain
inline bool istream::aligned (streamsize grain) const
{
    return pos() % grain == 0;
}

/// aligns the read position on \p grain
inline void istream::align (streamsize grain)
{
    seek (Align (pos(), grain));
}

/// Reads type T from the stream via a direct pointer cast.
template <typename T>
inline void istream::iread (T& v)
{
    assert (aligned (stream_align_of (v)));
#if WANT_STREAM_BOUNDS_CHECKING
    if (!verify_remaining ("read", typeid(v).name(), sizeof(T)))
	return;
#else
    assert (remaining() >= sizeof(T));
#endif
    v = *reinterpret_cast<const T*>(ipos());
    _pos += sizeof(T);
}

/// Swaps contents with \p is
inline void istream::swap (istream& is)
{
    cmemlink::swap (is);
    ::ustl::swap (_pos, is._pos);
}

/// Reads \p n bytes into \p buffer.
inline void istream::read (void* buffer, size_type n)
{
#if WANT_STREAM_BOUNDS_CHECKING
    if (!verify_remaining ("read", "binary data", n))
	return;
#else
    assert (remaining() >= n && "Reading past end of buffer. Make sure you are reading the right format.");
#endif
    memcpy (reinterpret_cast<value_type*>(buffer), ipos(), n);
    _pos += n;
}

//----------------------------------------------------------------------

template <typename T> struct object_reader {
    inline void operator()(istream& is, T& v) const { v.read (is); }
};
template <typename T> struct integral_object_reader {
    inline void operator()(istream& is, T& v) const { is.iread (v); }
};
template <typename T>
inline istream& operator>> (istream& is, T& v) {
    typedef typename tm::Select <numeric_limits<T>::is_integral,
	integral_object_reader<T>, object_reader<T> >::Result object_reader_t;
    object_reader_t()(is, v);
    return is;
}
template <typename T>
inline istream& operator>> (istream& is, const T& v) { v.read (is); return is; }

//----------------------------------------------------------------------

typedef istream_iterator<utf8subchar_t> istream_iterator_for_utf8;
typedef utf8in_iterator<istream_iterator_for_utf8> utf8istream_iterator;

/// Returns a UTF-8 adaptor reading from \p is.
inline utf8istream_iterator utf8in (istream& is)
{
    istream_iterator_for_utf8 si (is);
    return utf8istream_iterator (si);
}

//----------------------------------------------------------------------

} // namespace ustl

namespace ustl {

class istream;
class string;

/// \class ostream mostream.h ustl.h
/// \ingroup BinaryStreams
///
/// \brief Helper class to write packed binary streams.
///
/// This class contains a set of functions to write integral types into an
/// unstructured memory block. Packing binary file data can be done this
/// way, for instance. aligning the data is your responsibility, and can
/// be accomplished by proper ordering of writes and by calling align.
/// Unaligned access is usually slower by orders of magnitude and,
/// on some architectures, such as PowerPC, can cause your program to crash.
/// Therefore, all write functions have asserts to check alignment.
/// See \ref istream documentation for rules on designing your data format.
/// Overwriting the end of the stream will also cause a crash (an assert in
/// debug builds). Oh, and don't be intimidated by the size of the inlines
/// here. In the assembly code the compiler will usually chop everything down
/// to five instructions each.
///
/// Example code:
/// \code
///     memblock b;
///     ostream os (b);
///     os << boolVar << ios::talign<int>();
///     os << intVar << floatVar;
///     os.write (binaryData, binaryDataSize);
///     os.align();
///     b.resize (os.pos());
///     b.write_file ("test.file");
/// \endcode
///
class ostream : public memlink, public ios_base {
public:
    inline		ostream (void)			: memlink(), _pos(0) {}
    inline		ostream (void* p, streamsize n)	: memlink (p, n), _pos (0) {}
    inline explicit	ostream (const memlink& source)	: memlink (source), _pos (0) {}
    inline iterator	end (void)			{ return memlink::end(); }
    inline const_iterator end (void) const		{ return memlink::end(); }
    inline void		seek (uoff_t newPos);
    inline void		iseek (const_iterator newPos);
    inline void		skip (streamsize nBytes);
    inline uoff_t	pos (void) const		{ return _pos; }
    inline iterator	ipos (void)			{ return begin() + pos(); }
    inline const_iterator ipos (void) const		{ return begin() + pos(); }
    inline streamsize	remaining (void) const;
    inline bool		aligned (streamsize grain = c_DefaultAlignment) const;
    bool		verify_remaining (const char* op, const char* type, size_t n);
    inline streamsize	align_size (streamsize grain = c_DefaultAlignment) const;
    void		align (streamsize grain = c_DefaultAlignment);
    inline void		write (const void* buffer, streamsize size);
    inline void		write (const cmemlink& buf);
    void		write_strz (const char* str);
    void		read (istream& is);
    inline void		write (ostream& os) const	{ os.write (begin(), pos()); }
    void		text_write (ostringstream& os) const;
    inline size_t	stream_size (void) const	{ return pos(); }
    void		insert (iterator start, streamsize size);
    void		erase (iterator start, streamsize size);
    inline void		swap (ostream& os);
    template <typename T>
    inline void		iwrite (const T& v);
    inline virtual ostream&	flush (void)		{ return *this; }
    inline virtual streamsize	overflow (streamsize=1)	{ return remaining(); }
    virtual void	unlink (void) noexcept override;
    inline void		link (void* p, streamsize n)	{ memlink::link (p, n); }
    inline void		link (memlink& l)		{ memlink::link (l.data(), l.writable_size()); }
    inline void		link (void* f, void* l)		{ memlink::link (f, l); }
    inline void		relink (void* p, streamsize n)	{ memlink::relink (p, n); _pos = 0; }
    inline void		relink (memlink& l)		{ relink (l.data(), l.writable_size()); }
    inline void		seekp (off_t p, seekdir d = beg);
    inline off_t	tellp (void) const		{ return pos(); }
protected:
    inline void		SetPos (uoff_t newPos)		{ _pos = newPos; }
private:
    streamoff		_pos;	///< Current write position.
};

//----------------------------------------------------------------------

/// \class ostream_iterator mostream.h ustl.h
/// \ingroup BinaryStreamIterators
///
/// \brief An iterator over an ostream to use with uSTL algorithms.
///
template <typename T, typename Stream = ostream>
class ostream_iterator {
public:
    typedef T			value_type;
    typedef ptrdiff_t		difference_type;
    typedef value_type*		pointer;
    typedef value_type&		reference;
    typedef typename Stream::size_type	size_type;
    typedef output_iterator_tag	iterator_category;
public:
    inline explicit		ostream_iterator (Stream& os)
				    : _os (os) {}
    inline			ostream_iterator (const ostream_iterator& iter)
				    : _os (iter._os) {}
    /// Writes \p v into the stream.
    inline ostream_iterator&	operator= (const T& v)
				    { _os << v; return *this; }
    inline ostream_iterator&	operator* (void) { return *this; }
    inline ostream_iterator&	operator++ (void) { return *this; }
    inline ostream_iterator	operator++ (int) { return *this; }
    inline ostream_iterator&	operator+= (streamsize n) { _os.skip (n); return *this; }
    inline bool			operator== (const ostream_iterator& i) const
				    { return _os.pos() == i._os.pos(); }
    inline bool			operator< (const ostream_iterator& i) const
				    { return _os.pos() < i._os.pos(); }
private:
    Stream&	_os;
};

//----------------------------------------------------------------------

typedef ostream_iterator<utf8subchar_t> ostream_iterator_for_utf8;
typedef utf8out_iterator<ostream_iterator_for_utf8> utf8ostream_iterator;

/// Returns a UTF-8 adaptor writing to \p os.
inline utf8ostream_iterator utf8out (ostream& os)
{
    ostream_iterator_for_utf8 si (os);
    return utf8ostream_iterator (si);
}

//----------------------------------------------------------------------

/// Checks that \p n bytes are available in the stream, or else throws.
inline bool ostream::verify_remaining (const char* op, const char* type, size_t n)
{
    const size_t rem = remaining();
    bool enough = n <= rem;
    if (!enough) overrun (op, type, n, pos(), rem);
    return enough;
}

/// Move the write pointer to \p newPos
inline void ostream::seek (uoff_t newPos)
{
#if WANT_STREAM_BOUNDS_CHECKING
    if (newPos > size())
	return overrun ("seekp", "byte", newPos, pos(), size());
#else
    assert (newPos <= size());
#endif
    SetPos (newPos);
}

/// Sets the current write position to \p newPos
inline void ostream::iseek (const_iterator newPos)
{
    seek (distance (begin(), const_cast<iterator>(newPos)));
}

/// Sets the current write position to \p p based on \p d.
inline void ostream::seekp (off_t p, seekdir d)
{
    switch (d) {
	case beg:	seek (p); break;
	case cur:	seek (pos() + p); break;
	case ios_base::end:	seek (size() - p); break;
    }
}

/// Skips \p nBytes without writing anything.
inline void ostream::skip (streamsize nBytes)
{
    seek (pos() + nBytes);
}

/// Returns number of bytes remaining in the write buffer.
inline streamsize ostream::remaining (void) const
{
    return size() - pos();
}

/// Returns \c true if the write pointer is aligned on \p grain
inline bool ostream::aligned (streamsize grain) const
{
    return pos() % grain == 0;
}

/// Returns the number of bytes to skip to be aligned on \p grain.
inline streamsize ostream::align_size (streamsize grain) const
{
    return Align (pos(), grain) - pos();
}

/// Writes \p n bytes from \p buffer.
inline void ostream::write (const void* buffer, size_type n)
{
#if WANT_STREAM_BOUNDS_CHECKING
    if (!verify_remaining ("write", "binary data", n))
	return;
#else
    assert (remaining() >= n && "Buffer overrun. Check your stream size calculations.");
#endif
    memcpy (ipos(), const_iterator(buffer), n);
    _pos += n;
}

/// Writes the contents of \p buf into the stream as a raw dump.
inline void ostream::write (const cmemlink& buf)
{
    write (buf.begin(), buf.size());
}

/// Writes type T into the stream via a direct pointer cast.
template <typename T>
inline void ostream::iwrite (const T& v)
{
    assert (aligned (stream_align_of (v)));
#if WANT_STREAM_BOUNDS_CHECKING
    if (!verify_remaining ("write", typeid(v).name(), sizeof(T)))
	return;
#else
    assert (remaining() >= sizeof(T));
#endif
    *reinterpret_cast<T*>(ipos()) = v;
    SetPos (pos() + sizeof(T));
}

/// Swaps with \p os
inline void ostream::swap (ostream& os)
{
    memlink::swap (os);
    ::ustl::swap (_pos, os._pos);
}

//----------------------------------------------------------------------

template <typename T> struct object_writer {
    inline void operator()(ostream& os, const T& v) const { v.write (os); }
};
template <typename T> struct integral_object_writer {
    inline void operator()(ostream& os, const T& v) const { os.iwrite (v); }
};
template <typename T>
inline ostream& operator<< (ostream& os, const T& v) {
    typedef typename tm::Select <numeric_limits<T>::is_integral,
	integral_object_writer<T>, object_writer<T> >::Result object_writer_t;
    object_writer_t()(os, v);
    return os;
}

//----------------------------------------------------------------------

} // namespace ustl

namespace ustl {

/// \class array array.h stl.h
/// \ingroup Sequences
///
/// \brief A fixed-size array of \p N \p Ts.
///
template <typename T, size_t N>
class array {
public:
    typedef T						value_type;
    typedef unsigned					size_type;
    typedef value_type*					pointer;
    typedef const value_type*				const_pointer;
    typedef value_type&					reference;
    typedef const value_type&				const_reference;
    typedef pointer					iterator;
    typedef const_pointer				const_iterator;
    typedef ::ustl::reverse_iterator<iterator>		reverse_iterator;
    typedef ::ustl::reverse_iterator<const_iterator>	const_reverse_iterator;
public:
#if HAVE_CPP11
    using initlist_t = std::initializer_list<value_type>;
    inline array&		operator+= (initlist_t v)		{ for (size_type i = 0; i < N; ++i) _v[i] += v.begin()[i]; return *this; }
    inline array&		operator-= (initlist_t v)		{ for (size_type i = 0; i < N; ++i) _v[i] -= v.begin()[i]; return *this; }
    inline array&		operator*= (initlist_t v)		{ for (size_type i = 0; i < N; ++i) _v[i] *= v.begin()[i]; return *this; }
    inline array&		operator/= (initlist_t v)		{ for (size_type i = 0; i < N; ++i) _v[i] /= v.begin()[i]; return *this; }
    inline array		operator+ (initlist_t v) const		{ array result; for (size_type i = 0; i < N; ++i) result[i] = _v[i] + v.begin()[i]; return result; }
    inline array		operator- (initlist_t v) const		{ array result; for (size_type i = 0; i < N; ++i) result[i] = _v[i] - v.begin()[i]; return result; }
    inline array		operator* (initlist_t v) const		{ array result; for (size_type i = 0; i < N; ++i) result[i] = _v[i] * v.begin()[i]; return result; }
    inline array		operator/ (initlist_t v) const		{ array result; for (size_type i = 0; i < N; ++i) result[i] = _v[i] / v.begin()[i]; return result; }
#else
    inline			array (void)				{ fill_n (_v, N, T()); }
    template <typename T2>
    inline			array (const array<T2,N>& v)		{ copy_n (v.begin(), N, _v); }
    inline			array (const array& v)			{ copy_n (v.begin(), N, _v); }
    inline			array (const_pointer v)			{ copy_n (v, N, _v); }
    explicit inline		array (const_reference v0)		{ fill_n (_v, N, v0); }
    inline			array (const_reference v0, const_reference v1)	{ _v[0] = v0; fill_n (_v+1,N-1,v1); }
    inline			array (const_reference v0, const_reference v1, const_reference v2)	{ _v[0] = v0; _v[1] = v1; fill_n (_v+2,N-2,v2); }
    inline			array (const_reference v0, const_reference v1, const_reference v2, const_reference v3)	{ _v[0] = v0; _v[1] = v1; _v[2] = v2; fill_n (_v+3,N-3,v3); }
    template <typename T2>
    inline array&		operator= (const array<T2,N>& v)	{ copy_n (v.begin(), N, _v); return *this; }
    inline array&		operator= (const array& v)		{ copy_n (v.begin(), N, _v); return *this; }
#endif
    inline iterator		begin (void)				{ return _v; }
    inline iterator		end (void)				{ return begin() + N; }
    inline reference		at (size_type i)			{ return _v[i]; }
    inline reference		operator[] (size_type i)		{ return _v[i]; }
    inline constexpr const_iterator	begin (void) const		{ return _v; }
    inline constexpr const_iterator	end (void) const		{ return begin() + N; }
    inline constexpr size_type		size (void) const		{ return N; }
    inline constexpr size_type		max_size (void) const		{ return N; }
    inline constexpr bool		empty (void) const		{ return N == 0; }
    inline constexpr const_reference	at (size_type i) const		{ return _v[i]; }
    inline constexpr const_reference	operator[] (size_type i) const	{ return _v[i]; }
    inline void			read (istream& is)			{ nr_container_read (is, *this); }
    inline void			write (ostream& os) const		{ nr_container_write (os, *this); }
    inline void			text_write (ostringstream& os) const	{ container_text_write (os, *this); }
    inline size_t		stream_size (void) const		{ return nr_container_stream_size (*this); }
    inline array&		operator+= (const_reference v)		{ for (size_type i = 0; i < N; ++i) _v[i] += v; return *this; }
    inline array&		operator-= (const_reference v)		{ for (size_type i = 0; i < N; ++i) _v[i] -= v; return *this; }
    inline array&		operator*= (const_reference v)		{ for (size_type i = 0; i < N; ++i) _v[i] *= v; return *this; }
    inline array&		operator/= (const_reference v)		{ for (size_type i = 0; i < N; ++i) _v[i] /= v; return *this; }
    inline array		operator+ (const_reference v) const	{ array result; for (size_type i = 0; i < N; ++i) result[i] = _v[i] + v; return result; }
    inline array		operator- (const_reference v) const	{ array result; for (size_type i = 0; i < N; ++i) result[i] = _v[i] - v; return result; }
    inline array		operator* (const_reference v) const	{ array result; for (size_type i = 0; i < N; ++i) result[i] = _v[i] * v; return result; }
    inline array		operator/ (const_reference v) const	{ array result; for (size_type i = 0; i < N; ++i) result[i] = _v[i] / v; return result; }
    inline void			fill (const_reference v)		{ ::ustl::fill (begin(), end(), v); }
    inline void			swap (array& v)				{ swap_ranges (begin(), end(), v.begin()); }
public:
    T				_v [N];
};

//----------------------------------------------------------------------

template <typename T1, size_t N, typename T2>
inline bool operator== (const array<T1,N>& t1, const array<T2,N>& t2)
{
    for (unsigned i = 0; i < N; ++ i)
	if (t1[i] != t2[i])
	    return false;
    return true;
}

template <typename T1, size_t N, typename T2>
inline bool operator< (const array<T1,N>& t1, const array<T2,N>& t2)
{
    for (unsigned i = 0; i < N && t1[i] <= t2[i]; ++ i)
	if (t1[i] < t2[i])
	    return true;
    return false;
}

template <typename T1, size_t N, typename T2>
inline const array<T1,N>& operator+= (array<T1,N>& t1, const array<T2,N>& t2)
    { for (unsigned i = 0; i < N; ++ i) t1[i] = T1(t1[i] + t2[i]); return t1; }

template <typename T1, size_t N, typename T2>
inline const array<T1,N>& operator-= (array<T1,N>& t1, const array<T2,N>& t2)
    { for (unsigned i = 0; i < N; ++ i) t1[i] = T1(t1[i] - t2[i]); return t1; }

template <typename T1, size_t N, typename T2>
inline const array<T1,N>& operator*= (array<T1,N>& t1, const array<T2,N>& t2)
    { for (unsigned i = 0; i < N; ++ i) t1[i] = T1(t1[i] * t2[i]); return t1; }

template <typename T1, size_t N, typename T2>
inline const array<T1,N>& operator/= (array<T1,N>& t1, const array<T2,N>& t2)
    { for (unsigned i = 0; i < N; ++ i) t1[i] = T1(t1[i] / t2[i]); return t1; }

template <typename T1, size_t N, typename T2>
inline const array<T1,N> operator+ (const array<T1,N>& t1, const array<T2,N>& t2)
{
    array<T1,N> result;
    for (unsigned i = 0; i < N; ++ i) result[i] = T1(t1[i] + t2[i]);
    return result;
}

template <typename T1, size_t N, typename T2>
inline const array<T1,N> operator- (const array<T1,N>& t1, const array<T2,N>& t2)
{
    array<T1,N> result;
    for (unsigned i = 0; i < N; ++ i) result[i] = T1(t1[i] - t2[i]);
    return result;
}

template <typename T1, size_t N, typename T2>
inline const array<T1,N> operator* (const array<T1,N>& t1, const array<T2,N>& t2)
{
    array<T1,N> result;
    for (unsigned i = 0; i < N; ++ i) result[i] = T1(t1[i] * t2[i]);
    return result;
}

template <typename T1, size_t N, typename T2>
inline const array<T1,N> operator/ (const array<T1,N>& t1, const array<T2,N>& t2)
{
    array<T1,N> result;
    for (unsigned i = 0; i < N; ++ i) result[i] = T1(t1[i] / t2[i]);
    return result;
}

} // namespace ustl

namespace ustl {

/// Copy copies elements from the range [first, last) to the range
/// [result, result + (last - first)). That is, it performs the assignments
/// *result = *first, *(result + 1) = *(first + 1), and so on. [1] Generally,
/// for every integer n from 0 to last - first, copy performs the assignment
/// *(result + n) = *(first + n). Assignments are performed in forward order,
/// i.e. in order of increasing n. 
/// \ingroup MutatingAlgorithms
///
template <typename Container, typename OutputIterator>
inline OutputIterator copy (const Container& ctr, OutputIterator result)
{
    return copy (ctr.begin(), ctr.end(), result);
}

/// Copy_if copies elements from the range [first, last) to the range
/// [result, result + (last - first)) if pred(*i) returns true.
/// \ingroup MutatingAlgorithms
///
template <typename Container, typename OutputIterator, typename Predicate>
inline OutputIterator copy_if (Container& ctr, OutputIterator result, Predicate pred)
{
    return copy_if (ctr.begin(), ctr.end(), result, pred);
}

/// For_each applies the function object f to each element in the range
/// [first, last); f's return value, if any, is ignored. Applications are
/// performed in forward order, i.e. from first to last. For_each returns
/// the function object after it has been applied to each element.
/// \ingroup MutatingAlgorithms
///
template <typename Container, typename UnaryFunction>
inline UnaryFunction for_each (Container& ctr, UnaryFunction f)
{
    return for_each (ctr.begin(), ctr.end(), f);
}

/// For_each applies the function object f to each element in the range
/// [first, last); f's return value, if any, is ignored. Applications are
/// performed in forward order, i.e. from first to last. For_each returns
/// the function object after it has been applied to each element.
/// \ingroup MutatingAlgorithms
///
template <typename Container, typename UnaryFunction>
inline UnaryFunction for_each (const Container& ctr, UnaryFunction f)
{
    return for_each (ctr.begin(), ctr.end(), f);
}

/// Returns the first iterator i in the range [first, last) such that
/// *i == value. Returns last if no such iterator exists. 
/// \ingroup SearchingAlgorithms
///
template <typename Container, typename EqualityComparable>
inline typename Container::const_iterator find (const Container& ctr, const EqualityComparable& value)
{
    return find (ctr.begin(), ctr.end(), value);
}
template <typename Container, typename EqualityComparable>
inline typename Container::iterator find (Container& ctr, const EqualityComparable& value)
{
    return find (ctr.begin(), ctr.end(), value);
}

/// Returns the first iterator i in the range [first, last) such that
/// pred(*i) is true. Returns last if no such iterator exists.
/// \ingroup SearchingAlgorithms
///
template <typename Container, typename Predicate>
inline typename Container::const_iterator find_if (const Container& ctr, Predicate pred)
{
    return find_if (ctr.begin(), ctr.end(), pred);
}
template <typename Container, typename Predicate>
inline typename Container::iterator find_if (Container& ctr, Predicate pred)
{
    return find_if (ctr.begin(), ctr.end(), pred);
}

/// Count finds the number of elements in [first, last) that are equal
/// to value. More precisely, the first version of count returns the
/// number of iterators i in [first, last) such that *i == value.
/// \ingroup ConditionAlgorithms
///
template <typename Container, typename EqualityComparable>
inline size_t count (const Container& ctr, const EqualityComparable& value)
{
    return count (ctr.begin(), ctr.end(), value);
}

/// Count_if finds the number of elements in [first, last) that satisfy the
/// predicate pred. More precisely, the first version of count_if returns the
/// number of iterators i in [first, last) such that pred(*i) is true.
/// \ingroup ConditionAlgorithms
///
template <typename Container, typename Predicate>
inline size_t count_if (const Container& ctr, Predicate pred)
{
    return count_if (ctr.begin(), ctr.end(), pred);
}

/// The first version of transform performs the operation op(*i) for each
/// iterator i in the range [first, last), and assigns the result of that
/// operation to *o, where o is the corresponding output iterator. That is,
/// for each n such that 0 <= n < last - first, it performs the assignment
/// *(result + n) = op(*(first + n)).
/// The return value is result + (last - first).
/// \ingroup MutatingAlgorithms
///
template <typename Container, typename UnaryFunction>
inline void transform (Container& ctr, UnaryFunction op)
{
    transform (ctr.begin(), ctr.end(), ctr.begin(), op);
}

/// The first version of transform performs the operation op(*i) for each
/// iterator i in the range [first, last), and assigns the result of that
/// operation to *o, where o is the corresponding output iterator. That is,
/// for each n such that 0 <= n < last - first, it performs the assignment
/// *(result + n) = op(*(first + n)).
/// The return value is result + (last - first).
/// \ingroup MutatingAlgorithms
///
template <typename Container, typename OutputIterator, typename UnaryFunction>
inline OutputIterator transform (Container& ctr, OutputIterator result, UnaryFunction op)
{
    return transform (ctr.begin(), ctr.end(), result, op);
}

/// The second version of transform is very similar, except that it uses a
/// Binary Function instead of a Unary Function: it performs the operation
/// op(*i1, *i2) for each iterator i1 in the range [first1, last1) and assigns
/// the result to *o, where i2 is the corresponding iterator in the second
/// input range and where o is the corresponding output iterator. That is,
/// for each n such that 0 <= n < last1 - first1, it performs the assignment
/// *(result + n) = op(*(first1 + n), *(first2 + n).
/// The return value is result + (last1 - first1).
/// \ingroup MutatingAlgorithms
///
template <typename Container, typename InputIterator, typename OutputIterator, typename BinaryFunction>
inline OutputIterator transform (Container& ctr, InputIterator first, OutputIterator result, BinaryFunction op)
{
    return transform (ctr.begin(), ctr.end(), first, result, op);
}

/// Replace replaces every element in the range [first, last) equal to
/// old_value with new_value. That is: for every iterator i,
/// if *i == old_value then it performs the assignment *i = new_value.
/// \ingroup MutatingAlgorithms
///
template <typename Container, typename T>
inline void replace (Container& ctr, const T& old_value, const T& new_value)
{
    replace (ctr.begin(), ctr.end(), old_value, new_value);
}

/// Replace_if replaces every element in the range [first, last) for which
/// pred returns true with new_value. That is: for every iterator i, if
/// pred(*i) is true then it performs the assignment *i = new_value.
/// \ingroup MutatingAlgorithms
///
template <typename Container, typename Predicate, typename T>
inline void replace_if (Container& ctr, Predicate pred, const T& new_value)
{
    replace_if (ctr.begin(), ctr.end(), pred, new_value);
}

/// Replace_copy copies elements from the range [first, last) to the range
/// [result, result + (last-first)), except that any element equal to old_value
/// is not copied; new_value is copied instead. More precisely, for every
/// integer n such that 0 <= n < last-first, replace_copy performs the
/// assignment *(result+n) = new_value if *(first+n) == old_value, and
/// *(result+n) = *(first+n) otherwise.
/// \ingroup MutatingAlgorithms
///
template <typename Container, typename OutputIterator, typename T>
inline OutputIterator replace_copy (const Container& ctr, OutputIterator result, const T& old_value, const T& new_value)
{
    return replace_copy (ctr.begin(), ctr.end(), result, old_value, new_value);
}

/// Replace_copy_if copies elements from the range [first, last) to the range
/// [result, result + (last-first)), except that any element for which pred is
/// true is not copied; new_value is copied instead. More precisely, for every
/// integer n such that 0 <= n < last-first, replace_copy_if performs the
/// assignment *(result+n) = new_value if pred(*(first+n)),
/// and *(result+n) = *(first+n) otherwise.
/// \ingroup MutatingAlgorithms
///
template <typename Container, typename OutputIterator, typename Predicate, typename T>
inline OutputIterator replace_copy_if (const Container& ctr, OutputIterator result, Predicate pred, const T& new_value) 
{
    return replace_copy_if (ctr.begin(), ctr.end(), result, pred, new_value);
}

/// Fill assigns the value value to every element in the range [first, last).
/// That is, for every iterator i in [first, last),
/// it performs the assignment *i = value.
/// \ingroup GeneratorAlgorithms
///
template <typename Container, typename T>
inline void fill (Container& ctr, const T& value)
{
    fill (ctr.begin(), ctr.end(), value);
}

/// Generate assigns the result of invoking gen, a function object that
/// takes no arguments, to each element in the range [first, last).
/// \ingroup GeneratorAlgorithms
///
template <typename Container, typename Generator>
inline void generate (Container& ctr, Generator gen)
{
    generate (ctr.begin(), ctr.end(), gen);
}

/// Randomly permute the elements of the container.
/// \ingroup GeneratorAlgorithms
///
template <typename Container>
inline void random_shuffle (Container& ctr)
{
    random_shuffle (ctr.begin(), ctr.end());
}

/// Remove_copy copies elements that are not equal to value from the range
/// [first, last) to a range beginning at result. The return value is the
/// end of the resulting range. This operation is stable, meaning that the
/// relative order of the elements that are copied is the same as in the
/// range [first, last).
/// \ingroup MutatingAlgorithms
///
template <typename Container, typename OutputIterator, typename T>
inline OutputIterator remove_copy (const Container& ctr, OutputIterator result, const T& value)
{
    return remove_copy (ctr.begin(), ctr.end(), result, value);
}

/// Remove_copy_if copies elements from the range [first, last) to a range
/// beginning at result, except that elements for which pred is true are not
/// copied. The return value is the end of the resulting range. This operation
/// is stable, meaning that the relative order of the elements that are copied
/// is the same as in the range [first, last).
/// \ingroup MutatingAlgorithms
///
template <typename Container, typename OutputIterator, typename Predicate>
inline OutputIterator remove_copy_if (const Container& ctr, OutputIterator result, Predicate pred)
{
    return remove_copy_if (ctr.begin(), ctr.end(), result, pred);
}

/// Remove removes from the range [first, last) all elements that are equal to
/// value. That is, remove returns an iterator new_last such that the range
/// [first, new_last) contains no elements equal to value. Remove is stable,
/// meaning that the relative order of elements that are not equal to value is
/// unchanged.
/// \ingroup MutatingAlgorithms
///
template <typename Container, typename T>
inline void remove (Container& ctr, const T& value)
{
    ctr.erase (remove_copy (ctr.begin(), ctr.end(), ctr.begin(), value), ctr.end());
}

/// Remove removes from the range [first, last) all elements that have an iterator
/// in range [rfirst, rlast). The range is assumed to be sorted. That is, remove
/// returns an iterator new_last such that the range [first, new_last) contains
/// no elements whose iterators are in [rfirst, rlast). Remove is stable,
/// meaning that the relative order of elements that are not equal to value is
/// unchanged. This version of the algorithm is a uSTL extension.
/// \ingroup MutatingAlgorithms
///
template <typename Container, typename ForwardIterator>
inline void remove (Container& ctr, ForwardIterator rfirst, ForwardIterator rlast)
{
    ctr.erase (remove_copy (ctr.begin(), ctr.end(), ctr.begin(), rfirst, rlast), ctr.end());
}

/// Remove_if removes from the range [first, last) every element x such that
/// pred(x) is true. That is, remove_if returns an iterator new_last such that
/// the range [first, new_last) contains no elements for which pred is true.
/// The iterators in the range [new_last, last) are all still dereferenceable,
/// but the elements that they point to are unspecified. Remove_if is stable,
/// meaning that the relative order of elements that are not removed is
/// unchanged.
/// \ingroup MutatingAlgorithms
///
template <typename Container, typename Predicate>
inline void remove_if (Container& ctr, Predicate pred)
{
    ctr.erase (remove_copy_if (ctr.begin(), ctr.end(), ctr.begin(), pred), ctr.end());
}

/// Unique_copy copies elements from the range [first, last) to a range
/// beginning with result, except that in a consecutive group of duplicate
/// elements only the first one is copied. The return value is the end of
/// the range to which the elements are copied. This behavior is similar
/// to the Unix filter uniq.
/// \ingroup MutatingAlgorithms
///
template <typename Container, typename OutputIterator>
inline OutputIterator unique_copy (const Container& ctr, OutputIterator result)
{
    return unique_copy (ctr.begin(), ctr.end(), result);
}

/// Every time a consecutive group of duplicate elements appears in the range
/// [first, last), the algorithm unique removes all but the first element.
/// That is, unique returns an iterator new_last such that the range [first,
/// new_last) contains no two consecutive elements that are duplicates.
/// The iterators in the range [new_last, last) are all still dereferenceable,
/// but the elements that they point to are unspecified. Unique is stable,
/// meaning that the relative order of elements that are not removed is
/// unchanged.
/// \ingroup MutatingAlgorithms
///
template <typename Container>
inline void unique (Container& ctr)
{
    ctr.erase (unique_copy (ctr.begin(), ctr.end(), ctr.begin()), ctr.end());
}

/// Every time a consecutive group of duplicate elements appears in the range
/// [first, last), the algorithm unique removes all but the first element.
/// That is, unique returns an iterator new_last such that the range [first,
/// new_last) contains no two consecutive elements that are duplicates.
/// The iterators in the range [new_last, last) are all still dereferenceable,
/// but the elements that they point to are unspecified. Unique is stable,
/// meaning that the relative order of elements that are not removed is
/// unchanged.
/// \ingroup MutatingAlgorithms
///
template <typename Container, typename BinaryPredicate>
inline void unique (Container& ctr, BinaryPredicate binary_pred)
{
    ctr.erase (unique_copy (ctr.begin(), ctr.end(), ctr.begin(), binary_pred), ctr.end());
}

/// Reverse reverses a range.
/// That is: for every i such that 0 <= i <= (last - first) / 2),
/// it exchanges *(first + i) and *(last - (i + 1)).
/// \ingroup MutatingAlgorithms
///
template <typename Container>
inline void reverse (Container& ctr)
{
    reverse (ctr.begin(), ctr.end());
}

/// Exchanges ranges [first, middle) and [middle, last)
/// \ingroup MutatingAlgorithms
///
template <typename Container>
inline void rotate (Container& ctr, off_t offset)
{
    assert (size_t(offset > 0 ? offset : -offset) < ctr.size());
    if (offset > 0)
	rotate (ctr.begin(), ctr.end() - offset, ctr.end());
    else
	rotate (ctr.begin(), ctr.begin() - offset, ctr.end());
}

/// Returns the furthermost iterator i in [first, last) such that,
/// for every iterator j in [first, i), *j < value
/// Assumes the range is sorted.
/// \ingroup SearchingAlgorithms
///
template <typename Container, typename LessThanComparable>
inline typename Container::const_iterator lower_bound (const Container& ctr, const LessThanComparable& value)
{
    return lower_bound (ctr.begin(), ctr.end(), value);
}
template <typename Container, typename LessThanComparable>
inline typename Container::iterator lower_bound (Container& ctr, const LessThanComparable& value)
{
    return lower_bound (ctr.begin(), ctr.end(), value);
}

/// Returns the furthermost iterator i in [first,last) such that for
/// every iterator j in [first,i), value < *j is false.
/// \ingroup SearchingAlgorithms
///
template <typename Container, typename LessThanComparable>
inline typename Container::const_iterator upper_bound (const Container& ctr, const LessThanComparable& value)
{
    return upper_bound (ctr.begin(), ctr.end(), value);
}
template <typename Container, typename LessThanComparable>
inline typename Container::iterator upper_bound (Container& ctr, const LessThanComparable& value)
{
    return upper_bound (ctr.begin(), ctr.end(), value);
}

/// Performs a binary search for \p value.
/// Assumes the range is sorted.
/// \ingroup SearchingAlgorithms
///
template <typename Container>
inline bool binary_search (const Container& ctr, const typename Container::value_type& value)
{
    return binary_search (ctr.begin(), ctr.end(), value);
}
template <typename Container>
inline bool binary_search (Container& ctr, const typename Container::value_type& value)
{
    return binary_search (ctr.begin(), ctr.end(), value);
}

/// Returns pair<lower_bound,upper_bound>
/// \ingroup SearchingAlgorithms
///
template <typename Container, typename LessThanComparable>
inline pair<typename Container::const_iterator,typename Container::const_iterator> equal_range (const Container& ctr, const LessThanComparable& value)
{
    return equal_range (ctr.begin(), ctr.end(), value);
}
template <typename Container, typename LessThanComparable>
inline pair<typename Container::iterator,typename Container::iterator> equal_range (Container& ctr, const LessThanComparable& value)
{
    return equal_range (ctr.begin(), ctr.end(), value);
}

/// Sorts the container
/// \ingroup SortingAlgorithms
///
template <typename Container>
inline void sort (Container& ctr)
{
    sort (ctr.begin(), ctr.end());
}

/// Sorts the container
/// \ingroup SortingAlgorithms
///
template <typename Container, typename Compare>
inline void sort (Container& ctr, Compare comp)
{
    sort (ctr.begin(), ctr.end(), comp);
}

/// Sorts the container
/// \ingroup SortingAlgorithms
///
template <typename Container>
inline void stable_sort (Container& ctr)
{
    stable_sort (ctr.begin(), ctr.end());
}

/// Sorts the container
/// \ingroup SortingAlgorithms
///
template <typename Container, typename Compare>
inline void stable_sort (Container& ctr, Compare comp)
{
    stable_sort (ctr.begin(), ctr.end(), comp);
}

} // namespace ustl

namespace ustl {

class string;

/// \class ostringstream sostream.h ustl.h
/// \ingroup TextStreams
///
/// \brief This stream writes textual data into a memory block.
///
class ostringstream : public ostream {
public:
				ostringstream (const string& v = "");
				ostringstream (void* p, size_t n) noexcept;
    inline fmtflags		flags (void) const		{ return _flags; }
    inline fmtflags		flags (fmtflags f)		{ fmtflags of (_flags); _flags = f; return of; }
    inline fmtflags		setf (fmtflags f)		{ fmtflags of (_flags); _flags |= f; return of; }
    inline fmtflags		unsetf (fmtflags f)		{ fmtflags of (_flags); _flags &= ~f; return of; }
    inline fmtflags		setf (fmtflags f, fmtflags m)	{ unsetf(m); return setf(f); }
    void			iwrite (unsigned char v);
    void			iwrite (wchar_t v);
    inline void			iwrite (char v)			{ iwrite ((unsigned char) v); }
    inline void			iwrite (short v)		{ iformat (v); }
    inline void			iwrite (unsigned short v)	{ iformat (v); }
    inline void			iwrite (int v)			{ iformat (v); }
    inline void			iwrite (unsigned int v)		{ iformat (v); }
    inline void			iwrite (long int v)		{ iformat (v); }
    inline void			iwrite (unsigned long int v)	{ iformat (v); }
    inline void			iwrite (float v)		{ iformat (v); }
    inline void			iwrite (double v)		{ iformat (v); }
    inline void			iwrite (long double v)		{ iformat (v); }
    void			iwrite (bool v);
    inline void			iwrite (const char* s)		{ write (s, strlen(s)); }
    inline void			iwrite (const unsigned char* s)	{ iwrite ((const char*) s); }
    inline void			iwrite (const string& v)	{ write (v.begin(), v.size()); }
    inline void			iwrite (fmtflags_bits f);
#if HAVE_THREE_CHAR_TYPES
    inline void			iwrite (signed char v)		{ iwrite ((char) v); }
#endif
#if HAVE_LONG_LONG
    inline void			iwrite (long long v)		{ iformat (v); }
    inline void			iwrite (unsigned long long v)	{ iformat (v); }
#endif
    inline size_type		max_size (void) const		{ return _buffer.max_size(); }
    inline ostringstream&	put (char c)			{ iwrite (uint8_t(c)); return *this; }
    int				vformat (const char* fmt, va_list args);
    int				format (const char* fmt, ...) __attribute__((__format__(__printf__, 2, 3)));
    inline uint16_t		width (void) const		{ return _width; }
    inline void			width (uint16_t w)		{ _width = w; }
    inline void			set_width (uint16_t w)		{ _width = w; }
    inline char			fill (void) const		{ return _fill; }
    inline void			fill (char c)			{ _fill = c; }
    inline uint8_t		precision (void) const		{ return _precision; }
    inline void			precision (uint8_t v)		{ _precision = v; }
    inline void			set_precision (uint8_t v)	{ _precision = v; }
    void			link (void* p, size_type n) noexcept;
    inline void			link (memlink& l)		{ link (l.data(), l.writable_size()); }
    inline const string&	str (void)			{ flush(); return _buffer; }
    void			str (const string& s);
    ostringstream&		write (const void* buffer, size_type size);
    inline ostringstream&	write (const cmemlink& buf)	{ return write (buf.begin(), buf.size()); }
    inline ostringstream&	seekp (off_t p, seekdir d =beg)	{ ostream::seekp(p,d); return *this; }
    virtual ostream&		flush (void) override		{ ostream::flush(); _buffer.resize (pos()); return *this; }
    virtual size_type		overflow (size_type n = 1) override;
protected:
    inline void			reserve (size_type n)		{ _buffer.reserve (n, false); }
    inline size_type		capacity (void) const		{ return _buffer.capacity(); }
private:
    inline void			write_strz (const char*)	{ assert (!"Writing nul characters into a text stream is not allowed"); }
    inline char*		encode_dec (char* fmt, uint32_t n) const noexcept;
    void			fmtstring (char* fmt, const char* typestr, bool bInteger) const;
    template <typename T>
    void			iformat (T v);
private:
    string			_buffer;	///< The output buffer.
    fmtflags			_flags;		///< See ios_base::fmtflags.
    uint16_t			_width;		///< Field width.
    uint8_t			_precision;	///< Number of digits after the decimal separator.
    char			_fill;		///< Character for padding variable width fields (space or 0 only)
};

//----------------------------------------------------------------------

template <typename T>
inline const char* printf_typestring (const T&)	{ return ""; }
#define PRINTF_TYPESTRING_SPEC(type,str)	\
template <> inline const char* printf_typestring (const type&)	{ return str; }
PRINTF_TYPESTRING_SPEC (short,		"hd")
PRINTF_TYPESTRING_SPEC (unsigned short,	"hu")
PRINTF_TYPESTRING_SPEC (int,		"d")
PRINTF_TYPESTRING_SPEC (unsigned int,	"u")
PRINTF_TYPESTRING_SPEC (long,		"ld")
PRINTF_TYPESTRING_SPEC (unsigned long,	"lu")
PRINTF_TYPESTRING_SPEC (float,		"f")
PRINTF_TYPESTRING_SPEC (double,		"lf")
PRINTF_TYPESTRING_SPEC (long double,	"Lf")
#if HAVE_LONG_LONG
PRINTF_TYPESTRING_SPEC (long long,	"lld")
PRINTF_TYPESTRING_SPEC (unsigned long long, "llu")
#endif
#undef PRINTF_TYPESTRING_SPEC

template <typename T>
void ostringstream::iformat (T v)
{
    char fmt [16];
    fmtstring (fmt, printf_typestring(v), numeric_limits<T>::is_integer);
    format (fmt, v);
}

void ostringstream::iwrite (fmtflags_bits f)
{
    if (f & basefield) setf (f, basefield);
    else if (f & floatfield) setf (f, floatfield);
    else if (f & adjustfield) setf (f, adjustfield);
    setf (f);
}

//----------------------------------------------------------------------

template <typename T> struct object_text_writer {
    inline void operator()(ostringstream& os, const T& v) const { v.text_write (os); }
};
template <typename T> struct integral_text_object_writer {
    inline void operator()(ostringstream& os, const T& v) const { os.iwrite (v); }
};
template <typename T>
inline ostringstream& operator<< (ostringstream& os, const T& v) {
    typedef typename tm::Select <tm::TypeTraits<T>::isFundamental
					|| tm::TypeTraits<T>::isPointer
					|| tm::Conversion<T,long>::exists,
	integral_text_object_writer<T>, object_text_writer<T> >::Result object_writer_t;
    object_writer_t()(os, v);
    return os;
}
// Needed because if called with a char[], numeric_limits will not work. Should be removed if I find out how to partial specialize for arrays...
inline ostringstream& operator<< (ostringstream& os, const char* v)
    { os.iwrite (v); return os; }
inline ostringstream& operator<< (ostringstream& os, char* v)
    { os.iwrite (v); return os; }

//----------------------------------------------------------------------
// Object writer operators

template <> struct object_text_writer<string> {
    inline void operator()(ostringstream& os, const string& v) const { os.iwrite (v); }
};
template <typename T> struct integral_text_object_writer<T*> {
    inline void operator() (ostringstream& os, const T* const& v) const
	{ os.iwrite ((uintptr_t)(v)); }
};

//----------------------------------------------------------------------
// Manipulators

namespace {
static constexpr const struct Sendl {
    inline constexpr Sendl (void) {}
    inline void text_write (ostringstream& os) const	{ os << '\n'; os.flush(); }
    inline void write (ostream& os) const		{ os.iwrite ('\n'); }
} endl;
static constexpr const struct Sflush {
    inline constexpr Sflush (void) {}
    inline void text_write (ostringstream& os) const	{ os.flush(); }
    inline void write (ostringstream& os) const		{ os.flush(); }
    inline void write (ostream&) const			{ }
} flush;
constexpr const char ends = '\0';		///< End of string character.
} // namespace

struct setiosflags {
    inline constexpr setiosflags (ios_base::fmtflags f) : _f(f) {}
    inline void text_write (ostringstream& os) const	{ os.setf(_f); }
private:
    const ios_base::fmtflags _f;
};
struct resetiosflags {
    inline constexpr resetiosflags (ios_base::fmtflags f) : _f(f) {}
    inline void text_write (ostringstream& os) const	{ os.unsetf(_f); }
private:
    const ios_base::fmtflags _f;
};
class setw {
    uint16_t _w;
public:
    inline constexpr setw (uint16_t w)			: _w(w) {}
    inline void text_write (ostringstream& os) const	{ os.width(_w); }
    inline void write (ostringstream& os) const		{ os.width(_w); }
};
class setfill {
    char _c;
public:
    inline constexpr setfill (char c)			: _c(c) {}
    inline void text_write (ostringstream& os) const	{ os.fill(_c); }
    inline void write (ostringstream& os) const		{ os.fill(_c); }
};
class setprecision {
    uint8_t _p;
public:
    inline constexpr setprecision (uint8_t p)		: _p(p) {}
    inline void text_write (ostringstream& os) const	{ os.precision(_p); }
    inline void write (ostringstream& os) const		{ os.precision(_p); }
};

} // namespace ustl

namespace ustl {

class ios : public ios_base {
public:
    /// \class align uiosfunc.h ustl.h
    /// \ingroup StreamFunctors
    /// \brief Stream functor to allow inline align() calls.
    ///
    /// Example: os << ios::align(sizeof(uint16_t));
    ///
    class align {
    public:
	inline explicit		align (size_t grain = c_DefaultAlignment) : _grain(grain) {}
	inline istream&		apply (istream& is) const { is.align (_grain); return is; }
	inline ostream&		apply (ostream& os) const { os.align (_grain); return os; }
	inline void		read (istream& is) const  { apply (is); }
	inline void		write (ostream& os) const { apply (os); }
	inline size_t		stream_size (void) const  { return _grain - 1; }
    private:
	const size_t		_grain;
    };

    /// \class talign uiosfunc.h ustl.h
    /// \ingroup StreamFunctors
    /// \brief Stream functor to allow type-based alignment.
    template <typename T>
    class talign : public align {
    public:
	inline explicit		talign (void) : align (stream_align_of (NullValue<T>())) {}
    };

    /// \class skip uiosfunc.h ustl.h
    /// \ingroup StreamFunctors
    /// \brief Stream functor to allow inline skip() calls.
    ///
    /// Example: os << ios::skip(sizeof(uint16_t));
    ///
    class skip {
    public:
	inline explicit 	skip (size_t nBytes) : _nBytes(nBytes) {}
	inline istream&		apply (istream& is) const { is.skip (_nBytes); return is; }
	inline ostream&		apply (ostream& os) const { os.skip (_nBytes); return os; }
	inline void		read (istream& is) const  { apply (is); }
	inline void		write (ostream& os) const { apply (os); }
	inline size_t		stream_size (void) const  { return _nBytes; }
    private:
	const size_t		_nBytes;
    };

    /// \class width uiosfunc.h ustl.h
    /// \ingroup StreamFunctors
    /// \brief Stream functor to allow inline set_width() calls.
    ///
    /// Example: os << ios::width(15);
    ///
    class width {
    public:
	inline explicit		width (size_t nBytes) : _nBytes(nBytes) {}
	inline ostringstream&	apply (ostringstream& os) const { os.width (_nBytes); return os; }
	inline void		text_write (ostringstream& os) const { apply (os); }
    private:
	const size_t		_nBytes;
    };

    // Deprecated way to set output format base. Use setiosflags manipulator instead.
    struct base {
	inline explicit		base (size_t n) : _f (n == 16 ? hex : (n == 8 ? oct : dec)) {}
	inline void		text_write (ostringstream& os) const { os.setf (_f, basefield); }
    private:
	fmtflags		_f;
    };
};

} // namespace ustl

namespace ustl {

//----------------------------------------------------------------------
// Macros for easily declaring a container streamable.
//----------------------------------------------------------------------

/// \brief Declares container template \p type streamable.
///
/// Use TEMPLATE_TYPE and TEMPLATE_DECL macros to pass in templated
/// type with commas and the template declaration.
///
#define STD_TEMPLATE_CTR_STREAMABLE(type, template_decl)	\
    template_decl						\
    inline istream& operator>> (istream& is, type& v)		\
    { return container_read (is, v); } 			\
    template_decl						\
    inline ostream& operator<< (ostream& os, const type& v)	\
    { return container_write (os, v); } 			\
    template_decl						\
    inline ostringstream& operator<< (ostringstream& os, const type& v)	\
    { return container_text_write (os, v); }			\
    template_decl						\
    struct object_stream_size<type > {				\
	inline size_t operator()(const type& v)	const		\
	    { return container_stream_size (v); }		\
    };

/// \brief Declares non-resizable container template \p type streamable.
#define STD_TEMPLATE_NR_CTR_STREAMABLE(type, template_decl)	\
    template_decl						\
    inline istream& operator>> (istream& is, type& v)		\
    { return nr_container_read (is, v); } 			\
    template_decl						\
    inline ostream& operator<< (ostream& os, const type& v)	\
    { return nr_container_write (os, v); } 			\
    template_decl						\
    inline ostringstream& operator<< (ostringstream& os, const type& v)	\
    { return container_text_write (os, v); }			\
    template_decl						\
    struct object_stream_size<type > {				\
	inline size_t operator()(const type& v)	const		\
	    { return nr_container_stream_size (v); }		\
    };

//----------------------------------------------------------------------
// Fixed size container serialization.
//----------------------------------------------------------------------

/// Reads fixed size container \p v from stream \p is.
template <typename Container>
inline istream& nr_container_read (istream& is, Container& v)
{
    foreach (typename Container::iterator, i, v)
	is >> *i;
    return is;
}

/// Writes fixed size container \p v into stream \p os.
template <typename Container>
inline ostream& nr_container_write (ostream& os, const Container& v)
{
    foreach (typename Container::const_iterator, i, v)
	os << *i;
    return os;
}

/// Computes the stream size of a fixed size standard container.
template <typename Container>
inline size_t nr_container_stream_size (const Container& v)
{
    typedef typename Container::const_iterator vciter_t;
    if (!v.size())
	return 0;
    size_t s = 0, dvs;
    vciter_t i = v.begin();
    do {
	dvs = stream_size_of(*i);
	s += dvs;
    } while (++i != v.end() && !__builtin_constant_p(dvs));
    if (__builtin_constant_p(dvs))
	s *= v.size();
    return s;
}

//----------------------------------------------------------------------
// Resizable container serialization.
//----------------------------------------------------------------------

/// Reads container \p v from stream \p is.
template <typename Container>
istream& container_read (istream& is, Container& v)
{
    typedef typename Container::value_type value_type;
    typedef typename Container::written_size_type written_size_type;
    written_size_type n = 0;
    is >> n;
    const size_t expectedSize = n * stream_size_of(value_type());
#if WANT_STREAM_BOUNDS_CHECKING
    if (!is.verify_remaining ("read", typeid(v).name(), expectedSize))
	return is;
#else
	assert (remaining() >= expectedSize);
#endif
    if (stream_align_of(NullValue<value_type>()) > stream_align_of(n))
	is >> ios::talign<value_type>();
    v.resize (n);
    nr_container_read (is, v);
    is >> ios::talign<written_size_type>();
    return is;
}

/// Writes the vector to stream \p os.
template <typename Container>
ostream& container_write (ostream& os, const Container& v)
{
    typedef typename Container::value_type value_type;
    typedef typename Container::written_size_type written_size_type;
    const written_size_type sz (v.size());
    os << sz;
    if (stream_align_of(NullValue<value_type>()) > stream_align_of(sz))
	os << ios::talign<value_type>();
    nr_container_write (os, v);
    os << ios::talign<written_size_type>();
    return os;
}

/// Computes the stream size of a standard container.
template <typename Container>
size_t container_stream_size (const Container& v)
{
    typedef typename Container::value_type value_type;
    typedef typename Container::written_size_type written_size_type;
    const written_size_type sz (v.size());
    size_t sizeSize = stream_size_of (sz);
    if (stream_align_of(NullValue<value_type>()) > stream_align_of(sz))
	sizeSize = Align (sizeSize, stream_align_of(NullValue<value_type>()));
    return Align (sizeSize + nr_container_stream_size (v), stream_align_of(sz));
}

/// \brief Writes element \p v into stream \p os as text.
/// Specialize to custom print elements.
template <typename T>
inline ostringstream& container_element_text_write (ostringstream& os, const T& v)
{ return os << v; }

/// Writes container \p v into stream \p os as text.
template <typename Container>
ostringstream& container_text_write (ostringstream& os, const Container& v)
{
    os << '(';
    for (typename Container::const_iterator i = v.begin(); i < v.end(); ++i) {
	if (i != v.begin())
	    os << ',';
	container_element_text_write (os, *i);
    }
    os << ')';
    return os;
}

//----------------------------------------------------------------------

} // namespace ustl

#ifndef EOF
    #define EOF (-1)
#endif

namespace ustl {

/// \class istringstream sistream.h ustl.h
/// \ingroup TextStreams
///
/// \brief A stream that reads textual data from a memory block.
///
class istringstream : public istream {
public:
    static const size_type	c_MaxDelimiters = 16;	///< Maximum number of word delimiters.
public:
				istringstream (void) noexcept;
				istringstream (const void* p, size_type n) noexcept;
    explicit			istringstream (const cmemlink& source) noexcept;
    inline fmtflags		flags (void) const		{ return _flags; }
    inline fmtflags		flags (fmtflags f)		{ fmtflags of (_flags); _flags = f; return of; }
    inline fmtflags		setf (fmtflags f)		{ fmtflags of (_flags); _flags |= f; return of; }
    inline fmtflags		unsetf (fmtflags f)		{ fmtflags of (_flags); _flags &= ~f; return of; }
    inline fmtflags		setf (fmtflags f, fmtflags m)	{ unsetf(m); return setf(f); }
    inline void			iread (char& v)			{ v = skip_delimiters(); }
    inline void			iread (unsigned char& v)	{ char c; iread(c); v = c; }
    void			iread (int& v);
    inline void			iread (unsigned int& v)		{ int c; iread(c); v = c; }
    inline void			iread (short& v)		{ int c; iread(c); v = c; }
    inline void			iread (unsigned short& v)	{ int c; iread(c); v = c; }
    void			iread (long& v);
    inline void			iread (unsigned long& v)	{ long c; iread(c); v = c; }
#if HAVE_THREE_CHAR_TYPES
    void			iread (signed char& v)		{ char c; iread(c); v = c; }
#endif
#if HAVE_LONG_LONG
    void			iread (long long& v);
    inline void			iread (unsigned long long& v)	{ long long c; iread(c); v = c; }
#endif
    void			iread (double& v);
    inline void			iread (float& v)		{ double c; iread(c); v = c; }
    inline void			iread (long double& v)		{ double c; iread(c); v = c; }
    void			iread (bool& v);
    void			iread (wchar_t& v);
    void			iread (string& v);
    inline void			iread (fmtflags_bits f);
    inline string		str (void) const	{ string s; s.link (*this); return s; }
    inline istringstream&	str (const string& s)	{ link (s); return *this; }
    inline istringstream&	get (char& c)	{ return read (&c, sizeof(c)); }
    inline int			get (void)	{ char c = EOF; get(c); return c; }
    istringstream&		get (char* p, size_type n, char delim = '\n');
    istringstream&		get (string& s, char delim = '\n');
    istringstream&		getline (char* p, size_type n, char delim = '\n');
    istringstream&		getline (string& s, char delim = '\n');
    istringstream&		ignore (size_type n, char delim = '\0');
    inline char			peek (void)	{ char v = get(); ungetc(); return v; }
    inline istringstream&	unget (void)	{ ungetc(); return *this; }
    inline void			set_delimiters (const char* delimiters);
    istringstream&		read (void* buffer, size_type size);
    inline istringstream&	read (memlink& buf)		{ return read (buf.begin(), buf.size()); }
    inline size_type		gcount (void) const		{ return _gcount; }
    inline istringstream&	seekg (off_t p, seekdir d =beg)	{ istream::seekg(p,d); return *this; }
    inline int			sync (void)			{ skip (remaining()); return 0; }
protected:
    char			skip_delimiters (void);
private:
    inline void			read_strz (string&)	{ assert (!"Reading nul characters is not allowed from text streams"); }
    inline bool			is_delimiter (char c) const noexcept;
    template <typename T> void	read_number (T& v);
private:
    fmtflags			_flags;
    uint32_t			_gcount;
    char			_delimiters [c_MaxDelimiters];
};

//----------------------------------------------------------------------

void istringstream::iread (fmtflags_bits f)
{
    if (f & basefield) setf (f, basefield);
    else if (f & floatfield) setf (f, floatfield);
    else if (f & adjustfield) setf (f, adjustfield);
    setf (f);
}

/// Sets delimiters to the contents of \p delimiters.
void istringstream::set_delimiters (const char* delimiters)
{
#if (__i386__ || __x86_64__) && CPU_HAS_SSE && HAVE_VECTOR_EXTENSIONS
    typedef uint32_t v16ud_t __attribute__((vector_size(16)));
    asm("xorps\t%%xmm0, %%xmm0\n\tmovups\t%%xmm0, %0":"=m"(*noalias_cast<v16ud_t*>(_delimiters))::"xmm0");
#else
    memset (_delimiters, 0, sizeof(_delimiters));
#endif
    memcpy (_delimiters, delimiters, min (strlen(delimiters),sizeof(_delimiters)-1));
}

/// Reads a line of text from \p is into \p s
inline istringstream& getline (istringstream& is, string& s, char delim = '\n')
    { return is.getline (s, delim); }

//----------------------------------------------------------------------

template <typename T> struct object_text_reader {
    inline void operator()(istringstream& is, T& v) const { v.text_read (is); }
};
template <typename T> struct integral_text_object_reader {
    inline void operator()(istringstream& is, T& v) const { is.iread (v); }
};
template <typename T>
inline istringstream& operator>> (istringstream& is, T& v) {
    typedef typename tm::Select <numeric_limits<T>::is_integral,
	integral_text_object_reader<T>, object_text_reader<T> >::Result object_reader_t;
    object_reader_t()(is, v);
    return is;
}

//----------------------------------------------------------------------

template <> struct object_text_reader<string> {
    inline void operator()(istringstream& is, string& v) const { is.iread (v); }
};
} // namespace ustl

namespace ustl {

//----------------------------------------------------------------------
// Alogrithm specializations not in use by the library code.
//----------------------------------------------------------------------

template <> inline void swap (cmemlink& a, cmemlink& b)			{ a.swap (b); }
template <> inline void swap (memlink& a, memlink& b)			{ a.swap (b); }
template <> inline void swap (memblock& a, memblock& b)			{ a.swap (b); }
template <> inline void swap (string& a, string& b)			{ a.swap (b); }
#define TEMPLATE_SWAP_PSPEC(type, template_decl)	\
template_decl inline void swap (type& a, type& b) { a.swap (b); }
TEMPLATE_SWAP_PSPEC (TEMPLATE_TYPE1 (vector,T),		TEMPLATE_DECL1 (T))
TEMPLATE_SWAP_PSPEC (TEMPLATE_TYPE1 (set,T),		TEMPLATE_DECL1 (T))
TEMPLATE_SWAP_PSPEC (TEMPLATE_TYPE1 (multiset,T),	TEMPLATE_DECL1 (T))
TEMPLATE_SWAP_PSPEC (TEMPLATE_TYPE2 (tuple,N,T),	TEMPLATE_FULL_DECL2 (size_t,N,typename,T))

//----------------------------------------------------------------------
// Streamable definitions. Not used in the library and require streams.
//----------------------------------------------------------------------

//----{ pair }----------------------------------------------------------

/// \brief Reads pair \p p from stream \p is.
template <typename T1, typename T2>
istream& operator>> (istream& is, pair<T1,T2>& p)
{
    is >> p.first;
    is.align (stream_align_of(p.second));
    is >> p.second;
    is.align (stream_align_of(p.first));
    return is;
}

/// Writes pair \p p to stream \p os.
template <typename T1, typename T2>
ostream& operator<< (ostream& os, const pair<T1,T2>& p)
{
    os << p.first;
    os.align (stream_align_of(p.second));
    os << p.second;
    os.align (stream_align_of(p.first));
    return os;
}

/// Writes pair \p p to stream \p os.
template <typename T1, typename T2>
ostringstream& operator<< (ostringstream& os, const pair<T1,T2>& p)
{
    os << '(' << p.first << ',' << p.second << ')';
    return os;
}

/// Returns the written size of the object.
template <typename T1, typename T2>
struct object_stream_size<pair<T1,T2> > {
    inline size_t operator()(const pair<T1,T2>& v) const
    {
	return Align (stream_size_of(v.first), stream_align_of(v.second)) +
		Align (stream_size_of(v.second), stream_align_of(v.first));
    }
};

/// \brief Takes a pair and returns pair.first
/// This is an extension, available in uSTL and the SGI STL.
template <typename Pair> struct select1st : public unary_function<Pair,typename Pair::first_type> {
    typedef typename Pair::first_type result_type;
    inline const result_type&	operator()(const Pair& a) const { return a.first; }
    inline result_type&		operator()(Pair& a) const { return a.first; }
};

/// \brief Takes a pair and returns pair.second
/// This is an extension, available in uSTL and the SGI STL.
template <typename Pair> struct select2nd : public unary_function<Pair,typename Pair::second_type> {
    typedef typename Pair::second_type result_type;
    inline const result_type&	operator()(const Pair& a) const { return a.second; }
    inline result_type&		operator()(Pair& a) const { return a.second; }
};

/// \brief Converts a const_iterator pair into an iterator pair
/// Useful for converting pair ranges returned by equal_range, for instance.
/// This is an extension, available in uSTL.
template <typename Container>
inline pair<typename Container::iterator, typename Container::iterator>
unconst (const pair<typename Container::const_iterator, typename Container::const_iterator>& i, Container&)
{
    typedef pair<typename Container::iterator, typename Container::iterator> unconst_pair_t;
    return *noalias_cast<unconst_pair_t*>(&i);
}

//----{ vector }--------------------------------------------------------

template <typename T>
inline size_t stream_align_of (const vector<T>&)
{
    typedef typename vector<T>::written_size_type written_size_type;
    return stream_align_of (written_size_type());
}

//----{ bitset }--------------------------------------------------------

/// Writes bitset \p v into stream \p os.
template <size_t Size>
istringstream& operator>> (istringstream& is, bitset<Size>& v)
{
    char c;
    for (int i = Size; --i >= 0 && (is >> c).good();)
	v.set (i, c == '1');
    return is;
}

//----{ tuple }---------------------------------------------------------

template <size_t N, typename T>
inline istream& operator>> (istream& is, tuple<N,T>& v)
    { v.read (is); return is; }
template <size_t N, typename T>
inline ostream& operator<< (ostream& os, const tuple<N,T>& v)
    { v.write (os); return os; }
template <size_t N, typename T>
inline ostringstream& operator<< (ostringstream& os, const tuple<N,T>& v)
    { v.text_write (os); return os; }

template <size_t N, typename T>
struct numeric_limits<tuple<N,T> > {
    typedef numeric_limits<T> value_limits;
    static inline tuple<N,T> min (void)	{ tuple<N,T> v; fill (v, value_limits::min()); return v; }
    static inline tuple<N,T> max (void)	{ tuple<N,T> v; fill (v, value_limits::max()); return v; }
    static const bool is_signed = value_limits::is_signed;
    static const bool is_integer = value_limits::is_integer;
    static const bool is_integral = value_limits::is_integral;
};

template <size_t N, typename T>
inline size_t stream_align_of (const tuple<N,T>&) { return stream_align_of (NullValue<T>()); }

template <typename T, typename IntT>
inline ostringstream& chartype_text_write (ostringstream& os, const T& v)
{
    os.format (_FmtPrtChr[!isprint(v)], v);
    return os;
}

template <>
inline ostringstream& container_element_text_write (ostringstream& os, const uint8_t& v)
{ return chartype_text_write<uint8_t, unsigned int> (os, v); }
template <>
inline ostringstream& container_element_text_write (ostringstream& os, const int8_t& v)
{ return chartype_text_write<int8_t, int> (os, v); }

//----{ matrix }--------------------------------------------------------

/// Writes tuple \p v into stream \p os.
template <size_t NX, size_t NY, typename T>
ostringstream& operator<< (ostringstream& os, const matrix<NX,NY,T>& v)
{
    os << '(';
    for (uoff_t row = 0; row < NY; ++ row) {
	os << '(';
        for (uoff_t column = 0; column < NX; ++column)
	    os << v[row][column] << ",)"[column == NX-1];
    }
    os << ')';
    return os;
}

//----{ long4grain }----------------------------------------------------

#if SIZE_OF_LONG == 8 && HAVE_INT64_T
// Helper class for long4grain and ptr4grain wrappers.
class _long4grain {
public:
    inline	_long4grain (uint64_t v)	: _v (v) {}
#if __x86_64__
    inline void	read (istream& is)
    {
	assert (is.aligned(4));
	#if WANT_STREAM_BOUNDS_CHECKING
	    if (!is.verify_remaining ("read", "long4grain", sizeof(_v))) return;
	#else
	    assert (is.remaining() >= sizeof(_v));
	#endif
	_v = *reinterpret_cast<const uint64_t*>(is.ipos());
	is.skip (sizeof(_v));
    }
    inline void	write (ostream& os) const
    {
	assert (os.aligned(4));
	#if WANT_STREAM_BOUNDS_CHECKING
	    if (!os.verify_remaining ("write", "long4grain", sizeof(_v))) return;
	#else
	    assert (os.remaining() >= sizeof(_v));
	#endif
	*reinterpret_cast<uint64_t*>(os.ipos()) = _v;
	os.skip (sizeof(_v));
    }
#elif USTL_BYTE_ORDER == USTL_BIG_ENDIAN
    inline void	read (istream& is)		{ uint32_t vl, vh; is >> vh >> vl; _v = (uint64_t(vh) << 32) | uint64_t(vl); }
    inline void	write (ostream& os) const	{ os << uint32_t(_v >> 32) << uint32_t(_v); }
#else
    inline void	read (istream& is)		{ uint32_t vl, vh; is >> vl >> vh; _v = (uint64_t(vh) << 32) | uint64_t(vl); }
    inline void	write (ostream& os) const	{ os << uint32_t(_v) << uint32_t(_v >> 32); }
#endif
    inline size_t stream_size (void) const	{ return stream_size_of(_v); }
private:
    uint64_t	_v;
};

/// Wrap long values to allow writing them on 4-grain even on 64bit platforms.
inline _long4grain& long4grain (unsigned long& v)		{ asm("":"+m"(v)); return *noalias_cast<_long4grain*>(&v); }
/// Wrap long values to allow writing them on 4-grain even on 64bit platforms.
inline const _long4grain long4grain (const unsigned long& v)	{ return _long4grain(v); }
/// Wrap pointer values to allow writing them on 4-grain even on 64bit platforms.
template <typename T>
inline _long4grain& ptr4grain (T*& p)				{ asm("":"+m"(p)); return *noalias_cast<_long4grain*>(&p); }
/// Wrap pointer values to allow writing them on 4-grain even on 64bit platforms.
template <typename T>
inline const _long4grain ptr4grain (const T* const& p)		{ return _long4grain(uintptr_t(p)); }
#else	// if not SIZE_OF_LONG == 8 && HAVE_INT64_T
inline unsigned long& long4grain (unsigned long& v)		{ return v; }
inline const unsigned long& long4grain (const unsigned long& v)	{ return v; }
template <typename T> inline T*& ptr4grain (T*& p)		{ return p; }
template <typename T> inline const T* const& ptr4grain (const T* const& p) { return p; }
#endif	// SIZE_OF_LONG == 8

//----------------------------------------------------------------------

} // namespace ustl

// This is here because there really is no other place to put it.
#if SIZE_OF_BOOL != SIZE_OF_CHAR
// bool is a big type on some machines (like DEC Alpha), so it's written as a byte.
ALIGNOF(bool, sizeof(uint8_t))
CAST_STREAMABLE(bool, uint8_t)
#endif
#if SIZE_OF_LONG == 8 && HAVE_INT64_T
ALIGNOF (_long4grain, 4)
#endif

ALIGNOF(ustl::CBacktrace, sizeof(void*))
ALIGNOF (ustl::string, stream_align_of (string::value_type()))

namespace ustl {

template <typename Pair, typename Comp>
struct pair_compare_first : public binary_function<Pair,Pair,bool> {
    inline bool operator()(const Pair& a, const Pair& b) { return Comp()(a.first,b.first); }
};
template <typename K, typename V, typename Comp>
struct pair_compare_first_key : public binary_function<pair<K,V>,K,bool> {
    inline bool operator()(const pair<K,V>& a, const K& b) { return Comp()(a.first,b); }
    inline bool operator()(const K& a, const pair<K,V>& b) { return Comp()(a,b.first); }
};

/// \class map umap.h ustl.h
/// \ingroup AssociativeContainers
///
/// \brief A sorted associative container of pair<K,V>
///
template <typename K, typename V, typename Comp = less<K> >
class map : public vector<pair<K,V> > {
public:
    typedef K						key_type;
    typedef V						data_type;
    typedef const K&					const_key_ref;
    typedef const V&					const_data_ref;
    typedef const map<K,V,Comp>&			rcself_t;
    typedef vector<pair<K,V> >				base_class;
    typedef typename base_class::value_type		value_type;
    typedef typename base_class::size_type		size_type;
    typedef typename base_class::pointer		pointer;
    typedef typename base_class::const_pointer		const_pointer;
    typedef typename base_class::reference		reference;
    typedef typename base_class::const_reference	const_reference;
    typedef typename base_class::const_iterator		const_iterator;
    typedef typename base_class::iterator		iterator;
    typedef typename base_class::reverse_iterator	reverse_iterator;
    typedef typename base_class::const_reverse_iterator	const_reverse_iterator;
    typedef pair<const_iterator,const_iterator>		const_range_t;
    typedef pair<iterator,iterator>			range_t;
    typedef pair<iterator,bool>				insertrv_t;
    typedef Comp					key_compare;
    typedef pair_compare_first<value_type,Comp>		value_compare;
    typedef pair_compare_first_key<K,V,Comp>		value_key_compare;
public:
    inline			map (void)			: base_class() {}
    explicit inline		map (size_type n)		: base_class (n) {}
    inline			map (rcself_t v)		: base_class (v) {}
    inline			map (const_iterator i1, const_iterator i2) : base_class() { insert (i1, i2); }
    inline rcself_t		operator= (rcself_t v)		{ base_class::operator= (v); return *this; }
    inline const_data_ref	at (const_key_ref k) const	{ assert (find(k) != end()); return find(k)->second; }
    inline data_type&		at (const_key_ref k)		{ assert (find(k) != end()); return find(k)->second; }
    inline const_data_ref	operator[] (const_key_ref i) const	{ return at(i); }
    data_type&			operator[] (const_key_ref i);
    inline key_compare		key_comp (void) const		{ return key_compare(); }
    inline value_compare	value_comp (void) const		{ return value_compare(); }
    inline size_type		size (void) const		{ return base_class::size(); }
    inline iterator		begin (void)			{ return base_class::begin(); }
    inline const_iterator	begin (void) const		{ return base_class::begin(); }
    inline iterator		end (void)			{ return base_class::end(); }
    inline const_iterator	end (void) const		{ return base_class::end(); }
    inline void			assign (const_iterator i1, const_iterator i2)	{ clear(); insert (i1, i2); }
    inline void			push_back (const_reference v)	{ insert (v); }
    inline const_iterator	find (const_key_ref k) const;
    inline iterator		find (const_key_ref k)		{ return const_cast<iterator> (const_cast<rcself_t>(*this).find (k)); }
    inline const_iterator	find_data (const_data_ref v, const_iterator first = nullptr, const_iterator last = nullptr) const;
    inline iterator		find_data (const_data_ref v, iterator first = nullptr, iterator last = nullptr)	{ return const_cast<iterator> (find_data (v, const_cast<const_iterator>(first), const_cast<const_iterator>(last))); }
    const_iterator		lower_bound (const_key_ref k) const	{ return ::ustl::lower_bound (begin(), end(), k, value_key_compare()); }
    inline iterator		lower_bound (const_key_ref k)		{ return const_cast<iterator>(const_cast<rcself_t>(*this).lower_bound (k)); }
    const_iterator		upper_bound (const_key_ref k) const	{ return ::ustl::upper_bound (begin(), end(), k, value_key_compare()); }
    inline iterator		upper_bound (const_key_ref k)		{ return const_cast<iterator>(const_cast<rcself_t>(*this).upper_bound (k)); }
    const_range_t		equal_range (const_key_ref k) const	{ return ::ustl::equal_range (begin(), end(), k, value_key_compare()); }
    inline range_t		equal_range (const_key_ref k)		{ return ::ustl::equal_range (begin(), end(), k, value_key_compare()); }
    inline size_type		count (const_key_ref v) const		{ const_range_t r = equal_range(v); return distance(r.first,r.second); }
    insertrv_t			insert (const_reference v);
    inline iterator		insert (const_iterator, const_reference v)	{ return insert(v).first; }
    void			insert (const_iterator i1, const_iterator i2)	{ for (; i1 != i2; ++i1) insert (*i1); }
    inline void			erase (const_key_ref k);
    inline iterator		erase (iterator ep)		{ return base_class::erase (ep); }
    inline iterator		erase (iterator ep1, iterator ep2) { return base_class::erase (ep1, ep2); }
    inline void			clear (void)			{ base_class::clear(); }
    inline void			swap (map& v)			{ base_class::swap (v); }
#if HAVE_CPP11
    using initlist_t = std::initializer_list<value_type>;
    inline			map (map&& v)			: base_class (move(v)) {}
    inline			map (initlist_t v)		: base_class() { insert (v.begin(), v.end()); }
    inline map&			operator= (map&& v)		{ base_class::operator= (move(v)); return *this; }
    insertrv_t			insert (value_type&& v);
    inline iterator		insert (const_iterator, value_type&& v)	{ return insert(move(v)).first; }
    inline void			insert (initlist_t v)		{ insert (v.begin(), v.end()); }
    template <typename... Args>
    inline insertrv_t		emplace (Args&&... args)	{ return insert (value_type(forward<Args>(args)...)); }
    template <typename... Args>
    inline iterator		emplace_hint (const_iterator h, Args&&... args)	{ return insert (h, value_type(forward<Args>(args)...)); }
    template <typename... Args>
    inline insertrv_t		emplace_back (Args&&... args)	{ return insert (value_type(forward<Args>(args)...)); }
#endif
};

/// Returns the pair<K,V> where K = \p k.
template <typename K, typename V, typename Comp>
inline typename map<K,V,Comp>::const_iterator map<K,V,Comp>::find (const_key_ref k) const
{
    const_iterator i = lower_bound (k);
    return (i < end() && Comp()(k,i->first)) ? end() : i;
}

/// Returns the pair<K,V> where V = \p v, occuring in range [first,last).
template <typename K, typename V, typename Comp>
inline typename map<K,V,Comp>::const_iterator map<K,V,Comp>::find_data (const_data_ref v, const_iterator first, const_iterator last) const
{
    if (!first) first = begin();
    if (!last) last = end();
    for (; first != last && first->second != v; ++first) ;
    return first;
}

/// Returns data associated with key \p k.
template <typename K, typename V, typename Comp>
typename map<K,V,Comp>::data_type& map<K,V,Comp>::operator[] (const_key_ref k)
{
    iterator ip = lower_bound (k);
    if (ip == end() || Comp()(k,ip->first))
	ip = base_class::insert (ip, make_pair (k, V()));
    return ip->second;
}

/// Inserts the pair into the container.
template <typename K, typename V, typename Comp>
typename map<K,V,Comp>::insertrv_t map<K,V,Comp>::insert (const_reference v)
{
    iterator ip = lower_bound (v.first);
    bool bInserted = ip == end() || Comp()(v.first, ip->first);
    if (bInserted)
	ip = base_class::insert (ip, v);
    return make_pair (ip, bInserted);
}

#if HAVE_CPP11
/// Inserts the pair into the container.
template <typename K, typename V, typename Comp>
typename map<K,V,Comp>::insertrv_t map<K,V,Comp>::insert (value_type&& v)
{
    iterator ip = lower_bound (v.first);
    bool bInserted = ip == end() || Comp()(v.first, ip->first);
    if (bInserted)
	ip = base_class::insert (ip, move(v));
    return make_pair (ip, bInserted);
}
#endif

/// Erases the element with key value \p k.
template <typename K, typename V, typename Comp>
inline void map<K,V,Comp>::erase (const_key_ref k)
{
    iterator ip = find (k);
    if (ip != end())
	erase (ip);
}

} // namespace ustl

namespace ustl {

/// \class multimap umultimap.h ustl.h
/// \ingroup AssociativeContainers
///
/// \brief A sorted associative container that may container multiple entries for each key.
///
template <typename K, typename V, typename Comp = less<K> >
class multimap : public vector<pair<K,V> > {
public:
    typedef K						key_type;
    typedef V						data_type;
    typedef const K&					const_key_ref;
    typedef const V&					const_data_ref;
    typedef const multimap<K,V,Comp>&			rcself_t;
    typedef vector<pair<K,V> >				base_class;
    typedef typename base_class::value_type		value_type;
    typedef typename base_class::size_type		size_type;
    typedef typename base_class::pointer		pointer;
    typedef typename base_class::const_pointer		const_pointer;
    typedef typename base_class::reference		reference;
    typedef typename base_class::const_reference	const_reference;
    typedef typename base_class::const_iterator		const_iterator;
    typedef typename base_class::iterator		iterator;
    typedef typename base_class::reverse_iterator	reverse_iterator;
    typedef typename base_class::const_reverse_iterator	const_reverse_iterator;
    typedef pair<const_iterator,const_iterator>		const_range_t;
    typedef pair<iterator,iterator>			range_t;
    typedef Comp					key_compare;
    typedef pair_compare_first<value_type,Comp>		value_compare;
    typedef pair_compare_first_key<K,V,Comp>		value_key_compare;
public:
    inline			multimap (void)		: base_class() {}
    explicit inline		multimap (size_type n)	: base_class (n) {}
    inline			multimap (rcself_t v)	: base_class (v) {}
    inline			multimap (const_iterator i1, const_iterator i2)	: base_class() { insert (i1, i2); }
    inline rcself_t		operator= (rcself_t v)	{ base_class::operator= (v); return *this; }
    inline key_compare		key_comp (void) const	{ return key_compare(); }
    inline value_compare	value_comp (void) const	{ return value_compare(); }
    inline size_type		size (void) const	{ return base_class::size(); }
    inline iterator		begin (void)		{ return base_class::begin(); }
    inline const_iterator	begin (void) const	{ return base_class::begin(); }
    inline iterator		end (void)		{ return base_class::end(); }
    inline const_iterator	end (void) const	{ return base_class::end(); }
    inline const_iterator	find (const_key_ref k) const;
    inline iterator		find (const_key_ref k)			{ return const_cast<iterator> (const_cast<rcself_t>(*this).find (k)); }
    const_iterator		lower_bound (const_key_ref k) const	{ return ::ustl::lower_bound (begin(), end(), k, value_key_compare()); }
    inline iterator		lower_bound (const_key_ref k)		{ return const_cast<iterator>(const_cast<rcself_t>(*this).lower_bound (k)); }
    const_iterator		upper_bound (const_key_ref k) const	{ return ::ustl::upper_bound (begin(), end(), k, value_key_compare()); }
    inline iterator		upper_bound (const_key_ref k)		{ return const_cast<iterator>(const_cast<rcself_t>(*this).upper_bound (k)); }
    const_range_t		equal_range (const_key_ref k) const	{ return ::ustl::equal_range (begin(), end(), k, value_key_compare()); }
    inline range_t		equal_range (const_key_ref k)		{ return ::ustl::equal_range (begin(), end(), k, value_key_compare()); }
    inline size_type		count (const_key_ref v) const		{ const_range_t r = equal_range(v); return distance(r.first,r.second); }
    inline void			assign (const_iterator i1, const_iterator i2) { clear(); insert (i1, i2); }
    inline void			push_back (const_reference v)		{ insert (v); }
    inline iterator		insert (const_reference v)		{ return base_class::insert (upper_bound (v.first), v); }
    void			insert (const_iterator i1, const_iterator i2)	{ for (; i1 != i2; ++i1) insert (*i1); }
    inline void			clear (void)				{ base_class::clear(); }
    inline void			erase (const_key_ref k)			{ erase (const_cast<iterator>(lower_bound(k)), const_cast<iterator>(upper_bound(k))); }
    inline iterator		erase (const_iterator ep)			{ return base_class::erase (ep); } 
    inline iterator		erase (const_iterator ep1, const_iterator ep2)	{ return base_class::erase (ep1, ep2); } 
    inline void			swap (multimap& v)			{ base_class::swap (v); }
#if HAVE_CPP11
    using initlist_t = std::initializer_list<value_type>;
    inline			multimap (multimap&& v)			: base_class (move(v)) {}
    inline			multimap (initlist_t v)			: base_class() { insert (v.begin(), v.end()); }
    inline multimap&		operator= (multimap&& v)		{ base_class::operator= (move(v)); return *this; }
    iterator			insert (value_type&& v)			{ return base_class::insert (upper_bound (v.first), move(v)); }
    inline iterator		insert (const_iterator, value_type&& v)	{ return insert(move(v)); }
    inline void			insert (initlist_t v)			{ insert (v.begin(), v.end()); }
    template <typename... Args>
    inline iterator		emplace (Args&&... args)		{ return insert (value_type(forward<Args>(args)...)); }
    template <typename... Args>
    inline iterator		emplace_hint (const_iterator h, Args&&... args)	{ return insert (h, value_type(forward<Args>(args)...)); }
    template <typename... Args>
    inline iterator		emplace_back (Args&&... args)		{ return insert (value_type(forward<Args>(args)...)); }
#endif
};

/// Returns the pair<K,V> where K = \p k.
template <typename K, typename V, typename Comp>
inline typename multimap<K,V,Comp>::const_iterator multimap<K,V,Comp>::find (const_key_ref k) const
{
    const_iterator i = lower_bound (k);
    return (i < end() && Comp()(k, i->first)) ? end() : i;
}

} // namespace ustl

namespace ustl {

/// \class stack ustack.h ustl.h
/// \ingroup Sequences
///
/// Stack adapter to uSTL containers.
///
template <typename T, typename Container = vector<T> >
class stack {
public:
    typedef T			value_type;
    typedef Container		container_type;
    typedef typename container_type::size_type size_type;
    typedef typename container_type::difference_type difference_type;
    typedef value_type&		reference;
    typedef const value_type&	const_reference;
public:
    inline			stack (void)			: _storage () { }
    explicit inline		stack (const container_type& s)	: _storage (s) { }
    explicit inline		stack (const stack& s)		: _storage (s._storage) { }
    inline bool			empty (void) const		{ return _storage.empty(); }
    inline size_type		size (void) const		{ return _storage.size(); }
    inline reference		top (void)			{ return _storage.back(); }
    inline const_reference	top (void) const		{ return _storage.back(); }
    inline void			push (const_reference v)	{ _storage.push_back (v); }
    inline void			pop (void)			{ _storage.pop_back(); }
    inline void			swap (stack& v)			{ _storage.swap (v); }
    inline bool			operator== (const stack& s) const	{ return _storage == s._storage; }
    inline bool			operator< (const stack& s) const	{ return _storage.size() < s._storage.size(); }
#if HAVE_CPP11
    inline			stack (stack&& v)		: _storage(move(v._storage)) {}
    inline			stack (container_type&& s)	: _storage(move(s)) {}
    inline stack&		operator= (stack&& v)		{ swap (v); return *this; }
    template <typename... Args>
    inline void			emplace (Args&&... args)	{ _storage.emplace_back (forward<Args>(args)...); }
#endif
private:
    container_type		_storage;	///< Where the data actually is.
};

} // namespace ustl

namespace ustl {

/// \class queue uqueue.h ustl.h
/// \ingroup Sequences
///
/// Queue adapter to uSTL containers.
///
template <typename T, typename Container = vector<T> >
class queue {
public:
    typedef T			value_type;
    typedef Container		container_type;
    typedef typename container_type::size_type size_type;
    typedef typename container_type::difference_type difference_type;
    typedef value_type&		reference;
    typedef const value_type&	const_reference;
public:
    inline			queue (void)			: _storage(), _front (0) { }
    explicit inline		queue (const container_type& s)	: _storage (s), _front (0) { }
    explicit inline		queue (const queue& s)		: _storage (s._storage), _front (0) { }
    inline size_type		size (void) const		{ return _storage.size() - _front; }
    inline bool			empty (void) const		{ return !size(); }
    inline reference		front (void)			{ return _storage [_front]; }
    inline const_reference	front (void) const		{ return _storage [_front]; }
    inline reference		back (void)			{ return _storage.back(); }
    inline const_reference	back (void) const		{ return _storage.back(); }
    inline void			push (const_reference v)	{ _storage.push_back (v); }
    void			pop (void) {
				    if (++_front > _storage.size()/2) {
					_storage.erase (_storage.begin(), _front);
					_front = 0;
				    }
				}
    inline void			swap (queue& v)			{ _storage.swap (v); swap (_front, v._front); }
    inline bool			operator== (const queue& s) const	{ return _storage == s._storage && _front == s._front; }
    inline bool			operator< (const queue& s) const	{ return size() < s.size(); }
#if HAVE_CPP11
    inline			queue (queue&& v)		: _storage(move(v._storage)),_front(v._front) { v._front = 0; }
    inline			queue (container_type&& s)	: _storage(move(s)),_front(0) {}
    inline queue&		operator= (queue&& v)		{ swap (v); return *this; }
    template <typename... Args>
    inline void			emplace (Args&&... args)	{ _storage.emplace_back (forward<Args>(args)...); }
#endif
private:
    container_type		_storage;	///< Where the data actually is.
    size_type			_front;	///< Index of the element returned by next pop.
};

} // namespace ustl

struct stat;

namespace ustl {

/// \class fstream fstream.h ustl.h
/// \ingroup DeviceStreams
///
/// \brief Implements file operations.
///
/// This is not implemented as a stream, but rather as a base for one. You
/// should use ifstream or ofstream if you want flow operators. Otherwise
/// this only implements functions for binary i/o.
///
class fstream : public ios_base {
public:
			fstream (void) noexcept;
    explicit		fstream (const char* filename, openmode mode = in|out);
    explicit		fstream (int nfd, const char* filename = "");
		       ~fstream (void) noexcept;
    void		open (const char* filename, openmode mode, mode_t perms = 0644);
    void		attach (int nfd, const char* filename = "");
    void		detach (void) noexcept;
    void		close (void);
    void		sync (void);
    off_t		read (void* p, off_t n);
    off_t		readsome (void* p, off_t n);
    off_t		write (const void* p, off_t n);
    off_t		size (void) const;
    off_t		seek (off_t n, seekdir whence = beg);
    off_t		pos (void) const noexcept;
    void		stat (struct stat& rs) const;
    int			ioctl (const char* rname, int request, long argument = 0);
    inline int		ioctl (const char* rname, int request, int argument)	{ return fstream::ioctl (rname, request, long(argument)); }
    inline int		ioctl (const char* rname, int request, void* argument)	{ return fstream::ioctl (rname, request, intptr_t(argument)); }
    int			fcntl (const char* rname, int request, long argument = 0);
    inline int		fcntl (const char* rname, int request, int argument)	{ return fstream::fcntl (rname, request, long(argument)); }
    inline int		fcntl (const char* rname, int request, void* argument)	{ return fstream::fcntl (rname, request, intptr_t(argument)); }
    void		set_nonblock (bool v = true) noexcept;
#if HAVE_SYS_MMAN_H
    memlink		mmap (off_t n, off_t offset = 0);
    void		munmap (memlink& l);
    void		msync (memlink& l);
#endif
    inline int		fd (void) const		{ return _fd; }
    inline bool		is_open (void) const	{ return fd() >= 0; }
    inline off_t	tellg (void) const	{ return pos(); }
    inline off_t	tellp (void) const	{ return pos(); }
    inline void		seekg (off_t n, seekdir whence = beg)	{ seek (n, whence); }
    inline void		seekp (off_t n, seekdir whence = beg)	{ seek (n, whence); }
    inline void		flush (void)		{ sync(); }
    inline const string& name (void) const	{ return _filename; }
private:
   DLL_LOCAL static int	om_to_flags (openmode m) noexcept;
    DLL_LOCAL void	set_and_throw (iostate s, const char* op);
private:
    int			_fd;		///< Currently open file descriptor.
    string		_filename;	///< Currently open filename.
};

/// Argument macro for fstream::ioctl. Use like fs.ioctl (IOCTLID (TCGETS), &ts).
#define IOCTLID(r)	"ioctl("#r")", r
#define FCNTLID(r)	"fcntl("#r")", r

} // namespace ustl

namespace ustl {

/// \class ofstream fdostream.h ustl.h
/// \ingroup DeviceStreams
/// \brief A string stream that writes to an fd. Implements cout and cerr.
class ofstream : public ostringstream {
public:
			ofstream (void);
    explicit		ofstream (int ofd);
    explicit		ofstream (const char* filename, openmode mode = out);
    virtual		~ofstream (void) noexcept;
    inline void		open (const char* filename, openmode mode = out) { _file.open (filename, mode); clear (_file.rdstate()); }
    void		close (void);
    inline bool		is_open (void) const		{ return _file.is_open(); }
    inline iostate	exceptions (iostate v)		{ ostringstream::exceptions(v); return _file.exceptions(v); }
    inline void		setstate (iostate v)		{ ostringstream::setstate(v); _file.setstate(v); }
    inline void		clear (iostate v = goodbit)	{ ostringstream::clear(v); _file.clear(v); }
    inline off_t	tellp (void) const		{ return _file.tellp() + ostringstream::tellp(); }
    inline int		fd (void) const			{ return _file.fd(); }
    inline void		stat (struct stat& rs) const	{ _file.stat (rs); }
    inline void		set_nonblock (bool v = true)	{ _file.set_nonblock (v); }
    inline int		ioctl (const char* rname, int request, long argument = 0)	{ return _file.ioctl (rname, request, argument); }
    inline int		ioctl (const char* rname, int request, int argument)		{ return _file.ioctl (rname, request, argument); }
    inline int		ioctl (const char* rname, int request, void* argument)		{ return _file.ioctl (rname, request, argument); }
    ofstream&		seekp (off_t p, seekdir d = beg);
    virtual ostream&	flush (void) override;
    virtual size_type	overflow (size_type n = 1) override;
private:
    fstream		_file;
};

/// \class ifstream fdostream.h ustl.h
/// \ingroup DeviceStreams
/// \brief A string stream that reads from an fd. Implements cin.
class ifstream : public istringstream {
public:
			ifstream (void);
    explicit		ifstream (int ifd);
    explicit		ifstream (const char* filename, openmode mode = in);
    inline void		open (const char* filename, openmode mode = in)	{ _file.open (filename, mode); clear (_file.rdstate()); }
    inline void		close (void)			{ _file.close(); clear (_file.rdstate()); }
    inline bool		is_open (void) const		{ return _file.is_open(); }
    inline iostate	exceptions (iostate v)		{ istringstream::exceptions(v); return _file.exceptions(v); }
    inline void		setstate (iostate v)		{ istringstream::setstate(v); _file.setstate(v); }
    inline void		clear (iostate v = goodbit)	{ istringstream::clear(v); _file.clear(v); }
    inline off_t	tellg (void) const		{ return _file.tellg() - remaining(); }
    inline int		fd (void) const			{ return _file.fd(); }
    inline void		stat (struct stat& rs) const	{ _file.stat (rs); }
    inline void		set_nonblock (bool v = true)	{ _file.set_nonblock (v); }
    void		set_buffer_size (size_type sz);
    ifstream&		putback (char c)		{ ungetc(); _buffer[pos()] = c; return *this; }
    inline int		ioctl (const char* rname, int request, long argument = 0)	{ return _file.ioctl (rname, request, argument); }
    inline int		ioctl (const char* rname, int request, int argument)		{ return _file.ioctl (rname, request, argument); }
    inline int		ioctl (const char* rname, int request, void* argument)		{ return _file.ioctl (rname, request, argument); }
    ifstream&		seekg (off_t p, seekdir d = beg);
    int			sync (void);
    virtual size_type	underflow (size_type n = 1) override;
private:
    string		_buffer;
    fstream		_file;
};

extern ofstream cout, cerr;
extern ifstream cin;

} // namespace ustl

namespace ustl {

/// Returns the sum of all elements in [first, last) added to \p init.
/// \ingroup NumericAlgorithms
///
template <typename InputIterator, typename T>
inline T accumulate (InputIterator first, InputIterator last, T init)
{
    while (first < last)
	init += *first++;
    return init;
}

/// Returns the sum of all elements in [first, last) via \p op, added to \p init.
/// \ingroup NumericAlgorithms
///
template <typename InputIterator, typename T, typename BinaryFunction>
inline T accumulate (InputIterator first, InputIterator last, T init, BinaryFunction binary_op)
{
    while (first < last)
	init = binary_op (init, *first++);
    return init;
}

/// Assigns range [value, value + (last - first)) to [first, last)
/// \ingroup NumericAlgorithms
///
template <typename ForwardIterator, typename T>
inline void iota (ForwardIterator first, ForwardIterator last, T value)
{
    while (first < last)
	*first++ = value++;
}

/// Returns the sum of products of respective elements in the given ranges.
/// \ingroup NumericAlgorithms
///
template <typename InputIterator1, typename InputIterator2, typename T>
inline T inner_product (InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init)
{
    while (first1 < last1)
	init += *first1++ * *first2++;
    return init;
}

/// Returns the sum of products of respective elements in the given ranges.
/// \ingroup NumericAlgorithms
///
template <typename InputIterator1, typename InputIterator2, typename T,
    	  typename BinaryOperation1, typename BinaryOperation2>
inline T inner_product
(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init,
 BinaryOperation1 sumOp, BinaryOperation2 productOp)
{
    while (first1 < last1)
	init = sumOp (init, productOp (*first1++, *first2++));
    return init;
}

/// Writes result such that result[i] = sum (first...first+i)
/// \ingroup NumericAlgorithms
///
template <typename InputIterator, typename OutputIterator>
inline OutputIterator partial_sum (InputIterator first, InputIterator last, OutputIterator result)
{
    if (first < last)
	*result = *first++;
    while (first < last)
	*++result = *first++ + *result;
    return result;
}

/// Writes result such that result[i] = sumOp (first...first+i)
/// \ingroup NumericAlgorithms
///
template <typename InputIterator, typename OutputIterator, typename BinaryOperation>
inline OutputIterator partial_sum (InputIterator first, InputIterator last, OutputIterator result, BinaryOperation sumOp)
{
    if (first < last)
	*result = *first++;
    while (first < last)
	*++result = sumOp (*first++, *result);
    return result;
}

/// Writes result such that result[i] = first[i] - first[i - 1]
/// \ingroup NumericAlgorithms
///
template <typename InputIterator, typename OutputIterator>
inline OutputIterator adjacent_difference (InputIterator first, InputIterator last, OutputIterator result)
{
    if (first < last)
	*result++ = *first++;
    while (first < last)
	*result++ = *first - *(first - 1);
    return result;
}

/// Writes result such that result[i] = differenceOp (first[i], first[i - 1])
/// \ingroup NumericAlgorithms
///
template <typename InputIterator, typename OutputIterator, typename BinaryOperation>
inline OutputIterator adjacent_difference (InputIterator first, InputIterator last, OutputIterator result, BinaryOperation differenceOp)
{
    if (first < last)
	*result++ = *first++;
    while (first < last)
	*result++ = differenceOp (*first, *(first - 1));
    return result;
}

/// \brief Returns x^n.
/// Donald Knuth's Russian Peasant algorithm.
/// \ingroup NumericAlgorithms
///
template <typename T>
inline T power (T x, unsigned n)
{
    T result (n % 2 ? x : 1);
    while (n /= 2) {
	x *= x;
	if (n % 2)
	    result *= x;
    }
    return result;
}

/// \brief Returns x^n, using \p op instead of multiplication.
/// Donald Knuth's Russian Peasant algorithm.
/// \ingroup NumericAlgorithms
///
template <typename T, typename BinaryOperation>
inline T power (T x, unsigned n, BinaryOperation op)
{
    T result (n % 2 ? x : 1);
    while (n /= 2) {
	x = op (x, x);
	if (n % 2)
	    result = op (result, x);
    }
    return result;
}

} // namespace ustl

namespace ustl {

/// \class list ulist.h ustl.h
/// \ingroup Sequences
///
/// \brief Linked list, defined as an alias to vector.
///
template <typename T>
class list : public vector<T> {
public:
    typedef typename vector<T>::size_type	size_type;
    typedef typename vector<T>::iterator	iterator;
    typedef typename vector<T>::const_iterator	const_iterator;
    typedef typename vector<T>::reference	reference;
    typedef typename vector<T>::const_reference	const_reference;
public:
    inline			list (void)			: vector<T> () {}
    inline explicit		list (size_type n)		: vector<T> (n) {}
    inline			list (size_type n, const T& v)	: vector<T> (n, v) {}
    inline			list (const list<T>& v)		: vector<T> (v) {}
    inline			list (const_iterator i1, const_iterator i2)	: vector<T> (i1, i2) {}
    inline size_type		size (void) const		{ return vector<T>::size(); }
    inline iterator		begin (void)			{ return vector<T>::begin(); }
    inline const_iterator	begin (void) const		{ return vector<T>::begin(); }
    inline iterator		end (void)			{ return vector<T>::end(); }
    inline const_iterator	end (void) const		{ return vector<T>::end(); }
    inline void			push_front (const T& v)		{ insert (begin(), v); }
    inline void			pop_front (void)		{ erase (begin()); }
    inline const_reference	front (void) const		{ return *begin(); }
    inline reference		front (void)			{ return *begin(); }
    inline void			remove (const T& v)		{ ::ustl::remove (*this, v); }
    template <typename Predicate>
    inline void			remove_if (Predicate p)		{ ::ustl::remove_if (*this, p); }
    inline void			reverse (void)			{ ::ustl::reverse (*this); }
    inline void			unique (void)			{ ::ustl::unique (*this); }
    inline void			sort (void)			{ ::ustl::sort (*this); }
    void			merge (list<T>& l);
    void			splice (iterator ip, list<T>& l, iterator first = nullptr, iterator last = nullptr);
#if HAVE_CPP11
    inline			list (list&& v)			: vector<T> (move(v)) {}
    inline			list (std::initializer_list<T> v) : vector<T>(v) {}
    inline list&		operator= (list&& v)		{ vector<T>::operator= (move(v)); return *this; }
    template <typename... Args>
    inline void			emplace_front (Args&&... args)	{ vector<T>::emplace (begin(), forward<Args>(args)...); }
    inline void			push_front (T&& v)		{ emplace_front (move(v)); }
#endif
};

/// Merges the contents with \p l. Assumes both lists are sorted.
template <typename T>
void list<T>::merge (list& l)
{
    this->insert_space (begin(), l.size());
    ::ustl::merge (this->iat(l.size()), end(), l.begin(), l.end(), begin());
}

/// Moves the range [first, last) from \p l to this list at \p ip.
template <typename T>
void list<T>::splice (iterator ip, list<T>& l, iterator first, iterator last)
{
    if (!first)
	first = l.begin();
    if (!last)
	last = l.end();
    this->insert (ip, first, last);
    l.erase (first, last);
}

#if HAVE_CPP11
    template <typename T> using deque = list<T>;
#else
    #define deque list ///< list has all the functionality provided by deque
#endif


} // namespace ustl

namespace ustl {

/// \brief Returns true if the given range is a heap under \p comp.
/// A heap is a sequentially encoded binary tree where for every node
/// comp(node,child1) is false and comp(node,child2) is false.
/// \ingroup HeapAlgorithms
/// \ingroup ConditionAlgorithms
///
template <typename RandomAccessIterator, typename Compare>
bool is_heap (RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
    RandomAccessIterator iChild (first);
    for (; ++iChild < last; ++first)
	if (comp (*first, *iChild) || (++iChild < last && comp (*first, *iChild)))
	    return false;
    return true;
}

/// Utility function to "trickle down" the root item - swaps the root item with its
/// largest child and recursively fixes the proper subtree.
template <typename RandomAccessIterator, typename Compare>
void trickle_down_heap (RandomAccessIterator first, size_t iHole, size_t heapSize, Compare comp)
{
    typedef typename iterator_traits<RandomAccessIterator>::value_type value_type;
    const value_type v (first[iHole]);
    for (size_t iChild; (iChild = 2 * iHole + 1) < heapSize;) {
	if (iChild + 1 < heapSize)
	    iChild += comp (first[iChild], first[iChild + 1]);
	if (comp (v, first[iChild])) {
	    first[iHole] = first[iChild];
	    iHole = iChild;
	} else
	    break;
    }
    first[iHole] = v;
}

/// \brief make_heap turns the range [first, last) into a heap
/// At completion, is_heap (first, last, comp) is true.
/// The algorithm is adapted from "Classic Data Structures in C++" by Timothy Budd.
/// \ingroup HeapAlgorithms
/// \ingroup SortingAlgorithms
///
template <typename RandomAccessIterator, typename Compare>
void make_heap (RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
    if (last <= first)
	return;
    const size_t heapSize = distance (first, last);
    for (RandomAccessIterator i = first + (heapSize - 1)/2; i >= first; --i)
	trickle_down_heap (first, distance(first,i), heapSize, comp);
}

/// \brief Inserts the *--last into the preceeding range assumed to be a heap.
/// \ingroup HeapAlgorithms
/// \ingroup MutatingAlgorithms
template <typename RandomAccessIterator, typename Compare>
void push_heap (RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
    if (last <= first)
	return;
    typedef typename iterator_traits<RandomAccessIterator>::value_type value_type;
    const value_type v (*--last);
    while (first < last) {
	RandomAccessIterator iParent = first + (distance(first, last) - 1) / 2;
	if (comp (v, *iParent))
	    break;
	*last = *iParent;
	last = iParent;
    }
    *last = v;
}

/// Removes the largest element from the heap (*first) and places it at *(last-1)
/// [first, last-1) is a heap after this operation.
/// \ingroup HeapAlgorithms
/// \ingroup MutatingAlgorithms
template <typename RandomAccessIterator, typename Compare>
void pop_heap (RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
    if (--last <= first)
	return;
    iter_swap (first, last);
    trickle_down_heap (first, 0, distance(first,last), comp);
}

/// Sorts heap [first, last) in descending order according to comp.
/// \ingroup HeapAlgorithms
/// \ingroup SortingAlgorithms
template <typename RandomAccessIterator, typename Compare>
void sort_heap (RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
    for (; first < last; --last)
	pop_heap (first, last, comp);
}

#define HEAP_FN_WITH_LESS(rtype, name)	\
template <typename RandomAccessIterator>\
inline rtype name (RandomAccessIterator first, RandomAccessIterator last)		\
{											\
    typedef typename iterator_traits<RandomAccessIterator>::value_type value_type;	\
    return name (first, last, less<value_type>());					\
}
HEAP_FN_WITH_LESS (bool, is_heap)
HEAP_FN_WITH_LESS (void, make_heap)
HEAP_FN_WITH_LESS (void, push_heap)
HEAP_FN_WITH_LESS (void, pop_heap)
HEAP_FN_WITH_LESS (void, sort_heap)
#undef HEAP_FN_WITH_LESS

/// \class priority_queue uheap.h ustl.h
/// \ingroup Sequences
///
/// \brief Sorted queue adapter to uSTL containers.
///
/// Acts just like the queue adapter, but keeps the elements sorted by priority
/// specified by the given comparison operator.
///
template <typename T, typename Container = vector<T>, typename Comp = less<typename Container::value_type> >
class priority_queue {
public:
    typedef Container				container_type;
    typedef typename container_type::size_type	size_type;
    typedef typename container_type::value_type	value_type;
    typedef typename container_type::reference	reference;
    typedef typename container_type::const_reference	const_reference;
    typedef typename container_type::const_iterator	const_iterator;
public:
    inline explicit		priority_queue (const Comp& c = Comp()) : _v(), _c(c) {}
    inline			priority_queue (const Comp& c, const container_type& v) : _v(v), _c(c) {}
				priority_queue (const_iterator f, const_iterator l, const Comp& c = Comp())
				    : _v(f, l), _c(c) { make_heap (_v.begin(), _v.end(), _c); }
    inline size_type		size (void) const	{ return _v.size(); }
    inline bool			empty (void) const	{ return _v.empty(); }
    inline const_reference	top (void) const	{ return _v.front(); }
    inline void			push (const_reference v){ _v.push_back (v); push_heap (_v.begin(), _v.end(), _c); }
    inline void			pop (void)		{ pop_heap (_v.begin(), _v.end()); _v.pop_back(); }
    inline void			swap (priority_queue& v){ _v.swap (v._v); swap (_c, v._c); }
#if HAVE_CPP11
    inline explicit		priority_queue (priority_queue&& v)	: _v(move(v._v)),_c(v._c) {}
    inline			priority_queue (const Comp& c, container_type&& v)	: _v(move(v)),_c(c) {}
				priority_queue (const_iterator f, const_iterator l, const Comp& c, container_type&& v)
				    : _v(move(v)), _c(c) { _v.insert (_v.end(), f, l); make_heap (_v.begin(), _v.end(), _c); }
    inline priority_queue&	operator= (priority_queue&& v)	{ swap (v); return *this; }
    template <typename... Args>
    inline void			emplace (Args&&... args)	{ _v.emplace_back (forward<Args>(args)...); push_heap (_v.begin(), _v.end(), _c); }
#endif
private:
    container_type		_v;	///< Element container.
    Comp			_c;	///< Comparison functor by value.
};

} // namespace ustl

#if HAVE_CPP14
//----------------------------------------------------------------------
namespace ustl {

class error_code;
class error_condition;

template <typename T> struct is_error_code_enum : public false_type { };
template <typename T> struct is_error_condition_enum : public false_type {};

//{{{ errc enum of error codes for error_code and error_condition ------

enum class errc {
    address_family_not_supported	= EAFNOSUPPORT,
    address_in_use			= EADDRINUSE,
    address_not_available		= EADDRNOTAVAIL,
    already_connected			= EISCONN,
    argument_list_too_long		= E2BIG,
    argument_out_of_domain		= EDOM,
    bad_address				= EFAULT,
    bad_file_descriptor			= EBADF,
    bad_message				= EBADMSG,
    broken_pipe				= EPIPE,
    connection_aborted			= ECONNABORTED,
    connection_already_in_progress	= EALREADY,
    connection_refused			= ECONNREFUSED,
    connection_reset			= ECONNRESET,
    cross_device_link			= EXDEV,
    destination_address_required	= EDESTADDRREQ,
    device_or_resource_busy		= EBUSY,
    directory_not_empty			= ENOTEMPTY,
    executable_format_error		= ENOEXEC,
    file_exists				= EEXIST,
    file_too_large			= EFBIG,
    filename_too_long			= ENAMETOOLONG,
    function_not_supported		= ENOSYS,
    host_unreachable			= EHOSTUNREACH,
    identifier_removed			= EIDRM,
    illegal_byte_sequence		= EILSEQ,
    inappropriate_io_control_operation	= ENOTTY,
    interrupted				= EINTR,
    invalid_argument			= EINVAL,
    invalid_seek			= ESPIPE,
    io_error				= EIO,
    is_a_directory			= EISDIR,
    message_size			= EMSGSIZE,
    network_down			= ENETDOWN,
    network_reset			= ENETRESET,
    network_unreachable			= ENETUNREACH,
    no_buffer_space			= ENOBUFS,
    no_child_process			= ECHILD,
    no_link				= ENOLINK,
    no_lock_available			= ENOLCK,
    no_message_available		= ENODATA,
    no_message				= ENOMSG,
    no_protocol_option			= ENOPROTOOPT,
    no_space_on_device			= ENOSPC,
    no_stream_resources			= ENOSR,
    no_such_device_or_address		= ENXIO,
    no_such_device			= ENODEV,
    no_such_file_or_directory		= ENOENT,
    no_such_process			= ESRCH,
    not_a_directory			= ENOTDIR,
    not_a_socket			= ENOTSOCK,
    not_a_stream			= ENOSTR,
    not_connected			= ENOTCONN,
    not_enough_memory			= ENOMEM,
    not_supported			= EINVAL,
    operation_canceled			= ECANCELED,
    operation_in_progress		= EINPROGRESS,
    operation_not_permitted		= EPERM,
    operation_not_supported		= EOPNOTSUPP,
    operation_would_block		= EAGAIN,
    owner_dead				= EOWNERDEAD,
    permission_denied			= EACCES,
    protocol_error			= EPROTO,
    protocol_not_supported		= EPROTONOSUPPORT,
    read_only_file_system		= EROFS,
    resource_deadlock_would_occur	= EDEADLK,
    resource_unavailable_try_again	= EAGAIN,
    result_out_of_range			= ERANGE,
    state_not_recoverable		= ENOTRECOVERABLE,
    stream_timeout			= ETIME,
    text_file_busy			= ETXTBSY,
    timed_out				= ETIMEDOUT,
    too_many_files_open_in_system	= ENFILE,
    too_many_files_open			= EMFILE,
    too_many_links			= EMLINK,
    too_many_symbolic_link_levels	= ELOOP,
    value_too_large			= EOVERFLOW,
    wrong_protocol_type			= EPROTOTYPE
};

template<> struct is_error_code_enum<errc> : public true_type {};
template<> struct is_error_condition_enum<errc> : public true_type {};

error_code make_error_code (errc) noexcept;
error_condition make_error_condition (errc) noexcept;

//}}}-------------------------------------------------------------------
//{{{ error_category

class error_category {
public:
    constexpr			error_category (void) noexcept = default;
    error_category&		operator= (const error_category&) = delete;
    inline constexpr auto	name (void) const	{ return "system"; }
    inline auto			message (int ec) const	{ return strerror(ec); }
    inline error_condition	default_error_condition (int ec) const noexcept;
    inline constexpr bool	equivalent (int ec, const error_condition& cond) const noexcept;
    inline constexpr bool	equivalent (const error_code& ec, int c) const noexcept;
    inline constexpr bool	operator== (const error_category&) const { return true; }
    inline constexpr bool	operator!= (const error_category&) const { return false; }
};

inline constexpr auto system_category (void)	{ return error_category(); }
inline constexpr auto generic_category (void)	{ return error_category(); }

//}}}-------------------------------------------------------------------
//{{{ error_code

class error_code {
public:
    inline constexpr		error_code (void) : _ec(0) { }
    inline constexpr		error_code (int ec, const error_category&) : _ec(ec) {}
    template <typename ErrorCodeEnum,
		typename = typename enable_if<is_error_code_enum<ErrorCodeEnum>::value>::type>
    inline constexpr		error_code (ErrorCodeEnum e) : error_code (make_error_code(e)) {}
    inline void			assign (int ec, const error_category&) noexcept	{ _ec = ec; }
    template <typename ErrorCodeEnum>
    inline typename enable_if<is_error_code_enum<ErrorCodeEnum>::value, error_code&>::type
				operator= (ErrorCodeEnum e)	{ return *this = make_error_code(e); }
    inline void			clear (void)			{ assign (0, system_category()); }
    inline constexpr auto	value (void) const		{ return _ec; }
    inline constexpr auto	category (void) const		{ return system_category(); }
    inline error_condition	default_error_condition (void) const noexcept;
    inline auto			message (void) const		{ return category().message (value()); }
    inline constexpr explicit		operator bool (void) const	{ return _ec; }
    inline void			read (istream& is);
    inline void			write (ostream& os) const;
    inline constexpr bool	operator== (const error_code& v) const	{ return category() == v.category() && value() == v.value(); }
    inline constexpr bool	operator== (const error_condition& v) const noexcept;
private:
    int				_ec;
};

inline error_code make_error_code (errc e) noexcept
    { return error_code (static_cast<int>(e), system_category()); }

//}}}-------------------------------------------------------------------
//{{{ error_condition

class error_condition {
public:
    inline constexpr		error_condition (void) : _ec(0) { }
    inline constexpr		error_condition (int ec, const error_category&) : _ec(ec) {}
    template <typename ErrorConditionEnum,
		typename = typename enable_if<is_error_condition_enum<ErrorConditionEnum>::value>::type>
    inline constexpr		error_condition (ErrorConditionEnum e) : error_condition (make_error_condition(e)) {}
    inline void			assign (int ec, const error_category&) noexcept	{ _ec = ec; }
    template <typename ErrorConditionEnum>
    inline typename enable_if<is_error_condition_enum<ErrorConditionEnum>::value, error_condition&>::type
				operator=(ErrorConditionEnum e)	{ return *this = make_error_condition(e); }
    inline void			clear (void)			{ assign (0, generic_category()); }
    inline constexpr auto	value (void) const		{ return _ec; }
    inline constexpr auto	category (void) const		{ return generic_category(); }
    inline auto			message (void) const		{ return category().message (value()); }
    inline constexpr explicit	operator bool (void) const	{ return value(); }
    inline void			read (istream& is);
    inline void			write (ostream& os) const;
    inline constexpr bool	operator== (const error_condition& v) const
				    { return category() == v.category() && value() == v.value(); }
    inline constexpr bool	operator== (const error_code& v) const
				    { return category() == v.category() && value() == v.value(); }
private:
    int				_ec;
};

inline error_condition make_error_condition (errc e) noexcept
    { return error_condition (static_cast<int>(e), generic_category()); }

error_condition	error_category::default_error_condition (int ec) const noexcept
    { return error_condition (ec, *this); }
constexpr bool error_category::equivalent (int ec, const error_condition& cond) const noexcept
    { return *this == cond.category() && ec == cond.value(); }
constexpr bool error_category::equivalent (const error_code& ec, int c) const noexcept
    { return *this == ec.category() && c == ec.value(); }

error_condition	error_code::default_error_condition (void) const noexcept
    { return error_condition (value(), category()); }
constexpr bool error_code::operator== (const error_condition& v) const noexcept
    { return category() == v.category() && value() == v.value(); }

//}}}-------------------------------------------------------------------
} // namespace ustl
#endif // HAVE_CPP14

namespace ustl {

enum {
    xfmt_ErrorMessage	= xfmt_BadAlloc+1,
    xfmt_LogicError	= xfmt_ErrorMessage,
    xfmt_RuntimeError	= xfmt_ErrorMessage,
    xfmt_SystemError,
    xfmt_FileException	= 13,
    xfmt_StreamBoundsException
};

/// \class logic_error ustdxept.h ustl.h
/// \ingroup Exceptions
///
/// \brief Logic errors represent problems in the internal logic of the program.
///
class error_message : public exception {
public:
    explicit		error_message (const char* arg) noexcept;
    virtual		~error_message (void) noexcept;
    inline virtual const char*	what (void) const noexcept override { return _arg.c_str(); }
    inline virtual const char*	name (void) const noexcept { return "error"; }
    virtual void	info (string& msgbuf, const char* fmt = nullptr) const noexcept override;
    virtual void	read (istream& is) override;
    virtual void	write (ostream& os) const override;
    virtual size_t	stream_size (void) const noexcept override;
protected:
    string		_arg;
};

/// \class logic_error ustdxept.h ustl.h
/// \ingroup Exceptions
///
/// \brief Logic errors represent problems in the internal logic of the program.
///
class logic_error : public error_message {
public:
    inline explicit		logic_error (const char* arg) noexcept : error_message (arg) {}
    inline virtual const char*	name (void) const noexcept override { return "logic error"; }
};

/// \class domain_error ustdxept.h ustl.h
/// \ingroup Exceptions
///
/// \brief Reports domain errors ("domain" is in the mathematical sense)
///
class domain_error : public logic_error {
public:
    inline explicit		domain_error (const char* arg) noexcept : logic_error (arg) {}
    inline virtual const char*	name (void) const noexcept override { return "domain error"; }
};

/// \class invalid_argument ustdxept.h ustl.h
/// \ingroup Exceptions
///
/// \brief Reports an invalid argument to a function.
///
class invalid_argument : public logic_error {
public:
    inline explicit		invalid_argument (const char* arg) noexcept : logic_error (arg) {}
    inline virtual const char*	name (void) const noexcept override { return "invalid argument"; }
};

/// \class length_error ustdxept.h ustl.h
/// \ingroup Exceptions
///
/// \brief Reports when an object exceeds its allowed size.
///
class length_error : public logic_error {
public:
    inline explicit		length_error (const char* arg) noexcept : logic_error (arg) {} 
    inline virtual const char*	name (void) const noexcept override { return "length error"; }
};

/// \class out_of_range ustdxept.h ustl.h
/// \ingroup Exceptions
///
/// \brief Reports arguments with values out of allowed range.
///
class out_of_range : public logic_error {
public:
    inline explicit		out_of_range (const char* arg) noexcept : logic_error (arg) {}
    inline virtual const char*	name (void) const noexcept override { return "out of range"; }
};

/// \class runtime_error ustdxept.h ustl.h
/// \ingroup Exceptions
///
/// \brief Reports errors that are dependent on the data being processed.
///
class runtime_error : public error_message {
public:
    inline explicit		runtime_error (const char* arg) noexcept : error_message (arg) {}
    inline virtual const char*	name (void) const noexcept override { return "runtime error"; }
};

/// \class range_error ustdxept.h ustl.h
/// \ingroup Exceptions
///
/// \brief Reports data that does not fall within the permitted range.
///
class range_error : public runtime_error {
public:
    inline explicit		range_error (const char* arg) noexcept : runtime_error (arg) {}
    inline virtual const char*	name (void) const noexcept override { return "range error"; }
};

/// \class overflow_error ustdxept.h ustl.h
/// \ingroup Exceptions
///
/// \brief Reports arithmetic overflow.
///
class overflow_error : public runtime_error {
public:
    inline explicit		overflow_error (const char* arg) noexcept : runtime_error (arg) {}
    inline virtual const char*	name (void) const noexcept override { return "overflow error"; }
};

/// \class underflow_error ustdxept.h ustl.h
/// \ingroup Exceptions
///
/// \brief Reports arithmetic underflow.
///
class underflow_error : public runtime_error {
public:
    inline explicit		underflow_error (const char* arg) noexcept : runtime_error (arg) {}
    inline virtual const char*	name (void) const noexcept override { return "underflow error"; }
};

/// \class system_error uexception.h ustl.h
/// \ingroup Exceptions
///
/// \brief Thrown when a libc function returns an error.
///
/// Contains an errno and description. This is a uSTL extension.
///
class system_error : public runtime_error {
public:
    explicit		system_error (const char* operation) noexcept;
    inline virtual const char*	what (void) const noexcept override { return "system error"; }
    inline virtual const char*	name (void) const noexcept override { return _operation.c_str(); }
    virtual void	read (istream& is) override;
    virtual void	write (ostream& os) const override;
    virtual size_t	stream_size (void) const noexcept override;
    inline int		Errno (void) const	{ return _errno; }
    inline const char*	Operation (void) const	{ return _operation.c_str(); }
#if HAVE_CPP14
			system_error (error_code ec, const char* operation) noexcept
			    : runtime_error (ec.message()),_operation(operation),_errno(ec.value()) {}
    inline              system_error (error_code ec, const string& operation) noexcept
			    : system_error (ec, operation.c_str()) {}
    inline              system_error (int ec, const error_category& ecat, const char* operation)
			    : system_error (error_code(ec,ecat), operation) {}
    inline              system_error (int ec, const error_category& ecat, const string& operation)
			    : system_error (ec, ecat, operation.c_str()) {}
    inline auto		code (void) const	{ return error_code (_errno, system_category()); }
#endif
private:
    string		_operation;	///< Name of the failed operation.
    int			_errno;		///< Error code returned by the failed operation.
};

typedef system_error libc_exception;

/// \class file_exception uexception.h ustl.h
/// \ingroup Exceptions
///
/// \brief File-related exceptions.
///
/// Contains the file name. This is a uSTL extension.
///
class file_exception : public system_error {
public:
			file_exception (const char* operation, const char* filename) noexcept;
    inline virtual const char* what (void) const noexcept override { return "file error"; }
    virtual void	info (string& msgbuf, const char* fmt = nullptr) const noexcept override;
    virtual void	read (istream& is) override;
    virtual void	write (ostream& os) const override;
    virtual size_t	stream_size (void) const noexcept override;
    inline const char*	Filename (void) const	{ return _filename; }
private:
    char		_filename [PATH_MAX];	///< Name of the file causing the error.
};

/// \class stream_bounds_exception uexception.h ustl.h
/// \ingroup Exceptions
///
/// \brief Stream bounds checking.
///
/// Only thrown in debug builds unless you say otherwise in config.h
/// This is a uSTL extension.
///
class stream_bounds_exception : public system_error {
public:
			stream_bounds_exception (const char* operation, const char* type, uoff_t offset, size_t expected, size_t remaining) noexcept;
    inline virtual const char*	what (void) const noexcept override { return "stream bounds exception"; }
    virtual void	info (string& msgbuf, const char* fmt = nullptr) const noexcept override;
    virtual void	read (istream& is) override;
    virtual void	write (ostream& os) const override;
    virtual size_t	stream_size (void) const noexcept override;
    inline const char*	TypeName (void) const	{ return _typeName; }
    inline uoff_t	Offset (void) const	{ return _offset; }
    inline size_t	Expected (void) const	{ return _expected; }
    inline size_t	Remaining (void) const	{ return _remaining; }
private:
    const char*		_typeName;
    uoff_t		_offset;
    size_t		_expected;
    size_t		_remaining;
};

} // namespace ustl

#if HAVE_CPP11

//{{{ memory_order -----------------------------------------------------

namespace ustl {

enum memory_order {
    memory_order_relaxed = __ATOMIC_RELAXED,
    memory_order_consume = __ATOMIC_CONSUME,
    memory_order_acquire = __ATOMIC_ACQUIRE,
    memory_order_release = __ATOMIC_RELEASE,
    memory_order_acq_rel = __ATOMIC_ACQ_REL,
    memory_order_seq_cst = __ATOMIC_SEQ_CST
};

//}}}-------------------------------------------------------------------
//{{{ atomic

template <typename T>
class atomic {
    T			_v;
public:
			atomic (void) = default;
    inline constexpr	atomic (T v) : _v(v) {}
			atomic (const atomic&) = delete;
    atomic&		operator= (const atomic&) = delete;
    inline bool		is_lock_free (void) const
			    { return __atomic_is_lock_free (sizeof(T), &_v); }
    inline void		store (T v, memory_order order = memory_order_seq_cst)
			    { __atomic_store_n (&_v, v, order); }
    inline T		load (memory_order order = memory_order_seq_cst) const
			    { return __atomic_load_n (&_v, order); }
    inline T		exchange (T v, memory_order order = memory_order_seq_cst)
			    { return __atomic_exchange_n (&_v, v, order); }
    inline bool		compare_exchange_weak (T& expected, T desired, memory_order order = memory_order_seq_cst)
			    { return __atomic_compare_exchange_n (&_v, &expected, desired, true, order, order); }
    inline bool		compare_exchange_weak (T& expected, T desired, memory_order success, memory_order failure)
			    { return __atomic_compare_exchange_n (&_v, &expected, desired, true, success, failure); }
    inline bool		compare_exchange_strong (T& expected, T desired, memory_order success, memory_order failure)
			    { return __atomic_compare_exchange_n (&_v, &expected, desired, false, success, failure); }
    inline T		fetch_add (T v, memory_order order = memory_order_seq_cst )
			    { return __atomic_fetch_add (&_v, v, order); }
    inline T		fetch_sub (T v, memory_order order = memory_order_seq_cst )
			    { return __atomic_fetch_sub (&_v, v, order); }
    inline T		fetch_and (T v, memory_order order = memory_order_seq_cst )
			    { return __atomic_fetch_and (&_v, v, order); }
    inline T		fetch_or (T v, memory_order order = memory_order_seq_cst )
			    { return __atomic_fetch_or (&_v, v, order); }
    inline T		fetch_xor (T v, memory_order order = memory_order_seq_cst )
			    { return __atomic_fetch_xor (&_v, v, order); }
    inline T		add_fetch (T v, memory_order order = memory_order_seq_cst )
			    { return __atomic_add_fetch (&_v, v, order); }
    inline T		sub_fetch (T v, memory_order order = memory_order_seq_cst )
			    { return __atomic_sub_fetch (&_v, v, order); }
    inline T		and_fetch (T v, memory_order order = memory_order_seq_cst )
			    { return __atomic_and_fetch (&_v, v, order); }
    inline T		or_fetch (T v, memory_order order = memory_order_seq_cst )
			    { return __atomic_or_fetch (&_v, v, order); }
    inline T		xor_fetch (T v, memory_order order = memory_order_seq_cst )
			    { return __atomic_xor_fetch (&_v, v, order); }
    inline		operator T (void) const	{ return load(); }
    inline T		operator= (T v)		{ store(v); return v; }
    inline T		operator++ (int)	{ return fetch_add (1); }
    inline T		operator-- (int)	{ return fetch_sub (1); }
    inline T		operator++ (void)	{ return add_fetch (1); }
    inline T		operator-- (void)	{ return sub_fetch (1); }
    inline T		operator+= (T v)	{ return add_fetch (v); }
    inline T		operator-= (T v)	{ return sub_fetch (v); }
    inline T		operator&= (T v)	{ return and_fetch (v); }
    inline T		operator|= (T v)	{ return  or_fetch (v); }
    inline T		operator^= (T v)	{ return xor_fetch (v); }
};
#define ATOMIC_VAR_INIT	{0}

//}}}-------------------------------------------------------------------
//{{{ atomic_flag

class atomic_flag {
    bool		_v;
public:
			atomic_flag (void) = default;
    inline constexpr	atomic_flag (bool v)	: _v(v) {}
			atomic_flag (const atomic_flag&) = delete;
    atomic_flag&	operator= (const atomic_flag&) = delete;
    void		clear (memory_order order = memory_order_seq_cst)
			    { __atomic_clear (&_v, order); }
    bool		test_and_set (memory_order order = memory_order_seq_cst)
			    { return __atomic_test_and_set (&_v, order); }
};
#define ATOMIC_FLAG_INIT	{false}

//}}}-------------------------------------------------------------------
//{{{ fence functions

namespace {

template <typename T>
static inline T kill_dependency (T v) noexcept
    { T r (v); return r; }
static inline void atomic_thread_fence (memory_order order) noexcept
    { __atomic_thread_fence (order); }
static inline void atomic_signal_fence (memory_order order) noexcept
    { __atomic_signal_fence (order); }

} // namespace
} // namespace ustl
#endif // HAVE_CPP11
//}}}-------------------------------------------------------------------

/// \mainpage
///
/// \section intro Introduction
///
/// uSTL is a partial implementation of the STL specification intended to
/// reduce code size of the derivative programs. Usually, the STL containers
/// manage their own storage with new[] and delete[] operators, which create
/// strongly typed storage. That is the standard way of allocating C++ object
/// vectors, allowing appropriate constructors and destructors to be called on
/// the allocated storage and ensuring that objects are copied via their copy
/// operators. Although type safety is a good thing, placing memory management
/// code into a template necessitates its reinstantiation for every template
/// instance used by the derivative program. This produces substantial code
/// bloat, that is frequently derided by C developers and used by them as
/// an argument that C is better than C++. The uSTL implementation solves
/// this problem by factoring memory management code into a non-template base
/// class, ustl::memblock, which performs unstructured memory allocation. STL
/// containers are then implemented as template wrappers for memblock to
/// provide a measure of type safety. The result is that each template
/// instantiation contains less code, and although it does not completely
/// "disappear", due to the requirement for calling placement constructors
/// on the allocated memory, most of it does, being replaced by calls to
/// memblock methods. The base classes for unstructured storage management
/// (cmemlink - link to constant memory, memlink - link to mutable memory,
/// and memblock - owner of mutable memory) are, of course, also available
/// for use as data buffers wherever those are needed, and streams that
/// efficiently read and write binary data into them are also available.
//
/// \defgroup Containers Containers
/// Here you'll find all the containers for your objects and data.
//
///	\defgroup MemoryManagement Memory Management
///	\ingroup Containers
///	Classes that implement low-level memory management and form the base for
///	all containers in the library. Almost all functionality in the containers
///	is reduced to calls to these base classes through a great deal of inline
///	crunching by the compiler, and thus you end up storing all your data in
///	ustl::memblock objects with the container templates as mere syntactic sugar.
//
///	\defgroup Sequences Sequence Containers
///	\ingroup Containers
///	Containers containing sequences of objects.
//
///	\defgroup AssociativeContainers Associative Containers
///	\ingroup Containers
///	Containers containing associations of objects.
//
/// \defgroup Streams Streams
/// Streams convert objects into flat data.
//
/// 	\defgroup BinaryStreams Binary Streams
///	\ingroup Streams
///	Unlike the C++ standard library,
///	the default behaviour is very strongly biased toward binary streams. I
///	believe that text formats should be used very sparingly due to numerous
///	problems they cause, such as total lack of structure, buffer overflows,
///	the great multitude of formats and encodings for even the most
///	trivial of things like integers, and the utter lack of readability
///	despite ardent claims to the contrary. Binary formats are well-structured,
///	are simpler to define exhaustively, are aggregates of basic types which
///	are universal to all architectures (with the exception of two types of
///	byte ordering, which I hope to be an issue that will go away soon), and
///	are much more readable (through an appropriate formatting tool equipped
///	to read binary format specifications).
//
///		\defgroup BinaryStreamIterators Binary Stream Iterators
///		\ingroup BinaryStreams
///		\ingroup Iterators
///		Iterators for using STL algorithms with binary streams.
//
///	\defgroup TextStreams TextStreams
///	\ingroup Streams
///	Streams converting objects into streams of text.
//
///		\defgroup DeviceStreams Device Streams
///		\ingroup Streams
///		Standard cout, cerr, and cin implementations for reading
///		and writing text through standard file descriptors.
//
/// \defgroup Iterators Iterators
/// Generalizations of the pointer concept, allowing algorithms to treat
/// all containers in a unified fashion.
//
///	\defgroup IteratorAdaptors Iterator Adaptors
///	\ingroup Iterators
///	Iterators made out of other iterators.
//
/// \defgroup Algorithms Algorithms
/// STL algorithms are the heart of generic programming. The idea is to
/// separate algorithms from containers to take advantage of the fact that
/// there are fewer distinct algorithms than typed containers. This is
/// diametrically opposed to object oriented programming, where each object
/// must contain all functionality related to its internal data. You will
/// find, I think, that in practice, generic programming is not terribly
/// convenient because it prevents you from encapsulating all your data.
/// The best approach is to compromise and have raw data classes that will
/// be manipulated by algorithms and to treat the rest of the objects as
/// stateful data transformers.
//
///	\defgroup MutatingAlgorithms Mutating Algorithms
///	\ingroup Algorithms
///	Algorithms for modifying your data in some way.
//
///		\defgroup SortingAlgorithms Sorting Algorithms
///		\ingroup MutatingAlgorithms
///		Algorithms for sorting containers.
//
///		\defgroup GeneratorAlgorithms Generator Algorithms
///		\ingroup MutatingAlgorithms
///		Algorithms for generating data.
//
///		\defgroup NumericAlgorithms Numeric Algorithms
///		\ingroup MutatingAlgorithms
///		Algorithms generalizing mathematical operations.
//
///		\defgroup SetAlgorithms Set Algorithms
///		\ingroup MutatingAlgorithms
///		Algorithms for working with sorted sets.
//
///		\defgroup HeapAlgorithms Heap Algorithms
///		\ingroup MutatingAlgorithms
///		Algorithms for generating and manipulating heaps.
//
///		\defgroup SwapAlgorithms Swap Algorithms
///		\ingroup MutatingAlgorithms
///		Algorithms for swapping elements.
//
///		\defgroup RawStorageAlgorithms Raw Storage Algorithms
///		\ingroup MutatingAlgorithms
///		Algorithms for manipulating unstructured memory.
//
///	\defgroup ConditionAlgorithms Condition Algorithms
///	\ingroup Algorithms
///	Algorithms for obtaining information about data.
//
///		\defgroup SearchingAlgorithms Searching Algorithms
///		\ingroup ConditionAlgorithms
///		Algorithms for searching through containers.
//
///	\defgroup PredicateAlgorithms Predicate Algorithms
///	\ingroup Algorithms
///	Algorithms that take a functor object. Avoid these if you can,
///	and carefully check the generated assembly if you can't. These
///	algorithms can and will generate prodigious amounts of bloat
///	if you are not very very careful about writing your functors.
//
/// \defgroup Functors Functors
/// Functors are inteded to be passed as arguments to \link PredicateAlgorithms
/// predicate algorithms\endlink. Ivory tower academics make much of this capability,
/// no doubt happy that C++ can now be made to look just like their precious lisp.
/// In practice, however, functors and predicate algorithms are mostly useless.
/// An iterative solution using \ref foreach is usually far simpler to write
/// and to maintain. Furthermore, functional programming in C++ often
/// generates much bloat and slowness, which is difficult to avoid with any
/// but the most primitive functors. Try them if you wish, now and then, but
/// compare with an iterative solution to see if the compiler really can see
/// through all your functional trickery.
//
///	\defgroup FunctorObjects Functor Objects
///	\ingroup Functors
///	Objects that wrap other functors to provide new functionality.
//
///	\defgroup FunctorAccessors Functor Object Accessors
///	\ingroup Functors
///	Because C++ is so very unsuited to functional programming, trying
///	to do so may require a lot of typing. These accessor functions
///	are somewhat helpful in making functional constructs more readable.


