
#include <stdlib.h>
#include <termui/term.h>
#include <termui/getline.h>

extern struct device_s *console_dev;

static TERMUI_GETLINE_FCN_PROMPT(prompt)
{
  return termui_term_printf(tm, "[%31AMutekH%A] ");
}

void app_start()
{
  struct termui_term_s			*tm;
  struct termui_term_behavior_s	*bhv;

  /* initialize terminal */
  if (!(tm = termui_term_alloc(console_dev, console_dev, NULL)))
    abort();

  /* set capabilities */
  termui_term_set(tm, "xterm");

  termui_term_printf(tm, "libtermui getline example, use Ctrl-D to quit\n\n");

  /* initialize getline behavior according to term capabilities */
  if (!(bhv = termui_getline_alloc(tm, 256)))	/* max line len = 256 */
    abort();

  termui_getline_history_init(bhv, 64); /* 64 entries max */
  termui_getline_setprompt(bhv, prompt);

  while (1)
    {
      const char *line;

      if (!(line = termui_getline_process(bhv)))
	break;

      /* skip blank line */
      if (!*(line += strspn(line, "\n\r\t ")))
	continue;

      termui_getline_history_addlast(bhv);

      termui_term_printf(tm, "entered line is: `%s'\n\n", line);
    }

  /* free resources allocated by getline behavior */
  termui_getline_free(bhv);

  /* free resources and restore terminal attributes */
  termui_term_free(tm);

  while (1)
    ;
}

