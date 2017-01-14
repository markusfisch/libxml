libxml
======

Incremental XML parser in plain C.

Processes XML input in chunks without caching the entire document.

Creates a _immutable_ structure of nested linked lists and provides some
handy functions to find elements in that structure.

It's designed to be as small, simple and portable as possible and will
never extend it's original purpose of just parsing XML data on the fly.

Compatibility
-------------

libxml is intended to be used on simple XML data and does not support
[document type declarations][1].

Sample
------

Reading XML data from STDIN and watch for a certain element:

	#include <stdio.h>

	#include "xml.h"

	int main(void) {
		struct xml_state st;

		memset(&st, 0, sizeof(st));

		for (;;) {
			char buf[256];

			/* read from stdin */
			{
				size_t n = read(
					STDIN_FILENO,
					buf,
					sizeof(buf));

				if (n < 1) {
					break;
				}

				buf[n] = 0;
			}

			if (xml_parse_chunk(&st, buf)) {
				xml_free(st.root);
				fprintf(stderr, "error: parse error\n");
				return -1;
			}

			if (st.root) {
				struct xml_element *e;

				if ((e = xml_find(st.root, "hello/world"))) {
					printf("Tag found: %s\n", e->key);
					break;
				}
			}
		}

		xml_free(st.root);

		return 0;
	}

If the XML document is already completely available, you may also use
those shortcut functions:

	#include <stdio.h>

	#include "xml.h"

	int main(void) {
		struct xml_element *root;

		if (!(root = xml_parse(
				"<hello><world>Hello World</world></hello>")) {
			char *s = xml_content_find(root, "hello/world");

			if (s) {
				printf("%s\n", s);
				free(s);
			}

			xml_free(root);
		}

		return 0;
	}

Structure
---------

The XML elements are parsed into a structure of nested linked lists.

Each XML element becomes a xml_element struct:

	struct xml_element {
		/* The tag name if this is a tag element or NULL if this
		 * element represents character data. */
		char *key;

		/* Character data segment of parent XML element or NULL if
		 * this is a tag element. "key" and "value" are exclusive
		 * because character data is treated as "nameless" child
		 * element. So each xml_element can have either a "key"
		 * or a "value" but not both. */
		char *value;

		/* Parent element. May be NULL. */
		struct xml_element *parent;

		/* First child element. May be NULL. */
		struct xml_element *first_child;

		/* Last child element. May be NULL. */
		struct xml_element *last_child;

		/* Pointer to next sibling. May be NULL. */
		struct xml_element *next;

		/* First and last attribute. Both may be NULL. */
		struct xml_attribute {
			/* Argument name */
			char *key;

			/* Argument value */
			char *value;

			/* Pointer to next argument. May be NULL. */
			struct xml_attribute *next;
		} *first_attribute, *last_attribute;
	};

Element path format
-------------------

Element paths for xml_find() and xml_find_next() are specified like file
paths. Each "directory" can also have a query-string like appendix to
identify a certain child element by evaluating its attributes, e.g.:

	hello/world?name=earth/country?name=usa&year=2013/city?name=miami

License
-------

libxml is licensed under the [MIT license][4].

[1]: http://www.w3.org/TR/REC-xml/#dt-doctype
[4]: http://opensource.org/licenses/mit-license.php
