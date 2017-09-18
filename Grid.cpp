/*****************************************************************************/
/*!
\file   Grid.cpp
\author Yeongki Baek
\par    email: yeongki.baek\@digipen.edu
\par    GAM400
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
	Grid::Grid() : Total(0.)/*, pos{ TilePosition(0, 0) },
						LeftTop{ WalkPosition(0, 0) },
						RightTop{ WalkPosition(0, 0) },
						LeftBot{ WalkPosition(0, 0) },
						RightBot{ WalkPosition(0, 0) }*/
	{
	};

	Grid::Grid(int sameValue)
	{
		quarter[0] = quarter[1] = quarter[2] = quarter[3] = static_cast<short>(sameValue);
	}

	Grid::Grid(short sameValue)
	{
		quarter[0] = quarter[1] = quarter[2] = quarter[3] = sameValue;
	}

	Grid::Grid(short LeftTop, short RightTop, short LeftBot, short RightBot)
	{
		quarter[0] = LeftTop;
		quarter[1] = RightTop;
		quarter[2] = LeftBot;
		quarter[3] = RightBot;
	}

	void Grid::SetLeftTop(short value)
	{
		quarter[0] = value;
	}
	void Grid::SetRightTop(short value)
	{
		quarter[1] = value;
	}
	void Grid::SetLeftBot(short value)
	{
		quarter[2] = value;
	}
	void Grid::SetRightBot(short value)
	{
		quarter[3] = value;
	}

	short Grid::GetLeftTop() const
	{
		return quarter[0];
	}
	short Grid::GetRightTop() const
	{
		return quarter[1];
	}
	short Grid::GetLeftBot() const
	{
		return quarter[2];
	}
	short Grid::GetRightBot() const
	{
		return quarter[3];
	}

	short* Grid::GetQuarters()
	{
		return *&quarter;
	}

	int Grid::GetTotal()
	{
		return static_cast<int>(quarter[0]) + static_cast<int>(quarter[1]) + static_cast<int>(quarter[2]) + static_cast<int>(quarter[3]);
	}

	bool Grid::IsAllSame()
	{
		if (quarter[0] == quarter[1]
			&& quarter[0] == quarter[2]
			&& quarter[0] == quarter[3])
			return true;

		return false;
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
		if (this->quarter[0] != value
			|| this->quarter[1] != value
			|| this->quarter[2] != value
			|| this->quarter[3] != value)
			return true;
		return false;
	}
	bool Grid::operator==(short value)
	{
		if (this->quarter[0] == value
			&& this->quarter[1] == value
			&& this->quarter[2] == value
			&& this->quarter[3] == value)
			return true;
		return false;
	}

	Grid& Grid::operator=(const Grid& rhs)
	{
		quarter[0] = rhs.quarter[0];
		quarter[1] = rhs.quarter[1];
		quarter[2] = rhs.quarter[2];
		quarter[3] = rhs.quarter[3];
		return *this;
	}

	Grid& Grid::operator=(const int & value)
	{
		quarter[0] = quarter[1] = quarter[2] = quarter[3] = static_cast<short>(value);
		return *this;
	}

	short& Grid::operator[] (const int index)
	{
		//todo : assert
		return quarter[index];
	}
	const short& Grid::operator[] (const int index) const
	{
		return quarter[index];
	}



}//namespace HOLD