#pragma once
#include "Delegate.h"
#include <vector>

template<typename T>
class Event;

template<typename RetVal_t, typename ...Args_t>
class Event<RetVal_t(Args_t...)>
{
public:

	bool AddListener(Delegate<RetVal_t(Args_t...)> a_listener)
	{
		if (std::find(m_Listeners.begin(), m_Listeners.end(), a_listener) != m_Listeners.end())
		{
			return false;
		}

		m_Listeners.push_back(a_listener);

		return true;
	}

	template <typename T, RetVal_t(T:: * pFunc)(Args_t...)>
	bool AddListener(T* a_pOwner)
	{
		Delegate< RetVal_t(Args_t...)> del = Delegate< RetVal_t(Args_t...)>::template Bind<T, pFunc>(a_pOwner);

		return AddListener(del);
	}

	bool RemoveListener(Delegate<RetVal_t(Args_t...)> a_listener)
	{
		auto it = std::find(m_Listeners.begin(), m_Listeners.end(), a_listener);
		if (it == m_Listeners.end())
		{
			return false;
		}

		m_Listeners.erase(it);
		return true;
	}

	template <typename T, RetVal_t(T:: * pFunc)(Args_t...)>
	bool RemoveListener(T* a_pOwner)
	{
		Delegate< RetVal_t(Args_t...)> del = Delegate< RetVal_t(Args_t...)>::template Bind<T, pFunc>(a_pOwner);

		return RemoveListener(del);
	}

	bool Contains(Delegate<RetVal_t(Args_t...)> a_listener)
	{
		return std::find(m_Listeners.begin(), m_Listeners.end(), a_listener) != m_Listeners.end();
	}

	void operator()(Args_t... a_args)
	{
		for(auto& listener : m_Listeners)
		{
			listener(a_args...);
		}
	}

	void Raise(Args_t... a_args)
	{
		for (auto& listener : m_Listeners)
		{
			listener(a_args...);
		}
	}

private:
	std::vector<Delegate<RetVal_t(Args_t...)>> m_Listeners;
};

#define INTERNAL_DECLARE_EVENT(name, ...) typedef Event<__VA_ARGS__> name

#define DECLARE_EVENT(name, Ret) INTERNAL_DECLARE_EVENT(name, Ret())
#define DECLARE_EVENT_ONE_PARAM(name, Ret, Param1) INTERNAL_DECLARE_EVENT(name, Ret(Param1))
#define DECLARE_EVENT_TWO_PARAM(name, Ret, Param1, Param2) INTERNAL_DECLARE_EVENT(name, Ret(Param1, Param2))
#define DECLARE_EVENT_THREE_PARAM(name, Ret, Param1, Param2, Param3) INTERNAL_DECLARE_EVENT(name, Ret(Param1, Param2, Param3))