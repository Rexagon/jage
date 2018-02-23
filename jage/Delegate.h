#pragma once

#include <functional>
#include <tuple>

template<class T>
class Delegate
{
public:
	virtual T call() = 0;
};

// Function with delegate call
// Explain: you create a function, pass there parameters, but call after some time
template <class T, class... Args>
class FunctionDelegate : Delegate<T>
{
	// Helper functions
	template <int... Is>
	struct index {};

	template <int N, int... Is>
	struct gen_seq : gen_seq<N - 1, N - 1, Is...> {};

	template <int... Is>
	struct gen_seq<0, Is...> : index<Is...> {};

public:
	FunctionDelegate(std::function<T(Args...)> action, Args&&... args) :
		m_action(action), m_arguments(std::forward<Args>(args)...)
	{
	}

	// Calls function with arguments, which were specified befor
	T call() override
	{
		return call(gen_seq<sizeof...(Args)>{});
	}

	// Updates function arguments values
	void updateArguments(Args&&... args) {
		m_arguments = { std::forward<Args>(args)... };
	}

private:
	template <int... Is>
	T call(index<Is...>)
	{
		return m_action(std::get<Is>(m_arguments)...);
	}

	std::function<T(Args...)> m_action;
	std::tuple<Args...> m_arguments;
};
