#pragma once

#include <vector>

#include <Vector.h>
#include <cmath>

class Liquids {
public:
	float Density, Viscosity, Tension;
	std::vector<FlatVector> FluidBoundries;
	float HighestBoundry;
	enum LiquidType {
		Water = 0
	};
	LiquidType LType;

	Liquids(float Density, float Viscosity, float Tension, std::vector<FlatVector> FluidBoundries, LiquidType LType, float HighestBoundry) : 
		Density(Density), Viscosity(Viscosity), Tension(Tension), FluidBoundries(FluidBoundries), LType(LType), HighestBoundry(HighestBoundry){}

	static Liquids CreateBodyOfWater(std::vector<FlatVector> FluidBoundries) {
		float densityOfWater = 997; // [kg/m^3]
		float tension = 72; // [mN/m]
		float HighestBoundry = FindHighestBoundry(FluidBoundries);
		return Liquids(densityOfWater, 0, tension, FluidBoundries, LiquidType::Water, HighestBoundry);
	}

private:
	static float FindHighestBoundry(std::vector<FlatVector> FluidBoundries) {
		float HighestBoundry = 0;
		for (int i = 0; i < FluidBoundries.size(); i++) {
			if (HighestBoundry < FluidBoundries[i].y) HighestBoundry = FluidBoundries[i].y;
		}
		return HighestBoundry;
	}
};