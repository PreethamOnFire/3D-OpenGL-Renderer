#version 330 core
in vec3 worldPos;
uniform vec3 lightPos;
uniform float farPlane;
void main()
{
   gl_FragDepth = length(worldPos - lightPos) / farPlane;
}
