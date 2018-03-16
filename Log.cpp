/*****************************************************************************/
/*!
\file   Log.cpp
\author Yeongki Baek
\par    email: yeongki.baek\@digipen.edu
\date   08/01/2017
\brief
This is the interface file for the module
Copyright 2017, Digipen Institute of Technology
*/
/*****************************************************************************/

#include "Precompiled.hpp"

#include <iomanip>

#ifndef NDEBUG
std::chrono::time_point<std::chrono::system_clock> performancetimer_start, performancetimer_end;

tracer::tracer(std::ostream & cout, char const * file, int line, char const * fnc)
	: cout(cout) {
	std::string outfilename = "logfile.txt";
	fout.open(outfilename, std::ios_base::out | std::ios_base::app);

	performancetimer_end = std::chrono::system_clock::now();
	cout.setf(std::ios::showpoint); // show zero points
	cout.precision(6);
	
	std::chrono::duration<double> elapsed_seconds = performancetimer_end - performancetimer_start;
	cout << std::setw(20) << file << ":" << std::setw(4) << line << ":" << std::setw(20) << fnc << "(): " << "time: " << elapsed_seconds.count() << ", ";

	fout.setf(std::ios::showpoint); // show zero points
	fout.precision(6);
	fout << std::setw(20) << file << ":" << std::setw(4) << line << ":" << std::setw(20) << fnc << "(): " << "time: " << elapsed_seconds.count() << ", ";
}
tracer::~tracer() {
	cout << std::endl;
	fout << std::endl;
	fout.close();
}

void tracer::write() {
	//handle the empty params case
}

#endif