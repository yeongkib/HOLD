/*****************************************************************************/
/*!
\file   HoldAIModule.cpp
\author Yeongki Baek
\par    email: yeongki.baek\@digipen.edu
\date   08/01/2017
\brief
This is the interface file for the module
Copyright 2017, Digipen Institute of Technology
*/
/*****************************************************************************/
#include "Precompiled.hpp"

#include "HoldAIModule.hpp"
#include "../BWEM_1_3/bwem.h"

#include "Shapes.hpp"


#include "HeadQuarters.hpp"

#include "Grid.hpp"

#include <BWAPI/UnitType.h>

#include <fstream>
#include <BWAPI/Client/UnitImpl.h>
#include <BWAPI/Client/GameData.h>



using namespace BWAPI;
using namespace Filter;
using namespace HOLD;

//static bool foundEnemyBase = false;


static bool drawRange = false;
static bool drawBoundaries = false;
static bool drawDirection = false;
static bool drawGridMap = false;
static bool drawAll = false;

//static bool firstattack_mutal = false;
//static bool firstattack_hydra = false;

static int mapHeight = 0;
static int mapWidth = 0;


std::vector<std::pair<Position, Position>> DiagonalLineOfBase;
std::vector<Position> mineralpos;

int isPointOnLine(Position p1, Position p2, Position p3) // returns true if p3 is on line p1, p2
{
	if (p3.x >= p1.x && p3.x <= p2.x && (p3.y >= p1.y && p3.y <= p2.y || p3.y <= p1.y && p3.y >= p2.y))
	{
		int vx = p2.x - p1.x;
		int vy = p2.y - p1.y;

		int vcx = p3.x - p1.x;
		int vcy = p3.y - p1.y;

		if (vx * vcy == vy * vcx)
			return 1;
	}
	return 0;
}

#include "BehaviorTree.hpp"
#include "Drone.hpp"

 


BWAPI::Position NextTarget()
{
	return Position();
};



BehaviorTree behaviorTree;



// Combine two char into short int
short int stitch(char c1, char c2) {
	return (static_cast<unsigned char>(c2) << 8) | static_cast<unsigned char>(c1);
}












std::vector<std::pair<Position, float>> distBtwMineral;

std::vector<std::pair<Position, Position>> lineBtwMineral;



Vector2 closest_point(const Vector2& v, const Vector2 & w, const Vector2 & p)
{
	// Return minimum distance between line segment vw and point p
	const float l2 = (w - v).LengthSq();  // i.e. |w-v|^2 -  avoid a sqrt
										  //if (l2 == 0.0) return Math::Distance(p, v);   // v == w case
										  //										// Consider the line extending the segment, parameterized as v + t (w - v).
										  //										// We find projection of point p onto the line. 
										  //										// It falls where t = [(p-v) . (w-v)] / |w-v|^2
										  //										// We clamp t from [0,1] to handle points outside the segment vw.
	const float t = Math::Max(0.f, Math::Min(1.f, Math::Dot(p - v, w - v) / l2));
	return v + t * (w - v);  // Projection falls on the segment
							 //return Math::Distance(p, projection);
}

float intersection(const Box& b, const Ray& r) {
	float t1 = (b.GetMin().x - r.mStart.x)*r.mDirection.x;
	float t2 = (b.GetMax().x - r.mStart.x)*r.mDirection.x;

	float tmin = Math::Min(t1, t2);

	t1 = (b.GetMin().y - r.mStart.y)*r.mDirection.y;
	t2 = (b.GetMax().y - r.mStart.y)*r.mDirection.y;

	tmin = Math::Max(tmin, Math::Min(t1, t2));

	return tmin;
}

float distance(const Box& r, const Vector2& p) {
	float squared_dist = 0.0f;

	if (p.x > r.GetMax().x)
		squared_dist += (p.x - r.GetMax().x)*(p.x - r.GetMax().x);
	else if (p.x < r.GetMin().x)
		squared_dist += (r.GetMin().x - p.x)*(r.GetMin().x - p.x);

	if (p.y > r.GetMax().y)
		squared_dist += (p.y - r.GetMax().y)*(p.y - r.GetMax().y);
	else if (p.y < r.GetMin().y)
		squared_dist += (r.GetMin().y - p.y)*(r.GetMin().y - p.y);

	return sqrt(squared_dist);


}

float distance(const Box& b1, const Box& b2)
{
	bool left = b2.GetMax().x < b1.GetMin().x;
	bool right = b1.GetMax().x< b2.GetMin().x;
	bool bottom = b2.GetMax().y < b1.GetMin().y;
	bool top = b1.GetMax().y < b2.GetMin().y;
	if (top && left)
		return Math::Distance(Vector2(b1.GetMin().x, b1.GetMax().y), Vector2(b2.GetMax().x, b2.GetMin().y));
	else if (left && bottom)
		return Math::Distance(Vector2(b1.GetMin().x, b1.GetMin().y), Vector2(b2.GetMax().x, b2.GetMax().y));
	else if (bottom && right)
		return Math::Distance(Vector2(b1.GetMax().x, b1.GetMin().y), Vector2(b2.GetMin().x, b2.GetMax().y));
	else if (right && top)
		return Math::Distance(Vector2(b1.GetMax().x, b1.GetMax().y), Vector2(b2.GetMin().x, b2.GetMin().y));
	else if (left)
		return b1.GetMin().x - b2.GetMax().x;
	else if (right)
		return b2.GetMin().x - b1.GetMax().x;
	else if (bottom)
		return b1.GetMin().y - b2.GetMax().y;
	else if (top)
		return b2.GetMin().y - b1.GetMax().y;
	else            // rectangles intersect
		return 0;
}

float distance(float x, float y, float px, float py, float width, float height)
{
	float dx = Math::Max(abs(px - x) - width / 2.f, 0.f);
	float dy = Math::Max(abs(py - y) - height / 2.f, 0.f);
	return dx * dx + dy * dy;
}

bool analyzed;
bool analysis_just_finished;


std::vector<BWEM::Base> rechableBases;

int BestMineral(int SCVindex, int depth) {
	return 0;
}


void HoldAIModule::onStart()
{
	runflag = true;
	if (strcmp(Broodwar->mapHash().c_str(), "ed7c5b1b03234a0f7dd484112bbb1bc49db1d6f0") == 0) // testcase
		runflag = false;

	Broodwar->printf("2018_03_31_11:48");

	
	try
	{
		//DataContext data;
		//Unitset_drone drones(5);  // Building with 5 doors to get in.
		//BehaviorTree::Sequence sequence[3];
		//BehaviorTree::Selector selector;
		//BehaviorTree::Inverter inverter[2];
		//BehaviorTree::Succeeder succeeder;
		//BehaviorTree::RepeatUntilFail untilFail;
		//BehaviorTree::GetStack<zerg_drone> getDroneStackFromUnitset(data.drones, drones.getDrones());
		//BehaviorTree::PopFromStack<zerg_drone> popFromStack(data.currentResource, data.drones);
		//BehaviorTree::SetVariable<zerg_drone> setVariable(data.ResourceTarget, data.currentResource);
		//BehaviorTree::IsNull<zerg_drone> isNull(data.ResourceTarget);
		//Gathering Gather("Walk to resource", 99), Mining("Mining", 15), Move("Move", 25), Return("Return to cargo", 60);

		//behaviorTree.setRootChild(&sequence[0]);
		//sequence[0].addChildren({ &getDroneStackFromUnitset, &untilFail, &inverter[0] });
		//untilFail.setChild(&sequence[1]);
		//inverter[0].setChild(&isNull);
		//sequence[1].addChildren({ &popFromStack, &inverter[1] });
		//inverter[1].setChild(&sequence[2]);
		//sequence[2].addChildren({ &Gather, &selector, &Mining, &succeeder, &setVariable });
		//selector.addChildren({ &Gather, &Mining, &Move, &Return });
		//succeeder.setChild(&Return);




		Broodwar->setCommandOptimizationLevel(1);
		Broodwar->setLocalSpeed(0);
		Broodwar->setFrameSkip(0);
		//Broodwar->setLocalSpeed(40);
		//Broodwar->setFrameSkip(16);

#ifndef _DEBUG
#else
		Broodwar->sendText("show me the money");
		Broodwar->sendText("show me the money");
		Broodwar->sendText("show me the money");
		Broodwar->sendText("show me the money");
		Broodwar->sendText("operation cwal");
		Broodwar->sendText("food for thought");
#endif
		/*Broodwar->sendText("operatioan cwal");
		Broodwar->sendText("show me the money");*/
		/*Broodwar->sendText("power overwhelming");
		Broodwar->sendText("operation cwal");
		Broodwar->sendText("the gathering");
		Broodwar->sendText("modify the phase variance");
		Broodwar->sendText("food for thought");
		Broodwar->sendText("war aint what it used to be");
		Broodwar->sendText("staying alive");
		Broodwar->sendText("show me the money");
		Broodwar->sendText("show me the money");
		Broodwar->sendText("show me the money");
		Broodwar->sendText("show me the money");
		Broodwar->sendText("show me the money");*/
		/*std::string cheat("/cheats");
		Broodwar->sendText("%s", cheat.c_str());*/

		//turn off debug info
		//Broodwar->setGUI(false);



		HQ = HeadQuarters::GetInstance();

		/*if( runflag)
			HQ->AttachCommander(new LogisticsCommand);
		*/
		HQ->AttachCommander(new IntelligenceCommand);
		HQ->Initialize();

		mapHeight = Broodwar->mapHeight();
		mapWidth = Broodwar->mapWidth();



		for (Unit u : Broodwar->self()->getUnits())
		{
			int type = u->getType();
			// just skip this for now
			/*if (type == UnitTypes::Enum::Zerg_Larva)
			continue;*/

			if (runflag)
			{
				//HQ->GetCommand<IntelligenceCommand>()->AddUnit(u);
				

				//UnitDataSets[Broodwar->self()][u->getType()].m_units.emplace(u);
				//++MyUnitSets[u->getType()].Completed;
			}

			/*
			if (u->getType() == UnitTypes::Enum::Zerg_Hatchery)
			{
				(*UnitDataSets[Broodwar->self()][u->getType()].m_units.begin())->setClientInfo(true, '1st');
			}
			*/

			if (!u->gather(u->getClosestUnit(IsMineralField)))
			{
				// If the call fails, then print the last error message
				Broodwar << Broodwar->getLastError() << std::endl;
			}
		}

		// Print the map name.
		// BWAPI returns std::string when retrieving a string, don't forget to add .c_str() when printing!
		Broodwar << "The map is " << Broodwar->mapName() << "!" << std::endl;

		// Enable the UserInput flag, which allows us to control the bot and type messages.
		Broodwar->enableFlag(Flag::UserInput);

		// Check if this is a replay
		if (Broodwar->isReplay())
		{

			// Announce the players in the replay
			Broodwar << "The following players are in this replay:" << std::endl;

			// Iterate all the players in the game using a std:: iterator
			Playerset players = Broodwar->getPlayers();
			for (auto p : players)
			{
				// Only print the player if they are not an observer
				if (!p->isObserver())
					Broodwar << p->getName() << ", playing as " << p->getRace() << std::endl;
			}

		}
		else // if this is not a replay
		{
			// Retrieve you and your enemy's races. enemy() will just return the first enemy.
			// If you wish to deal with multiple enemies then you must use enemies().
			if (Broodwar->enemy()) // First make sure there is an enemy
				Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;

			Broodwar << "Map initialization..." << std::endl;

			/*if (runflag)
			{
				theMap.Initialize();
				theMap.EnableAutomaticPathAnalysis();
				bool startingLocationOK = theMap.FindBasesForStartingLocations();
				assert(startingLocationOK);
			}*/

			Broodwar << "gl hf" << std::endl;
		}

		
		analyzed = false;
		analysis_just_finished = false;

		
	}
	catch (const std::exception & e)
	{
		Broodwar << "EXCEPTION from " << __func__ << ": " << e.what() << std::endl;
	}

}

void HoldAIModule::onEnd(bool isWinner)
{
	// Called when the game ends
	if (isWinner)
	{
		HQ->Shutdown();

		distBtwMineral.clear();
		lineBtwMineral.clear();
		/*MyUnitSets.clear();
		EnemyUnitSets.clear();*/
		/*for (auto ds : UnitDataSets)
		{
			ds.second.clear();
		}*/
		//UnitDataSets[Broodwar->self()].clear();

		/*
		std::vector<Grid>().swap(myinfluenceGround);
		std::vector<Grid>().swap(myinfluenceAir);
		*/
		/*std::vector<Grid>().swap(opinfluenceGround);
		std::vector<Grid>().swap(opinfluenceAir);
		std::vector<Grid>().swap(influenceGround);
		std::vector<Grid>().swap(influenceAir);
		std::vector<Grid>().swap(tensionGround);
		std::vector<Grid>().swap(tensionAir);
		std::vector<Grid>().swap(tensionTotal);
		std::vector<Grid>().swap(vulGround);
		std::vector<Grid>().swap(vulAir);
		std::vector<Grid>().swap(vulTotal);*/
	}
}




void HoldAIModule::onFrame()
{/*
	for( auto & u : unit_infos_)
	{
		Broodwar->drawTextMap(u()->getPosition(), "%d", u.GetAverageHP(24));
	}
*/
	if (runflag)
	{
		if(Broodwar->getStartLocations().size() != 2)
			if (Broodwar->getFrameCount() < 10)
			{
				auto it = std::find(std::begin(HQ->GetCommand<IntelligenceCommand>()->startingLocations), std::end(HQ->GetCommand<IntelligenceCommand>()->startingLocations), Broodwar->self()->getStartLocation());
				++it;
				if (it == std::end(HQ->GetCommand<IntelligenceCommand>()->startingLocations))
					it = std::begin(HQ->GetCommand<IntelligenceCommand>()->startingLocations);

				(HQ->GetCommand<IntelligenceCommand>()->Units[Broodwar->self()][UnitTypes::Enum::Zerg_Overlord]).move(Position{ *it });
			}
	}

	/*if (Broodwar->getFrameCount() % 50 == 0)
	{
		for(auto su : Broodwar->self()->getUnits())
		{
			if(su->getType() == UnitTypes::Protoss_High_Templar)
			{
				su->issueCommand(UnitCommand::useTech(su, TechTypes::Psionic_Storm, Position{ 800, 3400 }));
			}
			else if(su->getType() == UnitTypes::Enum::Zerg_Defiler)
			{
				su->issueCommand(UnitCommand::useTech(su, TechTypes::Plague, Position{ 1050, 3400 }));
			}
		}
	}*/

	//for (auto u : Broodwar->self()->getUnits())weap
	//{
	//	if (u->getType() == UnitTypes::Zerg_Zergling)
	//		Broodwar->drawBoxMap(u->getLeft(), u->getTop(), u->getRight(), u->getBottom(), Colors::White);

	//	/*if (Broodwar->getKeyState(Key::K_A))
	//		if (u->getType() == UnitTypes::Enum::Zerg_Mutalisk)
	//			u->issueCommand(BWAPI::UnitCommand::attack(nullptr, Broodwar->getScreenPosition() + Broodwar->getMousePosition()));*/
	//}
		
	//zerg_swarm, web
	/*for(auto &spell : Broodwar->getNeutralUnits())
	{
		if (!spell->getType().isBuilding())
		{
			Broodwar->drawTextMap(spell->getPosition(), "%s", spell->getType().c_str());

			Broodwar->drawBoxMap(spell->getLeft(), spell->getTop(), spell->getRight(), spell->getBottom(), Colors::Red);

			Broodwar->drawTextMap(spell->getPosition(), "%c%d - %d = %d \n %d - %d = %d", Text::White, spell->getRight(), spell->getLeft(), spell->getRight() - spell->getLeft(), spell->getTop(), spell->getBottom(), spell->getTop() - spell->getBottom());
		}
	}*/

	//for (auto &b : Broodwar->getBullets())
	//{
	//	Position p = b->getPosition();
	//	double velocityX = b->getVelocityX();
	//	double velocityY = b->getVelocityY();
	//	Broodwar->drawLineMap(p, p + Position((int)velocityX, (int)velocityY), b->getPlayer() == Broodwar->self() ? Colors::Green : Colors::Red);
	//	Broodwar->drawTextMap(p, "%c%s", b->getPlayer() == Broodwar->self() ? Text::Green : Text::Red, b->getType().c_str());

	//	if (b->getType() == BulletTypes::Enum::Psionic_Storm)
	//	{
	//		Broodwar->drawCircleMap(b->getPosition(), 50, Colors::Red);
	//	}

	//	if (b->getType() == BulletTypes::Enum::Plague_Cloud)
	//	{
	//		Position a{ 65, 65 };
	//		Broodwar->drawBoxMap(b->getPosition() - a, b->getPosition() + a, Colors::Red);
	//	}

	//	if(b->getType() == BulletTypes::Enum::n)//nuclear
	//}

	try
	{
		auto m_StartTime = std::chrono::high_resolution_clock::now();
		//std::chrono::time_point<std::chrono::system_clock> m_StartTime = std::chrono::system_clock::now();
		//std::chrono::high_resolution_clock::time_point m_StartTime = std::chrono::high_resolution_clock::now();
		HQ->Run();

		for (auto worker : HQ->GetCommand<IntelligenceCommand>()->Units[Broodwar->self()][UnitTypes::Zerg_Drone])
		{
			float closest = std::numeric_limits<float>::max();
			Vector2 cp{ 0,0 };
			Position begin{ 0,0 };
			for (auto line : DiagonalLineOfBase)
			{
				Vector2 projection{ closest_point(Vector2{ 1.f * line.first.x, 1.f * line.first.y },
					Vector2{ 1.f * line.second.x, 1.f * line.second.y },
					Vector2{ 1.f * worker->getPosition().x, 1.f * worker->getPosition().y }) };
				float distance = Math::Distance(Vector2{ static_cast<float>(worker->getPosition().x), static_cast<float>(worker->getPosition().y) }, projection);
				if (distance < closest)
				{
					closest = distance;
					cp = projection;
					begin = line.first;
				}
			}

			
			for(auto mineral : mineralpos)
			{
				for(auto hat : HQ->GetCommand<IntelligenceCommand>()->Units[Broodwar->self()][UnitTypes::Zerg_Hatchery])
					Broodwar->drawLineMap(mineral, hat->getPosition(), Colors::Red);
			}
			//todo : find proper equation
			// variables : distance, angle, speed
			/*
			if (closest < 16.27
				&& closest > 16.25)
				worker->m_unit->issueCommand(UnitCommand::returnCargo(nullptr));
			*/
			#define PI 3.14159265358979323846

			Broodwar->drawLineMap(worker->getPosition(), Position{ static_cast<int>(cp.x), static_cast<int>(cp.y) }, Colors::Red);
			Broodwar->drawTextMap(worker->getPosition() + Position{ 0, 10 }, "%.2f", closest);
			Broodwar->drawTextMap(worker->getPosition() + Position{ 0, 20 }, "%.2f", worker->getAngle() * 180.0 / PI);
			//Broodwar->drawTextMap(worker->getPosition() + Position{ 0, 30 }, "%d", worker->getType().haltDistance());
			//Broodwar->drawTextMap(worker->getPosition() + Position{ 0, 40 }, "%d", worker->getType().turnRadius());

			Broodwar->drawTextMap(worker->getPosition() + Position{ 0, 30 }, "%d, %d", worker->getPosition().x, worker->getPosition().y);

			Broodwar->drawLineMap(worker->getPosition(), worker->getTargetPosition(), Colors::Orange);
			drawBoundary(UnitTypes::Enum::Zerg_Drone, worker->getTargetPosition(), Colors::Yellow);


			Broodwar->drawTextMap(worker->getPosition() + Position{ 0, 40 }, "%f", sqrt(worker->getVelocityX()*worker->getVelocityX() +  worker->m_unit->getVelocityY()*worker->m_unit->getVelocityY()));
			

			
			
			float distfromcenter = Math::Distance(Vector2{ static_cast<float>(worker->getPosition().x), static_cast<float>(worker->getPosition().y) },
				Vector2{ static_cast<float>(begin.x), static_cast<float>(begin.y) });
				Broodwar->drawTextMap(worker->getPosition() + Position{ 0, 50 }, "%.2f", distfromcenter);


			
			Broodwar->drawTextMap(worker->getPosition() + Position{ 0, 60 }, "%d, %d", worker->getOrderTargetPosition().x - worker->getPosition().x,
																					worker->getOrderTargetPosition().y - worker->getPosition().y);

			Broodwar->drawCircleMap(worker->getOrderTargetPosition(), 2, Colors::Red, true);

			//Broodwar->drawTextMap(worker->getPosition() + Position{ 0, -20 }, "%d", static_cast<int>((worker->getVelocityX()*worker->getVelocityX() + worker->m_unit->getVelocityY()*worker->m_unit->getVelocityY()) / static_cast<double>(worker->getType().acceleration() * 2)));
			//Broodwar->drawTextMap(worker->getPosition() + Position{ 0, -20 }, "%d", static_cast<int>((worker->getType().topSpeed() * worker->getType().topSpeed()) / static_cast<double>(worker->getType().acceleration() * 2.0)));
			//Broodwar->drawTextMap(worker->getPosition() + Position{ 0, -20 }, "%f", worker->getType().topSpeed());
			//Broodwar->drawTextMap(worker->getPosition() + Position{ 0, -30 }, "%d", worker->getType().acceleration());
			//double angleN = atan2(static_cast<double>(worker->getTargetPosition().y - worker->getPosition().y), static_cast<double>(worker->getTargetPosition().x - worker->getPosition().x));// *180.0 / PI;
			
			//Broodwar->drawLineMap(worker->getPosition().x, worker->getPosition().y,
			//	static_cast<int>(worker->getPosition().x + cos(worker->getAngle()) * 10.0), static_cast<int>(worker->getPosition().y + sin(worker->getAngle()) * 10.0), Colors::White);
			DrawDirection(worker->m_unit, 30.0);
			

			/*float dottt = Dot(Vector2(static_cast<double>((worker->getTargetPosition().x + worker->getPosition().x)), static_cast<double>((worker->getTargetPosition().y + worker->getPosition().y))).Normalized(),
				Vector2(static_cast<double>(worker->getPosition().x + cos(worker->getAngle()) * 10.0), static_cast<double>(worker->getPosition().y + sin(worker->getAngle()) * 10.0)).Normalized());


			double angle1 = atan2(worker->getPosition().y - worker->getTargetPosition().y + DebugEpsilon(), worker->getPosition().x - worker->getTargetPosition().x + DebugEpsilon());
			double angle2 = atan2(-sin(worker->getAngle() - (90.0 * PI / 180.0)), -cos(worker->getAngle() - (90.0 * PI / 180.0)));
			double result = (angle2 - angle1) * 180.0 / PI;
			if (result<DebugEpsilon()) {
				result += 360.0;
			}*/


			double theta = worker->getAngle() + (90.0 * PI / 180.0);


			//vec1 : from current position to target position
			double x1 = worker->getTargetPosition().x - worker->getPosition().x;
			double y1 = worker->getTargetPosition().y - worker->getPosition().y;
			
			//vec2 : direction vector
			double x2 = worker->getPosition().x + cos(theta);// worker->getAngle());
			double y2 = worker->getPosition().y + sin(theta);// worker->getAngle());

			double sin1 = x1 * y2 - x2 * y1;
			double cos1 = x1 * x2 + y1 * y2;

			double angle = atan2(sin1, cos1) * 180.0 / PI;

			Broodwar->drawTextMap(worker->getPosition() + Position{ 0, -20 }, "%d, %d", 
				static_cast<int>(worker->getPosition().x + worker->getVelocityX() * cos(angle)),
				static_cast<int>(worker->getPosition().y + worker->getVelocityY() * sin(angle)));


			Broodwar->drawTextMap(worker->getPosition() + Position{ 0, -30 }, "%f, %f",
				worker->getAngle() * 180.0 / PI, angle);

			/*double angle = atan2(worker->getPosition().y + sin(theta) - worker->getTargetPosition().y, worker->getPosition().x + cos(theta) - worker->getTargetPosition().x);
			angle = angle * 180.0 / PI;*/



			/*double dx21 = static_cast<double>(worker->getTargetPosition().x - worker->getPosition().x) + DebugEpsilon();
			double dx31 = static_cast<double>(cos(worker->getAngle()) * 10.0);
			double dy21 = static_cast<double>(worker->getTargetPosition().y - worker->getPosition().y) + DebugEpsilon();
			double dy31 = static_cast<double>(sin(worker->getAngle()) * 10.0);
			double m12 = sqrt(dx21*dx21 + dy21*dy21);
			double m13 = sqrt(dx31*dx31 + dy31*dy31);
			double theta = acos((dx21*dx31 + dy21*dy31) / (m12 * m13));

			Broodwar->drawCircleMap(worker->getPosition().x + static_cast<int>(cos(theta + worker->getAngle()) * 47.0), static_cast<int>(worker->getPosition().y + sin(theta + worker->getAngle()) * 47.0), 2, Colors::Green, true);*/

			//Broodwar->drawCircleMap(worker->getPosition().x + static_cast<int>(cos(theta) * static_cast<double>(worker->getType().turnRadius()) / 32.0), static_cast<int>(worker->getPosition().y + sin(theta) * static_cast<double>(worker->getType().turnRadius()) / 32.0), 2, Colors::Green, true);
			
			//float dottt = Dot(Vector2(static_cast<double>((worker->getTargetPosition().y - worker->getPosition().y) + DebugEpsilon()), static_cast<double>((worker->getTargetPosition().x - worker->getPosition().x)+DebugEpsilon())).Normalized(),
			//	Vector2(static_cast<double>(worker->getPosition().x + cos(worker->getAngle()) * 10.0), static_cast<double>(worker->getPosition().y + sin(worker->getAngle()) * 10.0)).Normalized());

			//double angleN = acos(Dot(Vector2(static_cast<double>(worker->getTargetPosition().y - worker->getPosition().y), static_cast<double>(worker->getTargetPosition().x - worker->getPosition().x) ).Normalized(),
			//	Vector2( static_cast<double>(worker->getPosition().x - cos(worker->getAngle()) * 10.0), static_cast<double>(worker->getPosition().y - sin(worker->getAngle()) * 10.0) ).Normalized()));// * 180.0 / PI;

			/*double angleN = (atan2(static_cast<double>(worker->getTargetPosition().y - worker->getPosition().y), static_cast<double>(worker->getTargetPosition().x - worker->getPosition().x))
				- atan2(static_cast<double>(worker->getPosition().x + cos(worker->getAngle()) * 10.0), static_cast<double>(worker->getPosition().y + sin(worker->getAngle()) * 10.0))); * 180.0 / PI;*/


			//Broodwar->drawTextMap(worker->getPosition() + Position{ 0, -20 }, "%f", angle);// *180.0 / PI );//acos(dottt * PI / 180.0) );

			//Broodwar->drawCircleMap(worker->getPosition() + Position{static_cast<int>(cos(angleN * 180 / PI/** PI / 180.0*/) * 20.0 /*static_cast<double>(worker->getVelocityX())*/), static_cast<int>(sin(angleN * 180 / PI/** PI / 180.0*/) * 20.0 /*static_cast<double>(worker->getVelocityY())*/)}, 2, Colors::White, true);




/*
			Broodwar->drawTextMap(worker->getPosition() + Position{ 0, -10 }, "%.2f, %.2f", worker->getVelocityX(), worker->getVelocityY());
			//Calculate the radius of the turn. V
			//Calculate acceleration and add to velocity. a = v^2/r
			double accl = worker->getVelocityX()*worker->getVelocityX() + worker->m_unit->getVelocityY()*worker->m_unit->getVelocityY() / static_cast<double>(worker->getType().turnRadius() / 256.0);

			//Use velocity magnitude to determine distance traveled in one update.
			int distancetravled = sqrt(worker->getVelocityX()*worker->getVelocityX() + worker->m_unit->getVelocityY()*worker->m_unit->getVelocityY());

			//Use arctan(distanceTraveled / turnRadius) to get the angle of rotation.
			double rotation = ArcTan(static_cast<double>(distancetravled) / static_cast<double>(worker->getType().turnRadius() / 256.0));

			//Update vehicle angle.
			//Rotate velocity by vehicle angle.e.g.velocity *= Quaternion.AngleAxis(angle, Vector3.up)
			//Update vehicle position with velocity.

			Position newpos{ worker->getPosition() * rotation * accl };

			Broodwar->drawCircleMap(newpos, 3, Colors::White, true);

			DrawDirection(worker->m_unit, 30.0);
			*/

		}


		/*for (auto su : Broodwar->getSelectedUnits())
		{
			Position origin{su->getPosition()};
			if (Broodwar->getKeyState(K_P))
				origin = su->getPosition();
			if(su->isMoving() && Broodwar->getKeyState(K_P) && Broodwar->getFrameCount() % 50 == 0)
			{
				su->patrol(origin + Position{ 0, 150 }, true);
				su->patrol(origin + Position{ 150, 150 }, true);
				su->patrol(origin + Position{ 150, 0 }, true);
				su->patrol(origin, true);
			}
			
		}*/
		
		
		
		//for (const BWEM::Area & area : theMap.Areas())
		//const TilePosition myStartingPos = Broodwar->self()->getStartLocation();

		//todo: uncomment
		//const BWEM::Area * area = theMap.GetArea(Broodwar->self()->getStartLocation());
		//int cpx, cpy;

		//for (const BWEM::ChokePoint * cp : area->ChokePoints())
		//	for (BWEM::ChokePoint::node end : {BWEM::ChokePoint::end1, BWEM::ChokePoint::end2})
		//	{
		//		//Broodwar->drawLineMap(Position(cp->Pos(BWEM::ChokePoint::middle)), Position(cp->Pos(end)), BWEM::utils::MapDrawer::Color::cp);
		//		//Broodwar->drawTextMap(cp->Center().x * 8, cp->Center().y * 8, "%c%d, %d", Text::Cyan, cp->Center().x * 8, cp->Center().y * 8);
		//		//Broodwar->drawTextMap(Position(cp->Pos(BWEM::ChokePoint::middle)), "%d, %d", cp->Pos((BWEM::ChokePoint::middle)).x, cp->Pos((BWEM::ChokePoint::middle)).y);
		//		cpx = Position(cp->Pos(BWEM::ChokePoint::middle)).x;
		//		cpy = Position(cp->Pos(BWEM::ChokePoint::middle)).y;
		//	}






		/*if (behaviorTree.run())
		std::cout << "It's running" << std::endl;
		else
		std::cout << "Done" << std::endl;*/
		// Called once every game frame

		//todo: BWEM Part
		//BWEM::utils::gridMapExample(theMap);
		//BWEM::utils::drawMap(theMap);

		// Display the game frame rate as text in the upper left area of the screen
		Broodwar->drawTextScreen(415, 15, "%cFrames%c%d", Text::White, Text::Green, Broodwar->getFrameCount());
		Broodwar->drawTextScreen(500, 15, "%cFPS%c%d", Text::White, Text::Green, Broodwar->getFPS());
		//Broodwar->drawTextScreen(560, 15, "%cAFPS%c%.f", Text::White, Text::Green, Broodwar->getAverageFPS());


		int frameCount = Broodwar->getFrameCount();
		//BWAPI::Broodwar->drawTextScreen(500, 25, "\x04%d %4dm %3ds", BWAPI::Broodwar->getFrameCount(), (int)(BWAPI::Broodwar->getFrameCount() / (23.8 * 60)), (int)((int)(BWAPI::Broodwar->getFrameCount() / 23.8) % 60));
		Broodwar->drawTextScreen(300, 0, "\x04 %4dm %3ds", (int)(frameCount / (24 * 60)), (int)((int)(frameCount / 24) % 60));

		// Return if the game is a replay or is paused
		if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
			return;



		//todo: make sure
		//Broodwar->drawTextScreen(400, 100, "rechable%c%d", Text::Cyan, rechableBases.size());
		// Prevent spamming by only running our onFrame once every number of latency frames.
		// Latency frames are the number of frames before commands are processed.
		if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
			return;


		Unitset larvaAndOver;
		Unitset mutalAndOver;

		
		//for (auto u : Broodwar->self()->getUnits())
		//{
		//	if (drawDirection)
		//		if (!u->getType().isBuilding()
		//			&& u->getType() != UnitTypes::Enum::Zerg_Larva
		//			&& u->getType() != UnitTypes::Enum::Zerg_Egg)
		//			HOLD::DrawDirection(u, 50.0);



		//	// see unitupdate.cpp for reference
		//	// exception; //todo: ?? How to handle undefined behavior
		//	if (u->getType() == UnitTypes::Unknown) continue;
		//	// Register an event that draws the ID
		//	/*Broodwar->registerEvent([=](Game*)
		//	{*/
		//	if (drawRange)
		//	{
		//		/*Broodwar->setTextSize(Text::Size::Huge);
		//		Broodwar->drawTextMap(u->getPosition(), "\n\n%d", u->getType().groundWeapon().maxRange());*/
		//		Broodwar->setTextSize(Text::Size::Large);
		//		Broodwar->drawTextMap(u->getPosition(),
		//			"%c%.2f\n ", u->isAccelerating() ? Text::Grey : Text::Red, abs(u->getVelocityX()) + abs(u->getVelocityY()));// , Text::BrightRed, i->getGroundWeaponCooldown());
		//		Broodwar->setTextSize(Text::Size::Small);

		//		//Broodwar->drawTextMap(u->getPosition(),
		//		//	"%c#%d %s\n %.2f\n%d, %d\n%d", u->isAccelerating() ? Text::Grey : Text::Red, u->getID(), u->getOrder().c_str(), abs(u->getVelocityX()) + abs(u->getVelocityY()), u->getOrderTargetPosition().x, u->getOrderTargetPosition().y, u->getType().sightRange());// , Text::BrightRed, i->getGroundWeaponCooldown());

		//		if (u->getTarget() != nullptr)
		//		{
		//			if (u->getTargetPosition() != Positions::Invalid
		//				&& u->getTargetPosition() != Positions::None
		//				&& u->getTargetPosition() != Positions::Unknown
		//				&& u->getTargetPosition() != Positions::Origin) Broodwar->drawLineMap(u->getPosition(), u->getTargetPosition(), Colors::Orange);
		//		}

		//		else if (u->getOrderTarget() != nullptr)
		//		{
		//			if (u->getOrderTargetPosition() != Positions::Invalid
		//				&& u->getOrderTargetPosition() != Positions::None
		//				&& u->getOrderTargetPosition() != Positions::Unknown
		//				&& u->getOrderTargetPosition() != Positions::Origin) Broodwar->drawLineMap(u->getPosition(), u->getOrderTargetPosition(), Colors::Orange);
		//		}

		//		//"%c#%d %s\n %.2f::%.2f", Text::Grey, i->getID(), i->getOrder().c_str(), i->getVelocityX(), i->getVelocityY());// , Text::BrightRed, i->getGroundWeaponCooldown());

		//		//HOLD::drawWeaponCooldown(u);
		//	}

		//	/*int length = 0;
		//	BWEM::CPPath cpPath = theMap.GetPath();
		//	*/
		//	//Unit target = u->getClosestUnit(IsEnemy, 300);
		//	/*auto canAttackAir = [](auto a, auto b)
		//	{
		//	if (a->getType().groundWeapon().targetsAir()
		//	|| a->getType().airWeapon().targetsAir())
		//	return a;
		//	else if (b->getType().groundWeapon().targetsAir()
		//	|| b->getType().airWeapon().targetsAir())
		//	return b;
		//	else
		//	{

		//	}
		//	};*/

		//	//auto canAttackAir = [u](Unit a, Unit b)->Unit
		//	//{
		//	//	/*if (a->getType().groundWeapon().targetsAir()
		//	//		|| a->getType().airWeapon().targetsAir())
		//	//		return a;
		//	//	else if (b->getType().groundWeapon().targetsAir()
		//	//		|| b->getType().airWeapon().targetsAir())
		//	//		return b;
		//	//	else
		//	//		return nullptr;*/
		//	//	double distua = d
		//	//	double distub = 
		//	//	if (a->canAttack(u))
		//	//		return a;
		//	//	else if (b->canAttack(u))
		//	//		return b;
		//	//	else
		//	//		return nullptr;
		//	//};
		//	//Unit target = Broodwar->getBestUnit(canAttackAir, IsEnemy, u->getPosition(), 300);

		//	/*if (target)
		//	{
		//	BWAPI::Position fleeVec(u->getPosition() - target->getPosition());
		//	double fleeAngle = atan2(fleeVec.y, fleeVec.x);
		//	fleeVec = BWAPI::Position(static_cast<int>(64.0 * cos(fleeAngle)), static_cast<int>(64.0 * sin(fleeAngle)));
		//	Broodwar->drawLineMap(u->getPosition(), u->getPosition() + fleeVec, Colors::Red);
		//	Broodwar->drawCircleMap(target->getPosition(), 5, Colors::Red, true);
		//	}*/
		//	Unitset targets;
		//	if (u->isFlying())
		//		targets = u->getUnitsInRadius((WeaponTypes::Hellfire_Missile_Pack).maxRange() + 3 * 32/*upgrade*/ + 16, IsEnemy);
		//	else
		//		targets = u->getUnitsInRadius((WeaponTypes::Arclite_Cannon_Edmund_Duke).maxRange() + 16, IsEnemy);

		//	//this is stand to check ground threaten
		//	//(WeaponTypes::Arclite_Cannon_Edmund_Duke).maxRange() //siege tank

		//	//this is for air
		//	//(WeaponTypes::Hellfire_Missile_Pack).maxRange() //goliath

		//	if (u->getType() == UnitTypes::Enum::Zerg_Mutalisk)
		//		mutalAndOver.insert(u);
		//	/*int closestDist = 10000000;
		//	BWAPI::Unit closestTarget = nullptr;
		//	for(auto target : targets)
		//	{
		//	if(target->canAttack(u, false, false, false))
		//	{
		//	int dist = target->getDistance(u);
		//	if (dist < closestDist)
		//	{
		//	closestDist = dist;
		//	closestTarget = target;
		//	}
		//	}
		//	}
		//	if (closestTarget)
		//	{
		//	BWAPI::Position fleeVec(u->getPosition() - closestTarget->getPosition());
		//	double fleeAngle = atan2(fleeVec.y, fleeVec.x);
		//	fleeVec = BWAPI::Position(static_cast<int>(64.0 * cos(fleeAngle)), static_cast<int>(64.0 * sin(fleeAngle)));
		//	Broodwar->drawLineMap(u->getPosition(), u->getPosition() + fleeVec, Colors::Red);
		//	Broodwar->drawCircleMap(closestTarget->getPosition(), 5, Colors::Red, true);

		//	Position forwardVec(closestTarget->getPosition() - u->getPosition());
		//	Broodwar->drawLineMap(u->getPosition(), u->getPosition(), Colors::Blue);


		//	float cooldown = static_cast<float>(u->getGroundWeaponCooldown()) / static_cast<float>(u->getType().groundWeapon().damageCooldown());

		//	if(cooldown > 0.5f)
		//	{
		//	u->issueCommand(UnitCommand::move(u, u->getPosition() + fleeVec));
		//	}
		//	else
		//	{
		//	u->issueCommand(UnitCommand::move(u, u->getPosition() + fleeVec));
		//	u->issueCommand(UnitCommand::rightClick(u, closestTarget));
		//	}
		//	}*/

		//	//},
		//	//	nullptr,  // condition
		//	//	Broodwar->getLatencyFrames());  // frames to run


		//} // closure: failed to train idle unit
		  //todo : uncomment
		  //		mutalAndOver.insert(*MyUnitSets[UnitTypes::Enum::Zerg_Overlord].m_units.begin());

		  // draw direction vector
		/*if (drawDirection)
		{
			for (auto enemy : Broodwar->enemies())
			{
				for (auto u : enemy->getUnits())
				{
					HOLD::DrawDirection(u, 50.0);
				}
			}
		}*/


		//// Iterate through all the units that we own
		//for (auto &eus : UnitDataSets[Broodwar->enemy()])
		//{
		//	for (auto &u : eus.second.m_units)
		//	{
		//		if (u->getPosition() != Positions::Unknown)
		//		{
		//			HQ->GetCommand<IntelligenceCommand>()->UpdateInfluences(u);
		//		}
		//	}
		//}


		// Iterate through all the units that we own
		//for (auto &u : Broodwar->self()->getUnits())
		//{
		//	// Ignore the unit if it no longer exists
		//	// Make sure to include this block when handling any Unit pointer!
		//	if (!u->exists())
		//		continue;

		//	// Ignore the unit if it has one of the following status ailments
		//	if (u->isLockedDown() || u->isMaelstrommed() || u->isStasised())
		//		continue;

		//	// Ignore the unit if it is in one of the following states
		//	if (u->isLoaded() || !u->isPowered() || u->isStuck())
		//		continue;

		//	// Ignore the unit if it is incomplete or busy constructing
		//	if (!u->isCompleted() || u->isConstructing())
		//		continue;


		//	//HQ->GetCommand<IntelligenceCommand>()->UpdateInfluences(u);

		//	int dx, dy;

		//	if (drawBoundaries)
		//		HOLD::drawBoundary(u);





		//	// Finally make the unit do some stuff!


		//	//todo: cooldown
		//	//if(MyUnitSets[u->getType()].cooldown < u->getType().groundWeapon().damageCooldown()
		//	


		//	//else if (BWAPI::Broodwar->getFrameCount() > 30000)
		//	//{
		//	//	int size = rechableBases.size();
		//	//	static auto tt = 0;

		//	//	if (u->getType() == UnitTypes::Zerg_Zergling
		//	//		|| u->getType() == UnitTypes::Zerg_Mutalisk
		//	//		|| u->getType() == UnitTypes::Zerg_Hydralisk)
		//	//	{
		//	//		if (!(u->getOrder() == Orders::AttackMove)
		//	//			|| !(u->getOrder() == Orders::AttackUnit))
		//	//		{
		//	//			u->issueCommand(BWAPI::UnitCommand::attack(u, Position(rechableBases[tt].Center().x * 32, rechableBases[tt].Center().y * 32)));
		//	//			++tt;
		//	//			if (tt >= size)
		//	//				tt = 0;
		//	//			//u->issueCommand(BWAPI::UnitCommand::attack(u, Position(Broodwar->getStartLocations()[i].x * 32, Broodwar->getStartLocations()[i].y * 32)));
		//	//		}
		//	//	}
		//	//					
		//	//}

		//	//			else if (u->getType().isResourceDepot()) // A resource depot is a Command Center, Nexus, or Hatchery
		//	//			{
		//	//
		//	//				// Order the depot to construct more workers! But only when it is idle.
		//	//				if (u->isIdle())
		//	//				{
		//	//					// If that fails, draw the error at the location so that you can visibly see what went wrong!
		//	//					// However, drawing the error once will only appear for a single frame
		//	//					// so create an event that keeps it on the screen for some frames
		//	//					Position pos = u->getPosition();
		//	//					Error lastErr = Broodwar->getLastError();
		//	//					Broodwar->registerEvent([pos, lastErr](Game*) { Broodwar->drawTextMap(pos, "%c%s", Text::White, lastErr.c_str()); },   // action
		//	//						nullptr,    // condition
		//	//						Broodwar->getLatencyFrames());  // frames to run
		//	//
		//	//// Retrieve the supply provider type in the case that we have run out of supplies
		//	//					UnitType supplyProviderType = u->getType().getRace().getSupplyProvider();
		//	//					static int lastChecked = 0;
		//	//
		//	//					// If we are supply blocked and haven't tried constructing more recently
		//	//					if (lastErr == Errors::Insufficient_Supply &&
		//	//						lastChecked + 400 < Broodwar->getFrameCount() &&
		//	//						Broodwar->self()->incompleteUnitCount(supplyProviderType) == 0)
		//	//					{
		//	//						lastChecked = Broodwar->getFrameCount();
		//	//
		//	//						// Retrieve a unit that is capable of constructing the supply needed
		//	//						Unit supplyBuilder = u->getClosestUnit(GetType == supplyProviderType.whatBuilds().first &&
		//	//							(IsIdle || IsGatheringMinerals) &&
		//	//							IsOwned);
		//	//						// If a unit was found
		//	//						if (supplyBuilder)
		//	//						{
		//	//							if (supplyProviderType.isBuilding())
		//	//							{
		//	//								TilePosition targetBuildLocation = Broodwar->getBuildLocation(supplyProviderType, supplyBuilder->getTilePosition());
		//	//								if (targetBuildLocation)
		//	//								{
		//	//									// Register an event that draws the target build location
		//	//									Broodwar->registerEvent([targetBuildLocation, supplyProviderType](Game*)
		//	//									{
		//	//										Broodwar->drawBoxMap(Position(targetBuildLocation),
		//	//											Position(targetBuildLocation + supplyProviderType.tileSize()),
		//	//											Colors::Blue);
		//	//									},
		//	//										nullptr,  // condition
		//	//										supplyProviderType.buildTime() + 100);  // frames to run
		//	//
		//	//				// Order the builder to construct the supply structure
		//	//									supplyBuilder->build(supplyProviderType, targetBuildLocation);
		//	//								}
		//	//							}
		//	//							else
		//	//							{
		//	//								// Train the supply provider (Overlord) if the provider is not a structure
		//	//								/*if(!MyUnitSets[supplyProviderType])
		//	//									if (supplyBuilder->train(supplyProviderType)) ++MyUnitSets[supplyProviderType].Morphing;*/
		//	//								if (!MyUnitSets[supplyProviderType])
		//	//									if (supplyBuilder->build(supplyProviderType)) ++MyUnitSets[supplyProviderType].Morphing;
		//	//							}
		//	//						} // closure: supplyBuilder is valid
		//	//					} // closure: insufficient supply
		//	//				} // closure: failed to train idle unit
		//	//
		//	//			}

		//	//todo : uncomment
		//	Unitset targets;
		//	if (u->isFlying())
		//		targets = u->getUnitsInRadius((WeaponTypes::Hellfire_Missile_Pack).maxRange() + 3 * 32/*upgrade*/ + 16, IsEnemy);
		//	else
		//		targets = u->getUnitsInRadius((WeaponTypes::Arclite_Cannon_Edmund_Duke).maxRange() + 16, IsEnemy);

		//	int closestDist = 10000000;
		//	BWAPI::Unit closestTarget = nullptr;
		//	for (auto target : targets)
		//	{
		//		//todo : I think it should return false whenn the target is invisible, doesn't it?
		//		// why hydra aimed hidden lurker?
		//		if (target->canAttack(u, false, false, false))
		//		{
		//			int dist = target->getDistance(u);
		//			if (dist < closestDist)
		//			{
		//				closestDist = dist;
		//				closestTarget = target;
		//			}
		//		}
		//	}
		//	if (closestTarget)
		//		if (closestTarget->getType() != UnitTypes::Unknown)
		//		{
		//			Position targetpos = closestTarget->getPosition();
		//			if (targetpos != Positions::Invalid
		//				&& targetpos != Positions::None
		//				&& targetpos != Positions::Unknown
		//				&& targetpos != Positions::Origin)
		//			{
		//				BWAPI::Position fleeVec(u->getPosition() - targetpos);
		//				double fleeAngle = atan2(fleeVec.y, fleeVec.x);
		//				fleeVec = BWAPI::Position(static_cast<int>(64.0 * cos(fleeAngle)), static_cast<int>(64.0 * sin(fleeAngle)));
		//				Broodwar->drawLineMap(u->getPosition(), u->getPosition() + fleeVec, Colors::Red);
		//				Broodwar->drawCircleMap(targetpos, 5, Colors::Red, true);

		//				Position forwardVec(fleeVec * -1);//targetpos - u->getPosition());
		//				Broodwar->drawLineMap(u->getPosition(), targetpos, Colors::Blue);


		//				float cooldown = static_cast<float>(u->getGroundWeaponCooldown()) / static_cast<float>(u->getType().groundWeapon().damageCooldown());

		//				if (cooldown > 0.f)
		//				{
		//					u->issueCommand(UnitCommand::move(nullptr, FindMostSafetyZone_Flee(u->getType().isFlyer() ? *HQ->GetCommand<IntelligenceCommand>()->GetGridMap("opinfluenceAir") : *HQ->GetCommand<IntelligenceCommand>()->GetGridMap("opinfluenceGround"), u->getPosition(), u->getType().isFlyer())));
		//					//u->issueCommand(UnitCommand::move(u, u->getPosition() + fleeVec));
		//				}
		//				else
		//				{
		//					//u->issueCommand(UnitCommand::move(u, u->getPosition() + fleeVec));
		//					//todo : How can I handle simple moving case?
		//					u->issueCommand(UnitCommand::move(nullptr, FindMostSafetyZone_Attack(u->getType().isFlyer() ? *HQ->GetCommand<IntelligenceCommand>()->GetGridMap("vulAir") : *HQ->GetCommand<IntelligenceCommand>()->GetGridMap("vulGround"), u->getPosition(), forwardVec, u->getType().isFlyer())));

		//					if (closestTarget->getType() != UnitTypes::Unknown)
		//						u->issueCommand(UnitCommand::rightClick(nullptr, closestTarget));
		//				}
		//			}
		//		}
		//} // closure: unit iterator
		  //if (show_visibility_data)
		  // drawVisibilityData();

		  //for (auto & u : mutalAndOver)
		  //todo : uncomment
		  //{
		  //	Unit u = nullptr;
		  //	Unit overload = nullptr;
		  //	for (auto & mo : mutalAndOver)
		  //	{
		  //		if (mo->getType() == UnitTypes::Enum::Zerg_Mutalisk)
		  //		{
		  //			u = mo;
		  //		}
		  //		else if(mo->getType() == UnitTypes::Enum::Zerg_Overlord)
		  //		{
		  //			overload = mo;
		  //		}
		  //		if(u && overload)
		  //			if (u->getType() == UnitTypes::Enum::Zerg_Mutalisk && overload->getType() == UnitTypes::Enum::Zerg_Overlord)
		  //			break;
		  //	}
		  //	if (u && overload)
		  //	if (u->getType() == UnitTypes::Enum::Zerg_Mutalisk && overload->getType() == UnitTypes::Enum::Zerg_Overlord)
		  //	{

		  //		Unitset targets;
		  //		targets = u->getUnitsInRadius((WeaponTypes::Hellfire_Missile_Pack).maxRange() + 3 * 32/*upgrade*/ + 16, IsEnemy);

		  //		int closestDist = 10000000;
		  //		BWAPI::Unit closestTarget = nullptr;
		  //		for (auto u : mutalAndOver)
		  //		{
		  //			for (auto target : targets)
		  //			{
		  //				if (target->canAttack(u, false, false, false))
		  //				{
		  //					int dist = target->getDistance(u);
		  //					if (dist < closestDist)
		  //					{
		  //						closestDist = dist;
		  //						closestTarget = target;
		  //					}
		  //				}
		  //			}
		  //		}
		  //		if (closestTarget)
		  //		{
		  //			BWAPI::Position fleeVec(u->getPosition() - closestTarget->getPosition());
		  //			double fleeAngle = atan2(fleeVec.y, fleeVec.x);
		  //			fleeVec = BWAPI::Position(static_cast<int>(64.0 * cos(fleeAngle)), static_cast<int>(64.0 * sin(fleeAngle)));
		  //			Broodwar->drawLineMap(u->getPosition(), u->getPosition() + fleeVec, Colors::Red);
		  //			Broodwar->drawCircleMap(closestTarget->getPosition(), 5, Colors::Red, true);

		  //			Position forwardVec(closestTarget->getPosition() - u->getPosition());
		  //			Broodwar->drawLineMap(u->getPosition(), u->getPosition(), Colors::Blue);

		  //			float cooldown = static_cast<float>(u->getGroundWeaponCooldown()) / static_cast<float>(u->getType().groundWeapon().damageCooldown());

		  //			//Position back = u->getPosition() + fleeVec * 2.0f;
		  //			if (cooldown > 0.8f)
		  //			{
		  //				/*Broodwar->registerEvent(
		  //					[=](Game*)
		  //				{
		  //					mutalAndOver.issueCommand(UnitCommand::move(nullptr, back));
		  //				},
		  //					[=](Game*)
		  //				{
		  //					return cooldown > 0.5f;
		  //				},
		  //					100);*/
		  //				mutalAndOver.issueCommand(UnitCommand::move(nullptr, u->getPosition() + fleeVec));
		  //				overload->issueCommand(UnitCommand::move(nullptr, overload->getPosition() + fleeVec));
		  //			}
		  //			else
		  //			{
		  //				mutalAndOver.issueCommand(UnitCommand::move(nullptr, u->getPosition() + forwardVec));
		  //				mutalAndOver.issueCommand(UnitCommand::rightClick(nullptr, closestTarget));
		  //				overload->issueCommand(UnitCommand::move(nullptr, overload->getPosition() + fleeVec));
		  //			}
		  //		}
		  //	}
		  //}
		  //for (auto & u : mutalAndOver)
		  //{
		  //	Unitset targets;
		  //	if (u->isFlying())
		  //		targets = u->getUnitsInRadius((WeaponTypes::Hellfire_Missile_Pack).maxRange() + 3 * 32/*upgrade*/ + 16, IsEnemy);
		  //	else
		  //		targets = u->getUnitsInRadius((WeaponTypes::Arclite_Cannon_Edmund_Duke).maxRange() + 16, IsEnemy);

		  //	int closestDist = 10000000;
		  //	BWAPI::Unit closestTarget = nullptr;
		  //	for (auto target : targets)
		  //	{
		  //		if (target->canAttack(u, false, false, false))
		  //		{
		  //			int dist = target->getDistance(u);
		  //			if (dist < closestDist)
		  //			{
		  //				closestDist = dist;
		  //				closestTarget = target;
		  //			}
		  //		}
		  //	}
		  //	if (closestTarget)
		  //	{
		  //		BWAPI::Position fleeVec(u->getPosition() - closestTarget->getPosition());
		  //		double fleeAngle = atan2(fleeVec.y, fleeVec.x);
		  //		fleeVec = BWAPI::Position(static_cast<int>(64.0 * cos(fleeAngle)), static_cast<int>(64.0 * sin(fleeAngle)));
		  //		Broodwar->drawLineMap(u->getPosition(), u->getPosition() + fleeVec, Colors::Red);
		  //		Broodwar->drawCircleMap(closestTarget->getPosition(), 5, Colors::Red, true);

		  //		Position forwardVec(closestTarget->getPosition() - u->getPosition());
		  //		Broodwar->drawLineMap(u->getPosition(), u->getPosition(), Colors::Blue);


		  //		if (u->getType() == UnitTypes::Enum::Zerg_Overlord)
		  //		{
		  //			u->issueCommand(UnitCommand::move(u, u->getPosition() + fleeVec));
		  //		}

		  //		float cooldown = static_cast<float>(u->getGroundWeaponCooldown()) / static_cast<float>(u->getType().groundWeapon().damageCooldown());

		  //		if (cooldown > 0.5f)
		  //		{
		  //			u->issueCommand(UnitCommand::move(u, u->getPosition() + fleeVec));
		  //			u->issueCommand(UnitCommand::move(u, u->getPosition() + fleeVec));

		  //		}
		  //		else
		  //		{
		  //			u->issueCommand(UnitCommand::move(u, u->getPosition() + forwardVec));
		  //			u->issueCommand(UnitCommand::rightClick(u, closestTarget));
		  //		}
		  //	}
		  //}

		if (Broodwar->isReplay())
			return;


		
		//if (buildorder.front().getType() == UnitCommandTypes::Unknown)
		//{
		//	if (!buildorder.empty())
		//	{
		//		++currentorder;
		//		if (buildorder.size() > 2)
		//			buildorder.pop_front();
		//		switch (buildorder.front().getType())
		//		{
		//		case UnitCommandTypes::Build:
		//		case UnitCommandTypes::Morph:
		//			reservedMineral += buildorder.front().getUnitType().mineralPrice();
		//			reservedGas += buildorder.front().getUnitType().gasPrice();
		//			reservedSupply += buildorder.front().getUnitType().supplyRequired();
		//			break;
		//		case UnitCommandTypes::Upgrade:
		//			reservedMineral += buildorder.front().getUpgradeType().mineralPrice();
		//			reservedGas += buildorder.front().getUpgradeType().gasPrice();
		//			break;
		//		case UnitCommandTypes::Research:
		//			reservedMineral += buildorder.front().getTechType().mineralPrice();
		//			reservedGas += buildorder.front().getTechType().gasPrice();
		//		}
		//	}
		//}
		//else //if(currentBO has some value)
		//{
		//	//if(currentBO.data_type != MetaType::Unknown)
		//	//	UnitCommand current = buildorder.front();
		//	if (build(buildorder.front()))
		//	{
		//		if (buildorder.front().getType() != UnitCommandTypes::Build
		//			&& buildorder.front().getType() != UnitCommandTypes::Morph)
		//		{
		//			++currentorder;
		//			if (buildorder.size() > 2)
		//			{
		//				buildorder.pop_front();
		//				switch (buildorder.front().getType())
		//				{
		//				case UnitCommandTypes::Build:
		//				case UnitCommandTypes::Morph:
		//					reservedMineral += buildorder.front().getUnitType().mineralPrice();
		//					reservedGas += buildorder.front().getUnitType().gasPrice();
		//					if (buildorder.front().getUnitType().isTwoUnitsInOneEgg())
		//						reservedSupply += buildorder.front().getUnitType().supplyRequired();
		//					reservedSupply += buildorder.front().getUnitType().supplyRequired();
		//					break;
		//				case UnitCommandTypes::Upgrade:
		//					reservedMineral += buildorder.front().getUpgradeType().mineralPrice();
		//					reservedGas += buildorder.front().getUpgradeType().gasPrice();
		//					break;
		//				case UnitCommandTypes::Research:
		//					reservedMineral += buildorder.front().getTechType().mineralPrice();
		//					reservedGas += buildorder.front().getTechType().gasPrice();
		//				}
		//			}
		//		}
		//		//reservedMineral -= current.
		//		/*if (!buildorder.empty())
		//		{
		//		++currentorder;
		//		currentBO = buildorder.front();
		//		buildorder.pop_front();
		//		reservedMineral += currentBO.mineral;
		//		reservedGas += currentBO.gas;
		//		reservedSupply += currentBO.supply;
		//		}*/
		//	}
		//}




		// until 9, train worker
		// @9, overload
		// @M200, spawningpool
		// @9, worker
		// @10, worker
		// @11, worker
		// @12, geyser
		// @M200, G200 -> spire
		// 



		current_time = Broodwar->getFrameCount();
		if (current_time - last_time > 30.0)
		{
			//frame_rate = frame_count / (current_time - last_time);
			//frame_count = 0;
			for (auto i = larvaAndOver.begin(); i != larvaAndOver.end(); ++i)
			{
				Broodwar->issueCommand(larvaAndOver, UnitCommand::stop((*i)));
			}
			for (auto &u : Broodwar->self()->getUnits().getLarva())
			{
				Broodwar->issueCommand(Broodwar->self()->getUnits().getLarva(), UnitCommand::stop(u));
			}

			last_time = current_time;
		}

		//drawStats();
		/*
		if (Broodwar->getFrameCount() % 500 == 0) {
		fout << Broodwar->self()->gatheredMinerals() << "\n";
		}
		if (Broodwar->getFrameCount() > 8005) {
		Broodwar->leaveGame();
		}
		*/


		for (auto &i : distBtwMineral)
		{
			Broodwar->drawText(CoordinateType::Map, i.first.x, i.first.y, "%g", i.second);
		}

		for (auto dl : DiagonalLineOfBase)
		{
			Broodwar->drawLineMap(dl.first, dl.second, Colors::Teal);
		}



		/*switch (buildorder.front().getType())
		{
		case UnitCommandTypes::Build:
		case UnitCommandTypes::Morph:
			Broodwar->drawTextScreen(5, 0, "current: %d - %s", currentorder, buildorder.front().getUnitType().c_str());
			Broodwar->drawTextScreen(5, 10, "Mineral : %d ", buildorder.front().getUnitType().mineralPrice());
			Broodwar->drawTextScreen(5, 20, "Gas : %d ", buildorder.front().getUnitType().gasPrice());
			Broodwar->drawTextScreen(5, 30, "Supply : %d ", buildorder.front().getUnitType().supplyRequired());
			break;
		case UnitCommandTypes::Upgrade:
			Broodwar->drawTextScreen(5, 0, "current: %d - %s", currentorder, buildorder.front().getUpgradeType().c_str());
			Broodwar->drawTextScreen(5, 10, "Mineral : %d ", buildorder.front().getUpgradeType().mineralPrice());
			Broodwar->drawTextScreen(5, 20, "Gas : %d ", buildorder.front().getUpgradeType().gasPrice());
			break;
		case UnitCommandTypes::Research:
			Broodwar->drawTextScreen(5, 0, "current: %d - %s", currentorder, buildorder.front().getTechType().c_str());
			Broodwar->drawTextScreen(5, 10, "Mineral : %d ", buildorder.front().getTechType().mineralPrice());
			Broodwar->drawTextScreen(5, 20, "Gas : %d ", buildorder.front().getTechType().gasPrice());
			break;
		}

		Broodwar->drawTextScreen(150, 10, "reservedMineal : %d ", reservedMineral);
		Broodwar->drawTextScreen(150, 20, "Gas : %d ", reservedGas);
		Broodwar->drawTextScreen(150, 30, "Supply : %d ", reservedSupply);*/





		// it looks this compiler doesn't support c++17
		//int uy = 50;
		//for (int lists : {UnitTypes::Zerg_Drone, UnitTypes::Zerg_Zergling, UnitTypes::Zerg_Hydralisk, UnitTypes::Zerg_Mutalisk})
		//{
		//	auto i = UnitDataSets[Broodwar->self()].find(lists);
		//	if (i != UnitDataSets[Broodwar->self()].end())
		//	{
		//		if (i->second.m_units.size())
		//		{
		//			Broodwar->drawTextScreen(5, uy, "%s %d", (*i->second.m_units.begin())->getType().getName().c_str(), i->second.m_units.size());
		//			uy += 10;
		//		}
		//	}
		//}

		//int by = 50;
		//for (int lists : {UnitTypes::Zerg_Hatchery, UnitTypes::Zerg_Lair, UnitTypes::Zerg_Hive, UnitTypes::Zerg_Spawning_Pool, UnitTypes::Zerg_Hydralisk_Den, UnitTypes::Zerg_Spire})
		//{
		//	auto i = UnitDataSets[Broodwar->self()].find(lists);
		//	if (i != UnitDataSets[Broodwar->self()].end())
		//	{
		//		if (i->second.m_units.size())
		//		{
		//			Broodwar->drawTextScreen(150, by, "%s %d", (*i->second.m_units.begin())->getType().getName().c_str(), i->second.m_units.size());
		//			by += 10;
		//		}
		//	}
		//}

		//int ey = 50;
		////for (auto eus : UnitDataSets[Broodwar->enemy()])
		////{
		////	auto us = UnitDataSets[Broodwar->enemy()].find(eus.first);
		////	if (us != UnitDataSets[Broodwar->enemy()].end())
		////	{
		////		if (us->second.m_savedUnits.size())
		////		{
		////			//Broodwar->drawTextScreen(280, ey, "%s %d", (*us->second.m_units.begin())->getType().getName().c_str(), us->second.m_units.size());
		////			Broodwar->drawTextScreen(280, ey, "%s %d", UnitType(eus.first).c_str(), us->second.m_savedUnits.size());
		////			ey += 10;
		////		}
		////	}
		////}

		//for (auto eus : UnitDataSets[Broodwar->enemy()])
		//{
		//	auto us = UnitDataSets[Broodwar->enemy()].find(eus.first);
		//	if (us != UnitDataSets[Broodwar->enemy()].end())
		//	{
		//		if (drawBoundaries)
		//			for (auto su : us->second.m_savedUnits)
		//			{
		//				if (su.second.m_unit != nullptr)
		//				{
		//					drawBoundary(su.second.m_lastType, su.second.m_lastPosition);
		//					Broodwar->drawTextMap(su.second.m_lastPosition, "%c%d", Text::Cyan, su.second.m_lastSeen);
		//				}
		//			}
		//		Broodwar->drawTextScreen(280, ey, "%s %d", UnitType(eus.first).c_str(), us->second.m_savedUnits.size());
		//		ey += 10;
		//	}
		//}

		/*
		auto test1 = std::hash<std::string>{}(std::string("star"));
		auto test2 = std::hash<std::string>{}(std::string("starcraft"));
		auto test3 = std::hash<std::string>{}(std::string("s"));
		auto test4 = std::hash<std::string>{}(std::string("c"));

		if(test1 == test2)
		Broodwar->drawTextScreen(300, 300, "%ctest1==test2", BWAPI::Text::Red);

		int i = 0;

		void *ref = nullptr;
		for(auto & u : Broodwar->self()->getUnits())
		{
		if(u->getClientInfo<Unit>('ref'))
		ref = u->getClientInfo<Unit>('ref');
		if (u->getClientInfo<int>('ref'))
		++i;
		}
		if(ref)
		Broodwar->drawTextScreen(400, 300, "%c%d drone:%d", BWAPI::Text::Red, static_cast<Unit>(ref)->getID(), i);

		*/


		//My Influence
		BWAPI::Position scrPos = Broodwar->getScreenPosition();

		auto DrawInfluenceMap = [](auto & scrPos, std::array<double, 256*256>& map, auto boxColor, auto txtColor)
		{
			for (int y = scrPos.y / 32; y < (scrPos.y + 14 * 32) / 32 + 1 && y < Broodwar->mapHeight(); ++y)
			{
				for (int x = scrPos.x / 32; x < (scrPos.x + 19 * 32) / 32 + 1 && x < Broodwar->mapWidth(); ++x)
				{
					if (map[y * mapHeight + x] > 0.0)
					{
						if (Grid::IsAllSame(map[y * mapHeight + x]))
						{
							Broodwar->setTextSize(Text::Size::Huge);
							Broodwar->drawTextMap(x * 32, y * 32, " %c%d", txtColor, Grid::GetLeftTop(map[y * mapHeight + x]));
							Broodwar->drawBoxMap(x * 32, y * 32, x * 32 + 32, y * 32 + 32, boxColor);
						}
						else
						{
							Broodwar->setTextSize(Text::Size::Small);

							if (Grid::GetLeftTop(map[y * mapHeight + x]) != 0)
							{
								Broodwar->drawTextMap(x * 32, y * 32, " %c%d", txtColor, Grid::GetLeftTop(map[y * mapHeight + x]));
								Broodwar->drawBoxMap(x * 32, y * 32, x * 32 + 16, y * 32 + 16, boxColor);
							}

							if (Grid::GetRightTop(map[y * mapHeight + x]) != 0)
							{
								Broodwar->drawTextMap(x * 32 + 16, y * 32, " %c%d", txtColor, Grid::GetRightTop(map[y * mapHeight + x]));
								Broodwar->drawBoxMap(x * 32 + 16, y * 32, x * 32 + 32, y * 32 + 16, boxColor);
							}

							if (Grid::GetLeftBot(map[y * mapHeight + x]) != 0)
							{
								Broodwar->drawTextMap(x * 32, y * 32 + 16, " %c%d", txtColor, Grid::GetLeftBot(map[y * mapHeight + x]));
								Broodwar->drawBoxMap(x * 32, y * 32 + 16, x * 32 + 16, y * 32 + 32, boxColor);
							}

							if (Grid::GetRightBot(map[y * mapHeight + x]) != 0)
							{
								Broodwar->drawTextMap(x * 32 + 16, y * 32 + 16, " %c%d", txtColor, Grid::GetRightBot(map[y * mapHeight + x]));
								Broodwar->drawBoxMap(x * 32 + 16, y * 32 + 16, x * 32 + 32, y * 32 + 32, boxColor);
							}
						}
					}
				}
			}
			Broodwar->setTextSize(Text::Size::Small);
		};


		/*
		* // Green
		* Q : my influence (Ground)
		* A : my influence (Air)
		*
		* // Red
		* W : enemy influence (Ground)
		* S : enemy influence (Air)
		*
		* // Purple
		* E : tension map (Total)
		* D : tension map (Ground)
		* C : tension map (Air)
		*
		* // WHITE
		* R : vulnerability map (Total)
		* F : vulnerability map (Ground)
		* V : vulnerability map (Air)
		*/

		// my influence - green
		/*
		if (Broodwar->getKeyState(Key::K_Q))
		DrawInfluenMap(scrPos, myinfluenceGround, Colors::Green, Text::Green);

		if (Broodwar->getKeyState(Key::K_A))
		DrawInfluenMap(scrPos, myinfluenceAir, Colors::Green, Text::Green);
		*/
		// my influence - green

		if (Broodwar->getKeyState(Key::K_Q))
			DrawInfluenceMap(scrPos, *HQ->GetCommand<IntelligenceCommand>()->GetGridMap(std::string{ "influenceGround" }), Colors::Green, Text::Green);

		if (Broodwar->getKeyState(Key::K_A))
			DrawInfluenceMap(scrPos, *HQ->GetCommand<IntelligenceCommand>()->GetGridMap(std::string{ "influenceAir" }), Colors::Green, Text::Green);

		// enemy influence - red
		if (Broodwar->getKeyState(Key::K_W))
			DrawInfluenceMap(scrPos, *HQ->GetCommand<IntelligenceCommand>()->GetGridMap(std::string{ "opinfluenceGround" }), Colors::Red, Text::Red);

		if (Broodwar->getKeyState(Key::K_S))
			DrawInfluenceMap(scrPos, *HQ->GetCommand<IntelligenceCommand>()->GetGridMap(std::string{ "opinfluenceAir" }), Colors::Red, Text::Red);

		// tension map - purple
		if (Broodwar->getKeyState(Key::K_E))
			DrawInfluenceMap(scrPos, *HQ->GetCommand<IntelligenceCommand>()->GetGridMap(std::string{ "tensionTotal" }), Colors::Purple, Text::Purple);

		if (Broodwar->getKeyState(Key::K_D))
			DrawInfluenceMap(scrPos, *HQ->GetCommand<IntelligenceCommand>()->GetGridMap(std::string{ "tensionGround" }), Colors::Purple, Text::Purple);

		if (Broodwar->getKeyState(Key::K_C))
			DrawInfluenceMap(scrPos, *HQ->GetCommand<IntelligenceCommand>()->GetGridMap(std::string{ "tensionAir" }), Colors::Purple, Text::Purple);

		// vulnerability map - white
		if (Broodwar->getKeyState(Key::K_R))
			DrawInfluenceMap(scrPos, *HQ->GetCommand<IntelligenceCommand>()->GetGridMap(std::string{ "vulTotal" }), Colors::White, Text::White);

		if (Broodwar->getKeyState(Key::K_F))
			DrawInfluenceMap(scrPos, *HQ->GetCommand<IntelligenceCommand>()->GetGridMap(std::string{ "vulGround" }), Colors::White, Text::White);

		if (Broodwar->getKeyState(Key::K_V))
			DrawInfluenceMap(scrPos, *HQ->GetCommand<IntelligenceCommand>()->GetGridMap(std::string{ "vulAir" }), Colors::White, Text::White);

		//if (Broodwar->getKeyState(Key::K_Q))
		//	DrawInfluenceMap(scrPos, HQ->GetCommand<IntelligenceCommand>()->influenceGround, Colors::Green, Text::Green);

		//if (Broodwar->getKeyState(Key::K_A))
		//	DrawInfluenceMap(scrPos, HQ->GetCommand<IntelligenceCommand>()->influenceAir, Colors::Green, Text::Green);

		//// enemy influence - red
		//if (Broodwar->getKeyState(Key::K_W))
		//	DrawInfluenceMap(scrPos, HQ->GetCommand<IntelligenceCommand>()->opinfluenceGround, Colors::Red, Text::Red);

		//if (Broodwar->getKeyState(Key::K_S))
		//	DrawInfluenceMap(scrPos, HQ->GetCommand<IntelligenceCommand>()->opinfluenceAir, Colors::Red, Text::Red);

		//// tension map - purple
		//if (Broodwar->getKeyState(Key::K_E))
		//	DrawInfluenceMap(scrPos, HQ->GetCommand<IntelligenceCommand>()->tensionTotal, Colors::Purple, Text::Purple);

		//if (Broodwar->getKeyState(Key::K_D))
		//	DrawInfluenceMap(scrPos, HQ->GetCommand<IntelligenceCommand>()->tensionGround, Colors::Purple, Text::Purple);

		//if (Broodwar->getKeyState(Key::K_C))
		//	DrawInfluenceMap(scrPos, HQ->GetCommand<IntelligenceCommand>()->tensionAir, Colors::Purple, Text::Purple);

		//// vulnerability map - white
		//if (Broodwar->getKeyState(Key::K_R))
		//	DrawInfluenceMap(scrPos, HQ->GetCommand<IntelligenceCommand>()->vulTotal, Colors::White, Text::White);

		//if (Broodwar->getKeyState(Key::K_F))
		//	DrawInfluenceMap(scrPos, HQ->GetCommand<IntelligenceCommand>()->vulGround, Colors::White, Text::White);

		//if (Broodwar->getKeyState(Key::K_V))
		//	DrawInfluenceMap(scrPos, HQ->GetCommand<IntelligenceCommand>()->vulAir, Colors::White, Text::White);


		/*Broodwar->setTextSize(Text::Size::Small);
		Broodwar->drawTextScreen(64, 288, "%c(%u, %u)", Text::White, (scrPos.x + Broodwar->getMousePosition().x) / 32, (scrPos.y + Broodwar->getMousePosition().y) / 32);
		Broodwar->setTextSize();

		Broodwar->drawTextScreen(64, 268, "%c(%u * %u)", Text::White, Broodwar->mapWidth(), Broodwar->mapHeight());*/

		//for (auto u : Broodwar->getAllUnits())
		//{
		//	Broodwar->setTextSize(Text::Size::Huge);
		//	//Broodwar->drawTextMap(u->getPosition(), "%d", u->getID(), HQ->GetCommand<IntelligenceCommand>()->);
		//	Broodwar->setTextSize(Text::Size::Small);
		//}

		//auto selectedUnit = Broodwar->getSelectedUnits();
		//if(selectedUnit.size() == 1)
		//{
		//	const Unit* u = &(*selectedUnit.begin());
		//	for(int x = 0; x < 460; ++x)
		//	{
		//		Broodwar->drawTextScreen(x, 400, "%d", (*u)->getID());
		//	}
		//}

		
		//auto elapsedTime = (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_StartTime)).count();
		//auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_StartTime).count();
		//std::chrono::duration<double, std::milli> elapsedTime = std::chrono::high_resolution_clock::now() - m_StartTime;
		//Broodwar->drawTextScreen(100, 100, "%f", elapsedTime.count());

		Broodwar->drawTextScreen(100, 100, "%d", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_StartTime).count());


		/*
		 * More than 1 frame longer than 10 seconds, or
		 * more than 10 frames longer than 1 second, or
		 * more than 320 frames longer than 85 milliseconds.
		 */


	}
	catch (const std::exception & e)
	{
		Broodwar << "EXCEPTION from " << __func__ << ": " << e.what() << std::endl;
	}
}

void HoldAIModule::onSendText(std::string text)
{
	char* endptr = nullptr;
	const char *input_ptr = text.c_str();
	long value = std::strtol(input_ptr, &endptr, 10);

	if (endptr == input_ptr)
	{
		// Input was not a valid number
	}
	else if (*endptr != '\0')
	{
		// Input starts with a valid number, but ends with some extra characters
		// (for example "123abc")
		// `value` is set to the numeric part of the string
	}
	else
	{
		// Input was a valid number
		//Broodwar->sendText("%s", text.c_str());
		BWAPI::Broodwar->setLocalSpeed(value);
	}


	BWEM::utils::MapDrawer::ProcessCommand(text);




	// Make sure to use %s and pass the text as a parameter,
	// otherwise you may run into problems when you use the %(percent) character!

	if (text == "/analyze")
	{
		if (analyzed == false)
		{
			Broodwar->printf("Analyzing map... this may take a minute");
			//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL);
		}
	}
	else if ("range" == text)
	{
		drawRange = !drawRange;

	}
	else if ("boundary" == text)
	{
		drawBoundaries = !drawBoundaries;

	}
	else if ("dir" == text)
	{
		drawDirection = !drawDirection;

	}
	else if ("grid" == text)
	{
		drawGridMap = !drawGridMap;
	}
	else if ("all" == text)
	{
		drawAll = !drawAll;
		drawRange = drawBoundaries = drawDirection = drawGridMap = drawAll;
	}
	else if ("cheat" == text)
	{
		Broodwar->sendText("power overwhelming");
		Broodwar->sendText("operation cwal");
		Broodwar->sendText("the gathering");
		Broodwar->sendText("modify the phase variance");
		Broodwar->sendText("food for thought");
		Broodwar->sendText("war aint what it used to be");
		Broodwar->sendText("staying alive");
		Broodwar->sendText("show me the money");
		Broodwar->sendText("show me the money");
		Broodwar->sendText("show me the money");
	}
	else
	{
		//else
		//{
		// if (text == "f") {
		//  BWAPI::Broodwar->setLocalSpeed(0);
		// }
		// if (text == "m") {
		//  BWAPI::Broodwar->setLocalSpeed(20);
		// }
		// if (text == "s") {
		//  BWAPI::Broodwar->setLocalSpeed(40);
		// }
		// 
		// //Broodwar->printf("You typed '%s'!",text.c_str());
		// //Broodwar->sendText("%s",text.c_str());
		//}

		// Send the text to the game if it is not being processed.
		Broodwar->sendText("%s", text.c_str());
	}

}

void HoldAIModule::onReceiveText(BWAPI::Player player, std::string text)
{
	// Parse the received text
	Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void HoldAIModule::onPlayerLeft(BWAPI::Player player)
{
	// Interact verbally with the other players in the game by
	// announcing that the other player has left.
	//Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void HoldAIModule::onNukeDetect(BWAPI::Position target)
{

	// Check if the target is a valid position
	if (target)
	{
		// if so, print the location of the nuclear strike target
		Broodwar << "Nuclear Launch Detected at " << target << std::endl;
	}
	else
	{
		// Otherwise, ask other players where the nuke is!
		Broodwar->sendText("Where's the nuke?");
	}

	// You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

void HoldAIModule::onUnitDiscover(BWAPI::Unit unit)
{
	//void, use show fn
	//BWAPI::Broodwar->registerEvent([=](BWAPI::Game*)
	//{
	//	//Broodwar->drawTextMap(unit->getPosition(), "Event:%d", i);
	//	if(unit)
	//	Broodwar->drawCircleMap(unit->getPosition(), 3, Colors::Teal, true);
	//},
	//	[=](BWAPI::Game*) {return true; },  // condition
	//	50);  // frames to run
}

void HoldAIModule::onUnitEvade(BWAPI::Unit unit)
{
	//void, use hide fn
	//BWAPI::Broodwar->registerEvent([=](BWAPI::Game*)
	//{
	//	//Broodwar->drawTextMap(unit->getPosition(), "Event:%d", i);
	//	if(unit)
	//	Broodwar->drawCircleMap(unit->getPosition(), 3, Colors::Red, true);
	//},
	//	[=](BWAPI::Game*) {return true; },  // condition
	//	50);  // frames to run
	//HQ->GetCommand<IntelligenceCommand>()->Units[unit->getPlayer()][unit->getType()].find(unit->getID()).m_lastPosition = unit->getPosition();
}

void HoldAIModule::onUnitShow(BWAPI::Unit unit)
{
	HQ->GetCommand<IntelligenceCommand>()->OnUnitShow(unit);
	//BWAPI::Broodwar->registerEvent([=](BWAPI::Game*)
	//{
	//	//Broodwar->drawTextMap(unit->getPosition(), "Event:%d", i);
	//	if (unit)
	//		Broodwar->drawCircleMap(unit->getPosition(), 10, Colors::Teal, false);
	//},
	//	[=](BWAPI::Game*) {return true; },  // condition
	//	50);  // frames to run

	//// if it's tournament, then try to use enemies
	//if (unit->getPlayer() == Broodwar->enemy())
	//{
	//	//since it's not the discover function, we should check whether it is in container or new
	//	UnitType unitType = unit->getType();

	//	if (unitType.isResourceDepot())
	//	{
	//		foundEnemyBase = true;
	//		enemyBase.emplace_back(unit->getTilePosition());
	//	}

	//	//if (unitType.isBuilding())
	//	{

	//		//UnitDataSets[unit->getPlayer()][unitType].AddUnit(HOLD::UnitData(unit));

	//		//if the container doesn't exist, it's new, just add it
	//		if (UnitDataSets[unit->getPlayer()].find(unitType) != UnitDataSets[unit->getPlayer()].end())
	//		{
	//			//if the container exist, check this is new or old
	//			if (UnitDataSets[unit->getPlayer()][unitType].m_savedUnits.find(unit->getID()) != UnitDataSets[unit->getPlayer()][unitType].m_savedUnits.end())
	//			{
	//				UnitDataSets[unit->getPlayer()][unitType].UpdateUnit(unit);

	//			}
	//			else
	//			{
	//				UnitDataSets[unit->getPlayer()][unitType].AddUnit(unit);
	//			}


	//		}
	//		else
	//		{
	//			UnitDataSets[unit->getPlayer()][unitType].AddUnit(unit);
	//		}
	//	}
	//	//if (UnitDataSets[unit->getPlayer()][unitType].m_savedUnits[unit->getID()].m_lastPlayer == nullptr)
	//	//{
	//	//	UnitDataSets[unit->getPlayer()][unitType].AddUnit(unit);
	//	//}



	//	//if(UnitDataSets[unit->getPlayer()][unitType].m_savedUnits[unit->getID()].m_lastPlayer != unit->getPlayer())
	//	//{
	//	//	UnitDataSets[UnitDataSets[unit->getPlayer()][unitType].m_savedUnits[unit->getID()].m_lastPlayer][unitType].RemoveUnit(unit);
	//	//	UnitDataSets[unit->getPlayer()][unitType].AddUnit(unit);// .m_savedUnits[unit->getID()]..m_lastPlayer. = unit->getPlayer()
	//	//}
	//	//else
	//	//{
	//	//	UnitDataSets[unit->getPlayer()][unitType].UpdateUnit(unit);
	//	//}

	//	/*	UnitDataSets[unit->getPlayer()][unit->getType()].m_savedUnits[unit->getID()].m_lastPosition = unit->getPosition();
	//	UnitDataSets[unit->getPlayer()][unit->getType()].m_savedUnits[unit->getID()].m_lastSeen = Broodwar->getFrameCount();
	//	UnitDataSets[unit->getPlayer()][unit->getType()].m_savedUnits[unit->getID()].m_lastType = unit->getType();*/
	//}
	//else
	{
		auto FindDirection = [](auto & b)
		{
			
		};
		//todo : check the direction to resources from the base
		auto AddDiagonalLineOfBase = [](auto & b)
		{
			DiagonalLineOfBase.emplace_back(std::make_pair<Position, Position>(Position{ b->getLeft(), b->getTop() }, Position{ b->getLeft() - 100, b->getTop() - 100 }));
			DiagonalLineOfBase.emplace_back(std::make_pair<Position, Position>(Position{ b->getRight(), b->getTop() }, Position{ b->getRight() + 100, b->getTop() - 100 }));
			DiagonalLineOfBase.emplace_back(std::make_pair<Position, Position>(Position{ b->getLeft(), b->getBottom() }, Position{ b->getLeft() - 100, b->getBottom() + 100 }));
			DiagonalLineOfBase.emplace_back(std::make_pair<Position, Position>(Position{ b->getRight(), b->getBottom() }, Position{ b->getRight() + 100, b->getBottom() + 100 }));

			/*Broodwar->drawLineMap(b->getLeft(), b->getTop(), b->getLeft() - 100, b->getTop() - 100, Colors::Teal);
			Broodwar->drawLineMap(b->getRight(), b->getTop(), b->getRight() + 100, b->getTop() - 100, Colors::Teal);
			Broodwar->drawLineMap(b->getLeft(), b->getBottom(), b->getLeft() - 100, b->getBottom() + 100, Colors::Teal);
			Broodwar->drawLineMap(b->getRight(), b->getBottom(), b->getRight() + 100, b->getBottom() + 100, Colors::Teal);*/
		};

		if (unit->getType().isResourceDepot())
			AddDiagonalLineOfBase(unit);

		if (unit->getType() == UnitTypes::Resource_Mineral_Field
			|| unit->getType() == UnitTypes::Resource_Mineral_Field_Type_2
			|| unit->getType() == UnitTypes::Resource_Mineral_Field_Type_3)
			mineralpos.emplace_back(unit->getPosition());
	}
}

//todo : the thing I want is exatly what onUnitEvade is doing, so if I want that, I have to implement that function my own way.
//otherwise, this will be used to catch zerg' burrow, spider mine, whatever...
void HoldAIModule::onUnitHide(BWAPI::Unit unit)
{
	int i = Broodwar->getFrameCount();
	//BWAPI::Broodwar->registerEvent([=](BWAPI::Game*)
	//{
	//	//Broodwar->drawTextMap(unit->getPosition(), "Event:%d", i);
	//	Broodwar->drawCircleMap(unit->getPosition(), 3, Colors::Brown, true);
	//},
	//	[=](BWAPI::Game*) {return true; },  // condition
	//	10);  // frames to run
			  //todo : set unit's hide position
	UnitType unitType = unit->getType();

	//HQ->GetCommand<IntelligenceCommand>()->Units[unit->getPlayer()][unitType].find(unit->getID()).m_lastPosition = unit->getPosition();

	

	/*EnemyUnitSets[unitType.c_str()]._realUnits[unit->getID()]._lastPosition = unit->getPosition();
	EnemyUnitSets[unitType.c_str()]._realUnits[unit->getID()]._lastSeen = Broodwar->getFrameCount();*/


	/*UnitDataSets[unit->getPlayer()][unit->getType()].m_savedUnits[unit->getID()].m_lastPlayer = unit->getPlayer();
	UnitDataSets[unit->getPlayer()][unit->getType()].m_savedUnits[unit->getID()].m_lastPosition = unit->getPosition();
	UnitDataSets[unit->getPlayer()][unit->getType()].m_savedUnits[unit->getID()].m_lastSeen = Broodwar->getFrameCount();
	UnitDataSets[unit->getPlayer()][unit->getType()].m_savedUnits[unit->getID()].m_lastType = unit->getType();*/
	//(*HQ->GetCommand<IntelligenceCommand>()->GetUnitDataSets())[unit->getPlayer()][unitType].UpdateUnit(unit);
}

void HoldAIModule::onUnitCreate(BWAPI::Unit unit)
{
	if (Broodwar->isReplay())
	{
		// if we are in a replay, then we will print out the build order of the structures
		if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
		{
			int seconds = Broodwar->getFrameCount() / 24;
			int minutes = seconds / 60;
			seconds %= 60;
			Broodwar->sendText("%.2d:%.2d: %s creates a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
		}
	}
	else
	{
		//if (unit->getPlayer() == Broodwar->self())
		//{
		//	UnitType unitType = unit->getType();

		//	//todo : uncomment these lines
		//	if (unitType == BWAPI::UnitTypes::Enum::Enum::Zerg_Larva)
		//		UnitDataSets[unit->getPlayer()][unitType].m_units.emplace(unit);

		//	else if (unitType == BWAPI::UnitTypes::Zerg_Zergling)
		//		UnitDataSets[unit->getPlayer()][unitType].m_units.emplace(unit);

		//	else if (unitType == BWAPI::UnitTypes::Zerg_Scourge)
		//		UnitDataSets[unit->getPlayer()][unitType].m_units.emplace(unit);

		//	if (UnitTypes::Zerg_Egg == unitType)
		//		return;

		//}
		HQ->GetCommand<IntelligenceCommand>()->OnUnitCreate(unit);
		//// todo: enemy stuff; zerg_egg,

		//Broodwar->registerEvent([=](Game*)
		//{
		//	Broodwar->drawTextScreen(410, 50, "onUnitCreate : %s", unit->getType().getName().c_str());
		//},
		//	nullptr,  // condition
		//	50);  // frames to run
	}
}


void HoldAIModule::onUnitDestroy(BWAPI::Unit unit)
{
	try
	{
		UnitType unitType = unit->getType();
		/*if (unit->getType().isMineralField()) theMap.OnMineralDestroyed(unit);
		else if (unit->getType().isSpecialBuilding()) theMap.OnStaticBuildingDestroyed(unit);*/

		HQ->GetCommand<IntelligenceCommand>()->OnUnitDestroy(unit);

		/*Unitset tobedeleted;
		tobedeleted.insert(unit);
		std::remove(Broodwar->self()->getUnits().begin(), Broodwar->self()->getUnits().end(), tobedeleted);*/

		/*Unitset mine = Broodwar->self()->getUnits();
		mine.erase_if(Filter);*/
		//if (unit->getPlayer() == Broodwar->self())
		//{
		//	UnitDataSets[Broodwar->self()][unitType].m_units.erase(unit);
		//}

		//// if it's tournament, then try to use enemies
		//else if (unit->getPlayer() == Broodwar->enemy())
		//{
		//	EnemyUnitSets[unitType.c_str()].m_units.erase(unit);
		//	if (EnemyUnitSets[unitType.c_str()].m_units.empty())
		//	{
		//		for (auto it = std::begin(EnemyUnitSets); it != std::end(EnemyUnitSets);)
		//		{
		//			it = EnemyUnitSets.erase(it);
		//			break;
		//		}
		//	}
		//}
		//UnitDataSets[unit->getPlayer()][unitType].RemoveUnit(unit);

		/*if (unitType.isResourceDepot() && unit->getPlayer() != Broodwar->self())
			enemyBase.erase(std::find(enemyBase.begin(), enemyBase.end(), unit->getTilePosition()));*/

		//todo : the unit must removed from the unit container
		//Broodwar->registerEvent([=](Game*)
		//{
		//	Broodwar->drawTextScreen(410, 60, "onUnitDestroy : %c%s m:%d g:%d", BWAPI::Text::Red, unitType.getName().c_str(), unitType.mineralPrice(), unitType.gasPrice());
		//},
		//	nullptr,  // condition
		//	100);  // frames to run
	}
	catch (const std::exception & e)
	{
		Broodwar << "EXCEPTION from " << __func__ << ": " << e.what() << std::endl;
	}
}

void HoldAIModule::onUnitMorph(BWAPI::Unit unit)
{
	try
	{
		HQ->OnUnitMorph(unit);
		UnitType unitType = unit->getType();



		//if (!buildorder.empty())
		//{
		//	if (unit->getBuildType() == buildorder.front().getUnitType())
		//	{
		//		//if(currentBO.getUnitType().isBuilding())
		//		// this will be faster
		//		if (buildorder.front().getUnitType() > 62)
		//		{
		//			UnitDataSets[Broodwar->self()][UnitTypes::Enum::Zerg_Drone].m_units.erase(unit);
		//			reservedMineral -= unitType.mineralPrice();
		//			reservedGas -= unitType.gasPrice();
		//			//reservedSupply -= unitType.supplyRequired();
		//		}
		//		else
		//		{
		//			reservedMineral -= unit->getBuildType().mineralPrice();
		//			reservedGas -= unit->getBuildType().gasPrice();
		//			if (unitType.isTwoUnitsInOneEgg())
		//				reservedSupply -= unit->getBuildType().supplyRequired();
		//			reservedSupply -= unit->getBuildType().supplyRequired();
		//		}
		//		++currentorder;

		//		if (buildorder.size() > 2)
		//			buildorder.pop_front();
		//		switch (buildorder.front().getType())
		//		{
		//		case UnitCommandTypes::Build:
		//		case UnitCommandTypes::Morph:
		//			reservedMineral += buildorder.front().getUnitType().mineralPrice();
		//			reservedGas += buildorder.front().getUnitType().gasPrice();
		//			reservedSupply += buildorder.front().getUnitType().supplyRequired();
		//			break;
		//		case UnitCommandTypes::Upgrade:
		//			reservedMineral += buildorder.front().getUpgradeType().mineralPrice();
		//			reservedGas += buildorder.front().getUpgradeType().gasPrice();
		//			break;
		//		case UnitCommandTypes::Research:
		//			reservedMineral += buildorder.front().getTechType().mineralPrice();
		//			reservedGas += buildorder.front().getTechType().gasPrice();
		//		}
		//	}
		//}


		//UnitDataSets[unit->getPlayer()][unitType.whatBuilds().first].RemoveUnit(unit);

		//UnitDataSets[unit->getPlayer()][unitType].AddUnit(unit);

		if (!Broodwar->isReplay())
		{
			// does this affect frame drop?
			// these eggs cost (1,1)
			//if (unitType != UnitTypes::Zerg_Egg
			//	|| unitType != UnitTypes::Zerg_Cocoon
			//	|| unitType != UnitTypes::Zerg_Lurker_Egg)
			//{
			//	//todo: when the cancle lurker_egg -> this function will be called for the hydra
			//	reservedMineral -= unit->getType().mineralPrice();
			//	reservedGas -= unit->getType().gasPrice();
			//}


			//Broodwar->drawTextScreen(300, 300, "%s", unit->getType().getName().c_str());

			// Register an event that draws the target build location
			//Broodwar->registerEvent([targetBuildLocation, supplyProviderType](Game*)
			//{
			//	Broodwar->drawBoxMap(Position(targetBuildLocation),
			//		Position(targetBuildLocation + supplyProviderType.tileSize()),
			//		Colors::Blue);
			//},
			//	nullptr,  // condition
			//	supplyProviderType.buildTime() + 100);  // frames to run

			//if (UnitTypes::Zerg_Egg == unitType)
			//	return;

			//if (!unitType.isBuilding())
			//	return;

			//Broodwar->registerEvent([=](Game*)
			//{
			//	Broodwar->drawTextScreen(410, 70, "onUnitMorph %s m:%d g:%d", unitType.getName().c_str(), unitType.mineralPrice(), unitType.gasPrice());
			//},
			//	nullptr,  // condition
			//	150);  // frames to run
		}
		else //if (Broodwar->isReplay())
		{
			// if we are in a replay, then we will print out the build order of the structures
			if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
			{
				int seconds = Broodwar->getFrameCount() / 24;
				int minutes = seconds / 60;
				seconds %= 60;
				Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
			}
		}
	}
	catch (const std::exception & e)
	{
		Broodwar << "EXCEPTION from " << __func__ << ": " << e.what() << std::endl;
	}
}

void HoldAIModule::onUnitRenegade(BWAPI::Unit unit)
{
}

void HoldAIModule::onSaveGame(std::string gameName)
{
	Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void HoldAIModule::onUnitComplete(BWAPI::Unit unit)
{
	UnitType unitType = unit->getType();

	if (UnitTypes::Zerg_Larva == unitType)
		return;

	//Broodwar->registerEvent([=](Game*)
	//{
	//	Broodwar->drawTextScreen(410, 80, "onUnitComplete %s m:%d g:%d", unitType.getName().c_str(), unitType.mineralPrice(), unitType.gasPrice());
	//},
	//	nullptr,  // condition
	//	50);  // frames to run

	if (unit->getType().isWorker())
	{
		if (!unit->gather(unit->getClosestUnit(IsMineralField)))
		{
			// If the call fails, then print the last error message
			Broodwar << Broodwar->getLastError() << std::endl;
		}
	}
	else if (unit->getType().isRefinery())
	{
		auto work = std::hash<std::string>{}((std::string(std::to_string(unit->getID())) + std::string("_work")));

		//static size_t nWorkersAssigned = unit->getClientInfo<size_t>(work);
		HOLD::registerEvent(
			[=](BWAPI::Game*)
		{
			size_t nWorkersAssigned = unit->getClientInfo<size_t>(work);
			if (nWorkersAssigned < 3)
			{
				Unit pClosestIdleWorker = unit->getClosestUnit(IsWorker && !IsGatheringGas);//&& !IsCarryingSomething && !IsGatheringGas && IsMoving);
				if (pClosestIdleWorker)
				{
					// gather from the refinery (and check if successful)
					if (pClosestIdleWorker->gather(unit))
					{
						// set a back reference for when the unit is killed or re-assigned (code not provided)
						//pClosestIdleWorker->setClientInfo(unit, 'ref');
						pClosestIdleWorker->setClientInfo(unit, 'ref');
						// Increment the number of workers assigned and associate it with the refinery
						++nWorkersAssigned;
						//unit->setClientInfo(unit->getClientInfo<int>('work'), 'work');
						unit->setClientInfo(nWorkersAssigned, work);
					}
				}
			}
		},
			//[=](BWAPI::Game*) {return unit->getClientInfo<int>('work') < 3; },  // condition
			[=](BWAPI::Game*)
		{
			return unit->getClientInfo<size_t>(work) < 3;
		},  // condition
			120,//60,// frames to run
			30);  // frames to check

				  //Broodwar->registerEvent([=](Game*)
				  //{
				  //	Broodwar->drawTextScreen(410, 200, "info : %d", unit->getClientInfo<int>('work'));
				  //	//Broodwar->drawTextScreen(400, 210, "info : %d", nWorkersAssigned);
				  //},
				  //	nullptr,  // condition
				  //	1000);  // frames to run

	}
	auto FindDirection = [](auto & b)
	{

	};
	//todo : check the direction to resources from the base
	auto AddDiagonalLineOfBase = [](auto & b)
	{
		DiagonalLineOfBase.emplace_back(std::make_pair<Position, Position>(Position{ b->getLeft(), b->getTop() }, Position{ b->getLeft() - 100, b->getTop() - 100 }));
		DiagonalLineOfBase.emplace_back(std::make_pair<Position, Position>(Position{ b->getRight(), b->getTop() }, Position{ b->getRight() + 100, b->getTop() - 100 }));
		DiagonalLineOfBase.emplace_back(std::make_pair<Position, Position>(Position{ b->getLeft(), b->getBottom() }, Position{ b->getLeft() - 100, b->getBottom() + 100 }));
		DiagonalLineOfBase.emplace_back(std::make_pair<Position, Position>(Position{ b->getRight(), b->getBottom() }, Position{ b->getRight() + 100, b->getBottom() + 100 }));

		/*Broodwar->drawLineMap(b->getLeft(), b->getTop(), b->getLeft() - 100, b->getTop() - 100, Colors::Teal);
		Broodwar->drawLineMap(b->getRight(), b->getTop(), b->getRight() + 100, b->getTop() - 100, Colors::Teal);
		Broodwar->drawLineMap(b->getLeft(), b->getBottom(), b->getLeft() - 100, b->getBottom() + 100, Colors::Teal);
		Broodwar->drawLineMap(b->getRight(), b->getBottom(), b->getRight() + 100, b->getBottom() + 100, Colors::Teal);*/
	};

	if (unit->getType().isResourceDepot())
		AddDiagonalLineOfBase(unit);
}

//todo : debug draw and optimizing
//
//#include <iostream>
//#include <cassert>
//#include <thread>
//#include <chrono>
//#include <string>
//
//#include <Windows.h>
//
//#include <BWAPI/Client.h>
//#include <BWAPI.h>
//
//
//using namespace BWAPI;
//
//void reconnect()
//{
//	while (!BWAPIClient.connect())
//		std::this_thread::sleep_for(std::chrono::milliseconds{ 1000 });
//}
//
//int main(int argc, const char* argv[])
//{
//	std::string dllPath;
//
//	if (argc >= 2)
//	{
//		dllPath = argv[1];
//	}
//	else
//	{
//		std::cout << "Enter path name to AI DLL: " << std::endl;
//		std::getline(std::cin, dllPath);
//	}
//
//	std::cout << "Connecting..." << std::endl;
//
//	assert(BWAPIClient.isConnected() == false);
//	reconnect();
//	assert(BroodwarPtr != nullptr);
//
//	while (true)
//	{
//		std::cout << "waiting to enter match" << std::endl;
//		while (!Broodwar->isInGame())
//		{
//			//std::cout << "attempting update" << std::endl;
//			BWAPI::BWAPIClient.update();
//			if (!BWAPI::BWAPIClient.isConnected())
//			{
//				std::cout << "Reconnecting..." << std::endl;
//				reconnect();
//			}
//		}
//		std::cout << "entered match" << std::endl;
//
//		AIModule* client = NULL;
//		HMODULE hMod = LoadLibraryA(dllPath.c_str());
//		if (hMod == NULL)
//		{
//			std::cerr << "ERROR: Failed to load the AI Module" << std::endl;
//			client = new AIModule();
//			Broodwar->sendText("Error: Failed to load the AI Module");
//		}
//		else
//		{
//			typedef AIModule* (*PFNCreateA1)();
//			typedef void(*PFNGameInit)(Game *);
//
//			PFNGameInit newGame = (PFNGameInit)GetProcAddress(hMod, "gameInit");
//			PFNCreateA1 newAIModule = (PFNCreateA1)GetProcAddress(hMod, "newAIModule");
//
//			if (!newGame || !newAIModule)
//			{
//				std::cerr << "ERROR: Failed to find AI Module exports" << std::endl;
//				client = new AIModule();
//				Broodwar->sendText("Error: Failed to find AI Module exports");
//			}
//			else
//			{
//				newGame(BroodwarPtr);
//				client = newAIModule();
//			}
//		}
//		//std::cout << "starting match!" << std::endl;
//		while (Broodwar->isInGame())
//		{
//			for (std::list<Event>::const_iterator e = Broodwar->getEvents().begin(); e != Broodwar->getEvents().end(); ++e)
//			{
//				EventType::Enum et = e->getType();
//				switch (et)
//				{
//				case EventType::MatchStart:
//					client->onStart();
//					break;
//				case EventType::MatchEnd:
//					client->onEnd(e->isWinner());
//					break;
//				case EventType::MatchFrame:
//					client->onFrame();
//					break;
//				case EventType::MenuFrame:
//					break;
//				case EventType::SendText:
//					client->onSendText(e->getText());
//					break;
//				case EventType::ReceiveText:
//					client->onReceiveText(e->getPlayer(), e->getText());
//					break;
//				case EventType::PlayerLeft:
//					client->onPlayerLeft(e->getPlayer());
//					break;
//				case EventType::NukeDetect:
//					client->onNukeDetect(e->getPosition());
//					break;
//				case EventType::UnitDiscover:
//					client->onUnitDiscover(e->getUnit());
//					break;
//				case EventType::UnitEvade:
//					client->onUnitEvade(e->getUnit());
//					break;
//				case EventType::UnitShow:
//					client->onUnitShow(e->getUnit());
//					break;
//				case EventType::UnitHide:
//					client->onUnitHide(e->getUnit());
//					break;
//				case EventType::UnitCreate:
//					client->onUnitCreate(e->getUnit());
//					break;
//				case EventType::UnitDestroy:
//					client->onUnitDestroy(e->getUnit());
//					break;
//				case EventType::UnitMorph:
//					client->onUnitMorph(e->getUnit());
//					break;
//				case EventType::UnitRenegade:
//					client->onUnitRenegade(e->getUnit());
//					break;
//				case EventType::SaveGame:
//					client->onSaveGame(e->getText());
//					break;
//				case EventType::UnitComplete:
//					client->onUnitComplete(e->getUnit());
//					break;
//				default:
//					break;
//				}
//				// I think this is for networking battle
//				BWAPI::Broodwar->drawBoxScreen(0, 0, 400, 400, Colors::Orange, true);
//			}
//			BWAPI::BWAPIClient.update();
//			if (!BWAPI::BWAPIClient.isConnected())
//			{
//				std::cout << "Reconnecting..." << std::endl;
//				reconnect();
//			}
//
//		}
//		delete client;
//		FreeLibrary(hMod);
//		std::cout << "Game ended" << std::endl;
//	}
//	std::cout << "Press ENTER to continue..." << std::endl;
//	std::cin.ignore();
//	return 0;
//}
//