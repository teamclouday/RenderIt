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
    AllOrdered,   // draw everything
    AllUnOrdered
};

} // namespace RenderIt
