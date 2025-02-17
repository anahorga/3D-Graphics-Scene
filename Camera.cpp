#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;

        this->cameraFrontDirection = glm::normalize(glm::vec3(cameraTarget - cameraPosition));
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, cameraUp));

        this->originalCameraUpDirection = this->cameraUpDirection;
        this->originalCameraFrontDirection = this->cameraFrontDirection;

    }
    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraTarget, cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed)
    {
        switch (direction)
        {
        case MOVE_LEFT:
            cameraPosition -= speed * cameraRightDirection;
            cameraTarget -= speed * cameraRightDirection;
            break;

        case MOVE_RIGHT:
            cameraPosition += speed * cameraRightDirection;
            cameraTarget += speed * cameraRightDirection;
            break;

        case MOVE_FORWARD:
            cameraPosition += speed * cameraFrontDirection;
            cameraTarget += speed * cameraFrontDirection;
            break;

        case MOVE_BACKWARD:
            cameraPosition -= speed * cameraFrontDirection;
            cameraTarget -= speed * cameraFrontDirection;
            break;
        case MOVE_UP:
            cameraPosition += speed * cameraUpDirection;
            cameraTarget += speed * cameraUpDirection;
            break;

        case MOVE_DOWN:
            cameraPosition -= speed * cameraUpDirection;
            cameraTarget -= speed * cameraUpDirection;
            break;
        }
        this->cameraFrontDirection = normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = normalize(cross(cameraFrontDirection, cameraUpDirection));
    }
    glm::mat3 E(float h, float p, float r)
    {
        return glm::rotate(glm::mat4(1.0f), glm::radians(h), glm::vec3(0.0f, 1.0f, 0.0f))
            * glm::rotate(glm::mat4(1.0f), glm::radians(p), glm::vec3(1.0f, 0.0f, 0.0f));
    }
    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        cameraFrontDirection = glm::normalize(E(yaw, pitch, 0.0f) * originalCameraFrontDirection);
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, originalCameraUpDirection));
        cameraUpDirection = glm::cross(cameraRightDirection, cameraFrontDirection);
        cameraTarget = cameraPosition + cameraFrontDirection;
    }

    void Camera::setCamera(
        glm::vec3 cameraPosition,
        glm::vec3 cameraTarget,
        glm::vec3 cameraFrontDirection,
        glm::vec3 cameraRightDirection,
        glm::vec3 cameraUpDirection,
        glm::vec3 originalCameraUpDirection,
        glm::vec3 originalCameraFrontDirection)
    {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraFrontDirection = cameraFrontDirection;

        this->cameraRightDirection = cameraRightDirection;
        this->cameraUpDirection = cameraUpDirection;

        this->originalCameraUpDirection = originalCameraUpDirection;
        this->originalCameraFrontDirection = originalCameraFrontDirection;
    }




}