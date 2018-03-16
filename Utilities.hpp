/*****************************************************************************/
/*!
\file   Utilities.hpp
\author Yeongki Baek
\par    email: yeongki.baek\@digipen.edu
\date   08/01/2017
\brief
This is the interface file for the module
Copyright 2017, Digipen Institute of Technology
*/
/*****************************************************************************/
#pragma once

// Includes

#include "Grid.hpp"
#include "Vector2.hpp"
#include "Reals.hpp"
#include <stdio.h>
#include <BWAPI/Unitset.h>
#include <BWAPI/Unit.h>
#include <BWAPI/Game.h>
#include <BWAPI/Player.h>


using namespace BWAPI;
using namespace HOLD::Math;


namespace HOLD
{
	#define PI 3.14159265358979323846

	const auto registerEvent = [](const auto &action, auto condition = nullptr, int timesToRun = -1, int framesToCheck = 0)
	{
		Broodwar->registerEvent(action, condition, 1, 1); // do
		Broodwar->registerEvent(action, condition, timesToRun, framesToCheck); // while
	};

	const inline float angleBetween(const TilePosition &v1, const TilePosition &v2)
	{
		//float len1 = sqrt(v1.x * v1.x + v1.y * v1.y);
		//float len2 = sqrt(v2.x * v2.x + v2.y * v2.y);

		//float dot = v1.x * v2.x + v1.y * v2.y;

		//float a = dot / (len1 * len2);

		//if (a >= 1.0)
		//	return 0.0;
		//else if (a <= -1.0)
		//	return PI;
		//else
		//	return acos(a); // 0..PI
		float dot = v1.x*v2.x + v1.y*v2.y;      // dot product between[x1, y1] and [x2, y2]
		float	det = v1.x*v2.y - v1.y*v2.x;      // determinant
		return atan2(det, dot);  // atan2(y, x) or atan2(sin, cos)
	}

	const auto DrawDirection = [](Unit & u, const double& distance)
	{
		if (u->getType().isBuilding()
			|| u->getType() == UnitTypes::Enum::Zerg_Larva
			|| u->getType() == UnitTypes::Enum::Zerg_Egg)
			return;
		double x1 = static_cast<double>(u->getPosition().x);
		double y1 = static_cast<double>(u->getPosition().y);

		double _cos = cos(u->getAngle());
		double _sin = sin(u->getAngle());

		double x2 = x1 + distance * _cos;
		double y2 = y1 + distance * _sin;

		Broodwar->drawLineMap(x1, y1, x2, y2,
			Colors::White);

		// draw triangle
		double a0 = x1 + distance * 1.2 * _cos;
		double b0 = y1 + distance * 1.2 * _sin;
		double xdif = x2 - x1;
		double ydif = y2 - y1;
		double a1 = x2 - ydif / 8.0;
		double b1 = y2 + xdif / 8.0;
		double a2 = x2 + ydif / 8.0;
		double b2 = y2 - xdif / 8.0;

		Broodwar->drawTriangleMap(a0, b0, a1, b1, a2, b2,
			Colors::White);
	};

	inline void drawWeaponCooldown(Unit u)
	{
		if (u->isIdle())
			return;
		// Store the types as locals
		UnitType uType = u->getType();

		// Obtain the weapon type
		WeaponType groundWeapon = uType.groundWeapon();
		WeaponType airWeapon = uType.airWeapon();

		if (groundWeapon == WeaponTypes::None
			|| groundWeapon == WeaponTypes::Unknown)
		{
		}
		else
		{
			// Retrieve the min and max weapon ranges
			int minRange = groundWeapon.minRange();
			int maxRange = u->getPlayer()->weaponMaxRange(groundWeapon); // it need to check range upgrade

			if (minRange == maxRange)
			{
				Broodwar->drawCircleMap(u->getPosition(), maxRange + 1, Colors::White);
			}
			else
			{
				if (minRange)
					Broodwar->drawCircleMap(u->getPosition(), minRange + 1, Colors::White);
				Broodwar->drawCircleMap(u->getPosition(), maxRange + 1, Colors::White);
			}
			Broodwar->drawCircleMap(u->getPosition(),
				static_cast<int>(static_cast<float>(maxRange) * (1. - static_cast<float>(u->getGroundWeaponCooldown() / static_cast<float>(groundWeapon.damageCooldown())))),
				Colors::Grey);
		}

		if (airWeapon == WeaponTypes::None
			|| airWeapon == WeaponTypes::Unknown)
		{
		}
		else
		{
			// Retrieve the min and max weapon ranges
			int minRange = airWeapon.minRange();
			int maxRange = u->getPlayer()->weaponMaxRange(airWeapon); // it need to check range upgrade

			if (minRange == maxRange)
			{
				Broodwar->drawCircleMap(u->getPosition(), minRange, Colors::Cyan);
			}
			else
			{
				if (minRange)
					Broodwar->drawCircleMap(u->getPosition(), minRange, Colors::Blue);
				Broodwar->drawCircleMap(u->getPosition(), maxRange, Colors::Cyan);
			}
			Broodwar->drawCircleMap(u->getPosition(),
				static_cast<int>(static_cast<float>(maxRange) * (1. - static_cast<float>(u->getAirWeaponCooldown() / static_cast<float>(airWeapon.damageCooldown())))),
				Colors::Grey);
		}
	}



	inline void drawBoundary(UnitType ut, Position pos, Color color = Colors::Red)
	{
		Broodwar->drawBoxMap(pos.x - ut.dimensionLeft(), pos.y - ut.dimensionUp(), pos.x + ut.dimensionRight(), pos.y + ut.dimensionDown(), color);
	}

	inline void drawBoundary(Unit u, Color color = Colors::White)
	{
		UnitType ut = u->getType();
		int x = u->getPosition().x;
		int y = u->getPosition().y;

		Broodwar->drawBoxMap(u->getLeft(), u->getTop(), u->getRight(), u->getBottom(), color);
	}

	inline Grid abs(const Grid & grid)
	{
		return Grid{ static_cast<short>(std::abs(grid[0])), static_cast<short>(std::abs(grid[1])), static_cast<short>(std::abs(grid[2])), static_cast<short>(std::abs(grid[3])) };
	};

	inline short GetInfluenceValue(std::vector<Grid> & inf_map, const int& posX, const int& posY)
	{
		int x = posX % 32;
		int y = posY % 32;

		if (x < 16 && y < 16)
			return inf_map[posY / 32 * Broodwar->mapHeight() + posX / 32].GetLeftTop();
		else if (x > 16 && y < 16)
			return inf_map[posY / 32 * Broodwar->mapHeight() + posX / 32].GetRightTop();
		else if (x > 16 && y > 16)
			return inf_map[posY / 32 * Broodwar->mapHeight() + posX / 32].GetRightBot();
		else// if (x < 16 && y > 16)
			return inf_map[posY / 32 * Broodwar->mapHeight() + posX / 32].GetLeftBot();
	};

	inline short GetInfluenceValue(std::vector<Grid> & inf_map, Position & pos)
	{
		int x = pos.x % 32;
		int y = pos.y % 32;

		if (x < 16 && y < 16)
			return inf_map[pos.y / 32 * Broodwar->mapHeight() + pos.x / 32].GetLeftTop();
		else if (x > 16 && y < 16)
			return inf_map[pos.y / 32 * Broodwar->mapHeight() + pos.x / 32].GetRightTop();
		else if (x > 16 && y > 16)
			return inf_map[pos.y / 32 * Broodwar->mapHeight() + pos.x / 32].GetRightBot();
		else// if (x < 16 && y > 16)
			return inf_map[pos.y / 32 * Broodwar->mapHeight() + pos.x / 32].GetLeftBot();
	};
}//namespace HOLD
//
//static int gConditionalFalseConstant = 0;
//
//#ifdef _MSC_VER
//#define ZERO_DEBUG_BREAK __debugbreak();
//#else
//#define ZERO_DEBUG_BREAK
//#endif
//
//#if !defined(ZERO_ENABLE_ERROR) 
//#   if defined(_DEBUG)
//#       define ZERO_ENABLE_ERROR 1
//#   else
//#       define ZERO_ENABLE_ERROR 0
//#   endif
//#endif
//
//#if defined(_MSC_VER) && _MSC_VER >= 1600
//#define StaticAssert(name, Expression, error) \
//    static_assert(Expression, error)
//#else
//#define StaticAssert(name, Expression, error) \
//    static int name = (sizeof(char[1 - 2 * !(Expression)]))
//#endif
//
//void PrintError(const char* format, ...);
//
//#if ZERO_ENABLE_ERROR
//
//#define UnusedParameter(param) param
//
//#define WarnIf(Expression, ...) \
//do { if((Expression)) { PrintError(__VA_ARGS__); fprintf(stderr, "\n"); } } while(gConditionalFalseConstant)
//
//#define ErrorIf(Expression, ...) \
//do { if((Expression)) { \
//  ZERO_DEBUG_BREAK; PrintError(__VA_ARGS__); fprintf(stderr, "\n"); } } while(gConditionalFalseConstant)
//
//#define Assert(Expression, ...) \
//do { if(!(Expression)) { \
//  ZERO_DEBUG_BREAK; PrintError(__VA_ARGS__); fprintf(stderr, "\n"); } } while(gConditionalFalseConstant)
//
//#define Error(...)\
//do { ZERO_DEBUG_BREAK; PrintError(__VA_ARGS__); fprintf(stderr, "\n"); } while(gConditionalFalseConstant)
//
//#define ErrorWithCondititionAssert(Expression, ...) \
//do { if((Expression)) ZERO_DEBUG_BREAK; PrintError(__VA_ARGS__); fprintf(stderr, "\n"); } while(gConditionalFalseConstant)
//
//#define Warn(...)\
//do { PrintError(__VA_ARGS__); fprintf(stderr, "\n"); } while(gConditionalFalseConstant)
//
//#define FileErrorIf(Expression, file, Line, ...) \
//do { ZERO_DEBUG_BREAK; PrintError(__VA_ARGS__); fprintf(stderr, "\n"); } while(gConditionalFalseConstant)
//
//#define Verify(funccall) ErrorIf(funcall != 0);
//
//#else
//
//#define UnusedParameter(param)
//#define WarnIf(...) ((void)0)
//#define ErrorIf(...) ((void)0)
//#define Assert(...) ((void)0)
//#define Error(...) ((void)0)
//#define ErrorWithCondititionAssert( ...) ((void)0)
//#define Warn(...) ((void)0)
//#define FileErrorIf(...) ((void)0)
//#define Verify(funccall) funccall
//
//#endif
//
//#define ReturnIf(Expression , whatToReturn, ...) \
//  do { if(Expression) {                          \
//    WarnIf(Expression, __VA_ARGS__);             \
//    return whatToReturn;                         \
//  } } while(gConditionalFalseConstant)
//
//#include <cstddef>
//#include <cfloat>

//
//using namespace BWAPI;
//
//
//namespace HOLD
//{
//	auto registerEvent = [](auto action, auto condition = nullptr, int timesToRun = -1, int framesToCheck = 0)
//	{
//		Broodwar->registerEvent(action, condition, 1, 1); // do
//		Broodwar->registerEvent(action, condition, timesToRun, framesToCheck); // while
//	};
//
//	auto DrawDirection = [](Unit & u, double distance)
//	{
//		double x1 = static_cast<double>(u->getPosition().x);
//		double y1 = static_cast<double>(u->getPosition().y);
//
//		double _cos = cos(u->getAngle());
//		double _sin = sin(u->getAngle());
//
//		double x2 = x1 + distance * _cos;
//		double y2 = y1 + distance * _sin;
//
//		Broodwar->drawLineMap(x1, y1, x2, y2,
//			Colors::White);
//
//		// draw triangle
//		double a0 = x1 + distance * 1.2 * _cos;
//		double b0 = y1 + distance * 1.2 * _sin;
//		double xdif = x2 - x1;
//		double ydif = y2 - y1;
//		double a1 = x2 - ydif / 8.0;
//		double b1 = y2 + xdif / 8.0;
//		double a2 = x2 + ydif / 8.0;
//		double b2 = y2 - xdif / 8.0;
//
//		Broodwar->drawTriangleMap(a0, b0, a1, b1, a2, b2,
//			Colors::White);
//	};
//
//	void drawWeaponCooldown(Unit u)
//	{
//		if (u->isIdle())
//			return;
//		// Store the types as locals
//		UnitType uType = u->getType();
//
//		// Obtain the weapon type
//		WeaponType groundWeapon = uType.groundWeapon();
//		WeaponType airWeapon = uType.airWeapon();
//
//		if (groundWeapon == WeaponTypes::None
//			|| groundWeapon == WeaponTypes::Unknown)
//		{
//		}
//		else
//		{
//			// Retrieve the min and max weapon ranges
//			int minRange = groundWeapon.minRange();
//			int maxRange = u->getPlayer()->weaponMaxRange(groundWeapon); // it need to check range upgrade
//
//			if (minRange == maxRange)
//			{
//				Broodwar->drawCircleMap(u->getPosition(), maxRange + 1, Colors::White);
//			}
//			else
//			{
//				if (minRange)
//					Broodwar->drawCircleMap(u->getPosition(), minRange + 1, Colors::White);
//				Broodwar->drawCircleMap(u->getPosition(), maxRange + 1, Colors::White);
//			}
//			//(double)d * 3.14159265358979323846 / 128.0;
//			//#define RADIANS(a) (a * 3.14f / 180.0f)
//			//cos(RADIANS(_angle))
//			/*Broodwar->drawLineMap(u->getPosition().x, u->getPosition().y,
//			static_cast<int>(static_cast<float>(u->getPosition().x)) + static_cast<float>(maxRange) * cos(3.14159265358979323846 * (static_cast<float>(u->getGroundWeaponCooldown()) / static_cast<float>(groundWeapon.damageCooldown()))),
//			static_cast<int>(static_cast<float>(u->getPosition().y)) + static_cast<float>(maxRange) * sin(3.14159265358979323846 * (static_cast<float>(u->getGroundWeaponCooldown()) / static_cast<float>(groundWeapon.damageCooldown()))),
//			Colors::White);*/
//			/*Broodwar->drawLineMap(u->getPosition().x, u->getPosition().y,
//			static_cast<int>(static_cast<float>(u->getPosition().x)) + static_cast<float>(maxRange) * -1.0 * cos(45.0 + 2.0 * 3.14159265358979323846 * (static_cast<float>(u->getGroundWeaponCooldown()) / static_cast<float>(groundWeapon.damageCooldown()))),
//			static_cast<int>(static_cast<float>(u->getPosition().y)) + static_cast<float>(maxRange) * -1.0 * -sin(45.0 + 2.0 * 3.14159265358979323846 * (static_cast<float>(u->getGroundWeaponCooldown()) / static_cast<float>(groundWeapon.damageCooldown()))),
//			Colors::White);*/
//#define PI 3.14159265358979323846
//			/*Broodwar->drawLineMap(u->getPosition().x, u->getPosition().y,
//			static_cast<int>(static_cast<float>(u->getPosition().x)) + static_cast<float>(maxRange) * cos(static_cast<float>((u->getGroundWeaponCooldown()) / static_cast<float>(groundWeapon.damageCooldown())*360.0 + 90.0) * PI / 180.0),
//			static_cast<int>(static_cast<float>(u->getPosition().y)) + static_cast<float>(maxRange) * -sin(static_cast<float>((u->getGroundWeaponCooldown()) / static_cast<float>(groundWeapon.damageCooldown())*360.0 + 90.0) * PI / 180.0),
//			Colors::White);*/
//			Broodwar->drawCircleMap(u->getPosition(),
//				static_cast<int>(static_cast<float>(maxRange) * (1. - static_cast<float>(u->getGroundWeaponCooldown() / static_cast<float>(groundWeapon.damageCooldown())))),
//				Colors::Grey);
//			//radians = atan(y/x)
//			//degrees = radians * (180.0 / 3.14)
//			/*double x = static_cast<int>(static_cast<float>(u->getPosition().x)) * static_cast<float>(maxRange) * (static_cast<float>(u->getGroundWeaponCooldown()) / static_cast<float>(groundWeapon.damageCooldown()));
//			double y = static_cast<int>(static_cast<float>(u->getPosition().y)) * static_cast<float>(maxRange) * (static_cast<float>(u->getGroundWeaponCooldown()) / static_cast<float>(groundWeapon.damageCooldown()));
//			double radian = atan(y / x);
//			double degree = radian * (180.0 / 3.14);
//			Broodwar->drawLineMap(u->getPosition(),
//			u->getPosition() * degree,
//			Colors::White);*/
//		}
//
//		if (airWeapon == WeaponTypes::None
//			|| airWeapon == WeaponTypes::Unknown)
//		{
//		}
//		else
//		{
//			// Retrieve the min and max weapon ranges
//			int minRange = airWeapon.minRange();
//			int maxRange = u->getPlayer()->weaponMaxRange(airWeapon); // it need to check range upgrade
//
//			if (minRange == maxRange)
//			{
//				Broodwar->drawCircleMap(u->getPosition(), minRange, Colors::Cyan);
//			}
//			else
//			{
//				if (minRange)
//					Broodwar->drawCircleMap(u->getPosition(), minRange, Colors::Blue);
//				Broodwar->drawCircleMap(u->getPosition(), maxRange, Colors::Cyan);
//			}
//			//#define PI 3.14159265358979323846
//			//			Broodwar->drawLineMap(u->getPosition().x, u->getPosition().y,
//			//				static_cast<int>(static_cast<float>(u->getPosition().x)) + static_cast<float>(maxRange) * cos(static_cast<float>((u->getAirWeaponCooldown()) / static_cast<float>(airWeapon.damageCooldown())*360.0 + 90.0) * PI / 180.0),
//			//				static_cast<int>(static_cast<float>(u->getPosition().y)) + static_cast<float>(maxRange) * -sin(static_cast<float>((u->getAirWeaponCooldown()) / static_cast<float>(airWeapon.damageCooldown())*360.0 + 90.0) * PI / 180.0),
//			//				Colors::White);
//			Broodwar->drawCircleMap(u->getPosition(),
//				static_cast<int>(static_cast<float>(maxRange) * (1. - static_cast<float>(u->getAirWeaponCooldown() / static_cast<float>(airWeapon.damageCooldown())))),
//				Colors::Grey);
//		}
//	}
//
//
//	//	void drawGrid(Unit u)
//	//	{
//	//		UnitType ut = u->getType();
//	//		int x = u->getLeft();
//	//		int y = u->getTop();
//	//
//	//#define DRAWUNITBOX(x, y, la, ra, color) Broodwar->drawBoxMap(x, y, x+la, y+ra, color);
//	//
//	//		switch(ut)
//	//		{
//	//		case UnitTypes::Zerg_Egg://32x32
//	//			DRAWUNITBOX(x, y, 32, 32, Colors::White)
//	//			break;
//	//
//	//		
//	//
//	//		case UnitTypes::Zerg_Drone://23x23
//	//			DRAWUNITBOX(x, y, 23, 23, Colors::White)
//	//			break;
//	//
//	//		case UnitTypes::Zerg_Overlord:
//	//			break;
//	//
//	//		case UnitTypes::Zerg_Zergling://16x16
//	//			DRAWUNITBOX(x, y, 16, 16, Colors::White)
//	//			break;
//	//
//	//		case UnitTypes::Zerg_Hydralisk://21x23
//	//			DRAWUNITBOX(x, y, 21, 34, Colors::White)
//	//			break;
//	//
//	//		case UnitTypes::Zerg_Lurker_Egg://32x32
//	//			DRAWUNITBOX(x, y, 32, 32, Colors::White)
//	//			break;
//	//
//	//		case UnitTypes::Zerg_Mutalisk:
//	//			break;
//	//
//	//		case UnitTypes::Zerg_Scourge:
//	//			break;
//	//
//	//
//	//		case UnitTypes::Zerg_Ultralisk://38x32
//	//			DRAWUNITBOX(x, y, 38, 32, Colors::White)
//	//			break;
//	//
//	//		case UnitTypes::Zerg_Defiler://27x25
//	//			DRAWUNITBOX(x, y, 27, 25, Colors::White)
//	//			break;
//	//		
//	//		
//	//		
//	//		case UnitTypes::Zerg_Guardian:
//	//			break;
//	//
//	//		case UnitTypes::Zerg_Devourer:
//	//			break;
//	//
//	//		case UnitTypes::Zerg_Queen:
//	//			break;
//	//
//	//		case UnitTypes::Zerg_Broodling:
//	//			break;
//	//		
//	//
//	//			//buildings
//	//
//	//		case UnitTypes::Zerg_Hatchery://99x65
//	//		case UnitTypes::Zerg_Lair:
//	//		case UnitTypes::Zerg_Hive:
//	//			DRAWUNITBOX(x+15, y+16, 99, 65, Colors::White)
//	//			break;
//	//
//	//
//	//		case UnitTypes::Zerg_Creep_Colony://48x48
//	//		case UnitTypes::Zerg_Sunken_Colony:
//	//		case UnitTypes::Zerg_Spore_Colony:
//	//			DRAWUNITBOX(x+8, y+8, 48, 48, Colors::White)
//	//			break;
//	//
//	//		case UnitTypes::Zerg_Spawning_Pool://77x47
//	//			DRAWUNITBOX(x+12, y+4, 77, 47, Colors::White)
//	//			break;
//	//
//	//		case UnitTypes::Zerg_Extractor://128x64
//	//			DRAWUNITBOX(x, y, 128, 64, Colors::White)
//	//			break;
//	//
//	//		case UnitTypes::Zerg_Hydralisk_Den://81x57
//	//			DRAWUNITBOX(x+8, y, 81, 57, Colors::White)
//	//			break;
//	//
//	//		case UnitTypes::Zerg_Evolution_Chamber://77x53
//	//			DRAWUNITBOX(x+4, y, 77, 53, Colors::White)
//	//			break;
//	//		
//	//		case UnitTypes::Zerg_Spire://57x57
//	//		case UnitTypes::Zerg_Greater_Spire:
//	//			DRAWUNITBOX(x+4, y, 57, 57, Colors::White)
//	//			break;
//	//
//	//
//	//		case UnitTypes::Zerg_Queens_Nest://71x57
//	//			DRAWUNITBOX(x+10, y+4, 71, 57, Colors::White)
//	//			break;
//	//
//	//		case UnitTypes::Zerg_Nydus_Canal://64x64
//	//			DRAWUNITBOX(x, y, 64, 64, Colors::White)
//	//			break;
//	//
//	//		case UnitTypes::Zerg_Ultralisk_Cavern://73x64
//	//			DRAWUNITBOX(x+8, y, 73, 64, Colors::White)
//	//			break;
//	//
//	//		case UnitTypes::Zerg_Defiler_Mound://97x37
//	//			DRAWUNITBOX(x+16, y, 97, 37, Colors::White)
//	//			break;
//	//
//	//
//	//
//	//
//	//			
//	//		}
//
//	void drawGrid(Unit u)
//	{
//		UnitType ut = u->getType();
//		int x = u->getPosition().x;
//		int y = u->getPosition().y;
//
//		/*auto DRAWUNITBOX = [](auto &leftTop, auto & rightBottom, auto & color)
//		{
//		Broodwar->drawBoxMap(leftTop, rightBottom, color);
//		};*/
//		//#define DRAWUNITBOX(x, y, la, ra, color) Broodwar->drawBoxMap(x, y, x+la, y+ra, color);
//
//		Broodwar->drawBoxMap(u->getLeft(), u->getTop(), u->getRight(), u->getBottom(), Colors::White);
//		switch (ut)
//		{
//			//case UnitTypes::Zerg_Egg://32x32
//			//	DRAWUNITBOX(x, y, x+32, y+32, Colors::White)
//			//		break;
//
//
//
//			//case UnitTypes::Zerg_Drone://23x23
//			//	DRAWUNITBOX(x, y, 23, 23, Colors::White)
//			//		break;
//
//			//case UnitTypes::Zerg_Overlord:
//			//	break;
//
//			//case UnitTypes::Zerg_Zergling://16x16
//			//	DRAWUNITBOX(x, y, 16, 16, Colors::White)
//			//		break;
//
//			//case UnitTypes::Zerg_Hydralisk://21x23
//			//	DRAWUNITBOX(x, y, 21, 34, Colors::White)
//			//		break;
//
//			//case UnitTypes::Zerg_Lurker_Egg://32x32
//			//	DRAWUNITBOX(x, y, 32, 32, Colors::White)
//			//		break;
//
//			//case UnitTypes::Zerg_Mutalisk:
//			//	break;
//
//			//case UnitTypes::Zerg_Scourge:
//			//	break;
//
//
//			//case UnitTypes::Zerg_Ultralisk://38x32
//			//	DRAWUNITBOX(x, y, 38, 32, Colors::White)
//			//		break;
//
//			//case UnitTypes::Zerg_Defiler://27x25
//			//	DRAWUNITBOX(x, y, 27, 25, Colors::White)
//			//		break;
//
//
//
//			//case UnitTypes::Zerg_Guardian:
//			//	break;
//
//			//case UnitTypes::Zerg_Devourer:
//			//	break;
//
//			//case UnitTypes::Zerg_Queen:
//			//	break;
//
//			//case UnitTypes::Zerg_Broodling:
//			//	break;
//
//
//			//	//buildings
//
//			//case UnitTypes::Zerg_Hatchery://99x65
//			//case UnitTypes::Zerg_Lair:
//			//case UnitTypes::Zerg_Hive:
//			//	//Broodwar->drawBoxMap(u->getLeft(), u->getTop(), u->getRight(), u->getBottom(), Colors::White);
//			//	//DRAWUNITBOX(x + 15, y + 16, 99, 65, Colors::White)
//			//		break;
//
//
//			//case UnitTypes::Zerg_Creep_Colony://48x48
//			//case UnitTypes::Zerg_Sunken_Colony:
//			//case UnitTypes::Zerg_Spore_Colony:
//			//	DRAWUNITBOX(x + 8, y + 8, 48, 48, Colors::White)
//			//		break;
//
//			//case UnitTypes::Zerg_Spawning_Pool://77x47
//			//	DRAWUNITBOX(x + 12, y + 4, 77, 47, Colors::White)
//			//		break;
//
//			//case UnitTypes::Zerg_Extractor://128x64
//			//	DRAWUNITBOX(x, y, 128, 64, Colors::White)
//			//		break;
//
//			//case UnitTypes::Zerg_Hydralisk_Den://81x57
//			//	DRAWUNITBOX(x + 8, y, 81, 57, Colors::White)
//			//		break;
//
//			//case UnitTypes::Zerg_Evolution_Chamber://77x53
//			//	DRAWUNITBOX(x + 4, y, 77, 53, Colors::White)
//			//		break;
//
//			//case UnitTypes::Zerg_Spire://57x57
//			//case UnitTypes::Zerg_Greater_Spire:
//			//	DRAWUNITBOX(x + 4, y, 57, 57, Colors::White)
//			//		break;
//
//
//			//case UnitTypes::Zerg_Queens_Nest://71x57
//			//	DRAWUNITBOX(x + 10, y + 4, 71, 57, Colors::White)
//			//		break;
//
//			//case UnitTypes::Zerg_Nydus_Canal://64x64
//			//	DRAWUNITBOX(x, y, 64, 64, Colors::White)
//			//		break;
//
//			//case UnitTypes::Zerg_Ultralisk_Cavern://73x64
//			//	DRAWUNITBOX(x + 8, y, 73, 64, Colors::White)
//			//		break;
//
//			//case UnitTypes::Zerg_Defiler_Mound://97x37
//			//	DRAWUNITBOX(x + 16, y, 97, 37, Colors::White)
//			//		break;
//
//
//
//
//
//		}
//
//		/*for (int j = 0; j < theMap->->Height(); ++j)
//		for (int i = 0; i < ai()->GetGridMap().Width(); ++i)
//		{
//		const auto & Cell = ai()->GetGridMap().GetCell(i, j);
//		TilePosition Origin(i*VGridMap::cell_width_in_tiles, j*VGridMap::cell_width_in_tiles);
//		const BWAPI::Color color[4] = { Colors::Green, Colors::Blue, Colors::Yellow, Colors::Red };
//		Broodwar->drawBoxMap(Position(Origin), Position(Origin + VGridMap::cell_width_in_tiles), color[2 * (j & 1) + (j & 1 ? i & 1 : 1 - (i & 1))]);
//		Broodwar->drawTextMap(Position(Origin) + Position(5, 5), "%c(%d)  my: %d, %d   his: %d, %d     %d / %d", Text::Enum::Cyan,
//		ai()->Frame() - Cell.lastFrameVisible,
//		Cell.MyUnits.size(), Cell.MyBuildings.size(), Cell.HisUnits.size(), Cell.HisBuildings.size(),
//		Cell.AvgMyUnitsAndBuildingsLast256Frames(), Cell.AvgHisUnitsAndBuildingsLast256Frames());
//		}*/
//	}
//
//	typedef int frame;
//	
//
//}//namespace HOLD//namespace HOLD