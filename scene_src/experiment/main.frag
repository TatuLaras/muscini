#version 330

#define FREQ_COUNT 512

in vec2 fragTexCoord;
in vec2 fragCoord;
in vec4 fragColor;
out vec4 finalColor;
uniform sampler2D texture0;
uniform float screen_width;
uniform float screen_height;
uniform float beat;
uniform float time;

#define PI 3.141592653589793238

float vector_angle(vec2 a) {
    vec2 norm = normalize(a);
    float angle = asin(norm.y);

    if (norm.x > 0) return angle;
    return PI - angle;
}

float rand(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    float height_to_width_ratio = screen_width / screen_height;
    vec2 middle = vec2(0.5 * height_to_width_ratio, 0.5);
    vec2 coords = vec2(fragTexCoord.x * height_to_width_ratio, fragTexCoord.y);
    float radius_ratio = 1.91;
    float distance_to_middle = 1 / distance(coords, middle);
    float angle = mod(vector_angle(coords - middle) + time + distance_to_middle * 0.1, PI);
    float distance_offset = mod(time * 2, 1.0);
    distance_offset += sin(distance_to_middle) * 0.2;
    vec4 tunnel = texture(texture0, vec2(angle * radius_ratio, distance_to_middle + distance_offset));

    tunnel = mix(tunnel, vec4(vec3(1.0), 1.0), clamp(distance_to_middle * 0.10 * beat, 0.0, 1.0));

    tunnel -= 0.02;
    tunnel.x = (1.0 + sin(time)) * 0.4;
    finalColor = tunnel;
}
