#pragma once

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
