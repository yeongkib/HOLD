/*****************************************************************************/
/*!
\file   Drone.hpp
\author Yeongki Baek
\par    email: yeongki.baek\@digipen.edu
\date   08/01/2017
\brief
This is the interface file for the module
Copyright 2017, Digipen Institute of Technology
*/
/*****************************************************************************/
#pragma once
#include "BehaviorTree.hpp"

namespace HOLD
{
	struct zerg_drone {
		int id;
	};

	class Unitset_drone {
	private:
		std::stack<zerg_drone*> drones;
	public:
		Unitset_drone(int nodrones) { initializeBuilding(nodrones); }
		const std::stack<zerg_drone*>& getDrones() const { return drones; }
	private:
		void initializeBuilding(int nodrones)
		{
			for (int i = 0; i < nodrones; i++)
				drones.push(new zerg_drone{ nodrones - i });
		}
	};

	struct DataContext
	{
		std::stack<zerg_drone*> drones;
		zerg_drone* currentResource;
		zerg_drone* ResourceTarget = nullptr;
	};

	class Gathering : public BehaviorTree::Node
	{
	private:
		std::string name;
		int probabilityOfSuccess;
	public:
		Gathering(const std::string newName, int prob) : name(newName), probabilityOfSuccess(prob) {}
	private:
		virtual bool run() override
		{
			if (std::rand() % 100 < probabilityOfSuccess) {
				std::cout << name << " succeeded." << std::endl;
				return true;
			}
			std::cout << name << " failed." << std::endl;
			return false;
		}
	};
}