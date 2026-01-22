#pragma once
#include <cmath>

class Rotation2d; // forward declaration

class Translation2d {
private:
    double x;
    double y;
    
public:
    Translation2d();
    Translation2d(double x, double y);
    Translation2d(double magnitude, Rotation2d direction);

    Translation2d operator+(const Translation2d& other) const;
    Translation2d operator-(const Translation2d& other) const;
    Translation2d operator*(double scalar) const;
    Translation2d operator/(double scalar) const;

    double norm() const;
    Translation2d normalized() const;

    double getX() const;
    double getY() const;
    double getAngle() const;

    Translation2d rotateBy(double angle) const;
    double getDistance(const Translation2d& other) const;
};
