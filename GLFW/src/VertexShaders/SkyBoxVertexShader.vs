#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;


out vec3 outTex;
void main()
{
   vec4 pos = projectionMatrix * viewMatrix * vec4(aPos.x, -aPos.y, aPos.z, 1.0);
   gl_Position = pos.xyww;
   outTex = aPos;
}