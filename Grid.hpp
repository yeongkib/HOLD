/*****************************************************************************/
/*!
\file   Grid.hpp
\author Yeongki Baek
\par    email: yeongki.baek\@digipen.edu
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
	public:
		union uQuarter
		{
			double u_double;
			long long u_ll;
			short g[4];
			uQuarter() : u_double() {};
			uQuarter(double value) : u_double(value) {};
		};
		uQuarter quarter;
		//short quarter[4];	
		//double Total;

	public:

		//todo : how to set up position? -> use macro
		/*TilePosition pos;
		WalkPosition LeftTop;
		WalkPosition RightTop;
		WalkPosition LeftBot;
		WalkPosition RightBot;*/

		Grid();

		explicit Grid(int sameValue);

		explicit Grid(short sameValue);

		explicit Grid(short LeftTop, short RightTop = 0, short LeftBot = 0, short RightBot = 0);

		Grid(double grid);

		~Grid() {};


		void SetLeftTop(short value = 0);
		void SetRightTop(short value = 0);
		void SetLeftBot(short value = 0);
		void SetRightBot(short value = 0);

		short GetLeftTop() const;
		short GetRightTop() const;
		short GetLeftBot() const;
		short GetRightBot() const;

		//short* GetQuarters();

		int GetTotal();

		bool IsAllSame();

		bool static IsAllSame(double& grid);
		short static GetLeftTop(double& grid);
		short static GetRightTop(double& grid);
		short static GetLeftBot(double& grid);
		short static GetRightBot(double& grid);



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