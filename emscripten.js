var thecanvas, ctx;
var colourStyle = {};

var current_callback;
function _c_callback() {
	current_callback.apply(null, arguments);
}
function set_callback(c) {
	current_callback = c;
}

var hide_menu = function() {};

function mousedown(e) {
	hide_menu();
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

function do_menu(o, callback, populate) {
	var menu = document.createElement("div");
	menu.className = "ontopmenu";
	var list = document.createElement("div");
	menu.appendChild(list);
	var menu_visible = true;

	hide_menu = function() {
		if (menu_visible) {
			document.body.removeChild(menu);
			menu_visible=false;
		}
	}

	o.list = list;

	set_callback(callback);
	populate();

	document.body.appendChild(menu);
	menu.tabIndex = 100;
	menu.focus();
	menu.onblur = function() {
		/* setTimeout needed so clicking the radio buttons works
		 * as the click will cause a loss of focus */
		setTimeout(hide_menu, 100);
	}
}

function start_newgame() {
	var stat = _em_status();
	if (stat == 0) {
		var c = confirm("Are you sure you want to start a new game?");
		if (!c) return;
	}
	_em_new_game(1);
	return false;
}

function choose_game() {
	var o = {};
	do_menu(o, function(name, i) {
		name = Pointer_stringify(name);
		var div = document.createElement("span");
		div.className = "gamemenuitem";
		var label = document.createElement("label");
		var img = document.createElement("img");
		img.src = "web-icons/"+name+".png";
		label.appendChild(img);
		label.appendChild(document.createElement("br"));
		var namediv = document.createElement("div");
		namediv.appendChild(document.createTextNode(name));
		namediv.style['text-align'] = "center";
		label.appendChild(namediv);
		div.appendChild(label);
		o.list.appendChild(div);
		label.onclick = function() {
			hide_menu();
			location.hash = name;
			_change_game(i);
		};
	}, _populate_game_menu);
	return false;
}

function choose_type() {
	var o = {};
	do_menu(o, function(name, i, sel) {
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
		o.list.appendChild(div);
		label.onclick = function() {
			hide_menu();
			_set_preset(i);
		};
	}, _populate_type_menu);
	return false;
}

var _set_undo_redo;

function setup_menu() {
	var menu = document.getElementById("menu");
	function createButton(text) {
		var b = document.createElement("button");
		b.appendChild(document.createTextNode(text));
		menu.appendChild(b);
		return b;
	}
	var newgamemenu = createButton("New Game");
	var gamemenu = createButton("Game");
	var typemenu = createButton("Type");
	var undo = createButton("Undo");
	var redo = createButton("Redo");
	newgamemenu.ontouchstart = newgamemenu.onmousedown = start_newgame;
	gamemenu.ontouchstart = gamemenu.onmousedown = choose_game;
	typemenu.ontouchstart = typemenu.onmousedown = choose_type;
	undo.ontouchstart = undo.onmousedown = _em_undo;
	redo.ontouchstart = redo.onmousedown = _em_redo;

	_set_undo_redo = function(u, r) {
		undo.disabled = !u;
		redo.disabled = !r;
	};
}


function puzzle_init() {
	setup_menu();
	thecanvas = document.getElementById("canvas");
	ctx = thecanvas.getContext('2d');
	thecanvas.ontouchstart = thecanvas.onmousedown = mousedown;
	thecanvas.ontouchmove = thecanvas.onmousemove = mousemove;
	thecanvas.ontouchend = thecanvas.onmouseup = mouseup;
	thecanvas.oncontextmenu = function() {return false;};
	_em_puzzle_init();

	window.onresize = function() {
		_em_new_game(0);
	};
}

function _is_selected_game(game) {
	game = Pointer_stringify(game);
	var hash = location.hash;
	if ("#"+game == hash) {
		return true;
	}
	return false;
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

var blitters = [];

function _canvas_blitter_save(id, x, y, w, h)
{
	var imagedata = ctx.getImageData(x, y, w, h);
	blitters[id] = imagedata;
}

function _canvas_blitter_load(id, x, y)
{
	ctx.putImageData(blitters[id], x, y);
}

function _canvas_blitter_free(id)
{
	blitters[id] = 0;
	delete blitters[id];
}

function _canvas_clip(x, y, w, h)
{
	ctx.save();
	ctx.beginPath();
	ctx.rect(x,y,w,h);
	ctx.clip();
}

function _canvas_unclip()
{
	ctx.restore();
	//ctx.resetClip();
}

function _canvas_beginPath()
{
	ctx.beginPath();
}

function _canvas_moveTo(x, y)
{
	ctx.moveTo(x,y);
}

function _canvas_lineTo(x, y)
{
	ctx.lineTo(x,y);
}

function _canvas_strokeandfill(fillcolour, outlinecolour)
{
	ctx.closePath();
	ctx.strokeStyle = colourStyle[outlinecolour];
	ctx.stroke();
	if (fillcolour != -1) {
		ctx.fillStyle = colourStyle[fillcolour];
		ctx.fill();
	}
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

var saved_state;

function _save_state_reset()
{
	window.localStorage.setItem("state", "");
	saved_state = '';
}

function _save_state_add(str)
{
	str = Pointer_stringify(str);
	saved_state += str;
}

function _save_state_save()
{
	window.localStorage.setItem("state", saved_state);
}

function _load_state()
{
	saved_state = window.localStorage.getItem("state") || "";
}

function _load_state_read(ctx, ptr, len)
{
	if (len > saved_state.length) return false;
	writeStringToMemory(saved_state.substr(0, len), ptr, true);
	saved_state = saved_state.substr(len);
	return true;
}

window['em'] = {
	'puzzle_init' : puzzle_init
};

}).call({});
