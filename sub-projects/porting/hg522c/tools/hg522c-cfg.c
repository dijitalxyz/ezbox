#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void usage(void)
{
	printf("hg522c-cfg [option] [in-file] [out-file]\n");
	printf("  option: -c compress the config file\n");
	printf("  option: -d decompress the config file\n");
}

int compress(char *in, char *out)
{
	int rc = 0;
	int c;
	FILE *fp_in = NULL, *fp_out = NULL;
	fp_in = fopen(in, "r");
	if (fp_in == NULL) {
		printf("can not open file [%s]\n", in);
		rc = -1;
		goto exit;
	}
	fp_out = fopen(out, "w");
	if (fp_out == NULL) {
		printf("can not open file [%s]\n", out);
		rc = -1;
		goto exit;
	}
	c = fgetc(fp_in);
	while (c != EOF) {
		if ( c*2 > 127) {
			c *= 2;
			c -= 127;
		}
		else {
			c *= 2;
		}
		fputc(c, fp_out);
		c = fgetc(fp_in);
	}
exit:
	if (fp_in != NULL) {
		fclose(fp_in);
	}
	if (fp_out != NULL) {
		fclose(fp_out);
	}
	return rc;
}

int decompress(char *in, char *out)
{
	int rc = 0;
	int c;
	FILE *fp_in = NULL, *fp_out = NULL;
	fp_in = fopen(in, "r");
	if (fp_in == NULL) {
		printf("can not open file [%s]\n", in);
		rc = -1;
		goto exit;
	}
	fp_out = fopen(out, "w");
	if (fp_out == NULL) {
		printf("can not open file [%s]\n", out);
		rc = -1;
		goto exit;
	}
	c = fgetc(fp_in);
	while (c != EOF) {
		if ( c%2 != 0) {
			c += 127;
			c /= 2;
		}
		else {
			c /= 2;
		}
		fputc(c, fp_out);
		c = fgetc(fp_in);
	}
exit:
	if (fp_in != NULL) {
		fclose(fp_in);
	}
	if (fp_out != NULL) {
		fclose(fp_out);
	}
	return rc;
}

int main(int argc, char **argv)
{
	char *in_file, *out_file;
	int flags, opt;
	if (argc != 4) {
		usage();
		return 1;
	}

	while ((opt = getopt(argc, argv, "c:d:")) != -1)
	{
		switch (opt) {
		case 'c':
			flags = 1;
			break;
		case 'd':
			flags = 0;
			break;
		default:
			usage();
			return 1;
		}
	}

	in_file = argv[2];
	out_file = argv[3];

	if (flags == 0) {
		decompress(in_file, out_file);
	}
	else {
		compress(in_file, out_file);
	}

	return 0;
}
