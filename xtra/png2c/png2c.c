//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// png2c - Converts a PNG image into a C vector, and adds an accessor
//         function that returns a static instance of the image.
//
// Adapted from Sandro Sigala's bin2c program, as permitted by the licence
// text below.
//
//////////////////////////////////////////////////////////////////////////

// bin2c.c
//
// convert a binary file into a C source vector
//
// THE "BEER-WARE LICENSE" (Revision 3.1415):
// sandro AT sigala DOT it wrote this file. As long as you retain this notice you can do
// whatever you want with this stuff.	If we meet some day, and you think this stuff is
// worth it, you can buy me a beer in return.	Sandro Sigala
//
// syntax:	bin2c [-c] [-z] <input_file> <output_file>
//
//					-c		add the "const" keyword to definition
//					-z		terminate the array with a zero (useful for embedded C strings)
//
// examples:
//		 bin2c -c myimage.png myimage_png.cpp
//		 bin2c -z sometext.txt sometext_txt.cpp
 
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#ifndef PATH_MAX
#define PATH_MAX 1024
#endif
 
int useconst = 0;
int zeroterminated = 0;
 
int myfgetc(FILE *f)
{
	int c = fgetc(f);
	if (c == EOF && zeroterminated) {
		zeroterminated = 0;
		return 0;
	}
	return c;
}
 
 
char* str2upr(char *s)
{
	char * r = (char *) calloc(strlen(s)+1, sizeof(char));
	
	int i = 0;
	while(*s)
		{
			r[i] = toupper(*s);
			++i;
			++s;
		}
	return r;
} 
 
 
 
void process(const char *ifname, const char *ofname)
{
	FILE *ifile, *ofile;

	char buf[PATH_MAX], *p;
	const char *cp;

	int c, col = 1;

	ifile = fopen(ifname, "rb");
	if (ifile == NULL) {
		fprintf(stderr, "cannot open %s for reading\n", ifname);
		exit(1);
	}
	ofile = fopen(ofname, "wb");
	if (ofile == NULL) {
		fprintf(stderr, "cannot open %s for writing\n", ofname);
		exit(1);
	}

	if ((cp = strrchr(ifname, '/')) != NULL)
		++cp;
	else {
		if ((cp = strrchr(ifname, '\\')) != NULL)
			++cp;
		else
			cp = ifname;
	}
	strcpy(buf, cp);
	for (p = buf; *p != '\0'; ++p)
		if (!isalnum(*p))
			*p = '_';
	fprintf(ofile, "#ifndef %s_H\n#define %s_H\nstatic %sunsigned char %s_data[] = {\n",
				 str2upr(buf), str2upr(buf), useconst ? "const " : "", buf);

	while ((c = myfgetc(ifile)) != EOF) {
		if (col >= 78 - 6) {
			fputc('\n', ofile);
			col = 1;
		}
		fprintf(ofile, "0x%.2x, ", c);
		col += 6;
 
	}
	fprintf(ofile, "\n};\n\n");

	// Include an accessor function
	fprintf(ofile, "#include \"wx/mstream.h\"\n");
	fprintf(ofile, "static wxImage *%s_img()\n", buf);
	fprintf(ofile, "{\n");
	fprintf(ofile, "	if (!wxImage::FindHandler(wxT(\"PNG file\")))\n");
	fprintf(ofile, "		wxImage::AddHandler(new wxPNGHandler());\n");
	fprintf(ofile, "	static wxImage *img_%s = new wxImage();\n", buf);
	fprintf(ofile, "	if (!img_%s || !img_%s->IsOk())\n", buf, buf);
	fprintf(ofile, "	{\n");
	fprintf(ofile, "		wxMemoryInputStream img_%sIS(%s_data, sizeof(%s_data));\n", buf, buf, buf);
	fprintf(ofile, "		img_%s->LoadFile(img_%sIS, wxBITMAP_TYPE_PNG);\n", buf, buf);
	fprintf(ofile, "	}\n");
	fprintf(ofile, "	return img_%s;\n", buf);
	fprintf(ofile, "}\n\n");
	fprintf(ofile, "#define %s_img %s_img()\n\n", buf, buf);

	fprintf(ofile, "static wxBitmap *%s_bmp()\n", buf);
	fprintf(ofile, "{\n");
	fprintf(ofile, "	static wxBitmap *bmp_%s;\n", buf);
	fprintf(ofile, "	if (!bmp_%s || !bmp_%s->IsOk())\n", buf, buf);
	fprintf(ofile, "		bmp_%s = new wxBitmap(*%s_img);\n", buf, buf);
	fprintf(ofile, "	return bmp_%s;\n", buf);
	fprintf(ofile, "}\n\n");
	fprintf(ofile, "#define %s_bmp %s_bmp()\n\n", buf, buf);

	fprintf(ofile, "static wxIcon *%s_ico()\n", buf);
	fprintf(ofile, "{\n");
	fprintf(ofile, "	static wxIcon *ico_%s;\n", buf);
	fprintf(ofile, "	if (!ico_%s || !ico_%s->IsOk())\n", buf, buf);
	fprintf(ofile, "	{\n");
	fprintf(ofile, "		ico_%s = new wxIcon();\n", buf);
	fprintf(ofile, "		ico_%s->CopyFromBitmap(*%s_bmp);\n", buf, buf);
	fprintf(ofile, "	}\n");
	fprintf(ofile, "	return ico_%s;\n", buf);
	fprintf(ofile, "}\n\n");
	fprintf(ofile, "#define %s_ico %s_ico()\n\n", buf, buf);

	fprintf(ofile, "\n#endif\n");
 
	fclose(ifile);
	fclose(ofile);
}
 
void usage(void)
{
	fprintf(stderr, "usage: bin2c [-cz] <input_file> <output_file>\n");
	exit(1);
}
 
int main(int argc, char **argv)
{
	while (argc > 3) {
		if (!strcmp(argv[1], "-c")) {
			useconst = 1;
			--argc;
			++argv;
		} else if (!strcmp(argv[1], "-z")) {
			zeroterminated = 1;
			--argc;
			++argv;
		} else {
			usage();
		}
	}
	if (argc != 3) {
		usage();
	}
	process(argv[1], argv[2]);
	return 0;
}
