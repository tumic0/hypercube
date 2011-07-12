#include <iostream>
#include <fstream>
#include "CORE/config.h"
#include "utf8cvt.h"
#include "svg.h"

using namespace std;


/*!
	\class SvgGraphOutput
	\brief SVG output provider.

	Lorem ipsum...
*/


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
	int width = graph->dimensions().x();
	int height = graph->dimensions().y();

	fs << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << endl
	   << "<!-- Created with Hypercube ("APP_HOMEPAGE") -->" << endl << endl
	   << "<svg width=\"" << width << "\" height=\"" << height
	   << "\" viewBox=\"0 0 " << width << " " << height
	   << "\" xmlns=\"http://www.w3.org/2000/svg\" "
	      "baseProfile=\"tiny\">" << endl << endl
	   << "<g font-family=\"" FONT_FAMILY "\" "
	      "font-weight=\"normal\">"<< endl;
}

static void edges(Graph *graph, wofstream &fs, int zValue)
{
	Coordinates c1, c2, t;
	Color color;

	for (int i = 0; i < graph->size(); i++) {
		c1 = graph->vertexCoordinates(i) + Coordinates(
		  graph->vertexSize(i) / 2, graph->vertexSize(i) / 2);
		for (int j = 0; j < i; j++) {
			if (graph->edge(i, j) && graph->edgeZValue(i, j) == zValue) {
				c2 = graph->vertexCoordinates(j) + Coordinates(
				  graph->vertexSize(j) / 2, graph->vertexSize(j) / 2);
				color = graph->edgeColor(i, j);

				fs << "<g>" << endl;
				fs << "\t<line x1=\"" << c1.x() << "\" y1=\"" << c1.y()
				   << "\" x2=\"" << c2.x() << "\" y2=\"" << c2.y()
				   << "\" stroke=\"" << color
				   << "\" stroke-width=\"" << graph->edgeSize(i, j)
				   << "\"/>" << endl;
				if (graph->edgeFontSize(i, j) > 0) {
					t = edgeValuePos(c1, c2, graph->edgeSize(i, j),
					  graph->edgeFontSize(i, j));
					fs << "\t<text x=\"" << t.x() << "\" y=\"" << t.y()
					   << "\" fill=\"" << color << "\" font-size=\""
					   << graph->edgeFontSize(i, j) << "\">"
					   << graph->edgeText(i, j) << "</text>" << endl;
				}
				fs << "</g>" << endl;
			}
		}
	}
	fs << endl;
}

static void vertexes(Graph *graph, wofstream &fs)
{
	Coordinates c1;
	Color color;

	for (int i = 0; i < graph->size(); i++) {
		c1 = graph->vertexCoordinates(i) + Coordinates(
		  graph->vertexSize(i) / 2, graph->vertexSize(i) / 2);
		fs << "<g fill=\"" << graph->vertexColor(i) << "\">" << endl;
		fs << "\t<circle cx=\"" << c1.x() << "\" cy=\"" << c1.y()
		   << "\" r=\"" << graph->vertexSize(i) / 2 << "\"/>" << endl;
		if (graph->vertexFontSize(i) > 0) {
			c1 += Coordinates(graph->vertexSize(i) / 2, graph->vertexSize(i));
			fs << "\t<text x=\"" << c1.x() << "\" y=\"" << c1.y()
			   << "\" font-size=\"" << graph->vertexFontSize(i) << "\">"
			   << graph->vertexText(i) << "</text>" << endl;
		}
		fs << "</g>" << endl;
	}
	fs << endl;
}


IO::Error SvgGraphOutput::writeGraph(Graph *graph, const char *filename)
{
	wofstream fs;
	locale utf8(std::locale(), new utf8cvt);

	fs.imbue(utf8);
	fs.open(filename);
	if (!fs)
		return OpenError;

	header(graph, fs);
	edges(graph, fs, -2);
	edges(graph, fs, -1);
	vertexes(graph, fs);

	/* SVG end */
	fs << "</g>" << endl << endl << "</svg>" << endl;

	fs.close();
	if (fs.fail())
		return WriteError;

	return Ok;
}
