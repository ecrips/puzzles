#include <string.h>
#include <emscripten.h>
#include "puzzles.h"

extern const game *gamelist[];
extern const int gamecount;

/* Defined in JS */
void init_colour(int,int,int,int);
void canvas_text(int,int,int,int,char*,char*,int,char*);
void canvas_rect(int,int,int,int,int);
void set_canvas_size(int,int);
void canvas_circle(int,int,int,int,int);

struct frontend
{
	midend *me;
	const game *game;

	int timer_active;
};

static void em_draw_text(void *handle, int x, int y, int fonttype, int fontsize,
	int align, int colour, char *text)
{
	char *alignstr = "start";
	char *valignstr = "alphabetic";
	switch(align) {
		case ALIGN_VCENTRE | ALIGN_HCENTRE:
			alignstr = "center";
			valignstr = "middle";
			y += fontsize/10; /* HACK */
			break;
		default:
			printf("Unknown text alignment (%d)\n", align);
	}
	canvas_text(x, y, fonttype == FONT_FIXED, fontsize,
		alignstr, valignstr, colour, text);
}

static void em_draw_rect(void *handle, int x, int y, int w, int h, int colour)
{
	canvas_rect(x, y, w, h, colour);
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
	canvas_circle(cx, cy, radius, fillcolour, outlinecolour);
}

static void em_draw_update(void *handle, int x, int y, int w, int h)
{
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
}

static void em_end_draw(void *handle)
{
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

void frontend_default_colour(frontend *fe, float *output)
{
	output[0] = 1;
	output[1] = 1;
	output[2] = 1;
}

void snaffle_colours(frontend *fe)
{
	int ncolours, i;
	float *colours;

	colours = midend_colours(fe->me, &ncolours);
	for(i=0; i<ncolours; i++) {
		init_colour(i, 	colours[i*3]*0xFF,
				colours[i*3+1]*0xFF,
				colours[i*3+2]*0xFF);
	}
}

void em_puzzle_init(void)
{
	int x = 1024,y = 1024;
	frontend *fe = snew(frontend);

	memset(fe, 0, sizeof(*fe));

	fe->game = pick_game();

	fe->me = midend_new(fe, fe->game, &drapi, fe);

	midend_new_game(fe->me);

	snaffle_colours(fe);

	midend_size(fe->me, &x, &y, 0);

	set_canvas_size(x,y);

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
