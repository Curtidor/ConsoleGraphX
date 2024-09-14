#pragma once
#include "vector3.h"
#include "component.h"

namespace ConsoleGraphX
{
    using TransformID = size_t;

    struct Transform : public ConsoleGraphX_Internal::Component
    {
    public:
        TransformID m_parent;
        Vector3 m_position;
        Vector3 m_scale;
        float m_rotation;

        Transform();
        Transform(const Transform& tansform);
        Transform(float x, float y, float scaleX, float scaleY);
        Transform(float x, float y, float z, float scaleX, float scaleY, float scaleZ);

        void SetParent(TransformID parent);
        Transform* GetParent() const;

        void Clone(Transform* transform) const;

        void SetPosition(float x, float y);
        void SetPosition(float x, float y, float z);
        void SetPosition(const Vector3& position);

        void SetScale(float x, float y);
        void SetScale(float x, float y, float z);

        void Translate(const Vector3& translation);

        const Vector3& GetLocalPosition() const;
        const Vector3 GetWorldPosition() const;
    };
};


