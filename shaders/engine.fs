#version 100

precision mediump float;

varying vec4 fragColor;

uniform vec4 colDiffuse;
uniform float engineBrightness;

void main()
{
    vec4 baseColor = colDiffuse * fragColor;
    gl_FragColor = vec4(baseColor.rgb * engineBrightness, 1.0);
}