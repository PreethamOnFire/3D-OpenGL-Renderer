#version 330 core
out vec4 FragColor;
in vec3 outPos;
in vec3 outNorm;
in vec2 outTex;
void main()
{
    vec3 color = normalize(outNorm) * 0.5 + 0.5;
    FragColor = vec4(color, 1.0);
}