/*
    This file is part of libtermui.

    libtermui is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libtermui is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libtermui.  If not, see <http://www.gnu.org/licenses/>.

    Copyright 2006, Alexandre Becoulet <alexandre.becoulet@free.fr>

*/

#ifndef TERMUI_CONSOLE_H_
#define TERMUI_CONSOLE_H_

#include <termui/termui_config.h>
#include <termui/types.h>
#include <unistd.h>

/**
 * @this creates a new console context
 *
 * @param root console entries array to use as command root
 * @param in input file descriptor
 * @param out output file descriptor
 * @param type terminal type string, passed to @ref termui_term_set
 */
struct termui_con_ctx_s *
termui_con_alloc(struct termui_con_entry_s **root,
		 termui_iostream_t in, termui_iostream_t out,
		 const char *type);


/**
 * @this frees console resources allocated by @ref termui_con_alloc
 */
void termui_con_free(struct termui_con_ctx_s *con);


/**
 * @this changes console prompt string
 */
void termui_con_set_prompt(struct termui_con_ctx_s *con,
			   const char *prompt);


/**
 * @this appends a command entries array at end of console root entry list
 */
void termui_con_register(struct termui_con_ctx_s *con,
			 struct termui_con_entry_s *array);


/**
 * @this appends a command entries array at end of given root.
 */
void termui_con_append(struct termui_con_entry_s **list,
		       struct termui_con_entry_s *array,
		       termui_size_t max_count);


/**
 * @this unregisters all entries in array from their list/group
 */
void termui_con_unregister(struct termui_con_entry_s *array);


/**
 * @this executes a command line. Line buffer may be modified.
 */
termui_err_t termui_con_execute(struct termui_con_ctx_s *con, char *line);


#ifdef CONFIG_LIBTERMUI_FILEOPS
/**
 * @this executes commands from file. Execution is aborted on error if
 * @tt err_abort is set.
 */
termui_err_t termui_con_execute_file(struct termui_con_ctx_s *con,
				     const char *file, termui_bool_t err_abort);
#endif

/**
 * @this waits for user entry using @ref termui_getline_process and execute
 * the command using @ref termui_con_execute. Line in added to
 * history.
 *
 * @return the @ref termui_con_command_t execution result.
 */
termui_err_t termui_con_process(struct termui_con_ctx_s *con);


/**
 * @this waits for user entry using @ref termui_getline_process and return
 * the entered line buffer. The prompt is temporarily changed if @tt
 * prompt parameter is non @ref #NULL.
 */
const char * termui_con_input(struct termui_con_ctx_s *con,
			      const char *prompt);


/**
 * @return console user private data.
 * @see termui_con_set_private
 */
void * termui_con_get_private(struct termui_con_ctx_s *con);


/**
 * @this sets console user private data.
 * @see termui_con_get_private
 */
void termui_con_set_private(struct termui_con_ctx_s *con, void *pv);


/**
 * @this sets the console access mask.
 */
#ifdef CONFIG_LIBTERMUI_CON_ACL
void termui_con_set_acl(struct termui_con_ctx_s *con,
			termui_con_bits_t acl_mask);
#endif

/**
 * @this prints formated string on the console. @see termui_term_printf.
 */
termui_ssize_t termui_con_printf(struct termui_con_ctx_s *con, const char *fmt, ...);

/**
 * @this prints string on the console. @see termui_term_print.
 */
termui_ssize_t termui_con_puts(struct termui_con_ctx_s *con, const char *string);

/**
 * @this initialize a remote telnet client by sending IAC commands
 * through the output file descriptor. This is useful when the console
 * is used with a TCP socket.
 */

void termui_con_telnet_setup(struct termui_con_ctx_s *con);


/**
 * @this describe a completion context and is used when writing a new
 * console command arguments completion handler.
 * @see termui_con_args_collect_t
 */
struct termui_con_complete_ctx_s
{
  /** completed word start */
  char *start;
  /** cursor position */
  char *end;
  /** characted to append at end of completed word */
  char suffix;
  /** completion entries count */
  termui_comp_index_t count;
  /** completion entries table to fill */
  const char *candidate[CONFIG_LIBTERMUI_CON_MAXCOMPLETE];
};

/**
 * @this is used for return value of the @ref termui_con_comp_match
 * function.
 */
enum termui_con_comp_match_e
{
  /** Match whole token, may not reach cursor position */
  termui_con_match_token = -1,
  /** Do not match */
  termui_con_match_dont = 0,
  /** Partially match token, reach cusor position */
  termui_con_match_partial = 1,
  /** Match whole token and reach cusor position */
  termui_con_match_complete = 2,
};

/**
 * @this may be used to check if a token string may be added to
 * completion candidate list. This is useful when writing a new
 * console command arguments completion handler.
 * @see termui_con_args_collect_t.
 *
 * @param cctx completion context
 * @param token Nul terminated string of candidate token.
 * @param len filled with match substring len if not @ref #NULL.
 * @return match status, see @ref termui_con_comp_match_e
 */
enum termui_con_comp_match_e
termui_con_comp_match(const struct termui_con_complete_ctx_s *cctx,
		      const char *token, termui_strlen_t *len);

/**
 * @this can be used to check if a token string may be added to
 * completion candidate list. This function may be used to implement
 * substring completion as a fallback completion method.
 * @see termui_con_comp_match.
 *
 * @param cctx completion context
 * @param token Nul terminated string of candidate token.
 */
termui_bool_t
termui_con_comp_substr(struct termui_con_complete_ctx_s *ctx,
		       const char *token);


/** @this defines the prototype for the @ref termui_con_command_t function type. */
#define TERMUI_CON_COMMAND_PROTOTYPE(f)			\
  termui_err_t f(struct termui_con_ctx_s *con, void *ctx,	\
		 termui_arg_index_t argc, char *argv[], termui_con_bits_t used)
/**
 * This function type must be used to implement a console command. The
 * associated functions will be called when console command are executed.
 *
 * @param con invoking console, may be used to retrieve console user private data.
 * @param ctx user argument parsing context filled by @ref termui_con_parse_opt_t
 *      functions. See @ref #TERMUI_CON_OPT_CONTEXT macro.
 * @param argc number of arguments in @tt argv, bounded by
 *	@ref #TERMUI_CON_ARGS macro.
 * @param argv command arguments. Scored options are not included
 *	if option parsing is used, see @ref #TERMUI_CON_OPTS_CTX.
 * @param used ored masks of options in use, see #TERMUI_CON_OPT_ENTRY.
 *
 * @return error code passed to @ref termui_con_process or @ref termui_con_execute.
 *
 * The @ref #TERMUI_CON_ENTRY macro must be used to describe a console command entry.
 */
typedef TERMUI_CON_COMMAND_PROTOTYPE(termui_con_command_t);



/** @this defines the prototype for the @ref termui_con_parse_opt_t function type. */
#define TERMUI_CON_PARSE_OPT_PROTOTYPE(f)		\
  termui_err_t f(struct termui_con_ctx_s *con,		\
		 void *ctx, char *argv[])

/**
 * This function type must be used to implement a command option
 * parser. The associated functions will be called when the command
 * line parser encounter a scored option argument as described by the
 * @ref #TERMUI_CON_OPT_ENTRY and @ref #TERMUI_CON_OPT_PARSE macro.
 *
 * @param con invoking console, may be used to retrieve associated user private data.
 * @param ctx private option parsing context to updated, later used by
 *      @ref termui_con_command_t functions. See @ref #TERMUI_CON_OPT_CONTEXT macro.
 * @param argv option arguments array.
 *
 * @return zero on success. Parsing and command execution is aborted if non zero.
 *
 * The @ref termui_con_args_cleanup_t function type can be used to
 * implement a user context cleanup handler which is called after
 * command execution. This give a chance to free resources allocated
 * by all options parsing handlers. This function is called even if
 * parsing is aborted at some point. The private parsing context is
 * initialized to zero when allocated by the library.
 */
typedef TERMUI_CON_PARSE_OPT_PROTOTYPE(termui_con_parse_opt_t);



/** @this defines the prototype for the @ref termui_con_args_collect_t function type. */
#define TERMUI_CON_ARGS_COLLECT_PROTOTYPE(f)		\
  void * (f)(struct termui_con_ctx_s *con,		\
	     struct termui_con_complete_ctx_s *cctx,	\
	     termui_con_bits_t id)

/**
 * This function must be used to implement both argument and options
 * completion handlers.
 *
 * Arguments completion handler may be defined for non option arguments
 * of a command using the @ref #TERMUI_CON_ENTRY along with @ref
 * #TERMUI_CON_COMPLETE macros.
 *
 * Scored option may have individual completion handlers for their
 * arguments. Options completion handler may be defined using the @ref
 * #TERMUI_CON_OPT_ENTRY along with @ref #TERMUI_CON_COMPLETE macros.
 *
 * Completion handlers must fill the list of matching completion
 * entries stored in the completion context. See @ref
 * termui_con_comp_match and @ref termui_con_complete_ctx_s.
 *
 * This function may need to allocate memory for entry strings. An
 * optional cleanup handler (@ref termui_con_args_cleanup_t) will be
 * called once entries are no longer needed. Pointer to user private
 * data returned by this function will be passed to the cleanup handler.
 * The handler is not called if no entries were added.
 *
 * @param con invoking console, may be used to retrieve console user private data.
 * @param cctx completion context to update with entries.
 * @param id index of the first command argument or option argument to complete.
 */
typedef TERMUI_CON_ARGS_COLLECT_PROTOTYPE(termui_con_args_collect_t);



/** @this defines the prototype for the @ref termui_con_args_cleanup_t function type. */
#define TERMUI_CON_ARGS_CLEANUP_PROTOTYPE(f)		\
  void f(struct termui_con_ctx_s *con,			\
	 void *ctx, termui_con_bits_t used)

/**
 * This function type must be used to implement cleanup handler for
 * option parsing handlers (@ref termui_con_parse_opt_t) and
 * completion handlers. (@ref termui_con_args_collect_t).
 *
 * @param con associated console
 * @param ctx user option parsing context or user completion context.
 * @param used same as @ref termui_con_parse_opt_t, not used for completion cleanup.
 *
 * @see #TERMUI_CON_COMPLETE @see #TERMUI_CON_OPTS_CTX
 */
typedef TERMUI_CON_ARGS_CLEANUP_PROTOTYPE(termui_con_args_cleanup_t);

/** @internal @this specifies the console command entry type and flags. @see termui_con_entry_s */
enum {
  /** Command entry */
  TERMUI_CON_FLAG_ISCMD		= 0x00000001,
  /** Command group entry */
  TERMUI_CON_FLAG_ISGROUP	= 0x00000002,
  /** Command alias entry */
  TERMUI_CON_FLAG_ISALIAS	= 0x00000004,
  /** Command type mask */
  TERMUI_CON_FLAG_TYPE		= 0x00000007,
  /** Command is registered, linked list fields are valids */
  TERMUI_CON_FLAG_REGISTERED	= 0x00000008,
  /** Command is hidden */
  TERMUI_CON_FLAG_HIDDEN	= 0x00000010,
  /** Command is dynamically allocated, must be freed when unregisterd */
  TERMUI_CON_FLAG_ALLOCATED	= 0x00000020,
  /** Command access is denied, used to handle acls */
  TERMUI_CON_FLAG_DENIED	= 0x00000040,
};

/** @internal @this hold completion handler functions */
struct					termui_con_complete_func_s
{
  termui_con_args_collect_t		*collect;
  termui_con_args_cleanup_t		*cleanup;
};

#define TERMUI_CON_MAXOPTS		(sizeof(termui_con_bits_t) * 8)

/** @internal @this store console command option descriptor */
struct					termui_con_opts_s
{
  const char				*str_short; /* short option name */
  const char				*str_long; /* long option name */
  const char				*desc;
  const char				*longdesc;

  termui_con_bits_t			id;	 /* option id (power of 2) */
  termui_con_bits_t			exclude; /* exclude mask */
  termui_con_bits_t			depend; /* depend mask */
  termui_arg_index_t			usecnt; /* allowed option use count - 1 */

#ifdef CONFIG_LIBTERMUI_CON_ACL
  termui_con_bits_t			acl; /* access control mask */
#endif					
  termui_con_parse_opt_t		*parse_args;
  struct termui_con_complete_func_s	complete;
  termui_arg_index_t			param_cnt;
};					

/** @internal @this store console command descriptor */
struct					termui_con_entry_s
{					
  /* user initialized fields */		
  char					cmd[CONFIG_LIBTERMUI_CON_MAXCMDLEN + 1];	/* command name */
  char					*desc;	/* command short description */
  char					*longdesc;
  uint_fast8_t				flag;
#ifdef CONFIG_LIBTERMUI_CON_ACL
  termui_con_bits_t			acl;	/* complemented authorized groups mask */
#endif					
					
  union {				
    termui_con_command_t		*process;	 /* used for commands */
    struct termui_con_entry_s		*subdir;	/* used for groups */
  } u;					

  struct termui_con_entry_s		*next;
  struct termui_con_entry_s		**prev;

  /* command arguments descripto	rs */
  termui_size_t				opts_ctx_size; /* arg parse context size */
  struct termui_con_opts_s		*opts_desc;
  termui_con_args_cleanup_t		*opts_cleanup;

  struct termui_con_complete_func_s	complete;

  termui_arg_index_t			args_min;	/* minimum args count */
  termui_arg_index_t			args_max;	/* maximum args count */
  termui_con_bits_t			opts_mandatory;	/* mandatory options mask */
  termui_con_bits_t			opts_disabled;	/* disables options mask */
};

#define TERMUI_CON_OPT_ENTRY(sname, lname, id_, ...)	\
      {						\
	.str_short = sname,			\
	.str_long = lname,			\
	.id = (id_),				\
	__VA_ARGS__				\
      }

#define TERMUI_CON_OPT_PARSE(parse_args_, count)		\
	.parse_args = (parse_args_),				\
	.param_cnt = (count),

#define TERMUI_CON_OPT_CONSTRAINTS(exclude_, depend_)	\
	.exclude = (exclude_),			\
	.depend = (depend_),			\

#define TERMUI_CON_ENTRY(cmd_func_, cname, ...)				\
      {									\
	.cmd =	cname,							\
	.u = { .process = cmd_func_ },					\
	.flag =	TERMUI_CON_FLAG_ISCMD,					\
	__VA_ARGS__							\
      }

#define TERMUI_CON_ARGS(minarg, maxarg)					\
	.args_min = (minarg),						\
	.args_max = (maxarg),

#define TERMUI_CON_OPTS(aname, amandatory, adisabled)		\
	.opts_mandatory = (amandatory),					\
	.opts_disabled = (adisabled),					\
	.opts_desc = con_opts_desc_##aname,

#define TERMUI_CON_OPTS_CTX(aname, amandatory, adisabled, cleanup)	\
	TERMUI_CON_OPTS(aname, amandatory, adisabled)			\
	.opts_ctx_size = sizeof (struct termui_optctx_##aname),		\
	.opts_cleanup = cleanup,

#define TERMUI_CON_GROUP_ENTRY(name, cname, ...)			\
      {									\
	.cmd = cname ".",						\
	.u = { .subdir = con_group_##name },				\
	.flag =	TERMUI_CON_FLAG_ISGROUP,				\
	__VA_ARGS__							\
      }

#ifdef CONFIG_LIBTERMUI_CON_ALIAS
# define TERMUI_CON_ALIAS_ENTRY(name, path, ...)			\
      {									\
	.cmd =	name,							\
	.desc = path,							\
	.flag =	TERMUI_CON_FLAG_ISALIAS,				\
	__VA_ARGS__							\
      }
#endif

#define TERMUI_CON_COMPLETE(collect_, cleanup_)				\
	.complete = {							\
          .collect = collect_,						\
	  .cleanup = cleanup_,						\
	},

#ifdef CONFIG_LIBTERMUI_CON_LONG_HELP
# define TERMUI_CON_HELP(desc_, longdesc_)				\
	.desc =	desc_,							\
	.longdesc = longdesc_,
#else
# define TERMUI_CON_HELP(desc_, longdesc_)				\
	.desc =	desc_,
#endif

#ifdef CONFIG_LIBTERMUI_CON_ACL
# define TERMUI_CON_ACL(acl_)						\
	.acl =	~(acl_),
#else
# define TERMUI_CON_ACL(acl_)
#endif


#define TERMUI_CON_LIST_END	{ }

#define TERMUI_CON_GROUP(name)		con_group_##name
#define TERMUI_CON_OPT_DECL(name)		struct termui_con_opts_s con_opts_desc_##name[]
#define TERMUI_CON_OPT_DECL_(name, size)	struct termui_con_opts_s con_opts_desc_##name[size]
#define TERMUI_CON_GROUP_DECL(name)	struct termui_con_entry_s con_group_##name[]
#define TERMUI_CON_OPT_CONTEXT(name)	struct termui_optctx_##name##_s
#define TERMUI_CON_EMPTY_ROOT(name)	struct termui_con_entry_s *name = 0

/*
 * console_complete.c
 */

TERMUI_CON_ARGS_COLLECT_PROTOTYPE(termui_con_collect_cmd);
TERMUI_CON_ARGS_COLLECT_PROTOTYPE(termui_con_collect_grp);
TERMUI_CON_ARGS_COLLECT_PROTOTYPE(termui_con_readdir_collect);
TERMUI_CON_ARGS_CLEANUP_PROTOTYPE(termui_con_readdir_cleanup);

#define TERMUI_CON_COMPLETE_FILE			\
	TERMUI_CON_COMPLETE(termui_con_readdir_collect,	\
			    termui_con_readdir_cleanup)

/*
 * termui_con_builtin.c
 */

#define con_opts_cleanup_console_builtin_list_opts NULL
extern TERMUI_CON_OPT_DECL_(termui_con_builtin_list_opts, 3);
TERMUI_CON_COMMAND_PROTOTYPE(termui_con_builtin_help);
TERMUI_CON_COMMAND_PROTOTYPE(termui_con_builtin_list);
TERMUI_CON_COMMAND_PROTOTYPE(termui_con_builtin_quit);

#ifdef CONFIG_LIBTERMUI_FILEOPS
TERMUI_CON_COMMAND_PROTOTYPE(termui_con_builtin_batch);
#endif

#ifdef CONFIG_LIBTERMUI_CON_ALIAS
TERMUI_CON_COMMAND_PROTOTYPE(termui_con_builtin_alias);
#endif

#define TERMUI_CON_BUILTIN_HELP(acl)					\
  TERMUI_CON_ENTRY(termui_con_builtin_help, "help",			\
		   TERMUI_CON_HELP("Display help about commands", NULL) \
                   TERMUI_CON_ACL(acl)					\
		   TERMUI_CON_ARGS(0, 1)				\
		   TERMUI_CON_COMPLETE(termui_con_collect_cmd, NULL))

#define TERMUI_CON_BUILTIN_BATCH(acl)					\
  TERMUI_CON_ENTRY(termui_con_builtin_batch, "batch",			\
		   TERMUI_CON_HELP("Execute commands in given files", NULL) \
                   TERMUI_CON_ACL(acl)					\
		   TERMUI_CON_ARGS(1, -1))

#define TERMUI_CON_BUILTIN_LIST(acl)					\
  TERMUI_CON_ENTRY(termui_con_builtin_list, "list",			\
		   TERMUI_CON_HELP("List available commands", NULL)	\
                   TERMUI_CON_OPTS(termui_con_builtin_list_opts, 0, 0)	\
                   TERMUI_CON_ACL(acl)					\
		   TERMUI_CON_ARGS(0, 1)				\
		   TERMUI_CON_COMPLETE(termui_con_collect_cmd, NULL))

#ifdef CONFIG_LIBTERMUI_CON_ALIAS
# define TERMUI_CON_BUILTIN_ALIAS(acl)					\
  TERMUI_CON_ENTRY(termui_con_builtin_alias, "alias",			\
		   TERMUI_CON_HELP("Define a new command alias", NULL)	\
                   TERMUI_CON_ACL(acl)					\
		   TERMUI_CON_ARGS(1, 2)				\
		   TERMUI_CON_COMPLETE(termui_con_collect_cmd, NULL))
#endif

#define TERMUI_CON_BUILTIN_QUIT(acl)				\
  TERMUI_CON_ENTRY(termui_con_builtin_quit, "quit",		\
                   TERMUI_CON_ACL(acl)				\
		   TERMUI_CON_HELP("Leave console", NULL))

#endif /* CONSOLE_H_ */

