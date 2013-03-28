var thecanvas, ctx;
var colourStyle = {};

function puzzle_init() {
	thecanvas = document.getElementById("canvas");
	ctx = thecanvas.getContext('2d');
	_em_puzzle_init();
}

function _init_colour(id, r, g, b)
{
	colourStyle[id] = "rgb("+r+","+g+","+b+")";
}

function _set_canvas_size(w, h)
{
	thecanvas.width = w;
	thecanvas.height = h;
}

function _canvas_text(x, y, fonttype, fontsize,
			alignstr, valignstr, colour, text)
{
	alignstr = Pointer_stringify(alignstr);
	valignstr = Pointer_stringify(valignstr);
	text = Pointer_stringify(text);
	var font = "bold " + fontsize + " px";
	if (fonttype == 1) {
		font += " monospace";
	} else {
		font += " Verdana,sans-serif";
	}
	ctx.font = font;
	ctx.textAlign = alignstr;
	ctx.textBaseline = valignstr;
	ctx.fillStyle = colourStyle[colour];
	ctx.fillText(text, x, y);
}

function _canvas_rect(x, y, w, h, colour)
{
	ctx.fillStyle = colourStyle[colour];
	ctx.fillRect(x, y, w, h);
}

function _canvas_circle(x, y, radius, fillcolour, outlinecolour)
{
	ctx.fillStyle = colourStyle[fillcolour];
	ctx.strokeStyle = colourStyle[outlinecolour];
	ctx.beginPath();
	ctx.arc(x, y, radius, 0, Math.PI*2, true);
	ctx.stroke();
	ctx.fill();
}

window['em'] = {
	'puzzle_init' : puzzle_init
};
