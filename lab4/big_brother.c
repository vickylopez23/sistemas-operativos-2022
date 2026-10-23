#include "big_brother.h"
#include "fat_volume.h"
#include "fat_table.h"
#include "fat_util.h"
#include <stdio.h>
#include <string.h>

int bb_is_log_file_dentry(fat_dir_entry dir_entry)
{
    return strncmp(LOG_FILE_BASENAME, (char *)(dir_entry->base_name), 3) == 0 &&
           strncmp(LOG_FILE_EXTENSION, (char *)(dir_entry->extension), 3) == 0;
}

int bb_is_log_filepath(char *filepath)
{
    return strcmp(filepath, BB_LOG_FILE) == 0;
}

int bb_is_log_dirpath(char *filepath)
{
    return strncmp(BB_DIRNAME, filepath, 15) == 0;
}

fat_file get_bb_orphan_dir()
{
    fat_volume vol = get_fat_volume();
    for (unsigned int i = 2; i < 10240 + 2; i++)
    {
        u32 cluster = fat_table_get_next_cluster(vol->table, i);
        if (!fat_table_cluster_is_bad_sector(cluster))
            continue;
        u32 bytes_per_cluster = fat_table_bytes_per_cluster(vol->table);
        u32 cluster_offset = fat_table_cluster_offset(vol->table, i);
        u8 *buffer = alloca(bytes_per_cluster);

        if (full_pread(vol->table->fd, buffer, bytes_per_cluster, cluster_offset) != bytes_per_cluster)
        {
            continue;
        }

        fat_dir_entry entry = ((fat_dir_entry)buffer);
        if(bb_is_log_file_dentry(entry))
            return fat_file_init_orphan_dir(strdup(BB_DIRNAME), vol->table, i);
    }
    return NULL;
}

fat_file init_bb_dir()
{
    fat_volume vol = get_fat_volume();
    fat_file bb_dir = fat_file_init(vol->table, true, strdup(BB_DIRNAME));
    // Maybe set cluster data to 0 so avoid random entries?
    // CHECK FOR ERRORS.
    fat_table_set_next_cluster(vol->table, bb_dir->start_cluster, FAT_CLUSTER_BAD_SECTOR);
    return bb_dir;
}
