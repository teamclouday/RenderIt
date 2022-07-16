#include "PostProcess/PostProcessLuminance.hpp"
#include "Tools.hpp"

#include <algorithm>
#include <cmath>
#include <string>

#define DISPATCH_X 16.0f
#define DISPATCH_Y 16.0f
#define DISPATCH_GROUPSIZE 256

namespace RenderIt
{

PostProcessLuminance::PostProcessLuminance()
    : _lumMax(1.0f), _lumMin(0.0f), _lumAvg(0.5f), _lumAvgSmooth(0.5f), _minLog(-10.0f), _maxLog(2.0f), _RBO(nullptr),
      _TEX(nullptr)
{
    _frameWidth = 1;
    _frameHeight = 1;
    _FBO = nullptr;
    _rangeLog = _maxLog - _minLog;
    _rangeLogInv = 1.0f / _rangeLog;
    loadShader();
    loadSSBO();
}

void PostProcessLuminance::StartRecord()
{
    if (_FBO)
        _FBO->Bind();
}

void PostProcessLuminance::StopRecord()
{
    if (_FBO)
        _FBO->UnBind();
}

bool PostProcessLuminance::Update(int screenWidth, int screenHeight)
{
    if (screenWidth == _frameWidth && screenHeight == _frameHeight && _FBO)
        return true;
    _frameWidth = std::max(1, screenWidth);
    _frameHeight = std::max(1, screenHeight);
    _dispatchX = static_cast<GLuint>(std::ceil(_frameWidth / DISPATCH_X));
    _dispatchY = static_cast<GLuint>(std::ceil(_frameHeight / DISPATCH_Y));
    _numPixels = static_cast<unsigned>(_frameHeight * _frameWidth);
    return loadFBO();
}

void PostProcessLuminance::Draw(std::function<void(const Shader *)> func)
{
    Tools::display_message(NAME, "drawing nothing!", Tools::MessageType::WARN);
}

void PostProcessLuminance::Compute(float timeDelta)
{
    if (!_FBO)
        return;
    // dispatch to fill histogram buffer
    _histSSBO->BindBase(1u);
    _shaderFill->Bind();
    _shaderFill->UniformFloat("minLogLum", _minLog);
    _shaderFill->UniformFloat("rangeLogLumInv", _rangeLogInv);
    glBindImageTexture(0u, _TEX->Get(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA16F);
    glDispatchCompute(_dispatchX, _dispatchY, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    _shaderFill->UnBind();
    // dispatch to compute values (max, min, avg)
    _shaderComp->Bind();
    _shaderComp->UniformUInt("numPixels", _numPixels);
    _shaderComp->UniformFloat("minLogLum", _minLog);
    _shaderComp->UniformFloat("rangeLogLum", _rangeLog);
    _valsSSBO->BindBase(0u);
    glDispatchCompute(1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    _valsSSBO->UnBindBase(0u);
    _shaderComp->UnBind();
    _histSSBO->UnBindBase(1u);
    // retrieve values
    _valsSSBO->Bind();
    auto dataPtr = reinterpret_cast<float *>(glMapBuffer(_valsSSBO->type, GL_READ_ONLY));
    _lumMax = *dataPtr;
    _lumMin = *(dataPtr + 1);
    _lumAvg = *(dataPtr + 2);
    glUnmapBuffer(_valsSSBO->type);
    _valsSSBO->UnBind();
    // compute smooth luminance average
    if (timeDelta >= 0.0f)
        _lumAvgSmooth = _lumAvgSmooth + (_lumAvg - _lumAvgSmooth) * (1.0f - std::exp(-timeDelta * 1.1f));
    else
        _lumAvgSmooth = _lumAvg;
}

float PostProcessLuminance::GetMaxLuminance() const
{
    return _lumMax;
}

float PostProcessLuminance::GetMinLuminance() const
{
    return _lumMin;
}

float PostProcessLuminance::GetAvgLuminance() const
{
    return _lumAvg;
}

float PostProcessLuminance::GetAvgSmoothLuminance() const
{
    return _lumAvgSmooth;
}

void PostProcessLuminance::loadShader()
{
    std::string compShaderFill = R"(
        #version 450 core
        layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

        // reference: https://alextardif.com/HistogramLuminance.html
        // reference: https://bruop.github.io/exposure/

        layout(rgba16f, binding = 0) readonly uniform image2D imageIn;

        #define GROUP_SIZE 256 // 16 * 16
        layout(std430, binding = 1) buffer HISTGRAM
        {
            uint hist[GROUP_SIZE];
            uint maxLum[GROUP_SIZE];
            uint minLum[GROUP_SIZE];
        }
        info;
        shared uint histShared[GROUP_SIZE];
        shared uint maxLumShared[GROUP_SIZE];
        shared uint minLumShared[GROUP_SIZE];

        uniform float minLogLum;
        uniform float rangeLogLumInv;

        #define LUMINANCE vec3(0.2125, 0.7154, 0.0721)

        // get histogram bin index
        uint GetBinIndex(float lum)
        {
            if (lum < 1e-4)
                return 0;
            return uint(1.0 + clamp((log2(lum) - minLogLum) * rangeLogLumInv, 0.0, 1.0) * (GROUP_SIZE - 2.0));
        }

        void main()
        {
            // init bin
            histShared[gl_LocalInvocationIndex] = 0;
            maxLumShared[gl_LocalInvocationIndex] = 0;
            minLumShared[gl_LocalInvocationIndex] = 255;
            barrier();
            // fill bin in group
            ivec2 imageDim = imageSize(imageIn).xy;
            ivec2 texCoords = ivec2(gl_GlobalInvocationID.xy);
            if (texCoords.x < imageDim.x && texCoords.y < imageDim.y)
            {
                vec3 color = imageLoad(imageIn, texCoords).rgb;
                float lum = dot(LUMINANCE, color);
                uint index = GetBinIndex(lum);
                atomicAdd(histShared[index], 1);
                uint lumVal = uint(lum * 255);
                atomicMax(maxLumShared[gl_LocalInvocationIndex], lumVal);
                atomicMin(minLumShared[gl_LocalInvocationIndex], lumVal);
            }
            barrier();
            // fill bin
            atomicAdd(info.hist[gl_LocalInvocationIndex], histShared[gl_LocalInvocationIndex]);
            atomicMax(info.maxLum[gl_LocalInvocationIndex], maxLumShared[gl_LocalInvocationIndex]);
            atomicMin(info.minLum[gl_LocalInvocationIndex], minLumShared[gl_LocalInvocationIndex]);
        }
    )";
    _shaderFill = std::make_unique<Shader>();
    _shaderFill->AddSource(compShaderFill, GL_COMPUTE_SHADER);
    _shaderFill->Compile();

    std::string compShaderComp = R"(
        #version 450 core
        layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

        // reference: https://alextardif.com/HistogramLuminance.html
        // reference: https://bruop.github.io/exposure/

        #define GROUP_SIZE 256 // 16 * 16
        layout(std430, binding = 0) writeonly buffer VALUES
        {
            float data[3];
        }
        vals;
        layout(std430, binding = 1) buffer HISTGRAM
        {
            uint hist[GROUP_SIZE];
            uint maxLum[GROUP_SIZE];
            uint minLum[GROUP_SIZE];
        }
        info;
        shared uint histShared[GROUP_SIZE];
        shared uint maxLumShared[GROUP_SIZE];
        shared uint minLumShared[GROUP_SIZE];

        uniform uint numPixels;
        uniform float minLogLum;
        uniform float rangeLogLum;

        void main()
        {
            // load buffer data
            uint count = info.hist[gl_LocalInvocationIndex];
            histShared[gl_LocalInvocationIndex] = count * gl_LocalInvocationIndex;
            maxLumShared[gl_LocalInvocationIndex] = info.maxLum[gl_LocalInvocationIndex];
            minLumShared[gl_LocalInvocationIndex] = info.minLum[gl_LocalInvocationIndex];
            barrier();
            // reset count for next pass
            info.hist[gl_LocalInvocationIndex] = 0;
            info.maxLum[gl_LocalInvocationIndex] = 0;
            info.minLum[gl_LocalInvocationIndex] = 255;
            // weighted count
            for (uint cutoff = (GROUP_SIZE >> 1); cutoff > 0; cutoff >>= 1)
            {
                if (gl_LocalInvocationIndex < cutoff)
                {
                    histShared[gl_LocalInvocationIndex] += histShared[gl_LocalInvocationIndex + cutoff];
                    atomicMax(maxLumShared[gl_LocalInvocationIndex], maxLumShared[gl_LocalInvocationIndex + cutoff]);
                    atomicMin(minLumShared[gl_LocalInvocationIndex], minLumShared[gl_LocalInvocationIndex + cutoff]);
                }
                barrier();
            }
            // compute avg, max, min (only once)
            if (gl_LocalInvocationIndex == 0)
            {
                // compute weighted average
                float weighted = float(histShared[0]) / max(1.0, float(numPixels - count)) - 1.0;
                weighted = exp2(minLogLum + (weighted / (GROUP_SIZE - 2.0) * rangeLogLum));
                // save
                vals.data[0] = float(maxLumShared[0]) / 255.0;
                vals.data[1] = float(minLumShared[0]) / 255.0;
                vals.data[2] = weighted;
            }
        }
    )";
    _shaderComp = std::make_unique<Shader>();
    _shaderComp->AddSource(compShaderComp, GL_COMPUTE_SHADER);
    _shaderComp->Compile();
}

bool PostProcessLuminance::loadFBO()
{
    _TEX = std::make_unique<STexture>(GL_TEXTURE_2D);
    _TEX->Bind();
    glTexImage2D(_TEX->type, 0, GL_RGBA16F, _frameWidth, _frameHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    _TEX->UnBind();

    _RBO = std::make_unique<SRBO>();
    _RBO->Bind();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _frameWidth, _frameHeight);
    _RBO->UnBind();

    _FBO = std::make_unique<SFBO>();
    _FBO->Bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _TEX->type, _TEX->Get(), 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _RBO->Get());
    _FBO->UnBind();

    if (!_FBO->Validate())
    {
        _FBO = nullptr;
        return false;
    }
    return true;
}

void PostProcessLuminance::loadSSBO()
{
    // histogram & buffers
    _histSSBO = std::make_unique<SBuffer>(GL_SHADER_STORAGE_BUFFER);
    _histSSBO->Bind();
    glBufferData(_histSSBO->type, 3 * DISPATCH_GROUPSIZE * sizeof(unsigned), nullptr, GL_DYNAMIC_COPY);
    _histSSBO->UnBind();
    // max, min, sum
    _valsSSBO = std::make_unique<SBuffer>(GL_SHADER_STORAGE_BUFFER);
    _valsSSBO->Bind();
    glBufferData(_valsSSBO->type, 3 * sizeof(float), nullptr, GL_DYNAMIC_READ);
    _valsSSBO->UnBind();
}

} // namespace RenderIt
