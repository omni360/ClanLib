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
*/

#include "CSSLayout/precomp.h"
#include "css_parser_display.h"
#include "API/CSSLayout/css_box_properties.h"

namespace clan
{

std::vector<std::string> CSSParserDisplay::get_names()
{
	std::vector<std::string> names;
	names.push_back("display");
	return names;
}

void CSSParserDisplay::parse(CSSBoxProperties &properties, const std::string &name, const std::vector<CSSToken> &tokens, std::map<std::string, CSSBoxProperty *> *out_change_set)
{
	size_t pos = 0;
	CSSToken token = next_token(pos, tokens);
	if (token.type == CSSToken::type_ident && pos == tokens.size())
	{
		if (equals(token.value, "inline"))
			properties.display.type = CSSBoxDisplay::type_inline;
		else if (equals(token.value, "block"))
			properties.display.type = CSSBoxDisplay::type_block;
		else if (equals(token.value, "list-item"))
			properties.display.type = CSSBoxDisplay::type_list_item;
		else if (equals(token.value, "run-in"))
			properties.display.type = CSSBoxDisplay::type_run_in;
		else if (equals(token.value, "inline-block"))
			properties.display.type = CSSBoxDisplay::type_inline_block;
		else if (equals(token.value, "table"))
			properties.display.type = CSSBoxDisplay::type_table;
		else if (equals(token.value, "inline-table"))
			properties.display.type = CSSBoxDisplay::type_inline_table;
		else if (equals(token.value, "table-row-group"))
			properties.display.type = CSSBoxDisplay::type_table_row_group;
		else if (equals(token.value, "table-header-group"))
			properties.display.type = CSSBoxDisplay::type_table_header_group;
		else if (equals(token.value, "table-footer-group"))
			properties.display.type = CSSBoxDisplay::type_table_footer_group;
		else if (equals(token.value, "table-row"))
			properties.display.type = CSSBoxDisplay::type_table_row;
		else if (equals(token.value, "table-column-group"))
			properties.display.type = CSSBoxDisplay::type_table_column_group;
		else if (equals(token.value, "table-column"))
			properties.display.type = CSSBoxDisplay::type_table_column;
		else if (equals(token.value, "table-cell"))
			properties.display.type = CSSBoxDisplay::type_table_cell;
		else if (equals(token.value, "table-caption"))
			properties.display.type = CSSBoxDisplay::type_table_caption;
		else if (equals(token.value, "none"))
			properties.display.type = CSSBoxDisplay::type_none;
		else if (equals(token.value, "inherit"))
			properties.display.type = CSSBoxDisplay::type_inherit;
		else if (equals(token.value, "flex"))
			properties.display.type = CSSBoxDisplay::type_flex;
		else if (equals(token.value, "inline-flex"))
			properties.display.type = CSSBoxDisplay::type_inline_flex;
		else if (equals(token.value, "-clan-grid"))
			properties.display.type = CSSBoxDisplay::type_clan_grid;
		else if (equals(token.value, "-clan-stacked"))
			properties.display.type = CSSBoxDisplay::type_clan_stacked;
	}
	if (out_change_set)
	{
		(*out_change_set)["display"] = &properties.bottom;
	}
}

}