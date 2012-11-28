/*
 * ----------------------------------------------------------------------------
 * Copyright (c) 2012 Miguel Angel Gabriel. All Rights Reserved. 
 * Released under the GNU GPL V3. 
 * ----------------------------------------------------------------------------
 */
"use strict";

var Mags = Mags || {};
Mags.Tools = Mags.Tools || {};

(function($, self) {

	self.getISODateTime =
			function(d) {
				// padding function
				var s = function(a, b) {
					return (1e15 + a + "").slice(-b);
				};

				// default date parameter
				if (typeof d === 'undefined') {
					d = new Date();
				}
				;

				// return ISO datetime
				return d.getFullYear()
						+ '-'
						+ s(d.getMonth() + 1, 2)
						+ '-'
						+ s(d.getDate(), 2)
						+ ' '
						+ s(d.getHours(), 2)
						+ ':'
						+ s(d.getMinutes(), 2)
						+ ':'
						+ s(d.getSeconds(), 2);
			};

	self.getDisplayTime = function(d) {
		// padding function
		var s = function(a, b) {
			return (1e15 + a + "").slice(-b);
		};

		// default date parameter
		if (typeof d === 'undefined') {
			d = new Date();
		} else if (typeof d === 'number') {
			d = new Date(d * 1000);
		}

		// return ISO datetime
		return s(d.getHours(), 2) + ':' + s(d.getMinutes(), 2);
	};

	self.getDisplayDuration = function(duration) {
		var h = Math.floor(duration / 3600);
		var s1 = duration - (h * 3600);
		var m = Math.floor(s1 / 60);

		h = (h < 10) ? '0' + h : h;
		m = (m < 10) ? '0' + m : m;

		return h + ':' + m;
	};

	self.getDisplayRemaining = function(start, end) {
		var dStart = new Date(start * 1000);
		var dEnd = new Date(end * 1000);
		var dNow = new Date();
		var duration = dEnd - dStart;

		var remaining = duration - (dNow - dStart);

		return this.getDisplayDuration(remaining / 1000);
	};

	self.getElapsedDuration = function(start, end, scale) {
		var dStart = new Date(start * 1000);
		var dEnd = new Date(end * 1000);
		var dNow = new Date();
		var duration = dEnd - dStart;

		if (dStart > dNow) {
			return 0;
		}

		if (dEnd < dNow) {
			return scale;
		}

		var elapsed = (duration > 0) ? ((dNow - dStart) / duration) : 0;

		var result = Math.floor(scale * elapsed);

		return result;
	};

	self.sortByKey = function(array, key, zeroLast) {
		return array.sort(function(a, b) {

			var keys = key.split(',');

			var x = '';
			var y = '';

			var padLength = 20;
			$.each(keys, function(i, key) {

				var keyParts = key.split(':');
				var keyName = keyParts[0];
				var keyType = 's';
				if (keyParts.length > 0) {
					keyType = keyParts[1];
				}

				var va = a[keyName].toString();
				var vb = b[keyName].toString();

				if (zeroLast) {
					if (va == 0) {
						va = self.strRepeat('z', padLength);
					}
					if (vb == 0) {
						vb = self.strRepeat('z', padLength);
					}
				}
				;

				var padA = self.strRepeat(' ', padLength - va.length);
				var padB = self.strRepeat(' ', padLength - vb.length);

				x += (keyType == 'n') ? padA + va : va + padA;
				y += (keyType == 'n') ? padB + vb : vb + padB;
			});

			x = x.toUpperCase();
			y = y.toUpperCase();

			return ((x < y) ? -1 : ((x > y) ? 1 : 0));
		});
	};

	self.strRepeat = function(str, num) {
		return new Array(isNaN(num) ? 1 : num + 1).join(str);
	};

	self.isArray = function(input) {
		return typeof (input) == 'object' && (input instanceof Array);
	};

	self.runTasks = function(tasks, idx) {
		idx = idx || 0;
		var task = tasks[idx];
		if (self.isArray(task)) {
			task[0](task[1]);
		} else {
			task();
		}
		idx++;
		var me = this;
		if (idx < tasks.length) {
			setTimeout(function() {
				me.runTasks(tasks, idx);
			}, 1);
		}
	};

	self.getCurrentBaseUrl = function() {
		var location = window.location;
		var port = location.port ? ':' + location.port : '';
		var ret = location.protocol + '//' + location.hostname + port + location.pathname;
		return ret;
	};

	self.isNumber = function(n) {
		return !isNaN(parseFloat(n)) && isFinite(n);
	};

})(jQuery, Mags.Tools);

/*
 * Class to manage asynchronous fill of a listview
 */
(function($, self) {

	/**
	 * Constructor
	 * 
	 * @param $list
	 * @returns {self.ListFiller}
	 */
	self.ListFiller = function($list) {
		this.$list = $list;
		this.tasks = [];
		this.items = [];
	};

	self.ListFiller.prototype = {

		/**
		 * Add one item to the task queue. A task item is made of a data element
		 * with the data to be added to the listview and a function object that
		 * transform the data element into a text representation of the listview
		 * item to be added.
		 * 
		 * @param entry
		 * @param fnFillOne
		 */
		addItem : function(entry, fnFillOne) {
			this.tasks.push([ fnFillOne, entry ]);
		},

		/**
		 * Run the tasks in the queue sequentially
		 * 
		 * @param fnWrapUp
		 *            A function object to finish the list
		 */
		run : function(fnWrapUp) {
			// Push a task to append all the items to the listview and refresh it
			this.tasks.push([ function(me) {
				me.$list.empty();
				me.$list.append(me.items.join(''));
				me.$list.listview('refresh')
			}, this ]);
			// Push the real wrapup function
			this.tasks.push(fnWrapUp);
			// And run everything
			this._doRun(0);
		},

		/**
		 * Internal recursive function to execute all task in sequence
		 * 
		 * @param idx
		 *            The next task index
		 */
		_doRun : function(idx) {
			// Get the task and execute it
			var task = this.tasks[idx];
			if (typeof (task) == 'object' && (task instanceof Array)) {
				// A task with data
				this.items.push(task[0](task[1]));
			} else {
				// Without data
				this.items.push(task());
			}
			idx++;
			var me = this;
			if (idx < this.tasks.length) {
				// Call self to execute next task
				setTimeout(function() {
					me._doRun(idx);
				}, 1);
			}
		}
	};

})(jQuery, Mags.Tools);

/*
 * A plugin to manage localization with jquery.localize.js
 */
(function($) {

	var options = {
		pathPrefix : "js", // Where the translation files are
		skipLanguage : /^en/
	}; // Default language (to avoid trying to translate it)

	var $items;

	/*
	 * jquery.localize doesn't work on buttons that have already been enhanced by
	 * JQuery Mobile. We need to override the translation callback to provide an
	 * additional case for the button's markup. 
	 */
	options.callback = function(data, defaultCallback) {

		return $items.each(function() {
			var elem, key, value;
			elem = $(this);
			key = elem.attr("rel").match(/localize\[(.*?)\]/)[1];
			value = valueForKey(key, data);
			if (elem.is('input')) {
				if (elem.is("[placeholder]")) {
					return elem.attr("placeholder", value);
				} else {
					return elem.val(value);
				}
			} else if (elem.is('optgroup')) {
				return elem.attr("label", value);
			} else if (elem.is('img')) {
				value = valueForKey("" + key + ".alt", data);
				if (value != null) {
					elem.attr("alt", value);
				}
				value = valueForKey("" + key + ".src", data);
				if (value != null) {
					return elem.attr("src", value);
				}
			} else if (elem.is('a')) {
				/* If this is an already-enhanced button, modify just
				 * the html part of the inner span
				 */
				var button = elem.find(".ui-btn-text");
				if (button && button.html()) {
					value = valueForKey(key, data);
					return button.html(value);
				} else {
					return elem.html(value);
				}
			} else {
				return elem.html(value);
			}
		});
	};

	/*
	 * This is just a copy of the same function inside jquery.localize, because
	 * it cannot be called directly from our callback.
	 */
	var valueForKey = function(key, data) {
		var keys, value, _i, _len;
		keys = key.split(/\./);
		value = data;
		for (_i = 0, _len = keys.length; _i < _len; _i++) {
			key = keys[_i];
			value = value != null ? value[key] : null;
		}
		return value;
	};

	/*
	 * The JQuery plugin call
	 */
	$.fn.translate = function() {
		$items = this.find("[rel*=localize]");
		if ($items.length == 0) {
			return this;
		}
		return $items.localize("lang", options);
	};
	
	/*
	 * Translate a item
	 */
	$.translate = function(item) {
		var $elem = $("<div/>").attr("rel", "localize["+item+"]");
		$('<p/>').append($elem).translate();
		var value = $elem.text();
		return value;
	};

	/*
	 * Used to force a target language, in case the autodetection
	 * cannot be reliable enough. 
	 */
	$.setLanguage = function(lang) {
		options.language = lang;
	};

})(jQuery);

/*
 * Several mini plugins
 */
(function($) {
	$.fn.reverse = [].reverse;
	$.fn.sort = [].sort;
	$.fn.shift = [].shift;
})(jQuery);