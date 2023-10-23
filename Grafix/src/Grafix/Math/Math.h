#pragma once

namespace Grafix
{
    namespace Math
    {
        // ****************************************************************************************************************************************************************
        // Arithmetic
        // ****************************************************************************************************************************************************************

        template <typename T>
            requires std::is_floating_point_v<T>
        inline int Round(T value)
        {
            return (int)(value + 0.5f);
        }

        template<typename T>
            requires std::is_arithmetic_v<T>
        inline T Clamp(T value, T min, T max)
        {
            if (value < min)
                return min;

            if (value > max)
                return max;

            return value;
        }

        // ****************************************************************************************************************************************************************
        // Transformation
        // ****************************************************************************************************************************************************************

        /*
            NOTE: Matrices in glm library are column major, so the data should be stored as:
                [0][0] [1][0] [2][0] [3][0] ...
                [0][1] [1][1] [2][1] [3][1] ...
                [0][2] [1][2] [2][2] [3][2] ...
                [0][3] [1][3] [2][3] [3][3] ...
                ...    ...    ...    ...    ...
        */

        inline glm::mat3 CalcTranslationMatrix(glm::vec2 translation)
        {
            return glm::mat3{
                1.0f, 0.0f, 0.0f,
                0.0f, 1.0f ,0.0f,
                translation.x, translation.y, 1.0f
            };
        }

        inline glm::mat3 CalcRotationMatrix(float rotation)
        {
            float cosine = glm::cos(rotation);
            float sine = glm::sin(rotation);

            return glm::mat3{
                cosine, sine, 0.0f,
                -sine, cosine, 0.0f,
                0.0f, 0.0f, 1.0f
            };
        }

        inline glm::mat3 CalcScalingMatrix(glm::vec2 scale)
        {
            return glm::mat3{
                scale.x, 0.0f, 0.0f,
                0.0f, scale.y, 0.0f,
                0.0f, 0.0f, 1.0f
            };
        }

        inline glm::vec2 Transform(const glm::mat3& matrix, glm::vec2 point)
        {
            return glm::vec2(matrix * glm::vec3(point, 1.0f));
        }

        inline std::vector<glm::vec2> Transform(const glm::mat3& matrix, const std::vector<glm::vec2>& points)
        {
            std::vector<glm::vec2> transformedPoints(points);
            for (auto& point : transformedPoints)
                point = Transform(matrix, point);
            return transformedPoints;
        }
    }
}
