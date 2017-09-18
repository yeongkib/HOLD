/*****************************************************************************/
/*!
\file   IntelligenceCommand.cpp
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

#include "IntelligenceCommand.hpp"
#include <BWAPI/UnitCommand.h>
#include "Grid.hpp"
#include "../BWEM_1_3/utils.h"
#include "Drone.hpp"

namespace HOLD
{
	IntelligenceCommand::IntelligenceCommand() : m_idStack(0), mapWidth(0), mapHeight(0)
	{
	}


	IntelligenceCommand::~IntelligenceCommand()
	{
	}

	void IntelligenceCommand::Init()
	{
		mapWidth = BWAPI::Broodwar->mapWidth();
		mapHeight = BWAPI::Broodwar->mapHeight();


		InitInfluenceMaps();
	}


	void IntelligenceCommand::Update(int dt)
	{
		if (BWAPI::Broodwar->getFrameCount() % 4)
		{
			ClearInfluenceMaps();

			//todo : call updateinfluences, updateGrid
			// Iterate through all the units that we own
			for (auto &eus : UnitDataSets[Broodwar->enemy()])
			{
				for (auto &u : eus.second.m_units)
					//for(auto & u : Broodwar->enemy()->getUnits())
				{
					if (u->getPosition() != Positions::Unknown)
					{
						UpdateInfluences(u);
					}
				}
			}

			for (auto &eus : UnitDataSets[Broodwar->self()])
			{
				for (auto &u : eus.second.m_units)
					//for(auto & u : Broodwar->self()->getUnits())
				{
					// Ignore the unit if it no longer exists
					// Make sure to include this block when handling any Unit pointer!
					if (!u->exists())
						continue;

					// Ignore the unit if it has one of the following status ailments
					if (u->isLockedDown() || u->isMaelstrommed() || u->isStasised())
						continue;

					// Ignore the unit if it is in one of the following states
					if (u->isLoaded() || !u->isPowered() || u->isStuck())
						continue;

					// Ignore the unit if it is incomplete or busy constructing
					if (!u->isCompleted() || u->isConstructing())
						continue;

					UpdateInfluences(u);


					//// If the unit is a worker unit
					//if (u->getType().isWorker())
					//{
					//	//if unit is on the line, call return to cargo, and set it as true
					//	// if our worker is idle
					//	if (u->isIdle())
					//	{
					//		// Order workers carrying a resource to return them to the center,
					//		// otherwise find a mineral patch to harvest.
					//		if (u->isCarryingGas() || u->isCarryingMinerals())
					//		{
					//			//	u->issueCommand(UnitCommand::returnCargo(u));
					//		}
					//		else if (!u->getPowerUp())  // The worker cannot harvest anything if it
					//		{                             // is carrying a powerup such as a flag
					//									  // Harvest from the nearest mineral patch or gas refinery
					//									  //if (!u->gather(u->getClosestUnit(IsMineralField || IsRefinery)))
					//									  //{
					//									  //	// If the call fails, then print the last error message
					//									  //	Broodwar << Broodwar->getLastError() << std::endl;
					//									  //}

					//		} // closure: has no powerup
					//	} // closure: if idle
					//	  // if our worker is idle
					//	if (Orders::ReturnMinerals == u->getOrder()
					//		&& !u->getClientInfo(std::hash<std::string>{}(std::string("boost"))))
					//	{
					//		for (auto dl : DiagonalLineOfBase)
					//		{
					//			int i = 0;
					//			int offset = 2;
					//			i += isPointOnLine(dl.first, dl.second, Position(u->getLeft() - offset, u->getTop() - offset));

					//			if (1 == i)
					//			{
					//				int left = u->getLeft() - offset;
					//				int top = u->getTop() - offset;
					//				BWAPI::Broodwar->registerEvent([=](BWAPI::Game*)
					//				{
					//					BWAPI::Broodwar->drawCircleMap(left, top, 3, BWAPI::Colors::Purple, true);
					//					//BWAPI::Broodwar->drawCircleScreen(400, 200, 5, Colors::Black, true);
					//				},
					//					[=](BWAPI::Game*) {return true; },  // condition
					//					30);  // frames to run
					//			}
					//			i += isPointOnLine(dl.first, dl.second, Position(u->getRight() + offset, u->getTop() - offset)) * 10;
					//			if (10 <= i)
					//			{
					//				int right = u->getRight() + offset;
					//				int top = u->getTop() - offset;
					//				BWAPI::Broodwar->registerEvent([=](BWAPI::Game*)
					//				{
					//					BWAPI::Broodwar->drawCircleMap(right, top, 3, BWAPI::Colors::Purple, true);
					//					//BWAPI::Broodwar->drawCircleScreen(400, 200, 5, Colors::Black, true);
					//				},
					//					[=](BWAPI::Game*) {return true; },  // condition
					//					30);  // frames to run
					//			}
					//			i += isPointOnLine(dl.first, dl.second, Position(u->getLeft() - offset, u->getBottom() + offset)) * 100;
					//			if (100 <= i)
					//			{
					//				int left = u->getLeft() - offset;
					//				int bot = u->getBottom() + offset;
					//				BWAPI::Broodwar->registerEvent([=](BWAPI::Game*)
					//				{
					//					BWAPI::Broodwar->drawCircleMap(left, bot, 3, BWAPI::Colors::Purple, true);
					//					//BWAPI::Broodwar->drawCircleScreen(400, 200, 5, Colors::Black, true);
					//				},
					//					[=](BWAPI::Game*) {return true; },  // condition
					//					30);  // frames to run
					//			}
					//			i += isPointOnLine(dl.first, dl.second, Position(u->getRight() + offset, u->getBottom() + offset)) * 1000;
					//			if (1000 <= i)
					//			{
					//				int right = u->getRight() + offset;
					//				int bot = u->getBottom() + offset;
					//				BWAPI::Broodwar->registerEvent([=](BWAPI::Game*)
					//				{
					//					BWAPI::Broodwar->drawCircleMap(right, bot, 3, BWAPI::Colors::Purple, true);
					//					//BWAPI::Broodwar->drawCircleScreen(400, 200, 5, Colors::Black, true);
					//				},
					//					[=](BWAPI::Game*) {return true; },  // condition
					//					30);  // frames to run
					//			}
					//			/*i += isPointOnLine(dl.first, dl.second, Position(u->getLeft() , u->getTop() ));
					//			i += isPointOnLine(dl.first, dl.second, Position(u->getRight() , u->getTop() ));
					//			i += isPointOnLine(dl.first, dl.second, Position(u->getLeft() , u->getBottom() ));
					//			i += isPointOnLine(dl.first, dl.second, Position(u->getRight() , u->getBottom() ));*/

					//			if (1 == i
					//				|| 10 == i
					//				|| 100 == i
					//				|| 1000 == i
					//				)//|| 1 <= i)
					//			{
					//				u->issueCommand(UnitCommand::returnCargo(u));
					//				u->setClientInfo(true, std::hash<std::string>{}(std::string("boost")));
					//				BWAPI::Broodwar->registerEvent([=](BWAPI::Game*)
					//				{
					//					BWAPI::Broodwar->drawBoxMap(u->getLeft(), u->getTop(), u->getRight(), u->getBottom(), BWAPI::Colors::White, true);
					//					BWAPI::Broodwar->drawTextScreen(400, 200, "%c%d", Colors::Black, "boost on");
					//				},
					//					[=](BWAPI::Game*) {return true; },  // condition
					//					30);  // frames to run
					//			}
					//		}
					//	}
					//	if (!u->isCarryingMinerals()
					//		&& u->getClientInfo(std::hash<std::string>{}(std::string("boost"))))
					//	{
					//		u->setClientInfo(false, std::hash<std::string>{}(std::string("boost")));
					//	}
					//}
					if (BWAPI::Broodwar->getFrameCount() % 50 == 0)
						//||Broodwar->getFrameCount() <= 30000)
					{
						/*if(BWEM::utils::contains()::find(enemyBaseCandidate.begin(), enemyBaseCandidate.end(), getTargetPosition()) != enemyBaseCandidate.end())
						enemyBaseCandidate.erase(remove(enemyBaseCandidate.begin(), enemyBaseCandidate.end(), getTargetPosition()), enemyBaseCandidate.end());*/

						const bool targetAlreadyReached = !BWEM::utils::contains(enemyBaseCandidate, TilePosition(u->getTargetPosition()));
						if (targetAlreadyReached ||
							u->getPosition().getApproxDistance(u->getTargetPosition() + Position(UnitType(UnitTypes::Terran_Command_Center).tileSize()) / 2) < 32 * 6)
						{
							///	if (targetAlreadyReached) Log << Agent()->NameWithId() << ": target already reached" << endl;
							if (enemyBaseCandidate.size() > 1)
								if (!targetAlreadyReached)
									BWEM::utils::really_remove(enemyBaseCandidate, TilePosition(u->getTargetPosition()));

							//Agent()->ChangeBehavior<Scouting>(Agent());
						}

						//todo fix it
						if (u->getType() == UnitTypes::Zerg_Zergling)
						{
							if (UnitDataSets[Broodwar->self()][u->getType()].m_units.size() >= 10)
							{
								if (!(u->getOrder() == Orders::AttackMove)
									|| !(u->getOrder() == Orders::AttackUnit))
								{
									//int i = rand() % Broodwar->getStartLocations().size();
									int i = 0;

									if (!enemyBaseCandidate.empty())
										u->issueCommand(BWAPI::UnitCommand::attack(nullptr, Position(enemyBaseCandidate[0].x * 32, enemyBaseCandidate[0].y * 32)));
									//u->issueCommand(BWAPI::UnitCommand::attack(u, Position(Broodwar->getStartLocations()[i].x * 32, Broodwar->getStartLocations()[i].y * 32)));
								}
							}
							//Unitset targets;
							//if (u->isFlying())
							//	targets = u->getUnitsInRadius((WeaponTypes::Hellfire_Missile_Pack).maxRange() + 3 * 32/*upgrade*/ + 16, IsEnemy);
							//else
							//	targets = u->getUnitsInRadius((WeaponTypes::Arclite_Cannon_Edmund_Duke).maxRange() + 16, IsEnemy);

							//int closestDist = 10000000;
							//BWAPI::Unit closestTarget = nullptr;
							//for (auto target : targets)
							//{
							//	if (target->canAttack(u, false, false, false))
							//	{
							//		int dist = target->getDistance(u);
							//		if (dist < closestDist)
							//		{
							//			closestDist = dist;
							//			closestTarget = target;
							//		}
							//	}
							//}
							//if (closestTarget)
							//{
							//	BWAPI::Position fleeVec(u->getPosition() - closestTarget->getPosition());
							//	double fleeAngle = atan2(fleeVec.y, fleeVec.x);
							//	fleeVec = BWAPI::Position(static_cast<int>(64.0 * cos(fleeAngle)), static_cast<int>(64.0 * sin(fleeAngle)));
							//	Broodwar->drawLineMap(u->getPosition(), u->getPosition() + fleeVec, Colors::Red);
							//	Broodwar->drawCircleMap(closestTarget->getPosition(), 5, Colors::Red, true);

							//	Position forwardVec(closestTarget->getPosition() - u->getPosition());
							//	Broodwar->drawLineMap(u->getPosition(), u->getPosition(), Colors::Blue);


							//	float cooldown = static_cast<float>(u->getGroundWeaponCooldown()) / static_cast<float>(u->getType().groundWeapon().damageCooldown());

							//	if (cooldown > 0.5f)
							//	{
							//		u->issueCommand(UnitCommand::move(u, u->getPosition() + fleeVec));
							//	}
							//	else
							//	{
							//		u->issueCommand(UnitCommand::move(u, u->getPosition() + fleeVec));
							//		u->issueCommand(UnitCommand::rightClick(u, closestTarget));
							//	}
							//}
							/*else
							{
							if (!(u->getOrder() == Orders::AttackMove)
							|| !(u->getOrder() == Orders::AttackUnit))
							{
							u->issueCommand(BWAPI::UnitCommand::attack(u, Position(cpx, cpy)));
							}
							}*/
						}
						else if (u->getType() == UnitTypes::Zerg_Mutalisk)
						{
							if (firstattack_mutal || UnitDataSets[Broodwar->self()][u->getType()].m_units.size() >= 10)
								if (!(u->getOrder() == Orders::AttackMove)
									|| !(u->getOrder() == Orders::AttackUnit))
								{
									firstattack_mutal = true;
									//int i = rand() % Broodwar->getStartLocations().size();
									int i = 0;


									if (foundEnemyBase)
									{
										if (enemyBase.empty())
										{
											//attack remain building first?, then search nearest base(it could be expansion) // it should be fixed to search expansion first
											bool selected = false;
											for (auto eus : UnitDataSets[Broodwar->enemy()])
											{
												auto us = UnitDataSets[Broodwar->enemy()].find(eus.first);
												if (selected)
													break;
												if (us != UnitDataSets[Broodwar->enemy()].end())
												{
													for (auto su : us->second.m_savedUnits)
													{
														if (su.second.m_unit != nullptr
															&& su.second.m_lastType.isBuilding())
														{
															//Broodwar->drawTextMap(su.second.m_lastPosition, "%c%d", Text::Cyan, su.second.m_lastSeen);
															u->issueCommand(BWAPI::UnitCommand::attack(nullptr, su.second.m_lastPosition));
															selected = true;
															break;
														}
													}
												}
											}
										}
										else //if enemyBase is remained
										{
											u->issueCommand(BWAPI::UnitCommand::attack(nullptr, Position(enemyBase[0].x * 32, enemyBase[0].y * 32)));
										}

									}

									else if (!enemyBaseCandidate.empty())
										u->issueCommand(BWAPI::UnitCommand::attack(nullptr, Position(enemyBaseCandidate[0].x * 32, enemyBaseCandidate[0].y * 32)));
									//u->issueCommand(BWAPI::UnitCommand::attack(u, Position(Broodwar->getStartLocations()[i].x * 32, Broodwar->getStartLocations()[i].y * 32)));
								}
						}
						else if (u->getType() == UnitTypes::Zerg_Hydralisk)
						{
							if (firstattack_hydra || UnitDataSets[Broodwar->self()][u->getType()].m_units.size() >= 20)
								if (!(u->getOrder() == Orders::AttackMove)
									|| !(u->getOrder() == Orders::AttackUnit))
								{
									firstattack_hydra = true;
									//int i = rand() % Broodwar->getStartLocations().size();
									int i = 0;

									if (foundEnemyBase)
									{
										if (enemyBase.empty())
										{
											//attack remain building first?, then search nearest base(it could be expansion) // it should be fixed to search expansion first
											bool selected = false;
											for (auto eus : UnitDataSets[Broodwar->enemy()])
											{
												auto us = UnitDataSets[Broodwar->enemy()].find(eus.first);
												if (selected)
													break;
												if (us != UnitDataSets[Broodwar->enemy()].end())
												{
													for (auto su : us->second.m_savedUnits)
													{
														if (su.second.m_unit != nullptr
															&& su.second.m_lastType.isBuilding())
														{
															//Broodwar->drawTextMap(su.second.m_lastPosition, "%c%d", Text::Cyan, su.second.m_lastSeen);
															u->issueCommand(BWAPI::UnitCommand::attack(nullptr, su.second.m_lastPosition));
															selected = true;
															break;
														}
													}
												}
											}
										}
										else //if enemyBase is remained
										{
											u->issueCommand(BWAPI::UnitCommand::attack(nullptr, Position(enemyBase[0].x * 32, enemyBase[0].y * 32)));
										}

									}

									else if (!enemyBaseCandidate.empty())
										u->issueCommand(BWAPI::UnitCommand::attack(nullptr, Position(enemyBaseCandidate[0].x * 32, enemyBaseCandidate[0].y * 32)));
									//u->issueCommand(BWAPI::UnitCommand::attack(u, Position(Broodwar->getStartLocations()[i].x * 32, Broodwar->getStartLocations()[i].y * 32)));
									//Unitset targets;
									//if (u->isFlying())
									//	targets = u->getUnitsInRadius((WeaponTypes::Hellfire_Missile_Pack).maxRange() + 3 * 32/*upgrade*/ + 16, IsEnemy);
									//else
									//	targets = u->getUnitsInRadius((WeaponTypes::Arclite_Cannon_Edmund_Duke).maxRange() + 16, IsEnemy);

									//int closestDist = 10000000;
									//BWAPI::Unit closestTarget = nullptr;
									//for (auto target : targets)
									//{
									//	if (target->canAttack(u, false, false, false))
									//	{
									//		int dist = target->getDistance(u);
									//		if (dist < closestDist)
									//		{
									//			closestDist = dist;
									//			closestTarget = target;
									//		}
									//	}
									//}
									//if (closestTarget)
									//{
									//	BWAPI::Position fleeVec(u->getPosition() - closestTarget->getPosition());
									//	double fleeAngle = atan2(fleeVec.y, fleeVec.x);
									//	fleeVec = BWAPI::Position(static_cast<int>(64.0 * cos(fleeAngle)), static_cast<int>(64.0 * sin(fleeAngle)));
									//	Broodwar->drawLineMap(u->getPosition(), u->getPosition() + fleeVec, Colors::Red);
									//	Broodwar->drawCircleMap(closestTarget->getPosition(), 5, Colors::Red, true);

									//	Position forwardVec(closestTarget->getPosition() - u->getPosition());
									//	Broodwar->drawLineMap(u->getPosition(), u->getPosition(), Colors::Blue);


									//	float cooldown = static_cast<float>(u->getGroundWeaponCooldown()) / static_cast<float>(u->getType().groundWeapon().damageCooldown());

									//	if (cooldown > 0.5f)
									//	{
									//		u->issueCommand(UnitCommand::move(u, u->getPosition() + fleeVec));
									//	}
									//	else
									//	{
									//		u->issueCommand(UnitCommand::move(u, u->getPosition() + fleeVec));
									//		u->issueCommand(UnitCommand::rightClick(u, closestTarget));
									//	}
									//}
								}
						}
					}
				}
			}

			UpdateGrid();
		}

		int uy = 50;
		for (int lists : {UnitTypes::Zerg_Drone, UnitTypes::Zerg_Zergling, UnitTypes::Zerg_Hydralisk, UnitTypes::Zerg_Mutalisk})
		{
			auto i = UnitDataSets[Broodwar->self()].find(lists);
			if (i != UnitDataSets[Broodwar->self()].end())
			{
				if (i->second.m_units.size())
				{
					Broodwar->drawTextScreen(5, uy, "%s %d", (*i->second.m_units.begin())->getType().getName().c_str(), i->second.m_units.size());
					uy += 10;
				}
			}
		}
		int by = 50;
		for (int lists : {UnitTypes::Zerg_Hatchery, UnitTypes::Zerg_Lair, UnitTypes::Zerg_Hive, UnitTypes::Zerg_Extractor, UnitTypes::Zerg_Spawning_Pool, UnitTypes::Zerg_Hydralisk_Den, UnitTypes::Zerg_Spire})
		{
			auto i = UnitDataSets[Broodwar->self()].find(lists);
			if (i != UnitDataSets[Broodwar->self()].end())
			{
				if (i->second.m_units.size())
				{
					Broodwar->drawTextScreen(150, by, "%s %d", (*i->second.m_units.begin())->getType().getName().c_str(), i->second.m_units.size());
					by += 10;
				}
			}
		}

		int ey = 50;
		for (auto eus : UnitDataSets[Broodwar->enemy()])
		{
			auto us = UnitDataSets[Broodwar->enemy()].find(eus.first);
			if (us != UnitDataSets[Broodwar->enemy()].end())
			{
				if (us->second.m_savedUnits.size())
				{
					//Broodwar->drawTextScreen(280, ey, "%s %d", (*us->second.m_units.begin())->getType().getName().c_str(), us->second.m_units.size());
					Broodwar->drawTextScreen(280, ey, "%s %d", UnitType(eus.first).c_str(), us->second.m_savedUnits.size());
					ey += 10;
				}
			}
		}

	}

	void IntelligenceCommand::Shutdown()
	{
		for (auto ds : UnitDataSets)
		{
			ds.second.clear();
		}

		/*for(auto grid : grids)
		{
			Vgrid().swap(grid.second);
		}*/
		Vgrid().swap(opinfluenceGround);
		Vgrid().swap(opinfluenceAir);
		Vgrid().swap(influenceGround);
		Vgrid().swap(influenceAir);
		Vgrid().swap(tensionGround);
		Vgrid().swap(tensionAir);
		Vgrid().swap(tensionTotal);
		Vgrid().swap(vulGround);
		Vgrid().swap(vulAir);
		Vgrid().swap(vulTotal);
	}
	void IntelligenceCommand::ProcessMessage(Message* message)
	{

	}

	void IntelligenceCommand::BroadcastMessage(Message* message)
	{
	}

	const int IntelligenceCommand::GetMapHeight() const
	{
		return mapHeight;
	}
	const int IntelligenceCommand::GetMapWidth() const
	{
		return mapWidth;
	}

	void IntelligenceCommand::UpdateGrid()
	{
		/*std::transform(grids[std::string{ "tensionGround" }].begin(), grids[std::string{ "tensionGround" }].end(), grids[std::string{ "influenceGround" }].begin(),
			grids[std::string{ "vulGround" }].begin(), [](Grid &first, Grid &second)->Grid
		{
			return first - HOLD::abs(second);
		});

		std::transform(grids[std::string{ "tensionAir" }].begin(), grids[std::string{ "tensionAir" }].end(), grids[std::string{ "influenceAir" }].begin(),
			grids[std::string{ "vulAir" }].begin(), [](Grid& first, Grid &second)->Grid
		{
			return first - HOLD::abs(second);
		});

		std::transform(grids[std::string{ "vulGround" }].begin(), grids[std::string{ "vulGround" }].end(), grids[std::string{ "vulAir" }].begin(),
			grids[std::string{ "vulTotal"}].begin(), std::plus<Grid>());*/
		/*std::transform(tensionGround.begin(), tensionGround.end(), influenceGround.begin(),
			vulGround.begin(), [](Grid &first, Grid &second)->Grid
		{
			return first - HOLD::abs(second);
		});

		std::transform(tensionAir.begin(), tensionAir.end(), influenceAir.begin(),
			vulAir.begin(), [](Grid& first, Grid &second)->Grid
		{
			return first - HOLD::abs(second);
		});

		std::transform(vulGround.begin(), vulGround.end(), vulAir.begin(),
			vulTotal.begin(), std::plus<Grid>());*/
		std::transform(begin(tensionGround), end(tensionGround), begin(influenceGround),
			vulGround.begin(), [](Grid &first, Grid &second)->Grid
		{
			return first - HOLD::abs(second);
		});

		std::transform(tensionAir.begin(), tensionAir.end(), influenceAir.begin(),
			vulAir.begin(), [](Grid& first, Grid &second)->Grid
		{
			return first - HOLD::abs(second);
		});

		std::transform(vulGround.begin(), vulGround.end(), vulAir.begin(),
			vulTotal.begin(), std::plus<Grid>());
	}

	Vgrid* IntelligenceCommand::GetGridMap(const std::string& name)
	{
		//todo : assert if not found

		/*auto it = grids.find(name);
		if (it != grids.end()) {
			return &it->second;
		}*/

		return nullptr;
	}

	//template<typename T>
	//void IntelligenceCommand::SetValues(Grid & cell, int targetpos_x, int targetpos_y, int  curpos_x, int  curpos_y, const int & damage, T op, const int& range)
	//{
	//	/*
	//	* input : each cell, and curret position and target position
	//	* loop for each quarter
	//	*/

	//	auto CalInfluence = [](short & cell, int targetpos_x, int targetpos_y, int curpos_x, int curpos_y, const int & damage, auto op, auto & range)
	//	{
	//		// make influence fall of with distance:
	//		float dist = Math::Distance(Vector2(curpos_x, curpos_y), Vector2(targetpos_x, targetpos_y));
	//		//dist = fmod(x, 32.f);

	//		//if (dist > static_cast<float>(range * 1.3))
	//		//if(Position(curpos_x, curpos_y).getApproxDistance(Position(targetpos_x, targetpos_y)) > range * 1.1414f)
	//		//	return;

	//		int distinp = Position(curpos_x, curpos_y).getApproxDistance(Position(targetpos_x, targetpos_y));
	//		/*	if (distinp >= range + 8)
	//		return;*/


	//		if (distinp <= range + 8)
	//		{
	//			if (distinp <= range)
	//			{
	//				//Broodwar->drawTextMap(targetpos_x, targetpos_y, "%d", static_cast<int>( static_cast<float>(damage) / dist));

	//				cell = op(cell, static_cast<short>(damage));
	//				return;
	//			}
	//			//Broodwar->drawTextMap(targetpos_x, targetpos_y, "%2.0f", dist);

	//			dist /= 32.f;
	//			if (dist <= 1.f)
	//				dist = 1.f;
	//			cell = op(cell, static_cast<short>(static_cast<float>(damage) / dist));
	//			return;
	//		}
	//	};

	//	for (char i = 0; i < 4; ++i)
	//	{
	//		CalInfluence(cell[i], targetpos_x * 32 + Neighbours_Walk[i].x + curpos_x % 4, targetpos_y * 32 + Neighbours_Walk[i].y + curpos_y % 4, curpos_x, curpos_y, damage, op, range);
	//	}
	//}
	void IntelligenceCommand::UpdateInfluences(const Unit u)
	{
		if (u->getType() == UnitTypes::Terran_Bunker)// && !u->getLoadedUnits().empty())
		{
			//can it be recursive? nvm it doens't work without complete map info
			//for(Unit loadedUnit : u->getLoadedUnits())
			{
				int x = u->getPosition().x;// / 32;
				int y = u->getPosition().y;// / 32;
				WeaponType groundWeapon = BWAPI::WeaponTypes::Gauss_Rifle;
				WeaponType airWeapon = BWAPI::WeaponTypes::Gauss_Rifle;
				float groundRadius = static_cast<float>(u->getPlayer()->weaponMaxRange(groundWeapon)) + 1.f;
				float airRadius = static_cast<float>(u->getPlayer()->weaponMaxRange(airWeapon)) + 1.f;

				// unit's center position
				float unitPositionX = static_cast<float>((static_cast<float>(u->getLeft()) + static_cast<float>(u->getRight())) / 2.f);
				float unitPositionY = static_cast<float>((static_cast<float>(u->getBottom()) + static_cast<float>(u->getTop())) / 2.f);

				int groundStartX = static_cast<int>(floor((unitPositionX - groundRadius) / 32.f));
				int groundStartY = static_cast<int>(floor((unitPositionY - groundRadius) / 32.f));

				int airStartX = static_cast<int>(floor((unitPositionX - airRadius) / 32.f));
				int airStartY = static_cast<int>(floor((unitPositionY - airRadius) / 32.f));

				int groundEndX = static_cast<int>(ceil((unitPositionX + groundRadius) / 32.f));
				int groundEndY = static_cast<int>(ceil((unitPositionY + groundRadius) / 32.f));

				int airEndX = static_cast<int>(ceil((unitPositionX + airRadius) / 32.f));
				int airEndY = static_cast<int>(ceil((unitPositionY + airRadius) / 32.f));

				//need to clamp
				groundStartX = Math::Clamp(groundStartX, 0, mapWidth - 1);
				groundStartY = Math::Clamp(groundStartY, 0, mapHeight - 1);
				groundEndX = Math::Clamp(groundEndX, 0, mapWidth - 1);
				groundEndY = Math::Clamp(groundEndY, 0, mapHeight - 1);

				airStartX = Math::Clamp(airStartX, 0, mapWidth - 1);
				airStartY = Math::Clamp(airStartY, 0, mapHeight - 1);
				airEndX = Math::Clamp(airEndX, 0, mapWidth - 1);
				airEndY = Math::Clamp(airEndY, 0, mapHeight - 1);


				int level = u->getPlayer()->getUpgradeLevel(groundWeapon.upgradeType());
				int damage = (groundWeapon.damageAmount() + level * groundWeapon.damageBonus()) * 4;// *groundWeapon.damageFactor(); //
																									//int damage = static_cast<int>(static_cast<double>((groundWeapon.damageAmount() + level * groundWeapon.damageBonus())) * (static_cast<double>(groundWeapon.damageCooldown() - u->getGroundWeaponCooldown()) / static_cast<double>(groundWeapon.damageCooldown())));

				for (int Y = groundStartY; Y < groundEndY; ++Y)
				{
					for (int X = groundStartX; X < groundEndX; ++X)
					{
						/*SetValues(GetGridMap(std::string{ "opinfluenceGround" })->at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), groundRadius);
						SetValues(GetGridMap(std::string{ "opinfluenceAir" })->at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), groundRadius);
						SetValues(GetGridMap(std::string{ "influenceGround" })->at(Y * mapHeight + X), X, Y, x, y, damage, std::minus<short>(), groundRadius);
						SetValues(GetGridMap(std::string{ "influenceGround" })->at(Y * mapHeight + X), X, Y, x, y, damage, std::minus<short>(), groundRadius);
						SetValues(GetGridMap(std::string{ "tensionGround" })->at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), groundRadius);
						SetValues(GetGridMap(std::string{ "opinfluenceGround" })->at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), groundRadius);*/
						SetValues(opinfluenceGround.at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), groundRadius);
						SetValues(opinfluenceAir.at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), groundRadius);
						SetValues(influenceGround.at(Y * mapHeight + X), X, Y, x, y, damage, std::minus<short>(), groundRadius);
						SetValues(influenceGround.at(Y * mapHeight + X), X, Y, x, y, damage, std::minus<short>(), groundRadius);
						SetValues(tensionGround.at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), groundRadius);
						SetValues(opinfluenceGround.at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), groundRadius);
					}
				}
			}
		}
		else
		{
			int x = u->getPosition().x;// / 32;
			int y = u->getPosition().y;// / 32;
			WeaponType groundWeapon = u->getType().groundWeapon();
			WeaponType airWeapon = u->getType().airWeapon();
			float groundRadius = static_cast<float>(u->getPlayer()->weaponMaxRange(groundWeapon));
			float airRadius = static_cast<float>(u->getPlayer()->weaponMaxRange(airWeapon));

			// unit's center position
			float unitPositionX = static_cast<float>((static_cast<float>(u->getLeft()) + static_cast<float>(u->getRight())) / 2.f);
			float unitPositionY = static_cast<float>((static_cast<float>(u->getBottom()) + static_cast<float>(u->getTop())) / 2.f);


			//todo : handle defense buildings
			float groundOffset = -0.8;

			if (groundRadius > 32)
				groundOffset = -1.8;

			//groundOffset = 0.f;

			//offset = 0.2;

			int groundStartX = static_cast<int>(floor((unitPositionX - groundRadius) / 32.f + groundOffset));
			int groundStartY = static_cast<int>(floor((unitPositionY - groundRadius) / 32.f + groundOffset));

			float airOffset = -0.8;

			if (airRadius > 32)
				airOffset = -1.8;

			//airOffset = 0.f;

			int airStartX = static_cast<int>(floor((unitPositionX - airRadius) / 32.f + airOffset));
			int airStartY = static_cast<int>(floor((unitPositionY - airRadius) / 32.f + airOffset));

			int groundEndX = static_cast<int>(ceil((unitPositionX + groundRadius) / 32.f - groundOffset));
			int groundEndY = static_cast<int>(ceil((unitPositionY + groundRadius) / 32.f - groundOffset));

			int airEndX = static_cast<int>(ceil((unitPositionX + airRadius) / 32.f - airOffset));
			int airEndY = static_cast<int>(ceil((unitPositionY + airRadius) / 32.f - airOffset));

			//need to clamp
			groundStartX = Math::Clamp(groundStartX, 0, mapWidth - 1);
			groundStartY = Math::Clamp(groundStartY, 0, mapHeight - 1);
			groundEndX = Math::Clamp(groundEndX, 0, mapWidth - 1);
			groundEndY = Math::Clamp(groundEndY, 0, mapHeight - 1);

			airStartX = Math::Clamp(airStartX, 0, mapWidth - 1);
			airStartY = Math::Clamp(airStartY, 0, mapHeight - 1);
			airEndX = Math::Clamp(airEndX, 0, mapWidth - 1);
			airEndY = Math::Clamp(airEndY, 0, mapHeight - 1);

			if (groundWeapon.damageAmount())
			{
				int level = u->getPlayer()->getUpgradeLevel(groundWeapon.upgradeType());
				int damage = (groundWeapon.damageAmount() + level * groundWeapon.damageBonus());// *groundWeapon.damageFactor(); //
																								//int damage = static_cast<int>(static_cast<double>((groundWeapon.damageAmount() + level * groundWeapon.damageBonus())) * (static_cast<double>(groundWeapon.damageCooldown() - u->getGroundWeaponCooldown()) / static_cast<double>(groundWeapon.damageCooldown())));

				if (damage)
					for (int Y = groundStartY; Y < groundEndY; ++Y)
					{
						for (int X = groundStartX; X < groundEndX; ++X)
						{
							if (u->getPlayer() == Broodwar->self())
							{
								/*SetValues(GetGridMap(std::string{ "influenceGround" })->at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), groundRadius);
								SetValues(GetGridMap(std::string{ "tensionGround" })->at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), groundRadius);
								SetValues(GetGridMap(std::string{ "tensionTotal" })->at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), groundRadius);*/
								SetValues(influenceGround.at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), groundRadius);
								SetValues(tensionGround.at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), groundRadius);
								SetValues(tensionTotal.at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), groundRadius);
							}
							else //if enemy
							{
								/*SetValues(GetGridMap(std::string{ "opinfluenceGround" })->at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), groundRadius);
								SetValues(GetGridMap(std::string{ "influenceGround" })->at(Y * mapHeight + X), X, Y, x, y, damage, std::minus<short>(), groundRadius);
								SetValues(GetGridMap(std::string{ "tensionGround" })->at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), groundRadius);
								SetValues(GetGridMap(std::string{ "tensionTotal" })->at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), groundRadius);*/
								SetValues(opinfluenceGround.at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), groundRadius);
								SetValues(influenceGround.at(Y * mapHeight + X), X, Y, x, y, damage, std::minus<short>(), groundRadius);
								SetValues(tensionGround.at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), groundRadius);
								SetValues(tensionTotal.at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), groundRadius);
							}
						}
					}
			}
			if (airWeapon.damageAmount())
			{
				int level = u->getPlayer()->getUpgradeLevel(airWeapon.upgradeType());
				int damage = (airWeapon.damageAmount() + level * airWeapon.damageBonus());// *groundWeapon.damageFactor(); //
																						  //int damage = static_cast<int>(static_cast<double>((airWeapon.damageAmount() + level * airWeapon.damageBonus())) * (static_cast<double>(airWeapon.damageCooldown() - u->getAirWeaponCooldown()) / static_cast<double>(airWeapon.damageCooldown())));

				if (damage)
					for (int Y = airStartY; Y < airEndY; ++Y)
					{
						for (int X = airStartX; X < airEndX; ++X)
						{
							if (u->getPlayer() == Broodwar->self())
							{
								/*SetValues(GetGridMap(std::string{ "influenceAir" })->at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), airRadius);
								SetValues(GetGridMap(std::string{ "tensionAir" })->at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), airRadius);
								SetValues(GetGridMap(std::string{ "tensionTotal" })->at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), airRadius);*/
								SetValues(influenceAir.at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), airRadius);
								SetValues(tensionAir.at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), airRadius);
								SetValues(tensionTotal.at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), airRadius);
							}
							else //if enemy
							{
								/*SetValues(GetGridMap(std::string{ "opinfluenceAir" })->at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), airRadius);
								SetValues(GetGridMap(std::string{ "influenceGround" })->at(Y * mapHeight + X), X, Y, x, y, damage, std::minus<short>(), airRadius);
								SetValues(GetGridMap(std::string{ "tensionAir" })->at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), airRadius);
								SetValues(GetGridMap(std::string{ "tensionTotal" })->at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), airRadius);*/
								SetValues(opinfluenceAir.at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), airRadius);
								SetValues(influenceGround.at(Y * mapHeight + X), X, Y, x, y, damage, std::minus<short>(), airRadius);
								SetValues(tensionAir.at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), airRadius);
								SetValues(tensionTotal.at(Y * mapHeight + X), X, Y, x, y, damage, std::plus<short>(), airRadius);
							}
						}
					}
			}
		}
	}

	void IntelligenceCommand::InitInfluenceMaps()
	{
		// should know the diffrence btw {} and ()
		/*grids[std::string{ "opinfluenceGround" }] = Vgrid(mapHeight * mapWidth, Grid{ 0 });
		grids[std::string{ "opinfluenceAir" }] = Vgrid(mapHeight * mapWidth, Grid{ 0 });
		grids[std::string{ "influenceGround" }] = Vgrid(mapHeight * mapWidth, Grid{ 0 });
		grids[std::string{ "influenceAir" }] = Vgrid(mapHeight * mapWidth, Grid{ 0 });
		grids[std::string{ "tensionGround" }] = Vgrid(mapHeight * mapWidth, Grid{ 0 });
		grids[std::string{ "tensionAir" }] = Vgrid(mapHeight * mapWidth, Grid{ 0 });
		grids[std::string{ "tensionTotal" }] = Vgrid(mapHeight * mapWidth, Grid{ 0 });
		grids[std::string{ "vulGround" }] = Vgrid(mapHeight * mapWidth, Grid{ 0 });
		grids[std::string{ "vulAir" }] = Vgrid(mapHeight * mapWidth, Grid{ 0 });
		grids[std::string{ "vulTotal" }] = Vgrid(mapHeight * mapWidth, Grid{ 0 });*/
		opinfluenceGround.resize(mapHeight * mapWidth, Grid{ 0 });

		opinfluenceAir.resize(mapHeight * mapWidth, Grid{ 0 });

		influenceGround.resize(mapHeight * mapWidth, Grid{ 0 });

		influenceAir.resize(mapHeight * mapWidth, Grid{ 0 });

		tensionGround.resize(mapHeight * mapWidth, Grid{ 0 });

		tensionAir.resize(mapHeight * mapWidth, Grid{ 0 });

		tensionTotal.resize(mapHeight * mapWidth, Grid{ 0 });

		vulGround.resize(mapHeight * mapWidth, Grid{ 0 });

		vulAir.resize(mapHeight * mapWidth, Grid{ 0 });

		vulTotal.resize(mapHeight * mapWidth, Grid{ 0 });
	}

	void IntelligenceCommand::ClearInfluenceMaps()
	{
		//I don't have any better idea :(
		/*memset(&grids[std::string{ "opinfluenceGround" }][0], 0, sizeof(grids["opinfluenceGround"][0]) * grids["opinfluenceGround"].size());
		memset(&grids[std::string{ "opinfluenceAir" }][0], 0, sizeof(grids["opinfluenceAir"][0]) * grids["opinfluenceAir"].size());
		memset(&grids[std::string{ "influenceGround" }][0], 0, sizeof(grids["influenceGround"][0]) * grids["influenceGround"].size());
		memset(&grids[std::string{ "influenceAir" }][0], 0, sizeof(grids["influenceAir"][0]) * grids["influenceAir"].size());
		memset(&grids[std::string{ "tensionGround" }][0], 0, sizeof(grids["tensionGround"][0]) * grids["tensionGround"].size());
		memset(&grids[std::string{ "tensionAir" }][0], 0, sizeof(grids["tensionAir"][0]) * grids["tensionAir"].size());
		memset(&grids[std::string{ "tensionTotal" }][0], 0, sizeof(grids["tensionTotal"][0]) * grids["tensionTotal"].size());
		memset(&grids[std::string{ "vulGround" }][0], 0, sizeof(grids["vulGround"][0]) * grids["vulGround"].size());
		memset(&grids[std::string{ "vulAir" }][0], 0, sizeof(grids["vulAir"][0]) * grids["vulAir"].size());
		memset(&grids[std::string{ "vulTotal" }][0], 0, sizeof(grids["vulTotal"][0]) * grids["vulTotal"].size());*/
		memset(&opinfluenceGround[0], 0, sizeof(opinfluenceGround[0]) * opinfluenceGround.size());
		memset(&opinfluenceAir[0], 0, sizeof(opinfluenceAir[0]) * opinfluenceAir.size());
		memset(&influenceGround[0], 0, sizeof(influenceGround[0]) * influenceGround.size());
		memset(&influenceAir[0], 0, sizeof(influenceAir[0]) * influenceAir.size());
		memset(&tensionGround[0], 0, sizeof(tensionGround[0]) * tensionGround.size());
		memset(&tensionAir[0], 0, sizeof(tensionAir[0]) * tensionAir.size());
		memset(&tensionTotal[0], 0, sizeof(tensionTotal[0]) * tensionTotal.size());
		memset(&vulGround[0], 0, sizeof(vulGround[0]) * vulGround.size());
		memset(&vulAir[0], 0, sizeof(vulAir[0]) * vulAir.size());
		memset(&vulTotal[0], 0, sizeof(vulTotal[0]) * vulTotal.size());
	}

	std::unordered_map<BWAPI::Player, std::unordered_map<int, HOLD::UnitDataSet>>* IntelligenceCommand::GetUnitDataSets()
	{
		return &UnitDataSets;
	}

	void IntelligenceCommand::AddUnit(Unit u)
	{
		//UnitDataSets[Broodwar->self()][u->getType()].m_units.emplace(u);
		UnitDataSets[Broodwar->self()][u->getType()].AddUnit(u);
	}

	void IntelligenceCommand::OnUnitMorph(Unit unit)
	{
		UnitType unitType = unit->getType();

		if (unitType.isBuilding())
			UnitDataSets[unit->getPlayer()][UnitTypes::Enum::Zerg_Drone].RemoveUnit(unit);
		else
			UnitDataSets[unit->getPlayer()][unitType.whatBuilds().first].RemoveUnit(unit);

		UnitDataSets[unit->getPlayer()][unitType].AddUnit(unit);
	}

	void IntelligenceCommand::OnUnitCreate(Unit unit)
	{
		if (unit->getPlayer() == Broodwar->self())
		{
			UnitType unitType = unit->getType();

			//todo : uncomment these lines
			/*if (unitType == BWAPI::UnitTypes::Enum::Enum::Zerg_Larva)
				UnitDataSets[unit->getPlayer()][unitType].m_units.emplace(unit);

			else if (unitType == BWAPI::UnitTypes::Zerg_Zergling)
				UnitDataSets[unit->getPlayer()][unitType].m_units.emplace(unit);

			else if (unitType == BWAPI::UnitTypes::Zerg_Scourge)
				UnitDataSets[unit->getPlayer()][unitType].m_units.emplace(unit);

			if (UnitTypes::Zerg_Egg == unitType)
				return;*/
			UnitDataSets[unit->getPlayer()][unitType].AddUnit(unit);
		}
	}

	void IntelligenceCommand::OnUnitShow(Unit unit)
	{
		// if it's tournament, then try to use enemies
		if (unit->getPlayer() == Broodwar->enemy())
		{
			//since it's not the discover function, we should check whether it is in container or new
			UnitType unitType = unit->getType();

			if (unitType.isResourceDepot())
			{
				foundEnemyBase = true;
				enemyBase.emplace_back(unit->getTilePosition());
			}

			//if (unitType.isBuilding())
			{

				//UnitDataSets[unit->getPlayer()][unitType].AddUnit(HOLD::UnitData(unit));

				//if the container doesn't exist, it's new, just add it
				if (UnitDataSets[unit->getPlayer()].find(unitType) != UnitDataSets[unit->getPlayer()].end())
				{
					//if the container exist, check this is new or old
					if (UnitDataSets[unit->getPlayer()][unitType].m_savedUnits.find(unit->getID()) != UnitDataSets[unit->getPlayer()][unitType].m_savedUnits.end())
					{
						UnitDataSets[unit->getPlayer()][unitType].UpdateUnit(unit);

					}
					else
					{
						UnitDataSets[unit->getPlayer()][unitType].AddUnit(unit);
					}


				}
				else
				{
					UnitDataSets[unit->getPlayer()][unitType].AddUnit(unit);
				}
			}
			//if (UnitDataSets[unit->getPlayer()][unitType].m_savedUnits[unit->getID()].m_lastPlayer == nullptr)
			//{
			//	UnitDataSets[unit->getPlayer()][unitType].AddUnit(unit);
			//}



			//if(UnitDataSets[unit->getPlayer()][unitType].m_savedUnits[unit->getID()].m_lastPlayer != unit->getPlayer())
			//{
			//	UnitDataSets[UnitDataSets[unit->getPlayer()][unitType].m_savedUnits[unit->getID()].m_lastPlayer][unitType].RemoveUnit(unit);
			//	UnitDataSets[unit->getPlayer()][unitType].AddUnit(unit);// .m_savedUnits[unit->getID()]..m_lastPlayer. = unit->getPlayer()
			//}
			//else
			//{
			//	UnitDataSets[unit->getPlayer()][unitType].UpdateUnit(unit);
			//}

			/*	UnitDataSets[unit->getPlayer()][unit->getType()].m_savedUnits[unit->getID()].m_lastPosition = unit->getPosition();
			UnitDataSets[unit->getPlayer()][unit->getType()].m_savedUnits[unit->getID()].m_lastSeen = Broodwar->getFrameCount();
			UnitDataSets[unit->getPlayer()][unit->getType()].m_savedUnits[unit->getID()].m_lastType = unit->getType();*/
		}
	}

	void IntelligenceCommand::OnUnitDestroy(Unit unit)
	{
		UnitDataSets[unit->getPlayer()][unit->getType()].RemoveUnit(unit);
	}
}//namespace HOLD
