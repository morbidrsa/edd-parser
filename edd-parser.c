/*
 * Copyright (C) 2015 SUSE <jthumshirn@suse.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public Licens
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/types.h>
#include <linux/edd.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

static void print_edd_device_params(struct edd_info *info)
{
	char interface_type[8];
	char host_bus_type[4];
	unsigned int i;

	printf("struct edd_device_params:\n");
	printf("info->params.length: %u\n", info->params.length);

	printf("info->params.info_flags: 0x%04x\n", info->params.info_flags);
	if (info->params.info_flags & EDD_INFO_DMA_BOUNDARY_ERROR_TRANSPARENT)
		printf("\tDMA boundary error transparent\n");
	if (info->params.info_flags & EDD_INFO_GEOMETRY_VALID)
		printf("\tgeometry valid\n");
	if (info->params.info_flags & EDD_INFO_REMOVABLE)
		printf("\tremovable\n");
	if (info->params.info_flags & EDD_INFO_WRITE_VERIFY)
		printf("\twrite verify\n");
	if (info->params.info_flags & EDD_INFO_MEDIA_CHANGE_NOTIFICATION)
		printf("\tmedia change notification\n");
	if (info->params.info_flags & EDD_INFO_LOCKABLE)
		printf("\tlockable\n");
	if (info->params.info_flags & EDD_INFO_NO_MEDIA_PRESENT)
		printf("\tno media present\n");
	if (info->params.info_flags & EDD_INFO_USE_INT13_FN50)
		printf("\tuse int13 fn50\n");

	printf("info->params.num_default_cylinders: %u\n",
		info->params.num_default_cylinders);
	printf("info->params.num_default_heads: %u\n",
		info->params.num_default_heads);
	printf("info->params.sectors_per_track: %u\n",
		info->params.sectors_per_track);
	printf("info->params.number_of_sectors: %llu\n",
		info->params.number_of_sectors);
	printf("info->params.bytes_per_sector: %u\n",
		info->params.bytes_per_sector); /* Not in kernel */
	printf("info->params.dpte_ptr: 0x%08x\n",
		info->params.dpte_ptr); /* Not in kernel */
	printf("info->params.key: 0x%04x\n", info->params.key);
	printf("info->params.device_path_info_length: %d\n",
		info->params.device_path_info_length);

	for (i = 0; i < sizeof(host_bus_type); i++) {
		if (!isalnum(info->params.host_bus_type[i]))
			break;

		host_bus_type[i] = info->params.host_bus_type[i];
	}

	for (i = 0; i < sizeof(interface_type); i++) {
		if (!isalnum(info->params.interface_type[i]))
			break;

		interface_type[i] = info->params.interface_type[i];
	}

	printf("info->params.host_bus_type[4]: '%s'\n", host_bus_type);
	printf("info->params.interface_type[8]: '%s'\n", interface_type);

}

int main(int argc, char **argv)
{
	struct edd_info *info;
	size_t size;
	unsigned char *buf;
	int rc = 0;
	int fd;

	if (argc != 2) {
		fprintf(stderr, "Usage %s: data\n", argv[0]);
		return 1;
	}

	size = sizeof(struct edd_info);

	buf = calloc(1, size);
	if (!buf) {
		perror("calloc buf");
		return 1;
	}

	info = calloc(1, size);
	if (!info) {
		goto out_free_buf;
	}

	fd = open(argv[1], O_RDONLY);
	if (!fd) {
		perror("open");
		rc = 1;
		goto out_free_info;
	}

	rc = read(fd, buf, size);
	if (rc == -1) {
		perror("read");
		rc = 1;
		goto out_close;
	}

	memcpy(&info->params, buf, sizeof(struct edd_device_params));

	print_edd_device_params(info);

out_close:
	close(fd);
out_free_info:
	free(info);
out_free_buf:
	free(buf);

	return rc;
}
