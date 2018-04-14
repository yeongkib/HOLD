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

		Grid();
		explicit Grid(const int& sameValue);
		explicit Grid(const short& sameValue);
		explicit Grid(short LeftTop, short RightTop, short LeftBot, short RightBot);
		Grid(double grid);
		~Grid() = default;

		void SetLeftTop(const short& value);
		void SetRightTop(const short& value);
		void SetLeftBot(const short& value);
		void SetRightBot(const short& value);

		short GetLeftTop() const;
		short GetRightTop() const;
		short GetLeftBot() const;
		short GetRightBot() const;

		int GetTotal();
		bool IsAllSame();

		bool static IsAllSame(const double& grid);
		short static GetLeftTop(const double& grid);
		short static GetRightTop(const double& grid);
		short static GetLeftBot(const double& grid);
		short static GetRightBot(const double& grid);

		Grid operator+(const int& value) const;
		Grid operator+(const Grid& rhs) const;
		Grid operator-(const int& value) const;
		Grid operator-(const Grid& rhs) const;
		bool operator!=(const short& value);
		bool operator==(const short& value);
		Grid& operator=(const Grid& rhs) = default;
		Grid& operator=(const int& value);
		short& operator[] (const int& index);
		const short& operator[] (const int& index) const;
	};
}//namespace HOLD