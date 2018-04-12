/*****************************************************************************/
/*!
\file   Shapes.hpp
\author Yeongki Baek
\par    email: yeongki.baek\@digipen.edu
\date   08/01/2017
\brief
This is the interface file for the module
Copyright 2017, Digipen Institute of Technology
*/
/*****************************************************************************/
#pragma once

#include "Reals.hpp"
#include "Vector2.hpp"

namespace HOLD
{
	using Math::Vector2;

	class Box
	{
	public:
		Box();
		Box(const Vector2& min, const Vector2& max);

		Vector2 GetMin() const;
		Vector2 GetMax() const;
		Vector2 GetCenter() const;
		Vector2 GetHalfSize() const;

		Vector2 mMin;
		Vector2 mMax;
	};

	class Ray
	{
	public:
		Ray();
		Ray(Math::Vec2Param start, Math::Vec2Param dir);

		// Returns the a point at the given t value.
		Vector2 GetPoint(float t) const;

		Vector2 mStart;
		Vector2 mDirection;
	};
} ///namespace HOLD