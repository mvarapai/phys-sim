/*****************************************************************//**
 * \file   IEnumerable.h
 * \brief  The purpose of this header is to define an interface and macros
 *		   necessary for enumeration of children of a class.
 * 
 * \author Mikalai Varapai
 * \date   May 2025
 *********************************************************************/
#pragma once

#include <vector>
#include <memory>
#include <functional>

// Statically-enumerable class.
// T - the type of group class - the same as the child.
template<typename T>
class IEnumerableFactory
{
public:
	// Factory is a function type that return unique_ptr<T>
	using FactoryFunction = std::function<std::unique_ptr<T>()>;

	static std::vector<FactoryFunction>& factoryFunctionList()
	{
		static std::vector<Factory> factoryFunctions;
		return factoryFunctions;
	}

	static void addFactoryFunction(FactoryFunction factory)
	{
		factoryFunctionList().push_back(factory);
	}

	static std::vector<std::unique_ptr<T>> createAll()
	{
		std::vector<std::unique_ptr<T>> instances;
		for (const auto& factoryFunction : factoryFunctionList())
		{
			instances.push_back(factoryFunction());
		}
		return instances;
	}
};

// Registration Macro
#define REGISTER_CLASS_SIGNATURE(CLASS, GROUP)									\
	inline static bool _registered_##CLASS = []()								\
	{																			\
		GROUP::addFactoryFunction([]() { return std::make_unique<CLASS>(); });	\
		return true;															\
	}();
