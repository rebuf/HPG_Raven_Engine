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
#include "Ray.h"

namespace Raven
{
    glm::vec3 Ray::ClosestPoint(const Ray& ray) const
    {
        glm::vec3 p13 = origin - ray.origin;
        glm::vec3 p43 = ray.direction;
        glm::vec3 p21 = direction;

        float d1343 = glm::dot(p13,p43);
        float d4321 = glm::dot(p43,p21);
        float d1321 = glm::dot(p13,p21);
        float d4343 = glm::dot(p43,p43);
        float d2121 = glm::dot(p21,p21);

        float d = d2121 * d4343 - d4321 * d4321;
        if (std::abs(d) < 0.000001f)
            return origin;
        float n = d1343 * d4321 - d1321 * d4343;
        float a = n / d;

        return origin + a * direction;
    }
};
