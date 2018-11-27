#version 400

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
uniform float epsilon_limit;

uniform float max_dist;
uniform float max_bailout;
uniform int max_iter;
uniform int min_iter;
uniform int max_steps;
uniform int power;

uniform float fog_max_dist;
uniform bool fog_enabled;

vec3 SKY_COLOR = vec3(1.0,1.0,1.0);

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


float sdfMandelbulb(vec3 p, in int power, out vec4 pixelColor)
{
	vec3 q = p;
	float r = length(q);
	float dr = 1.0;
	vec4 trap = vec4(abs(q), r);

	//float miter = mix(min_iter, max_iter, 1.0 - scale);
	int i = max_iter;
	while (r < max_bailout && i-- > 0)
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

		trap = min( trap, vec4(abs(p),r) );

		r = length(q);
	}

	pixelColor = vec4(r, trap.yzw);
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

float map(in vec3 p, out vec4 pixelColor)
{
	float bulb_distance = sdfMandelbulb(p, power, pixelColor);
    return bulb_distance;
}

vec3 calculate_normal(in vec3 p, in float mdist)
{
    vec4 tmp;
    float e = max(epsilon_limit, epsilon_factor * mdist);
	return normalize(vec3(
        map(vec3(p.x + e, p.y, p.z), tmp) - map(vec3(p.x - e, p.y, p.z), tmp),
        map(vec3(p.x, p.y + e, p.z), tmp) - map(vec3(p.x, p.y - e, p.z), tmp),
        map(vec3(p.x, p.y, p.z  + e), tmp) - map(vec3(p.x, p.y, p.z - e), tmp)
    ));
}

vec3 calculate_normal_fast(in vec3 p, in float mdist)
{
    vec4 tmp;
    vec2 e = vec2(1.0, -1.0) * max(epsilon_limit, epsilon_factor * mdist);;
    return normalize( e.xyy*map(p + e.xyy, tmp) +
	                  e.yyx*map(p + e.xyy, tmp) +
					  e.yxy*map(p + e.yxy, tmp) +
				      e.xxx*map(p + e.xxx, tmp) );
}


vec3 applyFog( in vec3  rgb,       // original color of the pixel
               in float distance ) // camera to point distance
{
	float fog_dist = distance / max(fog_max_dist*epsilon_limit, fog_max_dist*scale);
	float fogAmount = min(1.0, fog_dist);
    return mix( rgb, SKY_COLOR, fogAmount );
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

	// raymarch fractal distance field
	vec4 trap;
	float t = dis.x;
	float viewlimit = max(max_dist*epsilon_limit, max_dist * scale);
	while (t < viewlimit || steps_taken++ < max_steps) {
		vec3 pos = ro + rd*t;
		float h = map( pos, trap );
		float eps = max(epsilon_limit, 1.0*epsilon_factor*t);
		if (t > dis.y || t > viewlimit || h*(0.9) < eps) break;
		t += h*0.9;
	}
    
	if (t < dis.y && t < viewlimit) {
		color = trap;
		res = t;
	}

	return res;
}


vec3 glow(in vec3 col, in vec3 glowColor, in float strength)
{
	return mix(col, glowColor, strength); 
}

vec3 ray_march(in vec3 ro, in vec3 rd)
{
	vec4 tra;
	int steps_taken;
	float t = cast_ray(ro, rd, max_steps, tra, steps_taken);

	vec3 col;
	col = hsv2rgb(vec3(length(tra.yzw), .8, .8));

	if (t < 0.0) {
		/* Color sky */
     	col = SKY_COLOR;
	} else {
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
		//col = glow(col, vec3(1.0, 1.0, 0.0), steps_taken/max_steps);
		
		if (fog_enabled) col = applyFog(col, t);
	}

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

    vec3 shaded_color = ray_march(ro.xyz, rd.xyz);
    o_color = vec4(shaded_color, 1.0);
}