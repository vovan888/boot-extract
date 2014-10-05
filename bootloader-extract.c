/* ------------------------------------------------------------------------- */
/*                                                                           */
/* Android Bootloader image extraction tool                                  */
/*                                                                           */
/* Copyright (C) 2014 vovan888@gmail.com                                     */
/*                                                                           */
/* This program is free software; you can redistribute it and/or modify      */
/* it under the terms of the GNU General Public License as published by      */
/* the Free Software Foundation; either version 2 of the License, or         */
/* (at your option) any later version.                                       */
/*                                                                           */
/* This program is distributed in the hope that it will be useful,           */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU          */
/* General Public License for more details.                                  */
/*                                                                           */
/* You should have received a copy of the GNU General Public License         */
/* along with this program; if not, write to the Free Software               */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA   */
/*                                                                           */
/* ------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define BOOTLDR_MAGIC "BOOTLDR!"
#define BOOTLDR_MAGIC_SIZE 8
#defint MAX_NUM_IMAGES  10

struct bootloader_images_header {
         char magic[BOOTLDR_MAGIC_SIZE];
         uint32_t num_images;
         uint32_t start_offset;
         uint32_t bootldr_size;
         struct {
                 char name[64];
                 uint32_t size;
         } img_info[MAX_NUM_IMAGES] __attribute__((packed));
} __attribute__((packed));


int main(int argc, char **argv)
{
	int f;
	int fk;
	int n;
	char *buf;
	struct bootloader_images_header hdr;

	if (argc < 2) {
		printf("\nAndroid bootloader image extraction tool v1.0\n");
		printf("Copyright (C) 2014 Vladrimir Ananiev\n\n");
		printf("Usage %s <boot or recovery image file>\n", argv[0]);
		return 1;
	}

	f = open(argv[1], O_RDONLY);
	if (f == -1) {
		printf("Failed to open %s (%s)\n", argv[1], strerror(errno));
		return 1;
	}

	n = read(f, &hdr, sizeof(hdr));
	if (n < sizeof(hdr)) {
		printf("read failed\n");
		return 1;
	}

	if (strncmp((const char *)hdr.magic, BOOTLDR_MAGIC, BOOTLDR_MAGIC_SIZE) != 0) {
		printf("Bootloader magic missing: not a valid bootloader iamge\n");
		return 1;
	}

	printf("Bootloader header\n"
	       "  num_images\t%d\n"
	       "  start_offset\t0x%x\n"
	       "  bootldr_size\t0x%x\n",
	       hdr.num_images, hdr.start_offset, hdr.bootldr_size);


    uint32_t offset = hdr.start_offset;
	for (int i = 0; i < hdr.num_images; i++) {
	    char *name = hdr.img_info[i].name;
	    uint32_t size = hdr.img_info[i].size;
		printf("Extracting image [%d] %s, size=%d\n", i, name, size);
		lseek(f, offset, SEEK_SET);
		fk = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fk < 0) {
			printf("Failed to create image file %s\n",name);
			return 1;
		}
		buf = malloc(size);
		if (buf == 0) {
			printf("malloc failed\n");
			return 1;
		}
		n = read(f, buf, size);
		if (n != size) {
			printf ("Error in read\n");
			return 1;
		}
		write(fk, buf, size);
		free(buf);
		close(fk);
		printf("%s extracted\n", name);
		offset += size;
	}
	return 0;
}
