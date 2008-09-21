Object.prototype.mapWithKey = function(f) {
	var result = new Array();
	for (var key in this) {
		result[result.length] = f(key, this[key]);
	};
	return result;
};

Array.prototype.map = function(f) {
	var result = new Array();
	for (var i = 0; i < this.length; ++i)
	{
		result[i] = f(this[i]);
	}
	return result;
};

Object.prototype.inspect = function(json) {
	var kv = this.mapWithKey(function(k, v) { return k.toString() + ": " + v.inspect(json) });
	return "{" + kv.join(", ") + "}";
};

String.prototype.inspect = function(json) { return "\"" + this.replace(/\"/g, '\\"') + "\""; };
Function.prototype.inspect = function(json) { return json ? this.toString() : "(function)"; };
Number.prototype.inspect = Number.prototype.toString;
Array.prototype.inspect = function(json) { return "[" + this.map(function(v) { return v == undefined ? "undefined" : v.inspect(json); }).join(", ") + "]"};
RegExp.prototype.inspect = RegExp.prototype.toString;
Boolean.prototype.inspect = Boolean.prototype.toString;

function print()
{
	for (var i = 0; i < arguments.length; ++i) {
		var v = arguments[i];
		IO.stdout.write(v !== undefined ? v.toString() : 'undefined');
	}
}

function println()
{
	for (var i = 0; i < arguments.length; ++i) {
		print(arguments[i]);
		print("\n");
	}
}

function p() {
	for (var i = 0; i < arguments.length; ++i) {
		println(arguments[i] !== undefined ? arguments[i].inspect() : "undefined");
	}
}
