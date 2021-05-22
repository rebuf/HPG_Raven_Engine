/*
 * Developed by Raven Group at the University  of Leeds
 * Copyright (C) 2021 Ammar Herzallah, Ben Husle, Thomas Moreno Cooper, Sulagna Sinha & Tian Zeng
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * THIS PROGRAM IS DISTRIBUTED IN THE HOPE THAT IT WILL BE USEFUL,
 * BUT WITHOUT ANY WARRANTY; WITHOUT EVEN THE IMPLIED WARRANTY OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  SEE THE
 * GNU GENERAL PUBLIC LICENSE FOR MORE DETAILS.
 */
#include "ImageImporter.h"
#include "Utilities/StringUtils.h"

#include "Logger/Console.h"
#include "ResourceManager/Resources/Texture2D.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



namespace Raven {


ImageImporter::ImageImporter()
{
	type = StaticGetType();
}


void ImageImporter::ListExtensions(std::vector<std::string>& outExt)
{
	outExt.push_back("png");
	outExt.push_back("jpg");
	outExt.push_back("jpeg");
}


bool ImageImporter::Import(const std::string& path, std::vector< Ptr<IResource> >& resources)
{
	// the extension of the file we want to import.
	std::string ext = StringUtils::GetExtension(path);

	if (ext == "png" || ext == "jpg" || ext == "jpeg")
	{
		IResource* img2D = ImportImage2D(path);

		// Failed to load image2D?
		if (!img2D)
			return false;

		resources.emplace_back(img2D);
	}

	return true;
}


IResource* ImageImporter::ImportImage2D(const std::string& path)
{
	int width = 0;
	int height = 0;
	int channelsInFile = 0;

	// unique pointer containing data from a file, with the deleter provided by stbi
	std::unique_ptr<stbi_uc, std::function<void(void*)>> fromFile(
		stbi_load(path.c_str(), &width, &height, &channelsInFile, STBI_default), stbi_image_free
	);

	// no data...
	if (!fromFile.get())
		return nullptr;


	// Find the foramt...
	ETextureFormat format;

	switch (channelsInFile)
	{
	case STBI_grey:
		format = ETextureFormat::R8;
		break;

	case STBI_rgb:
		format = ETextureFormat::RGB24;
		break;

	case STBI_rgb_alpha:
		format = ETextureFormat::RGBA32;
		break;

	// Invalid Format?
	default:
		LOGE("Failed to import image file, invalid format {0}.", path.c_str());
		return nullptr;
	}


	Texture2D* texture = new Texture2D();
	texture->SetImageData(format, glm::ivec2(width, height), fromFile.get());
	texture->SetFitler(ETextureFilter::Linear);
	texture->SetWrap(ETextureWrap::Repeat);
	texture->SetGenMipmaps(true);

	// Render Resrouce...
	texture->LoadRenderResource();

	texture->SetName(StringUtils::GetFileNameWithoutExtension(path));

	return texture;
}




} // End of namespace Raven.
