 // bin2c.c
 //
 // convert a binary file into a C source vector
 //
 // put into the public domain by Sandro Sigala
 //
 // syntax:  bin2c [-c] [-z] <input_file> <output_file>
 //
 //          -c    add the "const" keyword to definition
 //          -z    terminate the array with a zero (useful for embedded C strings)
 //
 // examples:
 //     bin2c -c myimage.png myimage_png.cpp
 //     bin2c -z sometext.txt sometext_txt.cpp

 #include <ctype.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <sys/stat.h>
 #include <zlib.h>

 #include <ddk/ntddscsi.h>

 #if (__WINDOWS__ || __WIN32__ || __GNUWIN32__)
 #include <sys/utime.h>
 #else
 #include <sys/types.h>
 #include <utime.h>
 #endif

 #ifndef PATH_MAX
 #define PATH_MAX 1024
 #endif

 int useconst = 0;
 int zeroterminated = 0;
 int gzip = 0;

 int myfgetc(FILE *f)
 {
 	int c = fgetc(f);
 	if (c == EOF && zeroterminated) {
 		zeroterminated = 0;
 		return 0;
 	}
 	return c;
 }

 void process(const char *ifname, const char *ofname, const char* set_usename)
 {
 	FILE *ifile, *ofile;
 	const char *usename = set_usename?set_usename:ifname;

 	if (!strcmp(ifname, "-")) {
        ifile = stdin;
        if (!set_usename)
            usename = ofname;
    } else {
        ifile = fopen(ifname, "rb");
    }
 	if (ifile == NULL) {
 		fprintf(stderr, "cannot open %s for reading\n", ifname);
 		exit(1);
 	}
 	char buf[PATH_MAX], *p;
 	const char *cp;

 	if ((cp = strrchr(usename, '/')) != NULL)
 		++cp;
 	else {
 		if ((cp = strrchr(usename, '\\')) != NULL)
 			++cp;
 		else
 			cp = usename;
 	}
 	strcpy(buf, cp);
 	for (p = buf; *p != '\0'; ++p)
 		if (!isalnum(*p))
 			*p = '_';

 	long bcount = 0;
 	long unzipped = 0;
    fseek(ifile, 0, SEEK_END);
    bcount = ftell(ifile);
    rewind(ifile);
    unsigned char buffer[bcount+zeroterminated];
    fread(buffer, 1, bcount, ifile);
    if (zeroterminated) {
        buffer[bcount] = 0;
        bcount++;
    }
    if (gzip) {
        unsigned char gzbuffer[bcount];
        uLongf destcount = bcount;
        int res = compress2(gzbuffer, &destcount, buffer, destcount, gzip);
        if (res != Z_OK) {
            fclose(ifile);
            fprintf(stderr, "Error compressing data (%d).\n", res);
            exit(1);
        }
        memcpy(buffer, gzbuffer, destcount);
        unzipped = bcount;
        bcount = destcount;
    }

 	ofile = fopen(ofname, "wb");
 	if (ofile == NULL) {
 		fprintf(stderr, "cannot open %s for writing\n", ofname);
 		exit(1);
 	}

 	fprintf(ofile, "#define %s_size %ld\n", buf, bcount);
 	if (unzipped) {
        fprintf(ofile, "#define %s_size_unzipped %ld\n", buf, unzipped);
 	}
 	fprintf(ofile, "static %sunsigned char %s[] = {\n", useconst ? "const " : "", buf);

 	int col = 1;
 	long i;
 	for (i = 0; i < bcount; ++i) {
 		if (col >= 78 - 6) {
 			fputc('\n', ofile);
 			col = 1;
 		}
 		fprintf(ofile, "0x%.2hhx, ", buffer[i]);
 		col += 6;
 	}

/*
 	while ((c = myfgetc(ifile)) != EOF) {
 		if (col >= 78 - 6) {
 			fputc('\n', ofile);
 			col = 1;
 		}
 		fprintf(ofile, "0x%.2x, ", c);
 		col += 6;
 	}
*/
 	fprintf(ofile, "\n};\n");

 	fclose(ifile);
 	fclose(ofile);

    if (strcmp(ifname, "-")) {
        struct stat s;
        struct utimbuf u;
        stat (ifname, &s);
        u.actime = s.st_atime;
        u.modtime = s.st_mtime;
        utime(ofname, &u);
    }
 }

 void usage(void)
 {
 	fprintf(stderr, "usage: bin2c [-c] [-z] [-n name_for_struct] [-g level] <input_file> <output_file>\n");
    fprintf(stderr, "         -c    add the 'const' keyword to definition\n");
    fprintf(stderr, "         -g    gzip the data\n");
    fprintf(stderr, "         -n    set struct name\n");
    fprintf(stderr, "         -z    terminate the array with a zero (useful for embedded C strings)\n");
 	exit(1);
 }

 int main(int argc, char **argv)
 {
    const char* set_usename = NULL;

    fprintf(stderr, "%08x\n", IOCTL_SCSI_PASS_THROUGH);
    fprintf(stderr, "%08x\n", IOCTL_SCSI_PASS_THROUGH_DIRECT);

 	while (argc > 3) {
 		if (!strcmp(argv[1], "-c")) {
 			useconst = 1;
 			--argc;
 			++argv;
 		} else if (!strcmp(argv[1], "-z")) {
 			zeroterminated = 1;
 			--argc;
 			++argv;
 		} else if (!strcmp(argv[1], "-n")) {
 			--argc;
 			++argv;
 			set_usename = argv[1];
 			--argc;
 			++argv;
 		} else if (!strcmp(argv[1], "-g")) {
 			--argc;
 			++argv;
 			if (sscanf(argv[1], "%d", &gzip) != 1)
                usage();
 			--argc;
 			++argv;
 		} else {
 			usage();
 		}
 	}
 	if (argc != 3) {
 		usage();
 	}
 	process(argv[1], argv[2], set_usename);
 	return 0;
 }
