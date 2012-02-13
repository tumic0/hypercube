#include <iostream>
#include <fstream>
#include "CORE/config.h"
#include "CORE/vertex.h"
#include "CORE/edge.h"
#include "IO/encodings/utf8cvt.h"
#include "svg.h"


using namespace std;

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

static void edges(Graph *graph, wofstream &fs)
{
	Coordinates c1, c2, t;
	Color color;
	Edge *e;

	for (int zValue = -2; zValue < 0; zValue++) {
		for (size_t i = 0; i < graph->edge_size(); i++) {
			e = graph->edge(i);
			if (e->zValue() != zValue)
				continue;

			c1 = e->src()->coordinates() + Coordinates(
			  e->src()->size() / 2, e->src()->size() / 2);
			c2 = e->dst()->coordinates() + Coordinates(
			  e->dst()->size() / 2, e->dst()->size() / 2);
			color = e->color();

			fs << "<g>" << endl;
			fs << "\t<line x1=\"" << c1.x() << "\" y1=\"" << c1.y()
			   << "\" x2=\"" << c2.x() << "\" y2=\"" << c2.y()
			   << "\" stroke=\"" << color
			   << "\" stroke-width=\"" << e->size()
			   << "\"/>" << endl;
			if (e->fontSize() > 0) {
				t = edgeValuePos(c1, c2, e->size(),
				  e->fontSize());
				fs << "\t<text x=\"" << t.x() << "\" y=\"" << t.y()
				   << "\" fill=\"" << color << "\" font-size=\""
				   << e->fontSize() << "\">"
				   << e->text() << "</text>" << endl;
			}
			fs << "</g>" << endl;
		}
		fs << endl;
	}
}

static void vertexes(Graph *graph, wofstream &fs)
{
	Coordinates c;
	Color color;
	Vertex *v;

	for (size_t i = 0; i < graph->vertex_size(); i++) {
		v = graph->vertex(i);

		c = v->coordinates() + Coordinates(
		  v->size() / 2, v->size() / 2);

		fs << "<g fill=\"" << v->color() << "\">" << endl;
		fs << "\t<circle cx=\"" << c.x() << "\" cy=\"" << c.y()
		   << "\" r=\"" << v->size() / 2 << "\"/>" << endl;
		if (v->fontSize() > 0) {
			c += Coordinates(v->size() / 2, v->size());
			fs << "\t<text x=\"" << c.x() << "\" y=\"" << c.y()
			   << "\" font-size=\"" << v->fontSize() << "\">"
			   << v->text() << "</text>" << endl;
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
	edges(graph, fs);
	vertexes(graph, fs);
	fs << "</g>" << endl << endl << "</svg>" << endl;

	fs.close();
	if (fs.fail())
		return WriteError;

	return Ok;
}
