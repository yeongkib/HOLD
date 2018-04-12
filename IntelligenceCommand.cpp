/*****************************************************************************/
/*!
\file   IntelligenceCommand.cpp
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
#include <BWAPI/UnitCommand.h>
#include "Grid.hpp"
#include "../BWEM_1_3/bwem.h"

#include "Drone.hpp"
#include <BWAPI/Client/Event.h>

namespace { auto & theMap = BWEM::Map::Instance(); }


namespace HOLD
{
	//not sure containing current position(0,0)
	std::array<std::tuple<int, int, char>, 8> dirs =
	{
		std::make_tuple(-1, -1, 'UL'), std::make_tuple(0, -1, 'UM'), std::make_tuple(1, -1, 'UR'),
		std::make_tuple(-1, 0, 'ML'),/*std::make_tuple(0, 0, 'MM'),*/std::make_tuple(1, 0, 'MR'),
		std::make_tuple(-1, 1, 'BL'), std::make_tuple(0, 1, 'BM'), std::make_tuple(1, 1, 'BR')
	};

	std::array<std::tuple<int, int, char>, 8> walktTileOffsets =
	{
		std::make_tuple(-8, -8, 'UL'), std::make_tuple(0, -8, 'UM'), std::make_tuple(8, -8, 'UR'),
		std::make_tuple(-8, 0, 'ML'),/*std::make_tuple(0, 0, 'MM'),*/std::make_tuple(8, 0, 'MR'),
		std::make_tuple(-8, 8, 'BL'), std::make_tuple(0, 8, 'BM'), std::make_tuple(8, 8, 'BR')
	};

	/*
	*todo : to attack safely,
	* 1. find more safety place along the forward direction to target
	* 2. until the target is in attack radius
	* 3. once it got into the range
	* 4. try to hit and run
	*
	* status : attackmove
	* attackunit
	* move
	*
	*
	* 1. get direction vector
	* 2. normalize
	* 3. set flag depen on diffrence between x and y coordinates
	* 4.
	*/

	BWAPI::Position FindMostSafetyZone_Attack(std::array<double, 256*256> &inf_map, BWAPI::Position pos, BWAPI::Position& dir, bool ground = false) {
		std::vector<WalkPosition> candidates;

		std::vector<bool> neighbor(8, false);

		/*
		*  +--+--+     +-+-+-+
		*  | 0| 1|     |0|1|2|
		*  |  |  |	   +-+-+-+
		*  +--+--+	-> |3| |4|
		*  | 2| 3|	   +-+-+-+
		*  |  |  |	   |5|6|7|
		*  +--+--+	   +-+-+-+
		*
		* case 0 : x < 0 && y < 0
		* -> include 0,1,3
		*
		* case 1 : x > 0 && y < 0
		* -> include 1,2,4
		*
		* case 2 : x > 0 && y > 0
		* -> include 4,6,7
		*
		* case 3 : x < 0 && y > 0
		* -> include 3,5,6
		*/
		// I don't think pos will be equal to dir. Which means the unit and target at the same position
		// but just in case...
		if (dir.x <= 0 && dir.y <= 0) {
			neighbor[0] =
			neighbor[1] =
			neighbor[3] = true;
		}
		if (dir.x >= 0 && dir.y <= 0) {
			neighbor[1] =
			neighbor[2] =
			neighbor[4] = true;
		}
		if (dir.x >= 0 && dir.y >= 0) {
			neighbor[4] =
			neighbor[6] =
			neighbor[7] = true;
		}
		if (dir.x <= 0 && dir.y >= 0) {
			neighbor[3] =
			neighbor[5] =
			neighbor[6] = true;
		}

		int lowest = (std::numeric_limits<int>::max)();
		for (int i = 0; i < 8; ++i)
		{
			if (!neighbor[i])
				continue;

			int dx = std::get<0>(walktTileOffsets[i]);
			int dy = std::get<1>(walktTileOffsets[i]);

			WalkPosition targetWalkPosition{ pos.x / 8 + dx, pos.y / 8 + dy };
			
			if (targetWalkPosition.isValid()) {
				if (ground)
					if (!Broodwar->isWalkable(targetWalkPosition))
						continue;

				const int influence = HOLD::GetInfluenceValue(inf_map, pos.x + std::get<0>(walktTileOffsets[i]), pos.y + std::get<1>(walktTileOffsets[i]));// inf_map[targetPosition.y * mapHeight + targetPosition.x];

				if (influence < lowest) {
					std::vector<WalkPosition>().swap(candidates);
					candidates.push_back(targetWalkPosition);
					lowest = influence;
				} else if (lowest == influence) {
					candidates.push_back(targetWalkPosition);
				}
			}
		}

		if (candidates.size()) {
			Position returnPosition{ pos };
			int closest = (std::numeric_limits<int>::max)();

			for (WalkPosition candidate : candidates) {
				//not sure which function is better (getApproxDistance or getDistance)
				Position target(candidate.x * 8 + pos.x % 8, candidate.y * 8 + pos.y % 8);
				const int distance = pos.getApproxDistance(target);

				if (distance < closest) {
					closest = distance;
					returnPosition = target;
				}
			}
			return returnPosition;
		} else {
			if (candidates.empty())
				return pos;
			//Broodwar->leaveGame();

			//return Position{ candidates.at(0).x * 32 + pos.x % 32, candidates.at(0).y * 32 + pos.y % 32 };
		}
	};

	//todo : use astar within 2tile size with path length n for better place
	BWAPI::Position FindMostSafetyZone_Flee(std::array<double, 256*256> &inf_map, BWAPI::Position pos, bool ground = false) {
		std::vector<WalkPosition> candidates;

		int lowest = (std::numeric_limits<int>::max)();
		for (int i = 0; i < 8; ++i) {
			int dx = std::get<0>(walktTileOffsets[i]);
			int dy = std::get<1>(walktTileOffsets[i]);

			WalkPosition targetWalkPosition{ pos.x / 8 + pos.x % 8 + dx, pos.y / 8 + pos.y % 8 + dy };
			
			if (targetWalkPosition.isValid()) {
				if (ground)
					if (!Broodwar->isWalkable(targetWalkPosition))
						continue;

				const int influence = HOLD::GetInfluenceValue(inf_map, pos.x + std::get<0>(walktTileOffsets[i]), pos.y + std::get<1>(walktTileOffsets[i]));// inf_map[targetPosition.y * mapHeight + targetPosition.x];

				if (influence < lowest) {
					std::vector<WalkPosition>().swap(candidates);
					candidates.push_back(targetWalkPosition);
					lowest = influence;
				} else if (lowest == influence) {
					candidates.push_back(targetWalkPosition);
				}
			}
		}

		//todo : if all the candidates have same values, 
		if (candidates.size()) {
			Position returnPosition{ pos };
			int closest = (std::numeric_limits<int>::max)();

			for (WalkPosition candidate : candidates) {
				Position target(candidate.x * 8 + pos.x % 8, candidate.y * 8 + pos.y % 8);

				const int distance = pos.getApproxDistance(target);

				if (distance < closest) {
					closest = distance;
					returnPosition = target;
				}
			}
			return returnPosition;
		} else {
			
		}

	};

	IntelligenceCommand::IntelligenceCommand() : m_idStack(0), mapWidth(0), mapHeight(0), runflag(true) {
	}

	IntelligenceCommand::~IntelligenceCommand() {
	}

	void IntelligenceCommand::Init() {
		mapWidth = BWAPI::Broodwar->mapWidth();
		mapHeight = BWAPI::Broodwar->mapHeight();

		if (strcmp(Broodwar->mapHash().c_str(), "ed7c5b1b03234a0f7dd484112bbb1bc49db1d6f0") == 0) // testcase
			runflag = false;
		// getting starting position
		TilePosition starting{};
		if(Broodwar->self())
		starting = Broodwar->self()->getStartLocation();

		if (runflag) {
			theMap.Initialize();
			theMap.EnableAutomaticPathAnalysis();
			bool startingLocationOK = theMap.FindBasesForStartingLocations();
			assert(startingLocationOK);
			startingLocations = theMap.StartingLocations();
		
			for (const BWEM::Area & area : theMap.Areas()) {
				for (const BWEM::Base & base : area.Bases()) {
					/*if (starting.x == base.Location().x
					&& starting.y == base.Location().y)*/
					{
						std::vector<BWEM::Ressource *> AssignedResources(base.Minerals().begin(), base.Minerals().end());
					}
				}
			}

			enemyBaseCandidate = theMap.StartingLocations();

			TilePosition myStartingPos{};
			if(Broodwar->self())
			myStartingPos = Broodwar->self()->getStartLocation();

			std::swap(enemyBaseCandidate.front(), *find(enemyBaseCandidate.begin(), enemyBaseCandidate.end(), myStartingPos));

			const BWEM::Area * pMyStartingArea = theMap.GetArea(myStartingPos);
			BWEM::utils::really_remove_if(enemyBaseCandidate, [pMyStartingArea](TilePosition t)
			{ return !theMap.GetArea(t)->AccessibleFrom(pMyStartingArea); });

			if (enemyBaseCandidate.size() > 2) {
				// sorts m_HisPossibleLocations, making each element the nearest one from the previous one
				for (int i = 1; i < (int)enemyBaseCandidate.size(); ++i) {
					TilePosition lastPos = enemyBaseCandidate[i - 1];
					for (int j = i + 1; j < (int)enemyBaseCandidate.size(); ++j) {
						int groundDist_lastPos_i;
						int groundDist_lastPos_j;
						theMap.GetPath(Position(lastPos), Position(enemyBaseCandidate[i]), &groundDist_lastPos_i);
						theMap.GetPath(Position(lastPos), Position(enemyBaseCandidate[j]), &groundDist_lastPos_j);
						if (groundDist_lastPos_j < groundDist_lastPos_i)
							std::swap(enemyBaseCandidate[i], enemyBaseCandidate[j]);
					}
				}
			}
			BWEM::utils::really_remove(enemyBaseCandidate, myStartingPos);

			//https://stackoverflow.com/questions/6989100/sort-points-in-clockwise-order
			TilePosition center = TilePosition(Broodwar->mapWidth() / 2, Broodwar->mapHeight() / 2);
			auto less = [&center](TilePosition a, TilePosition b)->bool {
				if (a.x - center.x >= 0 && b.x - center.x < 0)
					return true;
				if (a.x - center.x < 0 && b.x - center.x >= 0)
					return false;
				if (a.x - center.x == 0 && b.x - center.x == 0) {
					if (a.y - center.y >= 0 || b.y - center.y >= 0)
						return a.y > b.y;
					return b.y > a.y;
				}

				// compute the cross product of vectors (center -> a) x (center -> b)
				int det = (a.x - center.x) * (b.y - center.y) - (b.x - center.x) * (a.y - center.y);
				if (det > 0)
					return true;
				if (det < 0)
					return false;

				// points a and b are on the same line from the center
				// check which point is closer to the center
				int d1 = (a.x - center.x) * (a.x - center.x) + (a.y - center.y) * (a.y - center.y);
				int d2 = (b.x - center.x) * (b.x - center.x) + (b.y - center.y) * (b.y - center.y);
				return d1 > d2;
			};
			if (enemyBaseCandidate.size() > 2)
				std::sort(begin(startingLocations), end(startingLocations), less);
		}

		InitInfluenceMaps();
	}

	void IntelligenceCommand::Update(int dt) {
		if (runflag) {
			UpdateLifePoint();

			for (const BWEM::Area & area : theMap.Areas()) {
				for (const BWEM::Base & base : area.Bases()) {
					/*if (starting.x == base.Location().x
					&& starting.y == base.Location().y)*/
					{
						std::vector<BWEM::Ressource *> AssignedResources(base.Minerals().begin(), base.Minerals().end());

						for (auto &mineral : AssignedResources)
						{
							DrawBoundary(mineral->Unit());
						}
					}
				}
			}
		}

		//if (BWAPI::Broodwar->getFrameCount() % 4)
		{
			ClearInfluenceMaps();
			UpdateBulletInfo(); // for bullets


								//todo : call updateinfluences, updateGrid
								// Iterate through all the units that we own
			for (auto player : Broodwar->enemies()) {
				for (const auto &enemyUnits : Units[player]) {
					for (const auto u : enemyUnits.second)
						//for(auto & u : Broodwar->enemy()->getUnits())
					{
						if (u->getPosition() != Positions::Unknown) {
							UpdateInfluences(u->m_unit);
						}
					}
				}
			}

			Position attackPosition{};

			if (!enemyBase.empty()) {
				attackPosition = Position{ enemyBase[0].x * 32, enemyBase[0].y * 32 };
			} else if (!enemyBaseCandidate.empty()) {
				attackPosition = Position{ enemyBaseCandidate[0].x * 32, enemyBaseCandidate[0].y * 32 };
			} else {
				for (const auto &enemyUnits : Units[Broodwar->enemy()]) {
					for (const auto u : enemyUnits.second) {
						if (u->getType().isBuilding()) {
							attackPosition = u->getPosition();
							break;
						}
					}
					if (attackPosition.isValid())
						break;
				}
			}
			for (auto & u : BWAPI::Broodwar->self()->getUnits()) {
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
				DrawBoundary(u);

				DrawWeaponCooldown(u);
				/*Broodwar->drawTextMap(u->getPosition(), "%d", UnitInfo(u).GetAverageHP());*/

				/*
				* 1. get near enemy units
				* 2. cal minimum hits to kill each unit
				* 3. cal exact time(frames) to kill each unit
				* 4. set lowest as the target
				*/

				if (//BWAPI::Broodwar->getFrameCount() % 6 == 0 &&
					//!u->isAttacking()
					//cooldown > 0.5f
					!u->isAttackFrame())
				{
					UnitType type = u->getType();

					Unitset enemiesInSight{ u->getUnitsInRadius(Filter::SightRange(u), Filter::IsEnemy) };

					std::vector<std::pair<Unit, int>> candidates{};

					int minimum_hit{ std::numeric_limits<int>::max() };
					int minimum_time{ minimum_hit };

					for (const auto& enemyInSight : enemiesInSight) {
						if (!u->canAttackUnit(enemyInSight)) continue;

						int total_life = enemyInSight->getHitPoints() + enemyInSight->getShields();
						int damageto = BWAPI::Broodwar->getDamageTo(enemyInSight->getType(), type, enemyInSight->getPlayer(), BWAPI::Broodwar->self());
						if (damageto == 0) // todo: add sumvalues for the spell units
							continue;
						int hitsToKill = total_life / damageto;

						if (total_life % damageto != 0)
							hitsToKill += 1;

						if (hitsToKill == minimum_hit) {
							candidates.emplace_back(std::make_pair(enemyInSight, hitsToKill));
						}
						if (hitsToKill < minimum_hit) {
							minimum_hit = hitsToKill;
							candidates.clear();
							candidates.emplace_back(std::make_pair(enemyInSight, hitsToKill));
						}
					}

					Unit target{};

					for (const auto& candidate : candidates) {
						bool targetInAir = candidate.first->isFlying();
						int weapon_cooldown = targetInAir ? u->getGroundWeaponCooldown() : u->getAirWeaponCooldown();
						int timeToKill = weapon_cooldown + (candidate.second - 1) * BWAPI::Broodwar->self()->weaponDamageCooldown(type);


						if (timeToKill < minimum_time) {
							minimum_time = timeToKill;
							target = candidate.first;
						}
					}

					if (target) {
						if (!u || !u->exists() || u->getPlayer() != BWAPI::Broodwar->self() ||
						    !target || !target->exists()) {
							continue;
						}

						// if we have issued a command to this unit already this frame, ignore this one
						if (u->getLastCommandFrame() >= BWAPI::Broodwar->getFrameCount() || u->isAttackFrame()) {
							continue;
						}

						//// get the unit's current command
						BWAPI::UnitCommand currentCommand(u->getLastCommand());

						// if we've already told this unit to attack this target, ignore this command
						if (currentCommand.getType() == BWAPI::UnitCommandTypes::Attack_Unit &&	currentCommand.getTarget() == target) {
							continue;
						}

						// if nothing prevents it, attack the target
						u->attack(target);

						Position targetpos = target->getPosition();
						if (targetpos != Positions::Invalid
							&& targetpos != Positions::None
							&& targetpos != Positions::Unknown
							&& targetpos != Positions::Origin) {
							BWAPI::Position fleeVec(u->getPosition() - targetpos);
							double fleeAngle = atan2(fleeVec.y, fleeVec.x);
							fleeVec = BWAPI::Position(static_cast<int>(64.0 * cos(fleeAngle)), static_cast<int>(64.0 * sin(fleeAngle)));
							
							{
								//todo : How can I handle simple moving case?
										
								if (u->attack(target)) {
									if (u->getType().groundWeapon().maxRange() < 30)
										continue;

									float cooldown = static_cast<float>(u->getGroundWeaponCooldown()) / static_cast<float>(u->getType().groundWeapon().damageCooldown());
									if(cooldown < 0.5f)
										u->move(FindMostSafetyZone_Flee(u->getType().isFlyer() ? *GetGridMap(std::string{ "opinfluenceAir" }) : *GetGridMap(std::string{ "opinfluenceGround" }), u->getPosition(), u->getType().isFlyer()));
								}
							}
						}							
					}
				}

				if (runflag) {
					if (BWAPI::Broodwar->getFrameCount() % 25 == 0) {
						const bool targetAlreadyReached = !BWEM::utils::contains(enemyBaseCandidate, TilePosition(u->getTargetPosition()));
						if (targetAlreadyReached ||
							u->getPosition().getApproxDistance(u->getTargetPosition() + Position(UnitType(UnitTypes::Terran_Command_Center).tileSize()) / 2) < 32 * 6)
						{
							if (enemyBaseCandidate.size() > 1)
								if (!targetAlreadyReached)
									BWEM::utils::really_remove(enemyBaseCandidate, TilePosition(u->getTargetPosition()));
						}

						//todo fix it
						if (u->getType() == UnitTypes::Zerg_Zergling) {
							if (Units[Broodwar->self()][u->getType()].size() >= 2) {
								if (!(u->getOrder() == Orders::AttackMove)
								 || !(u->getOrder() == Orders::AttackUnit)) {
									u->issueCommand(BWAPI::UnitCommand::attack(nullptr, attackPosition));
								}
							}
						} else if (u->getType() == UnitTypes::Zerg_Mutalisk) {
							if (firstattack_mutal || Units[Broodwar->self()][u->getType()].size() >= 10)
								if (!(u->getOrder() == Orders::AttackMove)
							     || !(u->getOrder() == Orders::AttackUnit)) {
									firstattack_mutal = true;
									u->issueCommand(BWAPI::UnitCommand::attack(nullptr, attackPosition));
								}
						} else if (u->getType() == UnitTypes::Zerg_Hydralisk) {
							if (firstattack_hydra || Units[Broodwar->self()][u->getType()].size() >= 20)
								if (!(u->getOrder() == Orders::AttackMove)
								 || !(u->getOrder() == Orders::AttackUnit)) {
									firstattack_hydra = true;
									u->issueCommand(BWAPI::UnitCommand::attack(nullptr, attackPosition));
								}
						}
					}
				}
			}
			UpdateGrid();
		}

		int uy = 50;
		for (auto lists : {
			UnitTypes::Zerg_Overlord,
			UnitTypes::Zerg_Drone,
			UnitTypes::Zerg_Zergling,
			UnitTypes::Zerg_Hydralisk,
			UnitTypes::Zerg_Mutalisk
		}) {
			const auto i = Units[Broodwar->self()][lists];
			if (i.size()) {
				Broodwar->drawTextScreen(5, uy, "%s %d", lists.c_str(), i.size());
				uy += 10;
			}
		}
		int by = 50;
		for (auto lists : {
			UnitTypes::Zerg_Hatchery,
			UnitTypes::Zerg_Lair,
			UnitTypes::Zerg_Hive,
			UnitTypes::Zerg_Extractor,
			UnitTypes::Zerg_Spawning_Pool,
			UnitTypes::Zerg_Hydralisk_Den,
			UnitTypes::Zerg_Spire
		}) {
			const auto i = Units[Broodwar->self()][lists];
			if (i.size()) {
				Broodwar->drawTextScreen(150, by, "%s %d", lists.c_str(), i.size());
				by += 10;
			}
		}

		int ey = 50;
		for (auto eus : Units[Broodwar->enemy()]) {
			auto us = Units[Broodwar->enemy()].find(eus.first);
			if (us != Units[Broodwar->enemy()].end()) {
				if (us->second.size()) {
					//Broodwar->drawTextScreen(280, ey, "%s %d", (*us->second.m_units.begin())->getType().getName().c_str(), us->second.m_units.size());
					Broodwar->drawTextScreen(280, ey, "%s %d", UnitType(eus.first).c_str(), us->second.size());
					ey += 10;
				}
			}
		}
	}

	void IntelligenceCommand::Shutdown() {
		for (auto ds : Units) {
			ds.second.clear();
		}
	}

	void IntelligenceCommand::ProcessMessage(Message* message) {
	}

	void IntelligenceCommand::BroadcastMessage(Message* message) {
	}
	
	const int IntelligenceCommand::GetMapHeight() const {
		return mapHeight;
	}

	const int IntelligenceCommand::GetMapWidth() const {
		return mapWidth;
	}

	void IntelligenceCommand::UpdateGrid() {
		std::transform(grids[std::string{ "tensionGround" }].begin(), grids[std::string{ "tensionGround" }].end(), grids[std::string{ "influenceGround" }].begin(),
			grids[std::string{ "vulGround" }].begin(), [](double &first, double &second)->double
		{
			return Grid(first - HOLD::Abs(second)).quarter.u_double;
		});

		std::transform(grids[std::string{ "tensionAir" }].begin(), grids[std::string{ "tensionAir" }].end(), grids[std::string{ "influenceAir" }].begin(),
			grids[std::string{ "vulAir" }].begin(), [](double& first, double &second)->double
		{
			return Grid(first - HOLD::Abs(second)).quarter.u_double;
		});

		std::transform(grids[std::string{ "vulGround" }].begin(), grids[std::string{ "vulGround" }].end(), grids[std::string{ "vulAir" }].begin(),
			grids[std::string{ "vulTotal" }].begin(), [](double& first, double &second)->double
		{
			return Grid(first + second).quarter.u_double;
		});
	}

	Vgrid* IntelligenceCommand::GetGridMap(const std::string& name) {
		//todo : assert if not found
		auto it = grids.find(name);
		if (it != grids.end()) {
			return &it->second;
		}

		return nullptr;
	}

	void IntelligenceCommand::UpdateBulletInfo() {
		for (auto &b : Broodwar->getBullets()) {
			Position p = b->getPosition();
			double velocityX = b->getVelocityX();
			double velocityY = b->getVelocityY();

			if (b->getType() == BulletTypes::Enum::Psionic_Storm
			 || b->getType() == BulletTypes::Enum::Plague_Cloud) {
				// Broodwar->drawCircleMap(b->getPosition(), 50, Colors::Red);
				UpdateInfluences(b);
			}
		}
	}

	void IntelligenceCommand::UpdateInfluences(const Bullet b) {
		if (b->getType() == BulletTypes::Enum::Psionic_Storm) {
			constexpr int storms[61] = { 
											14, 14, 14, 14, 14, 14, 14, 14,
											28, 28, 28, 28, 28, 28, 28,
											42, 42, 42, 42, 42, 42, 42,
											56, 56, 56, 56, 56, 56, 56,
											70, 70, 70, 70, 70, 70, 70,
											84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84,
											98, 98, 98, 98, 98, 98, 98,
											112, 112, 112, 112
										};
			int xPos = b->getPosition().x;// / 32;
			int yPos = b->getPosition().y;// / 32;
			float Radius = 50;

			int StartX = static_cast<int>(floor((xPos - Radius) / 32.f));
			int StartY = static_cast<int>(floor((yPos - Radius) / 32.f));

			int EndX = static_cast<int>(ceil((xPos + Radius) / 32.f));
			int EndY = static_cast<int>(ceil((yPos + Radius) / 32.f));

			//need to clamp
			StartX = Math::Clamp(StartX, 0, mapWidth - 1);
			StartY = Math::Clamp(StartY, 0, mapHeight - 1);
			EndX = Math::Clamp(EndX, 0, mapWidth - 1);
			EndY = Math::Clamp(EndY, 0, mapHeight - 1);

			int basedamage = 14;
			int life = b->getRemoveTimer();
			//Broodwar->drawTextMap(xPos, yPos, "%d", life);
			//int total_damage = basedamage * life;
			int total_damage = storms[b->getRemoveTimer()];

			for (int Y = StartY; Y < EndY; ++Y) {
				for (int X = StartX; X < EndX; ++X) {
					SetInfluence(X, Y, xPos, yPos, total_damage, std::plus<short>(), 0, Radius,
						5,
						&GetGridMap(std::string{ "opinfluenceGround" })->at(Y * mapHeight + X),
						&GetGridMap(std::string{ "tensionGround" })->at(Y * mapHeight + X),
						&GetGridMap(std::string{ "tensionTotal" })->at(Y * mapHeight + X),
						&GetGridMap(std::string{ "opinfluenceAir" })->at(Y * mapHeight + X),
						&GetGridMap(std::string{ "tensionAir" })->at(Y * mapHeight + X));
					SetInfluence(X, Y, xPos, yPos, total_damage, std::minus<short>(), 0, Radius,
						1,
						&GetGridMap(std::string{ "influenceGround" })->at(Y * mapHeight + X));
				}
			}
		}

		//todo : set zero ground influence
		if (b->getType() == BulletTypes::Enum::Plague_Cloud)
		{
		}
	}

	void IntelligenceCommand::UpdateInfluences(const Unit u) {
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

				//todo : airweapon?
				airStartX = Math::Clamp(airStartX, 0, mapWidth - 1);
				airStartY = Math::Clamp(airStartY, 0, mapHeight - 1);
				airEndX = Math::Clamp(airEndX, 0, mapWidth - 1);
				airEndY = Math::Clamp(airEndY, 0, mapHeight - 1);

				int level = u->getPlayer()->getUpgradeLevel(groundWeapon.upgradeType());
				int damage = (groundWeapon.damageAmount() + level * groundWeapon.damageBonus()) * 4;

				for (int Y = groundStartY; Y < groundEndY; ++Y) {
					for (int X = groundStartX; X < groundEndX; ++X) {
						SetInfluence(X, Y, x, y, damage, std::plus<short>(), 0, groundRadius,
							4,
							&GetGridMap(std::string{ "opinfluenceGround" })->at(Y * mapHeight + X),
							&GetGridMap(std::string{ "opinfluenceAir" })->at(Y * mapHeight + X),
							&GetGridMap(std::string{ "tensionGround" })->at(Y * mapHeight + X),
							&GetGridMap(std::string{ "opinfluenceGround" })->at(Y * mapHeight + X));
						SetInfluence(X, Y, x, y, damage, std::minus<short>(), 0, groundRadius,
							2,
							&GetGridMap(std::string{ "influenceGround" })->at(Y * mapHeight + X),
							&GetGridMap(std::string{ "influenceGround" })->at(Y * mapHeight + X));
					}
				}
			}
		} else {
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

			if (groundWeapon.damageAmount()) {
				int level = u->getPlayer()->getUpgradeLevel(groundWeapon.upgradeType());
				int damage = (groundWeapon.damageAmount() + level * groundWeapon.damageBonus());// *groundWeapon.damageFactor(); //
				//int damage = static_cast<int>(static_cast<double>((groundWeapon.damageAmount() + level * groundWeapon.damageBonus())) * (static_cast<double>(groundWeapon.damageCooldown() - u->getGroundWeaponCooldown()) / static_cast<double>(groundWeapon.damageCooldown())));
				if (damage)
					for (int Y = groundStartY; Y < groundEndY; ++Y) {
						for (int X = groundStartX; X < groundEndX; ++X) {
							if (u->getPlayer() == Broodwar->self()) {
								SetInfluence(X, Y, x, y, damage, std::plus<short>(), groundWeapon.minRange(), groundRadius,
									3, &GetGridMap(std::string{ "influenceGround" })->at(Y * mapHeight + X),
									&GetGridMap(std::string{ "tensionGround" })->at(Y * mapHeight + X),
									&GetGridMap(std::string{ "tensionTotal" })->at(Y * mapHeight + X));
							} else {//if enemy
								SetInfluence(X, Y, x, y, damage, std::plus<short>(), groundWeapon.minRange(), groundRadius,
									3, &GetGridMap(std::string{ "opinfluenceGround" })->at(Y * mapHeight + X),
									&GetGridMap(std::string{ "tensionGround" })->at(Y * mapHeight + X),
									&GetGridMap(std::string{ "tensionTotal" })->at(Y * mapHeight + X));
								SetInfluence(X, Y, x, y, damage, std::minus<short>(), groundWeapon.minRange(), groundRadius,
									1, &GetGridMap(std::string{ "influenceGround" })->at(Y * mapHeight + X));
							}
						}
					}
			}
			//todo : if(airweapon == groundweapon) pass airinfluence map at the sametime
			if (airWeapon.damageAmount()) {
				int level = u->getPlayer()->getUpgradeLevel(airWeapon.upgradeType());
				int damage = (airWeapon.damageAmount() + level * airWeapon.damageBonus());// *groundWeapon.damageFactor(); //
				//int damage = static_cast<int>(static_cast<double>((airWeapon.damageAmount() + level * airWeapon.damageBonus())) * (static_cast<double>(airWeapon.damageCooldown() - u->getAirWeaponCooldown()) / static_cast<double>(airWeapon.damageCooldown())));
				if (damage)
					for (int Y = airStartY; Y < airEndY; ++Y) {
						for (int X = airStartX; X < airEndX; ++X) {
							if (u->getPlayer() == Broodwar->self()) {
								SetInfluence(X, Y, x, y, damage, std::plus<short>(), airWeapon.minRange(), airRadius,
									3, &GetGridMap(std::string{ "influenceAir" })->at(Y * mapHeight + X),
									&GetGridMap(std::string{ "tensionAir" })->at(Y * mapHeight + X),
									&GetGridMap(std::string{ "tensionTotal" })->at(Y * mapHeight + X));
							} else {//if enemy
								SetInfluence(X, Y, x, y, damage, std::plus<short>(), airWeapon.minRange(), airRadius,
									3, &GetGridMap(std::string{ "opinfluenceAir" })->at(Y * mapHeight + X),
									&GetGridMap(std::string{ "tensionAir" })->at(Y * mapHeight + X),
									&GetGridMap(std::string{ "tensionTotal" })->at(Y * mapHeight + X));
								SetInfluence(X, Y, x, y, damage, std::minus<short>(), airWeapon.minRange(), airRadius,
									1, &GetGridMap(std::string{ "influenceAir" })->at(Y * mapHeight + X));
							}
						}
					}
			}
		}
	}

	void IntelligenceCommand::InitInfluenceMaps() {
		std::array<double, 256 * 256> maps;
		grids[std::string{"opinfluenceGround"}] = maps;
		grids[std::string{"opinfluenceAir"}]    = maps;
		grids[std::string{"influenceGround"}]   = maps;
		grids[std::string{"influenceAir"}]      = maps;
		grids[std::string{"tensionGround"}]     = maps;
		grids[std::string{"tensionAir"}]        = maps;
		grids[std::string{"tensionTotal"}]      = maps;
		grids[std::string{"vulGround"}]         = maps;
		grids[std::string{"vulAir"}]            = maps;
		grids[std::string{"vulTotal"}]          = maps;
	}

	void IntelligenceCommand::ClearInfluenceMaps() {
		for(auto grid: grids) {
			memset(&grid.second[0], 0, sizeof(grid.second[0]) * grid.second.size());
		}
	}

	std::unordered_map<BWAPI::Player, std::unordered_map<int, UnitInfoset>>* IntelligenceCommand::GetUnitDataSets() {
		return &Units;
	}

	void IntelligenceCommand::AddUnit(Unit u) {
		Units[u->getPlayer()][u->getType()].emplace(new UnitInfo(u));
	}

	void IntelligenceCommand::OnUnitMorph(Unit unit) {
		UnitType unitType = unit->getType();

		if (unitType.isBuilding())
			Units[unit->getPlayer()][UnitTypes::Enum::Zerg_Drone].RemoveUnit(unit);
		else
			Units[unit->getPlayer()][unitType.whatBuilds().first].RemoveUnit(unit);

		Units[unit->getPlayer()][unitType].AddUnit(unit);
	}

	void IntelligenceCommand::OnUnitCreate(Unit unit) {
		if (unit->getPlayer() == Broodwar->self()) 	{
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
			Units[unit->getPlayer()][unitType].AddUnit(unit);
		}
	}

	void IntelligenceCommand::OnUnitShow(Unit unit) {
		if (Broodwar->self()->isEnemy(unit->getPlayer())) {
			//since it's not the discover function, we should check whether it is in container or new
			UnitType unitType = unit->getType();

			if (unitType.isResourceDepot()) {
				if (enemyBase.empty()) {
					auto IsExpansion = std::find(begin(startingLocations), end(startingLocations), unit->getTilePosition());
					if (IsExpansion == end(startingLocations)) {
						int shortest = std::numeric_limits<int>::max();
						TilePosition orgBase{};
						for (auto candidate : startingLocations) {
							int distance = unit->getTilePosition().getApproxDistance(candidate);
							if (distance < shortest)
								orgBase = candidate;
						}
						enemyBase.emplace_back(orgBase);
						//todo: find original base
					}
					else
						enemyBase.emplace_back(unit->getTilePosition());
				}
			}

			//if (unitType.isBuilding())
			{

				//UnitDataSets[unit->getPlayer()][unitType].AddUnit(HOLD::UnitData(unit));

				//if the container doesn't exist, it's new, just add it
				if (Units[unit->getPlayer()].find(unitType) != Units[unit->getPlayer()].end()) {
					Units[unit->getPlayer()][unitType].UpdateUnit(unit);
				} else {
					Units[unit->getPlayer()][unitType].AddUnit(unit);
				}
			}
		}
	}

	void IntelligenceCommand::OnUnitDestroy(Unit unit) {
		Units[unit->getPlayer()][unit->getType()].RemoveUnit(unit);

		if (unit->getType().isMineralField()) {
			theMap.OnMineralDestroyed(unit);
		} else if (unit->getType().isSpecialBuilding()) {
			theMap.OnStaticBuildingDestroyed(unit);
		}

		if (unit->getType().isResourceDepot() && unit->getPlayer() != Broodwar->self())
			enemyBase.erase(std::find(enemyBase.begin(), enemyBase.end(), unit->getTilePosition()));
	}

	void IntelligenceCommand::UpdateLifePoint() {
		for(auto units : Units[BWAPI::Broodwar->self()]) {
			for(auto m_unit : units.second) {
				m_unit->m_prevHP[BWAPI::Broodwar->getFrameCount() % 256] = m_unit->m_unit->getHitPoints();
			}
		}
	}
}//namespace HOLD
