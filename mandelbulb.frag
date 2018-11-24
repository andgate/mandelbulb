#version 330

#define M_PI 3.1415926535897932384626433832795

uniform vec3 camera_position;
uniform vec3 camera_direction;
uniform vec3 camera_up;

uniform vec3 sphere_center;
uniform float sphere_radius;

uniform float aspect;
uniform float fov;
uniform float zoom;

uniform float screen_width;
uniform float screen_height;

in vec3 Color;
out vec4 o_color;

float Bailout = 4.0;
int Iterations = 4;


vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float sdfMandelbulb(vec3 p, out vec4 pixelColor)
{
	vec3 q = p;
	float m = dot(q,q);
	float dr = 1.0;
	vec4 trap = vec4(abs(q), m);

	for (int i = 0; i < Iterations; ++i)
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
		if( m > 256.0 )
            break;
	}

	pixelColor = normalize(vec4(hsv2rgb(vec3(m, 0.8, 0.8)), 1.0f) + trap.xyzw);
	return 0.25*log(m)*sqrt(m)/dr;
}

float map(in vec3 p, out vec4 pixelColor)
{
	float bulb_distance = sdfMandelbulb(p, pixelColor);
    return bulb_distance;
}

vec3 calculate_normal(in vec3 p, in float t, in float px)
{
	vec4 tmp;
    const vec3 e = vec2(1.0, -1.0)*0.5773*0.25*px;
    return normalize( e.xyy*map(p + e.xyy, tmp) +
	                  e.yyx*map(p + e.xyy, tmp) +
					  e.yxy*map(p + e.yxy, tmp) +
				      e.xxx*map(p + e.xxx, tmp) );
}


const vec3 light1 = vec3(  0.577, 0.577, -0.577 );
const vec3 light2 = vec3( -0.707, 0.000,  0.707 );

vec4 lighting(in vec4 color)
{
	vec3 normal = calculate_normal(current_position);
	vec3 light_position = vec3(2.0, -5.0, 3.0);
	vec3 direction_to_light = normalize(current_position - light_position);

	float diffuse_intensity = max(0.0, dot(normal, direction_to_light));

	float specular = pow(diffuse_intensity, 64.0);

	return pixelColor.xyz * (diffuse_intensity + specular);
}

vec3 cast_ray(in vec3 ro, in vec3 rd, vec4 color, float px)

vec3 ray_march(in vec3 ro, in vec3 rd)
{
	vec4 pixelColor;
	float camera_distance = abs(map(ro, pixelColor));
	float shrink = sqrt(camera_distance)/zoom;

    float total_distance_traveled = 0.0;
    int NUMBER_OF_STEPS = int(200.0/shrink);
    float MINIMUM_HIT_DISTANCE = 0.001 * shrink;     // these should be computed
    float MAXIMUM_TRACE_DISTANCE = 5.0;              // from zoom or

	cast_ray

    for (int i = 0; i < NUMBER_OF_STEPS; ++i)
    {
        vec3 current_position = ro + total_distance_traveled * rd;

        float distance_to_closest = abs(map(current_position, pixelColor));
        if (distance_to_closest < MINIMUM_HIT_DISTANCE) 
        {
            vec3 normal = calculate_normal(current_position);
            vec3 light_position = vec3(2.0, -5.0, 3.0);
            vec3 direction_to_light = normalize(current_position - light_position);

            float diffuse_intensity = max(0.0, dot(normal, direction_to_light));

			float specular = pow(diffuse_intensity, 64.0);

            return pixelColor.xyz * (diffuse_intensity + specular);
        }

        if (total_distance_traveled > MAXIMUM_TRACE_DISTANCE)
        {
            return vec3(0.7);
        }
        total_distance_traveled += distance_to_closest;
    }
    return vec3(0.7);
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