/*
 * sysmodule.h - wrapper around vita sysmodule facility
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef SYSMODULE_H
#define SYSMODULE_H

#include <memory>
#include <mutex>
#include <unordered_map>

namespace SysModule
{
	class Module
	{
		friend class Arbiter;

	private:
		Module(int id);
	
		template <typename... Args>
		Module(int id, Args... args)
			: m_id(id)
		{
			unsigned int argArray = { args... };
			initWithArg(&argArray, sizeof(unsigned int)*sizeof...(args));
		}

	public:
		~Module(void);

	private:
		void initWithArg(void *data, unsigned int size);

	private:
		int m_id;
	};

	class Arbiter
	{
	public:
		static std::shared_ptr<Module> load(int id)
		{
			return instance()->loadModule(id);
		}

		template <typename... Args>
		static std::shared_ptr<Module> load(int id, Args... args)
		{
			return instance()->loadModule(id, args...);
		}
		
	private:
		std::shared_ptr<Module> loadModule(int id)
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			auto it = m_moduleList.find(id);
		
			if (it != m_moduleList.end())
			{
				if (auto module = it->second.lock())
					return module;
			}

			std::shared_ptr<Module> module(new Module(id));
			m_moduleList[id] = module;
			return module;
		}
		
		template <typename... Args>
		std::shared_ptr<Module> loadModule(int id, Args... args)
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			auto it = m_moduleList.find(id);
		
			if (it != m_moduleList.end())
			{
				if (auto module = it->second.lock())
					return module;
			}

			std::shared_ptr<Module> module(new Module(id, args...));
			m_moduleList[id] = module;
			return module;
		}

	private:
		Arbiter(void) = default;

		static Arbiter *instance(void)
		{
			static Arbiter s;
			return &s;
		}

	private:
		std::unordered_map<int, std::weak_ptr<SysModule::Module>> m_moduleList;
		std::mutex m_mutex;
	};

	class ScopedModule
	{
	public:
		ScopedModule(int id)
		{
			m_module = Arbiter::load(id);
		}

		template <typename... Args>
		ScopedModule(int id, Args... args)
		{
			m_module = Arbiter::load(id, args...);
		}

	private:
		std::shared_ptr<Module> m_module;
	};
} // SysModule namespace

#endif // SYSMODULE_H
