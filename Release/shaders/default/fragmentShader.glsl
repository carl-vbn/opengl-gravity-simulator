#version 330 core

// DEFAULT FRAGMENT SHADER USED TO RENDER BODIES AND OTHER THINGS IN 3D SPACE

// Interpolated values from the vertex shaders
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in float visibility;

// Ouput data
layout(location = 0) out vec3 color;
layout(location = 1) out vec3 emission;

// Values that stay constant for the whole mesh.
uniform mat4 MV;
uniform bool Unlit;
uniform bool Emissive;
uniform vec3 LightPosition_worldspace;
uniform vec3 LightColor;
uniform vec3 ModelColor;

// For shadow raymarching. An occluder is a body that can cast a shadow on the currently rendered object.
uniform int OccluderCount;
uniform vec3 OccluderPositions[4];
uniform float OccluderRadiuses[4];
uniform float LightRadius;

const float MAX_RAYMARCH_DISTANCE = 1000.0F;
const float MIN_LIGHT_LEVEL = 0.1;

float estimateOccluderDistance(vec3 p) {
	float minDist = MAX_RAYMARCH_DISTANCE;

	for (int i = 0; i < OccluderCount; i++) {
		minDist = min(minDist, distance(p, OccluderPositions[i]) - OccluderRadiuses[i]);
	}

	return minDist;
}

void main() {
	if (Unlit) {
		color = ModelColor * visibility;
	} else {
		// Light emission properties
		float LightPower = 25.0f;

		// Material properties
		vec3 MaterialDiffuseColor = ModelColor;
		vec3 MaterialAmbientColor = vec3(MIN_LIGHT_LEVEL) * MaterialDiffuseColor;
		vec3 MaterialSpecularColor = vec3(0);

		// Distance to the light
		float distance = length(LightPosition_worldspace - Position_worldspace);

		// Normal of the computed fragment, in camera space
		vec3 n = normalize(Normal_cameraspace);
		// Direction of the light (from the fragment to the light)
		vec3 l = normalize(LightDirection_cameraspace);
		// Cosine of the angle between the normal and the light direction, 
		// clamped above 0
		//  - light is at the vertical of the triangle -> 1
		//  - light is perpendicular to the triangle -> 0
		//  - light is behind the triangle -> 0
		float cosTheta = clamp(dot(n, l), 0, 1);

		// Eye vector (towards the camera)
		vec3 E = normalize(EyeDirection_cameraspace);
		// Direction in which the triangle reflects the light
		vec3 R = reflect(-l, n);
		// Cosine of the angle between the Eye vector and the Reflect vector,
		// clamped to 0
		//  - Looking into the reflection -> 1
		//  - Looking elsewhere -> < 1
		float cosAlpha = clamp(dot(E, R), 0.0, 1.0);

		// Shadow raymarching
		float occlusion = 0.0;

		if (dot(l, n) > 0.0 && OccluderCount > 0) { // Don't compute occlusion if fragment is facing away from the light or if there isn't any occluder
			vec3 toLight = normalize(LightPosition_worldspace - Position_worldspace);
			float distToLight = length(LightPosition_worldspace - Position_worldspace);
			float d = LightRadius * 0.1;
			while (d < MAX_RAYMARCH_DISTANCE)
			{
				float ed = estimateOccluderDistance(Position_worldspace + d * toLight);
				occlusion = max(0.5 + (-ed) * distToLight / (2.0 * LightRadius * d), occlusion);
				if (occlusion >= 1.0) break;

				d += max(ed, LightRadius * d / distToLight);
				if (d >= distToLight) break;
			}
			occlusion = clamp(occlusion, 0.0, 1.0);
		}

		color =
			(max(MaterialDiffuseColor * LightColor * cosTheta * (1.0 - occlusion), MaterialAmbientColor) // Diffuse, shadow and ambient
			+ MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha, 5) / (distance * distance)) // Specular
			* visibility;
	}

	if (Emissive) {
		emission = ModelColor;
	}
}