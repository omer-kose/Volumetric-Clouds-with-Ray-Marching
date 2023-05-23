#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Utilities.h"

#ifndef QuaternionCamera_h
#define QuaternionCamera_h

class QuaternionCamera
{
public:
    QuaternionCamera();
    QuaternionCamera(const glm::vec3& pos_in, const glm::quat& quat_in);
    
    //Getters
    const glm::vec3 getPosition() const;
    const glm::vec3 right() const;
    const glm::vec3 up() const;
    const glm::vec3 front() const; //gaze
    
    
    //Transformations
    //Translation
    //Global move by a direction vector
    void move(const glm::vec3& direction, float deltaTime, float speed);
    //Camera relative movement
    //direction -1 or 1
    void moveXAxis(float direction, float deltaTime, float speed);
    void moveYAxis(float direction, float deltaTime, float speed);
    void moveZAxis(float direction, float deltaTime, float speed);
    //Rotation
    void rotate(float angle, glm::vec3& axis);
    //Camera-Axis relative rotations
    void pitch(float angle);
    void yaw(float angle);
    void roll(float angle);
    
    //Matrices
    glm::mat4 translation() const;
    glm::mat4 rotation() const;
    glm::mat4 view() const;
    
private:
    glm::vec3 position;
    glm::quat orientation;
};


#endif /* QuaternionCamera_h */
