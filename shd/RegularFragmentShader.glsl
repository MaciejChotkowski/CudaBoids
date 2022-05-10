#version 460 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in float dist;

const float density = 0.0002;
const float gradient = 1;

uniform sampler2D texture1;
uniform vec3 clearColor;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(vec3(0, 2500, -1100) - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(1, 1, 1) + vec3(0.1, 0.1, 0.1);
	float lightness = exp(-pow(dist*density, gradient));
	FragColor = vec4(diffuse, 1) * texture(texture1, TexCoord);
	FragColor = mix(vec4(clearColor, 1), FragColor, lightness);
}