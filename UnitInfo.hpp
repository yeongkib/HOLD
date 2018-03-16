/*****************************************************************************/
/*!
\file   UnitInfo.hpp
\author Yeongki Baek
\par    email: yeongki.baek\@digipen.edu
\par    GAM450
\date   11/05/2017
\brief
This is the interface file for the Unitinfo
Copyright 2017, Digipen Institute of Technology
*/
/*****************************************************************************/
#pragma once

#include "Precompiled.hpp"

#include <BWAPI.h>
#include <BWAPI/Game.h>
using namespace BWAPI;
using namespace Filter;

class UnitInfo
{
public:
	enum Enum
	{
		Nothing,

		AI_Controller_Type,
		AI_Controller_Timer,

		Worker_Task,

		Guard_Return_X,
		Guard_Return_Y,

		AI_Captain_Type,

		Town,

		Order,
		OrderState,
		OrderTarget_X,
		OrderTarget_Y,
		OrderTimer,
		QueuedOrder,
		AttackTarget,

		TargetFlags
	};

public:
	friend class Unitinteface;
	UnitInfo(){};// = delete;
	UnitInfo(BWAPI::Unit unit);

	UnitInfo &operator =(BWAPI::Unit unit);
	BWAPI::Unit operator->() const;
	Unit operator()() const;
	operator BWAPI::Unit() const;

	// GetInfo
	/*unsigned short GetAverageHP(unsigned int term = 128, int currentframe = Broodwar->getFrameCount()) const;
	unsigned short GetAverageShield(unsigned int term = 128, int currentframe = Broodwar->getFrameCount()) const;*/
	float GetAverageHP(unsigned int term = 128, int currentframe = Broodwar->getFrameCount()) const;
	float GetAverageShield(unsigned int term = 128, int currentframe = Broodwar->getFrameCount()) const;

	// assume it will be used after at least 128 frames
	int GetHPVariance(unsigned int term = 128, int currentframe = Broodwar->getFrameCount()) const;
	int GetShieldVariance(unsigned int term = 128, int currentframe = Broodwar->getFrameCount()) const;
	int GetOrderTimer() const;
	int GetOrderState() const;
	Orders::Enum::Enum GetUnitOrder() const;
	BWAPI::Position GetOrderTargetPosition() const;
	Orders::Enum::Enum GetQueuedOrder() const;
	Unit GetAttackTarget() const;
	BWAPI::Position GetGuardReturnPosition() const;
	int GetTargetFlags() const;
	int GetWorkerTask() const;

	// SetInfo
	void SetOrderState(int state);
	void SetOrderTimer(int time);
	void SetVirtualUnitOrder(BWAPI::Orders::Enum::Enum order, BWAPI::Position position = BWAPI::Positions::Origin);
	void SetQueuedOrder(BWAPI::Orders::Enum::Enum order);
	void SetAttackTarget(BWAPI::Unit pTarget);
	void SetControlType(int type);
	void SetCaptainType(int type);
	void SetGuardReturnPosition(BWAPI::Position pos);
	void SetTargetFlags(int flags);
	void SetWorkerTask(int type);

	// Procedures
	void UpdateOrderTimers();
	void EmulateOrder();
	BWAPI::Unit FindNewAttackTarget() const;
	bool IsMilitaryCaptain() const;
	bool HasNoCaptain() const;
	void AssignComputerIdleOrder();
	bool CanUseTech(TechType tech) const;
	bool UseTech(BWAPI::TechType tech, Unit pTarget, int targBit = 1);
	bool UseTechOnClosest(BWAPI::TechType tech, int range, const BWAPI::UnitFilter &pred, int targBit = 1);

	// AI Control
	void PerformSpecializedUnitControl();
	bool HasEnemyTarget();
	void DecloakCheck();
	bool DoUnitBehaviour();

	// Military
	bool TaskBunkerDefender();
	bool TaskSpellcaster(bool isAggressive);
	bool TaskAggression();
	void RunMilitaryController();

	// Task
	bool RunTaskController();

	// Worker
	bool AttendRepair();
	bool AttendTown();
	void RunWorkerController();

	// Orders
	void RunJunkYardDog();
	void RunComputerAI();
	void RunGuard();
	void RunGuardPost();
	void RunComputerReturn();




	// for Unit
public:
	int           getID() const;
	bool          exists() const;
	int           getReplayID() const;
	Player        getPlayer() const;
	UnitType      getType() const;
	Position      getPosition() const;
	double        getAngle() const;
	double        getVelocityX() const;
	double        getVelocityY() const;
	int           getHitPoints() const;
	int           getShields() const;
	int           getEnergy() const;
	int           getResources() const;
	int           getResourceGroup() const;

	int           getLastCommandFrame() const;
	UnitCommand   getLastCommand() const;
	BWAPI::Player getLastAttackingPlayer() const;

	UnitType      getInitialType() const;
	Position      getInitialPosition() const;
	TilePosition  getInitialTilePosition() const;
	int           getInitialHitPoints() const;
	int           getInitialResources() const;

	int getKillCount() const;
	int getAcidSporeCount() const;
	int getInterceptorCount() const;
	int getScarabCount() const;
	int getSpiderMineCount() const;
	int getGroundWeaponCooldown() const;
	int getAirWeaponCooldown() const;
	int getSpellCooldown() const;
	int getDefenseMatrixPoints() const;

	int getDefenseMatrixTimer() const;
	int getEnsnareTimer() const;
	int getIrradiateTimer() const;
	int getLockdownTimer() const;
	int getMaelstromTimer() const;
	int getOrderTimer() const;
	int getPlagueTimer() const;
	int getRemoveTimer() const;
	int getStasisTimer() const;
	int getStimTimer() const;

	UnitType      getBuildType() const;
	UnitType::list getTrainingQueue() const;
	TechType      getTech() const;
	UpgradeType   getUpgrade() const;
	int           getRemainingBuildTime() const;
	int           getRemainingTrainTime() const;
	int           getRemainingResearchTime() const;
	int           getRemainingUpgradeTime() const;
	Unit         getBuildUnit() const;

	Unit     getTarget() const;
	Position getTargetPosition() const;
	BWAPI::Order    getOrder() const;
	Unit     getOrderTarget() const;
	Position getOrderTargetPosition() const;
	BWAPI::Order    getSecondaryOrder() const;
	Position getRallyPosition() const;
	Unit     getRallyUnit() const;
	Unit     getAddon() const;
	Unit     getNydusExit() const;
	Unit     getPowerUp() const;

	Unit     getTransport() const;
	Unitset  getLoadedUnits() const;
	Unit     getCarrier() const;
	Unitset  getInterceptors() const;
	Unit     getHatchery() const;
	Unitset  getLarva() const;

	bool hasNuke() const;
	bool isAccelerating() const;
	bool isAttackFrame() const;
	bool isAttacking() const;
	bool isBeingGathered() const;
	bool isBeingHealed() const;
	bool isBlind() const;
	bool isBraking() const;
	bool isBurrowed() const;
	bool isCarryingGas() const;
	bool isCarryingMinerals() const;
	bool isCloaked() const;
	bool isCompleted() const;
	bool isConstructing() const;
	bool isDetected() const;
	bool isGatheringGas() const;
	bool isGatheringMinerals() const;
	bool isHallucination() const;
	bool isIdle() const;
	bool isInterruptible() const;
	bool isInvincible() const;
	bool isLifted() const;
	bool isMorphing() const;
	bool isMoving() const;
	bool isParasited() const;
	bool isSelected() const;
	bool isStartingAttack() const;
	bool isStuck() const;
	bool isTraining() const;
	bool isUnderAttack() const;
	bool isUnderDarkSwarm() const;
	bool isUnderDisruptionWeb() const;
	bool isUnderStorm() const;
	bool isPowered() const;
	bool isVisible(Player player = nullptr) const;
	bool isTargetable() const;

	bool canCommand() const;
	bool canCommandGrouped(bool checkCommandibility = true) const;
	bool canTargetUnit(Unit targetUnit, bool checkCommandibility = true) const;

	bool canAttack(bool checkCommandibility = true) const;
	bool canAttack(Position target, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canAttack(Unit target, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canAttackGrouped(bool checkCommandibilityGrouped = true, bool checkCommandibility = true) const;
	bool canAttackGrouped(Position target, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibilityGrouped = true, bool checkCommandibility = true) const;
	bool canAttackGrouped(Unit target, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibilityGrouped = true, bool checkCommandibility = true) const;
	bool canAttackMove(bool checkCommandibility = true) const;
	bool canAttackMoveGrouped(bool checkCommandibilityGrouped = true, bool checkCommandibility = true) const;
	bool canAttackUnit(bool checkCommandibility = true) const;
	bool canAttackUnit(Unit targetUnit, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canAttackUnitGrouped(bool checkCommandibilityGrouped = true, bool checkCommandibility = true) const;
	bool canAttackUnitGrouped(Unit targetUnit, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibilityGrouped = true, bool checkCommandibility = true) const;
	bool canBuild(bool checkCommandibility = true) const;
	bool canBuild(UnitType uType, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canBuild(UnitType uType, BWAPI::TilePosition tilePos, bool checkTargetUnitType = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canBuildAddon(bool checkCommandibility = true) const;
	bool canBuildAddon(UnitType uType, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canTrain(bool checkCommandibility = true) const;
	bool canTrain(UnitType uType, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canMorph(bool checkCommandibility = true) const;
	bool canMorph(UnitType uType, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canResearch(bool checkCommandibility = true) const;
	bool canResearch(TechType type, bool checkCanIssueCommandType = true) const;
	bool canUpgrade(bool checkCommandibility = true) const;
	bool canUpgrade(UpgradeType type, bool checkCanIssueCommandType = true) const;
	bool canSetRallyPoint(bool checkCommandibility = true) const;
	bool canSetRallyPoint(Position target, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canSetRallyPoint(Unit target, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canSetRallyPosition(bool checkCommandibility = true) const;
	bool canSetRallyUnit(bool checkCommandibility = true) const;
	bool canSetRallyUnit(Unit targetUnit, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canMove(bool checkCommandibility = true) const;
	bool canMoveGrouped(bool checkCommandibilityGrouped = true, bool checkCommandibility = true) const;
	bool canPatrol(bool checkCommandibility = true) const;
	bool canPatrolGrouped(bool checkCommandibilityGrouped = true, bool checkCommandibility = true) const;
	bool canFollow(bool checkCommandibility = true) const;
	bool canFollow(Unit targetUnit, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canGather(bool checkCommandibility = true) const;
	bool canGather(Unit targetUnit, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canReturnCargo(bool checkCommandibility = true) const;
	bool canHoldPosition(bool checkCommandibility = true) const;
	bool canStop(bool checkCommandibility = true) const;
	bool canRepair(bool checkCommandibility = true) const;
	bool canRepair(Unit targetUnit, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canBurrow(bool checkCommandibility = true) const;
	bool canUnburrow(bool checkCommandibility = true) const;
	bool canCloak(bool checkCommandibility = true) const;
	bool canDecloak(bool checkCommandibility = true) const;
	bool canSiege(bool checkCommandibility = true) const;
	bool canUnsiege(bool checkCommandibility = true) const;
	bool canLift(bool checkCommandibility = true) const;
	bool canLand(bool checkCommandibility = true) const;
	bool canLand(TilePosition target, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canLoad(bool checkCommandibility = true) const;
	bool canLoad(Unit targetUnit, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canUnloadWithOrWithoutTarget(bool checkCommandibility = true) const;
	bool canUnloadAtPosition(Position targDropPos, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canUnload(bool checkCommandibility = true) const;
	bool canUnload(Unit targetUnit, bool checkCanTargetUnit = true, bool checkPosition = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canUnloadAll(bool checkCommandibility = true) const;
	bool canUnloadAllPosition(bool checkCommandibility = true) const;
	bool canUnloadAllPosition(Position targDropPos, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canRightClick(bool checkCommandibility = true) const;
	bool canRightClick(Position target, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canRightClick(Unit target, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canRightClickGrouped(bool checkCommandibilityGrouped = true, bool checkCommandibility = true) const;
	bool canRightClickGrouped(Position target, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibilityGrouped = true, bool checkCommandibility = true) const;
	bool canRightClickGrouped(Unit target, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibilityGrouped = true, bool checkCommandibility = true) const;
	bool canRightClickPosition(bool checkCommandibility = true) const;
	bool canRightClickPositionGrouped(bool checkCommandibilityGrouped = true, bool checkCommandibility = true) const;
	bool canRightClickUnit(bool checkCommandibility = true) const;
	bool canRightClickUnit(Unit targetUnit, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canRightClickUnitGrouped(bool checkCommandibilityGrouped = true, bool checkCommandibility = true) const;
	bool canRightClickUnitGrouped(Unit targetUnit, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibilityGrouped = true, bool checkCommandibility = true) const;
	bool canHaltConstruction(bool checkCommandibility = true) const;
	bool canCancelConstruction(bool checkCommandibility = true) const;
	bool canCancelAddon(bool checkCommandibility = true) const;
	bool canCancelTrain(bool checkCommandibility = true) const;
	bool canCancelTrainSlot(bool checkCommandibility = true) const;
	bool canCancelTrainSlot(int slot, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canCancelMorph(bool checkCommandibility = true) const;
	bool canCancelResearch(bool checkCommandibility = true) const;
	bool canCancelUpgrade(bool checkCommandibility = true) const;
	bool canUseTechWithOrWithoutTarget(bool checkCommandibility = true) const;
	bool canUseTechWithOrWithoutTarget(BWAPI::TechType tech, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canUseTech(BWAPI::TechType tech, Unit target = nullptr, bool checkCanTargetUnit = true, bool checkTargetsType = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canUseTech(BWAPI::TechType tech, Position target, bool checkCanTargetUnit = true, bool checkTargetsType = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canUseTechWithoutTarget(BWAPI::TechType tech, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canUseTechUnit(BWAPI::TechType tech, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canUseTechUnit(BWAPI::TechType tech, Unit targetUnit, bool checkCanTargetUnit = true, bool checkTargetsUnits = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canUseTechPosition(BWAPI::TechType tech, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canUseTechPosition(BWAPI::TechType tech, Position target, bool checkTargetsPositions = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canPlaceCOP(bool checkCommandibility = true) const;
	bool canPlaceCOP(TilePosition target, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;

	bool canIssueCommandType(BWAPI::UnitCommandType ct, bool checkCommandibility = true) const;
	bool canIssueCommandTypeGrouped(BWAPI::UnitCommandType ct, bool checkCommandibilityGrouped = true, bool checkCommandibility = true) const;
	bool canIssueCommand(UnitCommand command, bool checkCanUseTechPositionOnPositions = true, bool checkCanUseTechUnitOnUnits = true, bool checkCanBuildUnitType = true, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibility = true) const;
	bool canIssueCommandGrouped(UnitCommand command, bool checkCanUseTechPositionOnPositions = true, bool checkCanUseTechUnitOnUnits = true, bool checkCanTargetUnit = true, bool checkCanIssueCommandType = true, bool checkCommandibilityGrouped = true, bool checkCommandibility = true) const;

	bool issueCommand(UnitCommand command);

	void die();
	void setID(int newID);
	bool canAccess() const;
	bool canAccessDetected() const;
	bool canAccessInside() const;

	/**
	* Gets index of the unit in the unit array. Note that the index is same
	* for the original unit array, BWAPI::Unit array and BWAI::Unit array, so
	* it is good way to compare units of different types and for conversion.
	* @return 0-based index of the unit in the unit array.
	*/
	//u16 getIndex() const;
	void setSelected(bool selectedState);
	void setLoaded(bool loadedState);
	//UnitImpl* getNext() const;

	void saveInitialState();
	void updateInternalData();
	void updateData();

	std::array<unsigned short, 256> m_prevHP;
	std::array<unsigned short, 256> m_prevShield;
	Unit m_unit;

//private:
	
	UnitType m_lastType;
	int m_id;

	Position m_lastPosition;
	frame m_lastSeen;

	

	mutable unsigned short m_avgHp;
	mutable unsigned short m_avgShield;

	Player m_lastPlayer;
};
