/*****************************************************************************/
/*!
\file   Command.hpp
\author Yeongki Baek
\par    email: yeongki.baek\@digipen.edu
\date   08/01/2017
\brief
This is the interface file for the module
Copyright 2017, Digipen Institute of Technology
*/
/*****************************************************************************/

#pragma once

#include "Message.hpp"

namespace HOLD
{
	class Command
	{
	public:
		virtual ~Command() = default;
		virtual void Init() = 0;
		virtual void Update(int dt) = 0;
		virtual void Shutdown() = 0;
		virtual void ProcessMessage(Message* message) = 0;
		virtual void OnUnitMorph(Unit unit) = 0;
		virtual void OnUnitCreate(Unit unit) = 0;
		virtual void OnUnitShow(Unit unit) = 0;
		virtual void OnUnitDestroy(Unit unit) = 0;
	};
}//namespace HOLD