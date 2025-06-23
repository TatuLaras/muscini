#version 330

#define FREQ_COUNT 512

in vec2 fragTexCoord;
in vec2 fragCoord;
in vec4 fragColor;
out vec4 finalColor;
uniform sampler2D texture0;
uniform float screen_width;
uniform float screen_height;
uniform float progress;
uniform float time;
uniform float frequencies[FREQ_COUNT];

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

float range(int min, int max) {
    float val = 0;
    for (int i = 0; i < max; i++) {
        val += frequencies[i];
    }
    return val / (max - min);
}

void main() {
    vec2 up = vec2(0.5, 0);

    float height_to_width_ratio = screen_width / screen_height;
    vec2 middle = vec2(0.5 * height_to_width_ratio + sin(time) * 0.15, 0.5 + cos(time) * 0.15 * height_to_width_ratio);
    vec2 coords = vec2(fragTexCoord.x * height_to_width_ratio, fragTexCoord.y);
    float radius_ratio = 1.91;

    float distance_to_middle = 1 / (distance(coords, middle) + (sin(time) + 1.0) * 0.05);

    float angle = mod(vector_angle(coords - middle) + time + sin(distance_to_middle) * 0.2, PI);

    float distance_offset = mod(time * 2 + angle * 0.6, 1.0);
    distance_offset += sin(distance_to_middle) * 0.2;
    distance_offset += progress * 0.06;

    float prob = 0.98;
    float noise = 5 * (rand(coords) - prob);
    vec4 background = vec4(vec3(noise), 1.0);

    vec4 tunnel = texture(texture0, vec2(angle * radius_ratio, distance_to_middle + distance_offset));

    float red = 0.5 + range(0, 5) * 2;
    float blue = 0.5 + range(300, 420) * 12;

    red *= tunnel.x;
    blue *= tunnel.x;

    tunnel = vec4(red, 0, blue, 1.0);

    float opacity = 0.95;
    tunnel = mix(background, tunnel, opacity);
    tunnel = mix(tunnel, vec4(vec3(0.0), 1.0), clamp(distance_to_middle * 0.05, 0.0, 1.0));

    finalColor = tunnel;
}
