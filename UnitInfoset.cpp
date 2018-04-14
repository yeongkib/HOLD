/*****************************************************************************/
/*!
\file   UnitInfoset.cpp
\author Yeongki Baek
\par    email: yeongki.baek\@digipen.edu
\par    GAM450
\date   11/06/2017
\brief
This is the interface file for the Unitinfo
Copyright 2017, Digipen Institute of Technology
*/
/*****************************************************************************/

#include "Precompiled.hpp"
#include "UnitInfoset.hpp"
#include <BWAPI/Unitset.h>
#include <BWAPI/UnitCommand.h>
#include <BWAPI/Position.h>
#include <BWAPI/Unit.h>
#include <BWAPI/Game.h>
#include <BWAPI/TechType.h>

#include <utility>

namespace HOLD
{
	// initialize empty set
	const UnitInfoset UnitInfoset::none;

	UnitInfoset::UnitInfoset(const Unitset& unitset)
	{
		for(auto unit : unitset)
			this->emplace(new UnitInfo(unit));
	}

	UnitInfo UnitInfoset::find(const int& id)
	{
		for(auto &i : *this)
			if (i->getID() == id)
				return *i;

		return nullptr;
	}

	void UnitInfoset::AddUnit(Unit& unit)
	{
		this->emplace(new UnitInfo(unit));
	}

	void UnitInfoset::RemoveUnit(const Unit& unit)
	{
		auto it = this->begin();
		while (it != this->end()) 
		{
			if ((*it)->getID() == unit->getID()) it = this->erase(it);
			else ++it;
		}
	}

	void UnitInfoset::UpdateUnit(Unit& unit)
	{
		for (auto &i : *this) 
		{
			if (i->getID() == unit->getID()) 
			{
				*i = UnitInfo(unit);
				return;
			}
		}
		//if not found
		this->emplace(new UnitInfo(unit));
	}

	////////////////////////////////////////////////////////// Position
	Position UnitInfoset::getPosition() const 
	{
		// Declare the local position
		Position retPosition(0, 0);
		int validPosCount = 0;

		// Add up the positions for all units in the set
		for (auto &u : *this)
		{
			Position pos = u->getPosition();

			if (pos.isValid()) 
			{
				retPosition += pos;
				++validPosCount;
			}
		}

		if (validPosCount > 0)
			retPosition /= validPosCount;

		return retPosition;
	}
	////////////////////////////////////////////////////////// sets
	UnitInfoset UnitInfoset::getLoadedUnits() const
	{
		UnitInfoset retSet;

		for (auto &u : *this) 
		{
			UnitInfoset units{ u->getLoadedUnits() };
			retSet.insert(units.begin(), units.end());
		}

		return retSet;
	}

	UnitInfoset UnitInfoset::getInterceptors() const
	{
		UnitInfoset retSet;

		for (auto &u : *this) 
		{
			UnitInfoset units{ u->getInterceptors() };
			retSet.insert(units.begin(), units.end());
		}

		return retSet;
	}

	UnitInfoset UnitInfoset::getLarva() const 
	{
		UnitInfoset retSet;

		for (auto &u : *this)
		{
			UnitInfoset units{ u->getLarva() };
			retSet.insert(units.begin(), units.end());
		}

		return retSet;
	}
	////////////////////////////////////////////////////////// Misc
	void UnitInfoset::setClientInfo(void* clientInfo, int index) const
	{
		if (index < 0 || index > 255)
			return;

		// Assign the client info to all units in the set
		for (auto &u : *this)
		{
			u->m_unit->setClientInfo(clientInfo, index);
		}
	}

	void UnitInfoset::setClientInfo(int clientInfo, int index) const 
	{
		this->setClientInfo(reinterpret_cast<void*>(clientInfo), index);
	}

	UnitInfoset UnitInfoset::getUnitsInRadius(int radius, const UnitFilter& pred) const
	{
		// Return if this unit does not exist
		if (this->empty())
			return UnitInfoset::none;

		return UnitInfoset{ Broodwar->getUnitsInRadius(this->getPosition(), radius, pred) };
	}

	UnitInfo UnitInfoset::getClosestUnit(const UnitFilter& pred, int radius) const
	{
		// Return if this unit does not exist
		if (this->empty())
			return nullptr;

		return UnitInfo{ Broodwar->getClosestUnit(this->getPosition(), pred, radius) };
	}

	bool UnitInfoset::issueCommand(UnitCommand command) const 
	{
		bool ret = false;
		for(auto unit : *this) 
		{
			ret = unit->m_unit->issueCommand(command);
			if (!ret)
				return false;
		}
		return true;
		//return Broodwar->issueCommand(*this, command);
	}
	//--------------------------------------------- ATTACK MOVE ------------------------------------------------
	bool UnitInfoset::attack(Position target, bool shiftQueueCommand) const
	{
		return this->issueCommand(UnitCommand::attack(nullptr, target, shiftQueueCommand));
	}

	bool UnitInfoset::attack(Unit target, bool shiftQueueCommand) const
	{
		return this->issueCommand(UnitCommand::attack(nullptr, target, shiftQueueCommand));
	}
	//--------------------------------------------- BUILD ------------------------------------------------------
	bool UnitInfoset::build(const UnitType& type, const TilePosition& target) const
	{
		if (target == TilePositions::None)
			return this->train(type);

		return this->issueCommand(UnitCommand::build(nullptr, target, type));
	}
	//--------------------------------------------- BUILD ADDON ------------------------------------------------
	bool UnitInfoset::buildAddon(UnitType type) const
	{
		bool result = false;
		for (auto &it : *this) {
			result |= it->m_unit->buildAddon(type);
		}
		return result;
	}
	//--------------------------------------------- TRAIN ------------------------------------------------------
	bool UnitInfoset::train(UnitType type) const 
	{
		return this->issueCommand(UnitCommand::train(nullptr, type));
	}
	//--------------------------------------------- MORPH ------------------------------------------------------
	bool UnitInfoset::morph(UnitType type) const
	{
		return this->issueCommand(UnitCommand::morph(nullptr, type));
	}
	//--------------------------------------------- SET RALLY POSITION -----------------------------------------
	bool UnitInfoset::setRallyPoint(Position target) const 
	{
		return this->issueCommand(UnitCommand::setRallyPoint(nullptr, target));
	}

	bool UnitInfoset::setRallyPoint(Unit target) const 
	{
		return this->issueCommand(UnitCommand::setRallyPoint(nullptr, target));
	}
	//--------------------------------------------- MOVE -------------------------------------------------------
	bool UnitInfoset::move(Position target, bool shiftQueueCommand) const 
	{
		return this->issueCommand(UnitCommand::move(nullptr, target, shiftQueueCommand));
	}
	//--------------------------------------------- PATROL -----------------------------------------------------
	bool UnitInfoset::patrol(Position target, bool shiftQueueCommand) const 
	{
		return this->issueCommand(UnitCommand::patrol(nullptr, target, shiftQueueCommand));
	}
	//--------------------------------------------- HOLD POSITION ----------------------------------------------
	bool UnitInfoset::holdPosition(bool shiftQueueCommand) const
	{
		return this->issueCommand(UnitCommand::holdPosition(nullptr, shiftQueueCommand));
	}
	//--------------------------------------------- STOP -------------------------------------------------------
	bool UnitInfoset::stop(bool shiftQueueCommand) const
	{
		return this->issueCommand(UnitCommand::stop(nullptr, shiftQueueCommand));
	}
	//--------------------------------------------- FOLLOW -----------------------------------------------------
	bool UnitInfoset::follow(Unit target, bool shiftQueueCommand) const
	{
		return this->issueCommand(UnitCommand::follow(nullptr, target, shiftQueueCommand));
	}
	//--------------------------------------------- GATHER -----------------------------------------------------
	bool UnitInfoset::gather(Unit target, bool shiftQueueCommand) const
	{
		return this->issueCommand(UnitCommand::gather(nullptr, target, shiftQueueCommand));
	}
	//--------------------------------------------- RETURN CARGO -----------------------------------------------
	bool UnitInfoset::returnCargo(bool shiftQueueCommand) const 
	{
		return this->issueCommand(UnitCommand::returnCargo(nullptr, shiftQueueCommand));
	}
	//--------------------------------------------- REPAIR -----------------------------------------------------
	bool UnitInfoset::repair(Unit target, bool shiftQueueCommand) const 
	{
		return this->issueCommand(UnitCommand::repair(nullptr, target, shiftQueueCommand));
	}
	//--------------------------------------------- BURROW -----------------------------------------------------
	bool UnitInfoset::burrow() const 
	{
		return this->issueCommand(UnitCommand::burrow(nullptr));
	}
	//--------------------------------------------- UNBURROW ---------------------------------------------------
	bool UnitInfoset::unburrow() const 
	{
		return this->issueCommand(UnitCommand::unburrow(nullptr));
	}
	//--------------------------------------------- CLOAK ------------------------------------------------------
	bool UnitInfoset::cloak() const 
	{
		return this->issueCommand(UnitCommand::cloak(nullptr));
	}
	//--------------------------------------------- DECLOAK ----------------------------------------------------
	bool UnitInfoset::decloak() const 
	{
		return this->issueCommand(UnitCommand::decloak(nullptr));
	}
	//--------------------------------------------- SIEGE ------------------------------------------------------
	bool UnitInfoset::siege() const
	{
		return this->issueCommand(UnitCommand::siege(nullptr));
	}
	//--------------------------------------------- UNSIEGE ----------------------------------------------------
	bool UnitInfoset::unsiege() const
	{
		return this->issueCommand(UnitCommand::unsiege(nullptr));
	}
	//--------------------------------------------- LIFT -------------------------------------------------------
	bool UnitInfoset::lift() const 
	{
		return this->issueCommand(UnitCommand::lift(nullptr));
	}
	//--------------------------------------------- LOAD -------------------------------------------------------
	bool UnitInfoset::load(Unit target, bool shiftQueueCommand) const 
	{
		return this->issueCommand(UnitCommand::load(nullptr, target, shiftQueueCommand));
	}
	//--------------------------------------------- UNLOAD ALL -------------------------------------------------
	bool UnitInfoset::unloadAll(bool shiftQueueCommand) const 
	{
		return this->issueCommand(UnitCommand::unloadAll(nullptr, shiftQueueCommand));
	}
	//--------------------------------------------- UNLOAD ALL -------------------------------------------------
	bool UnitInfoset::unloadAll(Position target, bool shiftQueueCommand) const
	{
		return this->issueCommand(UnitCommand::unloadAll(nullptr, target, shiftQueueCommand));
	}
	//--------------------------------------------- RIGHT CLICK ------------------------------------------------
	bool UnitInfoset::rightClick(Unit target, bool shiftQueueCommand) const
	{
		return this->issueCommand(UnitCommand::rightClick(nullptr, target, shiftQueueCommand));
	}
	bool UnitInfoset::rightClick(Position target, bool shiftQueueCommand) const 
	{
		return this->issueCommand(UnitCommand::rightClick(nullptr, target, shiftQueueCommand));
	}
	//--------------------------------------------- HALT CONSTRUCTION ------------------------------------------
	bool UnitInfoset::haltConstruction() const
	{
		return this->issueCommand(UnitCommand::haltConstruction(nullptr));
	}
	//--------------------------------------------- CANCEL CONSTRUCTION ----------------------------------------
	bool UnitInfoset::cancelConstruction() const
	{
		return this->issueCommand(UnitCommand::cancelConstruction(nullptr));
	}
	//--------------------------------------------- CANCEL ADDON -----------------------------------------------
	bool UnitInfoset::cancelAddon() const
	{
		return this->issueCommand(UnitCommand::cancelAddon(nullptr));
	}
	//--------------------------------------------- CANCEL TRAIN -----------------------------------------------
	bool UnitInfoset::cancelTrain(int slot) const
	{
		return this->issueCommand(UnitCommand::cancelTrain(nullptr, slot));
	}
	//--------------------------------------------- CANCEL MORPH -----------------------------------------------
	bool UnitInfoset::cancelMorph() const
	{
		return this->issueCommand(UnitCommand::cancelMorph(nullptr));
	}
	//--------------------------------------------- CANCEL RESEARCH --------------------------------------------
	bool UnitInfoset::cancelResearch() const 
	{
		return this->issueCommand(UnitCommand::cancelResearch(nullptr));
	}
	//--------------------------------------------- CANCEL UPGRADE ---------------------------------------------
	bool UnitInfoset::cancelUpgrade() const 
	{
		return this->issueCommand(UnitCommand::cancelUpgrade(nullptr));
	}
	//--------------------------------------------- USE TECH ---------------------------------------------------
	bool UnitInfoset::useTech(TechType tech, Unit target) const
	{
		if (target == nullptr)
			return this->issueCommand(UnitCommand::useTech(nullptr, tech));
		return this->issueCommand(UnitCommand::useTech(nullptr, tech, target));
	}
	bool UnitInfoset::useTech(TechType tech, Position target) const
	{
		return this->issueCommand(UnitCommand::useTech(nullptr, tech, target));
	}
}

