#define _LARGEFILE64_SOURCE	/* See feature_test_macros(7) */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/types.h>
#include <unistd.h>


void ReadExtended(char **argv, int EBR_StartingSector);
typedef struct {
    uint8_t status;
    uint8_t first_chs[3];
    uint8_t partition_type;
    uint8_t last_chs[3];
    uint32_t lba;
    uint32_t sector_count;
} PartitionEntry;


int main(int argc, char **argv)
{
    char buf[512];

    int fd = open(argv[1], O_RDONLY);
    read(fd, buf, 512);

    PartitionEntry *table_entry_ptr = (PartitionEntry *) & buf[446];

    printf("%-5s %-10s        %-10s %-10s %-10s %-10s %-10s %-10s\n",
	   "Device", "Boot", "Start", "End", "Sectors", "Size", "Id",
	   "Type");

    for (int i = 0; i < 4; i++) {
	printf("%s%-5d %-10c %-10u %-10u %-10u %uM      %-10X\n",
	       argv[1],
	       i + 1,
	       table_entry_ptr[i].status == 0x80 ? '*' : ' ',
	       table_entry_ptr[i].lba,
	       table_entry_ptr[i].lba + table_entry_ptr[i].sector_count -
	       1, table_entry_ptr[i].sector_count,
	       (uint32_t) (((uint64_t) table_entry_ptr[i].sector_count *
			    512) / (1024 * 1024)),
	       table_entry_ptr[i].partition_type);
	if (table_entry_ptr[i].partition_type == 0x05) {


	    ReadExtended(argv, table_entry_ptr[i].lba);

	}
    }


    return 0;
}

void ReadExtended(char **argv, int EBR_StartingSector)
{
    static int i = 5;
    char buf[512];

    int fd = open(argv[1], O_RDONLY);

    lseek64(fd, (off64_t) 512 * EBR_StartingSector, SEEK_SET);

    read(fd, buf, 512);

    PartitionEntry *table_entry_ptr = (PartitionEntry *) & buf[446];

    printf("%s%-5d %-10c %-10u %-10u %-10u %uM      %-10X\n",
	   argv[1],
	   i++,
	   table_entry_ptr[0].status == 0x80 ? '*' : ' ',
	   table_entry_ptr[0].lba + EBR_StartingSector,
	   table_entry_ptr[0].lba + table_entry_ptr[0].sector_count - 1,
	   table_entry_ptr[0].sector_count,
	   (uint32_t) (((uint64_t) table_entry_ptr[0].sector_count *
			512) / (1024 * 1024)),
	   table_entry_ptr[0].partition_type);


    if (table_entry_ptr[1].partition_type != 0
	&& table_entry_ptr[1].lba != 0) {
	ReadExtended(argv, EBR_StartingSector + table_entry_ptr[1].lba);
    }
}
