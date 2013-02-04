/*
**  ClanLib SDK
**  Copyright (c) 1997-2012 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
**    Mark Page
*/

#pragma once

#include "pixel_converter_impl.h"

namespace clan
{

class PixelWriter_4hf : public PixelWriter
{
public:
	void write(void *output, Vec4f *input, int num_pixels)
	{
		Vec4hf *d = static_cast<Vec4hf *>(output);
		for (int i = 0; i < num_pixels; i++)
		{
			d[i] = Vec4hf(input[i]);
		}
	}
};

class PixelWriter_3hf : public PixelWriter
{
public:
	void write(void *output, Vec4f *input, int num_pixels)
	{
		Vec3hf *d = static_cast<Vec3hf *>(output);
		for (int i = 0; i < num_pixels; i++)
		{
			d[i] = Vec3hf(input[i]);
		}
	}
};

class PixelWriter_2hf : public PixelWriter
{
public:
	void write(void *output, Vec4f *input, int num_pixels)
	{
		Vec2hf *d = static_cast<Vec2hf *>(output);
		for (int i = 0; i < num_pixels; i++)
		{
			d[i] = Vec2hf(input[i]);
		}
	}
};

class PixelWriter_1hf : public PixelWriter
{
public:
	void write(void *output, Vec4f *input, int num_pixels)
	{
		HalfFloat *d = static_cast<HalfFloat *>(output);
		for (int i = 0; i < num_pixels; i++)
		{
			d[i] = HalfFloat(input[i].x);
		}
	}
};

}
