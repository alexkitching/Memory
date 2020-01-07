#pragma once
#include "Debug.h"
#include <set>

template<typename T>
class Delegate;

template<typename RetVal_t, typename ...Args>
class Delegate<RetVal_t(Args...)>
{
	using Callback_t = RetVal_t(*)(void*, Args...);
public:
	template<RetVal_t(*pFunc)(Args...)> // Func Ptr Template
	static Delegate Bind() // Bind Global/Static Function Delegate
	{
		return Delegate(nullptr, // nullptr as first parameter since static/global function
						&GlobalCaller<pFunc>); // Get Ptr of Templated Global Function
	}

	template<typename T, RetVal_t(T::*pFunc)(Args...)>
	static Delegate Bind(T* a_pOwner) // Bind Member Function Delegate
	{
		ASSERT(a_pOwner != nullptr && "a_pOwner is Null!");
		return Delegate(a_pOwner,
			&MemberCaller<T, pFunc>); // Get Ptr of Templated Member Function
	}
	
	RetVal_t operator()(Args... args)
	{
		return m_Callback(m_pOwner, args...); // Call Delegate Callback
	}

	bool operator==(const Delegate& other)
	{
		return m_pOwner == other.m_pOwner && m_Callback == other.m_Callback;
	}

private:
	Delegate(void* a_pOwner, Callback_t a_pFunc)
		:
		m_pOwner(a_pOwner),
		m_Callback(a_pFunc)
	{
	}

	template<RetVal_t(*pFunc)(Args...)>
	static RetVal_t GlobalCaller(void*, Args... a_args) // Templated Global/Static Function 
	{
		return pFunc(a_args...); // Call Function
	}

	template<typename T, RetVal_t(T::*pFunc)(Args...)>
	static RetVal_t MemberCaller(void* a_pCallee, Args... a_args) // Templated Member Function
	{
		return (static_cast<T*>(a_pCallee)->*pFunc)(a_args...); // Call Member Function
	}
	
	void* m_pOwner;
	Callback_t m_Callback;
};


#define INTERNAL_DECLARE_DELEGATE(name, ...) typedef Delegate<__VA_ARGS__> name

#define DECLARE_DELEGATE(name, Ret) INTERNAL_DECLARE_DELEGATE(name, Ret())
#define DECLARE_DELEGATE_ONE_PARAM(name, Ret, Param1) INTERNAL_DECLARE_DELEGATE(name, Ret(Param1))
#define DECLARE_DELEGATE_TWO_PARAM(name, Ret, Param1, Param2) INTERNAL_DECLARE_DELEGATE(name, Ret(Param1, Param2))
#define DECLARE_DELEGATE_THREE_PARAM(name, Ret, Param1, Param2, Param3) INTERNAL_DECLARE_DELEGATE(name, Ret(Param1, Param2, Param3))
