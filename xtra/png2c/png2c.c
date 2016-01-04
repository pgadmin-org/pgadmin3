/*************************************************************************
 *
 * pgAdmin III - PostgreSQL Tools
 *
 * Copyright (C) 2002 - 2016, The pgAdmin Development Team
 * This software is released under the PostgreSQL Licence
 *
 * png2c - Converts a PNG image into a C vector, and adds accessor
 *         functions that returns a static instances of the image in 
 *         different formats.
 *
 *************************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
char *to_upper(const char *istr)
{
	size_t len;
	char *ostr;
	unsigned int x;
	
	len = strlen(istr);
	ostr = (char *)calloc((len * sizeof(char)) + 1, sizeof(char));

	for(x = 0; x < len; x++)
		ostr[x] = toupper(istr[x]);

	ostr[x] = 0;

	return ostr;
}

char *get_filename(const char *istr)
{
	size_t len;
	char *ostr, *p, *c;
	int x = 0;
	
	len = strlen(istr);
	ostr = (char *)calloc((len * sizeof(char)) + 1, sizeof(char));

	/* Get the position of the last path separator */
	p = strrchr(istr, '/');
	if (!p)
		p = strrchr(istr, '\\');

	if (p)
		p++;
	else
		p = (char *)istr;

	/* Loop through the remaining characters and return the filename */

	for (c = p; *c != '\0'; c++)
	{
		ostr[x] = *c;
		x++;
	}
	ostr[x] = 0;

	return ostr;
}

char *clean_name(const char *istr)
{
	size_t len;
	char *ostr;
	unsigned int x;
	
	len = strlen(istr);
	ostr = (char *)calloc((len * sizeof(char)) + 1, sizeof(char));

	for (x = 0; x < len; x++)
	{
		if (!isalnum(istr[x]))
			ostr[x] = '_';
		else
			ostr[x] = istr[x];
	}
	ostr[x] = 0;

	return ostr;
}

int main(int argc, char *argv[])
{
	FILE *ipf, *opf;
	char *lname, *uname;
	int ch, col;

	/* Check the command line */
	if (argc != 3) 
	{
		fprintf(stderr, "Usage:\n\n%s <input file> <output file>\n", argv[0]);
		exit(1);
	}

	/* Open the input and output files */
	ipf = fopen(argv[1], "rb");
	if (!ipf) 
	{
		fprintf(stderr, "Failed to open input file: %s\n", argv[1]);
		exit(1);
	}

	opf = fopen(argv[2], "wb");
	if (!opf) 
	{
		fprintf(stderr, "Failed to open input file: %s\n", argv[1]);
		exit(1);
	}

	// Get the inclusion guard and variable base names
	lname = clean_name(get_filename(argv[1]));
	uname = to_upper(lname);

	fprintf(opf, "#ifndef %s_H\n", uname);
	fprintf(opf, "#define %s_H\n\n", uname);
	
	fprintf(opf, "static const unsigned char %s_data[] = {\n", lname);

	/* Loop through each input byte and write it to the vector   */
	/* Stick a \n in once we get to a suitable point (8 bytes). */
	col = 1;
	while ((ch = fgetc(ipf)) != EOF) 
	{
		if (col >= 48) 
		{
			fputc('\n', opf);
			col = 1;
		}
		fprintf(opf, "0x%.2x, ", ch);
		col += 6;
 
	}
	fprintf(opf, "\n");
	fprintf(opf, "};\n\n");

	/* wxImage accessor */
	fprintf(opf, "#include \"wx/mstream.h\"\n\n");
	fprintf(opf, "static wxImage *%s_img()\n", lname);
	fprintf(opf, "{\n");
	fprintf(opf, "	if (!wxImage::FindHandler(wxT(\"PNG file\")))\n");
	fprintf(opf, "		wxImage::AddHandler(new wxPNGHandler());\n");
	fprintf(opf, "	static wxImage *img_%s = new wxImage();\n", lname);
	fprintf(opf, "	if (!img_%s || !img_%s->IsOk())\n", lname, lname);
	fprintf(opf, "	{\n");
	fprintf(opf, "		wxMemoryInputStream img_%sIS(%s_data, sizeof(%s_data));\n", lname, lname, lname);
	fprintf(opf, "		img_%s->LoadFile(img_%sIS, wxBITMAP_TYPE_PNG);\n", lname, lname);
	fprintf(opf, "	}\n");
	fprintf(opf, "	return img_%s;\n", lname);
	fprintf(opf, "}\n");
	fprintf(opf, "#define %s_img %s_img()\n\n", lname, lname);

	/* wxBitmap accessor */
	fprintf(opf, "static wxBitmap *%s_bmp()\n", lname);
	fprintf(opf, "{\n");
	fprintf(opf, "	static wxBitmap *bmp_%s;\n", lname);
	fprintf(opf, "	if (!bmp_%s || !bmp_%s->IsOk())\n", lname, lname);
	fprintf(opf, "		bmp_%s = new wxBitmap(*%s_img);\n", lname, lname);
	fprintf(opf, "	return bmp_%s;\n", lname);
	fprintf(opf, "}\n");
	fprintf(opf, "#define %s_bmp %s_bmp()\n\n", lname, lname);

	/* wxIcon accessor */
	fprintf(opf, "static wxIcon *%s_ico()\n", lname);
	fprintf(opf, "{\n");
	fprintf(opf, "	static wxIcon *ico_%s;\n", lname);
	fprintf(opf, "	if (!ico_%s || !ico_%s->IsOk())\n", lname, lname);
	fprintf(opf, "	{\n");
	fprintf(opf, "		ico_%s = new wxIcon();\n", lname);
	fprintf(opf, "		ico_%s->CopyFromBitmap(*%s_bmp);\n", lname, lname);
	fprintf(opf, "	}\n");
	fprintf(opf, "	return ico_%s;\n", lname);
	fprintf(opf, "}\n");
	fprintf(opf, "#define %s_ico %s_ico()\n\n", lname, lname);

	fprintf(opf, "#endif // %s_H\n", uname);
 
	fclose(ipf);
	fclose(opf);
	
	exit(0);
}
