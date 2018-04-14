/*****************************************************************************/
/*!
\file   IntelligenceCommand.hpp
\author Yeongki Baek
\par    email: yeongki.baek\@digipen.edu
\date   08/01/2017
\brief
This is the interface file for the module
Copyright 2017, Digipen Institute of Technology
*/
/*****************************************************************************/

#pragma once

#include "Command.hpp"
#include "HeadQuarters.hpp"
#include "UnitInfo.hpp"
#include "UnitInfoset.hpp"
#include <memory>
#include <BWAPI/Client/UnitImpl.h>


namespace HOLD
{
	typedef std::array<double, 256*256> Vgrid;

	class Message;
		
	class IntelligenceCommand : public Command
	{
	public:
		IntelligenceCommand();
		virtual ~IntelligenceCommand() = default;

		void Init() override;
		void Update(int dt) override;
		void Shutdown() override;
		void ProcessMessage(Message* message) override;
		void BroadcastMessage(Message* message);
		void UpdateLifePoint();

		int GetMapHeight() const;
		int GetMapWidth() const;

		void UpdateGrid();

		Vgrid* GetGridMap(const std::string& name);

		std::array<Position, 4> Neighbours_Walk{ Position{ 8, 8 }, Position{ 24, 8 }, Position{ 8, 24 }, Position{ 24 ,24 } };

		template<typename T>
		void SetInfluence(int &targetpos_x, int& targetpos_y, int &curpos_x, int &curpos_y, int damage, T& op, const int minRange, const int maxRange, int n, ...)
		{
			/*
			* input : each cell, and current position and target position
			* iterate through each quarter
			*/
			va_list ap, hack;

			// start our reference point as usual
			va_start(ap, n);

			for (auto i = 0; i < 4; ++i)
			{
				const int newtargetpos_x = targetpos_x * 32 + Neighbours_Walk[i].x;
				const int newtargetpos_y = targetpos_y * 32 + Neighbours_Walk[i].y;

				// make influence fall off with distance:
				const float dist = Math::Distance(Vector2(curpos_x, curpos_y), Vector2(newtargetpos_x, newtargetpos_y));
				
				const int distinp = Position(curpos_x, curpos_y).getApproxDistance(Position(newtargetpos_x, newtargetpos_y));

				if (distinp > minRange
					&& distinp <= maxRange + 8)
				{
					if (distinp <= maxRange)
					{
						// make a copy of it
						va_copy(hack, ap);
											
						// get the addresses for the variables we want to hack
						for (auto iter = 0; iter < n; ++iter)
						{
							double* grid = va_arg(hack, double*);
							Grid::uQuarter cell{*grid};
							cell.g[i]     = op(cell.g[i], static_cast<short>(damage));
							double& cell1 = *grid;
							cell1         = cell.u_double;
						}
						va_end(hack);
					} 
					else
					{
						// make a copy of it
						va_copy(hack, ap);

						// get the addresses for the variables we want to hack
						for (int iter = 0; iter < n; ++iter)
						{
							double* grid = va_arg(hack, double*);
							Grid::uQuarter cell{*grid};
							cell.g[i]     = op(cell.g[i], static_cast<short>(static_cast<float>(damage) / dist));
							double& cell1 = *grid;
							cell1         = cell.u_double;
						}
						va_end(hack);
					}
				}
			}
			va_end(ap);
		}

		void UpdateBulletInfo();
		void UpdateInfluences(Bullet b);
		void UpdateInfluences(Unit u);

		void InitInfluenceMaps();
		void ClearInfluenceMaps();

		std::unordered_map<BWAPI::Player, std::unordered_map<int, UnitInfoset>>* GetUnitDataSets();

		void AddUnit(Unit u);
		void RemoveUnit(Unit u);

		void OnUnitMorph(Unit unit) override;
		void OnUnitCreate(Unit unit) override;
		void OnUnitShow(Unit unit) override;
		void OnUnitDestroy(Unit unit) override;

		/*
		My Influence
		All Influence coming from my units,buildings etc
		Opponent Influence
		All influence coming from opposing units,buildings etc
		Influence map
		Calculated as My Influence-Opponent Influence
		Tension map
		Calculated as My Influence+OpponentInfluence
		Vulnerability Map
		Calculated as Tension map -Abs(Influence map)
		*/
		std::unordered_map<std::string, std::array<double, 256*256>> grids;

		int mapWidth;
		int mapHeight;

		std::vector<TilePosition> startingLocations;

		bool firstattack_hydra = false;
		bool firstattack_mutal = false;
		std::vector<TilePosition> enemyBaseCandidate;
		std::vector<TilePosition> enemyBase;
		std::unordered_map<BWAPI::Player, std::unordered_map<int, UnitInfoset>> Units;

		bool runflag;

		std::vector<UnitInfoset> squads;

	private:
		int m_idStack;
		int GenerateID();
	};
}//namespace HOLD