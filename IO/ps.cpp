#include <iostream>
#include <iomanip>
#include <fstream>
#include "CORE/config.h"
#include "ps.h"

using namespace std;


/*!
	\class PsGraphOutput
	\brief PostScript output provider.

	Lorem ipsum...
*/


#define tr(val) (dim.y()-(val))


static Coordinates edgeValuePos(Coordinates &p1, Coordinates &p2,
  int size, int fontSize)
{
	Coordinates u, m;

	u = p2 - p1;
	m = p1 + (u / 2);

	if ((u.x() > 0 && u.y() > 0) || (u.x() < 0 && u.y() < 0))
		return Coordinates(m.x() + size / 2, m.y() - size / 2);
	else
		return Coordinates(m.x() + size / 2, m.y() + size / 2 + fontSize);
}


static void header(Graph *graph, wofstream &fs)
{
	Coordinates dim = graph->dimensions();

	/* DSC header */
	fs << "%!PS-Adobe-3.0 EPSF-3.0" << endl
	   << "%%BoundingBox: 0 0 " << dim.x() << " " << dim.y() << endl
	   << "%%Creator: Hypercube" << endl
	   << "%%EndComments" << endl << endl;

	/* Macros definitions */
	fs << "/e {newpath moveto lineto stroke} def" << endl
	   << "/v {newpath arc closepath fill} def" << endl
	   << "/d {moveto show} def" << endl
	   << "/f {/" FONT_FAMILY " findfont exch scalefont setfont} def" << endl
	   << "/lw {setlinewidth} def" << endl
	   << "/c {setrgbcolor} def" << endl << endl;
}

static void edges(Graph *graph, wofstream &fs, int zValue)
{
	Coordinates c1, c2, t;
	Color color;
	int fontSize, lineWidth;
	Coordinates dim = graph->dimensions();


	color = Color();
	lineWidth = -1;
	for (int i = 0; i < graph->size(); i++) {
		c1 = graph->vertexCoordinates(i) + Coordinates(
		  graph->vertexSize(i) / 2, graph->vertexSize(i) / 2);
		for (int j = 0; j < i; j++) {
			if (graph->edge(i, j) && graph->edgeZValue(i, j) == zValue) {
				c2 = graph->vertexCoordinates(j) + Coordinates(
		  		  graph->vertexSize(j) / 2, graph->vertexSize(j) / 2);
				if (color != graph->edgeColor(i, j)) {
					color = graph->edgeColor(i, j);
					fs << color.red() << " " << color.green() << " "
					   << color.blue() << " c" << endl;
				}
				if (lineWidth != graph->edgeSize(i, j)) {
					lineWidth = graph->edgeSize(i, j);
					fs << lineWidth << " lw" << endl;
				}
				fs << c1.x() << " " << tr(c1.y()) << " "
				   << c2.x() << " " << tr(c2.y()) << " e" << endl;
			}
		}
	}
	fs << endl;

	fontSize = -1;
	for (int i = 0; i < graph->size(); i++) {
		c1 = graph->vertexCoordinates(i) + Coordinates(
		  graph->vertexSize(i) / 2, graph->vertexSize(i) / 2);
		for (int j = 0; j < i; j++) {
			if (graph->edge(i, j) && graph->edgeFontSize(i, j) > 0) {
				c2 = graph->vertexCoordinates(j) + Coordinates(
				  graph->vertexSize(j) / 2, graph->vertexSize(j) / 2);
				if (graph->edgeFontSize(i, j) != fontSize) {
					fontSize = graph->edgeFontSize(i, j);
					fs << fontSize << " f" << endl;
				}
				if (graph->edgeColor(i, j) != color) {
					color = graph->edgeColor(i, j);
					fs << color.red() << " " << color.green() << " "
					   << color.blue() << " c" << endl;
				}
				t = edgeValuePos(c1, c2, graph->edgeSize(i, j),
				  graph->edgeFontSize(i, j));
				fs << "(" << graph->edgeText(i, j) << ") "
				   << t.x() << " " << tr(t.y()) << " d" << endl;
			}
		}
	}
	fs << endl;
}

static void vertexes(Graph *graph, wofstream &fs)
{
	Coordinates c1;
	Color color;
	int fontSize;
	Coordinates dim = graph->dimensions();


	for (int i = 0; i < graph->size(); i++) {
		c1 = graph->vertexCoordinates(i) + Coordinates(
		  graph->vertexSize(i) / 2, graph->vertexSize(i) / 2);
		if (graph->vertexColor(i) != color) {
			color = graph->vertexColor(i);
			fs << color.red() << " " << color.green() << " " << color.blue()
			   << " c" << endl;
		}
		fs << c1.x() << " " << tr(c1.y()) << " " << graph->vertexSize(i) / 2
		   << " 0 360 v" << endl;
	}
	fs << endl;


	fontSize = -1;
	for (int i = 0; i < graph->size(); i++) {
		if (graph->vertexFontSize(i) > 0) {
			c1 = graph->vertexCoordinates(i) + Coordinates(
			  graph->vertexSize(i) / 2, graph->vertexSize(i) / 2);
			if (graph->vertexFontSize(i) != fontSize) {
				fontSize = graph->vertexFontSize(i);
				fs << fontSize << " f" << endl;
			}
			if (graph->vertexColor(i) != color) {
				color = graph->vertexColor(i);
				fs << color.red() << " " << color.green() << " " << color.blue()
				   << " c" << endl;
			}
			fs << "(" << graph->vertexText(i) << ") "
			   << c1.x() + graph->vertexSize(i) / 2 << " "
			   << tr(c1.y() + graph->vertexSize(i))
			   << " d" << endl;
		}
	}
	fs << endl;
}


IO::Error PsGraphOutput::writeGraph(Graph *graph, const char *filename)
{
	wofstream fs;

	fs.open(filename);

	header(graph, fs);
	edges(graph, fs, -2);
	edges(graph, fs, -1);
	vertexes(graph, fs);

	/* EPS end */
	fs << "%%EOF" << endl;

	fs.close();
	if (fs.fail())
		return WriteError;

	return Ok;
}
