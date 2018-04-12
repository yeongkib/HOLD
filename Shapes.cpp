/*****************************************************************************/
/*!
\file   Shapes.cpp
\author Yeongki Baek
\par    email: yeongki.baek\@digipen.edu
\date   08/01/2017
\brief
This is the interface file for the module
Copyright 2017, Digipen Institute of Technology
*/
/*****************************************************************************/
#include "Precompiled.hpp"
#include "Shapes.hpp"

namespace HOLD
{
	Box::Box() {
		//set the aabb to an initial bad value (where the min is smaller than the max)
		/*mMin.Splat(Math::PositiveMax());
		mMax.Splat(-Math::PositiveMax());*/
		mMin.x = Math::PositiveMax();
		mMin.y = Math::PositiveMin();
		mMax.x = Math::PositiveMax();
		mMax.y = Math::PositiveMin();
	}

	Box::Box(const Vector2& min, const Vector2& max) {
		mMin = min;
		mMax = max;
	}

	Vector2 Box::GetMin() const {
		return mMin;
	}

	Vector2 Box::GetMax() const {
		return mMax;
	}

	Vector2 Box::GetCenter() const {
		return (mMin + mMax) * 0.5f;
	}

	Vector2 Box::GetHalfSize() const {
		return (mMax - mMin) * 0.5f;
	}

	Ray::Ray() {
		mStart = mDirection = Vector2::cZero;
	}

	Ray::Ray(Math::Vec2Param start, Math::Vec2Param dir) {
		mStart = start;
		mDirection = dir;
	}

	Vector2 Ray::GetPoint(float t) const {
		return mStart + mDirection * t;
	}
}// namespace HOLD