#version 330

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;

uniform float Alpha;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inTexCoord;

out vec3 vNormal;
smooth out vec4 kolorek;

out vec2 texCoord;

out vec3 Position;
out vec4 pos;
out vec3 Nurm;

out float fogCoord;

void main()
{	
	vec3 Nurm = inNormal;
	vec4 pos = modelViewMatrix*vec4(inPosition, 1.0f);
	gl_Position = projectionMatrix*pos;
	Position = vec3(gl_Position.x,gl_Position.y,gl_Position.z);
	vec4 vRes = normalMatrix*vec4(inPosition, 0.0);
	vNormal = vRes.xyz;
	kolorek = vec4(inColor,Alpha);
	texCoord = vec2(inTexCoord[0],inTexCoord[1]);	

	fogCoord = length(Position);
}