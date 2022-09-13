#include "ft_ls.h"

#define HELP_USAGE "ft_ping [options] <destination>"
#define OPTIONS "-:CRacdilrtuLHgnops" "q1"

static const char *options[][2] = {
		{"-h", "print help and exit"},
		{NULL, NULL}
};

void	show_help()
{
	dprintf(2, "\nUsage:\n  %s\n\n", HELP_USAGE);
	dprintf(2, "Options:\n");
	for (int i = 0; options[i][0]; i++)
		dprintf(2, "  %-20s%s\n", options[i][0], options[i][1]);
}

status	parse_command(container *dirs, dir_stats *files, int argc, char **argv) {
	int retgetopt;
	status ret = OK;
	int n = 0;

	config.program_name = ft_strdup(argv[0]);
	if (!config.program_name)
		return (FATAL);
	config.block_size = ft_posixly_correct() ? 512 : 1024; // TODO: parse env
	while ((retgetopt = ft_getopt(argc, argv, OPTIONS)) != -1) {
		switch (retgetopt) {
			SWITCH_OPT(config.flags, 'u', 'c')
			SWITCH_OPT(config.flags, 'H', 'L')
			case ':':
				ft_fprintf(ft_stderr, "%s: missing argument for option -- '%c'\n", config.program_name, ft_optopt);
				goto error;
			case '?':
				ft_fprintf(ft_stderr, "%s: invalid option -- '%c'\n", config.program_name, ft_optopt);
				goto error;
			default:
				config.flags[retgetopt] = true;
				break;
		}
	}
	if (config.flags['g'] || config.flags['o'])
		config.flags['l'] = true;
	ft_optind = 1;
	while ((retgetopt = ft_getopt(argc, argv, OPTIONS)) != -1) {
		if (retgetopt == 1)
		{
			n++;
			t_file current = DEFAULT_FILE;
			container *dst = dirs;
			SWITCH_STATUS(get_stat(&current, ft_optarg,!(config.flags['d'] || config.flags['l'] || config.flags['F']) || config.flags['H'] || config.flags['L']),,ret = KO; continue,);
			if ((S_ISDIR(current.lstat.st_mode) && !S_ISLNK(current.real_mode)) ||
				(S_ISDIR(current.lstat.st_mode) && S_ISLNK(current.real_mode) && (!(config.flags['d'] || config.flags['l'] || config.flags['F']) || (config.flags['H'] || config.flags['L'])))) {
				init_file(&current, ft_optarg, ft_optarg, NULL);
			}
			else {
				init_file(&current, ft_optarg, ft_optarg, files);
				dst = &files->set;
			}
			if (current.stat_error)
				continue;
			iterator end = ft_btree_end(dst);
			iterator inserted = ft_btree_insert_val(dst, &current);
			if (ft_btree_iterator_compare(dst->value_type_metadata, &end, &inserted) == 0)
				return (FATAL);
		}
	}
	if (!n)
	{
		t_file current;
		get_stat(&current, ".", config.flags['L']);
		init_file(&current, ".", ".", NULL);
		iterator end = ft_btree_end(dirs);
		iterator inserted = ft_btree_insert_val(dirs, &current);
		if (ft_btree_iterator_compare(dirs->value_type_metadata, &end, &inserted) == 0)
			return (FATAL);
	}
	config.multiple = dirs->size + files->set.size > 1;
	return (ret);
	error:
	show_help();
	return (KO);
}
