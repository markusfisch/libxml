#ifndef _xml_h_
#define _xml_h_

struct xml_element
{
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
	struct xml_attribute
	{
		/* Argument name */
		char *key;

		/* Argument value */
		char *value;

		/* Pointer to next argument. May be NULL. */
		struct xml_attribute *next;
	} *first_attribute, *last_attribute;
};

struct xml_state
{
	/* the root element */
	struct xml_element *root;

	/* internal state variables */
	struct xml_element *current;
	struct xml_tag_pattern *tag;
	size_t length;
	size_t cursor;
	int empty;
	const char *(*parser)( struct xml_state *, const char * );
};

struct xml_element *xml_parse( const char * );
int xml_parse_chunk( struct xml_state *, const char * );
void xml_free( struct xml_element * );

struct xml_attribute *xml_find_attribute(
	struct xml_attribute *,
	const char * );

struct xml_element *xml_find( struct xml_element *, const char * );
struct xml_element *xml_find_next( struct xml_element *, const char * );

char *xml_content( struct xml_element * );
char *xml_content_find( struct xml_element *, const char * );

#endif
