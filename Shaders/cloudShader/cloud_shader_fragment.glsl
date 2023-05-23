#version 410 core

#define NUM_STEPS 30 //30 is nice
#define STEP_SIZE 0.1

in vec2 uv;

out vec4 FragColor;

//UNIFORMS
uniform vec2 resolution;
uniform vec3 camera_pos;
uniform vec3 front;
uniform vec3 right;
uniform vec3 up;
uniform float time;
uniform float lower_limit;
uniform float upper_limit;

uniform samplerCube skybox;


//Scene Properties
vec3 sky_color = vec3(0.2, 0.4, 0.69);
const vec3 cloud_color = vec3(1.0);
vec3 sun_dir;
float sun;


//Perlin Noise
vec3 mod289(vec3 x)
{
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x)
{
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x)
{
  return mod289(((x*34.0)+10.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

vec3 fade(vec3 t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}

// Classic Perlin noise
float cnoise(vec3 P)
{
  vec3 Pi0 = floor(P); // Integer part for indexing
  vec3 Pi1 = Pi0 + vec3(1.0); // Integer part + 1
  Pi0 = mod289(Pi0);
  Pi1 = mod289(Pi1);
  vec3 Pf0 = fract(P); // Fractional part for interpolation
  vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
  vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  vec4 iy = vec4(Pi0.yy, Pi1.yy);
  vec4 iz0 = Pi0.zzzz;
  vec4 iz1 = Pi1.zzzz;

  vec4 ixy = permute(permute(ix) + iy);
  vec4 ixy0 = permute(ixy + iz0);
  vec4 ixy1 = permute(ixy + iz1);

  vec4 gx0 = ixy0 * (1.0 / 7.0);
  vec4 gy0 = fract(floor(gx0) * (1.0 / 7.0)) - 0.5;
  gx0 = fract(gx0);
  vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
  vec4 sz0 = step(gz0, vec4(0.0));
  gx0 -= sz0 * (step(0.0, gx0) - 0.5);
  gy0 -= sz0 * (step(0.0, gy0) - 0.5);

  vec4 gx1 = ixy1 * (1.0 / 7.0);
  vec4 gy1 = fract(floor(gx1) * (1.0 / 7.0)) - 0.5;
  gx1 = fract(gx1);
  vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
  vec4 sz1 = step(gz1, vec4(0.0));
  gx1 -= sz1 * (step(0.0, gx1) - 0.5);
  gy1 -= sz1 * (step(0.0, gy1) - 0.5);

  vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
  vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
  vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
  vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
  vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
  vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
  vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
  vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

  vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
  g000 *= norm0.x;
  g010 *= norm0.y;
  g100 *= norm0.z;
  g110 *= norm0.w;
  vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
  g001 *= norm1.x;
  g011 *= norm1.y;
  g101 *= norm1.z;
  g111 *= norm1.w;

  float n000 = dot(g000, Pf0);
  float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));
  float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
  float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));
  float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));
  float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
  float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));
  float n111 = dot(g111, Pf1);

  vec3 fade_xyz = fade(Pf0);
  vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
  vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
  float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x);
  return 2.2 * n_xyz;
}

// Classic Perlin noise, periodic variant
float pnoise(vec3 P, vec3 rep)
{
  vec3 Pi0 = mod(floor(P), rep); // Integer part, modulo period
  vec3 Pi1 = mod(Pi0 + vec3(1.0), rep); // Integer part + 1, mod period
  Pi0 = mod289(Pi0);
  Pi1 = mod289(Pi1);
  vec3 Pf0 = fract(P); // Fractional part for interpolation
  vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
  vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  vec4 iy = vec4(Pi0.yy, Pi1.yy);
  vec4 iz0 = Pi0.zzzz;
  vec4 iz1 = Pi1.zzzz;

  vec4 ixy = permute(permute(ix) + iy);
  vec4 ixy0 = permute(ixy + iz0);
  vec4 ixy1 = permute(ixy + iz1);

  vec4 gx0 = ixy0 * (1.0 / 7.0);
  vec4 gy0 = fract(floor(gx0) * (1.0 / 7.0)) - 0.5;
  gx0 = fract(gx0);
  vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
  vec4 sz0 = step(gz0, vec4(0.0));
  gx0 -= sz0 * (step(0.0, gx0) - 0.5);
  gy0 -= sz0 * (step(0.0, gy0) - 0.5);

  vec4 gx1 = ixy1 * (1.0 / 7.0);
  vec4 gy1 = fract(floor(gx1) * (1.0 / 7.0)) - 0.5;
  gx1 = fract(gx1);
  vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
  vec4 sz1 = step(gz1, vec4(0.0));
  gx1 -= sz1 * (step(0.0, gx1) - 0.5);
  gy1 -= sz1 * (step(0.0, gy1) - 0.5);

  vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
  vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
  vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
  vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
  vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
  vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
  vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
  vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

  vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
  g000 *= norm0.x;
  g010 *= norm0.y;
  g100 *= norm0.z;
  g110 *= norm0.w;
  vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
  g001 *= norm1.x;
  g011 *= norm1.y;
  g101 *= norm1.z;
  g111 *= norm1.w;

  float n000 = dot(g000, Pf0);
  float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));
  float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
  float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));
  float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));
  float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
  float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));
  float n111 = dot(g111, Pf1);

  vec3 fade_xyz = fade(Pf0);
  vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
  vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
  float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x);
  return 2.2 * n_xyz;
}

float rand(vec3 p)
{
    return fract(sin(dot(p, vec3(12.345, 67.89, 412.12))) * 42123.45) * 2.0 - 1.0;
}

float valueNoise(vec3 p)
{
    vec3 u = floor(p);
    vec3 v = fract(p);
    vec3 s = smoothstep(0.0, 1.0, v);
    
    float a = rand(u);
    float b = rand(u + vec3(1.0, 0.0, 0.0));
    float c = rand(u + vec3(0.0, 1.0, 0.0));
    float d = rand(u + vec3(1.0, 1.0, 0.0));
    float e = rand(u + vec3(0.0, 0.0, 1.0));
    float f = rand(u + vec3(1.0, 0.0, 1.0));
    float g = rand(u + vec3(0.0, 1.0, 1.0));
    float h = rand(u + vec3(1.0, 1.0, 1.0));
    
    return mix(mix(mix(a, b, s.x), mix(c, d, s.x), s.y),
               mix(mix(e, f, s.x), mix(g, h, s.x), s.y),
               s.z);
}

float fbm(vec3 p)
{
    vec3 q = p - vec3(0.1, 0.0, 0.0) * time;;
    int numOctaves = 4;
    float lacunarity = 1.2f;
    float weight = 1.0;
    float ret = 0.0;
    float frequency = 1.0f;
    // fbm
    for (int i = 0; i < numOctaves; i++)
    {
        ret += weight * cnoise(frequency * q);
        q *= 2.0;
        weight *= 0.5;
        frequency *= lacunarity;
    }
    return clamp(ret, 0.0, 1.0);
}

vec3 ray_march(vec3 ro, vec3 rd)
{
    vec3 p;
    vec3 c = sky_color;
    for(int i = 0; i < NUM_STEPS; ++i)
    {    
        //March
        p = ro + STEP_SIZE * i * rd;
        float a = fbm(p);
        c = mix(c, cloud_color, a);
    }

    return clamp(c.rgb, 0.0, 1.0);
}


vec3 lighting(vec3 p, float density, float path_length)
{
    float density_light_dir = fbm(p + 0.3 * sun_dir); // sample in light dir
    float gradient_light_dir = clamp(density - density_light_dir, 0.0, 1.0);

    vec3 lit_color = vec3(0.91, 0.98, 1.0) + vec3(1.0, 0.6, 0.3) * 2.0 * gradient_light_dir;
    vec3 cloud_albedo = mix( vec3(1.0, 0.95, 0.8), vec3(0.25, 0.3, 0.35), density);

    const float extinction = 0.0003;
    float transmittance = exp( -extinction * path_length );
    return mix(sky_color, cloud_albedo * lit_color, transmittance );
}

vec4 ray_march_clouds(vec3 ro, vec3 rd)
{
    vec4 sum = vec4(0.0f);
    float t = 0.0f;
    for(int i = 0; i < NUM_STEPS; ++i)
    {
        vec3 p = ro + t * rd;
        if(sum.a > 0.99) break; //opaque
        float density = fbm(p);
        if(density > 0.01)
        {
            vec3 color_rgb = lighting(p, density, t);
            float alpha = density * 0.4f;
            vec4 color = vec4(color_rgb * alpha, alpha);
            sum += color * (1.0 - sum.a);
        }
        
        t += max(STEP_SIZE, 0.02 * t);
    }
    
    return clamp(sum, 0.0f, 1.0f);
}

vec3 volumetric_march(vec3 ro, vec3 rd)
{
    float depth = 0.0;
    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    color.xyz = sky_color;
    
    for (int i = 0; i < 30; i++)
    {
        vec3 p = ro + depth * rd;
        float density = fbm(p);
        
        // If density is unignorable...
        if (density > 1e-3)
        {
            // We estimate the color with w.r.t. density
            vec4 c = vec4(mix(vec3(1.0, 1.0, 1.0), vec3(0.0, 0.0, 0.0), density), density);
            // Multiply it by a factor so that it becomes softer
            c.a *= 0.4;
            c.rgb *= c.a;
            color += c * (1.0 - color.a);
        }
        
        // March forward a fixed distance
        depth += max(STEP_SIZE, 0.02 * depth);
    }
    
    return clamp(color.rgb, 0.0, 1.0);
}


void main()
{
    //I have also passed uv coordinates in range [0, 1] as texture coordinates. Positions of the fragments can also be determined in this way
    vec2 aspectRatio = vec2(resolution.x / resolution.y, 1.0);
    //vec2 p = aspectRatio * (gl_FragCoord.xy / resolution.xy/2.0 - 0.5);
    
    vec3 ro = camera_pos;
    mat3 lookAt = mat3(right, up, -front);
    //Interestingly, on the Windows Machine I tried this gave an incorrect ray origin (as if it was shifted from the center of the render plane)
    //vec3 rd = lookAt * normalize(vec3(p, 1.0));
    //This works fine 
    vec3 ord = lookAt * vec3(aspectRatio * (uv - 0.5), -1.0);
    vec3 rd = normalize(ord);

    // background sky
    sky_color = texture(skybox, ord).xyz;
    sun_dir = normalize( vec3(cos(time), 10.0, -sin(time)) );
    sun = clamp( dot(sun_dir, rd), 0.0, 1.0 );

    vec3 p = ro + ord;
    //Without Lighting
    //vec3 cloud_color = ray_march(ro, rd);
    // Gamma correction
    //cloud_color = pow(cloud_color, vec3(0.4545));
    //With Lighthing
    vec4 col = ray_march_clouds(ro, 1.5 * rd);
    col.rgb += 0.2 * vec3( 1.0, 0.4, 0.2 ) * pow( sun, 3.0 );
    float d_lower = smoothstep(lower_limit - 0.5, lower_limit, p.y);
    float d_upper = 1.0 - smoothstep(upper_limit - 0.5, upper_limit, p.y);
    //Gamma Correction
    //col.rgb = pow(res.rgb, vec3(0.4545));
    FragColor = vec4(col.rgb, col.a * d_lower * d_upper);
    
    //FragColor = vec4(color, d_lower * d_upper * 0.7);
    
}
