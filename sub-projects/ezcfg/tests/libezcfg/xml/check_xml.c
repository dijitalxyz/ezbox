#include <stdlib.h>
#include <check.h>
#include "ezcfg.h"
#include "ezcfg-private.h"

START_TEST(test_ezcfg_xml_new)
{
	struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;
	ezcfg = ezcfg_new(EZCFG_CONFIG_FILE_PATH);
	xml = ezcfg_xml_new(ezcfg);
	fail_if(xml == NULL, 
		"ezcfg_xml_new fail on creation");
	ezcfg_xml_delete(xml);
	ezcfg_delete(ezcfg);
}
END_TEST

static Suite * ezcfg_xml_suite(void)
{
	Suite *s = suite_create("ezcfg_xml");

	/* Core test case */
	TCase *tc_core = tcase_create("core");
	tcase_add_test(tc_core, test_ezcfg_xml_new);
	suite_add_tcase(s, tc_core);

	return s;
}

int main (void)
{
	int number_failed;
	Suite *s = ezcfg_xml_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
