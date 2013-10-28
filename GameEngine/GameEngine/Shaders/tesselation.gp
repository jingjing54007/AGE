 #version 330
 #extension GL_ARB_geometry_shader4 : enable

layout(triangles) in;
layout(triangle_strip, max_vertices=6) out;

layout (std140) uniform PerFrame
{
	mat4 projection;
	mat4 view;
	vec4 light;
	float time;
};

layout (std140) uniform PerModel
{
	mat4 model;
};

uniform	sampler2D fDayTexture;
uniform	sampler2D fNightTexture;
uniform	sampler2D fClouds;
uniform	sampler2D fBump;

in vec4 gPosition[3];
in vec4 gColor[3];
in vec4 gNormal[3];
in vec2 gTexCoord[3];

out vec4 fPosition;
out vec4 fColor;
out vec4 fNormal;
out vec2 fTexCoord;

float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}
void main()
{
int		i;

  for (i = 0; i < gl_VerticesIn; i++)
  {
    gl_Position = projection * view * model * (gl_PositionIn[i] + vec4(cos(time * gl_PositionIn[i].x * 10) / 40, 0,
																	   cos(time * gl_PositionIn[i].z * 10) / 40, 0));
	fPosition = gPosition[i];
	fColor = gColor[i];
	fNormal = gNormal[i];
	fTexCoord = gTexCoord[i];
    EmitVertex();
  }
  EndPrimitive();
}