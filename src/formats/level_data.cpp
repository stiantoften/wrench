#include "level_data.h"

std::unique_ptr<level_impl> import_level(stream& level_file) {

	auto lvl = std::make_unique<level_impl>();

	uint32_t segment_offset = locate_main_level_segment(level_file);

	array_stream level_data;
	{
		proxy_stream wad_segment(&level_file, segment_offset);
		decompress_wad(level_data, wad_segment);
	}

	auto level_header = level_data.read<level_data_header>(0);

	auto moby_table = level_data.read<level_data_moby_table>(level_header.mobies.value);
	auto moby_ptr = level_header.mobies.next<level_data_moby>().value;
	for(uint32_t i = 0; i < moby_table.num_mobies; i++) {
		auto moby_data = level_data.read<level_data_moby>(moby_ptr);
		uint32_t uid = moby_data.uid;

		auto current = std::make_unique<moby>(uid);
		current->name = std::to_string(moby_ptr);
		current->set_position(moby_data.position);
		lvl->add_moby(uid, std::move(current));

		moby_ptr += moby_data.size;
	}

	return lvl;
}

uint32_t locate_main_level_segment(stream& level_file) {
	
	// For now just find the largest 0x100 byte-aligned WAD segment.
	// This should work for most levels.

	uint32_t result_offset = 1;
	long result_size = -1;
	for(uint32_t offset = 0; offset < level_file.size() - sizeof(wad_header); offset += 0x100) {
		wad_header header = level_file.read<wad_header>(offset);
		if(validate_wad(header) && header.total_size > result_size) {
			result_offset = offset;
			result_size = header.total_size;
		}
	}

	if(result_offset == 1) {
		throw stream_format_error("File does not contain a valid WAD segment.");
	}

	return result_offset;
}
