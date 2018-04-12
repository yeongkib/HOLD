/*****************************************************************************/
/*!
\file   Precompiled.hpp
\author Yeongki Baek
\par    email: yeongki.baek\@digipen.edu
\date   08/01/2017
\brief
This is the interface file for the module
Copyright 2017, Digipen Institute of Technology
*/
/*****************************************************************************/
#pragma once

//#include "Precompiled.hpp"
//#include <bits/stdc++.h>
#include <vector>
#include <set>
#include <list>
#include <queue>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <algorithm>
#include <math.h>
#include <numeric>
#include <array>
#include <stdarg.h>
#include <BWAPI/Game.h>
#include "Reals.hpp"
#include "Utilities.hpp"
#include "Vector2.hpp"
#include "Log.hpp"
#include "Message.hpp"

typedef int frame;

template< typename ContainerT, typename PredicateT >
void erase_if(ContainerT& items, const PredicateT& predicate) {
	for (auto it = items.begin(); it != items.end(); ) {
		if (predicate(*it)) it = items.erase(it);
		else ++it;
	}
};