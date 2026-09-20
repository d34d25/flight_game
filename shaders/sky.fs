#version 100

precision mediump float;

varying vec3 fragPosition;
varying vec2 fragTexCoord;

uniform vec4 baseColor;
uniform vec4 topColor;
uniform vec4 bottomColor;

uniform float minHeight;
uniform float maxHeight;

void main()
{
    float shadeCount = 128.0;

    float factor = clamp((fragPosition.y - minHeight) / (maxHeight - minHeight), 0.0, 1.0);

    float finalFactor = floor(factor * shadeCount) / (shadeCount - 1.0);

    vec4 gradient = mix(bottomColor, topColor, finalFactor);

    gl_FragColor = baseColor * gradient;
}