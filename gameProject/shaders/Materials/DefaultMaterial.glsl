// Materail Texture Samplers...
uniform sampler2D inCheckerTexture;




void ComputeMaterial(in MaterialData inData, out MaterialOutput outParams)
{
	vec3 color = texture(inCheckerTexture, inData.texCoord * 4.0).rgb;
	
	outParams.color = color;
	outParams.emission = vec3(0.0);
	outParams.roughness = 0.4;
	outParams.metallic = 0.0;
	outParams.specular = 1.0;
	
	// Default...
	outParams.normal = inData.normal;
}

