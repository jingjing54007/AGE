#version 330

 out vec4 color;

uniform sampler2D light_buffer;
uniform sampler2D diffuse_map;
uniform sampler2D specular_map;

in vec2 interpolated_texCoord;

void main()
{
	vec3 diffuse = texture(diffuse_map, interpolated_texCoord).xyz;
	vec4 light = texture(light_buffer, interpolated_texCoord);
	vec3 specular = texture(specular_map, interpolated_texCoord).xyz;

	color = vec4(light.rgb * diffuse + vec3(light.a) /* * specular */, 1);
}