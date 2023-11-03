#pragma once

#include "Camera.h"

#include "Grafix/Input/Input.h"

#include <glm/glm.hpp>

namespace Grafix
{
    class Camera final  // Support translation only for now
    {
    public:
        Camera() = default;
        Camera(float width, float height);
        virtual ~Camera() = default;

        void OnUpdate(float ts);

        void SetViewportSize(float width, float height);

        glm::vec2 GetPosition() { return m_Position; }
        void SetPosition(glm::vec2 position);

        glm::mat3 GetTranslationMatrix() const;
        const glm::mat3& GetViewMatrix() const { return m_ViewMatrix; }
    private:
        void UpdateViewMatrix();
    private:
        float m_AspectRatio = 16.0f / 9.0f;
        float m_OrthographicSize = 720.0f;

        glm::vec2 m_Position{ 0.0f, 0.0f };
        float m_MoveSpeed = 10.0f;

        glm::mat3 m_ViewMatrix;

        glm::vec2 m_LastMousePosition{ 0.0f, 0.0f };
    };
}
