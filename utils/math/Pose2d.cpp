#include "Pose2d.hpp"

Pose2d::Pose2d(Translation2d translation, Rotation2d rotation) {
    this->x = translation.getX();
    this->y = translation.getY();
    this->angle = rotation.getRadians();
}
Pose2d::Pose2d(double x, double y, double radians) {
    this->x = x;
    this->y = y;
    this->angle = radians;
}
Pose2d::Pose2d() {
    this->x = 0.0;
    this->y = 0.0;
    this->angle = 0.0;
}
Pose2d::Pose2d(double x, double y, Rotation2d rotation) {
    this->x = x;
    this->y = y;
    this->angle = rotation.getRadians();
}
Translation2d Pose2d::getTranslation() const {
    return Translation2d(this->x, this->y);
}
Rotation2d Pose2d::getRotation() const {
    return Rotation2d(this->angle);
}
double Pose2d::getX() const {
    return this->x;
}
double Pose2d::getY() const {
    return this->y;
}
double Pose2d::getAngle() const {
    return this->angle;
}
Pose2d Pose2d::operator+(const Pose2d& other) const {
    Translation2d new_translation = this->getTranslation() + other.getTranslation().rotateBy(this->angle);
    Rotation2d new_rotation = this->getRotation() + other.getRotation();
    return Pose2d(new_translation, new_rotation.modulus());
}
Pose2d Pose2d::operator-(const Pose2d& other) const {
    Translation2d delta_translation = other.getTranslation() - this->getTranslation();
    Translation2d rotated_translation = delta_translation.rotateBy(-this->angle);
    Rotation2d delta_rotation = other.getRotation() - this->getRotation();
    return Pose2d(rotated_translation, delta_rotation.modulus());
}
Pose2d Pose2d::transformBy(const Pose2d& other) const {
    return (*this) + other;
}
Pose2d Pose2d::translate(const Translation2d& translation) const {
    Translation2d new_translation = this->getTranslation() + translation;
    return Pose2d(new_translation, this->getRotation());
}
Pose2d Pose2d::rotateInPlace(const Rotation2d& rotation) const {
    Rotation2d new_rotation = this->getRotation() + rotation;
    return Pose2d(this->x, this->y, new_rotation.modulus());
}
Pose2d Pose2d::rotateAround(const Translation2d& center, const Rotation2d& rotation) const {
    Translation2d translated_point = this->getTranslation() - center;
    Translation2d rotated_point = translated_point.rotateBy(rotation.getRadians());
    Translation2d final_point = rotated_point + center;
    Rotation2d new_rotation = this->getRotation() + rotation;
    return Pose2d(final_point, new_rotation.modulus());
}
Pose2d Pose2d::rotateOrigin(const Rotation2d& rotation) const {
    return this->rotateAround(Translation2d(0.0, 0.0), rotation);
}
double Pose2d::getDistance(const Pose2d& other) const {
    return this->getTranslation().getDistance(other.getTranslation());
}
