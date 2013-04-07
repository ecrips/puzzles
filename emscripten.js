var thecanvas, ctx;
var colourStyle = {};

var current_callback;
function _c_callback() {
	current_callback.apply(null, arguments);
}
function set_callback(c) {
	current_callback = c;
}

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

function choose_type() {
	var menu = document.createElement("div");
	menu.className = "ontopmenu";
	var list = document.createElement("div");
	menu.appendChild(list);
	var menu_visible = true;

	function hide_menu() {
		if (menu_visible) {
			document.body.removeChild(menu);
			menu_visible=false;
		}
	}

	set_callback(function(name, i, sel) {
		name = Pointer_stringify(name);
		var div = document.createElement("div");
		var radio = document.createElement("input");
		radio.type = "radio";
		radio.name = 'type';
		if (sel) radio.checked = true;
		var label = document.createElement("label");
		label.appendChild(radio);
		label.appendChild(document.createTextNode(name));
		div.appendChild(label);
		list.appendChild(div);
		label.onclick = function() {
			hide_menu();
			_set_preset(i);
		}
	});
	_populate_type_menu();

	document.body.appendChild(menu);
	menu.tabIndex = 100;
	menu.focus();
	menu.onblur = function() {
		/* setTimeout needed so clicking the radio buttons works
		 * as the click will cause a loss of focus */
		setTimeout(hide_menu, 100);
	}
}

function setup_menu() {
	var menu = document.getElementById("menu");
	var typemenu = document.createElement("button");
	typemenu.appendChild(document.createTextNode("Type"));
	menu.appendChild(typemenu);
	typemenu.onclick = choose_type;
}

function puzzle_init() {
	thecanvas = document.getElementById("canvas");
	ctx = thecanvas.getContext('2d');
	thecanvas.ontouchstart = thecanvas.onmousedown = mousedown;
	thecanvas.ontouchmove = thecanvas.onmousemove = mousemove;
	thecanvas.ontouchend = thecanvas.onmouseup = mouseup;
	_em_puzzle_init();
	setup_menu();
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
	return window.innerWidth - thecanvas.offsetLeft;
}

function _get_height()
{
	return window.innerHeight - thecanvas.offsetTop;
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
	'puzzle_init' : puzzle_init,
};

}).call({});
