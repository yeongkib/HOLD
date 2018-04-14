/*****************************************************************************/
/*!
\file   Grid.cpp
\author Yeongki Baek
\par    email: yeongki.baek\@digipen.edu
\date   08/01/2017
\brief
This is the interface file for the module
Copyright 2017, Digipen Institute of Technology
*/
/*****************************************************************************/

#include "Precompiled.hpp"

#include "Grid.hpp"

namespace HOLD
{
	Grid::Grid() : quarter(0)
	{
	};

	Grid::Grid(const int& sameValue)
	{
		quarter.g[0] =
		quarter.g[1] =
		quarter.g[2] =
		quarter.g[3] = static_cast<short>(sameValue);
	}

	Grid::Grid(const short& sameValue)
	{
		quarter.g[0] =
		quarter.g[1] =
		quarter.g[2] =
		quarter.g[3] = sameValue;
	}

	Grid::Grid(short LeftTop, short RightTop, short LeftBot, short RightBot)
	{
		quarter.g[0] = LeftTop;
		quarter.g[1] = RightTop;
		quarter.g[2] = LeftBot;
		quarter.g[3] = RightBot;
	}

	Grid::Grid(double grid) 
	{
		quarter.u_double = grid;
	}

	void Grid::SetLeftTop(const short& value) 
	{
		quarter.g[0] = value;
	}

	void Grid::SetRightTop(const short& value)
	{
		quarter.g[1] = value;
	}

	void Grid::SetLeftBot(const short& value) 
	{
		quarter.g[2] = value;
	}

	void Grid::SetRightBot(const short& value)
	{
		quarter.g[3] = value;
	}

	short Grid::GetLeftTop() const 
	{
		return quarter.g[0];
	}

	short Grid::GetRightTop() const
	{
		return quarter.g[1];
	}

	short Grid::GetLeftBot() const
	{
		return quarter.g[2];
	}

	short Grid::GetRightBot() const
	{
		return quarter.g[3];
	}

	int Grid::GetTotal()
	{
		return static_cast<int>(quarter.g[0])
		     + static_cast<int>(quarter.g[1])
		     + static_cast<int>(quarter.g[2])
		     + static_cast<int>(quarter.g[3]);
		
	}

	bool Grid::IsAllSame()
	{
		return quarter.g[0] == quarter.g[1]
			&& quarter.g[0] == quarter.g[2]
			&& quarter.g[0] == quarter.g[3];
	}

	bool Grid::IsAllSame(const double& grid)
	{
		Grid temp{ grid };
		return temp.IsAllSame();
	}

	short Grid::GetLeftTop(const double& grid) 
	{
		Grid temp{ grid };
		return temp.GetLeftTop();
	}

	short Grid::GetRightTop(const double& grid) 
	{
		Grid temp{ grid };
		return temp.GetRightTop();
	}

	short Grid::GetLeftBot(const double& grid) 
	{
		Grid temp{ grid };
		return temp.GetLeftBot();
	}

	short Grid::GetRightBot(const double& grid)
	{
		Grid temp{ grid };
		return temp.GetRightBot();
	}

	Grid Grid::operator+(const int & value) const
	{
		int temp = value;
		return *this + Grid{
			static_cast<short>(temp),
			static_cast<short>(temp),
			static_cast<short>(temp),
			static_cast<short>(temp) };
	}

	Grid Grid::operator+(const Grid & rhs) const
	{
		return Grid{
			this->GetLeftTop()  + rhs.GetLeftTop(),
			this->GetRightTop() + rhs.GetRightTop(),
			this->GetLeftBot()  + rhs.GetLeftBot(),
			this->GetRightBot() + rhs.GetRightBot() };
	}

	Grid Grid::operator-(const int & value) const
	{
		int temp = value;
		return *this - Grid{
			static_cast<short>(temp),
			static_cast<short>(temp),
			static_cast<short>(temp),
			static_cast<short>(temp) };
	}

	Grid Grid::operator-(const Grid & rhs) const
	{
		return Grid{
			this->GetLeftTop()  - rhs.GetLeftTop(),
			this->GetRightTop() - rhs.GetRightTop(),
			this->GetLeftBot()  - rhs.GetLeftBot(),
			this->GetRightBot() - rhs.GetRightBot() };
	}

	bool Grid::operator!=(const short& value)
	{
		return quarter.g[0] != value
			|| quarter.g[1] != value
			|| quarter.g[2] != value
			|| quarter.g[3] != value;
	}

	bool Grid::operator==(const short& value)
	{
		return quarter.g[0] == value
			&& quarter.g[1] == value
			&& quarter.g[2] == value
			&& quarter.g[3] == value;
	}
	
	Grid& Grid::operator=(const int & value)
	{
		quarter.g[0] =
		quarter.g[1] =
		quarter.g[2] =
		quarter.g[3] = static_cast<short>(value);
		return *this;
	}

	short& Grid::operator[] (const int& index)
	{
		//todo : assert
		return quarter.g[index];
	}

	const short& Grid::operator[] (const int& index) const
	{
		return quarter.g[index];
	}
}//namespace HOLD