#pragma once

#include "vec.hpp"

class Ray {
public:
    Ray(const Vec3& point_, const Vec3& dir_) :
        point(point_), dir(dir_) {}

    Vec3 point;
    Vec3 dir;
};

class Object {
public:
    virtual std::pair<Vec3, bool> intersect(const Ray& ray) = 0;
};

class Plane : public Object {
public:
    Plane(const Vec3& point_, const Vec3& normal_, const Vec3& color_) :
        point(point_), normal(normal_), color(color_) {
        normal.normalize();
    }

    std::pair<Vec3, bool> intersect(const Ray& ray) {
        auto denom = normal * ray.dir;
        if (denom == 0) {
            return std::pair<Vec3, bool>(Vec3(-1.0, -1.0, -1.0), false);
        }
        float t = (normal * (point - ray.point)) / denom;
        return std::pair<Vec3, bool>(ray.point + ray.dir * t, true);
    }

    Vec3 point;
    Vec3 normal;
    Vec3 color;
};

class Sphere : public Object {
public:
    Sphere(const Vec3& center_, const Vec3& color_, float radius_) :
        center(center_), color(color_), radius(radius_) {}

    std::pair<Vec3, bool> intersect(const Ray& ray) {
        float minT = (center - ray.point) * ray.dir;
        float minDist = minT * minT + 2 * minT * (ray.point - center) * ray.dir + (ray.point - center) * (ray.point - center);
        Vec3 intersection = ray.point + ray.dir * minT;
        if (minDist < (radius * radius)) {
            return std::pair<Vec3, bool>(intersection, true);
        }
        return std::pair<Vec3, bool>(Vec3(-1.0, -1.0, -1.0), false);
    }

    Vec3 center;
    Vec3 color;
    float radius;
};
