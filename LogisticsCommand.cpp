/*****************************************************************************/
/*!
\file   LogisticsCommand.cpp
\author Yeongki Baek
\par    email: yeongki.baek\@digipen.edu
\date   08/01/2017
\brief
This is the interface file for the module
Copyright 2017, Digipen Institute of Technology
*/
/*****************************************************************************/

#include "Precompiled.hpp"
#include "IntelligenceCommand.hpp"
#include "LogisticsCommand.hpp"
#include "../BWEM_1_3/bwem.h"
#include <BWAPI/UnitCommandType.h>
#include <BWAPI/UnitCommand.h>

namespace { auto & theMap = BWEM::Map::Instance(); }
using namespace Filter;

namespace HOLD
{
	LogisticsCommand::LogisticsCommand() : m_idStack(0), currentOrderNum(0)
	{
	}

	void LogisticsCommand::Init()
	{
		Units = HeadQuarters::GetInstance()->GetCommand<IntelligenceCommand>()->GetUnitDataSets();

		//todo : get enemy's race if possible
		if (Broodwar->enemy())
		{
			EmptyStrategy(); return;
			if (Broodwar->enemy()->getRace() == Races::Zerg)
				InitStrategyVsZ();

			else if (Broodwar->enemy()->getRace() == Races::Protoss)
				InitStrategyVs12pool();
				//InitStrategyVsP();

			else if (Broodwar->enemy()->getRace() == Races::Terran)
				InitStrategyVsT();

			else //if(==unknown)
			{
				InitStrategyVsT();
			}
		}
	}

	void LogisticsCommand::Update(int dt)
	{
		GetNextOrder();

		switch (buildorder.front().getType())
		{
		case UnitCommandTypes::Build:
			// falls through
		case UnitCommandTypes::Morph:
			{
				Broodwar->drawTextScreen(5, 0, "current: %d - %s", currentOrderNum, buildorder.front().getUnitType().c_str());
				Broodwar->drawTextScreen(5, 10, "Mineral : %d ", buildorder.front().getUnitType().mineralPrice());
				Broodwar->drawTextScreen(5, 20, "Gas : %d ", buildorder.front().getUnitType().gasPrice());
				Broodwar->drawTextScreen(5, 30, "Supply : %d ", buildorder.front().getUnitType().supplyRequired());
			}
			break;
		case UnitCommandTypes::Upgrade:
			{
				Broodwar->drawTextScreen(5, 0, "current: %d - %s", currentOrderNum, buildorder.front().getUpgradeType().c_str());
				Broodwar->drawTextScreen(5, 10, "Mineral : %d ", buildorder.front().getUpgradeType().mineralPrice());
				Broodwar->drawTextScreen(5, 20, "Gas : %d ", buildorder.front().getUpgradeType().gasPrice());
			}
			break;
		case UnitCommandTypes::Research:
			{
				Broodwar->drawTextScreen(5, 0, "current: %d - %s", currentOrderNum, buildorder.front().getTechType().c_str());
				Broodwar->drawTextScreen(5, 10, "Mineral : %d ", buildorder.front().getTechType().mineralPrice());
				Broodwar->drawTextScreen(5, 20, "Gas : %d ", buildorder.front().getTechType().gasPrice());
			}
			break;
		}

		Broodwar->drawTextScreen(150, 10, "reservedMineal : %d ", reservedMineral);
		Broodwar->drawTextScreen(150, 20, "Gas : %d ", reservedGas);
		Broodwar->drawTextScreen(150, 30, "Supply : %d ", reservedSupply);

		//fix drone to unknown : when it build extractor...
	}

	void LogisticsCommand::Shutdown()
	{
	}

	void LogisticsCommand::ProcessMessage(Message* message)
	{
	}

	void LogisticsCommand::BroadcastMessage(Message* message)
	{
	}

	void LogisticsCommand::InitStrategyVs12pool()
	{
		strat = pool12;
		AddOrder(UnitTypes::Zerg_Drone, 5);
		AddOrder(UnitTypes::Zerg_Overlord);
		AddOrder(UnitTypes::Zerg_Drone, 3);
		AddOrder(UnitTypes::Zerg_Hatchery);
		AddOrder(UnitTypes::Zerg_Spawning_Pool);
		AddOrder(UnitTypes::Zerg_Drone, 2);
		AddOrder(UnitTypes::Zerg_Hatchery);
		AddOrder(UnitTypes::Zerg_Extractor);
		AddOrder(UnitTypes::Zerg_Drone, 2);
		AddOrder(UnitTypes::Zerg_Zergling);
		AddOrder(UnitTypes::Zerg_Drone);
		AddOrder(UnitTypes::Zerg_Zergling);
		AddOrder(UnitTypes::Zerg_Drone);
		AddOrder(UnitTypes::Zerg_Zergling);
		AddOrder(UnitTypes::Zerg_Lair);
		AddOrder(UnitTypes::Zerg_Overlord);
		AddOrder(UnitTypes::Zerg_Drone, 2);
		AddOrder(UpgradeTypes::Metabolic_Boost);
		AddOrder(UnitTypes::Zerg_Extractor);
		AddOrder(UnitTypes::Zerg_Zergling, 3);
		AddOrder(UnitTypes::Zerg_Overlord);
		AddOrder(UnitTypes::Zerg_Drone, 4);
		AddOrder(UnitTypes::Zerg_Spire);
		AddOrder(UnitTypes::Zerg_Drone, 4);
		AddOrder(UnitTypes::Zerg_Overlord, 3);
		AddOrder(UnitTypes::Zerg_Mutalisk, 11);
		AddOrder(UnitTypes::Zerg_Hydralisk_Den);
		//AddOrder(TechTypes::Lurker_Aspect);
		//AddOrder(UpgradeTypes::Muscular_Augments); // hydra speed
		AddOrder(UpgradeTypes::Grooved_Spines); // hydra range
		AddOrder(UnitTypes::Zerg_Hydralisk, 3);
		AddOrder(UnitTypes::Zerg_Overlord, 3);
		AddOrder(UnitTypes::Zerg_Hydralisk, 3);
	}

	void LogisticsCommand::EmptyStrategy()
	{
		AddOrder(UnitTypes::Zerg_Drone, 1);
	}

	void LogisticsCommand::InitStrategyVsZ()
	{
		strat = pool5;
		AddOrder(UnitTypes::Zerg_Drone, 1);
		AddOrder(UnitTypes::Zerg_Spawning_Pool);
		AddOrder(UnitTypes::Zerg_Drone, 2);
		AddOrder(UnitTypes::Zerg_Zergling, 3);
		AddOrder(UnitTypes::Zerg_Overlord);
		AddOrder(UnitTypes::Zerg_Zergling, 3);
		AddOrder(UnitTypes::Zerg_Overlord);
		AddOrder(UnitTypes::Zerg_Overlord, 3);
		AddOrder(UnitTypes::Zerg_Zergling, 3);
	}

	void LogisticsCommand::InitStrategyVsP()
	{
		strat = pool5;
		AddOrder(UnitTypes::Zerg_Drone, 1);
		AddOrder(UnitTypes::Zerg_Spawning_Pool);
		AddOrder(UnitTypes::Zerg_Drone, 2);
		AddOrder(UnitTypes::Zerg_Zergling, 3);
		//add gas trick function
		//AddOrder(UnitTypes::Zerg_Zergling, bool ) // when we do gas trick, it should do it for only one unit
		AddOrder(UnitTypes::Zerg_Overlord);
		AddOrder(UnitTypes::Zerg_Zergling, 3);
		AddOrder(UnitTypes::Zerg_Overlord);
		AddOrder(UnitTypes::Zerg_Zergling, 3);
		AddOrder(UnitTypes::Zerg_Overlord, 3);
		AddOrder(UnitTypes::Zerg_Zergling, 3);
	}

	void LogisticsCommand::InitStrategyVsT()
	{
		strat = pool4;
		AddOrder(UnitTypes::Zerg_Drone, 1);
		AddOrder(UnitTypes::Zerg_Spawning_Pool);
		AddOrder(UnitTypes::Zerg_Drone, 1);
		AddOrder(UnitTypes::Zerg_Zergling, 3);
		AddOrder(UnitTypes::Zerg_Overlord);
		AddOrder(UnitTypes::Zerg_Zergling, 3);
		AddOrder(UnitTypes::Zerg_Overlord);
		AddOrder(UnitTypes::Zerg_Zergling, 3);
		AddOrder(UnitTypes::Zerg_Overlord, 3);
		AddOrder(UnitTypes::Zerg_Zergling, 3);
	}

	bool LogisticsCommand::Build(UnitCommand & mt)
	{
		switch (mt.getType())
		{
		case UnitCommandTypes::Build:
			// falls through
		case UnitCommandTypes::Morph:
			{
				if (mt.getUnitType() <= UnitTypes::Enum::Zerg_Lurker) // it's unit
				{
					//if (!MyUnitSets[BWAPI::UnitTypes::Enum::Zerg_Larva].m_units.empty())
					if (!(*Units)[Broodwar->self()][UnitTypes::Enum::Zerg_Larva].empty())
					{
						int minerals = BWAPI::Broodwar->self()->minerals();
						int requireMin = buildorder.front().getUnitType().mineralPrice();//mt.unitType.mineralPrice();

						int gas = BWAPI::Broodwar->self()->gas();
						int requireGas = buildorder.front().getUnitType().gasPrice();//mt.unitType.gasPrice();

						int supply = BWAPI::Broodwar->self()->supplyTotal() - BWAPI::Broodwar->self()->supplyUsed();
						int requireSupply = buildorder.front().getUnitType().supplyRequired();//mt.unitType.supplyRequired();

																							  //BWAPI::UnitCommand::train(*(MyUnitSets[BWAPI::UnitTypes::Zerg_Larva].m_units.begin()), mt.unitType);
						if (minerals >= requireMin
							&& gas >= requireGas
							&& supply >= requireSupply)
						{
							//if ((*MyUnitSets[BWAPI::UnitTypes::Enum::Zerg_Larva].m_units.begin())->issueCommand(BWAPI::UnitCommand::morph(nullptr, mt.getUnitType())))
							if (!(*Units)[Broodwar->self()][UnitTypes::Enum::Zerg_Larva].empty())
								if ((*begin((*Units)[Broodwar->self()][UnitTypes::Enum::Zerg_Larva]))->m_unit->issueCommand(BWAPI::UnitCommand::morph(nullptr, mt.getUnitType())))
								{
									/*reservedMineral -= requireMin;
									reservedGas -= requireGas;
									reservedSupply -= requireSupply;*/
									return true;
								}
							return false;
							/*BWAPI::Error err;
							if (!(*MyUnitSets[BWAPI::UnitTypes::Enum::Zerg_Larva].m_units.begin())->issueCommand(BWAPI::UnitCommand::morph(nullptr, mt.unitType)))
							{
							err = BWAPI::Broodwar->getLastError();

							}
							else
							return true;*/
						}
						return false;

						// todo : select desired(nearer to the position) larva from container, remove that larva from the container

					}
				}
				else if (UnitTypes::Enum::Zerg_Hatchery <= mt.getUnitType()
					  && mt.getUnitType() <= UnitTypes::Enum::Zerg_Extractor) // it's building
				{
					if ((*Units)[Broodwar->self()][BWAPI::UnitTypes::Zerg_Drone].empty())
						return false;

					int minerals = BWAPI::Broodwar->self()->minerals();
					int requireMin = buildorder.front().getUnitType().mineralPrice();//mt.unitType.mineralPrice();

					int gas = BWAPI::Broodwar->self()->gas();
					int requireGas = buildorder.front().getUnitType().gasPrice();//mt.unitType.gasPrice();

					if (minerals >= requireMin
					 && gas >= requireGas)
					{
						if (UnitTypes::Enum::Zerg_Hatchery == mt.getUnitType())
						{

							//for (auto & worker : MyUnitSets[BWAPI::UnitTypes::Zerg_Drone].m_units)
							//BWAPI::Unit supplyBuilder = *MyUnitSets[BWAPI::UnitTypes::Zerg_Drone].m_units.begin();
							auto supplyBuilder = *begin((*Units)[Broodwar->self()][BWAPI::UnitTypes::Zerg_Drone]);
							{
								// Retrieve a unit that is capable of constructing the supply needed
								/*BWAPI::Unit supplyBuilder = (worker)->getClosestUnit(BWAPI::Filter::GetType == mt.unitType.whatBuilds().first &&
								(BWAPI::Filter::IsIdle || BWAPI::Filter::IsGatheringMinerals) &&
								BWAPI::Filter::IsOwned);*/
								// If a unit was found
								if (supplyBuilder)
								{
									if ((*Units)[Broodwar->self()][UnitTypes::Enum::Zerg_Hatchery].size() < 2)
									{
										//const BWEM::Area * startBase = theMap.GetArea(Broodwar->self()->getStartLocation());
										TilePosition starting = Broodwar->self()->getStartLocation();

										static int asdfasdfasdf = 0;
										static int asdfasdf = 0;
										std::vector<const BWEM::Base*> candidates;

										for (const BWEM::Area & area : theMap.Areas())
										{
											for (const BWEM::Base & base : area.Bases())
											{
												if (std::find(Broodwar->getStartLocations().begin(), Broodwar->getStartLocations().end(), base.Location()) == Broodwar->getStartLocations().end())
												{
													candidates.emplace_back(&base);
												}
											}
										}

										BWAPI::TilePosition targetBuildLocation;
										int minLength = (std::numeric_limits<int>::max)();
										for (auto base : candidates)
										{
											if (Broodwar->hasPath(Position(starting.x * 32, starting.y * 32), base->Center()))
												if (!base->Minerals().empty() && !base->Geysers().empty())
													//if (base.Location() != starting)
												{
													int length;

													theMap.GetPath(base->Center(), Position(starting.x * 32, starting.y * 32), &length);
													//int length = Broodwar->self()->getStartLocation().getApproxDistance(TilePosition(base->Location()));

													if ((length > 0) && (length < minLength))
													{
														minLength = length;
														targetBuildLocation = base->Location();
													}
												}
										}


										if (targetBuildLocation != TilePositions::Invalid)
										{
											if (!Broodwar->isExplored(targetBuildLocation.x, targetBuildLocation.y))
												supplyBuilder->m_unit->issueCommand(BWAPI::UnitCommand::move(nullptr, Position(targetBuildLocation)));

											//// Register an event that draws the target build location
											//BWAPI::Broodwar->registerEvent([=](BWAPI::Game*)
											//{
											//	BWAPI::Broodwar->drawBoxMap(BWAPI::Position(targetBuildLocation),
											//		BWAPI::Position(targetBuildLocation + mt.getUnitType().tileSize()),
											//		BWAPI::Colors::Blue);
											//},
											//	[=](BWAPI::Game*) {return true; },  // condition
											//	mt.getUnitType().buildTime() + 500);  // frames to run
											BWAPI::Broodwar->registerEvent([=](BWAPI::Game*)
											{
												bool result = false;
												result = supplyBuilder->m_unit->build(mt.getUnitType(), targetBuildLocation);
												/*if (result == true)
												{
												reservedMineral -= mt.getUnitType().mineralPrice();
												reservedGas -= mt.getUnitType().gasPrice();
												}*/

												return result;
											},
												[=](BWAPI::Game*) {return !supplyBuilder->m_unit->isConstructing() || !supplyBuilder->m_unit->isMoving() || !supplyBuilder->m_unit->getType().isWorker(); },  // condition
												mt.getUnitType().buildTime() + 500,
												50);  // frames to run

													  // Order the builder to construct the supply structure
													  //return supplyBuilder->build(mt.getUnitType(), targetBuildLocation);
										}
									}
									else
									{
										BWAPI::TilePosition targetBuildLocation = BWAPI::Broodwar->getBuildLocation(mt.getUnitType(), supplyBuilder->m_unit->getTilePosition(), 256);
										if (targetBuildLocation != TilePositions::Invalid)
										{
											// Register an event that draws the target build location
											//BWAPI::Broodwar->registerEvent([=](BWAPI::Game*)
											//{
											//	BWAPI::Broodwar->drawBoxMap(BWAPI::Position(targetBuildLocation),
											//		BWAPI::Position(targetBuildLocation + mt.getUnitType().tileSize()),
											//		BWAPI::Colors::Blue);
											//},
											//	[=](BWAPI::Game*) {return (*Units)[Broodwar->self()][mt.getUnitType()].empty(); },  // condition
											//	mt.getUnitType().buildTime() + 100);  // frames to run

																					  // Order the builder to construct the supply structure

											bool result = supplyBuilder->m_unit->build(mt.getUnitType(), targetBuildLocation);
											if (result == true)
											{
												/*reservedMineral -= mt.getUnitType().mineralPrice();
												reservedGas -= mt.getUnitType().gasPrice();*/
											}

											return result;
											/*if( supplyBuilder->build(mt.unitType, targetBuildLocation))
											{
											if (supplyBuilder->isMorphing())
											{
											MyUnitSets[BWAPI::UnitTypes::Zerg_Drone].m_units.erase(supplyBuilder);
											return true;
											}
											}
											return false;*/
											//try {
											//	return supplyBuilder->build(mt.unitType, targetBuildLocation);
											//	Error last = Broodwar->getLastError();
											//	int i = 0;
											//}
											//catch (const std::exception & e)
											//{
											//	Broodwar << "EXCEPTION from " << __func__ << ": " << e.what() << std::endl;
											//}
										}
									}
									return false;
								} // closure: supplyBuilder is valid
							}
						}

						if (UnitTypes::Enum::Zerg_Extractor == mt.getUnitType())
						{
							if ((*Units)[Broodwar->self()][UnitTypes::Enum::Zerg_Extractor].size() == 1)
							{
								auto starting = theMap.GetArea(Broodwar->self()->getStartLocation());
								for (auto base : (*Units)[Broodwar->self()][UnitTypes::Enum::Zerg_Hatchery])
								{
									auto currentBase = theMap.GetArea(base->m_unit->getTilePosition());
									if (starting != currentBase)
									{
										for (auto gas : currentBase->Geysers())
										{
											BWAPI::TilePosition targetBuildLocation = gas->TopLeft();
											if (targetBuildLocation != TilePositions::Invalid)
											{
												Unit supplyBuilder = Broodwar->getClosestUnit(Position(targetBuildLocation), IsWorker);
												if (supplyBuilder)
												{
													bool result = supplyBuilder->build(mt.getUnitType(), targetBuildLocation);
													if (result)
													{
														//reservedMineral -= mt.getUnitType().mineralPrice();
													}
													return result;
												}
												else
													return false;
											}
										}
									}
								}
							}
						}
						// to do : select desired one
						if (mt.getUnitType() == BWAPI::UnitTypes::Enum::Zerg_Lair)
						{
							bool result = false;
							for (auto hatchery : (*Units)[Broodwar->self()][BWAPI::UnitTypes::Enum::Zerg_Hatchery])
								//auto & hatchery = *MyUnitSets[BWAPI::UnitTypes::Enum::Zerg_Hatchery].m_units.begin();
							{
								//if (hatchery->getClientInfo<bool>('1st'))
									if(hatchery->m_unit->getTilePosition() == Broodwar->self()->getStartLocation())
									if (!hatchery->m_unit->isMorphing())
									{
										result = hatchery->m_unit->issueCommand(BWAPI::UnitCommand::morph(nullptr, mt.getUnitType()));

										if (result)
										{
											(*Units)[Broodwar->self()][BWAPI::UnitTypes::Enum::Zerg_Hatchery].erase(hatchery);
											/*reservedMineral -= requireMin;
											reservedGas -= requireGas;*/
											return true;
										}
									}
							}
							return result;
						}
						else if (mt.getUnitType() == BWAPI::UnitTypes::Enum::Zerg_Hive)
						{
							for (auto lair : (*Units)[Broodwar->self()][BWAPI::UnitTypes::Enum::Zerg_Lair])
								//auto & lair = *MyUnitSets[BWAPI::UnitTypes::Enum::Zerg_Lair].m_units.begin();
							{
								if (lair->m_unit->getTilePosition() == Broodwar->self()->getStartLocation())
									if (!lair->m_unit->isMorphing())
									{
										bool result = lair->m_unit->issueCommand(BWAPI::UnitCommand::morph(nullptr, mt.getUnitType()));
										if (result)
										{
											(*Units)[Broodwar->self()][BWAPI::UnitTypes::Enum::Zerg_Lair].erase(lair);
											/*reservedMineral -= mt.getUnitType().mineralPrice();
											reservedGas -= mt.getUnitType().gasPrice();*/
										}
										return result;
									}
								return false;
							}
						}
						else
						{
							//for (auto & worker : MyUnitSets[BWAPI::UnitTypes::Zerg_Drone].m_units)
							if ((*Units)[Broodwar->self()][BWAPI::UnitTypes::Zerg_Drone].empty())
								return false;

							auto supplyBuilder = *begin((*Units)[Broodwar->self()][BWAPI::UnitTypes::Zerg_Drone]);
							{
								// Retrieve a unit that is capable of constructing the supply needed
								/*BWAPI::Unit supplyBuilder = (worker)->getClosestUnit(BWAPI::Filter::GetType == mt.unitType.whatBuilds().first &&
								(BWAPI::Filter::IsIdle || BWAPI::Filter::IsGatheringMinerals) &&
								BWAPI::Filter::IsOwned);*/
								// If a unit was found
								if (supplyBuilder)
								{
									BWAPI::TilePosition targetBuildLocation = BWAPI::Broodwar->getBuildLocation(mt.getUnitType(), supplyBuilder->m_unit->getTilePosition(), 256);
									if (targetBuildLocation != TilePositions::Invalid)
									{
										// Register an event that draws the target build location
										//BWAPI::Broodwar->registerEvent([=](BWAPI::Game*)
										//{
										//	BWAPI::Broodwar->drawBoxMap(BWAPI::Position(targetBuildLocation),
										//		BWAPI::Position(targetBuildLocation + mt.getUnitType().tileSize()),
										//		BWAPI::Colors::Blue);
										//},
										//	[=](BWAPI::Game*) {return (*Units)[Broodwar->self()][mt.getUnitType()].empty(); },  // condition
										//	mt.getUnitType().buildTime() + 100);  // frames to run

																				  // Order the builder to construct the supply structure


										bool result = supplyBuilder->m_unit->build(mt.getUnitType(), targetBuildLocation);
										if (result == true)
										{
											/*reservedMineral -= requireMin;
											reservedGas -= requireGas;*/
										}

										return result;
										/*if( supplyBuilder->build(mt.unitType, targetBuildLocation))
										{
										if (supplyBuilder->isMorphing())
										{
										MyUnitSets[BWAPI::UnitTypes::Zerg_Drone].m_units.erase(supplyBuilder);
										return true;
										}
										}
										return false;*/
										//try {
										//	return supplyBuilder->build(mt.unitType, targetBuildLocation);
										//	Error last = Broodwar->getLastError();
										//	int i = 0;
										//}
										//catch (const std::exception & e)
										//{
										//	Broodwar << "EXCEPTION from " << __func__ << ": " << e.what() << std::endl;
										//}
									}
									return false;
								} // closure: supplyBuilder is valid
							}
						}
					}
					return false;
				}
				return false;
			}
		case UnitCommandTypes::Upgrade:
			{
				int minerals = BWAPI::Broodwar->self()->minerals();
				int requireMin = buildorder.front().getUpgradeType().mineralPrice();//mt.unitType.mineralPrice();

				int gas = BWAPI::Broodwar->self()->gas();
				int requireGas = buildorder.front().getUpgradeType().gasPrice();//mt.unitType.gasPrice();

				if (minerals >= requireMin
				 && gas >= requireGas)
				{
					bool result = false;
					for (auto u : (*Units)[Broodwar->self()][mt.getUpgradeType().whatUpgrades()])
					{
						if (!u->m_unit->isUpgrading()
						 && !u->m_unit->isBeingConstructed())
						{
							result = u->m_unit->upgrade(mt.getUpgradeType());
						}
					}
					//todo if unable to find upgrade building or cannot upgrade this time
					/*if (*MyUnitSets[mt.getUpgradeType().whatUpgrades()]._units.begin() != nullptr)
					{
					bool result = (*MyUnitSets[mt.getUpgradeType().whatUpgrades()]._units.begin())->upgrade(mt.getUpgradeType());
					if(result)
					{
					reservedMineral -= requireMin;
					reservedGas -= requireGas;
					}
					return result;
					}
					return false;*/
					if (result)
					{
						reservedMineral -= requireMin;
						reservedGas -= requireGas;
					}
					return result;
				}
				//	Broodwar->registerEvent([=](Game*)
				//{
				//	Broodwar->drawTextScreen(350, 200, "%s", what.getName().c_str());
				//},
				//		nullptr,  // condition
				//	2500);  // frames to run


				return false;
			}
		case UnitCommandTypes::Research:
			{
				int minerals = BWAPI::Broodwar->self()->minerals();
				int requireMin = buildorder.front().getTechType().mineralPrice();//mt.unitType.mineralPrice();

				int gas = BWAPI::Broodwar->self()->gas();
				int requireGas = buildorder.front().getTechType().gasPrice();//mt.unitType.gasPrice();

				if (minerals >= requireMin
				 && gas >= requireGas)
				{
					if (*begin((*Units)[Broodwar->self()][mt.getTechType().whatResearches()]) != nullptr)
					{
						if ((*begin((*Units)[Broodwar->self()][mt.getTechType().whatResearches()]))->m_unit->research(mt.getTechType()))
						{
							reservedMineral -= requireMin;
							reservedGas -= requireGas;
							return true;
						}
					}
					return false;
				}
				//	Broodwar->registerEvent([=](Game*)
				//{
				//	Broodwar->drawTextScreen(350, 200, "%s", what.getName().c_str());
				//},
				//		nullptr,  // condition
				//	2500);  // frames to run


				return false;
			}
		//default:
		return false;
		}
	}

	void LogisticsCommand::AddOrder(const UnitType& ut, const int& count, const int& priority)
	{
		UnitCommand unitType;
		unitType.type = UnitCommandTypes::Morph;
		unitType.extra = ut;
		for(auto i = 0; i < count; ++i)
		{
			buildorder.push_back(unitType);
		}
	}
	void LogisticsCommand::AddOrder(const UpgradeType& ut, const int& count, const int& priority)
	{
		UnitCommand upgradeType;
		upgradeType.type = UnitCommandTypes::Upgrade;
		upgradeType.extra = ut;
		buildorder.push_back(upgradeType);
	}
	void LogisticsCommand::AddOrder(const TechType& tt, const int& count, const int& priority)
	{
		UnitCommand techType;
		techType.type = UnitCommandTypes::Research;
		techType.extra = tt;
		buildorder.push_back(techType);
	}

	void LogisticsCommand::ReserveResource(const UnitCommandType & uct)
	{
		switch (uct)
		{
		case UnitCommandTypes::Build:
			// falls through
		case UnitCommandTypes::Morph:
			{
				reservedMineral += buildorder.front().getUnitType().mineralPrice();
				reservedGas += buildorder.front().getUnitType().gasPrice();
				if (buildorder.front().getUnitType().isTwoUnitsInOneEgg())
					reservedSupply += buildorder.front().getUnitType().supplyRequired();
				reservedSupply += buildorder.front().getUnitType().supplyRequired();
			}
			break;
		case UnitCommandTypes::Upgrade:
			{
				reservedMineral += buildorder.front().getUpgradeType().mineralPrice();
				reservedGas += buildorder.front().getUpgradeType().gasPrice();
			}
			break;
		case UnitCommandTypes::Research:
			{
				reservedMineral += buildorder.front().getTechType().mineralPrice();
				reservedGas += buildorder.front().getTechType().gasPrice();
			}
		}
	}

	void LogisticsCommand::SpendResource(const UnitCommandType & uct)
	{
		
	}

	void LogisticsCommand::GetNextOrder()
	{
		if (buildorder.front().getType() == UnitCommandTypes::Unknown)
		{
			if (!buildorder.empty())
			{
				++currentOrderNum;
				if (buildorder.size() > 2)
					buildorder.pop_front();
				ReserveResource(buildorder.front().getType());
			}
		}
		else //if(currentBO has some value)
		{
			//if(currentBO.data_type != MetaType::Unknown)
			//	UnitCommand current = buildorder.front();
			if (Build(buildorder.front()))
			{
				if (buildorder.front().getType() != UnitCommandTypes::Build
				 && buildorder.front().getType() != UnitCommandTypes::Morph)
				{
					++currentOrderNum;
					if (buildorder.size() > 2)
					{
						buildorder.pop_front();
						ReserveResource(buildorder.front().getType());
					}
				}
			}
		}
	}

	void LogisticsCommand::OnUnitMorph(Unit unit)
	{
		UnitType unitType = unit->getType();
		if (!buildorder.empty())
		{
			if (unit->getBuildType() == buildorder.front().getUnitType())
			{
				//if(currentBO.getUnitType().isBuilding())
				// this will be faster
				if (buildorder.front().getUnitType() > 62)
				{
					//(*UnitDataSets)[Broodwar->self()][UnitTypes::Enum::Zerg_Drone].m_units.erase(unit);
					(*Units)[Broodwar->self()][UnitTypes::Enum::Zerg_Drone].RemoveUnit(unit);

					reservedMineral -= unitType.mineralPrice();
					reservedGas -= unitType.gasPrice();
					//reservedSupply -= unitType.supplyRequired();
				}
				else
				{
					reservedMineral -= unit->getBuildType().mineralPrice();
					reservedGas -= unit->getBuildType().gasPrice();
					if (unitType.isTwoUnitsInOneEgg())
						reservedSupply -= unit->getBuildType().supplyRequired();
					reservedSupply -= unit->getBuildType().supplyRequired();
				}
				++currentOrderNum;

				if (buildorder.size() > 2)
					buildorder.pop_front();
				switch (buildorder.front().getType())
				{
				case UnitCommandTypes::Build:
					// falls through
				case UnitCommandTypes::Morph:
					{
						reservedMineral += buildorder.front().getUnitType().mineralPrice();
						reservedGas += buildorder.front().getUnitType().gasPrice();
						reservedSupply += buildorder.front().getUnitType().supplyRequired();
					}
					break;
				case UnitCommandTypes::Upgrade:
					{
						reservedMineral += buildorder.front().getUpgradeType().mineralPrice();
						reservedGas += buildorder.front().getUpgradeType().gasPrice();
					}
					break;
				case UnitCommandTypes::Research:
					{
						reservedMineral += buildorder.front().getTechType().mineralPrice();
						reservedGas += buildorder.front().getTechType().gasPrice();
					}
				}
			}
		}
		
	}

	void LogisticsCommand::OnUnitCreate(Unit unit) {};
	void LogisticsCommand::OnUnitShow(Unit unit) {};
	void LogisticsCommand::OnUnitDestroy(Unit unit) {};
}//namespace HOLD
