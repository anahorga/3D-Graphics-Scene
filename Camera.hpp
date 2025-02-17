#ifndef Camera_hpp
#define Camera_hpp

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/euler_angles.hpp"

namespace gps {

    enum MOVE_DIRECTION { MOVE_FORWARD, MOVE_BACKWARD, MOVE_RIGHT, MOVE_LEFT, MOVE_UP, MOVE_DOWN };

    class Camera
    {
    public:
        Camera() = default;
        Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp);
        glm::mat4 getViewMatrix();
        void move(MOVE_DIRECTION direction, float speed);
        void rotate(float pitch, float yaw);
        glm::vec3 getCameraPosition() const;
        glm::vec3 getCameraTarget() const;
        glm::vec3 getCameraFrontDirection() const;
        glm::vec3 getCameraRightDirection() const;
        glm::vec3 getCameraUpDirection() const;

        glm::vec3 getOriginalCameraUpDirection() const;
        glm::vec3 getOriginalCameraFrontDirection() const;

        void setCamera(glm::vec3 cameraPosition,
            glm::vec3 cameraTarget,
            glm::vec3 cameraFrontDirection,
            glm::vec3 cameraRightDirection,
            glm::vec3 cameraUpDirection,
            glm::vec3 originalCameraUpDirection,
            glm::vec3 originalCameraFrontDirection);
    private:
        glm::vec3 cameraPosition;
        glm::vec3 cameraTarget;
        glm::vec3 cameraFrontDirection;
        glm::vec3 cameraRightDirection;
        glm::vec3 cameraUpDirection;

        glm::vec3 originalCameraUpDirection;
        glm::vec3 originalCameraFrontDirection;
    };

}

#endif