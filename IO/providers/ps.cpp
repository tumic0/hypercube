#include <cstring>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "CORE/vertex.h"
#include "CORE/edge.h"
#include "CORE/config.h"
#include "IO/modules.h"
#include "IO/providers/ps/snippet.h"
#include "ps.h"

using namespace std;


#define tr(val,dim) ((dim).y()-(val))

static void prolog(Graph *graph, PsSnippet *sn, wofstream &fs)
{
	Coordinates dim = graph->dimensions();

	fs << "%!PS-Adobe-3.0 EPSF-3.0" << endl
	   << "%%BoundingBox: 0 0 " << dim.x() << " " << dim.y() << endl
	   << "%%Creator: Hypercube ("APP_HOMEPAGE")" << endl;

	if (sn->font()->font())
		fs << "%%DocumentSuppliedResources: font ";
	else
		fs << "%%DocumentNeededResources: font ";
	fs << sn->font()->name() << endl;

	fs << "%%EndComments" << endl << endl
	   << "%%BeginProlog" << endl;

	if (sn->font()->font())
		fs << "%%BeginResource: font " << sn->font()->name() << endl
		   << sn->font()->font() << endl
		   << "%%EndResource" << endl << endl;

	if (sn->encoding()->encoding()) {
		fs << "/encoding [" << endl
		   << sn->encoding()->encoding() << endl
		   << "] def" << endl << endl;

		fs << "/reencode {" << endl
		   << "	findfont" << endl
		   << "	dup length dict begin" << endl
		   << "		{1 index /FID ne {def} {pop pop} ifelse} forall" << endl
		   << "		/Encoding encoding def" << endl
		   << "		currentdict" << endl
		   << "	end" << endl
		   << "	definefont pop" << endl
		   << "} def" << endl << endl;

		fs << "/rfont /" << sn->font()->name() << " reencode" << endl
		   << "/font /rfont def" << endl << endl;
	} else
		fs << "/font /" << sn->font()->name() << " def" << endl << endl;

	fs << "/e {newpath moveto lineto stroke} def" << endl
	   << "/a {newpath moveto lineto lineto closepath fill} def" << endl
	   << "/v {newpath arc closepath fill} def" << endl
	   << "/d {moveto show} def" << endl
	   << "/f {font findfont exch scalefont setfont} def" << endl
	   << "/lw {setlinewidth} def" << endl
	   << "/c {setrgbcolor} def" << endl << endl;

	fs << "%%EndProlog" << endl << endl;
}

static void edges(Graph *graph, wofstream &fs)
{
	Color color;
	int fontSize = -1, lineWidth = -1;
	Coordinates dim = graph->dimensions();


	for (int zValue = -2; zValue < 0; zValue++ ) {
		for (size_t i = 0; i < graph->edge_size(); i++) {
			Edge *e = graph->edge(i);
			if (e->zValue() != zValue)
				continue;

			LineF line(
			  CoordinatesF((float)e->src()->size() / 2, (float)e->src()->size() / 2)
				+ e->src()->coordinates(),
			  CoordinatesF((float)e->dst()->size() / 2, (float)e->dst()->size() / 2)
				+ e->dst()->coordinates()
			);

			if (color != e->color()) {
				color = e->color();
				fs << color.red() << " " << color.green() << " "
				   << color.blue() << " c" << endl;
			}

			if (e->fontSize()) {
				if (e->fontSize() != fontSize) {
					fontSize = e->fontSize();
					fs << fontSize << " f" << endl;
				}

				CoordinatesF textBox(
				  e->text().length() * AVG_CHAR_WIDTH * e->fontSize(),
				  (float)e->fontSize());
				CoordinatesF t = OutputProvider::edgeTextPosition(
				  line, (float)e->size(), textBox);

				fs << "(" << e->text() << ") "
				   << t.x() << " " << tr(t.y(), dim) << " d" << endl;
			}

			if (e->directed()) {
				OutputProvider::Arrow arrow = OutputProvider::arrow(line,
				  (float)e->dst()->size());

				fs << arrow.p[0].x() << " " << tr(arrow.p[0].y(), dim) << " "
				   << arrow.p[1].x() << " " << tr(arrow.p[1].y(), dim) << " "
				   << arrow.p[2].x() << " " << tr(arrow.p[2].y(), dim) << " a"
				   << endl;
			}

			if (lineWidth != e->size()) {
				lineWidth = e->size();
				fs << lineWidth << " lw" << endl;
			}
			fs << line.p1().x() << " " << tr(line.p1().y(), dim) << " "
			   << line.p2().x() << " " << tr(line.p2().y(), dim) << " e"
			   << endl;
		}
		fs << endl;
	}
}

static void vertexes(Graph *graph, wofstream &fs)
{
	Color color;
	int fontSize = -1;
	Coordinates dim = graph->dimensions();


	for (size_t i = 0; i < graph->vertex_size(); i++) {
		Vertex *v = graph->vertex(i);

		CoordinatesF c(v->coordinates() + CoordinatesF(
		  (float)v->size() / 2, (float)v->size() / 2));

		if (v->color() != color) {
			color = v->color();
			fs << color.red() << " " << color.green() << " " << color.blue()
			   << " c" << endl;
		}

		fs << c.x() << " " << tr(c.y(), dim) << " " << v->size() / 2.0
		   << " 0 360 v" << endl;


		if (v->fontSize() == 0)
			continue;

		if (v->fontSize() != fontSize) {
			fontSize = v->fontSize();
			fs << fontSize << " f" << endl;
		}

		c = OutputProvider::vertexTextPosition(c, (float)v->size());
		fs << "(" << v->text() << ") "
		   << c.x() << " " << tr(c.y(), dim) << " d" << endl;
	}
	fs << endl;
}


IO::Error PsGraphOutput::writeGraph(Graph *graph, const char *filename)
{
	for (PsSnippet **sp = snippets; *sp; sp++) {
		codecvt<wchar_t,char,mbstate_t> *cvt = 0;

		for (Encoding **ep = encodings; *ep; ep++)
			if (!strcmp((*sp)->encoding()->name(), (*ep)->name()))
				cvt = (*ep)->cvt();
		if (!cvt)
			return WriteError;

		wofstream fs;
		locale lc(locale(), cvt);
		fs.imbue(lc);
		fs.open(filename);

		prolog(graph, *sp, fs);
		edges(graph, fs);
		vertexes(graph, fs);
		fs << "%%EOF" << endl;

		fs.close();
		if (!fs.fail())
			return Ok;
	}

	return WriteError;
}
