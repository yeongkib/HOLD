/*****************************************************************************/
/*!
\file   LogisticsCommand.hpp
\author Yeongki Baek
\par    email: yeongki.baek\@digipen.edu
\par    GAM400
\date   08/01/2017
\brief
This is the interface file for the module
Copyright 2017, Digipen Institute of Technology
*/
/*****************************************************************************/
#pragma once

#include "Command.hpp"
#include "HeadQuarters.hpp"
#include <BWAPI/UnitCommandType.h>
#include <BWAPI/UnitCommand.h>

namespace HOLD
{
	//forward
	class Message;
	struct UnitDataSet;
		
	class LogisticsCommand : public Command
	{
	public:
		LogisticsCommand();
		virtual ~LogisticsCommand();

		void Init() override;
		void Update(int dt) override;
		void Shutdown() override;
		void ProcessMessage(Message* message) override;


		void BroadcastMessage(Message* message);
		void InitStrategyVsZ();
		void InitStrategyVsP();
		void InitStrategyVsT();

		bool Build(UnitCommand & mt);

		void AddOrder(const UnitType & ut, const int & count = 1, const int & priority = 0);
		void AddOrder(const UpgradeType& ut, const int& count = 1, const int& priority = 0);
		void AddOrder(const TechType& tt, const int& count = 1, const int& priority = 0);


		void ReserveResource(const UnitCommandType& uct = UnitCommandTypes::Unknown);
		void SpendResource(const UnitCommandType& uct = UnitCommandTypes::Unknown);

		void GetNextOrder();

		int GetCurrentOrderNum() const { return currentOrderNum; }

		void OnUnitMorph(Unit unit) override;
		void OnUnitCreate(Unit unit) override;
		void OnUnitShow(Unit unit) override;
		void OnUnitDestroy(Unit unit) override;

	private:

		std::deque<UnitCommand> buildorder;

		int m_idStack;
		int GenerateID();


		std::unordered_map< BWAPI::Player, std::unordered_map< int, HOLD::UnitDataSet > >* UnitDataSets;

		int currentOrderNum = 1;

		int reservedMineral = 0;
		int reservedGas = 0;
		int reservedSupply = 0;
	};
}
