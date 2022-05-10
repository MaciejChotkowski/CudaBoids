#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aCol;
layout (location = 4) in vec3 aOffset;
layout (location = 5) in vec3 aDirection;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec3 Col;
out float dist;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec3 dir = normalize(aDirection);
	mat4 rot;
	mat3 tmp;

	vec3 forward = vec3(-1, 0, 0);
	vec3 axis = normalize(cross(forward, dir));
	tmp[0] = vec3(0, axis.z, -axis.y);
	tmp[1] = vec3(-axis.z, 0, axis.z);
	tmp[2] = vec3(axis.y, -axis.z, 0);
	float theta = length(axis)/(length(forward) * length(dir));
	tmp = mat3(1) + sin(theta) * tmp + (1 - cos(theta)) * tmp * tmp;
	rot[0] = vec4(tmp[0], 0);
	rot[1] = vec4(tmp[1], 0);
	rot[2] = vec4(tmp[2], 0);
	rot[3] = vec4(0, 0, 0, 1);
	gl_Position = projection * view * model * (rot * vec4(aPos, 1.0f) + vec4(aOffset, 1.0f));
	dist = length((view * model * (rot * vec4(aPos, 1.0f) + vec4(aOffset, 1.0f))).xyz);

	TexCoord = aTexCoord;
	Normal = aNormal;
	FragPos = vec3(model * vec4(aPos, 1.0));

	Col = aCol;
}