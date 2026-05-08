#pragma once
#include "Translation2d.hpp"
#include "Rotation2d.hpp"

class Pose2d {
private:
    double x;
    double y;
    double angle;
public:
    Pose2d(Translation2d translation, Rotation2d rotation);
    Pose2d(double x, double y, double radians);
    Pose2d();
    Pose2d(double x, double y, Rotation2d rotation);

    Translation2d getTranslation() const;
    Rotation2d getRotation() const;
    double getX() const;
    double getY() const;
    double getAngle() const;

    Pose2d operator+(const Pose2d& other) const;
    Pose2d operator-(const Pose2d& other) const;
    Pose2d transformBy(const Pose2d& other) const;
    Pose2d translate(const Translation2d& translation) const;
    Pose2d rotateInPlace(const Rotation2d& rotation) const;
    Pose2d rotateAround(const Translation2d& center, const Rotation2d& rotation) const;
    Pose2d rotateOrigin(const Rotation2d& rotation) const;
    double getDistance(const Pose2d& other) const;
};
