/*
**  ClanLib SDK
**  Copyright (c) 1997-2013 The ClanLib Team
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
*/

#include "Display/precomp.h"
#include "API/Display/Font/font.h"
#include "API/Display/Font/font_metrics.h"
#include "API/Display/Font/font_description.h"
#include "API/Display/TargetProviders/graphic_context_provider.h"
#include "API/Core/IOData/path_help.h"
#include "API/Core/Text/string_help.h"
#include "API/Core/Text/string_format.h"
#include "API/Core/Text/utf8_reader.h"
#include "API/Core/XML/dom_element.h"
#include "API/Display/2D/canvas.h"
#include "API/Display/Resources/display_cache.h"
#include "font_impl.h"

namespace clan
{

/////////////////////////////////////////////////////////////////////////////
// Font Construction:

Font::Font()
{
}

Font::Font(Canvas &canvas, const std::string &typeface_name, int height)
{
	FontDescription desc;
	desc.set_typeface_name(typeface_name);
	desc.set_height(height);
	*this = Font(canvas, desc);
}

Font::Font( Canvas &canvas, const FontDescription &desc)
{
	*this = Font(canvas, desc, "");
}

  Font::Font( Canvas &canvas, const FontDescription &desc, const std::string &ttf_filename) : impl(std::make_shared<Font_Impl>())
{
  impl->load_font( canvas, desc, ttf_filename );
}

  Font::Font( Canvas &canvas, const FontDescription &desc, const std::string &ttf_filename, FileSystem fs) : impl(std::make_shared<Font_Impl>())
{
  impl->load_font( canvas, desc, ttf_filename, fs );
}


Font::Font( Canvas &canvas, Sprite &sprite, const std::string &glyph_list, int spacelen, bool monospace, const FontMetrics &metrics) : impl(std::make_shared<Font_Impl>())
{
	impl->load_font(canvas, sprite, glyph_list, spacelen, monospace, metrics);
}

/////////////////////////////////////////////////////////////////////////////
// Font Resources:

Resource<Font> Font::resource(Canvas &canvas, const FontDescription &desc, const ResourceManager &resources)
{
	return DisplayCache::get(resources).get_font(canvas, desc);
}


Font Font_Impl::load(Canvas &canvas, const FontDescription &reference_desc, const std::string &id, const XMLResourceDocument &doc, std::function<Resource<Sprite>(Canvas &, const std::string &)> cb_get_sprite)
{
	XMLResourceNode resource = doc.get_resource(id);
	std::string type = resource.get_element().get_tag_name();
	
	if (type != "font")
		throw Exception(string_format("Resource '%1' is not of type 'font'", id));

	DomElement sprite_element = resource.get_element().named_item("sprite").to_element();

	if (!sprite_element.is_null())
	{
		if (!sprite_element.has_attribute("glyphs")) 
			throw Exception(string_format("Font resource %1 has no 'glyphs' attribute.", resource.get_name()));
		
		if (!sprite_element.has_attribute("letters")) 
			throw Exception(string_format("Font resource %1 has no 'letters' attribute.", resource.get_name()));

		Resource<Sprite> spr_glyphs = cb_get_sprite(canvas, sprite_element.get_attribute("glyphs"));

		const std::string &letters = sprite_element.get_attribute("letters");

		int spacelen = StringHelp::text_to_int(sprite_element.get_attribute("spacelen", "-1"));
		bool monospace = StringHelp::text_to_bool(sprite_element.get_attribute("monospace", "false"));

		// Modify the default font metrics, if specified

		float height = 0.0f;
		float line_height = 0.0f;
		float ascent = 0.0f;
		float descent = 0.0f;
		float internal_leading = 0.0f;
		float external_leading = 0.0f;

		if (sprite_element.has_attribute("height")) 
			height = StringHelp::text_to_float(sprite_element.get_attribute("height", "0"));

		if (sprite_element.has_attribute("line_height"))
			line_height = StringHelp::text_to_float(sprite_element.get_attribute("line_height", "0"));

		if (sprite_element.has_attribute("ascent")) 
			ascent = StringHelp::text_to_float(sprite_element.get_attribute("ascent", "0"));

		if (sprite_element.has_attribute("descent")) 
			descent = StringHelp::text_to_float(sprite_element.get_attribute("descent", "0"));

		if (sprite_element.has_attribute("internal_leading")) 
			internal_leading = StringHelp::text_to_float(sprite_element.get_attribute("internal_leading", "0"));

		if (sprite_element.has_attribute("external_leading")) 
			external_leading = StringHelp::text_to_float(sprite_element.get_attribute("external_leading", "0"));

		FontMetrics font_metrics(height, line_height, ascent, descent, internal_leading, external_leading);

		return Font(canvas, spr_glyphs.get(), letters, spacelen, monospace, font_metrics);
	}

	DomElement ttf_element = resource.get_element().named_item("ttf").to_element();
	if (ttf_element.is_null())
		ttf_element = resource.get_element().named_item("freetype").to_element();

	if (!ttf_element.is_null())
	{
		FontDescription desc = reference_desc.clone();

		std::string filename;

		

		if (ttf_element.has_attribute("file"))
		  {
		    filename = PathHelp::combine(resource.get_base_path(), ttf_element.get_attribute("file"));
		    desc.set_typeface_name(ttf_element.get_attribute("file"));
		  }

		if (ttf_element.has_attribute("typeface"))
			desc.set_typeface_name(ttf_element.get_attribute("typeface"));

		if (ttf_element.has_attribute("height"))
			desc.set_height(ttf_element.get_attribute_int("height", 0));

		if (ttf_element.has_attribute("average_width"))
			desc.set_average_width(ttf_element.get_attribute_int("average_width", 0));

		if (ttf_element.has_attribute("anti_alias"))
			desc.set_anti_alias(ttf_element.get_attribute_bool("anti_alias", true));

		if (ttf_element.has_attribute("subpixel"))
			desc.set_subpixel(ttf_element.get_attribute_bool("subpixel", true));

		return Font(canvas, desc, filename, resource.get_file_system());
	}

	throw Exception(string_format("Font resource %1 did not have a <sprite> or <ttf> child element", resource.get_name()));

}


/////////////////////////////////////////////////////////////////////////////
// Font Attributes:

bool Font::is_null() const
{
	return !impl;
}

/////////////////////////////////////////////////////////////////////////////
// Font Operations:

GlyphMetrics Font::get_metrics(Canvas &canvas, unsigned int glyph)
{
	if (impl)
		return impl->glyph_cache.get_metrics(impl->font_engine, canvas, glyph);
	return GlyphMetrics();
}

GlyphMetrics Font::measure_text(Canvas &canvas, const std::string &string)
{
	GlyphMetrics total_metrics;

	if (!impl)
		return total_metrics;

	int line_spacing = impl->glyph_cache.font_metrics.get_height() + impl->glyph_cache.font_metrics.get_external_leading();

	UTF8_Reader reader(string.data(), string.length());
	while (!reader.is_end())
	{
		unsigned int glyph = reader.get_char();
		reader.next();

		if (glyph == '\n')
		{
			total_metrics.advance.width = 0;
			total_metrics.advance.height += line_spacing;
			continue;
		}

		GlyphMetrics metrics = impl->glyph_cache.get_metrics(impl->font_engine, canvas, glyph);

		float bb_left = clan::min(total_metrics.bbox_offset.x, metrics.bbox_offset.x + total_metrics.advance.width);
		float bb_top = clan::min(total_metrics.bbox_offset.y, metrics.bbox_offset.y + total_metrics.advance.height);
		float bb_right = clan::max(total_metrics.bbox_offset.x + total_metrics.bbox_size.width, metrics.bbox_offset.x + metrics.bbox_size.width + total_metrics.advance.width);
		float bb_bottom = clan::max(total_metrics.bbox_offset.y + total_metrics.bbox_size.height, metrics.bbox_offset.y + metrics.bbox_size.height + total_metrics.advance.height);

		total_metrics.bbox_offset.x = bb_left;
		total_metrics.bbox_offset.y = bb_top;
		total_metrics.bbox_size.width = bb_right - bb_left;
		total_metrics.bbox_size.height = bb_bottom - bb_top;

		total_metrics.advance += metrics.advance;
	}
	return total_metrics;
}

size_t Font::clip_from_left(Canvas &canvas, const std::string &text, float width)
{
	float x = 0.0f;
	UTF8_Reader reader(text.data(), text.length());
	while (!reader.is_end())
	{
		unsigned int glyph = reader.get_char();
		GlyphMetrics char_abc = get_metrics(canvas, glyph);

		if (x + char_abc.advance.width > width)
			return reader.get_position();

		x += char_abc.advance.width;
		reader.next();
	}

	return text.size();
}

size_t Font::clip_from_right(Canvas &canvas, const std::string &text, float width)
{
	float x = 0.0f;
	UTF8_Reader reader(text.data(), text.length());
	reader.set_position(text.length());
	while (reader.get_position() != 0)
	{
		reader.prev();

		unsigned int glyph = reader.get_char();
		GlyphMetrics char_abc = get_metrics(canvas, glyph);

		if (x + char_abc.advance.width > width)
		{
			reader.next();
			return reader.get_position();
		}

		x += char_abc.advance.width;
	}

	return 0;
}

void Font::draw_text(Canvas &canvas, const Pointf &position, const std::string &text, const Colorf &color)
{
	if (impl)
		impl->glyph_cache.draw(impl->font_engine, canvas, position, text, color);
}

void Font::draw_text_ellipsis(Canvas &canvas, const Pointf &position, Rectf content_box, const std::string &text, const Colorf &color)
{
	//FIXME!
	draw_text(canvas, position, text, color);
	/*
	if (impl)
	{
		Pointf pos = canvas.grid_fit(Pointf(dest_x, dest_y));
		dest_x = pos.x;
		dest_y = pos.y;

		FontMetrics fm = get_font_metrics();
		int ascent = fm.get_ascent();
		int descent = fm.get_descent();
		int line_spacing = fm.get_height() + fm.get_external_leading();
		std::vector<std::string> lines = StringHelp::split_text(text, "\n", false);
		for (std::vector<std::string>::size_type i=0; i<lines.size(); i++)
		{
			if (i == 0 || (dest_y - ascent >= content_box.top && dest_y + descent < content_box.bottom))
			{
				Size size = get_metrics(canvas, lines[i]);
				if (dest_x + size.width <= content_box.right)
				{
					draw_text(canvas, dest_x, dest_y, lines[i], color);
				}
				else
				{
					Size ellipsis = get_metrics(canvas, "...");

					int seek_start = 0;
					int seek_end = lines[i].size();
					int seek_center = (seek_start + seek_end) / 2;

					UTF8_Reader utf8_reader(lines[i].data(), lines[i].length());
					while (true)
					{
						utf8_reader.set_position(seek_center);
						utf8_reader.move_to_leadbyte();
						if (seek_center != utf8_reader.get_position())
							utf8_reader.next();
						seek_center = utf8_reader.get_position();
						if (seek_center == seek_end)
							break;

						utf8_reader.set_position(seek_start);
						utf8_reader.next();
						if (utf8_reader.get_position() == seek_end)
							break;

						Size text_size = get_metrics(canvas, lines[i].substr(0, seek_center));

						if (dest_x + text_size.width + ellipsis.width >= content_box.right)
							seek_end = seek_center;
						else
							seek_start = seek_center;
						seek_center = (seek_start+seek_end)/2;
					}

					draw_text(canvas, dest_x, dest_y, lines[i].substr(0, seek_center) + "...", color);
				}

				dest_y += line_spacing;
			}
		}
	}
	*/
}

FontMetrics Font::get_font_metrics()
{
	if (impl)
		return impl->get_font_metrics();
	return FontMetrics();
}

int Font::get_character_index(Canvas &canvas, const std::string &text, const Point &point)
{
	if (impl)
		return impl->get_character_index(canvas, text, point);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Font Implementation:


}
