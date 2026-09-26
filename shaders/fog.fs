#version 100

precision highp float;

varying vec3 fragPosition;

varying vec3 fragNormal;
varying vec4 fragColor;

uniform vec4 fogColor;

uniform vec3 lightDir;
uniform vec4 colDiffuse;

uniform float minIntensity;
uniform float maxIntensity;

uniform vec3 viewPos;
uniform float fogDensity;

void main()
{
	vec3 norm = normalize(fragNormal);
	vec3 light = normalize(lightDir);

	float intensity = dot(norm, light);

	intensity = clamp(intensity, minIntensity, maxIntensity);

	vec4 baseColor = colDiffuse * fragColor;

    vec4 finalColor = vec4(baseColor.rgb * intensity, 1.0);

    //fog

    float dist = length(viewPos - fragPosition);

    float fogStart = 2500.0;
    float fogEnd = 4000.0;

    float fogFactor = (fogEnd - dist) / (fogEnd - fogStart);  //1.0 / exp((dist * fogDensity) * (dist * fogDensity));

    fogFactor = clamp(fogFactor, 0.0, 1.0);

    gl_FragColor = mix(fogColor, finalColor, fogFactor);
}