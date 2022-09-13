#ifndef FT_LS
# define FT_LS
# include "libft/libft.h"
# include "libft/containers/containers.h"
# include "libft/stdio/printf/printf.h"
# include <sys/sysmacros.h>
# include <stdbool.h>
# include <stdio.h>
# include <sys/types.h>
# include <sys/xattr.h>
# include <linux/xattr.h>
# include <sys/stat.h>
# include <unistd.h>
# include <pwd.h>
# include <grp.h>
# include <time.h>
# include <dirent.h>
# define CONV_BLK(val, new_size) (val * S_BLKSIZE) / new_size + (((val * S_BLKSIZE) % new_size) != 0)
# define DEFAULT_FILE (t_file){.blocks = "?",.inode = "?",.mode= "l?????????",.nlink = "?",.uid = "?",.gid = "?",.reg = {.size = "?"},.time = "?"}
# define DEFAULT_DIR (dir_stats){.blocks = 1, .inode = 1,.mode=10,.nlink=1,.uid = 1,.gid=1,.size = 1,}
# define SWITCH_OPT(var, opt1, opt2) case opt1: var[opt1] = true; var[opt2] = false; break; case opt2: var[opt2] = true; var[opt1] = false; break;
typedef struct {
	char	*program_name;
	bool	flags[UCHAR_MAX + 1];
	bool	multiple; // multiple operands?
	blkcnt_t 	block_size;
}		t_config;

typedef struct {
	struct stat		lstat;
	bool			stat_error : 1;
	mode_t			real_mode;
	char			name[NAME_MAX + 1];

	char	blocks[20];
	char	inode[20];
	// long form information:
	char	mode[12];
	char	nlink[20];
	char	uid[LOGIN_NAME_MAX + 1];
	char	gid[LOGIN_NAME_MAX + 1];
	union {
		struct {
			char	size[20];
		}	reg;
		struct {
			char	dev[20];
			char	rdev[20];
		}	special;
	};
	char	time[13];
	char	Fchar;
	bool	uid_is_int : 1; // gnu ls pad the value to the left if it is a string and to the right if it is an int
	bool	gid_is_int : 1; // gnu ls pad the value to the left if it is a string and to the right if it is an int
	bool	acl : 1;
}			t_file;

typedef struct {
	container	set;
	size_t		total_blocks;
	// padding values:
	int			blocks;
	int			inode;
	int			mode;
	int			nlink;
	int			uid;
	int			gid;
	int			size;
	int			dev;
	int			rdev;
}			dir_stats;

extern	t_config config;

status	parse_command(container *dirs, dir_stats *files, int argc, char **argv);
status	ft_ls_file(t_file f, dir_stats *d, container *current_path);
status	ls_all_dir(container *current_path, container *set);
status	ft_ls(t_file file);
status	ft_ls_dir(container *current_path, dir_stats *dir);
void	my_perror(char *prefix);
int		t_file_compare(type_metadata prop, void *l, void *r);
status init_file(t_file *current, char *name, char *path, dir_stats *dir);
status	get_dir(container *dirname, dir_stats *dir);
char 	*getmode(t_file f, char *mode);
char	*gettime(t_file f, char *buf);

status	get_stat(t_file *current, char *path, bool follow_link);
status	get_Fchar(char *dst, mode_t mode);
status	get_blocks(t_file *current, dir_stats *dir);
status	get_inode(t_file *current, dir_stats *dir);
status	get_size(t_file *current, dir_stats *dir);
status	get_uid(t_file *current, dir_stats *dir);
status	get_gid(t_file *current, dir_stats *dir);
status	get_nlink(t_file *current, dir_stats *dir);
status	get_time(t_file *current);
status	get_mode(t_file *current, dir_stats *dir, char *path);

#define T_FILE_METATYPE ((type_metadata){.constructor = &pointer_constructor, .destructor = &pointer_destructor, .copy = &pointer_copy, .assign = &pointer_assign, .compare = &t_file_compare, .size = sizeof( t_file )})


#endif