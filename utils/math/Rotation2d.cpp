#include "Rotation2d.hpp"
#include "Translation2d.hpp"

Rotation2d::Rotation2d(double radians) {
    this->radians = radians;
}
Rotation2d::Rotation2d(double x, double y) {
    this->radians = std::atan2(y, x);
}
Rotation2d::Rotation2d() {
    this->radians = 0.0;
}
Rotation2d::Rotation2d(Translation2d translation){
    this->radians = translation.getAngle();
}
double Rotation2d::getRadians() const {
    return this->radians;
}
double Rotation2d::getDegrees() const {
    return this->radians * (180.0 / M_PI);
}
double Rotation2d::getRotations() const {
    return this->radians / (2.0 * M_PI);
}
double Rotation2d::sin() const {
    return std::sin(this->radians);
}
double Rotation2d::cos() const {
    return std::cos(this->radians);
}
double Rotation2d::tan() const {
    return std::tan(this->radians);
}
Rotation2d Rotation2d::operator+(const Rotation2d& other) const {
    return Rotation2d(this->radians + other.radians);
}
Rotation2d Rotation2d::operator-(const Rotation2d& other) const {
    return Rotation2d(this->radians - other.radians);
}
Rotation2d Rotation2d::operator*(double scalar) const {
    return Rotation2d(this->radians * scalar);
}
Rotation2d Rotation2d::operator/(double scalar) const {
    return Rotation2d(this->radians / scalar);
}
Rotation2d Rotation2d::rotateBy(const Rotation2d& other) const {
    return Rotation2d(this->radians + other.radians);
}
Rotation2d Rotation2d::modulus() const {
    double mod_radians = std::fmod(this->radians, 2.0 * M_PI);
    if (mod_radians > M_PI) {
        mod_radians -= 2.0 * M_PI;
    }else if(mod_radians < -M_PI){
        mod_radians += 2.0 * M_PI;
    }
    return Rotation2d(mod_radians);
}
Rotation2d Rotation2d::invert() const {
    return Rotation2d(this->radians + M_PI).modulus();
}
