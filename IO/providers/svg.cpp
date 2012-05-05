#include <cstring>
#include <cerrno>
#include <iostream>
#include <fstream>
#include "CORE/config.h"
#include "CORE/vertex.h"
#include "CORE/edge.h"
#include "CORE/line.h"
#include "CORE/misc.h"
#include "IO/encodings/utf8cvt.h"
#include "svg.h"

using namespace std;


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
	for (int zValue = -2; zValue < 0; zValue++) {
		for (size_t i = 0; i < graph->edge_size(); i++) {
			Edge *e = graph->edge(i);
			if (e->zValue() != zValue)
				continue;

			LineF line(
			  CoordinatesF(e->src()->coordinates())
				+ CoordinatesF((float)e->src()->size() / 2, (float)e->src()->size() / 2),
			  CoordinatesF(e->dst()->coordinates())
				+ CoordinatesF((float)e->dst()->size() / 2, (float)e->dst()->size() / 2)
			);

			fs << "<g>" << endl;

			if (e->fontSize() > 0) {
				CoordinatesF textBox(
				  e->text().length() * AVG_CHAR_WIDTH * e->fontSize(),
				  (float)e->fontSize());
				CoordinatesF t = OutputProvider::edgeTextPosition(
				  line, (float)e->size(), textBox);

				fs << "\t<text x=\"" << t.toCoordinates().x()
				   << "\" y=\"" << t.toCoordinates().y()
				   << "\" fill=\"" << e->color()
				   << "\" font-size=\"" << e->fontSize()
				   << "\">" << e->text() << "</text>" << endl;
			}

			if (e->directed()) {
				if (e->twin())
					line.setP1(line.pointAt(0.5));

				OutputProvider::Arrow arrow = OutputProvider::arrow(line,
				  (float)e->dst()->size());

				fs << "\t<polygon points=\""
				   << arrow.p[0].toCoordinates() << " "
				   << arrow.p[1].toCoordinates() << " "
				   << arrow.p[2].toCoordinates()
				   << "\" fill=\"" << e->color()
				   << "\"/>" << endl;
			}

			fs << "\t<line x1=\"" << line.p1().toCoordinates().x()
			   << "\" y1=\"" << line.p1().toCoordinates().y()
			   << "\" x2=\"" << line.p2().toCoordinates().x()
			   << "\" y2=\"" << line.p2().toCoordinates().y()
			   << "\" stroke=\"" << e->color()
			   << "\" stroke-width=\"" << e->size()
			   << "\"/>" << endl;

			fs << "</g>" << endl;
		}
		fs << endl;
	}
}

static void vertexes(Graph *graph, wofstream &fs)
{
	for (size_t i = 0; i < graph->vertex_size(); i++) {
		Vertex *v = graph->vertex(i);

		CoordinatesF c(v->coordinates() + Coordinates(
		  v->size() / 2, v->size() / 2));

		fs << "<g fill=\"" << v->color() << "\">" << endl;
		fs << "\t<circle cx=\"" << c.toCoordinates().x()
		   << "\" cy=\"" << c.toCoordinates().y()
		   << "\" r=\"" << v->size() / 2 << "\"/>" << endl;

		if (v->fontSize() > 0) {
			c = OutputProvider::vertexTextPosition(c, (float)v->size());
			fs << "\t<text x=\"" << c.x() << "\" y=\"" << c.y()
			   << "\" font-size=\"" << v->fontSize() << "\">"
			   << v->text() << "</text>" << endl;
		}

		fs << "</g>" << endl;
	}
	fs << endl;
}


IO::Error SvgGraphOutput::writeGraph(Graph *graph, const char *fileName)
{
	wofstream fs;
	locale utf8(std::locale(), new utf8cvt);

	fs.imbue(utf8);
	fs.open(fileName);
	if (!fs) {
		perror(fileName);
		return OpenError;
	}

	header(graph, fs);
	edges(graph, fs);
	vertexes(graph, fs);
	fs << "</g>" << endl << endl << "</svg>" << endl;

	fs.close();
	if (fs.fail())
		return WriteError;

	return Ok;
}
