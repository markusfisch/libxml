#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <xml.h>

struct search {
	struct search *next;
	char *pattern;
};

/**
 * Dump arguments of given XML element
 *
 * @param e - XML element
 */
void dump_arguments(struct xml_element *e) {
	struct xml_attribute *a = e->first_attribute;

	for (; a; a = a->next) {
		printf(" %s=\"%s\"", a->key, a->value);
	}
}

/**
 * Dump XML
 *
 * @param e - XML element
 */
void dump_xml(struct xml_element *e) {
	if (!e) {
		return;
	}

	if (!e->value) {
		struct xml_element *c;

		if (e->key) {
			printf("<%s", e->key);
			dump_arguments(e);

			if (*e->key == '?' || *e->key == '!') {
				printf(">");
				return;
			}

			if (!e->first_child) {
				printf("/>");
				return;
			}

			printf(">");
		}

		for (c = e->first_child; c; c = c->next) {
			dump_xml(c);
		}

		if (e->key) {
			printf("</%s>", e->key);
		}
	} else {
		printf("%s", e->value);
	}
}

/**
 * Dump XML as string
 *
 * @param e - XML element
 */
void dump_string(struct xml_element *e) {
	char *s = xml_content(e);

	if (s) {
		printf("%s\n", s);
		free(s);
	}
}

/**
 * Dump only matching elements
 *
 * @param e - root element
 * @param s - search elements
 * @param dump - dump function
 */
void dump_matching(
		struct xml_element *root,
		struct search *s,
		void (*dump)(struct xml_element *)) {
	struct xml_element *e;

	for (; s; s = s->next) {
		for (e = xml_find(root, s->pattern);
				e;
				e = xml_find_next(e, s->pattern)) {
			dump(e);
		}
	}
}

/**
 * Parse XML data
 *
 * @param d - XML string or file name or URL
 * @param s - search patterns (may be NULL)
 * @param dump - dump function
 */
int parse(
		const char *d,
		struct search *s,
		void (*dump)(struct xml_element *)) {
	struct xml_state st;

	memset(&st, 0, sizeof(st));

	if (*d == '<') {
		if (xml_parse_chunk(&st, d)) {
			xml_free(st.root);

			perror("xml_parse");
			return -1;
		}
	} else {
		int fd;
		int bytes;
		char buf[96];
		int len = sizeof(buf) - 1;

		if ((fd = open(d, O_RDONLY)) < 0) {
			perror("open");
			return -1;
		}

		while ((bytes = read(fd, buf, len)) > 0) {
			/* terminate input string */
			buf[bytes] = 0;

			if (xml_parse_chunk(&st, buf)) {
				xml_free(st.root);
				close(fd);

				perror("xml_parse");
				return -1;
			}
		}

		close(fd);
	}

	if (!st.root) {
		fprintf(stderr, "error: malformed XML document");
		return -1;
	}

	if (s) {
		dump_matching(st.root, s, dump);
	} else {
		dump(st.root);
	}

	xml_free(st.root);

	return 0;
}

/**
 * Add another search to list
 *
 * @param sibling - existing search item (may be NULL)
 * @param pattern - search pattern
 */
struct search *search_add(
	struct search *sibling,
	char *pattern) {
	struct search *s = malloc(sizeof(struct search));

	if (!s) {
		return NULL;
	}

	s->pattern = pattern;
	s->next = sibling;

	return s;
}

/**
 * Free search list
 *
 * @param s - first search item
 */
void search_free(struct search *s) {
	struct search *n;

	for (; s; s = n) {
		n = s->next;
		free(s);
	}
}

/**
 * Process command line arguments
 *
 * @param argc - number of arguments
 * @param argv - XML string or file name or URL
 */
int main(int argc, char **argv) {
	struct search *s = NULL;
	void *d = dump_xml;

	while (--argc && ++argv)
		if (**argv == '?') {
			s = search_add(s, *argv + 1);
		} else if (**argv == '-') {
			d = dump_string;
		} else if (**argv == '=') {
			d = dump_arguments;
		} else {
			parse(*argv, s, d);
		}

	search_free(s);

	return 0;
}
