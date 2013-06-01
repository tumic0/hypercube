#include <cstring>
#include <cerrno>
#include <fstream>
#include "CORE/config.h"
#include "CORE/misc.h"
#include "CORE/vertex.h"
#include "CORE/edge.h"
#include "CORE/line.h"
#include "IO/encodings/utf8cvt.h"
#include "svg.h"

using namespace std;


static void escape(wstring &str)
{
	stringReplace(str, L"&", L"&amp;");
	stringReplace(str, L"<", L"&lt;");
	stringReplace(str, L">", L"&gt;");
	stringReplace(str, L"\"", L"&quot;");
}

void SvgGraphOutput::header(Graph *graph, wofstream &fs)
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

void SvgGraphOutput::footer(wofstream &fs)
{
	fs << "</g>" << endl << endl << "</svg>" << endl;
}

void SvgGraphOutput::edges(Graph *graph, wofstream &fs)
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
				CoordinatesF t = edgeTextPosition(line, (float)e->size(),
				  textBox);

				wstring text(e->text());
				escape(text);

				fs << "\t<text x=\"" << t.toCoordinates().x()
				   << "\" y=\"" << t.toCoordinates().y()
				   << "\" fill=\"" << e->color()
				   << "\" font-size=\"" << e->fontSize()
				   << "\">" << text << "</text>" << endl;
			}

			if (graph->directed()) {
				if (e->twin())
					line.setP1(line.pointAt(0.5));

				Arrow arw = arrow(line, (float)e->dst()->size());

				fs << "\t<polygon points=\""
				   << arw.p[0].toCoordinates() << " "
				   << arw.p[1].toCoordinates() << " "
				   << arw.p[2].toCoordinates()
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

void SvgGraphOutput::vertexes(Graph *graph, wofstream &fs)
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
			wstring text(v->text());
			escape(text);

			c = vertexTextPosition(c, (float)v->size());
			fs << "\t<text x=\"" << c.x() << "\" y=\"" << c.y()
			   << "\" font-size=\"" << v->fontSize() << "\">"
			   << text << "</text>" << endl;
		}

		fs << "</g>" << endl;
	}
	fs << endl;
}

void SvgGraphOutput::legend(Graph *graph, wofstream &fs)
{
	float size = (float)graph->legend();
	Coordinates r = legendRectSize(size).toCoordinates();
	int index = 0;

	fs << "<g font-size=\"" << graph->legend() << "\">" << endl;

	for (ColorMap::iterator it = graph->colorMap()->begin();
	  it != graph->colorMap()->end(); it++) {
		Coordinates c = legendPosition(index, size).toCoordinates();
		Coordinates t = legendTextPosition(c, size).toCoordinates();
		wstring text((*it).first);

		escape(text);

		fs << "<g>" << endl;
		fs << "\t<rect x=\"" << c.x() << "\" y=\"" << c.y() << "\" "
		   << "width=\"" << r.x() << "\" " << "height=\"" << r.y() << "\" "
		   << "stroke=\"#000000\" fill=\"" << (*it).second << "\"/>" << endl;
		fs << "\t<text x=\"" << t.x() << "\" y=\"" << t.y() << "\">"
		   << text << "</text>" << endl;
		fs << "</g>" << endl;

		index++;
	}

	fs << "</g>" << endl << endl;
}

IO::Error SvgGraphOutput::writeGraph(Graph *graph, const char *fileName)
{
	wofstream fs;
	locale utf8(std::locale(), new utf8cvt);

	fs.imbue(utf8);
	fs.open(fileName);
	if (!fs) {
		ioerr << fileName << ": " << strerror(errno) << endl;
		return OpenError;
	}

	header(graph, fs);
	edges(graph, fs);
	vertexes(graph, fs);
	if (graph->legend())
		legend(graph, fs);
	footer(fs);

	fs.close();
	if (fs.fail()) {
		remove(fileName);
		return WriteError;
	}

	return Ok;
}
