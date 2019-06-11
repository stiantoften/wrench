#include <sstream>

#include "../command_line.h"
#include "../formats/fip.h"

int main(int argc, char** argv) {
	return run_cli_converter(argc, argv,
		"Converts indexed colour textures in the FIP format to BMP files",
		{
			{ "export", fip_to_bmp }
		});
}
