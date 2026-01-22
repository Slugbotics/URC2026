#include "Translation2d.hpp"
#include "Rotation2d.hpp"

Translation2d::Translation2d(double x, double y){
    this->x = x;
    this->y = y;
}
Translation2d::Translation2d(){
    this->x = 0.0;
    this->y = 0.0;
}
Translation2d::Translation2d(double magnitude, Rotation2d direction){
    this->x = magnitude * direction.cos();
    this->y = magnitude * direction.sin();
}
Translation2d Translation2d::operator+(const Translation2d& other) const {
    return Translation2d(this->x + other.x, this->y + other.y);
}
Translation2d Translation2d::operator-(const Translation2d& other) const {
    return Translation2d(this->x - other.x, this->y - other.y);
}
Translation2d Translation2d::operator*(double scalar) const {
    return Translation2d(this->x * scalar, this->y * scalar);
}
Translation2d Translation2d::operator/(double scalar) const {
    if (scalar == 0.0) {
        return Translation2d(0.0, 0.0);
    }
    return Translation2d(this->x / scalar, this->y / scalar);
}
double Translation2d::norm() const {
    return std::hypot(this->x, this->y);
}
Translation2d Translation2d::normalized() const {
    double n = this->norm();
    if (n == 0) {
        return Translation2d(0, 0);
    }
    return Translation2d(this->x / n, this->y / n);
}
double Translation2d::getX() const {
    return this->x;
}
double Translation2d::getY() const {
    return this->y;
}
double Translation2d::getAngle() const {
    return std::atan2(this->y, this->x);
}
Translation2d Translation2d::rotateBy(double angle) const {
    double cosA = std::cos(angle);
    double sinA = std::sin(angle);
    return Translation2d(this->x * cosA - this->y * sinA, this->x * sinA + this->y * cosA);
}
double Translation2d::getDistance(const Translation2d& other) const {
    return (*this - other).norm();
}