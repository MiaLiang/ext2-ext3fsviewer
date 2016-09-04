#ifndef EXT2_HEAD
#define EXT2_HEAD

typedef  unsigned int __le32;
typedef  unsigned short __le16; 


typedef  unsigned int __u32;
typedef  unsigned short __u16; 
typedef  unsigned char __u8;

//super block!!!
//size is 1024
typedef struct ext2_super_block_s {
	__le32	s_inodes_count;		/* Inodes count */
	__le32	s_blocks_count;		/* Blocks count */
	__le32	s_r_blocks_count;	/* Reserved blocks count */
	__le32	s_free_blocks_count;	/* Free blocks count */
	__le32	s_free_inodes_count;	/* Free inodes count */
	__le32	s_first_data_block;	/* First Data Block */
	__le32	s_log_block_size;	/* Block size */
	__le32	s_log_frag_size;	/* Fragment size */
	__le32	s_blocks_per_group;	/* # Blocks per group */
	__le32	s_frags_per_group;	/* # Fragments per group */
	__le32	s_inodes_per_group;	/* # Inodes per group */
	__le32	s_mtime;		/* Mount time */
	__le32	s_wtime;		/* Write time */
	__le16	s_mnt_count;		/* Mount count */
	__le16	s_max_mnt_count;	/* Maximal mount count */
	__le16	s_magic;		/* Magic signature */
	__le16	s_state;		/* File system state */
	__le16	s_errors;		/* Behaviour when detecting errors */
	__le16	s_minor_rev_level; 	/* minor revision level */
	__le32	s_lastcheck;		/* time of last check */
	__le32	s_checkinterval;	/* max. time between checks */
	__le32	s_creator_os;		/* OS */
	__le32	s_rev_level;		/* Revision level */
	__le16	s_def_resuid;		/* Default uid for reserved blocks */
	__le16	s_def_resgid;		/* Default gid for reserved blocks */
	/*
	 * These fields are for EXT2_DYNAMIC_REV superblocks only.
	 *
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
	__le32	s_first_ino; 		/* First non-reserved inode */
	__le16   s_inode_size; 		/* size of inode structure */
	__le16	s_block_group_nr; 	/* block group # of this superblock */
	__le32	s_feature_compat; 	/* compatible feature set */
	__le32	s_feature_incompat; 	/* incompatible feature set */
	__le32	s_feature_ro_compat; 	/* readonly-compatible feature set */
	__u8	s_uuid[16];		/* 128-bit uuid for volume */
	char	s_volume_name[16]; 	/* volume name */
	char	s_last_mounted[64]; 	/* directory where last mounted */
	__le32	s_algorithm_usage_bitmap; /* For compression */
	/*
	 * Performance hints.  Directory preallocation should only
	 * happen if the EXT2_COMPAT_PREALLOC flag is on.
	 */
	__u8	s_prealloc_blocks;	/* Nr of blocks to try to preallocate*/
	__u8	s_prealloc_dir_blocks;	/* Nr to preallocate for dirs */
	__u16	s_padding1;
	/*
	 * Journaling support valid if EXT3_FEATURE_COMPAT_HAS_JOURNAL set.
	 */
	__u8	s_journal_uuid[16];	/* uuid of journal superblock */
	__u32	s_journal_inum;		/* inode number of journal file */
	__u32	s_journal_dev;		/* device number of journal file */
	__u32	s_last_orphan;		/* start of list of inodes to delete */
	__u32	s_hash_seed[4];		/* HTREE hash seed */
	__u8	s_def_hash_version;	/* Default hash version to use */
	__u8	s_reserved_char_pad;
	__u16	s_reserved_word_pad;
	__le32	s_default_mount_opts;
 	__le32	s_first_meta_bg; 	/* First metablock block group */
	__u32	s_reserved[190];	/* Padding to the end of the block */
}ext2_super_block ;

//block������
typedef struct ext3_group_desc_s
{
 __u32 bg_block_bitmap;      /* block ָ��ָ�� block bitmap */
 __u32 bg_inode_bitmap;      /* block ָ��ָ�� inode bitmap */
 __u32 bg_inode_table;       /* block ָ��ָ�� inodes table */
 __u16 bg_free_blocks_count; /* ���е� blocks ���� */
 __u16 bg_free_inodes_count; /* ���е� inodes ���� */
 __u16 bg_used_dirs_count;   /* Ŀ¼���� */
 __u16 bg_pad;               /* ���Ժ��� */
 __u32 bg_reserved[3];       /* ���Ժ��� */
}ext3_group_desc;


#define	EXT3_NDIR_BLOCKS		12
#define	EXT3_IND_BLOCK			EXT3_NDIR_BLOCKS
#define	EXT3_DIND_BLOCK			(EXT3_IND_BLOCK + 1)
#define	EXT3_TIND_BLOCK			(EXT3_DIND_BLOCK + 1)
#define	EXT3_N_BLOCKS			(EXT3_TIND_BLOCK + 1)

typedef struct ext3_inode_s {
 __u16 i_mode;    /* File mode */
 __u16 i_uid;     /* Low 16 bits of Owner Uid */
 __u32 i_size;    /* �ļ���С����λ�� byte */
 __u32 i_atime;   /* Access time */
 __u32 i_ctime;   /* Creation time */
 __u32 i_mtime;   /* Modification time */
 __u32 i_dtime;   /* Deletion Time */
 __u16 i_gid;     /* Low 16 bits of Group Id */
 __u16 i_links_count;          /* Links count */
 __u32 i_blocks;               /* blocks ���� */
 __u32 i_flags;                /* File flags */
 __u32 l_i_reserved1;          /* ���Ժ��� */
 __u32 i_block[EXT3_N_BLOCKS]; /* һ�� block ָ�� */
 __u32 i_generation;           /* ���Ժ��� */
 __u32 i_file_acl;             /* ���Ժ��� */
 __u32 i_dir_acl;              /* ���Ժ��� */
 __u32 i_faddr;                /* ���Ժ��� */
 __u8  l_i_frag;               /* ���Ժ��� */
 __u8  l_i_fsize;              /* ���Ժ��� */
 __u16 i_pad1;                 /* ���Ժ��� */
 __u16 l_i_uid_high;           /* ���Ժ��� */
 __u16 l_i_gid_high;           /* ���Ժ��� */
 __u32 l_i_reserved2;          /* ���Ժ��� */
 
}ext3_inode_t;

/*
 * Structure of a directory entry
 */
#define EXT2_NAME_LEN 255
typedef struct ext2_dir_entry_2_s {
	__le32	inode;			/* Inode number */
	__le16	rec_len;		/* Directory entry length */
	__u8	name_len;		/* Name length */
	__u8	file_type;
	char	name[EXT2_NAME_LEN];	/* File name */
}ext2_dir_entry_2;


typedef struct ext2_fstat_s 
{
	__u32 st_dev;     /* �ļ������豸�ı�ʶ */
 	__u32     st_ino;     /* �ļ����� */
	__u32    st_mode;    /* �ļ�����ģʽ */
	__u32   st_nlink;   /* Ӳ������ */
	__u32     st_uid;     /* �ļ��û���ʶ */
	__u32     st_gid;     /* �ļ��û����ʶ */
	__u32     st_rdev;    /* �ļ�����ʾ�������豸�ļ����豸��ʶ */
	__u32     st_size;    /* �ܴ�С���ֽ�Ϊ��λ */
	__u32 	st_blksize; /* �ļ�ϵͳ�Ŀ��С */
	__u32 	st_blocks; /* ������ļ��Ŀ������*/
	__u32    st_atim;   /* ������ʱ�� */
	__u32    st_mtim;   /* ����޸�ʱ�� */
	__u32    st_ctim;   /* ���״̬�ı�ʱ�� */

	
}ext2_fstat; 

enum {
	EXT2_FT_UNKNOWN		= 0,
	EXT2_FT_REG_FILE	= 1,
	EXT2_FT_DIR		= 2,
	EXT2_FT_CHRDEV		= 3,
	EXT2_FT_BLKDEV		= 4,
	EXT2_FT_FIFO		= 5,
	EXT2_FT_SOCK		= 6,
	EXT2_FT_SYMLINK		= 7,
	EXT2_FT_MAX
};


#define S_IFMT  00170000
#define S_IFSOCK 0140000
#define S_IFLNK	 0120000
#define S_IFREG  0100000
#define S_IFBLK  0060000
#define S_IFDIR  0040000
#define S_IFCHR  0020000
#define S_IFIFO  0010000
#define S_ISUID  0004000
#define S_ISGID  0002000
#define S_ISVTX  0001000

#define S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m)	(((m) & S_IFMT) == S_IFSOCK)

#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001


#define ROOT_INODE  2

#endif
