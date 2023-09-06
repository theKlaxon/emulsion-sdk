//#pragma once
#include <Windows.h>
#include <tuple>
#include <functional>
#include <tuple>

template <typename t>
struct FuncArgs;

template<typename rt, typename s, typename ... a>
struct FuncArgs<rt(s::*)(a...)> {
	using rtype = rt;

};

template<typename t>
struct FArgs;

template <typename rt, typename s, typename ... a>
struct FArgs<rt(s::*)(a...)> {
	using rtype = rt;

	//template<template<typename...> typename Tup>
	typedef a args;
};

//int mmmm() {
//	using type = typename FArgs<>::type;
//}

#define D_CONCAT(A, B) A ## B 

#define SLIB_DEFINE(name)	\
HMODULE scopedlib = NULL;	\
const char* dllname = name;	\

// ENSURE THIS IS SCOPED PROPERLY,
//otherwise some fuckshits gonna happen
#define SLIB_LOAD()				\
scopedlib = LoadLibraryA(dllname);	\

#define SLIB_FLOAD(callname, name)										\
if(scopedlib != nullptr)												\
	callname = (D_CONCAT(P_, callname))GetProcAddress(scopedlib, name);	\

#define SLIB_MOCK(callname, func)																				\
typedef FuncArgs<decltype(func)>::rtype(__cdecl* D_CONCAT(P_, callname))(FuncArgs<decltype(func)>::args ...);	\
D_CONCAT(P_, callname) callname;																				\













#define SLIB_FTYPE(name, rtype, params)	\
typedef rtype(__cdecl *name)params;			\
