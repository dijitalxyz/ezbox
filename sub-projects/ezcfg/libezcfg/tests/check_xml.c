#include <check.h>
#include "ezcfg.h"

START_TEST (test_ezcfg_xml_new)
{
	struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;
	ezcfg = ezcfg_new();
	xml = ezcfg_xml_new(ezcfg);
	fail_unless (xml != NULL, 
		"ezcfg_xml_new fail on creation");
	ezcfg_xml_delete(xml);
	ezcfg_delete(ezcfg);
}
END_TEST

int main (void)
{
	return 0;
}
