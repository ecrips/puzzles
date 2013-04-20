#include <string.h>
#include <emscripten.h>
#include "puzzles.h"

extern const game *gamelist[];
extern const int gamecount;
extern frontend *fe;

/* Defined in JS */
int get_width(void);
int get_height(void);
void init_colour(int,int,int,int);
void canvas_text(int,int,int,int,char*,char*,int,char*);
void canvas_rect(int,int,int,int,int);
void set_canvas_size(int,int);
void canvas_circle(int,int,int,int,int);
void canvas_blitter_save(int,int,int,int,int);
void canvas_blitter_load(int,int,int);
void canvas_blitter_free(int);
void canvas_clip(int,int,int,int);
void canvas_unclip(void);
void canvas_beginPath(void);
void canvas_moveTo(int,int);
void canvas_lineTo(int,int);
void canvas_strokeandfill(int,int);
int is_selected_game(const char*);

void save_state_reset(void);
void save_state_add(char *str);
void save_state_save(void);

void load_state(void);
int load_state_read(void *ctx, void *buf, int len);

struct frontend
{
	midend *me;
	const game *game;
};

void save_state_callback(void *ctx, void *buf, int len)
{
	save_state_add(buf);
}

void save_state(void)
{
	save_state_reset();
	midend_serialise(fe->me, save_state_callback, NULL);
	save_state_save();
}

static void em_draw_text(void *handle, int x, int y, int fonttype, int fontsize,
	int align, int colour, char *text)
{
	char *alignstr = "start";
	char *valignstr = "alphabetic";
	switch(align) {
		case ALIGN_VCENTRE | ALIGN_HCENTRE:
			alignstr = "center";
			valignstr = "middle";
			break;
		case ALIGN_VCENTRE:
			alignstr = "left";
			valignstr = "middle";
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
	int i;
	canvas_beginPath();
	canvas_moveTo(coords[0], coords[1]);
	for(i=1;i<npoints;i++) {
		canvas_lineTo(coords[i*2], coords[i*2+1]);
	}
	canvas_strokeandfill(fillcolour, outlinecolour);
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
	canvas_clip(x, y, w, h);
}

static void em_unclip(void *handle)
{
	canvas_unclip();
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
	int id;
	int x, y, w, h;
};

static blitter *em_blitter_new(void *handle, int w, int h)
{
	struct blitter *b = snew(blitter);
	b->w = w;
	b->h = h;
	b->id = (int)b;
	return b;
}

static void em_blitter_free(void *handle, blitter *bl)
{
	canvas_blitter_free(bl->id);
	sfree(bl);
}

static void em_blitter_save(void *handle, blitter *bl, int x, int y)
{
	canvas_blitter_save(bl->id, x, y, bl->w, bl->h);
	bl->x = x;
	bl->y = y;
}

static void em_blitter_load(void *handle, blitter *bl, int x, int y)
{
	if (x == BLITTER_FROMSAVED && y == BLITTER_FROMSAVED) {
		x = bl->x; y = bl->y;
	}
	canvas_blitter_load(bl->id, x, y);
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
		if (is_selected_game(gamelist[i]->name)) {
			return gamelist[i];
		}
	}
	return gamelist[0];
}

static const game *find_game(char *name)
{
	int i;
	for(i=0;i<gamecount;i++) {
		if (!strcmp(name, gamelist[i]->name)) {
			return gamelist[i];
		}
	}
	return NULL;
}

/*
static void pick_preset(void)
{
	int num = midend_num_presets(fe->me);
	int i;
	for(i=0;i<num;i++) {
		char *name;
		game_params *params;
		midend_fetch_preset(fe->me, i, &name, &params);
		if (!strcmp(name, "15x15 medium")) {
			midend_set_params(fe->me, params);
		}
	}
}
*/

void c_callback(const char *name,int id,int sel);
void populate_game_menu()
{
	int i;
	for(i=0;i<gamecount;i++) {
		c_callback(gamelist[i]->name, i, 0);
	}
}

void populate_type_menu()
{
	int num = midend_num_presets(fe->me);
	int i;
	int sel = midend_which_preset(fe->me);
	for(i=0;i<num;i++) {
		char *name;
		game_params *params;
		midend_fetch_preset(fe->me, i, &name, &params);
		c_callback(name, i, i==sel);
	}
}

void em_new_game(int start_new)
{
	int x = get_width(),y = get_height();
	if (start_new) {
		midend_new_game(fe->me);
	}
	midend_size(fe->me, &x, &y, 1);
	set_canvas_size(x,y);
	midend_redraw(fe->me);

	save_state();
}

void set_preset(int i)
{
	char *name;
	game_params *params;
	midend_fetch_preset(fe->me, i, &name, &params);
	midend_set_params(fe->me, params);

	em_new_game(1);
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
	char *game;
	char *err;

	fe = snew(frontend);

	memset(fe, 0, sizeof(*fe));

	load_state();
	err = identify_game(&game, load_state_read, NULL);
	if (game) {
		fe->game = find_game(game);
	} else {
		printf("%s\n", err);
	}
	if (!fe->game) {
		fe->game = pick_game();
		game = NULL;
	}

	fe->me = midend_new(fe, fe->game, &drapi, fe);

	snaffle_colours(fe);

	if (game) {
		load_state();
		err = midend_deserialise(fe->me, load_state_read, NULL);
		if (err) {
			printf("%s\n", err);
		}
	}

	em_new_game(game==NULL);
}

void change_game(int id)
{
	midend_free(fe->me);
	fe->game = gamelist[id];
	fe->me = midend_new(fe, fe->game, &drapi, fe);
	snaffle_colours(fe);
	em_new_game(1);
}

void get_random_seed(void **randseed, int *randseedsize)
{
	float *r = snew(float);
	*r = emscripten_random();
	*randseed = r;
	*randseedsize = sizeof(float);
}

static int button_drag_down = 0;

void em_mousedown(int x, int y, int which)
{
	int button = 0;
	static const int button_down[] = {
		LEFT_BUTTON, MIDDLE_BUTTON, RIGHT_BUTTON
	};
	static const int button_drag[] = {
		LEFT_DRAG, MIDDLE_DRAG, RIGHT_DRAG
	};

	if (which < 3 && which >= 0) {
		button = button_down[which];
		button_drag_down = button_drag[which];
	} else
		return;
	midend_process_key(fe->me, x, y, button);
}

void em_mousemove(int x, int y)
{
	if (button_drag_down) {
		midend_process_key(fe->me, x, y, button_drag_down);
	}
}

void em_mouseup(int x, int y, int which)
{
	int button = 0;
	static const int button_up[] = {
		LEFT_RELEASE, MIDDLE_RELEASE, RIGHT_RELEASE
	};
	if (which < 3 && which >= 0) {
		button = button_up[which];
	} else
		return;
	midend_process_key(fe->me, x, y, button);
	button_drag_down = 0;

	save_state();
}

void do_timer(int t)
{
	float tplus = t/1000.f;
	midend_timer(fe->me, tplus);
}
