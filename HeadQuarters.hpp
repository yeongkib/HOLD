/*****************************************************************************/
/*!
\file   HeadQuarters.hpp
\author Yeongki Baek
\par    email: yeongki.baek\@digipen.edu
\date   08/01/2017
\brief
This is the interface file for the module
Copyright 2017, Digipen Institute of Technology
*/
/*****************************************************************************/
#pragma once

namespace HOLD
{
	class Message;
	class Command;
	class StateManager;
	typedef std::unordered_map<size_t, Command*> CommandUMap;

	class HeadQuarters
	{
	public:
		~HeadQuarters();
		void Initialize();
		void Run();
		void Shutdown() const;

		void BroadcastMessage(Message* message) const;

		void SetActive(bool active) { m_Active = active; }
		static HeadQuarters* GetInstance();

		template<typename T>
		T* GetCommand() const
		{
			return static_cast<T*>((*m_CommandList.find(typeid(T).hash_code())).second);
		}
		template<typename T>
		void AttachCommander(T* system)
		{
			//todo : assert if it already exist
			m_CommandList[typeid(T).hash_code()] = system;
		}
		template<typename T>
		void DetachCommander(T* system)
		{
			m_CommandList.erase(typeid(T).hash_code());
		}

		void Init();

		void OnUnitMorph(Unit unit);

		unsigned long frame_count = 0;
		double last_time, current_time;

	private:
		CommandUMap m_CommandList;
		StateManager* m_StateManager;
		bool m_Active;
		float m_CurrentTime;

		//singleton
		static HeadQuarters* m_Instance;
		HeadQuarters();
	};
}