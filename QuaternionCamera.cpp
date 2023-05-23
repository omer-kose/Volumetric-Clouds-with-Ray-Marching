#include "QuaternionCamera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

QuaternionCamera::QuaternionCamera()
{
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    orientation = glm::quat(0.0, 0.0, 0.0, 1.0);
}


QuaternionCamera::QuaternionCamera(const glm::vec3& pos_in, const glm::quat& quat_in)
{
    position = pos_in;
    orientation = quat_in;
}

const glm::vec3 QuaternionCamera::getPosition() const
{
    return position;
}


const glm::vec3 QuaternionCamera::right() const
{
    return glm::vec3(glm::row(rotation(), 0));
}

const glm::vec3 QuaternionCamera::up() const
{
    return glm::vec3(glm::row(rotation(), 1));
}

const glm::vec3 QuaternionCamera::front() const
{
    return -glm::vec3(glm::row(rotation(), 2));
}


void QuaternionCamera::move(const glm::vec3 &direction, float deltaTime, float speed)
{
    position += deltaTime * speed * direction;
}


void QuaternionCamera::moveXAxis(float direction, float deltaTime, float speed)
{
    position += deltaTime * speed * direction * right();
}


void QuaternionCamera::moveYAxis(float direction, float deltaTime, float speed)
{
    position += deltaTime * speed * direction * up();
}

void QuaternionCamera::moveZAxis(float direction, float deltaTime, float speed)
{
    position += deltaTime * speed * direction * front();
}

void QuaternionCamera::rotate(float angle, glm::vec3 & axis)
{
    glm::quat rot = glm::normalize(glm::angleAxis(angle, axis));
    
    orientation = orientation * rot;
}

void QuaternionCamera::pitch(float angle)
{
    glm::quat rot = glm::normalize(glm::angleAxis(angle, right()));

    orientation = orientation * rot;
}

void QuaternionCamera::yaw(float angle)

{
    glm::quat rot = glm::normalize(glm::angleAxis(angle, up()));

    orientation = orientation * rot;
}

void QuaternionCamera::roll(float angle)
{
    glm::quat rot = glm::normalize(glm::angleAxis(angle, front()));

    orientation = orientation * rot;
}

glm::mat4 QuaternionCamera::translation() const
{
    return glm::translate(glm::mat4(1.0f), -position);
}

glm::mat4 QuaternionCamera::rotation() const
{
    return glm::toMat4(orientation);
}

glm::mat4 QuaternionCamera::view() const
{
    return rotation() * translation();
}
