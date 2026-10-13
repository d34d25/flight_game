#version 100

precision highp float;

attribute vec3 vertexPosition;
attribute vec3 vertexNormal;
attribute vec4 vertexColor;

attribute vec2 vertexTexCoord;

uniform mat4 mvp;
uniform mat4 matModel;

varying vec3 fragPosition;

varying vec3 fragNormal;
varying vec4 fragColor;

varying vec2 fragTexCoord;

void main()
{
	gl_Position = mvp * vec4(vertexPosition, 1.0);
	fragColor = vertexColor;

	fragTexCoord = vertexTexCoord;

	fragNormal = mat3(matModel) * vertexNormal;

    fragPosition = vec3(matModel * vec4(vertexPosition, 1.0));
}