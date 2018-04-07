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
	Grid::Grid()/* : Total(0.)/*, pos{ TilePosition(0, 0) },
						LeftTop{ WalkPosition(0, 0) },
						RightTop{ WalkPosition(0, 0) },
						LeftBot{ WalkPosition(0, 0) },
						RightBot{ WalkPosition(0, 0) }*/
	{
		quarter.g[0] = quarter.g[1] = quarter.g[2] = quarter.g[3] = 0;
		//quarter[0] = quarter[1] = quarter[2] = quarter[3] = 0;
	};

	Grid::Grid(int sameValue)
	{
		quarter.g[0] = quarter.g[1] = quarter.g[2] = quarter.g[3] = static_cast<short>(sameValue);
		//quarter[0] = quarter[1] = quarter[2] = quarter[3] = static_cast<short>(sameValue);
	}

	Grid::Grid(short sameValue)
	{
		quarter.g[0] = quarter.g[1] = quarter.g[2] = quarter.g[3] = sameValue;
		//quarter[0] = quarter[1] = quarter[2] = quarter[3] = sameValue;
	}

	Grid::Grid(short LeftTop, short RightTop, short LeftBot, short RightBot)
	{
		quarter.g[0] = LeftTop;
		quarter.g[1] = RightTop;
		quarter.g[2] = LeftBot;
		quarter.g[3] = RightBot;
		/*quarter[0] = LeftTop;
		quarter[1] = RightTop;
		quarter[2] = LeftBot;
		quarter[3] = RightBot;*/
	}
	Grid::Grid(double grid)
	{
		quarter.u_double = grid;
	}

	void Grid::SetLeftTop(short value)
	{
		quarter.g[0] = value;
		//quarter[0] = value;
	}
	void Grid::SetRightTop(short value)
	{
		quarter.g[1] = value;
		//quarter[1] = value;
	}
	void Grid::SetLeftBot(short value)
	{
		quarter.g[2] = value;
		//quarter[2] = value;
	}
	void Grid::SetRightBot(short value)
	{
		quarter.g[3] = value;
		//quarter[3] = value;
	}

	short Grid::GetLeftTop() const
	{
		return quarter.g[0];
		//return quarter[0];
	}
	short Grid::GetRightTop() const
	{
		return quarter.g[1];
		//return quarter[1];
	}
	short Grid::GetLeftBot() const
	{
		return quarter.g[2];
		//return quarter[2];
	}
	short Grid::GetRightBot() const
	{
		return quarter.g[3];
		//return quarter[3];
	}

	/*short* Grid::GetQuarters()
	{
		return *&quarter;
	}*/

	int Grid::GetTotal()
	{
		return static_cast<int>(quarter.g[0]) + static_cast<int>(quarter.g[1]) + static_cast<int>(quarter.g[2]) + static_cast<int>(quarter.g[3]);
		//return static_cast<int>(quarter[0]) + static_cast<int>(quarter[1]) + static_cast<int>(quarter[2]) + static_cast<int>(quarter[3]);
	}

	bool Grid::IsAllSame()
	{
		return quarter.g[0] == quarter.g[1]
			&& quarter.g[0] == quarter.g[2]
			&& quarter.g[0] == quarter.g[3];
		/*return quarter[0] == quarter[1]
		&& quarter[0] == quarter[2]
		&& quarter[0] == quarter[3];*/
	}

	bool Grid::IsAllSame(double& grid)
	{
		Grid temp{ grid };
		return temp.IsAllSame();
	}
	short Grid::GetLeftTop(double& grid)
	{
		Grid temp{ grid };
		return temp.GetLeftTop();
	}
	short Grid::GetRightTop(double& grid)
	{
		Grid temp{ grid };
		return temp.GetRightTop();
	}
	short Grid::GetLeftBot(double& grid)
	{
		Grid temp{ grid };
		return temp.GetLeftBot();
	}
	short Grid::GetRightBot(double& grid)
	{
		Grid temp{ grid };
		return temp.GetRightBot();
	}

	//warn : is this best?
	Grid Grid::operator+(const int & value) const
	{
		int temp = value;
		return *this + Grid{ static_cast<short>(temp), static_cast<short>(temp), static_cast<short>(temp), static_cast<short>(temp) };
	}
	Grid Grid::operator+(const Grid & rhs) const
	{
		return Grid{ this->GetLeftTop() + rhs.GetLeftTop(), this->GetRightTop() + rhs.GetRightTop(), this->GetLeftBot() + rhs.GetLeftBot(), this->GetRightBot() + rhs.GetRightBot() };
	}
	Grid Grid::operator-(const int & value) const
	{
		int temp = value;
		return *this - Grid{ static_cast<short>(temp), static_cast<short>(temp), static_cast<short>(temp), static_cast<short>(temp) };
	}
	Grid Grid::operator-(const Grid & rhs) const
	{
		return Grid{ this->GetLeftTop() - rhs.GetLeftTop(), this->GetRightTop() - rhs.GetRightTop(), this->GetLeftBot() - rhs.GetLeftBot(), this->GetRightBot() - rhs.GetRightBot() };
	}
	bool Grid::operator!=(short value)
	{
		return quarter.g[0] != value
			|| quarter.g[1] != value
			|| quarter.g[2] != value
			|| quarter.g[3] != value;
		/*return this->quarter[0] != value
		|| this->quarter[1] != value
		|| this->quarter[2] != value
		|| this->quarter[3] != value;*/
	}
	bool Grid::operator==(short value)
	{
		return quarter.g[0] == value
			&& quarter.g[1] == value
			&& quarter.g[2] == value
			&& quarter.g[3] == value;
		/*return this->quarter[0] == value
		&& this->quarter[1] == value
		&& this->quarter[2] == value
		&& this->quarter[3] == value;*/
	}

	Grid& Grid::operator=(const Grid& rhs)
	{
		quarter = rhs.quarter;
		/*quarter[0] = rhs.quarter[0];
		quarter[1] = rhs.quarter[1];
		quarter[2] = rhs.quarter[2];
		quarter[3] = rhs.quarter[3];*/
		return *this;
	}

	Grid& Grid::operator=(const int & value)
	{
		quarter.g[0] = quarter.g[1] = quarter.g[2] = quarter.g[3] = static_cast<short>(value);
		//quarter[0] = quarter[1] = quarter[2] = quarter[3] = static_cast<short>(value);
		return *this;
	}

	short& Grid::operator[] (const int index)
	{
		//todo : assert
		return quarter.g[index];
	}
	const short& Grid::operator[] (const int index) const
	{
		return quarter.g[index];
	}



}//namespace HOLD