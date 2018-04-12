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

static bool drawRange = false;
static bool drawBoundaries = false;
static bool drawDirection = false;
static bool drawGridMap = false;
static bool drawAll = false;

static int mapHeight = 0;
static int mapWidth = 0;

std::vector<std::pair<Position, Position>> diagonalLineOfBase;
std::vector<Position> mineralpos;

int IsPointOnLine(Position p1, Position p2, Position p3) // returns true if p3 is on line p1, p2
{
	if (p3.x >= p1.x
	 && p3.x <= p2.x
	 && (
	 	 p3.y >= p1.y
	 	 && p3.y <= p2.y
		 || p3.y <= p1.y
		 && p3.y >= p2.y
	 ))
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

Vector2 closest_point(const Vector2& v, const Vector2 & w, const Vector2 & p) {
	// Return minimum distance between line segment vw and point p
	const float l2 = (w - v).LengthSq();  // i.e. |w-v|^2 -  avoid a sqrt
										  // if (l2 == 0.0) return Math::Distance(p, v);   
										  // v == w case
										  // Consider the line extending the segment, parameterized as v + t (w - v).
										  // We find projection of point p onto the line. 
										  // It falls where t = [(p-v) . (w-v)] / |w-v|^2
										  // We clamp t from [0,1] to handle points outside the segment vw.
	const float t = Math::Max(0.f, Math::Min(1.f, Math::Dot(p - v, w - v) / l2));
	return v + t * (w - v);  // Projection falls on the segment
							 // return Math::Distance(p, projection);
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

float distance(const Box& b1, const Box& b2) {
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
	else // rectangles intersect
		return 0;
}

float distance(float x, float y, float px, float py, float width, float height) {
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

void HoldAIModule::onStart() {
	runflag = true;
	if (strcmp(Broodwar->mapHash().c_str(), "ed7c5b1b03234a0f7dd484112bbb1bc49db1d6f0") == 0) // testcase
		runflag = false;

	Broodwar->printf("2018_03_31_11:48");

	try {
		Broodwar->setCommandOptimizationLevel(1);
		Broodwar->setLocalSpeed(0);
		Broodwar->setFrameSkip(0);

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
		
		HQ = HeadQuarters::GetInstance();

		if (runflag)
			HQ->AttachCommander(new LogisticsCommand);
		
		HQ->AttachCommander(new IntelligenceCommand);
		HQ->Initialize();

		mapHeight = Broodwar->mapHeight();
		mapWidth = Broodwar->mapWidth();
		
		for (Unit u : Broodwar->self()->getUnits()) {
			int type = u->getType();

			if (!u->gather(u->getClosestUnit(IsMineralField))) {
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
		if (Broodwar->isReplay()) {

			// Announce the players in the replay
			Broodwar << "The following players are in this replay:" << std::endl;

			// Iterate all the players in the game using a std:: iterator
			Playerset players = Broodwar->getPlayers();
			for (auto p : players) {
				// Only print the player if they are not an observer
				if (!p->isObserver())
					Broodwar << p->getName() << ", playing as " << p->getRace() << std::endl;
			}

		} else { // if this is not a replay
			// Retrieve you and your enemy's races. enemy() will just return the first enemy.
			// If you wish to deal with multiple enemies then you must use enemies().
			if (Broodwar->enemy()) // First make sure there is an enemy
				Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;

			Broodwar << "Map initialization..." << std::endl;

			Broodwar << "gl hf" << std::endl;
		}

		analyzed = false;
		analysis_just_finished = false;
	}
	catch (const std::exception & e) {
		Broodwar << "EXCEPTION from " << __func__ << ": " << e.what() << std::endl;
	}
}

void HoldAIModule::onEnd(bool isWinner) {
	// Called when the game ends
	if (isWinner)
	{
		HQ->Shutdown();

		distBtwMineral.clear();
		lineBtwMineral.clear();
	}
}

void HoldAIModule::onFrame() {
	if (runflag)
		if(Broodwar->getStartLocations().size() != 2)
			if (Broodwar->getFrameCount() < 10) {
				auto it = std::find(std::begin(HQ->GetCommand<IntelligenceCommand>()->startingLocations), std::end(HQ->GetCommand<IntelligenceCommand>()->startingLocations), Broodwar->self()->getStartLocation());
				++it;
				if (it == std::end(HQ->GetCommand<IntelligenceCommand>()->startingLocations))
					it = std::begin(HQ->GetCommand<IntelligenceCommand>()->startingLocations);

				(HQ->GetCommand<IntelligenceCommand>()->Units[Broodwar->self()][UnitTypes::Enum::Zerg_Overlord]).move(Position{ *it });
			}
	
	try {
		auto m_StartTime = std::chrono::high_resolution_clock::now();
		HQ->Run();

		for (auto worker : HQ->GetCommand<IntelligenceCommand>()->Units[Broodwar->self()][UnitTypes::Zerg_Drone]) {
			float closest = std::numeric_limits<float>::max();
			Vector2 cp{ 0,0 };
			Position begin{ 0,0 };
			for (auto line : diagonalLineOfBase) {
				Vector2 projection{ closest_point(Vector2{ 1.f * line.first.x, 1.f * line.first.y },
					Vector2{ 1.f * line.second.x, 1.f * line.second.y },
					Vector2{ 1.f * worker->getPosition().x, 1.f * worker->getPosition().y }) };
				float distance = Math::Distance(Vector2{ static_cast<float>(worker->getPosition().x), static_cast<float>(worker->getPosition().y) }, projection);
				if (distance < closest) {
					closest = distance;
					cp = projection;
					begin = line.first;
				}
			}

			for(auto mineral : mineralpos) {
				for(auto hat : HQ->GetCommand<IntelligenceCommand>()->Units[Broodwar->self()][UnitTypes::Zerg_Hatchery])
					Broodwar->drawLineMap(mineral, hat->getPosition(), Colors::Red);
			}
			
			#define PI 3.14159265358979323846

			Broodwar->drawLineMap(worker->getPosition(), Position{ static_cast<int>(cp.x), static_cast<int>(cp.y) }, Colors::Red);
			Broodwar->drawTextMap(worker->getPosition() + Position{ 0, 10 }, "%.2f", closest);
			Broodwar->drawTextMap(worker->getPosition() + Position{ 0, 20 }, "%.2f", worker->getAngle() * 180.0 / PI);
			Broodwar->drawTextMap(worker->getPosition() + Position{ 0, 30 }, "%d, %d", worker->getPosition().x, worker->getPosition().y);

			Broodwar->drawLineMap(worker->getPosition(), worker->getTargetPosition(), Colors::Orange);
			DrawBoundary(UnitTypes::Enum::Zerg_Drone, worker->getTargetPosition(), Colors::Yellow);

			Broodwar->drawTextMap(worker->getPosition() + Position{ 0, 40 }, "%f", sqrt(worker->getVelocityX()*worker->getVelocityX() +  worker->m_unit->getVelocityY()*worker->m_unit->getVelocityY()));
			
			float distfromcenter = Math::Distance(Vector2{ static_cast<float>(worker->getPosition().x), static_cast<float>(worker->getPosition().y) },
				Vector2{ static_cast<float>(begin.x), static_cast<float>(begin.y) });
				Broodwar->drawTextMap(worker->getPosition() + Position{ 0, 50 }, "%.2f", distfromcenter);

			Broodwar->drawTextMap(worker->getPosition() + Position{ 0, 60 }, "%d, %d", worker->getOrderTargetPosition().x - worker->getPosition().x,
																					worker->getOrderTargetPosition().y - worker->getPosition().y);

			Broodwar->drawCircleMap(worker->getOrderTargetPosition(), 2, Colors::Red, true);

			DrawDirection(worker->m_unit, 30.0);
			
			double theta = worker->getAngle() + (90.0 * PI / 180.0);

			//vec1 : from current position to target position
			double x1 = worker->getTargetPosition().x - worker->getPosition().x;
			double y1 = worker->getTargetPosition().y - worker->getPosition().y;
			
			//vec2 : direction vector
			double x2 = worker->getPosition().x + cos(theta);
			double y2 = worker->getPosition().y + sin(theta);

			double sin1 = x1 * y2 - x2 * y1;
			double cos1 = x1 * x2 + y1 * y2;

			double angle = atan2(sin1, cos1) * 180.0 / PI;

			Broodwar->drawTextMap(worker->getPosition() + Position{ 0, -20 }, "%d, %d", 
				static_cast<int>(worker->getPosition().x + worker->getVelocityX() * cos(angle)),
				static_cast<int>(worker->getPosition().y + worker->getVelocityY() * sin(angle)));

			Broodwar->drawTextMap(worker->getPosition() + Position{ 0, -30 }, "%f, %f",
				worker->getAngle() * 180.0 / PI, angle);
		}

		// Display the game frame rate as text in the upper left area of the screen
		Broodwar->drawTextScreen(415, 15, "%cFrames%c%d", Text::White, Text::Green, Broodwar->getFrameCount());
		Broodwar->drawTextScreen(500, 15, "%cFPS%c%d", Text::White, Text::Green, Broodwar->getFPS());
		
		int frameCount = Broodwar->getFrameCount();
		Broodwar->drawTextScreen(300, 0, "\x04 %4dm %3ds", (int)(frameCount / (24 * 60)), (int)((int)(frameCount / 24) % 60));

		// Return if the game is a replay or is paused
		if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
			return;

		if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
			return;

		Unitset larvaAndOver;
		Unitset mutalAndOver;

		if (Broodwar->isReplay())
			return;

		current_time = Broodwar->getFrameCount();
		if (current_time - last_time > 30.0) {
			for (auto i = larvaAndOver.begin(); i != larvaAndOver.end(); ++i) {
				Broodwar->issueCommand(larvaAndOver, UnitCommand::stop((*i)));
			}

			for (auto &u : Broodwar->self()->getUnits().getLarva()) {
				Broodwar->issueCommand(Broodwar->self()->getUnits().getLarva(), UnitCommand::stop(u));
			}

			last_time = current_time;
		}
		
		for (auto &i : distBtwMineral) {
			Broodwar->drawText(CoordinateType::Map, i.first.x, i.first.y, "%g", i.second);
		}

		for (auto dl : diagonalLineOfBase) {
			Broodwar->drawLineMap(dl.first, dl.second, Colors::Teal);
		}

		//My Influence
		BWAPI::Position scrPos = Broodwar->getScreenPosition();

		auto DrawInfluenceMap = [](auto & scrPos, std::array<double, 256*256>& map, auto boxColor, auto txtColor)
		{
			for (int y = scrPos.y / 32; y < (scrPos.y + 14 * 32) / 32 + 1 && y < Broodwar->mapHeight(); ++y) {
				for (int x = scrPos.x / 32; x < (scrPos.x + 19 * 32) / 32 + 1 && x < Broodwar->mapWidth(); ++x) {
					if (map[y * mapHeight + x] > 0.0) {
						if (Grid::IsAllSame(map[y * mapHeight + x])) {
							Broodwar->setTextSize(Text::Size::Huge);
							Broodwar->drawTextMap(x * 32, y * 32, " %c%d", txtColor, Grid::GetLeftTop(map[y * mapHeight + x]));
							Broodwar->drawBoxMap(x * 32, y * 32, x * 32 + 32, y * 32 + 32, boxColor);
						} else {
							Broodwar->setTextSize(Text::Size::Small);

							if (Grid::GetLeftTop(map[y * mapHeight + x]) != 0) {
								Broodwar->drawTextMap(x * 32, y * 32, " %c%d", txtColor, Grid::GetLeftTop(map[y * mapHeight + x]));
								Broodwar->drawBoxMap(x * 32, y * 32, x * 32 + 16, y * 32 + 16, boxColor);
							}

							if (Grid::GetRightTop(map[y * mapHeight + x]) != 0) {
								Broodwar->drawTextMap(x * 32 + 16, y * 32, " %c%d", txtColor, Grid::GetRightTop(map[y * mapHeight + x]));
								Broodwar->drawBoxMap(x * 32 + 16, y * 32, x * 32 + 32, y * 32 + 16, boxColor);
							}

							if (Grid::GetLeftBot(map[y * mapHeight + x]) != 0) {
								Broodwar->drawTextMap(x * 32, y * 32 + 16, " %c%d", txtColor, Grid::GetLeftBot(map[y * mapHeight + x]));
								Broodwar->drawBoxMap(x * 32, y * 32 + 16, x * 32 + 16, y * 32 + 32, boxColor);
							}

							if (Grid::GetRightBot(map[y * mapHeight + x]) != 0) {
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

		Broodwar->drawTextScreen(100, 100, "%d", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_StartTime).count());

		/*
		 * More than 1 frame longer than 10 seconds, or
		 * more than 10 frames longer than 1 second, or
		 * more than 320 frames longer than 85 milliseconds.
		 */
	} catch (const std::exception & e) {
		Broodwar << "EXCEPTION from " << __func__ << ": " << e.what() << std::endl;
	}
}

void HoldAIModule::onSendText(std::string text) {
	char* endptr = nullptr;
	const char *input_ptr = text.c_str();
	long value = std::strtol(input_ptr, &endptr, 10);

	if (endptr == input_ptr) {
		// Input was not a valid number
	} else if (*endptr != '\0') {
		// Input starts with a valid number, but ends with some extra characters
		// (for example "123abc")
		// `value` is set to the numeric part of the string
	} else {
		// Input was a valid number
		//Broodwar->sendText("%s", text.c_str());
		BWAPI::Broodwar->setLocalSpeed(value);
	}

	BWEM::utils::MapDrawer::ProcessCommand(text);

	// Make sure to use %s and pass the text as a parameter,
	// otherwise you may run into problems when you use the %(percent) character!
	if (text == "/analyze") {
		if (analyzed == false) {
			Broodwar->printf("Analyzing map... this may take a minute");
			//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL);
		}

	} else if ("range" == text) {
		drawRange = !drawRange;

	} else if ("boundary" == text) {
		drawBoundaries = !drawBoundaries;

	} else if ("dir" == text) {
		drawDirection = !drawDirection;

	} else if ("grid" == text) {
		drawGridMap = !drawGridMap;

	} else if ("all" == text) {
		drawAll = !drawAll;
		drawRange = drawBoundaries = drawDirection = drawGridMap = drawAll;

	} else if ("cheat" == text) {
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

	} else {
		Broodwar->sendText("%s", text.c_str());
	}

}

void HoldAIModule::onReceiveText(BWAPI::Player player, std::string text) {
	// Parse the received text
	Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void HoldAIModule::onPlayerLeft(BWAPI::Player player) {
	// Interact verbally with the other players in the game by
	// announcing that the other player has left.
	//Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void HoldAIModule::onNukeDetect(BWAPI::Position target) {
	// Check if the target is a valid position
	if (target) {
		// if so, print the location of the nuclear strike target
		Broodwar << "Nuclear Launch Detected at " << target << std::endl;
	} else {
		// Otherwise, ask other players where the nuke is!
		Broodwar->sendText("Where's the nuke?");
	}
	// You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

void HoldAIModule::onUnitDiscover(BWAPI::Unit unit) {
	//void, use show fn
}

void HoldAIModule::onUnitEvade(BWAPI::Unit unit) {
	//void, use hide fn
}

void HoldAIModule::onUnitShow(BWAPI::Unit unit) {
	HQ->GetCommand<IntelligenceCommand>()->OnUnitShow(unit);
	
	{
		auto FindDirection = [](auto & b)
		{
		};
		//todo : check the direction to resources from the base
		auto AddDiagonalLineOfBase = [](auto & b)
		{
			diagonalLineOfBase.emplace_back(std::make_pair<Position, Position>(Position{ b->getLeft(), b->getTop() }, Position{ b->getLeft() - 100, b->getTop() - 100 }));
			diagonalLineOfBase.emplace_back(std::make_pair<Position, Position>(Position{ b->getRight(), b->getTop() }, Position{ b->getRight() + 100, b->getTop() - 100 }));
			diagonalLineOfBase.emplace_back(std::make_pair<Position, Position>(Position{ b->getLeft(), b->getBottom() }, Position{ b->getLeft() - 100, b->getBottom() + 100 }));
			diagonalLineOfBase.emplace_back(std::make_pair<Position, Position>(Position{ b->getRight(), b->getBottom() }, Position{ b->getRight() + 100, b->getBottom() + 100 }));
		};

		if (unit->getType().isResourceDepot())
			AddDiagonalLineOfBase(unit);

		if (unit->getType() == UnitTypes::Resource_Mineral_Field
			|| unit->getType() == UnitTypes::Resource_Mineral_Field_Type_2
			|| unit->getType() == UnitTypes::Resource_Mineral_Field_Type_3)
			mineralpos.emplace_back(unit->getPosition());
	}
}

void HoldAIModule::onUnitHide(BWAPI::Unit unit) {
	int i = Broodwar->getFrameCount();
	
	//todo : set unit's hide position
	UnitType unitType = unit->getType();
}

void HoldAIModule::onUnitCreate(BWAPI::Unit unit) {
	if (Broodwar->isReplay()) {
		// if we are in a replay, then we will print out the build order of the structures
		if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral()) {
			int seconds = Broodwar->getFrameCount() / 24;
			int minutes = seconds / 60;
			seconds %= 60;
			Broodwar->sendText("%.2d:%.2d: %s creates a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
		}

	} else {
		HQ->GetCommand<IntelligenceCommand>()->OnUnitCreate(unit);
	}
}

void HoldAIModule::onUnitDestroy(BWAPI::Unit unit) {
	try {
		UnitType unitType = unit->getType();

		HQ->GetCommand<IntelligenceCommand>()->OnUnitDestroy(unit);
	} catch (const std::exception & e) {
		Broodwar << "EXCEPTION from " << __func__ << ": " << e.what() << std::endl;
	}
}

void HoldAIModule::onUnitMorph(BWAPI::Unit unit) {
	try {
		HQ->OnUnitMorph(unit);
		UnitType unitType = unit->getType();

		if (!Broodwar->isReplay()) {

		} else {//if (Broodwar->isReplay())
			// if we are in a replay, then we will print out the build order of the structures
			if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral()) {
				int seconds = Broodwar->getFrameCount() / 24;
				int minutes = seconds / 60;
				seconds %= 60;
				Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
			}
		}

	} catch (const std::exception & e) {
		Broodwar << "EXCEPTION from " << __func__ << ": " << e.what() << std::endl;
	}
}

void HoldAIModule::onUnitRenegade(BWAPI::Unit unit) {
}

void HoldAIModule::onSaveGame(std::string gameName) {
	Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void HoldAIModule::onUnitComplete(BWAPI::Unit unit) {
	UnitType unitType = unit->getType();

	if (UnitTypes::Zerg_Larva == unitType)
		return;

	if (unit->getType().isWorker()) 	{
		if (!unit->gather(unit->getClosestUnit(IsMineralField))) {
			// If the call fails, then print the last error message
			Broodwar << Broodwar->getLastError() << std::endl;
		}

	} else if (unit->getType().isRefinery()) {
		auto work = std::hash<std::string>{}((std::string(std::to_string(unit->getID())) + std::string("_work")));

		HOLD::RegisterEvent(
			[=](BWAPI::Game*)
		{
			size_t nWorkersAssigned = unit->getClientInfo<size_t>(work);
			if (nWorkersAssigned < 3) {
				Unit pClosestIdleWorker = unit->getClosestUnit(IsWorker && !IsGatheringGas);//&& !IsCarryingSomething && !IsGatheringGas && IsMoving);
				if (pClosestIdleWorker) {
					// gather from the refinery (and check if successful)
					if (pClosestIdleWorker->gather(unit)) {
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
	}
	auto FindDirection = [](auto & b) {

	};

	//todo : check the direction to resources from the base
	auto AddDiagonalLineOfBase = [](auto & b) {
		diagonalLineOfBase.emplace_back(std::make_pair<Position, Position>(Position{ b->getLeft(), b->getTop() }, Position{ b->getLeft() - 100, b->getTop() - 100 }));
		diagonalLineOfBase.emplace_back(std::make_pair<Position, Position>(Position{ b->getRight(), b->getTop() }, Position{ b->getRight() + 100, b->getTop() - 100 }));
		diagonalLineOfBase.emplace_back(std::make_pair<Position, Position>(Position{ b->getLeft(), b->getBottom() }, Position{ b->getLeft() - 100, b->getBottom() + 100 }));
		diagonalLineOfBase.emplace_back(std::make_pair<Position, Position>(Position{ b->getRight(), b->getBottom() }, Position{ b->getRight() + 100, b->getBottom() + 100 }));
	};

	if (unit->getType().isResourceDepot())
		AddDiagonalLineOfBase(unit);
}
