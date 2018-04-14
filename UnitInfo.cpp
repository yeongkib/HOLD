#include "Precompiled.hpp"
#include "UnitInfo.hpp"

UnitInfo::UnitInfo(const BWAPI::Unit unit) :
m_unit(unit),
m_lastType(unit->getType()),
m_id(unit->getID()),
m_lastPosition(unit->getPosition()),
m_lastSeen(Broodwar->getFrameCount()),
m_lastPlayer(unit->getPlayer())
{
	//const int currentFrame = m_lastSeen % 240;
	std::fill(begin(m_prevHP), end(m_prevHP), unit->getHitPoints());
	std::fill(begin(m_prevShield), end(m_prevShield), unit->getHitPoints());

	/*m_prevHP[currentFrame] = unit->getHitPoints();
	m_prevShield[currentFrame] = unit->getShields();*/

	if(m_prevShield[0] == m_lastType.maxHitPoints())
	{
		if(m_prevHP[0] == m_lastType.maxShields())
		{
			m_avgShield = unit->getShields();
		}
		m_avgHp = unit->getHitPoints();
	}
}

UnitInfo &UnitInfo::operator =(BWAPI::Unit unit)
{
	m_unit = unit;
	return *this;
}
BWAPI::Unit UnitInfo::operator->() const
{
	return m_unit;
}
UnitInfo::operator BWAPI::Unit() const
{
	return m_unit;
}

Unit UnitInfo::operator()() const
{
	return m_unit;
}

//unsigned short UnitInfo::GetAverageHP(unsigned int term, int currentframe) const
float UnitInfo::GetAverageHP(unsigned int term, int currentframe) const
{
	float sum_hp{ 0.f };
	for (auto i = 0; i < term; ++i)
		sum_hp += m_prevHP[((currentframe + i) % 256 )];

		//averageHp += m_prevHP[((currentframe + i) % 256 + 256) % 256];;
	m_avgHp = sum_hp / static_cast<float>(term);
	return m_avgHp;
}
//unsigned short UnitInfo::GetAverageShield(unsigned int term, int currentframe) const
float UnitInfo::GetAverageShield(unsigned int term, int currentframe) const
{
	float sumShield{ 0.f };
	for (auto i = 0; i < term; ++i)
		sumShield += m_prevShield[((currentframe + i) % 256)];

		//averageShield += m_prevShield[((currentframe + i) % 256 + 256) % 256];
	m_avgShield = sumShield / static_cast<float>(term);
	return m_avgShield;
}

int UnitInfo::GetHPVariance(unsigned int term, int currentframe) const
{
	return static_cast<int>(m_prevHP[(currentframe - term) % 256] - m_prevHP[currentframe % 256]);
}

int UnitInfo::GetShieldVariance(unsigned int term, int currentframe) const
{
	return static_cast<int>(m_prevShield[(currentframe - term) % 256] - m_prevShield[currentframe % 256]);
}

bool UnitInfo::CanUseTech(TechType tech) const
{
	return m_unit->getPlayer()->hasResearched(TechTypes::Lockdown) && m_unit->getEnergy() >= tech.energyCost();
}
bool UnitInfo::UseTech(BWAPI::TechType tech, Unit pTarget, int targBit)
{
	if (!pTarget)
		return false;

	if (m_unit->getOrder() == tech.getOrder() && m_unit->getTarget() == pTarget) // Already targetting it
		return false;

	int targFlags = UnitInfo(pTarget).GetTargetFlags();
	if (targFlags & targBit)
		return false;

	if (m_unit->useTech(tech, pTarget)) // Use the tech!
	{
		UnitInfo(pTarget).SetTargetFlags(targFlags | targBit);
		SetVirtualUnitOrder(static_cast<Orders::Enum::Enum>(tech.getOrder().getID()));
		return true;
	}
	return false;   // If the tech failed
}
bool UnitInfo::UseTechOnClosest(BWAPI::TechType tech, int range, const BWAPI::UnitFilter &pred, int targBit)
{
	Unit pTarget = m_unit->getClosestUnit(pred, range);
	return UseTech(tech, pTarget, targBit);
}

// ----------------------------------------------- GETTERS -----------------------------------------
int UnitInfo::GetOrderTimer() const
{
	return m_unit->getClientInfo<int>(UnitInfo::OrderTimer);
}

int UnitInfo::GetOrderState() const
{
	return m_unit->getClientInfo<int>(UnitInfo::OrderState);
}

Orders::Enum::Enum UnitInfo::GetUnitOrder() const
{
	return m_unit->getClientInfo<Orders::Enum::Enum>(UnitInfo::Order);
}

Orders::Enum::Enum UnitInfo::GetQueuedOrder() const
{
	return m_unit->getClientInfo<Orders::Enum::Enum>(UnitInfo::QueuedOrder);
}

Unit UnitInfo::GetAttackTarget() const
{
	return m_unit->getClientInfo<Unit >(UnitInfo::AttackTarget);
}

int UnitInfo::GetWorkerTask() const
{
	return m_unit->getClientInfo<int>(UnitInfo::Worker_Task);
}

BWAPI::Position UnitInfo::GetGuardReturnPosition() const
{
	return BWAPI::Position(m_unit->getClientInfo<int>(UnitInfo::Guard_Return_X),
		m_unit->getClientInfo<int>(UnitInfo::Guard_Return_Y));
}

BWAPI::Position UnitInfo::GetOrderTargetPosition() const
{
	return BWAPI::Position(m_unit->getClientInfo<int>(UnitInfo::OrderTarget_X),
		m_unit->getClientInfo<int>(UnitInfo::OrderTarget_Y));
}
int UnitInfo::GetTargetFlags() const
{
	return m_unit->getClientInfo<int>(UnitInfo::TargetFlags);
}
// ----------------------------------------------- SETTERS -----------------------------------------
void UnitInfo::SetOrderState(int state)
{
	m_unit->setClientInfo(state, UnitInfo::OrderState);
}

void UnitInfo::SetOrderTimer(int time)
{
	m_unit->setClientInfo(time, UnitInfo::OrderTimer);
}

void UnitInfo::SetQueuedOrder(Orders::Enum::Enum order)
{
	m_unit->setClientInfo(order, UnitInfo::QueuedOrder);
}

void UnitInfo::SetVirtualUnitOrder(Orders::Enum::Enum order, BWAPI::Position pos)
{
	m_unit->setClientInfo(order, UnitInfo::Order);
	m_unit->setClientInfo(0, UnitInfo::OrderState);
	m_unit->setClientInfo(pos.x, UnitInfo::OrderTarget_X);
	m_unit->setClientInfo(pos.y, UnitInfo::OrderTarget_Y);
}

void UnitInfo::SetAttackTarget(BWAPI::Unit pTarget)
{
	m_unit->setClientInfo(pTarget, UnitInfo::AttackTarget);
}

void UnitInfo::SetControlType(int type)
{
	m_unit->setClientInfo(type, UnitInfo::AI_Controller_Type);
}
void UnitInfo::SetCaptainType(int type)
{
	m_unit->setClientInfo(type, UnitInfo::AI_Captain_Type);
}
void UnitInfo::SetWorkerTask(int type)
{
	m_unit->setClientInfo(type, UnitInfo::Worker_Task);
}

void UnitInfo::SetGuardReturnPosition(Position pos)
{
	m_unit->setClientInfo(pos.x, UnitInfo::Guard_Return_X);
	m_unit->setClientInfo(pos.y, UnitInfo::Guard_Return_Y);
}
void UnitInfo::SetTargetFlags(int flags)
{
	m_unit->setClientInfo(flags, UnitInfo::TargetFlags);
}






//--------------------------------------------- GET ID -----------------------------------------------------
int UnitInfo::getID() const
{;
	return m_unit->getID();
}
//--------------------------------------------- GET REPLAY ID ----------------------------------------------
int UnitInfo::getReplayID() const
{
	return m_unit->getReplayID();
}
//--------------------------------------------- GET PLAYER -------------------------------------------------
Player UnitInfo::getPlayer() const
{
	//todo: should return saved value
	return m_unit->getPlayer();
}
//--------------------------------------------- GET TYPE ---------------------------------------------------
UnitType UnitInfo::getType() const
{
	//todo: should return saved value
	//return m_unit->getType();
	return m_lastType;
}
//--------------------------------------------- GET POSITION -----------------------------------------------
Position UnitInfo::getPosition() const
{
	if (m_unit->exists())
		return m_unit->getPosition();
	else
		return m_lastPosition;
}
//--------------------------------------------- GET ANGLE --------------------------------------------------
double UnitInfo::getAngle() const
{
	return m_unit->getAngle();
}
//--------------------------------------------- GET VELOCITY X ---------------------------------------------
double UnitInfo::getVelocityX() const
{
	return m_unit->getVelocityX();
}
//--------------------------------------------- GET VELOCITY Y ---------------------------------------------
double UnitInfo::getVelocityY() const
{
	return m_unit->getVelocityY();
}
//--------------------------------------------- GET HIT POINTS ---------------------------------------------
int UnitInfo::getHitPoints() const
{
	return m_unit->getHitPoints();
}
//--------------------------------------------- GET SHIELDS ------------------------------------------------
int UnitInfo::getShields() const
{
	return m_unit->getShields();
}
//--------------------------------------------- GET ENERGY -------------------------------------------------
int UnitInfo::getEnergy() const
{
	return m_unit->getEnergy();
}
//--------------------------------------------- GET RESOURCES ----------------------------------------------
int UnitInfo::getResources() const
{
	return m_unit->getResources();
}
//--------------------------------------------- GET RESOURCE GROUP -----------------------------------------
int UnitInfo::getResourceGroup() const
{
	return m_unit->getResourceGroup();
}
//--------------------------------------------- GET LAST COMMAND FRAME -------------------------------------
int UnitInfo::getLastCommandFrame() const
{
	return m_unit->getLastCommandFrame();
}
//--------------------------------------------- GET LAST COMMAND -------------------------------------------
UnitCommand UnitInfo::getLastCommand() const
{
	return m_unit->getLastCommand();
}
//--------------------------------------------- GET LAST ATTACKING PLAYER ----------------------------------
BWAPI::Player UnitInfo::getLastAttackingPlayer() const
{
	return m_unit->getLastAttackingPlayer();
	//return Broodwar->getPlayer(self->lastAttackerPlayer);
}
//--------------------------------------------- GET INITIAL TYPE -------------------------------------------
UnitType UnitInfo::getInitialType() const
{
	return m_unit->getInitialType();
	//return initialType;
}
//--------------------------------------------- GET INITIAL POSITION ---------------------------------------
Position UnitInfo::getInitialPosition() const
{
	return m_unit->getInitialPosition();
	//return initialPosition;
}
//--------------------------------------------- GET INITIAL TILE POSITION ----------------------------------
TilePosition UnitInfo::getInitialTilePosition() const
{
	return m_unit->getInitialTilePosition();
	/*if (initialPosition == Positions::None)
		return TilePositions::None;
	return TilePosition(initialPosition - Position(initialType.tileSize()) / 2);*/
}
//--------------------------------------------- GET INITIAL HIT POINTS -------------------------------------
int UnitInfo::getInitialHitPoints() const
{
	return m_unit->getInitialHitPoints();
	//return initialHitPoints;
}
//--------------------------------------------- GET INITIAL RESOURCES --------------------------------------
int UnitInfo::getInitialResources() const
{
	return m_unit->getInitialResources();
	//return initialResources;
}
//--------------------------------------------- GET KILL COUNT ---------------------------------------------
int UnitInfo::getKillCount() const
{
	return m_unit->getKillCount();
	//return self->killCount;
}
//--------------------------------------------- GET ACID SPORE COUNT ---------------------------------------
int UnitInfo::getAcidSporeCount() const
{
	return m_unit->getAcidSporeCount();
	//return self->acidSporeCount;
}
//--------------------------------------------- GET INTERCEPTOR COUNT --------------------------------------
int UnitInfo::getInterceptorCount() const
{
	return m_unit->getInterceptorCount();
	//return self->interceptorCount;
}
//--------------------------------------------- GET SCARAB COUNT -------------------------------------------
int UnitInfo::getScarabCount() const
{
	return m_unit->getScarabCount();
	//return self->scarabCount;
}
//--------------------------------------------- GET SPIDER MINE COUNT --------------------------------------
int UnitInfo::getSpiderMineCount() const
{
	return m_unit->getSpiderMineCount();
	//return self->spiderMineCount;
}
//--------------------------------------------- GET GROUND WEAPON COOLDOWN ---------------------------------
int UnitInfo::getGroundWeaponCooldown() const
{
	return m_unit->getGroundWeaponCooldown();
	//return self->groundWeaponCooldown;
}
//--------------------------------------------- GET AIR WEAPON COOLDOWN ------------------------------------
int UnitInfo::getAirWeaponCooldown() const
{
	return m_unit->getAirWeaponCooldown();
	//return self->airWeaponCooldown;
}
//--------------------------------------------- GET SPELL COOLDOWN -----------------------------------------
int UnitInfo::getSpellCooldown() const
{
	return m_unit->getSpellCooldown();
	//return self->spellCooldown;
}
//--------------------------------------------- GET DEFENSE MATRIX POINTS ----------------------------------
int UnitInfo::getDefenseMatrixPoints() const
{
	return m_unit->getDefenseMatrixPoints();
	//return self->defenseMatrixPoints;
}
//--------------------------------------------- GET DEFENSE MATRIX TIMER -----------------------------------
int UnitInfo::getDefenseMatrixTimer() const
{
	return m_unit->getDefenseMatrixTimer();
	//return self->defenseMatrixTimer;
}
//--------------------------------------------- GET ENSNARE TIMER ------------------------------------------
int UnitInfo::getEnsnareTimer() const
{
	return m_unit->getEnsnareTimer();
	//return self->ensnareTimer;
}
//--------------------------------------------- GET IRRADIATE TIMER ----------------------------------------
int UnitInfo::getIrradiateTimer() const
{
	return m_unit->getIrradiateTimer();
	//return self->irradiateTimer;
}
//--------------------------------------------- GET LOCKDOWN TIMER -----------------------------------------
int UnitInfo::getLockdownTimer() const
{
	return m_unit->getLockdownTimer();
	//return self->lockdownTimer;
}
//--------------------------------------------- GET MAELSTROM TIMER ----------------------------------------
int UnitInfo::getMaelstromTimer() const
{
	return m_unit->getMaelstromTimer();
	//return self->maelstromTimer;
}
//--------------------------------------------- GET ORDER TIMER --------------------------------------------
int UnitInfo::getOrderTimer() const
{
	return m_unit->getOrderTimer();
	//return self->orderTimer;
}
//--------------------------------------------- GET PLAGUE TIMER -------------------------------------------
int UnitInfo::getPlagueTimer() const
{
	return m_unit->getPlagueTimer();
	//return self->plagueTimer;
}
//--------------------------------------------- GET REMOVE TIMER -------------------------------------------
int UnitInfo::getRemoveTimer() const
{
	return m_unit->getRemoveTimer();
	//return self->removeTimer;
}
//--------------------------------------------- GET STASIS TIMER -------------------------------------------
int UnitInfo::getStasisTimer() const
{
	return m_unit->getStasisTimer();
	//return self->stasisTimer;
}
//--------------------------------------------- GET STIM TIMER ---------------------------------------------
int UnitInfo::getStimTimer() const
{
	return m_unit->getStimTimer();
	//return self->stimTimer;
}
//--------------------------------------------- GET BUILD TYPE ---------------------------------------------
UnitType UnitInfo::getBuildType() const
{
	return m_unit->getBuildType();
	//return UnitType(self->buildType);
}
//--------------------------------------------- GET TRAINING QUEUE -----------------------------------------
UnitType::list UnitInfo::getTrainingQueue() const
{
	return m_unit->getTrainingQueue();
	//return UnitType::list(std::begin(self->trainingQueue), std::begin(self->trainingQueue) + self->trainingQueueCount);
}
//--------------------------------------------- GET TECH ---------------------------------------------------
TechType UnitInfo::getTech() const
{
	return m_unit->getTech();
	//return TechType(self->tech);
}
//--------------------------------------------- GET UPGRADE ------------------------------------------------
UpgradeType UnitInfo::getUpgrade() const
{
	return m_unit->getUpgrade();
	//return UpgradeType(self->upgrade);
}
//--------------------------------------------- GET REMAINING BUILD TIME -----------------------------------
int UnitInfo::getRemainingBuildTime() const
{
	return m_unit->getRemainingBuildTime();
	//return self->remainingBuildTime;
}
//--------------------------------------------- GET REMAINING TRAIN TIME -----------------------------------
int UnitInfo::getRemainingTrainTime() const
{
	return m_unit->getRemainingTrainTime();
	//return self->remainingTrainTime;
}
//--------------------------------------------- GET REMAINING RESEARCH TIME --------------------------------
int UnitInfo::getRemainingResearchTime() const
{
	return m_unit->getRemainingResearchTime();
	//return self->remainingResearchTime;
}
//--------------------------------------------- GET REMAINING UPGRADE TIME ---------------------------------
int UnitInfo::getRemainingUpgradeTime() const
{
	return m_unit->getRemainingUpgradeTime();
	//return self->remainingUpgradeTime;
}
//--------------------------------------------- GET BUILD UNIT ---------------------------------------------
Unit UnitInfo::getBuildUnit() const
{
	return m_unit->getBuildUnit();
	//return Broodwar->getUnit(self->buildUnit);
}
//--------------------------------------------- GET TARGET -------------------------------------------------
Unit UnitInfo::getTarget() const
{
	return m_unit->getTarget();
	//return Broodwar->getUnit(self->target);
}
//--------------------------------------------- GET TARGET POSITION ----------------------------------------
Position UnitInfo::getTargetPosition() const
{
	return m_unit->getTargetPosition();
	//return Position(self->targetPositionX, self->targetPositionY);
}
//--------------------------------------------- GET ORDER --------------------------------------------------
Order UnitInfo::getOrder() const
{
	return m_unit->getOrder();
	//return Order(self->order);
}
//--------------------------------------------- GET ORDER TARGET -------------------------------------------
Unit UnitInfo::getOrderTarget() const
{
	return m_unit->getOrderTarget();
	//return Broodwar->getUnit(self->orderTarget);
}
//--------------------------------------------- GET ORDER TARGET POSITION ----------------------------------
Position UnitInfo::getOrderTargetPosition() const
{
	return m_unit->getOrderTargetPosition();
	//return Position(self->orderTargetPositionX, self->orderTargetPositionY);
}
//--------------------------------------------- GET SECONDARY ORDER ID -------------------------------------
Order UnitInfo::getSecondaryOrder() const
{
	return m_unit->getSecondaryOrder();
	//return Order(self->secondaryOrder);
}
//--------------------------------------------- GET RALLY POSITION -----------------------------------------
Position UnitInfo::getRallyPosition() const
{
	return m_unit->getRallyPosition();
	//return Position(self->rallyPositionX, self->rallyPositionY);
}
//--------------------------------------------- GET RALLY UNIT ---------------------------------------------
Unit UnitInfo::getRallyUnit() const
{
	return m_unit->getRallyUnit();
	//return Broodwar->getUnit(self->rallyUnit);
}
//--------------------------------------------- GET ADDON --------------------------------------------------
Unit UnitInfo::getAddon() const
{
	return m_unit->getAddon();
	//return Broodwar->getUnit(self->addon);
}
//--------------------------------------------- GET NYDUS EXIT ---------------------------------------------
Unit UnitInfo::getNydusExit() const
{
	return m_unit->getNydusExit();
	//return Broodwar->getUnit(self->nydusExit);
}
//--------------------------------------------- GET POWER UP -----------------------------------------------
Unit UnitInfo::getPowerUp() const
{
	return m_unit->getPowerUp();
	//return Broodwar->getUnit(self->powerUp);
}
//--------------------------------------------- GET TRANSPORT ----------------------------------------------
Unit UnitInfo::getTransport() const
{
	return m_unit->getTransport();
	//return Broodwar->getUnit(self->transport);
}
//--------------------------------------------- GET LOADED UNITS -------------------------------------------
Unitset UnitInfo::getLoadedUnits() const
{
	return m_unit->getLoadedUnits();
	//return loadedUnits;
}
//--------------------------------------------- GET CARRIER ------------------------------------------------
Unit UnitInfo::getCarrier() const
{
	return m_unit->getCarrier();
	//return Broodwar->getUnit(self->carrier);
}
//--------------------------------------------- GET INTERCEPTORS -------------------------------------------
Unitset UnitInfo::getInterceptors() const
{
	return m_unit->getInterceptors();
	/*if (getType() != UnitTypes::Protoss_Carrier && getType() != UnitTypes::Hero_Gantrithor)
		return Unitset{};
	return connectedUnits;*/
}
//--------------------------------------------- GET HATCHERY -----------------------------------------------
Unit UnitInfo::getHatchery() const
{
	return m_unit->getHatchery();
	//return Broodwar->getUnit(self->hatchery);
}
//--------------------------------------------- GET LARVA --------------------------------------------------
Unitset UnitInfo::getLarva() const
{
	return m_unit->getLarva();
	/*if (!getType().producesLarva())
		return Unitset{};
	return connectedUnits;*/
}
//--------------------------------------------- EXISTS -----------------------------------------------------
bool UnitInfo::exists() const
{
	return m_unit->exists();
	//return self->exists;
}
//--------------------------------------------- HAS NUKE ---------------------------------------------------
bool UnitInfo::hasNuke() const
{
	return m_unit->hasNuke();
	//return self->hasNuke;
}
//--------------------------------------------- IS ACCELERATING --------------------------------------------
bool UnitInfo::isAccelerating() const
{
	return m_unit->isAccelerating();
	//return self->isAccelerating;
}
//--------------------------------------------- IS ATTACKING -----------------------------------------------
bool UnitInfo::isAttacking() const
{
	return m_unit->isAttacking();
	//return self->isAttacking;
}
//--------------------------------------------- IS ATTACK FRAME --------------------------------------------
bool UnitInfo::isAttackFrame() const
{
	return m_unit->isAttackFrame();
	//return self->isAttackFrame;
}
//--------------------------------------------- IS BEING GATHERED ------------------------------------------
bool UnitInfo::isBeingGathered() const
{
	return m_unit->isBeingGathered();
	//return self->isBeingGathered;
}
//--------------------------------------------- IS BEING HEALED --------------------------------------------
bool UnitInfo::isBeingHealed() const
{
	return m_unit->isBeingHealed();
	/*return getType().getRace() == Races::Terran &&
		self->isCompleted &&
		self->hitPoints > self->lastHitPoints;*/
}
//--------------------------------------------- IS BLIND ---------------------------------------------------
bool UnitInfo::isBlind() const
{
	return m_unit->isBlind();
	//return self->isBlind;
}
//--------------------------------------------- IS BRAKING -------------------------------------------------
bool UnitInfo::isBraking() const
{
	return m_unit->isBraking();
	//return self->isBraking;
}
//--------------------------------------------- IS BURROWED ------------------------------------------------
bool UnitInfo::isBurrowed() const
{
	return m_unit->isBurrowed();
	//return self->isBurrowed;
}
//--------------------------------------------- IS CARRYING GAS --------------------------------------------
bool UnitInfo::isCarryingGas() const
{
	return m_unit->isCarryingGas();
	//return self->carryResourceType == 1;
}
//--------------------------------------------- IS CARRYING MINERALS ---------------------------------------
bool UnitInfo::isCarryingMinerals() const
{
	return m_unit->isCarryingMinerals();
	//return self->carryResourceType == 2;
}
//--------------------------------------------- IS CLOAKED -------------------------------------------------
bool UnitInfo::isCloaked() const
{
	return m_unit->isCloaked();
	//return self->isCloaked;
}
//--------------------------------------------- IS COMPLETED -----------------------------------------------
bool UnitInfo::isCompleted() const
{
	return m_unit->isCompleted();
	//return self->isCompleted;
}
//--------------------------------------------- IS CONSTRUCTING --------------------------------------------
bool UnitInfo::isConstructing() const
{
	return m_unit->isConstructing();
	//return self->isConstructing;
}
//--------------------------------------------- IS DETECTED ------------------------------------------------
bool UnitInfo::isDetected() const
{
	return m_unit->isDetected();
	//return self->isDetected;
}
//--------------------------------------------- IS GATHERING GAS -------------------------------------------
bool UnitInfo::isGatheringGas() const
{
	return m_unit->isGatheringGas();
	//if (!self->isGathering)
	//	return false;

	//if (self->order != Orders::Harvest1   &&
	//	self->order != Orders::Harvest2   &&
	//	self->order != Orders::MoveToGas  &&
	//	self->order != Orders::WaitForGas &&
	//	self->order != Orders::HarvestGas &&
	//	self->order != Orders::ReturnGas  &&
	//	self->order != Orders::ResetCollision)
	//	return false;

	//if (self->order == Orders::ResetCollision)
	//	return self->carryResourceType == 1;

	////return true if BWOrder is WaitForGas, HarvestGas, or ReturnGas
	//if (self->order == Orders::WaitForGas ||
	//	self->order == Orders::HarvestGas ||
	//	self->order == Orders::ReturnGas)
	//	return true;

	////if BWOrder is MoveToGas, Harvest1, or Harvest2 we need to do some additional checks to make sure the unit is really gathering
	//Unit targ = getTarget();
	//if (targ &&
	//	targ->exists() &&
	//	targ->isCompleted() &&
	//	targ->getPlayer() == getPlayer() &&
	//	targ->getType() != UnitTypes::Resource_Vespene_Geyser &&
	//	(targ->getType().isRefinery() || targ->getType().isResourceDepot()))
	//	return true;
	//targ = getOrderTarget();
	//if (targ &&
	//	targ->exists() &&
	//	targ->isCompleted() &&
	//	targ->getPlayer() == getPlayer() &&
	//	targ->getType() != UnitTypes::Resource_Vespene_Geyser &&
	//	(targ->getType().isRefinery() || targ->getType().isResourceDepot()))
	//	return true;
	//return false;
}
//--------------------------------------------- IS GATHERING MINERALS --------------------------------------
bool UnitInfo::isGatheringMinerals() const
{
	return m_unit->isGatheringMinerals();
	//if (!self->isGathering)
	//	return false;

	//if (self->order != Orders::Harvest1        &&
	//	self->order != Orders::Harvest2        &&
	//	self->order != Orders::MoveToMinerals  &&
	//	self->order != Orders::WaitForMinerals &&
	//	self->order != Orders::MiningMinerals  &&
	//	self->order != Orders::ReturnMinerals  &&
	//	self->order != Orders::ResetCollision)
	//	return false;

	//if (self->order == Orders::ResetCollision)
	//	return self->carryResourceType == 2;

	////return true if BWOrder is WaitForMinerals, MiningMinerals, or ReturnMinerals
	//if (self->order == Orders::WaitForMinerals ||
	//	self->order == Orders::MiningMinerals ||
	//	self->order == Orders::ReturnMinerals)
	//	return true;

	////if BWOrder is MoveToMinerals, Harvest1, or Harvest2 we need to do some additional checks to make sure the unit is really gathering
	//if (getTarget() &&
	//	getTarget()->exists() &&
	//	(getTarget()->getType().isMineralField() ||
	//	(getTarget()->isCompleted() &&
	//		getTarget()->getPlayer() == getPlayer() &&
	//		getTarget()->getType().isResourceDepot())))
	//	return true;
	//if (getOrderTarget() &&
	//	getOrderTarget()->exists() &&
	//	(getOrderTarget()->getType().isMineralField() ||
	//	(getOrderTarget()->isCompleted() &&
	//		getOrderTarget()->getPlayer() == getPlayer() &&
	//		getOrderTarget()->getType().isResourceDepot())))
	//	return true;
	//return false;
}
//--------------------------------------------- IS HALLUCINATION -------------------------------------------
bool UnitInfo::isHallucination() const
{
	return m_unit->isHallucination();
	//return self->isHallucination;
}
//--------------------------------------------- IS IDLE ----------------------------------------------------
bool UnitInfo::isIdle() const
{
	return m_unit->isIdle();
	//return self->isIdle;
}
//--------------------------------------------- IS INTERRUPTIBLE -------------------------------------------
bool UnitInfo::isInterruptible() const
{
	return m_unit->isInterruptible();
	//return self->isInterruptible;
}
//--------------------------------------------- IS INVINCIBLE ----------------------------------------------
bool UnitInfo::isInvincible() const
{
	return m_unit->isInvincible();
	//return self->isInvincible;
}
//--------------------------------------------- IS LIFTED --------------------------------------------------
bool UnitInfo::isLifted() const
{
	return m_unit->isLifted();
	//return self->isLifted;
}
//--------------------------------------------- IS MORPHING ------------------------------------------------
bool UnitInfo::isMorphing() const
{
	return m_unit->isMorphing();
	//return self->isMorphing;
}
//--------------------------------------------- IS MOVING --------------------------------------------------
bool UnitInfo::isMoving() const
{
	return m_unit->isMoving();
	//return self->isMoving;
}
//--------------------------------------------- IS PARASITED -----------------------------------------------
bool UnitInfo::isParasited() const
{
	return m_unit->isParasited();
	//return self->isParasited;
}
//--------------------------------------------- IS SELECTED ------------------------------------------------
bool UnitInfo::isSelected() const
{
	return m_unit->isSelected();
	//return self->isSelected;
}
//--------------------------------------------- IS STARTING ATTACK -----------------------------------------
bool UnitInfo::isStartingAttack() const
{
	return m_unit->isStartingAttack();
	//return self->isStartingAttack;
}
//--------------------------------------------- IS STUCK ---------------------------------------------------
bool UnitInfo::isStuck() const
{
	return m_unit->isStuck();
	//return self->isStuck;
}
//--------------------------------------------- IS TRAINING ------------------------------------------------
bool UnitInfo::isTraining() const
{
	return m_unit->isTraining();
	//return self->isTraining;
}
//--------------------------------------------- IS UNDER ATTACK --------------------------------------------
bool UnitInfo::isUnderAttack() const
{
	return m_unit->isUnderAttack();
	//return self->recentlyAttacked;
}
//--------------------------------------------- IS UNDER DARK SWARM ----------------------------------------
bool UnitInfo::isUnderDarkSwarm() const
{
	return m_unit->isUnderDarkSwarm();
	//return self->isUnderDarkSwarm;
}
//--------------------------------------------- IS UNDER DISRUPTION WEB ------------------------------------
bool UnitInfo::isUnderDisruptionWeb() const
{
	return m_unit->isUnderDisruptionWeb();
	//return self->isUnderDWeb;
}
//--------------------------------------------- IS UNDER STORM ---------------------------------------------
bool UnitInfo::isUnderStorm() const
{
	return m_unit->isUnderStorm();
	//return self->isUnderStorm;
}
//--------------------------------------------- IS UNPOWERED -----------------------------------------------
bool UnitInfo::isPowered() const
{
	return m_unit->isPowered();
	//return self->isPowered;
}
//--------------------------------------------- IS VISIBLE -------------------------------------------------
bool UnitInfo::isVisible(Player player) const
{
	return m_unit->isVisible(player);
	//if (player)
	//	return self->isVisible[player->getID()];

	//if (Broodwar->self())     // isVisible for current player
	//	return self->isVisible[Broodwar->self()->getID()];

	//for (int i = 0; i < 9; ++i)
	//{
	//	if (self->isVisible[i])
	//		return true;
	//}
	//return false;
}
//--------------------------------------------- IS TARGETABLE ----------------------------------------------
bool UnitInfo::isTargetable() const
{
	return m_unit->isTargetable();
	//return Templates::canTargetUnit(const_cast<UnitInfo*>(this));
}
//--------------------------------------------- CAN COMMAND ------------------------------------------------
bool UnitInfo::canCommand() const
{
	return m_unit->canCommand();
	//return Templates::canCommand(const_cast<UnitInfo*>(this));
}
bool UnitInfo::canCommandGrouped(bool checkCommandibility) const
{
	return m_unit->canCommandGrouped();
	//return Templates::canCommandGrouped(const_cast<UnitInfo*>(this), checkCommandibility);
}
//--------------------------------------------- CAN TARGET UNIT --------------------------------------------
bool UnitInfo::canTargetUnit(Unit targetUnit, bool checkCommandibility) const
{
	return m_unit->canTargetUnit(targetUnit, checkCommandibility);
	//return Templates::canTargetUnit(const_cast<UnitInfo*>(this), targetUnit, checkCommandibility);
}
//--------------------------------------------- CAN ATTACK -------------------------------------------------
bool UnitInfo::canAttack(bool checkCommandibility) const
{
	return m_unit->canAttack(checkCommandibility);
	//return Templates::canAttack(const_cast<UnitInfo*>(this), checkCommandibility);
}
bool UnitInfo::canAttack(Position target, bool checkCanTargetUnit, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canAttack(target, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canAttack(const_cast<UnitInfo*>(this), target, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
}
bool UnitInfo::canAttack(Unit target, bool checkCanTargetUnit, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canAttack(target, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canAttack(const_cast<UnitInfo*>(this), target, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
}
bool UnitInfo::canAttackGrouped(bool checkCommandibilityGrouped, bool checkCommandibility) const
{
	return m_unit->canAttackGrouped(checkCommandibilityGrouped, checkCommandibility);
	//return Templates::canAttackGrouped(const_cast<UnitInfo*>(this), checkCommandibilityGrouped, checkCommandibility);
}
bool UnitInfo::canAttackGrouped(Position target, bool checkCanTargetUnit, bool checkCanIssueCommandType, bool checkCommandibilityGrouped, bool checkCommandibility) const
{
	return m_unit->canAttackGrouped(target, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibilityGrouped, checkCommandibility);
	//return Templates::canAttackGrouped(const_cast<UnitInfo*>(this), target, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibilityGrouped, checkCommandibility);
}
bool UnitInfo::canAttackGrouped(Unit target, bool checkCanTargetUnit, bool checkCanIssueCommandType, bool checkCommandibilityGrouped, bool checkCommandibility) const
{
	return m_unit->canAttackGrouped(target, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibilityGrouped, checkCommandibility);
	//return Templates::canAttackGrouped(const_cast<UnitInfo*>(this), target, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibilityGrouped, checkCommandibility);
}
//--------------------------------------------- CAN ATTACK MOVE --------------------------------------------
bool UnitInfo::canAttackMove(bool checkCommandibility) const
{
	return m_unit->canAttackMove(checkCommandibility);
	//return Templates::canAttackMove(const_cast<UnitInfo*>(this), checkCommandibility);
}
bool UnitInfo::canAttackMoveGrouped(bool checkCommandibilityGrouped, bool checkCommandibility) const
{
	return m_unit->canAttackMoveGrouped(checkCommandibilityGrouped, checkCommandibility);
	//return Templates::canAttackMoveGrouped(const_cast<UnitInfo*>(this), checkCommandibilityGrouped, checkCommandibility);
}
//--------------------------------------------- CAN ATTACK UNIT --------------------------------------------
bool UnitInfo::canAttackUnit(bool checkCommandibility) const
{
	return m_unit->canAttackUnit(checkCommandibility);
	//return Templates::canAttackUnit(const_cast<UnitInfo*>(this), checkCommandibility);
}
bool UnitInfo::canAttackUnit(Unit targetUnit, bool checkCanTargetUnit, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canAttackUnit(targetUnit, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canAttackUnit(const_cast<UnitInfo*>(this), targetUnit, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
}
bool UnitInfo::canAttackUnitGrouped(bool checkCommandibilityGrouped, bool checkCommandibility) const
{
	return m_unit->canAttackUnitGrouped(checkCommandibilityGrouped, checkCommandibility);
	//return Templates::canAttackUnitGrouped(const_cast<UnitInfo*>(this), checkCommandibilityGrouped, checkCommandibility);
}
bool UnitInfo::canAttackUnitGrouped(Unit targetUnit, bool checkCanTargetUnit, bool checkCanIssueCommandType, bool checkCommandibilityGrouped, bool checkCommandibility) const
{
	return m_unit->canAttackUnitGrouped(targetUnit, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibilityGrouped, checkCommandibility);
	//return Templates::canAttackUnitGrouped(const_cast<UnitInfo*>(this), targetUnit, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibilityGrouped, checkCommandibility);
}
//--------------------------------------------- CAN BUILD --------------------------------------------------
bool UnitInfo::canBuild(bool checkCommandibility) const
{
	return m_unit->canBuild(checkCommandibility);
	//return Templates::canBuild(const_cast<UnitInfo*>(this), checkCommandibility);
}
bool UnitInfo::canBuild(UnitType uType, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canBuild(uType, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canBuild(const_cast<UnitInfo*>(this), uType, checkCanIssueCommandType, checkCommandibility);
}
bool UnitInfo::canBuild(UnitType uType, BWAPI::TilePosition tilePos, bool checkTargetUnitType, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canBuild(uType, tilePos, checkTargetUnitType, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canBuild(const_cast<UnitInfo*>(this), uType, tilePos, checkTargetUnitType, checkCanIssueCommandType, checkCommandibility);
}
//--------------------------------------------- CAN BUILD ADDON --------------------------------------------
bool UnitInfo::canBuildAddon(bool checkCommandibility) const
{
	return m_unit->canBuildAddon(checkCommandibility);
	//return Templates::canBuildAddon(const_cast<UnitInfo*>(this), checkCommandibility);
}
bool UnitInfo::canBuildAddon(UnitType uType, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canBuildAddon(uType, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canBuildAddon(const_cast<UnitInfo*>(this), uType, checkCanIssueCommandType, checkCommandibility);
}
//--------------------------------------------- CAN TRAIN --------------------------------------------------
bool UnitInfo::canTrain(bool checkCommandibility) const
{
	return m_unit->canTrain(checkCommandibility);
	//return Templates::canTrain(const_cast<UnitInfo*>(this), checkCommandibility);
}
bool UnitInfo::canTrain(UnitType uType, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canTrain(uType, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canTrain(const_cast<UnitInfo*>(this), uType, checkCanIssueCommandType, checkCommandibility);
}
//--------------------------------------------- CAN MORPH --------------------------------------------------
bool UnitInfo::canMorph(bool checkCommandibility) const
{
	return m_unit->canMorph(checkCommandibility);
	//return Templates::canMorph(const_cast<UnitInfo*>(this), checkCommandibility);
}
bool UnitInfo::canMorph(UnitType uType, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canMorph(uType, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canMorph(const_cast<UnitInfo*>(this), uType, checkCanIssueCommandType, checkCommandibility);
}
//--------------------------------------------- CAN RESEARCH -----------------------------------------------
bool UnitInfo::canResearch(bool checkCommandibility) const
{
	return m_unit->canResearch(checkCommandibility);
	//return Templates::canResearch(const_cast<UnitInfo*>(this), checkCommandibility);
}
bool UnitInfo::canResearch(TechType type, bool checkCanIssueCommandType) const
{
	return m_unit->canResearch(type, checkCanIssueCommandType);
	//return Templates::canResearch(const_cast<UnitInfo*>(this), type, checkCanIssueCommandType);
}
//--------------------------------------------- CAN UPGRADE ------------------------------------------------
bool UnitInfo::canUpgrade(bool checkCommandibility) const
{
	return m_unit->canUpgrade(checkCommandibility);
	//return Templates::canUpgrade(const_cast<UnitInfo*>(this), checkCommandibility);
}
bool UnitInfo::canUpgrade(UpgradeType type, bool checkCanIssueCommandType) const
{
	return m_unit->canUpgrade(type, checkCanIssueCommandType);
	//return Templates::canUpgrade(const_cast<UnitInfo*>(this), type, checkCanIssueCommandType);
}
//--------------------------------------------- CAN SET RALLY POINT ----------------------------------------
bool UnitInfo::canSetRallyPoint(bool checkCommandibility) const
{
	return m_unit->canSetRallyPoint(checkCommandibility);
	//return Templates::canSetRallyPoint(const_cast<UnitInfo*>(this), checkCommandibility);
}
bool UnitInfo::canSetRallyPoint(Position target, bool checkCanTargetUnit, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canSetRallyPoint(target, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canSetRallyPoint(const_cast<UnitInfo*>(this), target, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
}
bool UnitInfo::canSetRallyPoint(Unit target, bool checkCanTargetUnit, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canSetRallyPoint(target, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canSetRallyPoint(const_cast<UnitInfo*>(this), target, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
}
//--------------------------------------------- CAN SET RALLY POSITION -------------------------------------
bool UnitInfo::canSetRallyPosition(bool checkCommandibility) const
{
	return m_unit->canSetRallyPosition(checkCommandibility);
	//return Templates::canSetRallyPosition(const_cast<UnitInfo*>(this), checkCommandibility);
}
//--------------------------------------------- CAN SET RALLY UNIT -----------------------------------------
bool UnitInfo::canSetRallyUnit(bool checkCommandibility) const
{
	return m_unit->canSetRallyUnit(checkCommandibility);
	//return Templates::canSetRallyUnit(const_cast<UnitInfo*>(this), checkCommandibility);
}
bool UnitInfo::canSetRallyUnit(Unit targetUnit, bool checkCanTargetUnit, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canSetRallyUnit(targetUnit, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canSetRallyUnit(const_cast<UnitInfo*>(this), targetUnit, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
}
//--------------------------------------------- CAN MOVE ---------------------------------------------------
bool UnitInfo::canMove(bool checkCommandibility) const
{
	return m_unit->canMove(checkCommandibility);
	//return Templates::canMove(const_cast<UnitInfo*>(this), checkCommandibility);
}
bool UnitInfo::canMoveGrouped(bool checkCommandibilityGrouped, bool checkCommandibility) const
{
	return m_unit->canMoveGrouped(checkCommandibilityGrouped, checkCommandibility);
	//return Templates::canMoveGrouped(const_cast<UnitInfo*>(this), checkCommandibilityGrouped, checkCommandibility);
}
//--------------------------------------------- CAN PATROL -------------------------------------------------
bool UnitInfo::canPatrol(bool checkCommandibility) const
{
	return m_unit->canPatrol(checkCommandibility);
	//return Templates::canPatrol(const_cast<UnitInfo*>(this), checkCommandibility);
}
bool UnitInfo::canPatrolGrouped(bool checkCommandibilityGrouped, bool checkCommandibility) const
{
	return m_unit->canPatrolGrouped(checkCommandibilityGrouped, checkCommandibility);
	//return Templates::canPatrolGrouped(const_cast<UnitInfo*>(this), checkCommandibilityGrouped, checkCommandibility);
}
//--------------------------------------------- CAN FOLLOW -------------------------------------------------
bool UnitInfo::canFollow(bool checkCommandibility) const
{
	return m_unit->canFollow(checkCommandibility);
	//return Templates::canFollow(const_cast<UnitInfo*>(this), checkCommandibility);
}
bool UnitInfo::canFollow(Unit targetUnit, bool checkCanTargetUnit, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canFollow(targetUnit, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canFollow(const_cast<UnitInfo*>(this), targetUnit, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
}
//--------------------------------------------- CAN GATHER -------------------------------------------------
bool UnitInfo::canGather(bool checkCommandibility) const
{
	return m_unit->canGather(checkCommandibility);
	//return Templates::canGather(const_cast<UnitInfo*>(this), checkCommandibility);
}
bool UnitInfo::canGather(Unit targetUnit, bool checkCanTargetUnit, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canGather(targetUnit, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canGather(const_cast<UnitInfo*>(this), targetUnit, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
}
//--------------------------------------------- CAN RETURN CARGO -------------------------------------------
bool UnitInfo::canReturnCargo(bool checkCommandibility) const
{
	return m_unit->canReturnCargo(checkCommandibility);
	//return Templates::canReturnCargo(const_cast<UnitInfo*>(this), checkCommandibility);
}
//--------------------------------------------- CAN HOLD POSITION ------------------------------------------
bool UnitInfo::canHoldPosition(bool checkCommandibility) const
{
	return m_unit->canHoldPosition(checkCommandibility);
	//return Templates::canHoldPosition(const_cast<UnitInfo*>(this), checkCommandibility);
}
//--------------------------------------------- CAN STOP ---------------------------------------------------
bool UnitInfo::canStop(bool checkCommandibility) const
{
	return m_unit->canStop(checkCommandibility);
	//return Templates::canStop(const_cast<UnitInfo*>(this), checkCommandibility);
}
//--------------------------------------------- CAN REPAIR -------------------------------------------------
bool UnitInfo::canRepair(bool checkCommandibility) const
{
	return m_unit->canRepair(checkCommandibility);
	//return Templates::canRepair(const_cast<UnitInfo*>(this), checkCommandibility);
}
bool UnitInfo::canRepair(Unit targetUnit, bool checkCanTargetUnit, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canRepair(targetUnit, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canRepair(const_cast<UnitInfo*>(this), targetUnit, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
}
//--------------------------------------------- CAN BURROW -------------------------------------------------
bool UnitInfo::canBurrow(bool checkCommandibility) const
{
	return m_unit->canBurrow(checkCommandibility);
	//return Templates::canBurrow(const_cast<UnitInfo*>(this), checkCommandibility);
}
//--------------------------------------------- CAN UNBURROW -----------------------------------------------
bool UnitInfo::canUnburrow(bool checkCommandibility) const
{
	return m_unit->canBurrow(checkCommandibility);
	//return Templates::canUnburrow(const_cast<UnitInfo*>(this), checkCommandibility);
}
//--------------------------------------------- CAN CLOAK --------------------------------------------------
bool UnitInfo::canCloak(bool checkCommandibility) const
{
	return m_unit->canCloak(checkCommandibility);
	//return Templates::canCloak(const_cast<UnitInfo*>(this), checkCommandibility);
}
//--------------------------------------------- CAN DECLOAK ------------------------------------------------
bool UnitInfo::canDecloak(bool checkCommandibility) const
{
	return m_unit->canDecloak(checkCommandibility);
	//return Templates::canDecloak(const_cast<UnitInfo*>(this), checkCommandibility);
}
//--------------------------------------------- CAN SIEGE --------------------------------------------------
bool UnitInfo::canSiege(bool checkCommandibility) const
{
	return m_unit->canSiege(checkCommandibility);
	//return Templates::canSiege(const_cast<UnitInfo*>(this), checkCommandibility);
}
//--------------------------------------------- CAN UNSIEGE ------------------------------------------------
bool UnitInfo::canUnsiege(bool checkCommandibility) const
{
	return m_unit->canUnsiege(checkCommandibility);
	//return Templates::canUnsiege(const_cast<UnitInfo*>(this), checkCommandibility);
}
//--------------------------------------------- CAN LIFT ---------------------------------------------------
bool UnitInfo::canLift(bool checkCommandibility) const
{
	return m_unit->canLift(checkCommandibility);
	//return Templates::canLift(const_cast<UnitInfo*>(this), checkCommandibility);
}
//--------------------------------------------- CAN LAND ---------------------------------------------------
bool UnitInfo::canLand(bool checkCommandibility) const
{
	return m_unit->canLoad(checkCommandibility);
	//return Templates::canLand(const_cast<UnitInfo*>(this), checkCommandibility);
}
bool UnitInfo::canLand(TilePosition target, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canLand(target, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canLand(const_cast<UnitInfo*>(this), target, checkCanIssueCommandType, checkCommandibility);
}
//--------------------------------------------- CAN LOAD ---------------------------------------------------
bool UnitInfo::canLoad(bool checkCommandibility) const
{
	return m_unit->canLoad(checkCommandibility);
	//return Templates::canLoad(const_cast<UnitInfo*>(this), checkCommandibility);
}
bool UnitInfo::canLoad(Unit targetUnit, bool checkCanTargetUnit, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canLoad(targetUnit, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canLoad(const_cast<UnitInfo*>(this), targetUnit, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
}
//--------------------------------------------- CAN UNLOAD -------------------------------------------------
bool UnitInfo::canUnloadWithOrWithoutTarget(bool checkCommandibility) const
{
	return m_unit->canUnloadWithOrWithoutTarget(checkCommandibility);
	//return Templates::canUnloadWithOrWithoutTarget(const_cast<UnitInfo*>(this), checkCommandibility);
}
bool UnitInfo::canUnloadAtPosition(Position targDropPos, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canUnloadAtPosition(targDropPos, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canUnloadAtPosition(const_cast<UnitInfo*>(this), targDropPos, checkCanIssueCommandType, checkCommandibility);
}
bool UnitInfo::canUnload(bool checkCommandibility) const
{
	return m_unit->canUnload(checkCommandibility);
	//return Templates::canUnload(const_cast<UnitInfo*>(this), checkCommandibility);
}
bool UnitInfo::canUnload(Unit targetUnit, bool checkCanTargetUnit, bool checkPosition, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canUnload(targetUnit, checkCanTargetUnit, checkPosition, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canUnload(const_cast<UnitInfo*>(this), targetUnit, checkCanTargetUnit, checkPosition, checkCanIssueCommandType, checkCommandibility);
}
//--------------------------------------------- CAN UNLOAD ALL ---------------------------------------------
bool UnitInfo::canUnloadAll(bool checkCommandibility) const
{
	return m_unit->canUnloadAll(checkCommandibility);
	//return Templates::canUnloadAll(const_cast<UnitInfo*>(this), checkCommandibility);
}
//--------------------------------------------- CAN UNLOAD ALL POSITION ------------------------------------
bool UnitInfo::canUnloadAllPosition(bool checkCommandibility) const
{
	return m_unit->canUnloadAllPosition(checkCommandibility);
	//return Templates::canUnloadAllPosition(const_cast<UnitInfo*>(this), checkCommandibility);
}
bool UnitInfo::canUnloadAllPosition(Position targDropPos, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canUnloadAllPosition(targDropPos, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canUnloadAllPosition(const_cast<UnitInfo*>(this), targDropPos, checkCanIssueCommandType, checkCommandibility);
}
//--------------------------------------------- CAN RIGHT CLICK --------------------------------------------
bool UnitInfo::canRightClick(bool checkCommandibility) const
{
	return m_unit->canRightClick(checkCommandibility);
	//return Templates::canRightClick(const_cast<UnitInfo*>(this), checkCommandibility);
}
bool UnitInfo::canRightClick(Position target, bool checkCanTargetUnit, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canRightClick(target, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canRightClick(const_cast<UnitInfo*>(this), target, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
}
bool UnitInfo::canRightClick(Unit target, bool checkCanTargetUnit, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canRightClick(target, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canRightClick(const_cast<UnitInfo*>(this), target, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
}
bool UnitInfo::canRightClickGrouped(bool checkCommandibilityGrouped, bool checkCommandibility) const
{
	return m_unit->canRightClickGrouped(checkCommandibilityGrouped, checkCommandibility);
	//return Templates::canRightClickGrouped(const_cast<UnitInfo*>(this), checkCommandibilityGrouped, checkCommandibility);
}
bool UnitInfo::canRightClickGrouped(Position target, bool checkCanTargetUnit, bool checkCanIssueCommandType, bool checkCommandibilityGrouped, bool checkCommandibility) const
{
	return m_unit->canRightClickGrouped(target, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibilityGrouped, checkCommandibility);
	//return Templates::canRightClickGrouped(const_cast<UnitInfo*>(this), target, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibilityGrouped, checkCommandibility);
}
bool UnitInfo::canRightClickGrouped(Unit target, bool checkCanTargetUnit, bool checkCanIssueCommandType, bool checkCommandibilityGrouped, bool checkCommandibility) const
{
	return m_unit->canRightClickGrouped(target, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibilityGrouped, checkCommandibility);
	//return Templates::canRightClickGrouped(const_cast<UnitInfo*>(this), target, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibilityGrouped, checkCommandibility);
}
//--------------------------------------------- CAN RIGHT CLICK POSITION -----------------------------------
bool UnitInfo::canRightClickPosition(bool checkCommandibility) const
{
	return m_unit->canRightClickPosition(checkCommandibility);
	//return Templates::canRightClickPosition(const_cast<UnitInfo*>(this), checkCommandibility);
}
bool UnitInfo::canRightClickPositionGrouped(bool checkCommandibilityGrouped, bool checkCommandibility) const
{
	return m_unit->canRightClickPositionGrouped(checkCommandibilityGrouped, checkCommandibility);
	//return Templates::canRightClickPositionGrouped(const_cast<UnitInfo*>(this), checkCommandibilityGrouped, checkCommandibility);
}
//--------------------------------------------- CAN RIGHT CLICK UNIT ---------------------------------------
bool UnitInfo::canRightClickUnit(bool checkCommandibility) const
{
	return m_unit->canRightClickUnit(checkCommandibility);
	//return Templates::canRightClickUnit(const_cast<UnitInfo*>(this), checkCommandibility);
}
bool UnitInfo::canRightClickUnit(Unit targetUnit, bool checkCanTargetUnit, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canRightClickUnit(targetUnit, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canRightClickUnit(const_cast<UnitInfo*>(this), targetUnit, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
}
bool UnitInfo::canRightClickUnitGrouped(bool checkCommandibilityGrouped, bool checkCommandibility) const
{
	return m_unit->canRightClickUnitGrouped(checkCommandibilityGrouped, checkCommandibility);
	//return Templates::canRightClickUnitGrouped(const_cast<UnitInfo*>(this), checkCommandibilityGrouped, checkCommandibility);
}
bool UnitInfo::canRightClickUnitGrouped(Unit targetUnit, bool checkCanTargetUnit, bool checkCanIssueCommandType, bool checkCommandibilityGrouped, bool checkCommandibility) const
{
	return m_unit->canRightClickUnitGrouped(targetUnit, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibilityGrouped, checkCommandibility);
	//return Templates::canRightClickUnitGrouped(const_cast<UnitInfo*>(this), targetUnit, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibilityGrouped, checkCommandibility);
}
//--------------------------------------------- CAN HALT CONSTRUCTION --------------------------------------
bool UnitInfo::canHaltConstruction(bool checkCommandibility) const
{
	return m_unit->canHaltConstruction(checkCommandibility);
	//return Templates::canHaltConstruction(const_cast<UnitInfo*>(this), checkCommandibility);
}
//--------------------------------------------- CAN CANCEL CONSTRUCTION ------------------------------------
bool UnitInfo::canCancelConstruction(bool checkCommandibility) const
{
	return m_unit->canCancelConstruction(checkCommandibility);
	//return Templates::canCancelConstruction(const_cast<UnitInfo*>(this), checkCommandibility);
}
//--------------------------------------------- CAN CANCEL ADDON -------------------------------------------
bool UnitInfo::canCancelAddon(bool checkCommandibility) const
{
	return m_unit->canCancelAddon(checkCommandibility);
	//return Templates::canCancelAddon(const_cast<UnitInfo*>(this), checkCommandibility);
}
//--------------------------------------------- CAN CANCEL TRAIN -------------------------------------------
bool UnitInfo::canCancelTrain(bool checkCommandibility) const
{
	return m_unit->canCancelTrain(checkCommandibility);
	//return Templates::canCancelTrain(const_cast<UnitInfo*>(this), checkCommandibility);
}
//--------------------------------------------- CAN CANCEL TRAIN SLOT --------------------------------------
bool UnitInfo::canCancelTrainSlot(bool checkCommandibility) const
{
	return m_unit->canCancelTrainSlot(checkCommandibility);
	//return Templates::canCancelTrainSlot(const_cast<UnitInfo*>(this), checkCommandibility);
}
bool UnitInfo::canCancelTrainSlot(int slot, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canCancelTrainSlot(slot, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canCancelTrainSlot(const_cast<UnitInfo*>(this), slot, checkCanIssueCommandType, checkCommandibility);
}
//--------------------------------------------- CAN CANCEL MORPH -------------------------------------------
bool UnitInfo::canCancelMorph(bool checkCommandibility) const
{
	return m_unit->canCancelMorph(checkCommandibility);
	//return Templates::canCancelMorph(const_cast<UnitInfo*>(this), checkCommandibility);
}
//--------------------------------------------- CAN CANCEL RESEARCH ----------------------------------------
bool UnitInfo::canCancelResearch(bool checkCommandibility) const
{
	return m_unit->canCancelResearch(checkCommandibility);
	//return Templates::canCancelResearch(const_cast<UnitInfo*>(this), checkCommandibility);
}
//--------------------------------------------- CAN CANCEL UPGRADE -----------------------------------------
bool UnitInfo::canCancelUpgrade(bool checkCommandibility) const
{
	return m_unit->canCancelUpgrade(checkCommandibility);
	//return Templates::canCancelUpgrade(const_cast<UnitInfo*>(this), checkCommandibility);
}
//--------------------------------------------- CAN USE TECH -----------------------------------------------
bool UnitInfo::canUseTechWithOrWithoutTarget(bool checkCommandibility) const
{
	return m_unit->canUseTechWithOrWithoutTarget(checkCommandibility);
	//return Templates::canUseTechWithOrWithoutTarget(const_cast<UnitInfo*>(this), checkCommandibility);
}
bool UnitInfo::canUseTechWithOrWithoutTarget(BWAPI::TechType tech, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canUseTechWithOrWithoutTarget(tech, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canUseTechWithOrWithoutTarget(const_cast<UnitInfo*>(this), tech, checkCanIssueCommandType, checkCommandibility);
}
bool UnitInfo::canUseTech(BWAPI::TechType tech, Unit target, bool checkCanTargetUnit, bool checkTargetsType, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canUseTech(tech, target, checkCanTargetUnit, checkTargetsType, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canUseTech(const_cast<UnitInfo*>(this), tech, target, checkCanTargetUnit, checkTargetsType, checkCanIssueCommandType, checkCommandibility);
}
bool UnitInfo::canUseTech(BWAPI::TechType tech, Position target, bool checkCanTargetUnit, bool checkTargetsType, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canUseTech(tech, target, checkCanTargetUnit, checkTargetsType, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canUseTech(const_cast<UnitInfo*>(this), tech, target, checkCanTargetUnit, checkTargetsType, checkCanIssueCommandType, checkCommandibility);
}
bool UnitInfo::canUseTechWithoutTarget(BWAPI::TechType tech, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canUseTechWithoutTarget(tech, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canUseTechWithoutTarget(const_cast<UnitInfo*>(this), tech, checkCanIssueCommandType, checkCommandibility);
}
//--------------------------------------------- CAN USE TECH UNIT ------------------------------------------
bool UnitInfo::canUseTechUnit(BWAPI::TechType tech, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canUseTechUnit(tech, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canUseTechUnit(const_cast<UnitInfo*>(this), tech, checkCanIssueCommandType, checkCommandibility);
}
bool UnitInfo::canUseTechUnit(BWAPI::TechType tech, Unit targetUnit, bool checkCanTargetUnit, bool checkTargetsUnits, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canUseTechUnit(tech, targetUnit, checkCanTargetUnit, checkTargetsUnits, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canUseTechUnit(const_cast<UnitInfo*>(this), tech, targetUnit, checkCanTargetUnit, checkTargetsUnits, checkCanIssueCommandType, checkCommandibility);
}
//--------------------------------------------- CAN USE TECH POSITION --------------------------------------
bool UnitInfo::canUseTechPosition(BWAPI::TechType tech, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canUseTechPosition(tech, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canUseTechPosition(const_cast<UnitInfo*>(this), tech, checkCanIssueCommandType, checkCommandibility);
}
bool UnitInfo::canUseTechPosition(BWAPI::TechType tech, Position target, bool checkTargetsPositions, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canUseTechPosition(tech, target, checkTargetsPositions, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canUseTechPosition(const_cast<UnitInfo*>(this), tech, target, checkTargetsPositions, checkCanIssueCommandType, checkCommandibility);
}
//--------------------------------------------- CAN PLACE COP ----------------------------------------------
bool UnitInfo::canPlaceCOP(bool checkCommandibility) const
{
	return m_unit->canPlaceCOP(checkCommandibility);
	//return Templates::canPlaceCOP(const_cast<UnitInfo*>(this), checkCommandibility);
}
bool UnitInfo::canPlaceCOP(TilePosition target, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canPlaceCOP(target, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canPlaceCOP(const_cast<UnitInfo*>(this), target, checkCanIssueCommandType, checkCommandibility);
}
//--------------------------------------------- CAN ISSUE COMMAND TYPE -------------------------------------
bool UnitInfo::canIssueCommandType(BWAPI::UnitCommandType ct, bool checkCommandibility) const
{
	return m_unit->canIssueCommandType(ct, checkCommandibility);
	//return Templates::canIssueCommandType(const_cast<UnitInfo*>(this), ct, checkCommandibility);
}
bool UnitInfo::canIssueCommandTypeGrouped(BWAPI::UnitCommandType ct, bool checkCommandibilityGrouped, bool checkCommandibility) const
{
	return m_unit->canIssueCommandTypeGrouped(ct, checkCommandibilityGrouped, checkCommandibility);
	//return Templates::canIssueCommandTypeGrouped(const_cast<UnitInfo*>(this), ct, checkCommandibilityGrouped, checkCommandibility);
}
//--------------------------------------------- CAN ISSUE COMMAND ------------------------------------------
bool UnitInfo::canIssueCommand(UnitCommand command, bool checkCanUseTechPositionOnPositions, bool checkCanUseTechUnitOnUnits, bool checkCanBuildUnitType, bool checkCanTargetUnit, bool checkCanIssueCommandType, bool checkCommandibility) const
{
	return m_unit->canIssueCommand(command, checkCanUseTechPositionOnPositions, checkCanUseTechUnitOnUnits, checkCanBuildUnitType, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
	//return Templates::canIssueCommand(const_cast<UnitInfo*>(this), command, checkCanUseTechPositionOnPositions, checkCanUseTechUnitOnUnits, checkCanBuildUnitType, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibility);
}
bool UnitInfo::canIssueCommandGrouped(UnitCommand command, bool checkCanUseTechPositionOnPositions, bool checkCanUseTechUnitOnUnits, bool checkCanTargetUnit, bool checkCanIssueCommandType, bool checkCommandibilityGrouped, bool checkCommandibility) const
{
	return m_unit->canIssueCommandGrouped(command, checkCanUseTechPositionOnPositions, checkCanUseTechUnitOnUnits, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibilityGrouped, checkCommandibility);
	//return Templates::canIssueCommandGrouped(const_cast<UnitInfo*>(this), command, checkCanUseTechPositionOnPositions, checkCanUseTechUnitOnUnits, checkCanTargetUnit, checkCanIssueCommandType, checkCommandibilityGrouped, checkCommandibility);
}