#include "grafics2.h"

__int64 get_file_size(const char* file)
{
	HANDLE handle = CreateFile(
		file,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (handle == INVALID_HANDLE_VALUE)
	{
		printf("get_file_size(%s) --- INVALID_HANDLE_VALUE\n", file);
		return 0;
	}

	LARGE_INTEGER size;
	if (!GetFileSizeEx(handle, &size))
	{
		printf("get_file_size(%s) --- GetFileSizeEz() failed\n", file);
		CloseHandle(handle);
		return 0;
  	}

	CloseHandle(handle);
	return size.QuadPart;
}

void read_file(const char* file, char* buffer, __int64 size)
{
	HANDLE handle = CreateFile(
		file,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (handle == INVALID_HANDLE_VALUE)
	{
		printf("read_file(%s) --- INVALID_HANDLE_VALUE\n", file);
		return;
	}

	BOOL result = ReadFile(
		handle,
		buffer,
		size,
		NULL,
		NULL
	);

	if (!result)
	{
		printf("read_file(%s) --- ReadFile() failed\n", file);
	}

	CloseHandle(handle);
}

typedef struct bmp_header
{
	char signature[4];
	uint32_t file_size;
	uint32_t reserved;
	uint32_t data_offset;
} bmp_header;

typedef struct bmp_info_header
{
	uint32_t size;
	uint32_t width;
	uint32_t height;
	short planes;
	short bits_per_pixel;
	uint32_t compression;
	uint32_t image_size;
	uint32_t xpixel_per_m;		// horizontal resolution
	uint32_t ypixel_per_m;		// vertical resolution
	uint32_t colors_used;
	uint32_t important_colors;
} bmp_info_header;

char* bmp_load(const char* file, uint32_t* width, uint32_t* height)
{
	__int64 fsize = get_file_size(file);
	char* buffer = (char*) malloc(fsize);
	read_file(file, buffer, fsize);

	bmp_header header = (bmp_header) {
		.signature = { *buffer, *(buffer + 1), 0, 0 },
		.file_size = *((uint32_t*) (buffer + 2)),
		.reserved = *((uint32_t*) (buffer + 6)),
		.data_offset = *((uint32_t*) (buffer + 10))
	};

	bmp_info_header info_header = (bmp_info_header) {
		.size = *((uint32_t*) (buffer + 14)),
		.width = *((uint32_t*) (buffer + 18)),
		.height = *((uint32_t*) (buffer + 22)),
		.planes = *((short*) (buffer + 26)),
		.bits_per_pixel = *((short*) (buffer + 28)),
		.compression = *((uint32_t*) (buffer + 30)),
 		.image_size = *((uint32_t*) (buffer + 34)),
		.xpixel_per_m = *((uint32_t*) (buffer + 38)),
		.ypixel_per_m = *((uint32_t*) (buffer + 42)),
		.colors_used = *((uint32_t*) (buffer + 46)),
		.important_colors = *((uint32_t*) (buffer + 50))
	};

	*width = info_header.width;
	*height = info_header.height;

	char* bmp_data = buffer + header.data_offset;

	/*
		SPECIFICATION:
		 If bits_per_pixels are more then 8bits, pixel array begins with
		 last row's first element, and continues untill it reaches last
		 element on the row. Then it moves to second to last array and
		 so on. Each row is padded at the end with 0x00 so that it
		 reaches closest multiple of four. You can this:
		  ((4 * width) - (bits_per_pixel / 8) * width) % 4, and so you get
		 your padding per one row.

		NOTE:
	 	 This supports only 24 bits per pixel and returns data in
		 R8G8B8A8 format. First pixel is on row 0 position 0, and
		 it moves by rows.
	 */

	char* bmp_buffer = (char*) malloc(4 * (*width) * (*height));
	
	if (info_header.bits_per_pixel == 24)
	{
		uint32_t padding = ((4 * (*width) - 3 * (*width))) % 4;
		char* bmp_data_ptr = bmp_data + info_header.image_size;
		char* bmp_buffer_ptr = bmp_buffer;
		
		bmp_data_ptr--;
		
		for (uint32_t i = 0; i < (*height); i++)
		{
			bmp_data_ptr -= padding;

		   	for (uint32_t j = 0; j < (*width); j++)
			{
				*bmp_buffer_ptr = *bmp_data_ptr;
				bmp_buffer_ptr++;
				bmp_data_ptr--;

				*bmp_buffer_ptr = *bmp_data_ptr;
				bmp_buffer_ptr++;
				bmp_data_ptr--;

				*bmp_buffer_ptr = *bmp_data_ptr;
				bmp_buffer_ptr++;
				bmp_data_ptr--;

				*bmp_buffer_ptr = 0xff;
				bmp_buffer_ptr++;
			}
		}
	}
	else
	{
		printf("bmp_load(%s) --- info_header.bits_per_pixel unsupported\n", file);
		free(bmp_buffer);
		return NULL;
	}
	
	free(buffer);
	
	return bmp_buffer;
}

void bmp_free(char* bmp_buffer)
{
	if (bmp_buffer)
	{
		free(bmp_buffer);
	}
}

void bmp_save(const char* file, char* bmp, u32 width, u32 height, i32 bytes_per_pixel)
{
	u32 padding = width % 4;
	u32 file_size = width * height * 3 + 14 + 40 + height * padding;
	char* buffer = (char*) malloc(file_size);

	switch (bytes_per_pixel) {
		case 1:
		{
			strcpy(buffer, "BM");
			memcpy(buffer + 2, &file_size, sizeof(u32));
			memset(buffer + 6, 0, sizeof(u32));
			u32 data_offset = 14 + 40;
			memcpy(buffer + 10, &data_offset, sizeof(u32));

			bmp_info_header bmp_header = {
				40, width, height, 1, 24, 0, 0, 2835, 2835, 0, 0
			};
			// TODO: get proper resolution values
			
			memcpy(buffer + 14, &bmp_header.size, sizeof(u32));
			memcpy(buffer + 18, &bmp_header.width, sizeof(u32));
			memcpy(buffer + 22, &bmp_header.height, sizeof(u32));
			
			memcpy(buffer + 26, &bmp_header.planes, sizeof(i16));
			memcpy(buffer + 28, &bmp_header.bits_per_pixel, sizeof(i16));

			memcpy(buffer + 30, &bmp_header.compression, sizeof(u32));
			memcpy(buffer + 34, &bmp_header.image_size, sizeof(u32));
			memcpy(buffer + 38, &bmp_header.xpixel_per_m, sizeof(u32));
			memcpy(buffer + 42, &bmp_header.ypixel_per_m, sizeof(u32));
			memcpy(buffer + 46, &bmp_header.colors_used, sizeof(u32));
			memcpy(buffer + 50, &bmp_header.important_colors, sizeof(u32));

			char* data = buffer + 54;
			char* p_bmp = bmp + width * height;
			p_bmp--;
			
			for (u32 i = 0; i < height; i++) {
				for (u32 j = 0; j < width; j++) {
					*data = *p_bmp;
					data++;
					*data = *p_bmp;
					data++;
					*data = *p_bmp;
					data++;
					p_bmp--;
				}
				data += padding;
			}

			FILE* f;
			f = fopen(file, "wb");
			fwrite(buffer, sizeof(char), file_size, f);
			fclose(f);
		
			break;
		}
		default:
		{
			// UNSOPPORTED
			return;
		}
	}
	free(buffer);
}
