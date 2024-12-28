#pragma once[]
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Bodies.h>
#include <Vector.h>
#include <iomanip>
class Intersections {
public:

	static bool CircleIntersection(const FlatVector& centerA, float radiusA, const FlatVector& centerB, float radiusB, FlatVector& normal, float& depth) {
        normal = FlatVector(0.0f, 0.0f);
        depth = 0.0f;

        float distance = FlatVector::Distance(centerA, centerB);

        if (distance >= radiusA + radiusB) {
            return false;
        }

        float dx = centerB.x - centerA.x;
        float dy = centerB.y - centerA.y;

        if (distance != 0.0f) {
            normal.x = dx / distance;
            normal.y = dy / distance;
        }

        depth = radiusA + radiusB - distance;

        return true;
    }
    
    static bool IntersectPolygons(const FlatVector& centerA, const std::vector<FlatVector>& verticesA, const FlatVector& centerB, const std::vector<FlatVector>& verticesB, FlatVector& normal, float& depth)
    {
        normal = FlatVector();
        depth = std::numeric_limits<float>::max();
        float minA, minB, maxA, maxB;

        for (int i = 0; i < verticesA.size(); i++)
        {
            FlatVector va = verticesA[i];
            FlatVector vb = verticesA[(i + 1) % verticesA.size()];

            FlatVector edge = vb - va;
            FlatVector axis = FlatVector(-edge.y, edge.x);
            FlatVector::NormalizedVector(axis);

            ProjectVertices(centerA, verticesA, axis, minA, maxA);
            ProjectVertices(centerB, verticesB, axis, minB, maxB);

            if (minA > maxB || minB > maxA)
            {
                return false;
            }

            float axisDepth = std::min(maxB - minA, maxA - minB);

            if (axisDepth < depth)
            {
                depth = axisDepth;
                normal = axis;
            }

        }
        
        for (int i = 0; i < verticesB.size(); i++)
        {
            FlatVector va = verticesB[i];
            FlatVector vb = verticesB[(i + 1) % verticesB.size()];

            FlatVector edge = vb - va;
            FlatVector axis = FlatVector(-edge.y, edge.x);
            FlatVector::NormalizedVector(axis);

            ProjectVertices(centerA, verticesA, axis, minA, maxA);
            ProjectVertices(centerB, verticesB, axis, minB, maxB);

            if (minA > maxB || minB > maxA)
            {
                return false;
            }

            float axisDepth = std::min(maxB - minA, maxA - minB);

            if (axisDepth < depth)
            {
                depth = axisDepth;
                normal = axis;  
            }
        }

        FlatVector centerBtoA = centerB - centerA;
        if (FlatVector::Dot(normal*depth, centerBtoA) > 0)
        {
            normal = -normal;
        }
        
        return true;
    }

    static bool IntersectCirclePolygon(const FlatVector circleCenter, const float circleRadius, const FlatVector polygonCenter, const std::vector<FlatVector> vertices, FlatVector& normal, float& depth)
    {
        normal = FlatVector();
        depth = std::numeric_limits<float>::max();

        float axisDepth = 0.0f;
        float minA, maxA, minB, maxB;

        for (int i = 0; i < vertices.size(); i++)
        {
            FlatVector va = vertices[i];
            FlatVector vb = vertices[(i + 1) % vertices.size()];

            FlatVector edge = vb - va;
            FlatVector axis = FlatVector(-edge.y, edge.x);
            FlatVector::NormalizedVector(axis);

            ProjectVertices(polygonCenter, vertices, axis, minA, maxA);
            ProjectCircle(circleCenter, circleRadius, axis, minB, maxB);

            if (minA > maxB || minB > maxA)
            {
                return false;
            }

            axisDepth = std::min(maxB - minA, maxA - minB);

            if (axisDepth < depth)
            {
                depth = axisDepth;

                if (axisDepth == maxB - minA)
                {
                    normal = axis;
                }
                else
                {
                    FlatVector closestPoint = va + polygonCenter;
                    FlatVector vertexNormal = closestPoint - circleCenter;
                    FlatVector::NormalizedVector(vertexNormal);
                    normal = vertexNormal;
                }
            }
        }
        
        int cpIndex = FindClosestPointOnPolygon(circleCenter, vertices);
        FlatVector closestPoint = vertices[cpIndex] + polygonCenter;

        FlatVector axis = closestPoint - circleCenter;
        FlatVector::NormalizedVector(axis);

        ProjectVertices(polygonCenter, vertices, axis, minA, maxA);
        ProjectCircle(circleCenter, circleRadius, axis, minB, maxB);

        if (minA > maxB || minB > maxA)
        {
            return false;
        }

        axisDepth = std::min(maxB - minA, maxA - minB);

        if (axisDepth < depth)
        {
            depth = axisDepth;
            normal = axis;
        }
        
        return true;
    }

    static bool IntersectLiquidCircle(const FlatVector& circleCenter, const float circleRadius, const std::vector<FlatVector>& boundaries) {
        float closestX = std::max(boundaries[0].x, std::min(circleCenter.x, boundaries[1].x));
        float closestY = std::max(boundaries[2].y, std::min(circleCenter.y, boundaries[1].y));

        float distanceX = circleCenter.x - closestX;
        float distanceY = circleCenter.y - closestY;
        float distanceSquared = distanceX * distanceX + distanceY * distanceY;

        return distanceSquared < (circleRadius * circleRadius);
    }

    static void FindContactPoints(const Bodies& bodyA, const Bodies& bodyB, FlatVector& collisionPoint0, FlatVector& collisionPoint1, int& contactCount)
    {
        collisionPoint0 = FlatVector();
        collisionPoint1 = FlatVector();
        contactCount = 0;

        if (bodyA.Type == Bodies::ShapeType::Polygon)
        {
            if (bodyB.Type == Bodies::ShapeType::Polygon) {
                FindContactPoint(bodyA.Position, bodyA.Vertices, bodyB.Position, bodyB.Vertices, collisionPoint0, collisionPoint1, contactCount);
            }
            else if (bodyB.Type == Bodies::ShapeType::Circle) {
                FindContactPoint(bodyB.Position, bodyB.Radius, bodyA.Position, bodyA.Vertices, collisionPoint0);
                contactCount = 1;
            }
        }

        if (bodyA.Type == Bodies::ShapeType::Circle)
        {
            if (bodyB.Type == Bodies::ShapeType::Polygon) {
                FindContactPoint(bodyA.Position, bodyA.Radius, bodyB.Position, bodyB.Vertices, collisionPoint0);
                contactCount = 1;
            }
            else if (bodyB.Type == Bodies::ShapeType::Circle) {
                FindContactPoint(bodyA.Position, bodyA.Radius, bodyB.Position, collisionPoint0);
                contactCount = 1;
            }
        }
    }

private:

    static void FindContactPoint(const FlatVector& centerA, float radiusA, const FlatVector& centerB, FlatVector& collisionPoint)
    {
        FlatVector VecAtoB = centerB - centerA;
        FlatVector::NormalizedVector(VecAtoB);
        collisionPoint = centerA + (VecAtoB * radiusA);
    }

    static void FindContactPoint(const FlatVector& circleCenter, float radius, const FlatVector& polygonCenter, const std::vector<FlatVector> vertices, 
        FlatVector& collisionPoint)
    {
        float distanceSquered, minDistanceSquered= std::numeric_limits<float>::min();
        FlatVector ContactPoint;
        for (int i = 0; i < vertices.size(); i++)
        {
            FlatVector va = vertices[i];
            FlatVector vb = vertices[(i + 1) % vertices.size()];

            VectorPointDistance(circleCenter, va, vb, distanceSquered, ContactPoint);

            if (distanceSquered < minDistanceSquered) {
                minDistanceSquered = distanceSquered;
                collisionPoint = ContactPoint;
            }
        }
    }

    static void FindContactPoint(const FlatVector& polygonCenterA, const std::vector<FlatVector> verticesA, const FlatVector& polygonCenterB, const std::vector<FlatVector> verticesB,
        FlatVector& collisionPoint0, FlatVector& collisionPoint1, int& contactCount)
    {
        contactCount = 0;
        float distanceSquered, minDistanceSquered = std::numeric_limits<float>::min();
        FlatVector ContactPoint;

        for (int i = 0; i < verticesA.size(); i++) {
            FlatVector p = verticesA[i];

            for (int j = 0; j < verticesB.size(); j++) {
                FlatVector va = verticesB[j];
                FlatVector vb = verticesB[(j + 1) % verticesB.size()];

                VectorPointDistance(p, va, vb, distanceSquered, ContactPoint);
                 
                if (FlatVector::NearlyEqual(distanceSquered,minDistanceSquered)) {
                    if (!FlatVector::DistanceSquared(collisionPoint0,collisionPoint1)) {
                        collisionPoint1 = ContactPoint;
                        contactCount = 2;
                    }
                }
                if (distanceSquered < minDistanceSquered) {
                    minDistanceSquered = distanceSquered;
                    collisionPoint0 = ContactPoint;
                    contactCount = 1;
                }
                if (contactCount = 2) return;
            }
        }

        for (int i = 0; i < verticesB.size(); i++) {
            FlatVector p = verticesB[i];

            for (int j = 0; j < verticesA.size(); j++) {
                FlatVector va = verticesA[j];
                FlatVector vb = verticesA[(j + 1) % verticesA.size()];

                VectorPointDistance(p, va, vb, distanceSquered, ContactPoint);

                if (FlatVector::NearlyEqual(distanceSquered, minDistanceSquered)) {
                    if (!FlatVector::DistanceSquared(collisionPoint0, collisionPoint1)) {
                        collisionPoint1 = ContactPoint;
                        contactCount = 2;
                    }
                }
                if (distanceSquered < minDistanceSquered) {
                    minDistanceSquered = distanceSquered;
                    collisionPoint0 = ContactPoint;
                    contactCount = 1;
                }
                if (contactCount = 2) return;
            }
        }
    }

    static void ProjectVertices(const FlatVector& center, const std::vector<FlatVector>& vertices, const FlatVector& axis, float& min, float& max)
    {
        min = std::numeric_limits<float>::max();
        max = std::numeric_limits<float>::min();

        for (const auto& v : vertices) {
            FlatVector vertex = v + center;
            float proj = FlatVector::Dot(vertex, axis);
            min = std::min(proj, min);
            max = std::max(proj, max);
        }
    }
    static void ProjectCircle(const FlatVector& center, const float& radius, const FlatVector& axis, float& min, float& max)
    {
        min = std::numeric_limits<float>::max();
        max = std::numeric_limits<float>::min();

        FlatVector directionAndRadius = axis * radius;

        FlatVector p1 = center + directionAndRadius;
        FlatVector p2 = center - directionAndRadius;

        min = FlatVector::Dot(p1, axis);
        max = FlatVector::Dot(p2, axis);

        if (max < min) {
            float t = min;
            min = max;
            max = t;
        }
    }

    static void VectorPointDistance(const FlatVector& point, const FlatVector& firstPoint, const FlatVector& secondPoint, float& distanceSquered, FlatVector& collisionPoint){
        FlatVector Vector1p2p = secondPoint - firstPoint;
        FlatVector Vector = point - firstPoint;

        float proj = FlatVector::Dot(Vector, Vector1p2p);
        float disSq1p2p = FlatVector::DistanceSquared(Vector1p2p);
        float d = proj/ disSq1p2p;

        if (d <= 0) {
            collisionPoint = firstPoint;
        }
        else if (d >= 0) {
            collisionPoint = secondPoint;
        }
        else {
            collisionPoint = firstPoint + Vector1p2p * d;
        }
        distanceSquered = FlatVector::DistanceSquared(collisionPoint, point);
    }

    static int FindClosestPointOnPolygon(const FlatVector circleCenter, const std::vector<FlatVector> vertices)
    {
        int result = -1;
        float minDistance = std::numeric_limits<float>::max();

        for (int i = 0; i < vertices.size(); i++)
        {
            float distance = FlatVector::Distance(vertices[i], circleCenter);

            if (distance < minDistance)
            {
                minDistance = distance;
                result = i;
            }
        }

        return result;
    }

};