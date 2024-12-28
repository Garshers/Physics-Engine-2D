#pragma once

#include <vector>

#include <Materials.h>
#include <Vector.h>
#include <cmath>

class Bodies {
private:
	FlatVector linearVelocity, linearVelocitySquered;
	float rotation, rotationalVelocity;

public:
	FlatVector force;
	FlatVector Position, FirstVertex, LiquidDisplacement;
	std::vector<FlatVector> Vertices;
	const bool IsStatic;
	const float Radius, Area;
	float InvMass;
	const float Restitution, Density, Mass;
	int NumberOfVertices;
	FlatVector ColissionPoint0;
	FlatVector ColissionPoint1;
	int ContactCount;
	Materials Material;
	enum ShapeType {
		Circle = 0,
		Polygon = 1
	};
	ShapeType Type;

	const FlatVector& GetlinearVelocity() const {
		return linearVelocity;
	}
	const FlatVector& GetlinearVelocitySquered() const {
		return linearVelocitySquered;
	}
	void SetlinearVelocity(const FlatVector& newVelocity) {
		linearVelocity = newVelocity;
	}

	Bodies(FlatVector& Position, int NumberOfVertices, float Radius, float Mass, float Area, float Restitution, bool IsStatic, ShapeType Type, Materials Material)
		: linearVelocity(0.0f, 0.0f),linearVelocitySquered(FlatVector::VecSquared(linearVelocity)), rotation(0.0f), rotationalVelocity(0.0f), force(0.0f, 0.0f), ContactCount(0), Position(Position), NumberOfVertices(NumberOfVertices),
		Radius(Radius), Density(Material.Density), Mass(Mass), Area(Area), Restitution(Restitution), IsStatic(IsStatic), Type(Type), Material(Material), LiquidDisplacement(FlatVector(0.0f, 0.0f)) {
		if (!IsStatic) {
			InvMass = 1.0f / Mass;
		}
		else {
			InvMass = 0.0f;
		}

		if (NumberOfVertices > 2) {
			Vertices.reserve(NumberOfVertices);
			Vertices = CreatePolygonVertices(NumberOfVertices, Radius);
		}
	}

	static Bodies CreateCircleBody(FlatVector& Position, float Radius, float Restitution, bool IsStatic, Materials Material) {
		float area = 3.14159265358979323846f * Radius * Radius;

		if (Radius > 100.0f || Radius < 0.001f) {
			throw std::invalid_argument("Invalid size");
		}

		return Bodies(Position, 0 ,  Radius, area * Material.Density, area , Restitution, IsStatic, ShapeType::Circle, Material);
	}
	static Bodies CreatePolygonBody(int NumberOfVertices, FlatVector& Position,float Radius, float Restitution, bool IsStatic, Materials Material) {
		float area = 0.5 * Radius * Radius * NumberOfVertices * sin(2.0 * 3.14159265358979323846f / NumberOfVertices);
		
		if (NumberOfVertices < 3) {
			throw std::invalid_argument("Invalid number of vertices");
		}
		if (Radius > 100.0f || Radius < 0.1f) {
			throw std::invalid_argument("Invalid size");
		}

		return Bodies(Position, NumberOfVertices,  Radius, area * Material.Density, area, Restitution, IsStatic, ShapeType::Polygon, Material);
	}
	static Bodies CreatePolygonBody(const FlatVector& FirstVertex, const FlatVector& SecondVertex, float Restitution, bool IsStatic, Materials Material) {
		float Width = std::fabs(FirstVertex.x - SecondVertex.x);
		float Height = std::fabs(FirstVertex.y - SecondVertex.y);
		float area = Height * Width;

		FlatVector Position;

		if (FirstVertex != SecondVertex) {
			Position = FlatVector((SecondVertex.x + FirstVertex.x) / 4, (SecondVertex.y + FirstVertex.y) / 4);
		}
		else {
			throw std::invalid_argument("Invalid vertices: FirstVertex is equal to SecondVertex");
		}

		if (area > 1500.0f || area < 1.0f) {
			throw std::invalid_argument("Invalid size");
		}

		Bodies body = Bodies(Position, 0, FlatVector::VecLen((SecondVertex - FirstVertex) / 4), area * Material.Density, area, Restitution, IsStatic, ShapeType::Polygon, Material);
		body.Vertices.reserve(4);
		body.Vertices = CreateBoxVertices(Position, Width, Height);
		return body;
	}

	static std::vector<FlatVector> CreatePolygonVertices(int NumberOfVertices, float Radius) {
		std::vector<FlatVector> vertices;
		vertices.reserve(NumberOfVertices);

		float angleIncrement = 2.0 * 3.14159265358979323846f / NumberOfVertices;

		for (int i = 0; i < NumberOfVertices; ++i) {
			float x = Radius * cos(angleIncrement * i);
			float y = Radius * sin(angleIncrement * i);
			vertices.push_back(FlatVector(x, y));
		}

		return vertices;
	}
	static std::vector<FlatVector> CreateBoxVertices(FlatVector& Position, float width, float height) {
		float halfWidth = width / 2.0f;
		float halfHeight = height / 2.0f;

		std::vector<FlatVector> vertices(4);
		vertices[0] = FlatVector(Position.x - halfWidth, Position.y + halfHeight);
		vertices[1] = FlatVector(Position.x + halfWidth, Position.y + halfHeight);
		vertices[2] = FlatVector(Position.x + halfWidth, Position.y - halfHeight);
		vertices[3] = FlatVector(Position.x - halfWidth, Position.y - halfHeight);

		return vertices;
	}

	void Move(const FlatVector& amount) {
		Position += amount;
	}
	void Rotate(float amount) {
		if (Vertices.size() > 0) {
			FlatVector center = Position;

			for (auto& vertex : Vertices) {
				float x = vertex.x - center.x;
				float y = vertex.y - center.y;
				vertex.x = center.x + (x * cos(amount) - y * sin(amount));
				vertex.y = center.y + (x * sin(amount) + y * cos(amount));
			}
		}
	}
	void Step(float time, const FlatVector& gravity) {
		if (IsStatic) return;

		FlatVector acceleration = (force + LiquidDisplacement) * InvMass;
		acceleration += gravity;

		linearVelocity += acceleration * time;

		Position += linearVelocity * time + 0.5 * acceleration * time * time * InvMass;

		rotation += rotationalVelocity * time;

		LiquidDisplacement = FlatVector(0.0f, 0.0f);
		force = FlatVector(0.0f, 0.0f);
	}
};