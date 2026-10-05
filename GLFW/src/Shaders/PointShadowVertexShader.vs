#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 MVP;
uniform mat4 modelMatrix;
out vec3 worldPos;
void main()
{
   worldPos = vec3(modelMatrix * vec4(aPos, 1.0));
   gl_Position = MVP * vec4(aPos, 1.0);
}
