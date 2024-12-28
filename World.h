#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <cmath>

#include<Bodies.h>
#include<Liquids.h>
#include<Intersections.h>

struct World {

public:

    World() : isIntersectionThreadRunning(true) {
        gravity = FlatVector(0.0f, -9.81f);
    }

    ~World() {
    StopIntersectionThread();
    }

    const size_t BodyListSize() {
        return bodyList.size();
    }
    const size_t LiquidListSize() {
        return liquidList.size();
    }

	void AddBody(const Bodies& body) {
		bodyList.push_back(body);
	}
    void AddLiquid(const Liquids& liquid) {
        liquidList.push_back(liquid);
    }

    Bodies* GetBody(int index) {
        if (index >= 0 && index < bodyList.size()) {
            return &bodyList[index];
        }
        return nullptr;
    }
    Liquids* GetLiquid(int index) {
        if (index >= 0 && index < liquidList.size()) {
            return &liquidList[index];
        }
        return nullptr;
    }

    void IntersectionThread() {
    std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
    float deltaTime = 0.0f;

        while (isIntersectionThreadRunning) {

            std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();
            std::chrono::duration<float> timeElapsed = endTime - startTime;
            deltaTime = timeElapsed.count();
            startTime = endTime;

            for (size_t i = 0; i < bodyList.size(); i++) {
                Bodies& body = bodyList[i];

                if (!body.IsStatic) {
                    //std::lock_guard<std::mutex> lock(intersectionMutex);
                    body.Step(deltaTime, gravity);
                }
            }

            for (size_t i = 0; i < bodyList.size() - 1; i++) {
                Bodies& bodyA = bodyList[i];

                for (size_t j = i + 1; j < bodyList.size(); j++) {
                    Bodies& bodyB = bodyList[j];

                    if (bodyA.IsStatic && bodyB.IsStatic) {
                        continue;
                    }

                    FlatVector normal;
                    float depth;
                    FlatVector collisionPoint0;
                    FlatVector collisionPoint1;
                    int contactCount;
                    if (Collide(bodyA, bodyB, normal, depth)) { 
                        Intersections::FindContactPoints(bodyA, bodyB, collisionPoint0, collisionPoint1, contactCount);
                        if (bodyA.IsStatic) {
                            bodyB.Move(normal * depth);
                        }
                        else if (bodyB.IsStatic) {
                            bodyA.Move(-normal * depth);
                        }
                        else {
                            bodyA.Move(-normal * depth / 2.0f);
                            bodyB.Move(normal * depth / 2.0f);
                        }

                        ResolveCollision(bodyA, bodyB, normal);
                    }
                }
            }  

            for (size_t i = 0; i < bodyList.size(); i++) {
                Bodies& body = bodyList[i];

                for (size_t j = 0; j < liquidList.size(); j++) {
                    Liquids& liquid = liquidList[j];

                    if (body.IsStatic) {
                        continue;
                    }
                    if (CheckIfObjectIsInsideLiquid(body, liquid)) {
                        ResolveInteractionBodyFluid(body, liquid);
                    }
                    else {
                        ResolveInteractionBodyAir(body);
                    }
                    
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }

    void StopIntersectionThread() {
        isIntersectionThreadRunning = false;
    }

private:
    std::mutex intersectionMutex;
    FlatVector gravity;
    std::vector<Bodies> bodyList;
    std::vector<Liquids> liquidList;
    bool isIntersectionThreadRunning;

    bool Collide(Bodies& bodyA, Bodies& bodyB, FlatVector& normal, float& depth) {
        
        if (bodyA.Type == Bodies::ShapeType::Polygon) {
            if (bodyB.Type == Bodies::ShapeType::Polygon) {
                return Intersections::IntersectPolygons(bodyA.Position, bodyA.Vertices, bodyB.Position, bodyB.Vertices, normal, depth);
            }
            else if (bodyB.Type == Bodies::ShapeType::Circle) {
                bool intersection = Intersections::IntersectCirclePolygon(bodyB.Position, bodyB.Radius, bodyA.Position, bodyA.Vertices, normal, depth);
                normal = -normal;
                return intersection;
            }
        }
        
        if (bodyA.Type == Bodies::ShapeType::Circle) {
            if (bodyB.Type == Bodies::ShapeType::Polygon) {
                return Intersections::IntersectCirclePolygon(bodyA.Position, bodyA.Radius, bodyB.Position, bodyB.Vertices, normal, depth);
                
            }
            else if (bodyB.Type == Bodies::ShapeType::Circle) {
                return Intersections::CircleIntersection(bodyA.Position, bodyA.Radius, bodyB.Position, bodyB.Radius, normal, depth);
            }
        }

        return false;
    }

    bool CheckIfObjectIsInsideLiquid(Bodies& body, Liquids& liquid) {
        if (body.Type == Bodies::ShapeType::Circle) {
            return Intersections::IntersectLiquidCircle(body.Position, body.Radius, liquid.FluidBoundries);
        }
    }
    
    void ResolveCollision(Bodies& bodyA, Bodies& bodyB, FlatVector& normal) {
        FlatVector relativeVelocity = bodyB.GetlinearVelocity() - bodyA.GetlinearVelocity();

        if (FlatVector::Dot(relativeVelocity, normal) > 0.0f) {
            return;
        }

        float e = std::min(bodyA.Restitution, bodyB.Restitution);
        
        float j = -(1.0f + e) * FlatVector::Dot(relativeVelocity, normal);
        j /= bodyA.InvMass + bodyB.InvMass;
        
        FlatVector impulse = j * normal;

        std::lock_guard<std::mutex> lock(intersectionMutex);
        bodyA.SetlinearVelocity(bodyA.GetlinearVelocity() - impulse * bodyA.InvMass) ;
        bodyB.SetlinearVelocity(bodyB.GetlinearVelocity() + impulse * bodyB.InvMass);
    }

    void ResolveInteractionBodyAir(Bodies& body) {
        if (body.Type == Bodies::ShapeType::Circle) {
            float volume = (4.0f / 3.0f) * 3.1416 * pow(body.Radius, 3);
            float sphereResistanceCoefficient = 0.47;
            FlatVector bodyLinearVelocity = body.GetlinearVelocity();

            FlatVector airResistance = -0.5 * 1.293 * sphereResistanceCoefficient * bodyLinearVelocity * FlatVector::VecLen(bodyLinearVelocity) * body.Area; // Opór powietrza

            body.LiquidDisplacement += airResistance;
        }
    }

    void ResolveInteractionBodyFluid(Bodies& body, Liquids& liquid) {
        if (body.Type == Bodies::ShapeType::Circle) {
            float volume = (4.0f / 3.0f) * 3.1416 * pow(body.Radius,3);

            float submerged = CheckHowMuchIsUnderWater(body, liquid);
            float isunder = submerged / volume;
            FlatVector archimedesForce = -liquid.Density * gravity * submerged; //wypornoœæ

            float sphereResistanceCoefficient = 0.47;
            FlatVector bodyLinearVelocity = body.GetlinearVelocity();
            float crossSectionalArea = 3.1416 * pow(body.Radius, 2);
            
            FlatVector fluidResistance = - 0.5 * bodyLinearVelocity * crossSectionalArea * sphereResistanceCoefficient * liquid.Density * FlatVector::VecLen(bodyLinearVelocity); //Opór wody

            body.LiquidDisplacement += archimedesForce + fluidResistance;
        }
    }
    float CheckHowMuchIsUnderWater(Bodies& Body, Liquids& Liquid) {
        float submergedArea = 0;
        float volume = (4.0f / 3.0f) * 3.1416 * pow(Body.Radius, 3);

        if (Body.Type == Bodies::ShapeType::Circle) {
            float height = Body.Position.y - Liquid.HighestBoundry;

            if (height > Body.Radius) {
                submergedArea = 0;
            }
            else if (height < -Body.Radius) {
                submergedArea = volume;
            }
            else {
                float absHeight = std::fabs(height);
                if (height < 0) {
                    float centralAngle = 2.0 * std::acos(absHeight / Body.Radius);
                    float triangleArea = 0.5 * Body.Radius * Body.Radius * std::sin(centralAngle);
                    float sectorArea = volume * ((6.2832 - centralAngle) / 6.2832);
                    submergedArea = sectorArea + triangleArea;
                }
                else {
                    float centralAngle = 2.0 * std::acos(absHeight / Body.Radius);
                    float triangleArea = 0.5 * Body.Radius * Body.Radius * std::sin(centralAngle);
                    float sectorArea = volume * ((centralAngle) / 6.2832);
                    submergedArea = sectorArea - triangleArea;
                }
            }
        }
        return submergedArea;
    }
};