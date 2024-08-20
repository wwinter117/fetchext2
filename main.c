/**
 * Copyright (c) 2024 ZhangDD
 *
 * https://www.wwinter.cn
 *
 * This work is dedicated to the public domain under the Creative Commons CC0 Public Domain Dedication.
 * You can use, modify, and distribute this work without asking for permission.
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "ext2.h"

#define MIN_BLOCK_SIZE (1 << 10)

#define GROUP_DESC_PER_BLK (int)(BLOCK_SIZE / sizeof(struct ext2_group_desc))
#define GROUP_DESC_SIZE (sizeof(struct ext2_group_desc))
#define INODE_PER_BLK (int)(BLOCK_SIZE / sizeof(struct ext2_inode))
#define INODE_SIZE (sizeof(struct ext2_inode))

int fd;
int BLOCK_SIZE;

struct ext2_info fs_info;

void ls_imap(int block_no);

void ls_bmap(int block_no);

void ls_dirs(int block_no);

/*
 * read a block data to a malloced buf, do not forget free it after used
 */
char *read_block(int blk_no) {
    char *buf;
    if (lseek(fd, BLOCK_SIZE * blk_no, SEEK_SET) < 0) {
        goto err;
    }
    if ((buf = malloc(BLOCK_SIZE)) == NULL) {
        goto err;
    }
    if ((read(fd, buf, BLOCK_SIZE)) != BLOCK_SIZE) {
        free(buf);
        goto err;
    }
    return buf;

    err:
    perror("read_block");
    close(fd);
    exit(EXIT_FAILURE);
}

struct ext2_group_desc *
get_gd(int gd_no) {
    gd_no -= 1;
    char *buf;
    int blk_no, blk_off;
    struct ext2_group_desc *gd;
    blk_no = (int) fs_info.super_block->s_first_data_block + 1 + gd_no;
    blk_off = (int) ((gd_no % GROUP_DESC_PER_BLK) * GROUP_DESC_SIZE);
    buf = read_block(blk_no);
    gd = malloc(GROUP_DESC_SIZE);
    memcpy(gd, buf + blk_off, GROUP_DESC_SIZE);
    free(buf);
    return gd;
}

struct ext2_inode *
get_inode(int gd_no, int inode_no) {
    char *buf;
    int blk_no, blk_off;
    struct ext2_group_desc *gd;
    struct ext2_inode *ip;
    gd = get_gd(gd_no);
    int inode_table = (int) gd->bg_inode_table;
    free(gd);

    blk_no = inode_table + (inode_no - 1) / INODE_PER_BLK;
    blk_off = inode_no % INODE_PER_BLK;

    buf = read_block(blk_no);
    printf("3:blk_no: %d\n", blk_no);
    ip = malloc(INODE_SIZE);
    memcpy(ip, buf + blk_off * INODE_SIZE, INODE_SIZE);
    free(buf);
    return ip;
}

void print_imode(u16 mode) {
    // extract the file type from the high 4 bits
    u16 file_type = mode & 0xF000;

    switch (file_type) {
        case 0x8000: // Regular file
            printf("-");
            break;
        case 0x4000: // Directory
            printf("d");
            break;
        case 0x2000: // Character device
            printf("c");
            break;
        case 0x6000: // Block device
            printf("b");
            break;
        case 0x1000: // FIFO
            printf("p");
            break;
        case 0xA000: // Symbolic link
            printf("l");
            break;
        case 0xC000: // Socket
            printf("s");
            break;
        default:     // Unknown type
            printf("?");
            break;
    }

    // Print the permission bits (owner, group, others)
    printf("%c", (mode & 0x0100) ? 'r' : '-'); // Owner read
    printf("%c", (mode & 0x0080) ? 'w' : '-'); // Owner write
    printf("%c", (mode & 0x0040) ? ((mode & 0x0800) ? 's' : 'x') : ((mode & 0x0800) ? 'S' : '-')); // Owner execute

    printf("%c", (mode & 0x0020) ? 'r' : '-'); // Group read
    printf("%c", (mode & 0x0010) ? 'w' : '-'); // Group write
    printf("%c", (mode & 0x0008) ? ((mode & 0x0400) ? 's' : 'x') : ((mode & 0x0400) ? 'S' : '-')); // Group execute

    printf("%c", (mode & 0x0004) ? 'r' : '-'); // Others read
    printf("%c", (mode & 0x0002) ? 'w' : '-'); // Others write
    printf("%c", (mode & 0x0001) ? ((mode & 0x0200) ? 't' : 'x') : ((mode & 0x0200) ? 'T' : '-')); // Others execute

    printf("\n");
}

void print_int(const char *str, unsigned int x) {
    printf("%-30s = %d\n", str, x);
}

void print_time(char *str, time_t time_t) {
    char buffer[80];
    struct tm *timeinfo;
    timeinfo = localtime(&time_t);
    strftime(buffer, sizeof(buffer), "%a %b %e %H:%M:%S %Y", timeinfo);
    printf("%-30s = %s\n", str, buffer);
}


/*
 * print every bit(0/1) in a block
 */
void print_bi(int blk_no, int total) {
    char *buf;
    buf = read_block(blk_no);
    int count = 0;

    int x = 0, y = total;
    while (y != 0) {
        y /= 10;
        x++;
    }
    for (int i = 0; i < total; i++) {
        u8 byte = buf[i / 8];
        int bit = (byte >> (i % 8)) & 1;

        if (i % 64 == 0) {
            printf("%-*d: ", x, i);
            if (!bit) {
                printf("...\n");
                break;
            }
        }
        printf("%d", bit);
        count++;

        if (count % 8 == 0) {
            printf(" ");
        }

        if (count % 64 == 0) {
            printf("\n");
        }
    }
    printf("\n");
    free(buf);
}


// ======================================== SuperBlock ========================================
void ls_boot_block(void) {
    printf("--------------------------- [B0]BootBlock\n");
    printf("...\n");
}

// ======================================== SuperBlock ========================================

void ls_super_block(void) {
    printf("--------------------------- [B1]SuperBlock\n");
    print_int("s_magic", fs_info.super_block->s_magic);
    print_int("s_inodes_count", fs_info.super_block->s_inodes_count);
    print_int("s_blocks_count", fs_info.super_block->s_blocks_count);
    print_int("s_r_blocks_count", fs_info.super_block->s_r_blocks_count);
    print_int("s_free_inodes_count", fs_info.super_block->s_free_inodes_count);
    print_int("s_free_blocks_count", fs_info.super_block->s_free_blocks_count);
    print_int("s_first_data_block", fs_info.super_block->s_first_data_block);
    print_int("s_log_block_size", fs_info.super_block->s_log_block_size);
    print_int("s_blocks_per_group", fs_info.super_block->s_blocks_per_group);
    print_int("s_inodes_per_group", fs_info.super_block->s_inodes_per_group);
    print_int("s_max_mnt_count", fs_info.super_block->s_mnt_count);
    print_time("s_mtime", (time_t) fs_info.super_block->s_mtime);
    print_time("s_wtime", (time_t) fs_info.super_block->s_wtime);
    print_int("block size", fs_info.block_size);
    print_int("inode size", fs_info.super_block->s_inode_size);
    printf("\n");
}

// ======================================== Group Desc ========================================
void ls_group_desc(void) {
    char *buf;
    struct ext2_group_desc *gd;
    for (int i = 0; i < fs_info.gd_num; ++i) {
        int gd_no = (int) (i / GROUP_DESC_PER_BLK);
        int offset = (int) ((i % GROUP_DESC_PER_BLK) * GROUP_DESC_SIZE);
        int blk_no = gd_no + (int) fs_info.super_block->s_first_data_block + 1;
        buf = read_block(blk_no);
        gd = (struct ext2_group_desc *) (buf + offset);
        if (!gd->bg_used_dirs_count) {
            printf("--------------------------- [B%d]Group Desc-%d(not used)\n",
                   blk_no, i);
            free(buf);
            continue;
        }
        printf("--------------------------- [B%d]Group Desc-%d\n", blk_no, i);
        print_int("bg_block_bitmap", gd->bg_block_bitmap);
        print_int("bg_inode_bitmap", gd->bg_inode_bitmap);
        print_int("bg_inode_table", gd->bg_inode_table);
        print_int("bg_free_blocks_count", gd->bg_free_blocks_count);
        print_int("bg_free_inodes_count", gd->bg_free_inodes_count);
        print_int("bg_used_dirs_count", gd->bg_used_dirs_count);
        print_int("bg_pad", gd->bg_pad);
        printf("\n");
        ls_bmap((int) gd->bg_block_bitmap);
        ls_imap((int) gd->bg_inode_bitmap);
        ls_dirs((int) gd->bg_inode_table);
        free(buf);
        printf("\n");
    }
}

void ls_bmap(int block_no) {
    printf("--------------------------- [B%d]BMAP\n", block_no);
    print_bi((int) block_no, (int) fs_info.super_block->s_blocks_per_group);
}

void ls_imap(int block_no) {
    printf("--------------------------- [B%d]IMAP\n", block_no);
    print_bi((int) block_no, (int) fs_info.super_block->s_inodes_per_group);
}

/*
 * print dir_entrys in a block
 */
void ls_entry(int block_no) {
    printf("--------------------------- [B%d]ENTRYS\n", block_no);
    char *buf, *cp, *d_name;
    struct ext2_dir_entry *dp;
    buf = read_block(block_no);
    cp = buf;
    dp = (struct ext2_dir_entry *) cp;
    char type;
    switch (dp->file_type) {
        case EXT2_FT_REG_FILE:
            type = '-';  // Regular file
            break;
        case EXT2_FT_DIR:
            type = 'd';  // Directory
            break;
        case EXT2_FT_CHRDEV:
            type = 'c';  // Character device
            break;
        case EXT2_FT_BLKDEV:
            type = 'b';  // Block device
            break;
        case EXT2_FT_FIFO:
            type = 'p';  // FIFO (named pipe)
            break;
        case EXT2_FT_SOCK:
            type = 's';  // Socket
            break;
        case EXT2_FT_SYMLINK:
            type = 'l';  // Symbolic link
            break;
        case EXT2_FT_UNKNOWN:
        default:
            type = '?';  // Unknown type
    }
    printf("type inode rec_len name_len name\n");
    while (cp < buf + BLOCK_SIZE) {
        d_name = (char *) malloc(dp->name_len + 1);
        strncpy(d_name, dp->name, dp->name_len);
        d_name[dp->name_len] = 0;
        printf("%-4c %-5d %-7d %-8d %-20s\n", type, dp->inode, dp->rec_len, dp->name_len, d_name);
        free(d_name);
        cp += dp->rec_len;
        dp = (struct ext2_dir_entry *) cp;
    }
    free(buf);
}

void ls_dirs(int block_no) {
    struct ext2_inode *ip;
    int it = block_no;
    char *buf;
    printf("--------------------------- [B%d]DIRS\n", block_no);
    for (int i = 1; i < INODE_PER_BLK; ++i) {
        int b_offset = i / INODE_PER_BLK;
        int i_offset = i % INODE_PER_BLK;
        printf("%d\n", it + b_offset);
        buf = read_block(it + b_offset);
        printf("i_offset: %d\n", i_offset);
        ip = (struct ext2_inode *) (buf + i_offset * INODE_SIZE);
        print_imode(ip->i_mode);
        print_int("inode", i);
        print_int("i_uid", ip->i_uid);
        print_int("i_gid", ip->i_gid);
        print_int("i_size", ip->i_size);
        print_time("i_ctime", (time_t) ip->i_ctime);
        print_int("i_links_count", ip->i_links_count);
        for (int j = 0; j < 15; ++j) {
            if (ip->i_block[j]) {
                printf("i_block[%d] = %d\n", j, ip->i_block[j]);
                ls_entry((int) ip->i_block[j]);
            }
        }
        free(buf);
        printf("\n");
    }
}

void info_init(char *dev) {
    char *buf = NULL;
    struct ext2_super_block *sb;
    if ((fd = open(dev, O_RDONLY)) < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    if ((sb = malloc(sizeof(struct ext2_super_block))) == NULL) {
        goto err;
    }
    // read superblock
    lseek(fd, 1024, SEEK_SET);
    buf = malloc(1024);
    read(fd, buf, 1024);
    memcpy(sb, buf, sizeof(struct ext2_super_block));
    free(buf);
    if (sb->s_magic != 0xEF53) {
        printf("Not an EXT2 filesystem\n");
        free(sb);
        close(fd);
        exit(EXIT_FAILURE);
    }
    fs_info.block_size = MIN_BLOCK_SIZE << sb->s_log_block_size; // 0->1k 1->2k 2->4k ...
    fs_info.super_block = sb;
    fs_info.gd_num = (int) ((sb->s_blocks_count + sb->s_blocks_per_group - 1) / sb->s_blocks_per_group);
    BLOCK_SIZE = fs_info.block_size;
    return;

    err:
    perror("info_init");
    close(fd);
    exit(EXIT_FAILURE);
}

struct ext2_inode *
search_inode(struct ext2_inode *ip, char *token) {
    char *buf, *cp;
    struct ext2_dir_entry *dp = NULL;
    struct ext2_inode *res_ip = NULL;
    for (int i = 0; i < (int) ip->i_blocks; ++i) {
        if (ip->i_block[i]) {
            buf = read_block((int) ip->i_block[i]);
            cp = buf;
            dp = (struct ext2_dir_entry *) buf;
            while (cp < buf + BLOCK_SIZE) {
                char d_name[dp->name_len + 1];
                memcpy(d_name, dp->name, dp->name_len);
                d_name[dp->name_len] = 0;
                if (strcmp(d_name, token) == 0) {
                    printf("1:got: %s\n", d_name);
                    printf("2:dp->inode: %d\n", dp->inode);
                    res_ip = get_inode(1, (int) dp->inode);
                    break;
                }
                cp += dp->rec_len;
                dp = (struct ext2_dir_entry *) cp;
            }
            free(buf);
            if (res_ip) {
                return res_ip;
            }
        }
    }
    return NULL;
}

struct ext2_inode *
path2inode(char *path) {
    char *token;
    struct ext2_inode *ip = NULL, *tmp_ip = NULL;

    ip = get_inode(1, 2);
    token = strtok(path, "/");
    while (token != NULL) {
        printf("search: %s\n", token);
        print_imode(ip->i_mode);
        tmp_ip = search_inode(ip, token);
        if (ip) {
            free(ip);
        }
        printf("path2inode1\n");
        if (!tmp_ip) {
            printf("path2inode2\n");
            return NULL;
        } else {
            printf("search success: %s\n", token);
            for (int i = 0; i < 15; ++i) {
                printf("tmp_ip->i_block[%d]: %d\n", i, tmp_ip->i_block[i]);
            }
        }
        ip = tmp_ip;
        token = strtok(NULL, "/");
    }
    return ip;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: superblock [dev]\n");
        return 0;
    }
    info_init(argv[1]);

    ls_boot_block();
    ls_super_block();
    ls_group_desc();


    char path[20] = "a/hi.c";
    printf("start search: %s\n", path);
    struct ext2_inode *p = path2inode(path);
    if (p == NULL) {
        printf("not found: %s\n", path);
    } else {
        print_int("p->i_mode", (int) p->i_mode);
    }
    free(p);

//    ls_imap();
//    ls_bmap();
//    ls_dirs();
//
//    free(super_block);
//    free(group_desc);

    free(fs_info.super_block);
    close(fd);
    return 1;
}
