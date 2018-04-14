/*****************************************************************************/
/*!
\file   Message.hpp
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
	namespace MsgId
	{
		enum EMessageType
		{
			NONE,
			TOTAL_TYPE
		};
	}//namespace MsgId
	class Message
	{
	public:
		Message(MsgId::EMessageType id) : MessageId(id) {};
		MsgId::EMessageType MessageId;
		virtual ~Message() = default;
	};
}//namespace HOLD