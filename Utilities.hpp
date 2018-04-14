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
#include <cstdio>
#include <BWAPI/Unitset.h>
#include <BWAPI/Unit.h>
#include <BWAPI/Game.h>
#include <BWAPI/Player.h>
#include <array>

using namespace BWAPI;
using namespace HOLD::Math;

namespace HOLD
{
	#define PI 3.14159265358979323846

	const auto RegisterEvent = [](const auto &action, auto condition, int timesToRun = -1, int framesToCheck = 0)
	{
		Broodwar->registerEvent(action, condition, 1, 1); // do
		Broodwar->registerEvent(action, condition, timesToRun, framesToCheck); // while
	};

	inline float AngleBetween(const TilePosition &v1, const TilePosition &v2)
	{
		const float dot = v1.x * v2.x + v1.y * v2.y; // dot product between[x1, y1] and [x2, y2]
		const float det = v1.x * v2.y - v1.y * v2.x; // determinant
		return atan2(det, dot);                // atan2(y, x) or atan2(sin, cos)
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
		double a0   = x1 + distance * 1.2 * _cos;
		double b0   = y1 + distance * 1.2 * _sin;
		double xdif = x2 - x1;
		double ydif = y2 - y1;
		double a1   = x2 - ydif / 8.0;
		double b1   = y2 + xdif / 8.0;
		double a2   = x2 + ydif / 8.0;
		double b2   = y2 - xdif / 8.0;

		Broodwar->drawTriangleMap(a0, b0, a1, b1, a2, b2,
			Colors::White);
	};

	inline void DrawWeaponCooldown(Unit u)
	{
		if (u->isIdle())
			return;
		// Store the types as locals
		UnitType uType = u->getType();

		// Obtain the weapon type
		WeaponType groundWeapon = uType.groundWeapon();
		WeaponType airWeapon    = uType.airWeapon();

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
			int maxRange = u->getPlayer()->weaponMaxRange(airWeapon); //check range upgrade status

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

	inline void DrawBoundary(UnitType ut, Position pos, Color color = Colors::Red)
	{
		Broodwar->drawBoxMap(pos.x - ut.dimensionLeft(), pos.y - ut.dimensionUp(), pos.x + ut.dimensionRight(), pos.y + ut.dimensionDown(), color);
	}

	inline void DrawBoundary(Unit u, Color color = Colors::White)
	{
		UnitType ut = u->getType();
		int x = u->getPosition().x;
		int y = u->getPosition().y;

		Broodwar->drawBoxMap(u->getLeft(), u->getTop(), u->getRight(), u->getBottom(), color);
	}

	inline Grid Abs(const Grid& grid)
	{
		return Grid{ 
			static_cast<short>(std::abs(grid[0])),
			static_cast<short>(std::abs(grid[1])),
			static_cast<short>(std::abs(grid[2])),
			static_cast<short>(std::abs(grid[3]))
		};
	};

	inline double Abs(const double& grid)
	{
		Grid::uQuarter a(grid);
		/*int a0, a1, a2, a3;
		a0 = a.g[0] >> 15;
		a1 = a.g[1] >> 15;
		a2 = a.g[2] >> 15;
		a3 = a.g[3] >> 15;
		a.g[0] = (a.g[0] ^ a0) - a0;
		a.g[1] = (a.g[1] ^ a1) - a1;
		a.g[2] = (a.g[2] ^ a2) - a2;
		a.g[3] = (a.g[3] ^ a3) - a3;*/

		if (a.g[0] < 0)
			a.g[0] = a.g[0] - 1, a.g[0] = ~a.g[0];
		else
			a.g[0] = a.u_ll & 0x00000000000000FF;

		if (a.g[1] < 0)
			a.g[1] = a.g[1] - 1, a.g[1] = ~a.g[1];
		else
			a.g[1] = a.u_ll >> 16 & 0x00000000000000FF;

		if (a.g[2] < 0)
			a.g[2] = a.g[2] - 1, a.g[2] = ~a.g[2];
		else
			a.g[2] = a.u_ll >> 32 & 0x00000000000000FF;

		if (a.g[3] < 0)
			a.g[3] = a.g[3] - 1, a.g[3] = ~a.g[3];
		else
			a.g[3] = a.u_ll >> 48 & 0x00000000000000FF;

		return a.u_double;
	}

	inline short GetInfluenceValue(std::array<double, 256*256>& inf_map, const int& posX, const int& posY)
	{
		int x = posX % 32;
		int y = posY % 32;

		if (x < 16 && y < 16)
			return Grid::GetLeftTop(inf_map[posY / 32 * Broodwar->mapHeight() + posX / 32]);
		else if (x > 16 && y < 16)
			return Grid::GetRightTop(inf_map[posY / 32 * Broodwar->mapHeight() + posX / 32]);
		else if (x > 16 && y > 16)
			return Grid::GetRightBot(inf_map[posY / 32 * Broodwar->mapHeight() + posX / 32]);
		else// if (x < 16 && y > 16)
			return Grid::GetLeftBot(inf_map[posY / 32 * Broodwar->mapHeight() + posX / 32]);
	};
}//namespace HOLD