# fetchext2
简易获取ext2文件系统的信息

```shell
root@wwinter-00:~/dev/github/fetchext2# ./fetchext2 vdisk
  --------------------------- SuperBlock ---------------------------
s_magic                        =    61267
s_inodes_count                 =      176
s_blocks_count                 =     1440
s_r_blocks_count               =       72
s_free_inodes_count            =      165
s_free_blocks_count            =     1372
s_first_data_block             =        1
s_log_block_size               =        0
s_blocks_per_group             =     8192
s_inodes_per_group             =      176
s_max_mnt_count                =        0
s_mtime = Thu Jan  1 08:00:00 1970
s_wtime = Wed Aug 14 22:07:34 2024
block size = 1024
inode size = 256

  --------------------------- Group Desc ---------------------------
bg_block_bitmap                =        8
bg_inode_bitmap                =        9
bg_inode_table                 =       10
bg_free_blocks_count           =     1372
bg_free_inodes_count           =      165
bg_used_dirs_count             =        2
bg_pad                         =        0

  ------------------------------ IMAP ------------------------------
11111111 11100000 00000000 00000000 00000000 00000000 00000000 00000000 
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
00000000 00000000 00000000 00000000 00000000 00000000 11111111 

  ------------------------------ BMAP ------------------------------
11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111 
11100000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 
00000000 00000000 00000000 00000001 11111111 
```