#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>

class Vec3 {
public:
    Vec3(float x_, float y_, float z_) :
        x(x_), y(y_), z(z_) {
        m_norm = sqrt(x * x + y * y + z * z);
    }

    float norm() {
        return m_norm;
    }

    void normalize() {
        x /= m_norm;
        y /= m_norm;
        z /= m_norm;
    }

    float m_norm, x, y, z;
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

void draw(const std::string& fn, const std::vector<std::vector<Vec3> >& picture) {
    std::ofstream output;
    output.open(fn);
    output << "P3 " << picture.size() << " " << picture[0].size() << " 255" << std::endl;
    for (int x = 0; x < picture.size(); x++)  {
        for (int y = 0; y < picture[x].size(); y++) {
            auto color = picture[x][y];
            output << std::to_string(int(color.x)) + " " + std::to_string(int(color.y)) + " " + std::to_string(int(color.z)) << " ";
        }
        output << std::endl;
    }

    output.close();
}

int main(int argc, char** argv) {
    // pixel coordinates
    int width = std::stoi(argv[1]);
    int height = std::stoi(argv[2]);

    float pixelsToWorld = 2.0 / height; // arbitrarily choose world to pixel conversion scale
    // note: world coordinates with +z pointed OUT of the screen
    Vec3 camera(0.0, 0.0, 0.0);
    float imageZ = -1.0;

    std::vector<Sphere> scene;
    scene.push_back(Sphere(Vec3(0, -0.5, -3.0), Vec3(0, 255, 0), 1.0));
    scene.push_back(Sphere(Vec3(0, 0, -4.0), Vec3(0, 0, 255), 1.0));
    scene.push_back(Sphere(Vec3(0, 3.5, -5.0), Vec3(255, 0, 0), 1.0));

    std::vector<std::vector<Vec3> > picture;
    for (int x = 0; x < width; x++) {
        std::vector<Vec3> row;
        for (int y = 0; y < height; y++) {
            float ndcX = x - width / 2.0; // 2.0 * x / width - 1;
            float ndcY = y - height / 2.0; // 2.0 * y / height - 1;
            Vec3 dir(ndcX * pixelsToWorld, ndcY * pixelsToWorld, imageZ);
            dir.normalize();
            const Ray ray(Vec3(0.0, 0.0, 0.0), dir);

            Vec3 color(0, 0, 0);
            float minZ = std::numeric_limits<float>::infinity();
            for (auto& object : scene) {
                auto intersection = object.intersect(ray);
                if (intersection.second) {
                    if (intersection.first.z < minZ) {
                        minZ = intersection.first.z;
                        color = object.color;
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
