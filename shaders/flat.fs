#version 100

precision mediump float;

varying vec3 fragNormal;
varying vec4 fragColor;

uniform vec3 lightDir;
uniform vec4 colDiffuse;

uniform float minIntensity;
uniform float maxIntensity;

void main()
{
	vec3 norm = normalize(fragNormal);
	vec3 light = normalize(lightDir);

	float intensity = dot(norm, light);

	intensity = clamp(intensity, minIntensity, maxIntensity);

	vec4 baseColor = colDiffuse * fragColor;

	gl_FragColor = vec4(baseColor.rgb * intensity, 1.0); //vec4(1,0,0,1);
}