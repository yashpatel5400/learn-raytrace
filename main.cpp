#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>

#include "vec.hpp"
#include "scene.hpp"

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
    scene.push_back(Sphere(Vec3(-1.5, 0.5, -3.0), Vec3(255, 0, 0), 0.5));
    scene.push_back(Sphere(Vec3(1.5, 1.5, -3.0), Vec3(255, 255, 0), 1.25));

    std::vector<Light> lights;
    lights.push_back(Light(Vec3(-1.0, 0.75, -3.0), LightType::POINT));

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
            float maxZ = -std::numeric_limits<float>::infinity();
            for (auto& object : scene) {
                auto intersection = object.intersect(ray);
                if (intersection.second) {
                    if (intersection.first.z > maxZ) {
                        maxZ = intersection.first.z;
                        color = object.color;

                        for (auto& light: lights) {
                            Vec3 shadowRayDir = light.pos - intersection.first;
                            shadowRayDir.normalize();

                            Ray shadowRay(intersection.first + shadowRayDir * 10e-3, shadowRayDir);
                            for (auto& shadowObject : scene) {
                                auto shadowIntersection = shadowObject.intersect(shadowRay);
                                if (shadowIntersection.second) {
                                    color = color * 0.5;
                                }
                            }
                        }
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
