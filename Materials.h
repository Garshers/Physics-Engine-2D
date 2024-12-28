#pragma once

#include<vector>
	class Materials {
	private:

	public:
		float Density;
		std::vector<float> Color;
		enum MaterialType {
			Birch,
			Steel,
			Oak,
			Glass,
			Aluminum
		};
		MaterialType MType;

		Materials(float Density, std::vector<float> Color, MaterialType MType) : Density(Density), Color(Color), MType(MType) {}

		static Materials CreateBirch() {
			float red = 222.0f / 255.0f;
			float green = 184.0f / 255.0f;
			float blue = 135.0f / 255.0f;
			std::vector<float> color = { red, green, blue };
			return Materials(610.0f, color, MaterialType::Birch);
		}

		static Materials CreateSteel() {
			float Red = 70.0f / 255.0f;
			float Green = 130.0f / 255.0f;
			float Blue = 180.0f / 255.0f;
			std::vector<float> steelColor = { Red, Green, Blue };
			return Materials(7850.0f, steelColor, MaterialType::Steel);
		}

		static Materials CreateOak() {
			float red = 139.0f / 255.0f;
			float green = 69.0f / 255.0f;
			float blue = 19.0f / 255.0f;
			std::vector<float> color = { red, green, blue };
			return Materials(710.0f, color, MaterialType::Oak);
		}

		static Materials CreateGlass() {
			float red = 0.0f / 255.0f;
			float green = 191.0f / 255.0f;
			float blue = 255.0f / 255.0f;
			std::vector<float> color = { red, green, blue };
			return Materials(2500.0f, color, MaterialType::Glass);
		}

		static Materials CreateAluminum() {
			float red = 176.0f / 255.0f;
			float green = 196.0f / 255.0f;
			float blue = 222.0f / 255.0f;
			std::vector<float> color = { red, green, blue };
			return Materials(2700.0f, color, MaterialType::Aluminum);
		}
	};