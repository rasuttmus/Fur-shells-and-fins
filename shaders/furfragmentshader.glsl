#version 330 core

uniform mat4      NM;
uniform vec3      cameraPosition;
uniform vec3      color;
uniform vec3      ambientColor;
uniform vec3      diffuseColor;
uniform vec3      lightPosition;
uniform float     transparency;
uniform float     lightPower;
uniform float     layerOffset;
uniform float     furLength;
uniform int       numberOfLayers;
uniform int       layerIndex;
uniform int       noiseType;
uniform float     furNoiseLengthVariation;
uniform float     furNoiseSampleScale;
uniform sampler2D textureSampler;
uniform sampler2D hairMapSampler;

in vec3 normal;
in vec3 vertexPositionModelSpace;
in vec3 lightDirectionCameraSpace;
in vec2 UV;
in vec3 UV3D;

out vec4 fragmentColor;


//
// Description : Array and textureless GLSL 2D/3D/4D simplex 
//               noise functions.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : ijm
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
// 

vec3 mod289(vec3 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}


vec4 mod289(vec4 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}


vec4 permute(vec4 x) {
    return mod289(((x*34.0)+1.0)*x);
}


vec4 taylorInvSqrt(vec4 r) {
    return 1.79284291400159 - 0.85373472095314 * r;
}


float snoise(vec3 v) {

    const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
    const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

    // First corner
    vec3 i  = floor(v + dot(v, C.yyy) );
    vec3 x0 =   v - i + dot(i, C.xxx) ;

    // Other corners
    vec3 g = step(x0.yzx, x0.xyz);
    vec3 l = 1.0 - g;
    vec3 i1 = min( g.xyz, l.zxy );
    vec3 i2 = max( g.xyz, l.zxy );

    vec3 x1 = x0 - i1 + C.xxx;
    vec3 x2 = x0 - i2 + C.yyy;
    vec3 x3 = x0 - D.yyy;

    // Permutations
    i = mod289(i); 
    vec4 p = permute( permute( permute( 
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 )) 
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

    // Gradients: 7x7 points over a square, mapped onto an octahedron.
    // The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
    float n_ = 0.142857142857; // 1.0/7.0
    vec3  ns = n_ * D.wyz - D.xzx;

    vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

    vec4 x_ = floor(j * ns.z);
    vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

    vec4 x = x_ *ns.x + ns.yyyy;
    vec4 y = y_ *ns.x + ns.yyyy;
    vec4 h = 1.0 - abs(x) - abs(y);

    vec4 b0 = vec4( x.xy, y.xy );
    vec4 b1 = vec4( x.zw, y.zw );

    vec4 s0 = floor(b0)*2.0 + 1.0;
    vec4 s1 = floor(b1)*2.0 + 1.0;
    vec4 sh = -step(h, vec4(0.0));

    vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
    vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

    vec3 p0 = vec3(a0.xy,h.x);
    vec3 p1 = vec3(a0.zw,h.y);
    vec3 p2 = vec3(a1.xy,h.z);
    vec3 p3 = vec3(a1.zw,h.w);

    //Normalise gradients
    vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;

    // Mix final noise value
    vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
    m = m * m;
    return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), 
                            dot(p2,x2), dot(p3,x3) ) );
}


// Permutation polynomial: (34x^2 + x) mod 289
vec3 permute(vec3 x) {
    return mod((34.0 * x + 1.0) * x, 289.0);
}

// Cellular noise, returning F1 and F2 in a vec2.
// Speeded up by using 2x2x2 search window instead of 3x3x3,
// at the expense of some pattern artifacts.
// F2 is often wrong and has sharp discontinuities.
// If you need a good F2, use the slower 3x3x3 version.
vec2 cellular2x2x2(vec3 P) {
#define K 0.142857142857 // 1/7
#define Ko 0.428571428571 // 1/2-K/2
#define K2 0.020408163265306 // 1/(7*7)
#define Kz 0.166666666667 // 1/6
#define Kzo 0.416666666667 // 1/2-1/6*2
#define jitter 0.8 // smaller jitter gives less errors in F2
    vec3 Pi = mod(floor(P), 289.0);
    vec3 Pf = fract(P);
    vec4 Pfx = Pf.x + vec4(0.0, -1.0, 0.0, -1.0);
    vec4 Pfy = Pf.y + vec4(0.0, 0.0, -1.0, -1.0);
    vec4 p = permute(Pi.x + vec4(0.0, 1.0, 0.0, 1.0));
    p = permute(p + Pi.y + vec4(0.0, 0.0, 1.0, 1.0));
    vec4 p1 = permute(p + Pi.z); // z+0
    vec4 p2 = permute(p + Pi.z + vec4(1.0)); // z+1
    vec4 ox1 = fract(p1*K) - Ko;
    vec4 oy1 = mod(floor(p1*K), 7.0)*K - Ko;
    vec4 oz1 = floor(p1*K2)*Kz - Kzo; // p1 < 289 guaranteed
    vec4 ox2 = fract(p2*K) - Ko;
    vec4 oy2 = mod(floor(p2*K), 7.0)*K - Ko;
    vec4 oz2 = floor(p2*K2)*Kz - Kzo;
    vec4 dx1 = Pfx + jitter*ox1;
    vec4 dy1 = Pfy + jitter*oy1;
    vec4 dz1 = Pf.z + jitter*oz1;
    vec4 dx2 = Pfx + jitter*ox2;
    vec4 dy2 = Pfy + jitter*oy2;
    vec4 dz2 = Pf.z - 1.0 + jitter*oz2;
    vec4 d1 = dx1 * dx1 + dy1 * dy1 + dz1 * dz1; // z+0
    vec4 d2 = dx2 * dx2 + dy2 * dy2 + dz2 * dz2; // z+1

    // Sort out the two smallest distances (F1, F2)
#if 0
    // Cheat and sort out only F1
    d1 = min(d1, d2);
    d1.xy = min(d1.xy, d1.wz);
    d1.x = min(d1.x, d1.y);
    return sqrt(d1.xx);
#else
    // Do it right and sort out both F1 and F2
    vec4 d = min(d1,d2); // F1 is now in d
    d2 = max(d1,d2); // Make sure we keep all candidates for F2
    d.xy = (d.x < d.y) ? d.xy : d.yx; // Swap smallest to d.x
    d.xz = (d.x < d.z) ? d.xz : d.zx;
    d.xw = (d.x < d.w) ? d.xw : d.wx; // F1 is now in d.x
    d.yzw = min(d.yzw, d2.yzw); // F2 now not in d2.yzw
    d.y = min(d.y, d.z); // nor in d.z
    d.y = min(d.y, d.w); // nor in d.w
    d.y = min(d.y, d2.x); // F2 is now in d.y
    return sqrt(d.xy); // F1 and F2
#endif
}


float aastep(float threshold, float value) {

    float afwidth = 0.7 * length(vec2(dFdx(value), dFdy(value)));

    return smoothstep(threshold - afwidth, threshold + afwidth, value);
}


void main() {

    noiseType;

    // For diffuse shading
    vec3 n         = normalize(normal);
    vec3 l         = normalize(lightDirectionCameraSpace);
    float cosTheta = clamp(dot(n, l), 0, 1);

    // The worley noise pattern on the fur
    vec2  worleySample    = cellular2x2x2(UV3D * 0.2);
    float sw              = fwidth(worleySample.x);
    float darkSpotWorley  = smoothstep(0.4-sw, 0.6+sw, worleySample.x);
    float lightSpotWorley = smoothstep(0.3-sw, 0.05+sw, worleySample.x);
    float worleyColor     = darkSpotWorley + lightSpotWorley;

    // The simplex noise pattern for the fur
    float simplexSample   = snoise(UV3D * 0.2);
    float simplexColor    = smoothstep(0.3, 0.1, simplexSample);

    // Depending on the GUI input, pick the according noise function (simplex or worley)
    float noise = (noiseType == 0) ? simplexColor : worleyColor;

    // Apply shading, the diffuse term is determined by the index of the current shell
    fragmentColor.rgb = ambientColor  * color
                      + diffuseColor  * color * lightPower * cosTheta * ( 2.5 * float(layerIndex) / float(numberOfLayers) );

    // Apply the worley noise color
    fragmentColor.rgb *= (noise * 0.8) + 0.2;

    // Get value from fur noise texture
    float furSample = texture(textureSampler, UV).r;

    // Get value from hair map texture, detrmines if there should be fur or not
    vec3 heightSample = texture(hairMapSampler, UV).rgb;

    // Vary the fur length with some simplex noise
    float furLengthNoise = snoise(vertexPositionModelSpace * furNoiseSampleScale);

    // This where everything comes together, the noise texture is thresholded depending on a lot of factors
    furSample = aastep(0.2 + (float(layerIndex) / float(numberOfLayers) * 0.8) + (furLengthNoise * furNoiseLengthVariation), furSample);

    // Finaly apply the thresholded noise texture to the alpha channel of the fragment, 
    // this will create a surface that looks like fur.
    fragmentColor.a = heightSample.x * furSample * (1.0 - (float(layerIndex) / float(numberOfLayers)));
}