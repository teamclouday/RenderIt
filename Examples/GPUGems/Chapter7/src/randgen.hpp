#pragma once

#include <random>
#include <vector>

#include <glm/glm.hpp>

struct RandomGenerator
{
    RandomGenerator() : dist(0.0f, 1.0f)
    {
        std::random_device rd;
        rng.seed(rd());
    }

    float NextFloat()
    {
        return dist(rng);
    }

    glm::vec2 NextPosition()
    {
        return glm::vec2(NextFloat() - 0.5f, NextFloat() - 0.5f);
    }

    std::vector<glm::vec2> GeneratePositions(int size)
    {
        std::vector<glm::vec2> res(size);
        for (auto i = 0; i < size; ++i)
            res[i] = NextPosition();
        return res;
    }

    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
};
