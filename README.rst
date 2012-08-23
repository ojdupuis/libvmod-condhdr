===========
vmod_condhdr
===========

---------------------------
Varnish Conditionnal Header
---------------------------

:Author: Olivier Dupuis
:Date: 2012-08-23
:Version: 0.9
:Manual section: 3

SYNOPSIS
========

::

	import condhdr;

	condhdr.set(<string to test>,<regexp for test>,<header field name>,<header field value>)

DESCRIPTION
===========

Implements conditionnal (regexp driven) setting of http header field and value.
Useful when Varnish is used to set the Cache-Control headers in that case you would have many :

	if (bereq.url ~ "/^/path/*.html"){
		beresp.http.Cache-Control="max-age=30s";
	}

it allows more compact configuration file
If more than one condhdr.set is matched by string, the last one overrides the others

FUNCTIONS
=========

Example VCL::
backend foo { ... };

	import condhdr;

	sub vcl_fetch {
		condhdr.set(bereq.url,"^/path/*.html","Cache-Control","max-age=30s");
	}

set
---

Prototype
	condhdr.set(<string to test>,<regexp for test>,<header field name>,<header field value>)
Returns 
	void

