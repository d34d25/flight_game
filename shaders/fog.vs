#version 100

precision highp float;

attribute vec3 vertexPosition;
attribute vec3 vertexNormal;
attribute vec4 vertexColor;

uniform mat4 mvp;
uniform mat4 matModel;

varying vec3 fragPosition;

varying vec3 fragNormal;
varying vec4 fragColor;

void main()
{
	gl_Position = mvp * vec4(vertexPosition, 1.0);
	fragColor = vertexColor;

	fragNormal = mat3(matModel) * vertexNormal;

    fragPosition = vec3(matModel * vec4(vertexPosition, 1.0));
}