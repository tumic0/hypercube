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
	   << "/v {newpath arc closepath fill} def" << endl
	   << "/d {moveto show} def" << endl
	   << "/f {font findfont exch scalefont setfont} def" << endl
	   << "/lw {setlinewidth} def" << endl
	   << "/c {setrgbcolor} def" << endl << endl;

	fs << "%%EndProlog" << endl << endl;
}

static void edges(Graph *graph, wofstream &fs)
{
	Coordinates c1, c2, t;
	Color color;
	int fontSize = -1, lineWidth = -1;
	Coordinates dim = graph->dimensions();
	Edge *e;


	for (int zValue = -2; zValue < 0; zValue++ ) {
		for (size_t i = 0; i < graph->edge_size(); i++) {
			e = graph->edge(i);
			if (e->zValue() != zValue)
				continue;

			c1 = e->src()->coordinates() + Coordinates(
			  e->src()->size() / 2, e->src()->size() / 2);
			c2 = e->dst()->coordinates() + Coordinates(
			  e->dst()->size() / 2, e->dst()->size() / 2);

			if (color != e->color()) {
				color = e->color();
				fs << color.red() << " " << color.green() << " "
				   << color.blue() << " c" << endl;
			}
			if (lineWidth != e->size()) {
				lineWidth = e->size();
				fs << lineWidth << " lw" << endl;
			}
			fs << c1.x() << " " << tr(c1.y(), dim) << " "
			   << c2.x() << " " << tr(c2.y(), dim) << " e" << endl;
		}
		fs << endl;

		for (size_t i = 0; i < graph->edge_size(); i++) {
			e = graph->edge(i);
			if (e->zValue() != zValue || e->fontSize() == 0)
				continue;

			c1 = e->src()->coordinates() + Coordinates(
			  e->src()->size() / 2, e->src()->size() / 2);
			c2 = e->dst()->coordinates() + Coordinates(
			  e->dst()->size() / 2, e->dst()->size() / 2);

			if (e->fontSize() != fontSize) {
				fontSize = e->fontSize();
				fs << fontSize << " f" << endl;
			}
			if (e->color() != color) {
				color = e->color();
				fs << color.red() << " " << color.green() << " "
				   << color.blue() << " c" << endl;
			}

			t = edgeValuePos(c1, c2, e->size(), e->fontSize());
			fs << "(" << e->text() << ") "
			   << t.x() << " " << tr(t.y(), dim) << " d" << endl;
		}
		fs << endl;
	}
}

static void vertexes(Graph *graph, wofstream &fs)
{
	Coordinates c;
	Color color;
	int fontSize = -1;
	Coordinates dim = graph->dimensions();
	Vertex *v;


	for (size_t i = 0; i < graph->vertex_size(); i++) {
		v = graph->vertex(i);

		c = v->coordinates()
		  + Coordinates(v->size() / 2, v->size() / 2);

		if (v->color() != color) {
			color = v->color();
			fs << color.red() << " " << color.green() << " " << color.blue()
			   << " c" << endl;
		}
		fs << c.x() << " " << tr(c.y(), dim) << " " << v->size() / 2
		   << " 0 360 v" << endl;
	}
	fs << endl;

	for (size_t i = 0; i < graph->vertex_size(); i++) {
		v = graph->vertex(i);
		if (v->fontSize() == 0)
			continue;

		c = v->coordinates()
		  + Coordinates(v->size() / 2, v->size() / 2);

		if (v->fontSize() != fontSize) {
			fontSize = v->fontSize();
			fs << fontSize << " f" << endl;
		}
		if (v->color() != color) {
			color = v->color();
			fs << color.red() << " " << color.green() << " " << color.blue()
			   << " c" << endl;
		}
		fs << "(" << v->text() << ") "
		   << c.x() + v->size() / 2 << " "
		   << tr(c.y() + v->size(), dim)
		   << " d" << endl;
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
