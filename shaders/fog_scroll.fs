#version 100

precision highp float;

varying vec3 fragPosition;

varying vec4 fragColor;

varying vec2 fragTexCoord;

uniform sampler2D texture0;

uniform vec4 fogColor;

uniform vec4 colDiffuse;

uniform vec3 viewPos;
uniform float fogDensity;

uniform float fogStart;
uniform float fogEnd;

uniform vec2 playerXZ;

uniform float scrollingFactor;

void main()
{
    float shadeCount = 16.0; 

    vec2 worldTexCoord = fragTexCoord + playerXZ * scrollingFactor;

    vec4 texColor = texture2D(texture0, worldTexCoord);

    vec4 baseColor = texColor * colDiffuse * fragColor;

    vec4 finalColor = vec4(baseColor.rgb * 1.0, 1.0);

    float dist = length(viewPos - fragPosition);

    float fogFactor = (fogEnd - dist) / (fogEnd - fogStart);

    fogFactor = clamp(fogFactor, 0.0, 1.0);

    float finalFactor = floor(fogFactor * shadeCount) / (shadeCount - 1.0);

    gl_FragColor = mix(fogColor, finalColor, finalFactor);
}