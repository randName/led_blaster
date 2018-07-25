#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

long load_file(const char * path, char ** buf) {
	static long off_end;
	static int rc;
	static size_t fsz;
	FILE *fp;

	fp = fopen(path, "rb");
	if ( NULL == fp ) {
		return -1L;
	}

	rc = fseek(fp, 0L, SEEK_END);
	if ( 0 != rc ) {
		return -1L;
	}

	if ( 0 > (off_end = ftell(fp)) ) {
		return -1L;
	}
	fsz = (size_t)off_end;

	*buf = (char*)malloc(fsz);
	if ( NULL == buf ) {
		return -1L;
	}
	rewind(fp);

	if( fsz != fread(*buf, 1, fsz, fp) ) {
		free(*buf);
		return -1L;
	}

	if( EOF == fclose(fp) ) {
		free(*buf);
		return -1L;
	}

	return (long)fsz;
}
