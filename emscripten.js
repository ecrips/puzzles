var thecanvas, ctx;
var colourStyle = {};

function mousedown(e) {
	if (e.touches) e = e.targetTouches[0];
	_em_mousedown(e.clientX - thecanvas.offsetLeft,
		e.clientY - thecanvas.offsetTop, e.which-1);
	return false;
}

function mousemove(e) {
	if (e.touches) e = e.targetTouches[0];
	_em_mousemove(e.clientX - thecanvas.offsetLeft,
		e.clientY - thecanvas.offsetTop);
	return false;
}

function mouseup(e) {
	if (e.touches) e = e.changedTouches[0];
	_em_mouseup(e.clientX - thecanvas.offsetLeft,
		e.clientY - thecanvas.offsetTop, e.which-1);
	return false;
}

function puzzle_init() {
	thecanvas = document.getElementById("canvas");
	ctx = thecanvas.getContext('2d');
	thecanvas.ontouchstart = thecanvas.onmousedown = mousedown;
	thecanvas.ontouchmove = thecanvas.onmousemove = mousemove;
	thecanvas.ontouchend = thecanvas.onmouseup = mouseup;
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

function _get_width()
{
	return window.innerWidth;
}

function _get_height()
{
	return window.innerHeight;
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

var timer_active = false;
var timer_last;

function timer()
{
	var now = Date.now();
	if (timer_active) {
		_do_timer(now - timer_last);
		timer_last = now;
		setTimeout(timer, 20);
	}
}

function _activate_timer()
{
	if (!timer_active) {
		timer_active = true;
		timer_last = Date.now();
		setTimeout(timer, 20);
	}
}

function _deactivate_timer()
{
	timer_active = false;
}

window['em'] = {
	'puzzle_init' : puzzle_init
};
