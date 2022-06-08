#version 450

layout(binding = 0) uniform sampler2D img;

layout(location = 0) in vec3 in_color;
layout(location = 1) in vec2 in_uv;

layout(location = 0) out vec4 out_color;

void main() {
	// out_color = vec4(in_color, 1.0);
	out_color = texture(img, in_uv);
}