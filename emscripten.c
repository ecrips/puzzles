#include <string.h>
#include <emscripten.h>
#include "puzzles.h"

extern const game *gamelist[];
extern const int gamecount;

struct frontend
{
	midend *me;
	const game *game;

	int timer_active;
};

static void em_draw_text(void *handle, int x, int y, int fonttype, int fontsize,
	int align, int colour, char *text)
{
	printf("TODO: draw text '%s'\n",text);
}

static void em_draw_rect(void *handle, int x, int y, int w, int h, int colour)
{
	printf("TODO: draw rect\n");
}

static void em_draw_line(void *hande, int x1, int y1, int x2, int y2,
			int colour)
{
	printf("TODO: draw line\n");
}

static void em_draw_polygon(void *handle, int *coords, int npoints,
			int fillcolour, int outlinecolour)
{
	printf("TODO: draw polygon\n");
}

static void em_draw_circle(void *handle, int cx, int cy, int radius,
			int fillcolour, int outlinecolour)
{
	printf("TODO: draw circle\n");
}

static void em_draw_update(void *handle, int x, int y, int w, int h)
{
	printf("TODO: draw update\n");
}

static void em_clip(void *handle, int x, int y, int w, int h)
{
	printf("TODO: clip\n");
}

static void em_unclip(void *handle)
{
	printf("TODO: unclip\n");
}

static void em_start_draw(void *handle)
{
	printf("TODO: start draw\n");
}

static void em_end_draw(void *handle)
{
	printf("TODO: end draw\n");
}

static void em_status_bar(void *handle, char *text)
{
	printf("TODO: status bar\n");
}

struct blitter
{
	int dummy;
};

static blitter *em_blitter_new(void *handle, int w, int h)
{
	printf("TODO: blitter new\n");
	return snew(blitter);
}

static void em_blitter_free(void *handle, blitter *bl)
{
	printf("TODO: blitter free\n");
	sfree(bl);
}

static void em_blitter_save(void *handle, blitter *bl, int x, int y)
{
	printf("TODO: blitter save\n");
}

static void em_blitter_load(void *handle, blitter *bl, int x, int y)
{
	printf("TODO: blitter load\n");
}

static void em_begin_doc(void *handle, int pages)
{
	printf("TODO: begin doc\n");
}

static void em_begin_page(void *handle, int number)
{
	printf("TODO: begin page\n");
}

static void em_begin_puzzle(void *handle, float xm, float xc,
			 float ym, float yc, int pw, int ph, float wmm)
{
	printf("TODO: begin puzzle\n");
}

static void em_end_puzzle(void *handle)
{
	printf("TODO: end puzzle\n");
}

static void em_end_page(void *handle, int number)
{
	printf("TODO: end page\n");
}

static void em_end_doc(void *handle)
{
	printf("TODO: end doc\n");
}

static void em_line_width(void *handle, float width)
{
	printf("TODO: line width\n");
}

static void em_line_dotted(void *handle, int dotted)
{
	printf("TODO: line dotted\n");
}

static char *em_text_fallback(void *handle, const char *const *strings,
			   int nstrings)
{
	printf("TODO: text_fallback\n");
	return dupstr(strings[0]);
}

static void em_draw_thick_line(void *handle, float thickness,
			    float x1, float y1, float x2, float y2,
			    int colour)
{
	printf("TODO: draw thick line\n");
}

static struct drawing_api drapi = {
	em_draw_text,
	em_draw_rect,
	em_draw_line,
	em_draw_polygon,
	em_draw_circle,
	em_draw_update,
	em_clip,
	em_unclip,
	em_start_draw,
	em_end_draw,
	em_status_bar,
	em_blitter_new,
	em_blitter_free,
	em_blitter_save,
	em_blitter_load,
	em_begin_doc,
	em_begin_page,
	em_begin_puzzle,
	em_end_puzzle,
	em_end_page,
	em_end_doc,
	em_line_width,
	em_line_dotted,
	em_text_fallback,
	em_draw_thick_line,
};

static const game *pick_game(void)
{
	int i;
	for(i=0;i<gamecount;i++) {
		if (!strcmp(gamelist[i]->name, "Bridges")) {
			return gamelist[i];
		}
	}
	return gamelist[0];
}

void em_puzzle_init(void)
{
	int x = 1024,y = 1024;
	frontend *fe = snew(frontend);

	memset(fe, 0, sizeof(*fe));

	fe->game = pick_game();

	fe->me = midend_new(fe, fe->game, &drapi, NULL);

	midend_new_game(fe->me);

	midend_size(fe->me, &x, &y, 0);

	midend_redraw(fe->me);
}

void get_random_seed(void **randseed, int *randseedsize)
{
	float *r = snew(float);
	*r = emscripten_random();
	*randseed = r;
	*randseedsize = sizeof(float);
}

void deactivate_timer(frontend *fe)
{
	fe->timer_active = 0;
}
