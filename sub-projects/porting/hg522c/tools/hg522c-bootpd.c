#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void usage(void)
{
	printf("hg522c-bootpd [fw-file] [gw-mac-address]\n");
	printf("  [fw-file] : firmware file to be uploaded\n");
	printf("  [gw-mac-address] : HG522-C gateway MAC address in boot\n");
}

int upload(char *fw, char *mac)
{
	int rc = 0;
	FILE *fp = NULL, *fp_fw = NULL;
	int sock = -1;
	struct sockaddr_in sa;
	unsigned short http_port = 80;
	int recvbytes, readbytes, i, n;
	char buf[8192];
	char *data = NULL;
	size_t data_len;
	char *boundary = "hg522cc225gh";
	char *msg = NULL;
	size_t msg_len;
	char *p = NULL;

	/* get upload webpage */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		printf("socket error\n");
		goto exit;
	}

	memset((void *)&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(http_port);
	sa.sin_addr.s_addr = inet_addr(mac);

	if (connect(sock, (struct sockaddr *)&sa, sizeof(struct sockaddr_in)) < 0) {
		printf("connect error\n");
		goto exit;
	}

	snprintf(buf, sizeof(buf), "GET /upload.html HTTP/1.1\r\n");
	snprintf(buf+strlen(buf), sizeof(buf)-strlen(buf), "Host: 192.168.1.1\r\n");
	snprintf(buf+strlen(buf), sizeof(buf)-strlen(buf), "\r\n");
	if (send(sock, buf, strlen(buf), 0) < 0) {
		printf("send get error\n");
		goto exit;
	}

	memset(buf, 0, sizeof(buf));
	recvbytes = recv(sock, buf, sizeof(buf), 0);
	if (recvbytes < 0) {
		printf("recv error\n");
		goto exit;
	}
	i = 0;
	while (recvbytes > 0) {
		i += recvbytes;
		recvbytes = recv(sock, buf+i, sizeof(buf)-i, 0);
	}

	/* save the received info */
	fp = fopen("upload.html", "w");
	if (fp == NULL) {
		printf("can not open upload.html\n");
		goto exit;
	}
	recvbytes = fwrite(buf, sizeof(char), i, fp);
	if (recvbytes != i) {
		printf("write to upload.html error\n");
		goto exit;
	}
	fclose(fp);
	fp = NULL;

	/* open firmware file */
	fp_fw = fopen(fw, "r");
	if (fp_fw == NULL) {
		printf("can not open file [%s]\n", fw);
		rc = -1;
		goto exit;
	}

	/* close socket */
	close(sock);
	sock = -1;

	/* get firmware data from file */
	data_len = 8*1024*1024;
	data = (char *)malloc(data_len);
	if (data == NULL) {
		printf("cannot malloc data\n");
		goto exit;
	}
	memset(data, 0, data_len);

	readbytes = fread(data, sizeof(char), data_len, fp_fw);

	/* construct http message body */
	msg_len = 8*1024*1024;
	msg = (char *)malloc(msg_len);
	if (msg == NULL) {
		printf("cannot malloc msg\n");
		goto exit;
	}
	memset(msg, 0, msg_len);

	p = msg; i = 0;
	n = snprintf(p, msg_len-i, "--%s\r\n", boundary);
	p += n; i += n;

	n = snprintf(p, msg_len-i, "Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\n", "filename", fw);
	p += n; i += n;

	n = snprintf(p, msg_len-i, "Content-Type: application/octet-stream\r\n\r\n");
	p += n; i += n;

	/* copy data to message body */
	n = readbytes;
	memcpy(p, data, n);
	p += n; i += n;

	/* send finish boundary */
	n = snprintf(p, msg_len-i, "\r\n--%s--\r\n", boundary);
	p += n; i += n;

	/* get upload webpage */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		printf("post socket error\n");
		goto exit;
	}

	memset((void *)&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(http_port);
	sa.sin_addr.s_addr = inet_addr(mac);

	if (connect(sock, (struct sockaddr *)&sa, sizeof(struct sockaddr_in)) < 0) {
		printf("post connect error\n");
		goto exit;
	}

	sleep(1);

	memset(data, 0, data_len);

	p = data;
	n = snprintf(p, data_len-(p-data), "POST /upload.html HTTP/1.1\r\n");
	p += n;
	n = snprintf(p, data_len-(p-data), "Host: 192.168.1.1\r\n");
	p += n;
	n = snprintf(p, data_len-(p-data), "Content-Type: multipart/form-data; boundary=%s\r\n", boundary);
	p += n;
	n = snprintf(p, data_len-(p-data), "Content-Length: %d\r\n\r\n", i);
	p += n;

	/* copy message body */
	n = i;
	memcpy(p, msg, n);
	p += n;
	n = p-data;

	if (send(sock, data, (p-data), 0) < 0) {
		printf("send post error\n");
		goto exit;
	}

exit:
	if (fp != NULL) {
		fclose(fp);
	}
	if (fp_fw != NULL) {
		fclose(fp_fw);
	}
	if (sock >= 0) {
		close(sock);
	}
	if (data != NULL) {
		free(data);
	}
	if (msg != NULL) {
		free(msg);
	}
	return rc;
}

int main(int argc, char **argv)
{
	char *fw_file, *mac_addr;
	if (argc != 3) {
		usage();
		return 1;
	}

	fw_file = argv[1];
	mac_addr = argv[2];
	return upload(fw_file, mac_addr);
}
