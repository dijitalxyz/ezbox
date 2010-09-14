/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : libezcfg-xml.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-08-01   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <pthread.h>

#include "libezcfg.h"
#include "libezcfg-private.h"

struct elem_attribute {
	char *name;
	char *value;
	struct elem_attribute *next;
};

struct ezcfg_xml_element {
	char *name;
	struct elem_attribute *attr_head;
	struct elem_attribute *attr_tail;
	char *content;
	int etag_index; /* etag index in root stack */
};

struct ezcfg_xml {
	struct ezcfg *ezcfg;
	/* prolog part of the xml document */
	unsigned short version_major;
	unsigned short version_minor;
        unsigned short num_encoding_names; /* Number of supported encoding names */
        const char **encoding_names;
	unsigned short encoding_name_index;
	bool standalone;
	/* element part of the xml document */
	int max_elements;
	int num_elements;
	struct ezcfg_xml_element **root; /* stack for representing xml doc tree */
};

const char *default_character_encoding_names[] = {
	/* bad encoding string */
	NULL,
	/* http://www.iana.org/assignments/character-sets */
	"UTF-8",
	"UTF-16",
	"ISO-10646-UCS-2",
	"ISO-10646-UCS-4",
	"ISO-8859-1",
	"ISO-8859-2",
	"ISO-8859-3",
	"ISO-8859-4",
	"ISO-8859-5",
	"ISO-8859-6",
	"ISO-8859-7",
	"ISO-8859-8",
	"ISO-8859-9",
	"ISO-8859-10",
	"ISO-2022-JP",
	"Shift_JIS",
	"EUC-JP", 
	"GB2312",
};

/* Private functions */
static void delete_elements(struct ezcfg_xml *xml)
{
	int i;
	struct ezcfg_xml_element *p;
	for (i=0; i < xml->num_elements; i++) {
		p = xml->root[i];
		if (p == NULL)
			continue;

		/* clean etag pointer */
		if (p->etag_index > 0) {
			xml->root[p->etag_index] = NULL;
		}

		ezcfg_xml_element_delete(p);
	}
}

static bool is_white_space(char c) {
	if (c == 0x20 || c == 0x09 || c == 0x0d || c == 0x0a) {
		return true;
	}
	else {
		return false;
	}
}

static bool is_equal_sign(char c) {
	if (c == '=') {
		return true;
	}
	else {
		return false;
	}
}

/* Char ::= #x9 | #xA | #xD | [#x20-#xD7FF] | [#xE000-#xFFFD] | [#x10000-#x10FFFF] */
static bool is_char(char c) {
	if (c == 0x09 || c == 0x0d || c == 0x0a || c > 0x1f) {
		return true;
	}
	else {
		return false;
	}
}


/* FIXME! not considerate non-ASCII charactors */
/* NameStartChar ::= ":" | [A-Z] | "_" | [a-z] | [#xC0-#xD6] | [#xD8-#xF6] | [#xF8-#x2FF] | [#x370-#x37D] | [#x37F-#x1FFF] | [#x200C-#x200D] | [#x2070-#x218F] | [#x2C00-#x2FEF] | [#x3001-#xD7FF] | [#xF900-#xFDCF] | [#xFDF0-#xFFFD] | [#x10000-#xEFFFF] */
static bool is_name_start_char(char c) {
	if (isalpha(c) != 0 ||
	    c == ':' ||
	    c == '_') {
		return true;
	}
	else {
		return false;
	}
}

/* NameChar ::= NameStartChar | "-" | "." | [0-9] | #xB7 | [#x0300-#x036F] | [#x203F-#x2040] */
static bool is_name_char(char c) {
	if (is_name_start_char(c) ||
	    c == '-' ||
	    c == '.' ||
	    isdigit(c) != 0) {
		return true;
	}
	else {
		return false;
	}
}

static bool is_xml_element_start_tag(char *buf) {
	if (buf[0] == '<' && is_name_start_char(buf[1]) == true) {
		char *p = strchr(buf, '>'); /* find the start tag end part ">" */
		if (p == NULL) {
			return false;
		}
		return true;
	}
	return false;
}

static bool is_xml_element_end_tag(char *buf) {
	if (strncmp(buf, "</", 2) == 0 && is_name_start_char(buf[2]) == true) {
		char *p = strchr(buf, '>'); /* find the end tag end part ">" */
		if (p == NULL) {
			return false;
		}
		return true;
	}
	return false;
}

static bool is_xml_element_empty_element_tag(char *buf) {
	if (is_xml_element_start_tag(buf) == true) {
		char *p = strchr(buf, '>');
		if (p != NULL && *(p-1) == '/') {
			return true;
		}
	}
	return false;
}

static bool is_xml_entity_reference(char *buf) {
	if (buf[0] == '&' && is_name_start_char(buf[1]) == true) {
		return true;
	}
	return false;
}

static bool is_xml_character_reference(char *buf) {
	if (strncmp(buf, "&#", 2) == 0 && isdigit(buf[2]) != 0) {
		return true;
	}
	else if (strncmp(buf, "&#x", 3) == 0 && isxdigit(buf[3]) != 0) {
		return true;
	}
	return false;
}

static bool is_xml_cdata_section(char *buf) {
	if (strncmp(buf, "<![CDATA[", 9) == 0 && is_char(buf[9])) {
		char *p = strchr(buf, '>'); /* find the cdata section tag end part ">" */
		if (p == NULL) {
			return false;
		}
		return true;
	}
	return false;
}

static bool is_xml_declaration(char *buf) {
	if (strncmp(buf, "<?xml", 5) == 0 && is_white_space(buf[5])) {
		char *p = strchr(buf, '>'); /* find the XML declaration tag end part ">" */
		if (p == NULL) {
			return false;
		}
		return true;
	}
	return false;
}

static bool is_xml_document_type_declaration(char *buf) {
	if (strncmp(buf, "<!DOCTYPE", 9) == 0 && is_white_space(buf[9])) {
		char *p = strchr(buf, '>'); /* find the XML document type declaration tag end part ">" */
		if (p == NULL) {
			return false;
		}
		return true;
	}
	return false;
}

static bool is_xml_text_declaration(char *buf) {
	if (strncmp(buf, "<?xml", 5) == 0 && is_white_space(buf[5])) {
		char *p = strchr(buf, '>'); /* find the XML text declaration tag end part ">" */
		if (p == NULL) {
			return false;
		}
		return true;
	}
	return false;
}

static bool is_xml_comment(char *buf) {
	if (strncmp(buf, "<!--", 4) == 0) {
		char *p = strstr(buf, "-->"); /* find the XML comment tag end part "-->" */
		if (p == NULL) {
			return false;
		}
		return true;
	}
	return false;
}

static bool is_xml_processing_instruction(char *buf) {
	if (strncmp(buf, "<?", 2) == 0 && is_name_start_char(buf[2]) == true) {
		char *p = buf+2;
		if(strncasecmp(p, "xml", 3) == 0 && (is_white_space(p[3]) || p[3] == '?')) {
			return false;
		}
		p = strchr(buf, '>'); /* find the processing instruction tag end part ">" */
		if (p == NULL) {
			return false;
		}
		return true;
	}
	return false;
}

static bool is_xml_misc(char *buf) {
	if (is_xml_comment(buf) == true) {
		return true;
	}
	else if (is_xml_processing_instruction(buf) == true) {
		return true;
	}
	else if (is_white_space(*buf) == true) {
		return true;
	}
	return false;
}

/* XML 1.0 fifth edition section 2.4 */
static bool is_xml_markup(char *buf) {
	if (is_xml_element_start_tag(buf) == true) {
		return true;
	}
	else if (is_xml_element_end_tag(buf) == true) {
		return true;
	}
	else if (is_xml_element_empty_element_tag(buf) == true) {
		return true;
	}
	else if (is_xml_entity_reference(buf) == true) {
		return true;
	}
	else if (is_xml_character_reference(buf) == true) {
		return true;
	}
	else if (is_xml_comment(buf) == true) {
		return true;
	}
	else if (is_xml_cdata_section(buf) == true) {
		return true;
	}
	else if (is_xml_document_type_declaration(buf) == true) {
		return true;
	}
	else if (is_xml_processing_instruction(buf) == true) {
		return true;
	}
	else if (is_xml_declaration(buf) == true) {
		return true;
	}
	else if (is_xml_text_declaration(buf) == true) {
		return true;
	}
	return false;
}

static unsigned short find_encoding_name_index(struct ezcfg_xml *xml, const char *enc_name)
{
	struct ezcfg *ezcfg;
	int i;

	ASSERT(xml != NULL);
	ASSERT(enc_name != NULL);

	ezcfg = xml->ezcfg;

	for (i = xml->num_encoding_names; i > 0; i--) {
		if (strcasecmp(xml->encoding_names[i], enc_name) == 0)
			return i;
	}
	return 0;
}

static bool parse_xml_comment(struct ezcfg_xml *xml, char **pbuf, int *plen)
{
	struct ezcfg *ezcfg;
	char *p, *buf;
	int len;

	ezcfg = xml->ezcfg;

	buf = *pbuf;
	len = *plen;

	/* Comment ::= '<!--' ((Char - '-') | ('-' (Char - '-')))* '-->' */
	p = buf+4; /* skip "<!--" */

	/* match comment end tag */
	p = strstr(p, "-->");
	if (p == NULL) {
		return false;
	}
	p += 3; /* skip "-->" */

	*pbuf = p;
	*plen = len - (buf - p);

	return true;
}

static bool parse_xml_processing_instruction(struct ezcfg_xml *xml, char **pbuf, int *plen)
{
	struct ezcfg *ezcfg;
	char *p, *buf;
	int len;

	ezcfg = xml->ezcfg;

	buf = *pbuf;
	len = *plen;

	/* Processing Instructions */
	/* PI ::= '<?' PITarget (S (Char* - (Char* '?>' Char*)))? '?>'
	 * PITarget ::= Name - (('X' | 'x') ('M' | 'm') ('L' | 'l'))
	 */
	p = buf+2; /* skip "<?" */

	/* match PI end tag */
	p = strstr(p, "?>");
	if (p == NULL) {
		return false;
	}
	p += 2; /* skip "?>" */

	*pbuf = p;
	*plen = len - (buf - p);

	return true;
}

static bool parse_xml_document_misc(struct ezcfg_xml *xml, char **pbuf, int *plen)
{
	struct ezcfg *ezcfg;
	char *p;

	ezcfg = xml->ezcfg;

	p = *pbuf;

	/* Misc ::= Comment | PI | S */
	if (is_xml_comment(p) == true) {
		if ( parse_xml_comment(xml, pbuf, plen) == false) {
			return false;
		}
	}
	else if (is_xml_processing_instruction(p) == true) {
		if ( parse_xml_processing_instruction(xml, pbuf, plen) == false) {
			return false;
		}
	}
	else if (is_white_space(*p) == true) {
		*pbuf += 1;
		*plen -= 1;
	}

	return true;
}

static bool parse_xml_prolog_xmldecl(struct ezcfg_xml *xml, char **pbuf, int *plen)
{
	struct ezcfg *ezcfg;
	char *p, *buf;
	int len;

	ezcfg = xml->ezcfg;

	buf = *pbuf;
	len = *plen;

	/* XMLDecl */
	p = buf+5; /* skip "<?xml" */
	while (is_white_space(*p) == true) p++;
	if (strncmp(p, "version", 7) == 0) {
		/* get version info */
		p += 7; /* skip "version" */
		/* XML 1.0 fifth edition. Eq ::= S? '=' S? */
		while (is_white_space(*p) == true) p++;
		if (is_equal_sign(*p) == true) {
			p++;
			while (is_white_space(*p) == true) p++;
			/* VersionNum part, force to 1.0 if incorrect */
			if (p[0] == '1' && p[1] == '.') {
				xml->version_major = 1;
				p += 2; /* skip "1." */
				xml->version_minor = 0;
				while (isdigit(*p) != 0) {
					xml->version_minor *= 10;
					xml->version_minor += (*p - '0');
				}
			}
			while (is_white_space(*p) == true) p++;
		}
	}
	if (strncmp(p, "encoding", 8) == 0) {
		/* get encoding info */
		p += 8; /* skip "encoding" */
		/* XML 1.0 fifth edition. Eq ::= S? '=' S? */
		while (is_white_space(*p) == true) p++;
		if (is_equal_sign(*p) == true) {
			p++;
			while (is_white_space(*p) == true) p++;
			/* EncName part, force to UTF-8 if incorrect */
			if (p[0] == '\"' || p[0] == '\'') {
				char str_tag;
				char *enc_name;

				str_tag = p[0];
				p++;
				enc_name = p;
				while (*p != str_tag && p < buf+len) p++;
				if (*p == str_tag) {
					*p = '\0';
					xml->encoding_name_index = find_encoding_name_index(xml, enc_name);
					*p = str_tag;
				}
			}
			while (is_white_space(*p) == true) p++;
		}
	}
	if (strncmp(p, "standalone", 10) == 0) {
		/* get standalone info */
		p += 10; /* skip "standalone" */
		/* XML 1.0 fifth edition. Eq ::= S? '=' S? */
		while (is_white_space(*p) == true) p++;
		if (is_equal_sign(*p) == true) {
			p++;
			while (is_white_space(*p) == true) p++;
			/* EncName part, force to UTF-8 if incorrect */
			if (p[0] == '\"' || p[0] == '\'') {
				char str_tag;
				char *bool_str;

				str_tag = p[0];
				p++;
				bool_str = p;
				while (*p != str_tag && p < buf+len) p++;
				if (*p == str_tag) {
					*p = '\0';
					if (strcmp(bool_str, "yes") == 0) {
						xml->standalone = true;
					}
					*p = str_tag;
				}
			}
			while (is_white_space(*p) == true) p++;
		}
	}

	/* match XMLDecl end tag */
	p = strstr(p, "?>");
	if (p == NULL) {
		return false;
	}
	p += 2; /* skip "?>" */

	*pbuf = p;
	*plen = len - (buf - p);

	return true;
}

static bool parse_xml_prolog_doctypedecl(struct ezcfg_xml *xml, char **pbuf, int *plen)
{
	struct ezcfg *ezcfg;
	char *p, *buf;
	int len;

	ezcfg = xml->ezcfg;

	buf = *pbuf;
	len = *plen;

	/* doctypedecl */
	p = buf+9; /* skip "<!DOCTYPE" */
	while (is_white_space(*p) == true) p++;

	/* match doctypedecl end tag */
	p = strstr(p, ">");
	if (p == NULL) {
		return false;
	}
	p += 1; /* skip ">" */

	*pbuf = p;
	*plen = len - (buf - p);

	return true;
}

static bool parse_xml_document_prolog(struct ezcfg_xml *xml, char **pbuf, int *plen)
{
	struct ezcfg *ezcfg;

	ASSERT(xml != NULL);
	ASSERT(pbuf != NULL);
	ASSERT(plen != NULL);
	ASSERT(*pbuf != NULL);
	ASSERT(*plen > 0);

	ezcfg = xml->ezcfg;

	/* prolog ::= XMLDecl? Misc* (doctypedecl Misc*)? */ 

	/* check prolog XMLDecl? */
	if (is_xml_declaration(*pbuf) == true) {
		if (parse_xml_prolog_xmldecl(xml, pbuf, plen) == false) {
			return false;
		}
	}

	/* check prolog Misc* part */
	while(is_xml_misc(*pbuf) == true) {
		if (parse_xml_document_misc(xml, pbuf, plen) == false) {
			return false;
		}
	}

	/* check prolog (doctypedecl Misc*)? */
	if (is_xml_document_type_declaration(*pbuf) == true) {
		if (parse_xml_prolog_doctypedecl(xml, pbuf, plen) == false) {
			return false;
		}

		while(is_xml_misc(*pbuf) == true) {
			if (parse_xml_document_misc(xml, pbuf, plen) == false) {
				return false;
			}
		}
	}

	return true;
}

/* return element index in xml->root */
static int parse_element_empty_elem_tag(
	struct ezcfg_xml *xml,
	char **pbuf,
	int *plen,
	const int pi,
	const int si) {

	struct ezcfg *ezcfg;
	char *p, *name, *value;
	char *start_tag_end;
	struct ezcfg_xml_element *elem;
	char c;
	char *buf;
	int len;

	ASSERT(xml != NULL);
	ASSERT(pbuf != NULL);
	ASSERT(plen != NULL);
	ASSERT(*pbuf != NULL);
	ASSERT(*plen > 0);

	ezcfg = xml->ezcfg;

	buf = *pbuf;
	len = *plen;

	/* It's an EmptyElemTag */
	/* EmptyElemTag ::= '<' Name (S Attribute)* S? '/>' */

	/* Name part */
	name = *pbuf+1; /* skip '<' */
	start_tag_end = strstr(name, "/>");
	*start_tag_end = '\0';

	p = name+1;
	while(is_name_char(*p) == true && p < start_tag_end) p++;
	c = *p;
	*p = '\0';
	elem = ezcfg_xml_element_new(xml, name, NULL);
	if (elem == NULL) {
		err(ezcfg, "Cannot initialize xml root element\n");
		return -1;
	}

	*p = c;

	while(p < start_tag_end) {
		/* skip S */
		while(is_white_space(*p) == true) p++;

		/* Attribute part */
		/* Attribute ::= Name Eq AttValue */
		if (is_name_start_char(*p) == true) {
			name = p;
			p++;
			while(is_name_char(*p) == true) p++;

			if (is_white_space(*p) == true) { *p = '\0'; p++; }

			if (is_equal_sign(*p)) { *p = '\0'; p++; }
			else { return -1; }

			if (is_white_space(*p) == true) { *p = '\0'; p++; }

			value = p;

			if (*value == '\"') {
				value++;
				p = strchr(value, '\"');
				if (p == NULL) {
					return -1;
				}
				*p = '\0';
				p++;
				if (ezcfg_xml_element_add_attribute(xml, elem, name, value, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL) == false) {
					return -1;
				}
			}
			else { return -1; }
		}
	}
	p += 2; /* skip "/>" */
	*pbuf = p;
	*plen = len - (p - buf);

	return ezcfg_xml_add_element(xml, pi, si, elem);
}

static int parse_element_stag(
	struct ezcfg_xml *xml,
	char **pbuf,
	int *plen,
	const int pi,
	const int si) {

	struct ezcfg *ezcfg;
	char *p, *name, *value;
	char *start_tag_end;
	struct ezcfg_xml_element *elem;
	char c;
	char *buf;
	int len;

	ASSERT(xml != NULL);
	ASSERT(pbuf != NULL);
	ASSERT(plen != NULL);
	ASSERT(*pbuf != NULL);
	ASSERT(*plen > 0);

	ezcfg = xml->ezcfg;

	buf = *pbuf;
	len = *plen;

	/* STag ::= '<' Name (S Attribute)* S? '>' */

	/* Name part */
	name = *pbuf+1; /* skip '<' */
	start_tag_end = strchr(name, '>');
	*start_tag_end = '\0';

	p = name+1;
	while(is_name_char(*p) == true && p < start_tag_end) p++;
	c = *p;
	*p = '\0';
	elem = ezcfg_xml_element_new(xml, name, NULL);
	if (elem == NULL) {
		err(ezcfg, "Cannot initialize xml root element\n");
		return -1;
	}

	*p = c;

	while(p < start_tag_end) {
		/* skip S */
		while(is_white_space(*p) == true) p++;

		/* Attribute part */
		/* Attribute ::= Name Eq AttValue */
		if (is_name_start_char(*p) == true) {
			name = p;
			p++;
			while(is_name_char(*p) == true) p++;

			if (is_white_space(*p) == true) { *p = '\0'; p++; }

			if (is_equal_sign(*p)) { *p = '\0'; p++; }
			else { return -1; }

			if (is_white_space(*p) == true) { *p = '\0'; p++; }

			value = p;

			if (*value == '\"') {
				value++;
				p = strchr(value, '\"');
				if (p == NULL) {
					return -1;
				}
				*p = '\0';
				p++;
				if (ezcfg_xml_element_add_attribute(xml, elem, name, value, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL) == false) {
					return -1;
				}
			}
			else { return -1; }
		}
	}
	p++; /* skip '>' */
	*pbuf = p;
	*plen = len - (p - buf);

	return ezcfg_xml_add_element(xml, pi, si, elem);
}

static int parse_element_content(
	struct ezcfg_xml *xml,
	char **pbuf,
	int *plen,
	const int pi,
	const int si) {

	struct ezcfg *ezcfg;

	ASSERT(xml != NULL);

	ezcfg = xml->ezcfg;

#if 0

	/* content ::= CharData? ((element | Reference | CDSect | PI | Comment) CharData?)* */
	if (is_xml_markup(*pbuf) == false) {
		if (parse_xml_char_data(xml, pbuf, plen, pi) == false) {
			return -1;
		}
	}

	while(is_xml_element(*pbuf) == true ||
	      is_xml_reference(*pbuf) == true ||
	      is_xml_cdata_section(*pbuf) == true ||
	      is_xml_processing_instruction(*pbuf) == true ||
	      is_xml_comment(*pbuf) == true) {

		if (is_xml_element(*pbuf) == true) {
			if (parse_xml_element(xml, pbuf, plen, pi, si) == -1) {
				return -1;
			}
		}
		else if (is_xml_reference(*pbuf) == true) {
			if (parse_xml_reference(xml, pbuf, plen) == false) {
				return -1;
			}
		}
		else if (is_xml_cdata_section(*pbuf) == true) {
			if (parse_xml_cdata_section(xml, pbuf, plen) == false) {
				return -1;
			}
		}
		else if (is_xml_processing_instruction(*pbuf) == true) {
			if (parse_xml_process_instruction(xml, pbuf, plen) == false) {
				return -1;
			}
		}
		else if (is_xml_comment(*pbuf) == true) {
			if (parse_xml_comment(xml, pbuf, plen) == false) {
				return -1;
			}
		}

		if (is_xml_markup(*pbuf) == false) {
			if (parse_xml_char_data(xml, pbuf, plen, pi) == -1) {
				return -1;
			}
		}
	}

#endif

	return pi;
}

static int parse_element_etag(
	struct ezcfg_xml *xml,
	char **pbuf,
	int *plen,
	const int ei) {

	struct ezcfg *ezcfg;
	char *p, *name;
	char *end_tag_end;
	struct ezcfg_xml_element *elem;
	char c;
	char *buf;
	int len;

	ASSERT(xml != NULL);
	ASSERT(pbuf != NULL);
	ASSERT(plen != NULL);
	ASSERT(*pbuf != NULL);
	ASSERT(*plen > 0);


	ASSERT(xml != NULL);

	ezcfg = xml->ezcfg;

	buf = *pbuf;
	len = *plen;
	elem = ezcfg_xml_get_element_by_index(xml, ei);

	/* ETag ::= '</' Name S? '>' */

	/* Name part */
	name = *pbuf+2; /* skip '</' */
	end_tag_end = strchr(name, '>');
	*end_tag_end = '\0';

	p = name+1;
	while(is_name_char(*p) == true && p < end_tag_end) p++;
	c = *p;
	*p = '\0';

	if (strcmp(elem->name, name) != 0) {
		*p = c;
		return -1;
	}

	*p = c;

	p++; /* skip '>' */
	*pbuf = p;
	*plen = len - (p - buf);

	return elem->etag_index;
}

static int parse_element_stag_content_etag(
	struct ezcfg_xml *xml,
	char **pbuf,
	int *plen,
	const int pi,
	const int si) {

	struct ezcfg *ezcfg;
	int stag_index;

	ASSERT(xml != NULL);
	ASSERT(pbuf != NULL);
	ASSERT(plen != NULL);
	ASSERT(*pbuf != NULL);
	ASSERT(*plen > 0);

	ezcfg = xml->ezcfg;

	/* It's element ::= STag content ETag */
	stag_index = parse_element_stag(xml, pbuf, plen, pi, si);
	if (stag_index == -1) {
		return -1;
	}

	if (parse_element_content(xml, pbuf, plen, stag_index, -1) == -1) {
		return -1;
	}

	if (parse_element_etag(xml, pbuf, plen, stag_index) == -1) {
		return -1;
	}

	return stag_index;
}

static int parse_xml_document_element(
	struct ezcfg_xml *xml,
	char **pbuf,
	int *plen,
	const int pi,
	const int si) {

	struct ezcfg *ezcfg;

	ASSERT(xml != NULL);
	ASSERT(pbuf != NULL);
	ASSERT(plen != NULL);
	ASSERT(*pbuf != NULL);
	ASSERT(*plen > 0);

	ezcfg = xml->ezcfg;

	/* element ::= EmptyElemTag | STag content ETag */

	/* check element EmptyElemTag |STag start part */
	if (is_xml_element_start_tag(*pbuf) == true) {
		if (is_xml_element_empty_element_tag(*pbuf) == true) {
			/* It's an EmptyElemTag */
			/* EmptyElemTag ::= '<' Name (S Attribute)* S? '/>' */
			return parse_element_empty_elem_tag(xml, pbuf, plen, pi, si);
		}

		/* It's an STag */
		/* element ::= STag content ETag */
		return parse_element_stag_content_etag(xml, pbuf, plen, pi, si);
	}

	return -1;
}

/* Public functions */
void ezcfg_xml_delete(struct ezcfg_xml *xml)
{
	struct ezcfg *ezcfg;

	ASSERT(xml != NULL);

	ezcfg = xml->ezcfg;

	if (xml->root != NULL)
		delete_elements(xml);

	free(xml);
}

/**
 * ezcfg_xml_new:
 * Create ezcfg xml parser data structure
 * Returns: a new ezcfg xml parser data structure
 **/
struct ezcfg_xml *ezcfg_xml_new(struct ezcfg *ezcfg)
{
	struct ezcfg_xml *xml;

	ASSERT(ezcfg != NULL);

	/* initialize xml parser data structure */
	xml = calloc(1, sizeof(struct ezcfg_xml));
	if (xml == NULL) {
		err(ezcfg, "initialize xml parser error.\n");
		return NULL;
	}

	memset(xml, 0, sizeof(struct ezcfg_xml));

	xml->max_elements = EZCFG_XML_MAX_ELEMENTS * 2; /* must be times of 2 */
	xml->root=calloc(xml->max_elements, sizeof(struct ezcfg_xml_element *));
	if (xml->root == NULL) {
		err(ezcfg, "initialize xml element stack error.\n");
		free(xml);
		return NULL;
	}

	memset(xml->root, 0, sizeof(struct ezcfg_xml_element *) * xml->max_elements);

	xml->ezcfg = ezcfg;
	xml->num_encoding_names = ARRAY_SIZE(default_character_encoding_names) - 1; /* first item is NULL */
	xml->encoding_names = default_character_encoding_names;

	return xml;
}

void ezcfg_xml_element_delete(struct ezcfg_xml_element *elem)
{
	struct elem_attribute *a;
	ASSERT(elem != NULL);
	if (elem->name)
		free(elem->name);
	while (elem->attr_head != NULL) {
		a = elem->attr_head;
		elem->attr_head = a->next;
		if (a->name != NULL)
			free(a->name);
		if (a->value != NULL)
			free(a->value);
	}
	if (elem->content)
		free(elem->content);
	free(elem);
}

struct ezcfg_xml_element *ezcfg_xml_element_new(
	struct ezcfg_xml *xml,
	const char *name, const char *content) {

	struct ezcfg *ezcfg;
	struct ezcfg_xml_element *elem;

	ASSERT(xml != NULL);
	ASSERT(name != NULL);

	ezcfg = xml->ezcfg;

	elem = calloc(1, sizeof(struct ezcfg_xml_element));
	if (elem == NULL) {
		err(ezcfg, "calloc xml element error.\n");
		return NULL;
	}

	memset(elem, 0, sizeof(struct ezcfg_xml_element));

	elem->name = strdup(name);
	if (elem->name == NULL) {
		err(ezcfg, "initialize xml element name error.\n");
		ezcfg_xml_element_delete(elem);
		return NULL;
	}
	if (content != NULL) {
		elem->content = strdup(content);
		if (elem->content == NULL) {
			err(ezcfg, "initialize xml element content error.\n");
			ezcfg_xml_element_delete(elem);
			return NULL;
		}
	}
	return elem;
}

bool ezcfg_xml_element_add_attribute(
	struct ezcfg_xml *xml,
	struct ezcfg_xml_element *elem,
	const char *name, const char *value, int pos) {

	struct ezcfg *ezcfg;
	struct elem_attribute *a;

	ASSERT(xml != NULL);
	ASSERT(elem != NULL);
	ASSERT(name != NULL);
	ASSERT(value != NULL);

	ezcfg = xml->ezcfg;

	a = calloc(1, sizeof(struct elem_attribute));
	if (a == NULL) {
		err(ezcfg, "no memory for element attribute\n");
		return false;
	}

	memset(a, 0, sizeof(struct elem_attribute));

	a->name = strdup(name);
	if (a->name == NULL) {
		err(ezcfg, "no memory for element attribute name\n");
		free(a);
		return false;
	}

	a->value = strdup(value);
	if (a->value == NULL) {
		err(ezcfg, "no memory for element attribute value\n");
		free(a->name);
		free(a);
		return false;
	}

	if (pos == EZCFG_XML_ELEMENT_ATTRIBUTE_HEAD) {
		a->next = elem->attr_head;
		elem->attr_head = a;
		if (elem->attr_tail == NULL) {
			elem->attr_tail = a;
		}
	} else if (pos == EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL) {
		if (elem->attr_tail == NULL) {
			elem->attr_head = a;
			elem->attr_tail = a;
		} else {
			a->next = elem->attr_tail->next;
			elem->attr_tail->next = a;
			elem->attr_tail = a;
		}
	} else {
		err(ezcfg, "not support element attribute position\n");
		free(a->name);
		free(a->value);
		free(a);
		return false;
	}
	return true;
}

int ezcfg_xml_add_element(
	struct ezcfg_xml *xml,
	const int pi, /* parent element index */
	const int si, /* sibling element index */
	struct ezcfg_xml_element *elem) {

	struct ezcfg *ezcfg;
	struct ezcfg_xml_element **root;
	struct ezcfg_xml_element *parent;
	struct ezcfg_xml_element *sibling;
	int i;

	ASSERT(xml != NULL);
	ASSERT(xml->num_elements+2 <= xml->max_elements);

	ezcfg = xml->ezcfg;
	root = xml->root;
	i = 0;

	if (pi < 0) {
		parent = NULL;
	}
	else {
		parent = root[pi];
	}

	if (si < 0) {
		sibling = NULL;
	}
	else {
		sibling = root[pi];
	}

	if (parent == NULL) {
		/* root element */
		if(xml->num_elements != 0) {
			err(ezcfg, "xml root element must be only one, xml->num_elements=[%d]\n", xml->num_elements);
			return -1;
		}
		i = 0;
		root[i] = elem;
		xml->num_elements++;
		elem->etag_index = i+1;
		root[i+1] = elem;
		xml->num_elements++;
	} else if (sibling == NULL) {
		/* parent element's first child */
		for (i = xml->num_elements-1; i > 0; i--) {
			/* move 2 steps */
			root[i+2] = root[i];

			/* change etag_index for the start element */
			if (root[i]->etag_index == i) {
				root[i]->etag_index = i+2;
			}
			/* find parent element end tag */
			if (root[i] == parent) {
				break;
			}
		}
		if(i == 0) {
			err(ezcfg, "parent element not found\n");
			return -1;
		}
		root[i] = elem;
		xml->num_elements++;
		elem->etag_index = i+1;
		root[i+1] = elem;
		xml->num_elements++;
	} else {
		/* sibling element's next */
		for (i = xml->num_elements-1; i > 0; i--) {
			/* find sibling element end tag */
			if (root[i] == sibling) {
				break;
			}
			/* move 2 steps */
			root[i+2] = root[i];

			/* change etag_index for the start element */
			if (root[i]->etag_index == i) {
				root[i]->etag_index = i+2;
			}
		}
		if(i == 0) {
			err(ezcfg, "sibling element not found\n");
			return -1;
		}
		i++; /* move to next element slot */
		root[i] = elem;
		xml->num_elements++;
		elem->etag_index = i+1;
		root[i+1] = elem;
		xml->num_elements++;
	}
	return i;
}

/* XML 1.0 fifth edition section 2.11 End-of-Line Handling */
int ezcfg_xml_normalize_document(struct ezcfg_xml *xml, char *buf, int len)
{
	struct ezcfg *ezcfg;
	char *p, *q;

	ezcfg = xml->ezcfg;

	p = buf;
	q = buf;

	while(q < buf + len) {
		if (q[0] == 0x0d && q[1] == 0x0a) {
			q++;
			*p = *q;
		}
		else if (q[0] == 0x0d && q[1] != 0x0a) {
			*p = 0x0a;
		}
		else {
			*p = *q;
		}
		p++;
		q++;
	}

	/* \0-terminated the document */
	*p = '\0';

	return (p - buf);
}

bool ezcfg_xml_parse(struct ezcfg_xml *xml, char *buf, int len)
{
	struct ezcfg *ezcfg;
	struct ezcfg_xml_element **root;
	char *p;
	int i;

	ASSERT(xml != NULL);
	ASSERT(xml->root != NULL);
	ASSERT(buf != NULL);
	ASSERT(len > 0);

	ezcfg = xml->ezcfg;
	root = xml->root;

	/* clean up xml->root elements */
	delete_elements(xml);

	p = buf;
	i = len;

	/* normalize XML document */
	i = ezcfg_xml_normalize_document(xml, buf, len);

	/* XML document ::= prolog element Misc* */
	if (parse_xml_document_prolog(xml, &p, &i) == false) {
		return false;
	}

	/* parent = NULL, it's a root element */
	if (parse_xml_document_element(xml, &p, &i, -1, -1) == -1) {
		return false;
	}

	while(is_xml_misc(p) == true) {
		if (parse_xml_document_misc(xml, &p, &i) == false) {
			return false;
		}
	}

	return true;
}

int ezcfg_xml_write(struct ezcfg_xml *xml, char *buf, int len)
{
	struct ezcfg *ezcfg;
	struct ezcfg_xml_element **root;
	struct elem_attribute *a;
	int i;

	ASSERT(xml != NULL);
	ASSERT(xml->root != NULL);
	ASSERT(buf != NULL);
	ASSERT(len > 0);

	ezcfg = xml->ezcfg;
	root = xml->root;
	buf[0] = '\0';

	for (i = 0; i < xml->num_elements; i++) {
		if (root[i]->etag_index != i) {
			/* start tag */
			snprintf(buf+strlen(buf), len-strlen(buf), "<%s", root[i]->name);
			a = root[i]->attr_head;
			while(a != NULL) {
				snprintf(buf+strlen(buf), len-strlen(buf), " %s", a->name);
				snprintf(buf+strlen(buf), len-strlen(buf), "=\"%s\"", a->value);
				a = a->next;
			}
			if (root[i]->content == NULL && root[i]->etag_index == i+1) {
				snprintf(buf+strlen(buf), len-strlen(buf), "/>\n");
			}
			else {
				snprintf(buf+strlen(buf), len-strlen(buf), ">");
				if (root[i]->content != NULL) {
					snprintf(buf+strlen(buf), len-strlen(buf), "%s", root[i]->content);
				} else {
					snprintf(buf+strlen(buf), len-strlen(buf), "\n");
				}
			}
		}
		else {
			/* end tag */
			snprintf(buf+strlen(buf), len-strlen(buf), "</%s>\n", root[i]->name);
		}
	}
	return strlen(buf);
}

int ezcfg_xml_get_element_index(struct ezcfg_xml *xml, const int pi, const char *name)
{
	struct ezcfg *ezcfg;
	struct ezcfg_xml_element *elem;
	int i;

	ASSERT(xml != NULL);
	ASSERT(xml->root != NULL);
	ASSERT(pi < xml->num_elements);

	ezcfg = xml->ezcfg;
	for (i = pi; i < xml->num_elements; i++) {
		elem = xml->root[i];
		if (strcmp(elem->name, name) == 0 && (elem->etag_index > 0))
			return i;
	}
	return -1;
}

struct ezcfg_xml_element *ezcfg_xml_get_element_by_index(struct ezcfg_xml *xml, const int i)
{
	struct ezcfg *ezcfg;

	ASSERT(xml != NULL);
	ASSERT(xml->root != NULL);
	ASSERT(i < xml->num_elements);

	ezcfg = xml->ezcfg;

	return xml->root[i];
}

bool ezcfg_xml_set_element_content_by_index(struct ezcfg_xml *xml, const int i, const char *content)
{
	struct ezcfg *ezcfg;
	struct ezcfg_xml_element *elem;
	char *p;

	ASSERT(xml != NULL);
	ASSERT(xml->root != NULL);
	ASSERT(i < xml->num_elements);

	ezcfg = xml->ezcfg;

	elem = xml->root[i];
	p = strdup(content);
	if (p == NULL) {
		return false;
	}
	if (elem->content != NULL) {
		free(elem->content);
	}
	elem->content = p;
	return true;
}

char *ezcfg_xml_get_element_content_by_index(struct ezcfg_xml *xml, const int i)
{
	struct ezcfg *ezcfg;
	struct ezcfg_xml_element *elem;

	ASSERT(xml != NULL);
	ASSERT(xml->root != NULL);
	ASSERT(i < xml->num_elements);

	ezcfg = xml->ezcfg;

	elem = xml->root[i];
	return elem->content;
}
