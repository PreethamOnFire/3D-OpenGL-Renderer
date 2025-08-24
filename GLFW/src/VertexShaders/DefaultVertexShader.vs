#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;
uniform mat4 MVP;
uniform mat4 normalMatrix;
out vec3 outPos;
out vec3 outNorm;
out vec2 outTex;
void main()
{
   gl_Position = MVP * vec4(aPos, 1.0);
   outPos = aPos;
   outNorm = mat3(normalMatrix) * aNorm;
   outTex = aTex;
}