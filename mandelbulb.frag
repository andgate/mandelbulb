#version 330

uniform mat4 camera_combined;

uniform vec3 camera_position;
uniform vec3 camera_direction;
uniform vec3 camera_up;

uniform float screen_width;
uniform float screen_height;

in vec3 Color;
out vec4 o_color;

float Power = 8.0;
float Bailout = 3.0;
int Iterations = 150;


float distance_from_sphere(in vec3 p, in vec3 c, float r)
{
    return length(p - c) - r;
}

float distance_from_box(in vec3 p, in vec3 b)
{
	vec3 d = abs(p) - b;
	return length( max(d,0.0)) + min(max(d.x,max(d.y,d.z)),0.0);
}

float sdeMandelbulb(vec3 p)
{
	vec3 z = p;
	float dr = 1.0;
	float r = 0.0;
	for (int i = 0; i < Iterations ; i++) {
		r = length(z);
		if (r>Bailout) break;
		
		// convert to polar coordinates
		float theta = acos(z.z/r);
		float phi = atan(z.y,z.x);
		dr =  pow( r, Power-1.0)*Power*dr + 1.0;
		
		// scale and rotate the point
		float zr = pow( r,Power);
		theta = theta*Power;
		phi = phi*Power;
		
		// convert back to cartesian coordinates
		z = zr*vec3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta));
		z+=p;
	}
	return 0.5*log(r)*r/dr;
}

float map_the_world(in vec3 p)
{
    //float sphere_0 = distance_from_sphere(p, vec3(0.0), 1.0);

	//float box_0 = distance_from_box(p, vec3(.4, .4, .4));

	//float shape = distance_from_sphere(p, vec3(0.0), 1.0); 
	float shape = sdeMandelbulb(p);

    return shape;
}

vec3 calculate_normal(in vec3 p)
{
    const vec3 small_step = vec3(0.001, 0.0, 0.0);

    float gradient_x = map_the_world(p + small_step.xyy) - map_the_world(p - small_step.xyy);
    float gradient_y = map_the_world(p + small_step.yxy) - map_the_world(p - small_step.yxy);
    float gradient_z = map_the_world(p + small_step.yyx) - map_the_world(p - small_step.yyx);

    vec3 normal = vec3(gradient_x, gradient_y, gradient_z);

    return normalize(normal);
}

vec3 ray_march(in vec3 ro, in vec3 rd)
{
    float total_distance_traveled = 0.0;
    const int NUMBER_OF_STEPS = 40;
    const float MINIMUM_HIT_DISTANCE = 0.001;     // these should be computed
    const float MAXIMUM_TRACE_DISTANCE = 1000.0;  // from zoom or 

    for (int i = 0; i < NUMBER_OF_STEPS; ++i)
    {
        vec3 current_position = ro + total_distance_traveled * rd;

        float distance_to_closest = map_the_world(current_position);

        if (distance_to_closest < MINIMUM_HIT_DISTANCE) 
        {
            vec3 normal = calculate_normal(current_position);
            vec3 light_position = vec3(2.0, -5.0, 3.0);
            vec3 direction_to_light = normalize(current_position - light_position);

            float diffuse_intensity = max(0.0, dot(normal, direction_to_light));

			float specular = pow(diffuse_intensity, 32.0);

            return vec3(1.0, 0.0, 0.0) * (diffuse_intensity + specular);
        }

        if (total_distance_traveled > MAXIMUM_TRACE_DISTANCE)
        {
            return vec3(0.7);
        }
        total_distance_traveled += distance_to_closest;
    }
    return vec3(0.0);
}

void main()
{
    vec2 screen_size = vec2(screen_width, screen_height);
	vec2 screen_position = 2 * gl_FragCoord.xy / screen_size.xy - 1.0;
	//screen_position.x *= screen_size.x / screen_size.y;

	vec3 camera_right = normalize(cross(camera_direction, camera_up));

    vec3 ro = camera_position;
    vec3 rd = normalize(camera_right * screen_position.x + camera_up * screen_position.y + camera_direction);

    vec3 shaded_color = ray_march(ro, rd);

    o_color = vec4(shaded_color, 1.0);
}