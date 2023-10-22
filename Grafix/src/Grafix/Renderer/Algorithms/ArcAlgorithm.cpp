#include "pch.h"
#include "ArcAlgorithm.h"

namespace Grafix
{
    void ArcAlgorithm::Midpoint(glm::vec2 center, float radius, float angle1, float angle2, bool major, const glm::vec3& color)
    {
        if (std::abs(angle2 - angle1) < 1e-9f)
            return;

        // Preprocess the values
        const int centerX = Math::Round(center.x), centerY = Math::Round(center.y);
        const int r = Math::Round(radius);
        uint32_t colorValue = RGBToUint32(color);

        // Restrict angles to 0 <= angle < 360
        angle1 = glm::mod(angle1, 360.0f);
        angle2 = glm::mod(angle2, 360.0f);

        float beginAngle = std::min(angle1, angle2), endAngle = std::max(angle1, angle2);
        if ((!major && endAngle - beginAngle > 180.0f) || (major && endAngle - beginAngle <= 180.0f))
        {
            std::swap(beginAngle, endAngle);
            endAngle += 360.0f;  // 0 <= beginAngle < endAngle < 540
        }

        int a = 0, b = r;
        int e = 1 - r;

        while (a <= b)
        {
            SetArcPixels(centerX, centerY, a, b, beginAngle, endAngle, colorValue);

            if (e < 0)
            {
                e += 2 * a + 3;
            }
            else
            {
                e += 2 * (a - b) + 5;
                b--;
            }
            a++;
        }
    }

    void ArcAlgorithm::SetArcPixels(int centerX, int centerY, int x, int y, float beginAngle, float endAngle, uint32_t colorValue)
    {
        if (IsInRange(x, y, beginAngle, endAngle))
            SetPoint(centerX + x, centerY + y, colorValue);
        if (IsInRange(y, x, beginAngle, endAngle))
            SetPoint(centerX + y, centerY + x, colorValue);

        if (IsInRange(-x, y, beginAngle, endAngle))
            SetPoint(centerX - x, centerY + y, colorValue);
        if (IsInRange(-y, x, beginAngle, endAngle))
            SetPoint(centerX - y, centerY + x, colorValue);

        if (IsInRange(-x, -y, beginAngle, endAngle))
            SetPoint(centerX - x, centerY - y, colorValue);
        if (IsInRange(-y, -x, beginAngle, endAngle))
            SetPoint(centerX - y, centerY - x, colorValue);

        if (IsInRange(x, -y, beginAngle, endAngle))
            SetPoint(centerX + x, centerY - y, colorValue);
        if (IsInRange(y, -x, beginAngle, endAngle))
            SetPoint(centerX + y, centerY - x, colorValue);

        UpdateLinePatternIndex();
    }

    bool ArcAlgorithm::IsInRange(int x, int y, float beginAngle, float endAngle)
    {
        float angle = glm::degrees(glm::atan((float)y, (float)x));
        if (y < 0)
            angle += 360.0f;

        if (360.0f + angle <= endAngle)
            angle += 360.0f;

        return angle >= beginAngle && angle <= endAngle;
    }
}