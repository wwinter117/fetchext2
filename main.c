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

#define BLK_SIZE (1 << 10)

int fd;
struct ext2_super_block *super_block;
struct ext2_group_desc *group_desc;

void fmt_print(const char *str, unsigned int x) {
    printf("%-30s = %8d\n", str, x);
}

int read_block(int blk, char *buf) {
    if (lseek(fd, BLK_SIZE * blk, SEEK_SET) < 0) {
        perror("lseek");
        close(fd);
        return 0;
    }
    if ((read(fd, buf, BLK_SIZE)) != BLK_SIZE) {
        perror("read");
        close(fd);
        return 0;
    }
    return 1;
}

void blk_print_bi(int blk_no, int total) {
    char buf[BLK_SIZE];
    if (!read_block(blk_no, buf)) {
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i <= total / 8; ++i) {
        u8 byte = (u8) buf[i];
        for (int j = 0; j < 8; j++) {
            printf("%d", byte >> j & 1);
        }
        printf(" ");
        if ((i + 1) % 8 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

void ls_superblock(void) {
    char buffer[80];
    time_t mtime, wtime;
    struct tm *timeinfo;
    printf("  --------------------------- SuperBlock ---------------------------\n");
    if (super_block->s_magic != 0xEF53) {
        printf("Not an EXT2 filesystem\n");
        exit(EXIT_FAILURE);
    }
    fmt_print("s_magic", super_block->s_magic);
    fmt_print("s_inodes_count", super_block->s_inodes_count);
    fmt_print("s_blocks_count", super_block->s_blocks_count);
    fmt_print("s_r_blocks_count", super_block->s_r_blocks_count);
    fmt_print("s_free_inodes_count", super_block->s_free_inodes_count);
    fmt_print("s_free_blocks_count", super_block->s_free_blocks_count);
    fmt_print("s_first_data_block", super_block->s_first_data_block);
    fmt_print("s_log_block_size", super_block->s_log_block_size);
    fmt_print("s_blocks_per_group", super_block->s_blocks_per_group);
    fmt_print("s_inodes_per_group", super_block->s_inodes_per_group);
    fmt_print("s_max_mnt_count", super_block->s_mnt_count);
    mtime = (time_t) super_block->s_mtime;
    timeinfo = localtime(&mtime);
    strftime(buffer, sizeof(buffer), "%a %b %e %H:%M:%S %Y", timeinfo);
    printf("s_mtime = %s\n", buffer);
    wtime = (time_t) super_block->s_wtime;
    timeinfo = localtime(&wtime);
    strftime(buffer, sizeof(buffer), "%a %b %e %H:%M:%S %Y", timeinfo);
    printf("s_wtime = %s\n", buffer);

    int blksize = BLK_SIZE << super_block->s_log_block_size;
    printf("block size = %d\n", blksize);
    printf("inode size = %d\n", super_block->s_inode_size);
    printf("\n");
}

void ls_group_desc(void) {
    printf("  --------------------------- Group Desc ---------------------------\n");
    fmt_print("bg_block_bitmap", group_desc->bg_block_bitmap);
    fmt_print("bg_inode_bitmap", group_desc->bg_inode_bitmap);
    fmt_print("bg_inode_table", group_desc->bg_inode_table);
    fmt_print("bg_free_blocks_count", group_desc->bg_free_blocks_count);
    fmt_print("bg_free_inodes_count", group_desc->bg_free_inodes_count);
    fmt_print("bg_used_dirs_count", group_desc->bg_used_dirs_count);
    fmt_print("bg_pad", group_desc->bg_pad);
    printf("\n");
}

void ls_imap(void) {
    printf("  ------------------------------ IMAP ------------------------------\n");
    blk_print_bi((int) group_desc->bg_inode_bitmap, (int) super_block->s_inodes_count);
    printf("\n");
}

void ls_bmap(void) {
    printf("  ------------------------------ BMAP ------------------------------\n");
    blk_print_bi((int) group_desc->bg_block_bitmap, (int) super_block->s_blocks_count);
    printf("\n");
}

void info_init(char *dev) {
    char buf[BLK_SIZE];
    if ((fd = open(dev, O_RDONLY)) < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    read_block(1, buf);
    if ((super_block = malloc(sizeof(struct ext2_super_block))) == NULL) {
        goto err;
    }
    memcpy(super_block, buf, sizeof(struct ext2_super_block));

    read_block(2, buf);
    if ((group_desc = malloc(sizeof(struct ext2_group_desc))) == NULL) {
        free(super_block);
        goto err;
    }
    memcpy(group_desc, buf, sizeof(struct ext2_group_desc));

    return;

    err:
    perror("malloc");
    close(fd);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: superblock [dev]\n");
        return 0;
    }
    info_init(argv[1]);

    ls_superblock();
    ls_group_desc();
    ls_imap();
    ls_bmap();

    free(super_block);
    free(group_desc);
    close(fd);
    return 1;
}
