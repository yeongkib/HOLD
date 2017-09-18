/*****************************************************************************/
/*!
\file   HeadQuarters.cpp
\author Yeongki Baek
\par    email: yeongki.baek\@digipen.edu
\par    GAM400
\date   08/01/2017
\brief
This is the interface file for the module
Copyright 2017, Digipen Institute of Technology
*/
/*****************************************************************************/

#include "Precompiled.hpp"

#include "HeadQuarters.hpp"
#include "Command.hpp"


namespace HOLD
{
	HeadQuarters* HeadQuarters::m_Instance = nullptr;
	

	HeadQuarters::HeadQuarters() : m_StateManager(nullptr)
	{
		m_Active = true;
		m_CurrentTime = 0.f;
	}
	
	HeadQuarters::~HeadQuarters()
	{
		for (auto & it : m_CommandList)
		{
			delete it.second;
		}
	}
	void HeadQuarters::Initialize()
	{
		START();

		//start = std::chrono::system_clock::now();

		Init();

		
		for each(auto& it in m_CommandList)
		{
			it.second->Init();
		}
		
	}
	void HeadQuarters::Run(void)
	{
		for each(auto& it in m_CommandList)
		{
			it.second->Update(0);
		}
		/*float startTime = static_cast<float>(glfwGetTime());
		float dt = 0.f;
		while (!m_Active)
		{
			m_currentTime = static_cast<float>(glfwGetTime());

			dt = m_currentTime - startTime;

			startTime = m_currentTime;

			for each(auto& it in m_systemList)
			{
				it.pSystem->Update(dt);
			}

			++frame_count;

#ifdef _DEBUG
			current_time = glfwGetTime();
			if (current_time - last_time > 1.0)
			{
				frame_rate = frame_count / (current_time - last_time);
				frame_count = 0;
				last_time = current_time;
				update_window_title(m_window);
			}
#endif
		}*/

	}

	void HeadQuarters::Shutdown() const
	{
		for each(auto& it in m_CommandList)
		{
			it.second->Shutdown();
		}
	}

	HeadQuarters* HeadQuarters::GetInstance()
	{
		if (!m_Instance)
			m_Instance = new HeadQuarters();
		return m_Instance;
	}

	void HeadQuarters::BroadcastMessage(Message* message) const
	{
		for each(auto& it in m_CommandList)
		{
			it.second->ProcessMessage(message);
		}
	}

	void HeadQuarters::Init()
	{
		
	}

	void HeadQuarters::OnUnitMorph(Unit unit)
	{
		for each(auto& it in m_CommandList)
		{
			it.second->OnUnitMorph(unit);
		}
	}
}//namespace HOLD
