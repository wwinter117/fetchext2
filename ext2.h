#ifndef FETCHEXT2_EXT2_H
#define FETCHEXT2_EXT2_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;

struct ext2_super_block {
    u32 s_inodes_count;         /* Inodes count */
    u32 s_blocks_count;         /* Blocks count */
    u32 s_r_blocks_count;       /* Reserved blocks count */
    u32 s_free_blocks_count;    /* Free blocks count */
    u32 s_free_inodes_count;    /* Free inodes count */
    u32 s_first_data_block;     /* First Data Block */
    u32 s_log_block_size;       /* Block size */
    u32 s_log_frag_size;        /* Fragment size */
    u32 s_blocks_per_group;     /* # Blocks per group */
    u32 s_frags_per_group;      /* # Fragments per group */
    u32 s_inodes_per_group;     /* # Inodes per group */
    u32 s_mtime;                /* Mount time */
    u32 s_wtime;                /* Write time */
    u16 s_mnt_count;            /* Mount count */
    u16 s_max_mnt_count;        /* Maximal mount count */
    u16 s_magic;                /* Magic signature */
    u16 s_state;                /* File system state */
    u16 s_errors;               /* Behaviour when detecting errors */
    u16 s_minor_rev_level;      /* minor revision level */
    u32 s_lastcheck;            /* time of last check */
    u32 s_checkinterval;        /* max. time between checks */
    u32 s_creator_os;           /* OS */
    u32 s_rev_level;            /* Revision level */
    u16 s_def_resuid;           /* Default uid for reserved blocks */
    u16 s_def_resgid;           /* Default gid for reserved blocks */

    /*
     * These fields are for EXT2_DYNAMIC_REV superblocks only.
     * Note: the difference between the compatible feature set and
     * the incompatible feature set is that if there is a bit set
     * in the incompatible feature set that the kernel doesn't
     * know about, it should refuse to mount the filesystem.
     *
     * e2fsck's requirements are more strict; if it doesn't know
     * about a feature in either the compatible or incompatible
     * feature set, it must abort and not try to meddle with
     * things it doesn't understand...
     */
    u32 s_first_ino;            /* First non-reserved inode */
    u16 s_inode_size;           /* size of inode structure */
    u16 s_block_group_nr;       /* block group # of this superblock */
    u32 s_feature_compat;       /* compatible feature set */
    u32 s_feature_incompat;     /* incompatible feature set */
    u32 s_feature_ro_compat;    /* readonly-compatible feature set */
    u8 s_uuid[16];             /* 128-bit uuid for volume */
    char s_volume_name[16];      /* volume name */
    char s_last_mounted[64];     /* directory where last mounted */
    u32 s_algorithm_usage_bitmap; /* For compression */

    /*
     * Performance hints.  Directory preallocation should only
     * happen if the EXT2_FEATURE_COMPAT_DIR_PREALLOC flag is on.
     */
    u8 s_prealloc_blocks;      /* Nr of blocks to try to preallocate*/
    u8 s_prealloc_dir_blocks;  /* Nr to preallocate for dirs */
    u16 s_padding1;

    /*
     * Journaling support valid if EXT3_FEATURE_COMPAT_HAS_JOURNAL set.
     */
    u8 s_journal_uuid[16];     /* uuid of journal superblock */
    u32 s_journal_inum;         /* inode number of journal file */
    u32 s_journal_dev;          /* device number of journal file */
    u32 s_last_orphan;          /* start of list of inodes to delete */
    u32 s_hash_seed[4];         /* HTREE hash seed */
    u8 s_def_hash_version;     /* Default hash version to use */
    u8 s_jnl_backup_type;
    u16 s_desc_size;            /* size of group descriptor */
    u32 s_default_mount_opts;
    u32 s_first_meta_bg;        /* First metablock block group */
    u32 s_mkfs_time;            /* When the filesystem was created */
    u32 s_jnl_blocks[17];       /* Backup of the journal inode */

    u32 s_blocks_count_hi;      /* Blocks count high 32 bits */
    u32 s_r_blocks_count_hi;    /* Reserved blocks count high 32 bits */
    u32 s_free_blocks_hi;       /* Free blocks count high 32 bits */
    u16 s_min_extra_isize;      /* All inodes have at least # bytes */
    u16 s_want_extra_isize;     /* New inodes should reserve # bytes */
    u32 s_flags;                /* Miscellaneous flags */
    u16 s_raid_stride;          /* RAID stride */
    u16 s_mmp_interval;         /* # seconds to wait in MMP checking */
    u64 s_mmp_block;            /* Block for multi-mount protection */
    u32 s_raid_stripe_width;    /* blocks on all data disks (N * stride)*/
    u8 s_log_groups_per_flex;  /* FLEX_BG group size */
    u8 s_reserved_char_pad;
    u16 s_reserved_pad;
    u32 s_reserved[162];        /* Padding to the end of the block */
};

struct ext2_group_desc {
    u32 bg_block_bitmap;        /* Blocks bitmap block */
    u32 bg_inode_bitmap;        /* Inodes bitmap block */
    u32 bg_inode_table;         /* Inodes table block */
    u16 bg_free_blocks_count;   /* Free blocks count */
    u16 bg_free_inodes_count;   /* Free inodes count */
    u16 bg_used_dirs_count;     /* Directories count */
    u16 bg_pad;                 /* Padding to align to 32-bit */
    u32 bg_reserved[3];         /* Reserved for future use */
};

struct ext2_inode {
    u16 i_mode;        /* File mode */
    u16 i_uid;         /* Low 16 bits of Owner Uid */
    u32 i_size;        /* Size in bytes */
    u32 i_atime;       /* Access time */
    u32 i_ctime;       /* Creation time */
    u32 i_mtime;       /* Modification time */
    u32 i_dtime;       /* Deletion Time */
    u16 i_gid;         /* Low 16 bits of Group Id */
    u16 i_links_count; /* Links count */
    u32 i_blocks;      /* Blocks count */
    u32 i_flags;       /* File flags */
    union {
        struct {
            u32 l_i_reserved1;
        } linux1;
        struct {
            u32 h_i_translator;
        } hurd1;
        struct {
            u32 m_i_reserved1;
        } masix1;
    } osd1;            /* OS dependent 1 */
    u32 i_block[15];   /* Pointers to blocks */
    u32 i_generation;  /* File version (for NFS) */
    u32 i_file_acl;    /* File ACL */
    u32 i_dir_acl;     /* Directory ACL */
    u32 i_faddr;       /* Fragment address */
    union {
        struct {
            u8 l_i_frag;      /* Fragment number */
            u8 l_i_fsize;     /* Fragment size */
            u16 i_pad1;
            u16 l_i_uid_high; /* these 2 fields */
            u16 l_i_gid_high; /* were reserved2[0] */
            u32 l_i_reserved2;
        } linux2;
        struct {
            u8 h_i_frag;      /* Fragment number */
            u8 h_i_fsize;     /* Fragment size */
            u16 h_i_mode_high;
            u16 h_i_uid_high;
            u16 h_i_gid_high;
            u32 h_i_author;
        } hurd2;
        struct {
            u8 m_i_frag;      /* Fragment number */
            u8 m_i_fsize;     /* Fragment size */
            u16 m_pad1;
            u32 m_i_reserved2[2];
        } masix2;
    } osd2;            /* OS dependent 2 */
};

#define EXT2_FT_UNKNOWN  0
#define EXT2_FT_REG_FILE 1
#define EXT2_FT_DIR      2
#define EXT2_FT_CHRDEV   3
#define EXT2_FT_BLKDEV   4
#define EXT2_FT_FIFO     5
#define EXT2_FT_SOCK     6
#define EXT2_FT_SYMLINK  7

struct ext2_dir_entry {
    u32 inode;          /* Inode number of the entry */
    u16 rec_len;        /* Length of this directory entry */
    u8 name_len;        /* Length of the file name */
    u8 file_type;       /* File type (if applicable) */
    char name[];        /* File name (not null-terminated) */
};

struct ext2_info {
    struct ext2_super_block *super_block;
    int block_size;
    int gd_num;
};
#endif //FETCHEXT2_EXT2_H
