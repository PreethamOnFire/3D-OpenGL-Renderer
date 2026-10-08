#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec4 aTangent; // xyz = tangent, w = bitangent handedness
uniform mat4 MVP;
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;
out vec3 outPos;
out vec3 outNorm;
out vec2 outTex;
out mat3 outTBN; // tangent space -> world space
void main()
{
   gl_Position = MVP * vec4(aPos, 1.0);
   outPos = vec3(modelMatrix * vec4(aPos, 1.0));
   outNorm = mat3(normalMatrix) * aNorm;
   outTex = aTex;

   
   vec3 N = normalize(outNorm);
   vec3 T = normalize(mat3(modelMatrix) * aTangent.xyz);
   T = normalize(T - dot(T, N) * N);

   vec3 Bmodel = mat3(modelMatrix) * (cross(aNorm, aTangent.xyz) * aTangent.w);
   vec3 B = cross(N, T);
   B *= dot(B, Bmodel) < 0.0 ? -1.0 : 1.0;

   outTBN = mat3(T, B, N);
}
