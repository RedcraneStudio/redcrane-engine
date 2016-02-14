/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "noise.h"
#include <cmath>
namespace redc
{
  namespace
  {
    using pt_t = double;

    struct vec3
    {
      pt_t x, y, z;
    };

    static constexpr vec3 G[] =
    {
      {+1.0f, +1.0f,  0.0f},
      {-1.0f, +1.0f,  0.0f},
      {+1.0f, -1.0f,  0.0f},
      {-1.0f, -1.0f,  0.0f},

      {+1.0f,  0.0f, +1.0f},
      {-1.0f,  0.0f, +1.0f},
      {+1.0f,  0.0f, -1.0f},
      {-1.0f,  0.0f, -1.0f},

      { 0.0f, +1.0f, +1.0f},
      { 0.0f, -1.0f, +1.0f},
      { 0.0f, +1.0f, -1.0f},
      { 0.0f, -1.0f, -1.0f},

      {+1.0f, +1.0f,  0.0f},
      {-1.0f, +1.0f,  0.0f},
      { 0.0f, -1.0f, +1.0f},
      { 0.0f, -1.0f, -1.0f}
    };

    pt_t dot_gradient(int hash, pt_t x, pt_t y, pt_t z) noexcept
    {
      auto vec = G[hash & 0xf];
      return vec.x * x + vec.y * y + vec.z * z;
    }
    pt_t fade(pt_t t) noexcept
    {
      return t * t * t * (t * (t * 6 - 15) + 10);
    }
    pt_t lerp(pt_t t, pt_t a, pt_t b) noexcept
    {
      return a + t * (b - a);
    }
    static constexpr int P[] =
    {
      151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,
      142,8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,
      219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,
      68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,
      133,230,220,105,92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,
      73,209,76,132,187,208,89,18,169,200,196,135,130,116,188,159,86,164,100,
      109,198,173,186,3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,
      85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,
      152,2,44,154,163,70,221,153,101,155,167,43,172,9,129,22,39,253,19,98,
      108,110,79,113,224,232,178,185,112,104,218,246,97,228,251,34,242,193,238,
      210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,49,192,214,31,
      181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,138,236,205,
      93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,

      151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,
      142,8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,
      219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,
      68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,
      133,230,220,105,92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,
      73,209,76,132,187,208,89,18,169,200,196,135,130,116,188,159,86,164,100,
      109,198,173,186,3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,
      85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,
      152,2,44,154,163,70,221,153,101,155,167,43,172,9,129,22,39,253,19,98,
      108,110,79,113,224,232,178,185,112,104,218,246,97,228,251,34,242,193,238,
      210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,49,192,214,31,
      181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,138,236,205,
      93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
     };
  }

  double perlin_noise(double x, double y, double z) noexcept
  {
    // Floor each value modulo 0xff (255)
    int cube_x = (int) std::floor(x) & 255;
    int cube_y = (int) std::floor(y) & 255;
    int cube_z = (int) std::floor(z) & 255;

    // Relative positions to the integer cube.
    x -= std::floor(x);
    y -= std::floor(y);
    z -= std::floor(z);

    // Fade curves
    pt_t u = fade(x);
    pt_t v = fade(y);
    pt_t w = fade(z);

    // I'm pretty sure this doesn't overflow because P[cube_x] can only be at
    // most 255 and the same with cube_y (because of the above modulo).
    // Our array is 512 length so this works.
    int A = P[cube_x] + cube_y;
    int AA = P[A] + cube_z;
    int AB = P[A+1] + cube_z;

    int B = P[cube_x+1] + cube_y;
    int BA = P[B] + cube_z;
    int BB = P[B+1] + cube_z;

    return lerp(w, lerp(v, lerp(u, dot_gradient(P[AA],   x,   y,   z),
                                   dot_gradient(P[BA],   x-1, y,   z)),
                           lerp(u, dot_gradient(P[AB],   x,   y-1, z),
                                   dot_gradient(P[BB],   x-1, y-1, z))),
                   lerp(v, lerp(u, dot_gradient(P[AA+1], x,   y,   z-1),
                                   dot_gradient(P[BA+1], x-1, y,   z-1)),
                           lerp(u, dot_gradient(P[AB+1], x,   y-1, z-1),
                                   dot_gradient(P[BB+1], x-1, y-1, z-1))));
  }

  glm::vec4 mod289(glm::vec4 x)
  {
    return x - glm::floor(x * (1.0f / 289.0f)) * 289.0f;
  }

  glm::vec4 permute(glm::vec4 x)
  {
    return mod289(((x*34.0f)+1.0f)*x);
  }

  glm::vec4 taylorInvSqrt(glm::vec4 r)
  {
    return 1.79284291400159f - 0.85373472095314f * r;
  }

  glm::vec2 fade(glm::vec2 t)
  {
    return t*t*t*(t*(t*6.0f-15.0f)+10.0f);
  }

  // Classic Perlin noise
  float cnoise(glm::vec2 P)
  {
    auto Pi = glm::floor(glm::vec4(P.x, P.y, P.x, P.y)) +
              glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    auto Pf = glm::fract(glm::vec4(P.x, P.y, P.x, P.y)) -
              glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    Pi = mod289(Pi); // To avoid truncation effects in permutation
    auto ix = glm::vec4(Pi.x, Pi.z, Pi.x, Pi.z);
    auto iy = glm::vec4(Pi.y, Pi.y, Pi.w, Pi.w);
    auto fx = glm::vec4(Pf.x, Pf.z, Pf.x, Pf.z);
    auto fy = glm::vec4(Pf.y, Pf.y, Pf.w, Pf.w);

    glm::vec4 i = permute(permute(ix) + iy);

    glm::vec4 gx = glm::fract(i * (1.0f / 41.0f)) * 2.0f - 1.0f ;
    glm::vec4 gy = glm::abs(gx) - 0.5f ;
    glm::vec4 tx = glm::floor(gx + 0.5f);
    gx = gx - tx;

    glm::vec2 g00 = glm::vec2(gx.x,gy.x);
    glm::vec2 g10 = glm::vec2(gx.y,gy.y);
    glm::vec2 g01 = glm::vec2(gx.z,gy.z);
    glm::vec2 g11 = glm::vec2(gx.w,gy.w);

    glm::vec4 norm = taylorInvSqrt(glm::vec4(glm::dot(g00, g00), glm::dot(g01, g01), glm::dot(g10, g10), glm::dot(g11, g11)));
    g00 *= norm.x;
    g01 *= norm.y;
    g10 *= norm.z;
    g11 *= norm.w;

    float n00 = glm::dot(g00, glm::vec2(fx.x, fy.x));
    float n10 = glm::dot(g10, glm::vec2(fx.y, fy.y));
    float n01 = glm::dot(g01, glm::vec2(fx.z, fy.z));
    float n11 = glm::dot(g11, glm::vec2(fx.w, fy.w));

    glm::vec2 fade_xy = fade(glm::vec2(Pf.x, Pf.y));
    glm::vec2 n_x = glm::mix(glm::vec2(n00, n01), glm::vec2(n10, n11), fade_xy.x);
    float n_xy = glm::mix(n_x.x, n_x.y, fade_xy.y);
    return 2.3 * n_xy;
  }
}
