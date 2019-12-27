#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>

class Vec3 {
public:
    Vec3(float x_, float y_, float z_) :
        x(x_), y(y_), z(z_) {
        mag = sqrt(x * x + y * y + z * z);
    }

    void normalize() {
        x /= mag;
        y /= mag;
        z /= mag;
    }

    float mag, x, y, z;
};

Vec3 operator -(const Vec3& lhs, const Vec3& rhs) {
    return Vec3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

Vec3 operator +(const Vec3& lhs, const Vec3& rhs) {
    return Vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

float operator *(const Vec3& lhs, const Vec3& rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

Vec3 operator *(const Vec3& lhs, float t) {
    return Vec3(t * lhs.x, t * lhs.y, t * lhs.z);
}

Vec3 operator *(float t, const Vec3& rhs) {
    return Vec3(t * rhs.x, t * rhs.y, t * rhs.z);
}

std::ostream& operator<<(std::ostream& os, Vec3 const& vec) {
    os << std::to_string(vec.x) + " " + std::to_string(vec.y) + " " + std::to_string(vec.z);
    return os;
}

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

void draw(const std::string& fn, const std::vector<std::vector<Vec3> >& picture) {
    std::ofstream output;
    output.open(fn);
    output << "P3 " << picture.size() << " " << picture[0].size() << " 255" << std::endl;
    for (int x = 0; x < picture.size(); x++)  {
        for (int y = 0; y < picture[x].size(); y++) {
            output << picture[x][y] << " ";
        }
        output << std::endl;
    }

    output.close();
}

int main() {
    // pixel coordinates
    int width = 1080;
    int height = 720;

    float pixelsToWorld = 1.0 / 720; // arbitrarily choose world to pixel conversion scale
    // note: world coordinates with +z pointed OUT of the screen
    Vec3 camera(0.0, 0.0, 0.0);
    float imageZ = -1.0;

    auto table = new Plane(
        Vec3(0.0, -.25, 0.0),
        Vec3(0.0, 1.0, 0.0),
        Vec3(255, 0, 0)
    );

    std::vector<std::shared_ptr<Object> > scene;
    scene.push_back(std::shared_ptr<Plane>(table));

    std::vector<std::vector<Vec3> > picture;
    for (int x = 0; x < width; x++) {
        std::vector<Vec3> row;
        for (int y = 0; y < height; y++) {
            const Ray ray(
                Vec3(0.0, 0.0, 0.0),
                Vec3(x * pixelsToWorld, y * pixelsToWorld, imageZ)
            );

            Vec3 color(0, 0, 0);
            float minZ = std::numeric_limits<float>::infinity();
            for (const auto& object : scene) {
                auto intersection = object->intersect(ray);
                if (intersection.second) {
                    if (intersection.first.z < minZ) {
                        minZ = intersection.first.z;
                        color.x = 255;
                    }
                }
            }
            row.push_back(color);
        }
        picture.push_back(row);
    }

    draw("result.ppm", picture);
    return 0;
}
