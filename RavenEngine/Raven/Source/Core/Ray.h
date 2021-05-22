/*
 * Developed by Raven Group at the University  of Leeds
 * Copyright (C) 2021 Ammar Herzallah, Ben Husle, Thomas Moreno Cooper, Sulagna Sinha & Tian Zeng
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * THIS PROGRAM IS DISTRIBUTED IN THE HOPE THAT IT WILL BE USEFUL,
 * BUT WITHOUT ANY WARRANTY; WITHOUT EVEN THE IMPLIED WARRANTY OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  SEE THE
 * GNU GENERAL PUBLIC LICENSE FOR MORE DETAILS.
 */
#pragma once

#include <glm/glm.hpp>

namespace Raven
{
    class  Ray
    {
    public:
        friend class Camera;
        Ray() noexcept = default;

        Ray(const glm::vec3& origin, const glm::vec3& direction) 
        {
            Define(origin, direction);
        }

        bool operator ==(const Ray& rhs) const { return origin == rhs.origin && direction == rhs.direction; }

        bool operator !=(const Ray& rhs) const { return origin != rhs.origin || direction != rhs.direction; }

        inline void Define(const glm::vec3& origin, const glm::vec3& direction)
        {
            this->origin = origin;
            this->direction = glm::normalize(direction);
        }

        inline glm::vec3 Project(const glm::vec3& point) const
        {
            glm::vec3 offset = point - origin;
            return origin + glm::dot(offset, direction) * direction;
        }

        inline float Distance(const glm::vec3& point) const
        {
            glm::vec3 projected = Project(point);
            return glm::length(point - projected);
        }

        glm::vec3 ClosestPoint(const Ray& ray) const;

    private:
        glm::vec3 origin;
        glm::vec3 direction;
    };
}
