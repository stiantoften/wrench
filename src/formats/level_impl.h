/*
	wrench - A set of modding tools for the Ratchet & Clank PS2 games.
	Copyright (C) 2019 chaoticgd

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef FORMATS_LEVEL_IMPL_H
#define FORMATS_LEVEL_IMPL_H

#include <memory>
#include <stdint.h>

#include "../level.h"
#include "../stream.h"
#include "../worker_logger.h"
#include "wad.h"
#include "moby_impl.h"
#include "shrub_impl.h"
#include "texture_impl.h"

# /*
#	A level stored using a stream. The member function wrap read/write calls.
# */

/*
	LEVEL*.WAD LAYOUT
	=================

	master_header
	secondary_header {
		texture_header {
			... uncompressed textures ...
		}
	}
	???
	ram_image_wad
	???
	moby_wad: wad(
		level_header
		???
		some strings
		???
		moby_table
		???
	)
	???

	where entries in curly brackets are pointed to by a header, entries in
	wad(...) are within a compressed segment.
*/

class level_impl : public level {
public:
	struct fmt {
		struct master_header;
		struct secondary_header;

		packed_struct(master_header,
			uint8_t unknown1[0x14];              // 0x0
			// The offset between the secondary header and the moby WAD is
			// (secondary_moby_offset_part * 0x800 + 0xfff) & 0xfffffffffffff000.
			uint32_t secondary_moby_offset_part; // 0x14
			uint8_t unknown2[0xc];               // 0x18
			// The offset between something and the moby WAD is
			// moby_wad_offset_part * 0x800 + 0xfffU & 0xfffff000
			uint32_t moby_wad_offset_part;       // 0x28
		)

		// Pointers are relative to this header.
		packed_struct(secondary_header,
			uint32_t unknown1;                  // 0x0
			uint32_t unknown2;                  // 0x4
			file_ptr<level_texture_provider::fmt::header> textures; // 0x8
			uint32_t texture_segment_size;      // 0xc
			uint32_t tex_pixel_data_base;       // 0x10
			uint32_t unknown5;                  // 0x14
			uint32_t unknown6;                  // 0x18
			uint32_t unknown7;                  // 0x1c
			uint32_t unknown8;                  // 0x20
			uint32_t unknown9;                  // 0x24
			uint32_t unknown10;                 // 0x28
			uint32_t unknown11;                 // 0x2c
			uint32_t unknown12;                 // 0x30
			uint32_t unknown13;                 // 0x34
			uint32_t unknown14;                 // 0x38
			uint32_t unknown15;                 // 0x3c
			uint32_t unknown16;                 // 0x40
			uint32_t unknown17;                 // 0x44
			file_ptr<wad_header> ram_image_wad; // 0x48
		)

		struct moby_segment {
			struct header;
			struct ship_data;
			struct directional_light_table;
			struct string_table_header;
			struct string_table_entry;
			struct model_table_header;
			struct model_table_entry;
			struct shrub_table_header;
			struct moby_table_header;

			packed_struct(header,
				file_ptr<ship_data> ship;                             // 0x0
				file_ptr<directional_light_table> directional_lights; // 0x4
				uint32_t unknown1;                                    // 0x8
				uint32_t unknown2;                                    // 0xc
				file_ptr<string_table_header> english_strings;        // 0x10
				uint32_t unknown3; // Points to 16 bytes between the English and French tables (on Barlow).
				file_ptr<string_table_header> french_strings;         // 0x18
				file_ptr<string_table_header> german_strings;         // 0x1c
				file_ptr<string_table_header> spanish_strings;        // 0x20
				file_ptr<string_table_header> italian_strings;        // 0x24
				file_ptr<string_table_header> null_strings;           // 0x28 Also what is this thing?
				uint32_t unknown4;                                    // 0x2c
				uint32_t unknown5;                                    // 0x30
				file_ptr<model_table_header> static_models;           // 0x34
				uint32_t unknown7;                                    // 0x38
				uint32_t unknown8;                                    // 0x3c
				file_ptr<shrub_table_header> shrubs;                  // 0x40
				uint32_t unknown10;                                   // 0x44
				uint32_t unknown11;                                   // 0x48
				file_ptr<moby_table_header> mobies;                          // 0x4c
			)

			packed_struct(ship_data,
				uint32_t unknown1[0xf];
				vec3f position;
				float rotationZ;
			)

			packed_struct(directional_light_table,
				uint32_t num_directional_lights; // Max 0xb.
				// Directional lights follow.
			)

			packed_struct(directional_light,
				uint8_t unknown[64];
			)

			packed_struct(string_table_header,
				uint32_t num_strings;
				uint32_t unknown;
				// String table entries follow.
			)

			packed_struct(string_table_entry,
				file_ptr<char*> string; // Relative to this struct.
				uint32_t id;
				uint32_t padding[2];
			)

			packed_struct(model_table_header,
				uint32_t num_static_models;
				uint32_t pad[3];
				// Models follow.
			)

			packed_struct(model_table_entry,
				uint8_t unknown[0x18];
			)

			packed_struct(shrub_table_header,
				uint32_t num_shrubs;
				uint32_t pad[3];
			)

			packed_struct(moby_table_header,
				uint32_t num_mobies;
				uint32_t unknown[3];
				// Mobies follow.
			)
		};
	};

	level_impl(stream* iso_file, uint32_t offset, uint32_t size, std::string display_name, worker_logger& log);

	texture_provider* get_texture_provider();

	std::vector<shrub*> shrubs() override;
	std::map<uint32_t, moby*> mobies() override;

	std::map<std::string, std::map<uint32_t, std::string>> game_strings() override;

private:
	void read_game_strings(fmt::moby_segment::header header, worker_logger& log);
	void read_models(fmt::moby_segment::header header, worker_logger& log);
	void read_shrubs(fmt::moby_segment::header header, worker_logger& log);
	void read_mobies(fmt::moby_segment::header header, worker_logger& log);

	uint32_t locate_moby_wad();
	uint32_t locate_secondary_header(const fmt::master_header& header, uint32_t moby_wad_offset);

	proxy_stream _level_file;
	std::optional<level_texture_provider> _textures;
	std::optional<wad_stream> _moby_segment_stream;
	std::vector<std::unique_ptr<shrub_impl>> _shrubs;
	std::vector<std::unique_ptr<moby_impl>> _mobies;
	std::map<std::string, std::map<uint32_t, std::string>> _game_strings;
};

#endif
