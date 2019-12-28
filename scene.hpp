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
        float a = 1.0;
        float b = 2 * (ray.point - center) * ray.dir;
        float c = (ray.point - center) * (ray.point - center) - radius * radius;
        float det = b * b - 4 * a * c;

        if (det < 0) {
            // imaginary solution corresponds to no intersection with sphere
            return std::pair<Vec3, bool>(Vec3(-1.0, -1.0, -1.0), false);
        }

        float minT = (-b - sqrt(det)) / (2 * a);
        if (minT < 0) {
            // negative solution corresponds to intersection behind the ray origin
            return std::pair<Vec3, bool>(Vec3(-1.0, -1.0, -1.0), false);
        }

        Vec3 intersection = ray.point + ray.dir * minT;
        return std::pair<Vec3, bool>(intersection, true);
    }

    Vec3 center;
    Vec3 color;
    float radius;
};

enum LightType {
    POINT,
    DIRECTIONAL,
    FILL
};

class Light {
public:
    Light(const Vec3& pos_, LightType type_) :
        pos(pos_), type(type_) {}

    Vec3 pos;
    LightType type;
};
