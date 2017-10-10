tinymarkdown
============

Single-file C++ Markdown-to-HTML conversion code.


What is this for?
-----------------

This is for programs that need to generate HTML from Markdown (e.g. to display Markdown in a web view or generate web pages) that do not wish to pull in a lot of dependencies.

What does it do?
----------------

Currently it only implements part of Markdown (the parts I needed right now). Those are:

1. Underlined H1/H2 headers (using == and --)
2. Italics using *asterisks*
3. Image links using the ![alt text](url/or/path) syntax
4. Escaping of < and > characters.

More will be added as needed. Contributions are greatly welcome.


License
-------

	Copyright 2017 by Uli Kusterer.
	
	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.
	
	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:
	
	   1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	
	   2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	
	   3. This notice may not be removed or altered from any source
	   distribution.
