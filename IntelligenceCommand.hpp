/*****************************************************************************/
/*!
\file   IntelligenceCommand.hpp
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

namespace HOLD
{
	typedef std::vector<Grid> Vgrid;


	//forward declaration
	class Grid;

	typedef int frame;
	struct UnitData
	{
		UnitData()
		{
			m_unit = nullptr;
			m_lastPosition = Positions::Invalid;
			m_lastSeen = 0;
			m_lastPlayer = nullptr;
			m_lastType = UnitTypes::Unknown;
			m_id = -1;
		}
		UnitData(Unit u)
		{
			m_unit = u;
			m_lastPosition = u->getPosition();
			m_lastSeen = Broodwar->getFrameCount();
			m_lastPlayer = u->getPlayer();
			m_lastType = u->getType();
			m_id = u->getID();
		}
		~UnitData()
		{
		}
		Unit m_unit;
		Position m_lastPosition;
		frame m_lastSeen;
		Player m_lastPlayer;
		UnitType m_lastType;
		int m_id;

		std::array<short, 240> m_prevHP;
		std::array<short, 240> m_prevShield;
	};


	struct UnitDataSet
	{
		void AddUnit(Unit u)
		{
			m_units.emplace(u);
			m_savedUnits.emplace(u->getID(), UnitData(u));

			//BWAPI::BroodwarImpl.server.
		}

		void AddUnit(UnitData u)
		{
			m_units.emplace(u.m_unit);
			m_savedUnits.emplace(u.m_unit->getID(), u);
		}

		void UpdateUnit(Unit u)
		{
			m_savedUnits[u->getID()].m_unit = u;
			m_savedUnits[u->getID()].m_lastPlayer = u->getPlayer();
			m_savedUnits[u->getID()].m_lastPosition = u->getPosition();
			m_savedUnits[u->getID()].m_lastSeen = Broodwar->getFrameCount();
			m_savedUnits[u->getID()].m_lastType = u->getType();
		}
		/*void UpdateUnit(UnitData & u)
		{
		m_savedUnits[u->getID()].m_unit = u;
		m_savedUnits[u->getID()].m_lastPlayer = u->getPlayer();
		m_savedUnits[u->getID()].m_lastPosition = u->getPosition();
		m_savedUnits[u->getID()].m_lastSeen = Broodwar->getFrameCount();
		m_savedUnits[u->getID()].m_lastType = u->getType();
		}*/

		void RemoveUnit(Unit u)
		{
			//m_units.erase_if(m_units.find(u));

			//m_units.erase(std::remove_if(m_units.begin(), m_units.end(), [=](Unit & unit){return unit->getID() == u->getID(); }), m_units.end());
			
			m_units.erase(u);
			m_savedUnits.erase(u->getID());
			/*for(auto it = m_units.begin(); it != m_units.end(); ++it)
			{
				if ((*it)->getID() == u->getID())
					m_units.erase(it++);
				else
					++it;
			}

			for (auto it = m_savedUnits.begin(); it != m_savedUnits.end(); ++it)
			{
				if ((*it).second.m_id == u->getID())
					m_savedUnits.erase(it++);
				else
					++it;
			}
			*/

			//m_savedUnits.erase(m_savedUnits.find(u->getID()));
			/*Unitset::iterator it = m_units.find(u);
			if(it._Ptr != nullptr)
				m_units.erase(it);

			auto it2 = m_savedUnits.find(u->getID());
			if(it2._Ptr != nullptr)
				m_savedUnits.erase(it2);*/
		}
		void RemoveUnit(UnitData u)
		{
			Unitset::iterator it = m_units.find(u.m_unit);
			if (it._Ptr != nullptr)
				m_units.erase(it);

			auto it2 = m_savedUnits.find(u.m_unit->getID());
			if (it2._Ptr != nullptr)
				m_savedUnits.erase(it2);
			/*m_units.erase(u.m_unit);
			m_savedUnits.erase(u.m_unit->getID());*/
		}

		std::unordered_map<int, UnitData> m_savedUnits;
		Unitset m_units;
		//todo: add cell info
	};



	class Message;
		
	class IntelligenceCommand : public Command
	{
	public:
		IntelligenceCommand();
		virtual ~IntelligenceCommand();

		void Init() override;
		void Update(int dt) override;
		void Shutdown() override;
		void ProcessMessage(Message* message) override;


		void BroadcastMessage(Message* message);


		const int GetMapHeight() const;
		const int GetMapWidth() const;

		void UpdateGrid();

		
		Vgrid* GetGridMap(const std::string& name);

		std::array<Position, 4> Neighbours_Walk{ Position{ 8, 8 }, Position{ 24, 8 }, Position{ 8, 24 }, Position{ 24 ,24 } };

		//todo: make sure the area is in the range
		template<typename T>
		void SetValues(Grid & cell, int &targetpos_x, int& targetpos_y, int &curpos_x, int &curpos_y, const int & damage, T& op, const int& range)
		{
			/*
			* input : each cell, and curret position and target position
			* loop for each quarter
			*/

			auto CalInfluence = [](short & cell, int targetpos_x, int targetpos_y, int& curpos_x, int& curpos_y, const int & damage, auto &op, auto & range)
			{
				// make influence fall of with distance:
				float dist = Math::Distance(Vector2(curpos_x, curpos_y), Vector2(targetpos_x, targetpos_y));
				//dist = fmod(x, 32.f);

				//if (dist > static_cast<float>(range * 1.3))
				//if(Position(curpos_x, curpos_y).getApproxDistance(Position(targetpos_x, targetpos_y)) > range * 1.1414f)
				//	return;

				int distinp = Position(curpos_x, curpos_y).getApproxDistance(Position(targetpos_x, targetpos_y));
				/*	if (distinp >= range + 8)
				return;*/


				if (distinp <= range + 8)
				{
					if (distinp <= range)
					{
						//Broodwar->drawTextMap(targetpos_x, targetpos_y, "%d", static_cast<int>( static_cast<float>(damage) / dist));

						cell = op(cell, static_cast<short>(damage));
						return;
					}
					//Broodwar->drawTextMap(targetpos_x, targetpos_y, "%2.0f", dist);

					dist /= 32.f;
					if (dist <= 1.f)
						dist = 1.f;
					cell = op(cell, static_cast<short>(static_cast<float>(damage) / dist));
					return;
				}
			};

			for (char i = 0; i < 4; ++i)
			{
				CalInfluence(cell[i], targetpos_x * 32 + Neighbours_Walk[i].x + curpos_x % 4, targetpos_y * 32 + Neighbours_Walk[i].y + curpos_y % 4, curpos_x, curpos_y, damage, op, range);
			}
		}
		void UpdateInfluences(const Unit u);


		void InitInfluenceMaps();
		void ClearInfluenceMaps();

		std::unordered_map< BWAPI::Player, std::unordered_map< int, HOLD::UnitDataSet > >* GetUnitDataSets();


		void AddUnit(Unit u);
		void RemoveUnit(Unit u);


		void OnUnitMorph(Unit unit) override;
		void OnUnitCreate(Unit unit) override;
		void OnUnitShow(Unit unit) override;
		void OnUnitDestroy(Unit unit) override;

	private:

		int m_idStack;
		int GenerateID();


		// create array
		/*
		* My Influence
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
	public:
		Vgrid opinfluenceGround;
		Vgrid opinfluenceAir;
		Vgrid influenceGround;
		Vgrid influenceAir;
		Vgrid tensionGround;
		Vgrid tensionAir;
		Vgrid tensionTotal;
		Vgrid vulGround;
		Vgrid vulAir;
		Vgrid vulTotal;
		//std::unordered_map<std::string, Vgrid> grids;
		int mapWidth;
		int mapHeight;

		bool foundEnemyBase = false;
		bool firstattack_hydra = false;
		bool firstattack_mutal = false;
		std::vector<TilePosition> enemyBaseCandidate;
		std::vector<TilePosition> enemyBase;
		std::unordered_map< BWAPI::Player, std::unordered_map< int, HOLD::UnitDataSet > > UnitDataSets;
	};
}//namespace HOLD

#//include "IntelligenceCommand.cpp"