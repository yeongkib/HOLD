/*****************************************************************************/
/*!
\file   UnitInfoset.hpp
\author Yeongki Baek
\par    email: yeongki.baek\@digipen.edu
\par    GAM450
\date   11/06/2017
\brief
This is the interface file for the Unitinfo
Copyright 2017, Digipen Institute of Technology
*/
/*****************************************************************************/
#pragma once
#include "UnitInfo.hpp"

#include <BWAPI/Position.h>
#include <BWAPI/Filters.h>
#include <BWAPI/SetContainer.h>
#include <iterator>
#include <memory>

using namespace BWAPI;

namespace HOLD
{
	// Forward declarations
	class BWAPI::UnitType;
	class BWAPI::Regionset;
	class BWAPI::UnitCommand;
	class BWAPI::TechType;

	/// <summary>The Unitset is a container for a set of pointers to Unit objects. It is typically
	/// used for groups of units instead of having to manage each Unit individually.</summary>
	///
	/// @see Unit
	class UnitInfoset : /*public Unitset/*/public SetContainer<UnitInfo*, std::hash<void*>>
	{
	public:
		//friend class Unitset;
		/// <summary>A blank Unitset containing no elements.</summary> This is typically used as a
		/// return value for BWAPI interface functions that have encountered an error.
		static const UnitInfoset none;

		UnitInfoset& operator = (const UnitInfoset& other) {
			clear();
			std::copy(other.begin(), other.end(), std::inserter(*this, begin()));
			return *this;
		}
		UnitInfoset() {};
		UnitInfoset(const Unitset & unitset);
		UnitInfo find(const int id);
		void AddUnit(Unit & unit);
		void RemoveUnit(const Unit & unit);
		void UpdateUnit(Unit & unit);

		/// <summary>Calculates the average of all valid Unit positions in this set.</summary>
		///
		/// @returns Average Position of all units in the set.
		///
		/// @see UnitInterface::getPosition
		Position getPosition() const;

		/// <summary>Creates a single set containing all units that are loaded into units of this set.</summary>
		///
		/// @returns The set of all loaded units.
		///
		/// @see UnitInterface::getLoadedUnits
		UnitInfoset getLoadedUnits() const;

		/// <summary>Creates a single set containing all the @Interceptors of all @Carriers in this set.</summary>
		///
		/// @returns The set of all @Interceptors .
		/// 
		/// @see UnitInterface::getInterceptors
		UnitInfoset getInterceptors() const;

		/// <summary>Creates a single set containing all the @Larvae of all @Hatcheries, @Lairs, and
		/// @Hives in this set.</summary>
		///
		/// @returns The set of all @Larvae .
		///
		/// @see UnitInterface::getLarva
		UnitInfoset getLarva() const;

		/// <summary>Sets the client info for every unit in this set.</summary>
		///
		/// <param name="clientInfo"> (optional)
		///   A pointer to client information, managed by the AI module, or nullptr if client
		///   information is to be cleared.
		/// </param>
		/// <param name="index"> (optional)
		///   An key value for the client info mapping so that more than one piece of data can be
		///   mapped to the same unit.
		/// </param>
		///
		/// @see UnitInterface::setClientInfo
		void setClientInfo(void *clientInfo = nullptr, int index = 0) const;
		/// @overload
		void setClientInfo(int clientInfo = 0, int index = 0) const;

		/// @copydoc UnitInterface::getUnitsInRadius
		UnitInfoset getUnitsInRadius(int radius, const UnitFilter &pred = nullptr) const;

		/// @copydoc UnitInterface::getClosestUnit
		UnitInfo getClosestUnit(const UnitFilter &pred = nullptr, int radius = 999999) const;

		/// @name Unit Commands
		/// @{

		/// @copydoc UnitInterface::issueCommand
		bool issueCommand(UnitCommand command) const;

		/// @copydoc UnitInterface::attack
		bool attack(Position target, bool shiftQueueCommand = false) const;

		/// @copydoc UnitInterface::attack
		bool attack(Unit target, bool shiftQueueCommand = false) const;

		/// @copydoc UnitInterface::build
		bool build(UnitType type, TilePosition target = TilePositions::None) const;

		/// @copydoc UnitInterface::buildAddon
		bool buildAddon(UnitType type) const;

		/// @copydoc UnitInterface::train
		bool train(UnitType type) const;

		/// @copydoc UnitInterface::morph
		bool morph(UnitType type) const;

		/// @copydoc UnitInterface::setRallyPoint
		bool setRallyPoint(Unit target) const;

		/// @copydoc UnitInterface::setRallyPoint
		bool setRallyPoint(Position target) const;

		/// @copydoc UnitInterface::move
		bool move(Position target, bool shiftQueueCommand = false) const;

		/// @copydoc UnitInterface::patrol
		bool patrol(Position target, bool shiftQueueCommand = false) const;

		/// @copydoc UnitInterface::holdPosition
		bool holdPosition(bool shiftQueueCommand = false) const;

		/// @copydoc UnitInterface::stop
		bool stop(bool shiftQueueCommand = false) const;

		/// @copydoc UnitInterface::follow
		bool follow(Unit target, bool shiftQueueCommand = false) const;

		/// @copydoc UnitInterface::gather
		bool gather(Unit target, bool shiftQueueCommand = false) const;

		/// @copydoc UnitInterface::returnCargo
		bool returnCargo(bool shiftQueueCommand = false) const;

		/// @copydoc UnitInterface::repair
		bool repair(Unit target, bool shiftQueueCommand = false) const;

		/// @copydoc UnitInterface::burrow
		bool burrow() const;

		/// @copydoc UnitInterface::unburrow
		bool unburrow() const;

		/// @copydoc UnitInterface::cloak
		bool cloak() const;

		/// @copydoc UnitInterface::decloak
		bool decloak() const;

		/// @copydoc UnitInterface::siege
		bool siege() const;

		/// @copydoc UnitInterface::unsiege
		bool unsiege() const;

		/// @copydoc UnitInterface::lift
		bool lift() const;

		/// @copydoc UnitInterface::load
		bool load(Unit target, bool shiftQueueCommand = false) const;

		/// @copydoc UnitInterface::unloadAll(bool)
		bool unloadAll(bool shiftQueueCommand = false) const;

		/// @copydoc UnitInterface::unloadAll(Position,bool)
		bool unloadAll(Position target, bool shiftQueueCommand = false) const;

		/// @copydoc UnitInterface::rightClick
		bool rightClick(Unit target, bool shiftQueueCommand = false) const;

		/// @copydoc UnitInterface::rightClick
		bool rightClick(Position target, bool shiftQueueCommand = false) const;

		/// @copydoc UnitInterface::haltConstruction
		bool haltConstruction() const;

		/// @copydoc UnitInterface::cancelConstruction
		bool cancelConstruction() const;

		/// @copydoc UnitInterface::cancelAddon
		bool cancelAddon() const;

		/// @copydoc UnitInterface::cancelTrain
		bool cancelTrain(int slot = -2) const;

		/// @copydoc UnitInterface::cancelMorph
		bool cancelMorph() const;

		/// @copydoc UnitInterface::cancelResearch
		bool cancelResearch() const;

		/// @copydoc UnitInterface::cancelUpgrade
		bool cancelUpgrade() const;

		/// @copydoc UnitInterface::useTech
		bool useTech(TechType tech, Unit target = nullptr) const;

		/// @copydoc UnitInterface::useTech
		bool useTech(TechType tech, Position target) const;

		///@}

	public:
		UnitInfo Captain;
	};
}
