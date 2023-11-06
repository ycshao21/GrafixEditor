#include "Fish.h"

std::vector<Grafix::CurveComponent> Fish::s_Body;
std::vector<Grafix::LineComponent>  Fish::s_Tail, Fish::s_Gill;
Grafix::CircleComponent             Fish::s_Eye;

void Fish::Init()
{
    glm::vec3 color = { 0.681f, 0.635f, 0.455f };
    // Body
    {
        s_Body.resize(2);
        for (auto& body : s_Body)
        {
            body.LineWidth = 2.0f;
            body.Color = color;
        }

        s_Body[0].ControlPoints = {
            {-0.8f, 0.0f},
            {-0.4f, 0.6f},
            { 0.3f, 0.1f},
        };

        s_Body[1].ControlPoints = {
            { 0.3f, -0.1f},
            {-0.4f, -0.6f},
            {-0.8f,  0.0f},
        };
    }

    // Eye
    {
        s_Eye.Center = { -0.6f,0.0f };
        s_Eye.Radius = 0.05f;
        s_Eye.LineWidth = 1.0f;
        s_Eye.Color = glm::vec3(0.8f);
    }

    // Gill
    {
        s_Gill.resize(2);
        s_Gill[0].P0 = { -0.5f, 0.27f };
        s_Gill[0].P1 = { -0.3f, 0.0f };

        s_Gill[1].P0 = { -0.3f, 0.0f };
        s_Gill[1].P1 = { -0.5f, -0.27f };

        for (auto& gill : s_Gill)
        {
            gill.LineWidth = 2.0f;
            gill.Color = color;
        }
    }

    // Tail
    {
        s_Tail.resize(3);
        s_Tail[0].P0 = { 0.3f, 0.1f };
        s_Tail[0].P1 = { 0.6f, 0.3f };

        s_Tail[1].P0 = { 0.6f, -0.3f };
        s_Tail[1].P1 = { 0.3f, -0.1f };

        s_Tail[2].P0 = { 0.6f,  0.3f };
        s_Tail[2].P1 = { 0.6f, -0.3f };

        for(auto& tail: s_Tail)
        {
            tail.LineWidth = 2.0f;
            tail.Color = color;
        }
    }
}

void Fish::OnUpdate(float ts)
{
    m_Transform.Translation += m_Velocity * ts;
}

void Fish::OnRender()
{
    for(auto& body: s_Body)
        Grafix::Renderer::DrawCurve(m_Transform, body);

    Grafix::Renderer::DrawCircle(m_Transform, s_Eye);

    for(auto& gill: s_Gill)
        Grafix::Renderer::DrawLine(m_Transform, gill);

    for(auto& tail: s_Tail)
        Grafix::Renderer::DrawLine(m_Transform, tail);
}
