#version 330

#define M_PI 3.1415926535897932384626433832795
#define FLT_MIN 1.175494351e-38

uniform vec3 camera_position;
uniform vec3 camera_direction;
uniform vec3 camera_up;

uniform float scale;
uniform float aspect;
uniform float fov;

uniform float screen_width;
uniform float screen_height;

uniform float epsilon_factor;
uniform float max_dist;
uniform float max_bailout;
uniform int max_iter;
uniform int max_steps;



in vec3 Color;
out vec4 o_color;

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

float sdfMandelbulb(vec3 p, out vec4 pixelColor, in float mdist)
{
	vec3 q = p;
	float m = dot(q,q);
	float dr = 1.0;
	vec4 trap = vec4(abs(q), m);

	for (int i = 0; i < max_iter; ++i)
	{
		float m2 = m*m;
		float m4 = m2*m2;
		dr =  8.0*sqrt(m4*m2*m)*dr + 1.0;

		float x = q.x; float x2 = x*x; float x4 = x2*x2;
		float y = q.y; float y2 = y*y; float y4 = y2*y2;
		float z = q.z; float z2 = z*z; float z4 = z2*z2;

		float k3 = x2 + z2;
		float k2 = inversesqrt( k3*k3*k3*k3*k3*k3*k3 );
		float k1 = x4 + y4 + z4 - 6.0*y2*z2 - 6.0*x2*y2 + 2.0*z2*x2;
        float k4 = x2 - y2 + z2;

        q.x = p.x +  64.0*x*y*z*(x2-z2)*k4*(x4-6.0*x2*z2+z4)*k1*k2;
        q.y = p.y + -16.0*y2*k3*k4*k4 + k1*k1;
        q.z = p.z +  -8.0*y*k4*(x4*x4 - 28.0*x4*x2*z2 + 70.0*x4*z4 - 28.0*x2*z2*z4 + z4*z4)*k1*k2;

		trap = min( trap, vec4(abs(q),m) );

        m = dot(q,q);
		if( m > max_bailout )
            break;
	}

	//pixelColor = vec4(hsv2rgb(vec3(vec4(m, trap.yzw), 0.8, 0.8)), 1.0f;
	pixelColor = vec4(m, trap.yzw);
	return 0.25*log(m)*sqrt(m)/dr;
}

float map(in vec3 p, out vec4 pixelColor, in float mdist)
{
	float bulb_distance = sdfMandelbulb(p, pixelColor, mdist);
    return bulb_distance;
}

vec3 calculate_normal(in vec3 p, in float t, const float mdist)
{
    vec4 tmp;
    vec2 e = vec2(1.0, -1.0) * epsilon_factor * scale;
    return normalize( e.xyy*map(p + e.xyy, tmp, mdist) +
	                  e.yyx*map(p + e.xyy, tmp, mdist) +
					  e.yxy*map(p + e.yxy, tmp, mdist) +
				      e.xxx*map(p + e.xxx, tmp, mdist) );
}


vec3 applyFog( in vec3  rgb,       // original color of the pixel
               in float distance ) // camera to point distance
{
	float b = 1.0;
    float fogAmount = 1.0 - exp( -distance*b );
    vec3  fogColor  = vec3(0.5,0.6,0.7);
    return mix( rgb, fogColor, fogAmount );
}

vec3 applyFogWithSun( in vec3  rgb,      // original color of the pixel
					  in float distance, // camera to point distance
					  in vec3  rayDir,   // camera to point vector
					  in vec3  sunDir )  // sun light direction
{
	float b = 1.0;
    float fogAmount = 1.0 - exp( -distance*b );
    float sunAmount = max( dot( rayDir, sunDir ), 0.0 );
    vec3  fogColor  = mix( vec3(0.5,0.6,0.7), // bluish
                           vec3(1.0,0.9,0.7), // yellowish
                           pow(sunAmount,8.0) );
    return mix( rgb, fogColor, fogAmount );
}


vec3 applyFogNonConstant( in vec3  rgb,      // original color of the pixel
						  in float distance, // camera to point distance
						  in vec3  rayOri,   // camera position
						  in vec3  rayDir,
						  in vec3  sunDir  )  // camera to point vector
{
	float b = 1.0;
	float c = 1.0;
    float fogAmount = c * exp(-rayOri.y*b) * (1.0-exp( -distance*rayDir.y*b ))/rayDir.y;
	float sunAmount = max( dot( rayDir, sunDir ), 0.0 );
    vec3  fogColor  = mix( vec3(0.5,0.6,0.7), // bluish
                           vec3(1.0,0.9,0.7), // yellowish
                           pow(sunAmount, 8.0) );
    return mix( rgb, fogColor, fogAmount );
}

vec2 intersect_sphere(in vec4 sph, in vec3 ro, in vec3 rd)
{
    vec3 oc = ro - sph.xyz;
    
	float b = dot(oc,rd);
	float c = dot(oc,oc) - sph.w*sph.w;
    float h = b*b - c;
    
    if (h < 0.0) return vec2(-1.0);

    h = sqrt(h);

    return -b + vec2(-h,h);
}

float cast_ray(in vec3 ro, in vec3 rd, in float steps, out vec4 color, out int steps_taken)
{
    float res = -1.0;

	// bounding sphere
	vec2 dis = intersect_sphere (vec4(0.0,0.0,0.0,1.25), ro, rd);
	
	if (dis.y < 0.0) return -1.0;
	dis.x = max (dis.x, 0.0);
	dis.y = min (dis.y, 10.0);

	float eps = epsilon_factor * scale;

	// raymarch fractal distance field
	vec4 trap;
	float t = dis.x;
	for (int i = 0; i < steps; ++i) { 
		vec3 pos = ro + rd*t;
		float h = map( pos, trap, t );
		
		if ( t>dis.y || h<eps ) { steps_taken = i; break; }
		t += h;
	}
    
	if (t < dis.y) {
		color = trap;
		res = t;
	}

	return res;
}


const vec3 light1 = vec3(  0.577, 0.577, -0.577 );
const vec3 light2 = vec3( -0.707, 0.000,  0.707 );

vec3 ray_march(in vec3 ro, in vec3 rd)
{
	vec4 pixelColor;
	float mdist = abs(map(ro, pixelColor, 1.0));

	vec4 tra;
	int steps_taken;

	float t = cast_ray(ro, rd, max_steps, tra, steps_taken);

	vec3 col;
	if (t < 0.0) {
		/* Color sky */
     	col  = vec3(0.7,0.7,0.7);
	} else {
	    // Color Fractal
		col = hsv2rgb(vec3(length(tra.yzw), .8, .8));
		//col = tra.xyz;

		// Calculate Lighting
		vec3 pos = ro + t*rd;
        vec3 nor = calculate_normal( pos, t, mdist );
		vec3 lightDir = -rd;

		vec3 lightColor = vec3(1.0, 1.0, 1.0);

		float ambientStrength = 0.1;
		vec3 ambient = ambientStrength * lightColor;

		//float sha1 = soft_shadow( pos+0.001*nor, light1, 32.0 );
		float diff = max(dot(nor, lightDir), 0.0);
		vec3 diffuse = diff * lightColor;

		col *= (ambient + diffuse);
	}

	return applyFogNonConstant(col, t, ro, rd, light1);
}


void main()
{
    float fov_rad = fov * M_PI / 180.0;
	float px = (2 * (gl_FragCoord.x + 0.5) / screen_width - 1.0) * tan(fov_rad / 2) * aspect;
	float py = (1.0 - 2 * (gl_FragCoord.y + 0.5) / screen_height) * tan(fov_rad / 2); 

	vec3 camera_right = normalize(cross(camera_up, camera_direction));

    vec3 ro = camera_position;
	vec3 rd = normalize(camera_right * px + camera_up * py + camera_direction);

    vec3 shaded_color = ray_march(ro.xyz, rd.xyz);
    o_color = vec4(shaded_color, 1.0);
}