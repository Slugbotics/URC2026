#pragma once
#include <cmath>

class Translation2d; // forward declaration

class Rotation2d {
private:
    double radians;
public:
    Rotation2d();
    Rotation2d(double radians);
    Rotation2d(double x, double y);
    Rotation2d(Translation2d translation);

    double getRadians() const;
    double getDegrees() const;
    double getRotations() const;
    double sin() const;
    double cos() const;
    double tan() const;

    Rotation2d operator+(const Rotation2d& other) const;
    Rotation2d operator-(const Rotation2d& other) const;
    Rotation2d operator*(double scalar) const;
    Rotation2d operator/(double scalar) const;

    Rotation2d rotateBy(const Rotation2d& other) const;
    Rotation2d modulus() const;
    Rotation2d invert() const;
};
