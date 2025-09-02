//2bpp bitmap interlacer for windows 1.0 boot logo

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define LOGO_WIDTH 268
#define LOGO_HEIGHT 36
#define LOGO_BPP 2
#define GET_OFF(X, Y) ((Y * LOGO_WIDTH + X) / 4)

int main(int argc, char *argv[]) {

	if (argc < 3) {
		printf("\n %s \n Format %s", "Invalid number of arguments", 
			"[bitmap name 2bpp headerless] [output]");
		return EXIT_FAILURE;
	}
	
	FILE *source_bitmap_file = fopen(argv[1], "rb");

	if (source_bitmap_file == NULL) {
		printf("\n%s\n", "Invalid source filename");
		return EXIT_FAILURE; 
	}
	
	fseek(source_bitmap_file, 0, SEEK_END);
	
	size_t source_bitmap_size = ftell(source_bitmap_file);

	fseek(source_bitmap_file, 0, SEEK_SET);

	uint8_t *bitmap_source = calloc(source_bitmap_size, sizeof(uint8_t));
	
	fread(bitmap_source, sizeof(uint8_t), source_bitmap_size, source_bitmap_file);

	fclose(source_bitmap_file);

	uint16_t interlaced_line_count = LOGO_HEIGHT / 2;

	uint16_t *top_lines_list = calloc(interlaced_line_count, sizeof(uint16_t)),
		*bottom_lines_list = calloc(interlaced_line_count, sizeof(uint16_t));

	uint16_t odd_count = 1,
		even_count = 0;

	for (uint16_t i = 0; i < interlaced_line_count; i++) {
		top_lines_list[i] = odd_count;
		odd_count += 2;
		bottom_lines_list[i] = even_count;
		even_count += 2;
	}

	size_t logo_bytes = LOGO_WIDTH * LOGO_HEIGHT / 4;
	
	uint8_t *bitmap_output = calloc(logo_bytes, sizeof(uint8_t));

	for (uint16_t y = 0; y < interlaced_line_count; y++) {
		for (uint16_t x = 0; x < LOGO_WIDTH; x++) {
			bitmap_output[GET_OFF(x, y) + LOGO_WIDTH / 4] = bitmap_source[GET_OFF(x, top_lines_list[y])];
			bitmap_output[GET_OFF(x, y) + LOGO_HEIGHT * LOGO_WIDTH / 8] = bitmap_source[GET_OFF(x, bottom_lines_list[y])];
		}
	}

	free(bitmap_source);
	free(top_lines_list);
	free(bottom_lines_list);

	FILE *output_file = fopen(argv[2], "wb");
	fwrite(bitmap_output, sizeof(uint8_t), logo_bytes, output_file);

	free(bitmap_output);
	fclose(output_file);

	return EXIT_SUCCESS;
}
