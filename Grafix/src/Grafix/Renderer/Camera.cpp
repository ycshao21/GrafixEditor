#include "pch.h"
#include "Camera.h"

#include "Grafix/Math/Math.h"

namespace Grafix
{
    Camera::Camera(float width, float height)
    {
        SetViewportSize(width, height);
    }

    void Camera::OnUpdate(float ts)
    {
    }

    void Camera::SetViewportSize(float width, float height)
    {
        m_ViewportWidth = width;
        m_ViewportHeight = height;
        UpdateViewMatrix();
    }

    void Camera::SetPosition(glm::vec2 position)
    {
        m_Position = position;
        UpdateViewMatrix();
    }

    glm::mat3 Camera::GetTranslationMatrix() const
    {
        return Math::CalcTranslationMatrix(m_Position);
    }

    void Camera::UpdateViewMatrix()
    {
        glm::mat3 transform = Math::CalcTranslationMatrix(m_Position);
        if (m_OriginAtCenter)
        {
            glm::vec2 offset = glm::vec2(-0.5f * m_ViewportWidth, -0.5 * m_ViewportHeight);
            transform = Math::CalcTranslationMatrix(offset) * transform;
        }
        m_ViewMatrix = glm::inverse(transform);
    }
}