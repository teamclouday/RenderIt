#pragma once

/** @file */

namespace RenderIt
{

// Defines the render pass
enum class RenderPass
{
    Opaque,
    Transparent,
    Ordered,      // (default)
    Transmissive, // refraction
};

} // namespace RenderIt
