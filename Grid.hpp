/*****************************************************************************/
/*!
\file   Grid.hpp
\author Yeongki Baek
\par    email: yeongki.baek\@digipen.edu
\par    GAM400
\date   08/01/2017
\brief
This is the interface file for the module
Copyright 2017, Digipen Institute of Technology
*/
/*****************************************************************************/
#pragma once

namespace HOLD
{

	class Grid
	{
		/*
		* Topleft is the original position of Grid
		*  +--+--+
		*  | 0| 1|
		*  |  |  |
		*  +--+--+
		*  | 2| 3|
		*  |  |  |
		*  +--+--+
		*
		*/
		short quarter[4];	
		double Total;

	public:

		//todo : how to set up position? -> use macro
		/*TilePosition pos;
		WalkPosition LeftTop;
		WalkPosition RightTop;
		WalkPosition LeftBot;
		WalkPosition RightBot;*/

		Grid();

		Grid(int sameValue = 0);

		Grid(short sameValue = 0);

		Grid(short LeftTop = 0, short RightTop = 0, short LeftBot = 0, short RightBot = 0);

		void SetLeftTop(short value = 0);
		void SetRightTop(short value = 0);
		void SetLeftBot(short value = 0);
		void SetRightBot(short value = 0);

		short GetLeftTop() const;
		short GetRightTop() const;
		short GetLeftBot() const;
		short GetRightBot() const;

		short* GetQuarters();

		int GetTotal();

		bool IsAllSame();



		//warn : is this best?
		Grid Grid::operator+(const int & value) const;
		Grid Grid::operator+(const Grid & rhs) const;
		Grid Grid::operator-(const int & value) const;
		Grid Grid::operator-(const Grid & rhs) const;
		bool Grid::operator!=(short value);
		bool Grid::operator==(short value);

		Grid& operator=(const Grid& rhs);

		Grid& operator=(const int & value);
		short& Grid::operator[] (const int index);
		const short& operator[] (const int index) const;







		//todo : add converting function
	};
}//namespace HOLD