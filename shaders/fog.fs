#version 100

precision highp float;

varying vec3 fragPosition;

varying vec3 fragNormal;
varying vec4 fragColor;

varying vec2 fragTexCoord;

uniform sampler2D texture0;

uniform vec4 fogColor;

uniform vec3 lightDir;
uniform vec4 colDiffuse;

uniform float minIntensity;
uniform float maxIntensity;

uniform vec3 viewPos;
uniform float fogDensity;

uniform float fogStart;
uniform float fogEnd;

void main()
{
	//vec3 norm = normalize(fragNormal);
	//vec3 light = normalize(lightDir);

	//float intensity = dot(norm, light);

	//intensity = clamp(intensity, minIntensity, maxIntensity);

	//vec4 baseColor = colDiffuse * fragColor;

    //vec4 finalColor = vec4(baseColor.rgb * intensity, 1.0);

    //fog

    float shadeCount = 16.0;

    vec4 texColor = texture2D(texture0, fragTexCoord);

    vec4 baseColor = texColor * colDiffuse * fragColor;

    vec4 finalColor = vec4(baseColor.rgb * 1.0, 1.0);

    float dist = length(viewPos - fragPosition);

    float fogFactor = (fogEnd - dist) / (fogEnd - fogStart);  //1.0 / exp((dist * fogDensity) * (dist * fogDensity));

    fogFactor = clamp(fogFactor, 0.0, 1.0);

    float finalFactor = floor(fogFactor * shadeCount) / (shadeCount - 1.0);

    gl_FragColor = mix(fogColor, finalColor, finalFactor);
}