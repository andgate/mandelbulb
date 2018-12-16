#version 400

#define M_PI 3.1415926535897932384626433832795
#define M_2PI 6.28318530718
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
uniform float epsilon_limit;

uniform float max_dist;
uniform float max_bailout;
uniform int max_iter;
uniform int min_iter;
uniform int max_steps;
uniform int power;

uniform float fog_max_dist;
uniform bool fog_enabled;

uniform float glow_dist;
uniform bool glow_enabled;

uniform bool heat_enabled;

vec3 SKY_COLOR = vec3(0.0,0.0,0.0);

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


float sdfMandelbulb(vec3 p, in int power, out float iter, out vec4 trap)
{
	vec3 q = p;
	float r = length(q);
	float dr = 1.0;
	float dtrap = 1.0;
	trap = vec4(abs(q), r);

	//float miter = mix(min_iter, max_iter, 1.0 - scale);
	iter = 0;
	while (iter < max_iter && r < max_bailout)
	{
		float ph = asin( q.z/r );
		float th = atan( q.y / q.x );
		float zr = pow( r, power - 1.0f );

		dr = zr * dr * power + 1.0f;
		zr *= r;

		float sph = sin(power*ph); float cph = cos(power*ph);
		float sth = sin(power*th); float cth = cos(power*th);

        q.x = zr * cph*cth + p.x;
		q.y = zr * cph*sth + p.y;
		q.z = zr * sph     + p.z;

		trap = min( trap, vec4(abs(q), r) );
		r = length(q);
		iter++;
	}

	//trap = vec4(r, trap.yzw);
	return 0.5*log(r)*r/dr;
}

float sdfMandelbulb_fast(vec3 p, out vec4 pixelColor)
{
	vec3 q = p;
	float m = dot(q,q);
	float dr = 1.0;
	vec4 trap = vec4(abs(q), m);

	float miter = smoothstep(min_iter, max_iter, 1.0 - scale);
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
		if( m > max_bailout ) break;
	}

	//pixelColor = vec4(hsv2rgb(vec3(vec4(m, trap.yzw), 0.8, 0.8)), 1.0f;
	pixelColor = vec4(m, trap.yzw);
	return 0.25*log(m)*sqrt(m)/dr;
}

float map(in vec3 p, out float iter, out vec4 trap)
{
	float bulb_distance = sdfMandelbulb(p, power, iter, trap);
    return bulb_distance;
}

vec3 calculate_normal(in vec3 p, in float mdist)
{
	float tmp1;
    vec4 tmp2;
    float e = max(epsilon_limit, epsilon_factor * mdist);
	return normalize(vec3(
        map(vec3(p.x + e, p.y, p.z), tmp1, tmp2) - map(vec3(p.x - e, p.y, p.z), tmp1, tmp2),
        map(vec3(p.x, p.y + e, p.z), tmp1, tmp2) - map(vec3(p.x, p.y - e, p.z), tmp1, tmp2),
        map(vec3(p.x, p.y, p.z  + e), tmp1, tmp2) - map(vec3(p.x, p.y, p.z - e), tmp1, tmp2)
    ));
}

/*
vec3 calculate_normal_fast(in vec3 p, in float mdist)
{
    vec4 tmp;
    vec2 e = vec2(1.0, -1.0) * max(epsilon_limit, epsilon_factor * mdist);;
    return normalize( e.xyy*map(p + e.xyy, tmp) +
	                  e.yyx*map(p + e.xyy, tmp) +
					  e.yxy*map(p + e.yxy, tmp) +
				      e.xxx*map(p + e.xxx, tmp) );
}*/


vec3 applyFog( in vec3  rgb,       // original color of the pixel
               in float distance ) // camera to point distance
{
	float fog_dist = distance / max(fog_max_dist*epsilon_limit, fog_max_dist*scale);
	float fogAmount = min(1.0, fog_dist);
    return mix( rgb, SKY_COLOR, fogAmount );
}

vec3 glow(in vec3 col, in vec3 glowColor, in float mdist)
{
	// Apply glow
	float gd = glow_dist*scale;
	if (mdist < glow_dist*scale)
		return mix(col, glowColor, 1.0 - mdist/(glow_dist*scale));
	else
		return col;
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

float cast_ray(in vec3 ro, in vec3 rd, out int steps, out float eps, out float iter, out vec4 trap, out float mt, out float min_eps, out float max_v)
{
    float res = -1.0;

	vec3 pos;
	float t = 0.0;
	float h = 0.0;
	float prev_h = 0.0;
	mt = 1e10;
	float avg_v = 0.0;
	max_v = 0.0;
	min_eps = 10000.0;
	
	// Perform Ray March
	float focal_distance = max(max_dist*epsilon_limit, max_dist * scale);
	while (t < focal_distance && ++steps < max_steps) {
		pos = ro + t*rd;
		h = map( pos, iter, trap );

		eps = max(epsilon_limit, epsilon_factor * (t + 0.5*max_v));
		min_eps = min(eps, min_eps);
		if (h < eps) break;
		
		avg_v = (prev_h + h) / 2.0;
		max_v = max(avg_v, max_v);
		prev_h = h;

		mt = min(mt, h);
		t += h*0.9;

		
	} ;
    
	if (t < focal_distance) res = t;
	return res;
}




vec3 ray_march(in vec3 ro, in vec3 rd, in float eps)
{
	vec4 trap;
	int steps;
	float iter;
	float min_dist;
	float min_eps;
	float max_v;
	float t = cast_ray(ro, rd, steps, eps, iter, trap, min_dist, min_eps, max_v );
	

	vec3 trap_col = hsv2rgb(vec3(length(trap.yzw)*2.0, .8, .8));
	vec3 col = trap_col;

	if (t < 0.0) {
		/* Color sky */
		//if (glow_enabled) col = glow(SKY_COLOR, col, min_dist);
		col = SKY_COLOR;

	} 
	else {
		// Calculate Lighting
		vec3 pos = ro + t*rd;
        vec3 nor = calculate_normal(pos, t);
		vec3 lightDir = -camera_direction;

		vec3 lightColor = vec3(1.0, 1.0, 1.0);

		float ambientStrength = 0.1;
		vec3 ambient = ambientStrength * lightColor;

		//float sha1 = soft_shadow( pos+0.001*nor, light1, 32.0 );
		float diff = max(dot(nor, lightDir), 0.0);
		vec3 diffuse = diff * lightColor;

		col *= (ambient + diffuse);

		//if (glow_enabled) col = glow(col, col, min_dist);
	}
	
	

	if (fog_enabled) col = applyFog(col, t);

	// Cheap ambient AO, but we use all our steps :C
	//col *= 1.0 - steps/max_steps;

	float focal_distance = max(max_dist*epsilon_limit, max_dist * scale);

	//if (heat_enabled) col = mix(vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0), float(steps)/float(max_steps));
	//if (heat_enabled) col = mix(vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0), t/(scale*max_dist));
	//if (heat_enabled) col = mix(vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0), min(1.0, epsilon_limit/eps));
	//if (heat_enabled) col = mix(vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0), t/focal_distance);
	if (heat_enabled) col = mix(vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0), 1.0 - min(1.0, max_v));
	//if (heat_enabled) col = mix(vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0), float(iter)/float(max_iter));
	//if (heat_enabled) col = mix(vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0), 1.0 - trap.w/max_bailout);

	//float eps = max(epsilon_limit, epsilon_factor*min_dist);
	//col = mix(col, trap_col, max(0.2, eps/min_dist));

	//col = mix(col, trap_col, max(0.2, float(steps)/float(max_steps)));

	//col = mix(col, trap_col, max(0.2, 1.0 - trap.w/max_bailout));
	col = mix(col, vec3(1.0, 1.0, 1.0), max(0.1, float(steps)/float(max_steps)));
	//col = mix(col, trap_col, max(0.1, iter/max_iter));
	

	return col;
}


void main()
{
    float fov_rad = fov * M_PI / 180.0;
	float px = (2 * (gl_FragCoord.x + 0.5) / screen_width - 1.0) * tan(fov_rad / 2) * aspect;
	float py = (1.0 - 2 * (gl_FragCoord.y + 0.5) / screen_height) * tan(fov_rad / 2); 

	vec3 camera_right = normalize(cross(camera_up, camera_direction));

    vec3 ro = camera_position;
	vec3 rd = normalize(camera_right * px + camera_up * py + camera_direction);

	
    //vec3 shaded_color = ray_march(ro.xyz, rd.xyz);
	float eps;
	float tmp;
	vec3 c = ray_march(ro.xyz, rd.xyz, eps);

	/*
	// anti-alias
	float n = 2.0;
	float epsilonf = 0.000000000001;
    for (float theta = 0.0; M_2PI - theta > epsilonf; theta += M_2PI/n)
	{
		vec3 p = ro.xyz + camera_right * eps * sin(theta);
		c = mix(c, ray_march(p, rd.xyz, tmp), 1.0/n);
	}
	*/
	
	o_color = vec4(c, 1.0);
}