#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;
uniform mat4 MVP;
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;

// Wave Parameters
uniform float waveSpeed1;
uniform float waveSpeed2;
uniform float waveSpeed3;
uniform float waveAmplitude1;
uniform float waveAmplitude2;
uniform float waveAmplitude3;
uniform float waveFrequency1;
uniform float waveFrequency2;
uniform float waveFrequency3;

uniform float time;
out vec3 outPos;
out vec3 outNorm;
out vec2 outTex;
void main()
{
	vec3 pos = aPos;
	
	float wave1 = waveAmplitude1 * sin(waveFrequency1 * pos.x + waveSpeed1 * time);
	float wave2 = waveAmplitude2 * sin(waveFrequency2 * pos.z + waveSpeed2 * time);
	float wave3 = waveAmplitude3 * sin(waveFrequency3 * (pos.x + pos.z) + waveSpeed3 * time);

	pos.y += wave1 + wave2 + wave3;

	float dWave_dx = waveFrequency1 * waveAmplitude1 * cos(pos.x * waveFrequency1 + time * waveSpeed1) +
                     waveFrequency3 * waveAmplitude3 * cos((aPos.x + aPos.z) * waveFrequency3 + time * waveSpeed3);
    
    float dWave_dz = waveFrequency2 * waveAmplitude2 * cos(pos.z * waveFrequency2 + time * waveSpeed2) +
                     waveFrequency3 * waveAmplitude3 * cos((aPos.x + aPos.z) * waveFrequency3 + time * waveSpeed3);

	vec3 surfaceNormal = normalize(vec3(-dWave_dx, 1.0, -dWave_dz));

	gl_Position = MVP * vec4(pos, 1.0);
	outPos = vec3(modelMatrix * vec4(pos, 1.0));
	outNorm = mat3(normalMatrix) * surfaceNormal;
	outTex = aTex;
}