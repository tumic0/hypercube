#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <set>
#include "CORE/graph.h"
#include "CORE/sa.h"
#include "CORE/misc.h"
#include "CORE/config.h"
#include "IO/io.h"
#include "IO/modules.h"
#include "cli.h"

using namespace std;


#define map_iterator map<wstring, wstring>::const_iterator
#define set_iterator set<wstring>::iterator


static string replaceExtension(string const &fileName, string const &extension)
{
	size_t len;

	if ((len = fileName.rfind('.')) == string::npos)
		return fileName + string(".") + extension;
	else
		return fileName.substr(0, len + 1) + extension;
}


CLI::CLI(int argc, char *argv[])
{
	_argc = argc;
	_argv = argv;

	_graph = new Graph();
	_sa = new SA();

	_dimensions = Coordinates(GRAPH_WIDTH, GRAPH_HEIGHT);
	_vertexColor = VERTEX_COLOR;
	_edgeColor = EDGE_COLOR;
	_vertexSize = VERTEX_SIZE;
	_edgeSize = EDGE_SIZE;
	_vertexFontSize = VERTEX_FONT_SIZE;
	_edgeFontSize = EDGE_FONT_SIZE;
	_legend = 0;

	_nodeDistribution = NODE_DISTRIBUTION;
	_edgeLength = EDGE_LENGTH;
	_edgeCrossings = EDGE_CROSSINGS;
	_initTemp = INIT_TEMP;
	_finalTemp = FINAL_TEMP;
	_coolFactor = COOL_FACTOR;
	_numSteps = NUM_STEPS;

	_coloredEdges = false;
	_forceDirected = false;
	_forceUndirected = false;
}

CLI::~CLI()
{
	delete _graph;
	delete _sa;
}

int CLI::exec()
{
	if (!parseArguments())
		return EXIT_FAILURE;

	if (!readGraph())
		return EXIT_FAILURE;

	if (!setGraphProperties())
		return EXIT_FAILURE;
	setSAProperties();
	_graph->randomize();
	_sa->compute(_graph);
	_graph->center();

	if (!writeGraph())
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}


bool CLI::readGraph()
{
	Encoding **e = encodings;
	IO::Error error = IO::FormatError;
	int found = 0;

	if (_encoding.empty())
		_encoding = (*e)->name();

	for (e = encodings; *e; e++)
		if ((found = !_encoding.compare((*e)->name())))
			break;

	if (!found) {
		cerr << "Unknown input encoding: " << _encoding << endl;
		return false;
	}

	for (InputProvider **p = inputProviders; *p; p++) {
		(*p)->setInputEncoding(*e);
		error = (*p)->readGraph(_graph, _inputFileName.c_str());
		if (error != IO::FormatError)
			break;
	}

	if (error) {
		if (error == IO::FormatError)
			cerr << "Unknown/invalid input format!" << endl;
		else if (error == IO::OpenError)
			cerr << "Error opening input file!" << endl;
		else if (error == IO::ReadError)
			cerr << "Error reading input file!" << endl;

		if (!IO::ioerr.str().empty())
			cerr << IO::ioerr.str();

		return false;
	}

	return true;
}

bool CLI::writeGraph()
{
	OutputProvider **p = outputProviders;
	IO::Error error;
	int found = 0;

	if (_format.empty())
		_format = (*p)->type();
	if (_outputFileName.empty())
		_outputFileName = replaceExtension(_inputFileName, _format);

	for (p = outputProviders; *p; p++)
		if ((found = !_format.compare((*p)->type())))
			break;

	if (!found) {
		cerr << "Unknown output format: " << _format << endl;
		return false;
	}

	IO::ioerr.str("");
	error = (*p)->writeGraph(_graph, _outputFileName.c_str());

	if (error) {
		if (error == IO::OpenError)
			cerr << "Error opening output file!" << endl;
		else if (error == IO::WriteError)
			cerr << "Error writing output file!" << endl;

		if (!IO::ioerr.str().empty())
			cerr << IO::ioerr.str();

		return false;
	}

	return true;
}

void CLI::usage()
{
	cout << "Usage:    hypercube-cli [OPTIONS] FILE" << endl;
	cout << endl;
	cout << "OPTIONS:" << endl;
	cout << " -v               print the program version and exit" << endl;
	cout << endl;
	cout << " -s <dimensions>  set image size to <dimensions>" << endl;
	cout << " -f <format>      set output format to <format>" << endl;
	cout << " -e <encoding>    set input file encoding to <encoding>" << endl;
	cout << " -va <attribute>  use <attribute> for vertex labels" << endl;
	cout << " -ea <attribute>  use <attribute> for edge labels" << endl;
	cout << " -o <file>        set the output file to <file>" << endl;
	cout << " -vc <color>      set vertex color to <color>" << endl;
	cout << " -ec <color>      set edge color to <color>" << endl;
	cout << " -vs <size>       set vertex size to <size>" << endl;
	cout << " -es <size>       set edge size to <size>" << endl;
	cout << " -vf <size>       set vertex font size to <size>" << endl;
	cout << " -ef <size>       set edge font size to <size>" << endl;
	cout << " -d               directed graph" << endl;
	cout << " -u               undirected graph" << endl;
	cout << " -c               asign a unique color to every uniqe edge value"
		 << endl;
	cout << " -l <size>        show edge color legend with font size <size>"
		 << endl;
	cout << endl;
	cout << "option arguments:" << endl;
	cout << " <dimesnsions>    width,height" << endl;
	cout << " <color>          #RRGGBB" << endl;

	cout << " <format>         ";
	for (OutputProvider **p = outputProviders; *p; p++)
		cout << (*p)->type() << " ";
	cout << endl;

	cout << " <encoding>       ";
	for (Encoding **e = encodings; *e; e++)
		cout << (*e)->name() << " ";
	cout << endl;
}

int CLI::argument(int i)
{
#define ARG(arg, var) \
	if (string(_argv[i]) == string(arg)) { \
		if (i + 1 == _argc) \
			return -1; \
		istringstream is(_argv[i + 1]); \
		if (!(is >> var)) \
			return -1; \
		return 2; \
	}

#define SWITCH(arg, var) \
	if (string(_argv[i]) == string(arg)) { \
		var = true; \
		return 1; \
	}

	ARG("-f", _format);
	ARG("-o", _outputFileName);
	ARG("-e", _encoding);
	ARG("-va", _vertexAttribute);
	ARG("-ea", _edgeAttribute);

	ARG("-s", _dimensions);
	ARG("-vc", _vertexColor);
	ARG("-ec", _edgeColor);
	ARG("-vs", _vertexSize);
	ARG("-es", _edgeSize);
	ARG("-vf", _vertexFontSize);
	ARG("-ef", _edgeFontSize);
	ARG("-l", _legend);

	ARG("-nd", _nodeDistribution);
	ARG("-el", _edgeLength);
	ARG("-cr", _edgeCrossings);
	ARG("-it", _initTemp);
	ARG("-ft", _finalTemp);
	ARG("-cf", _coolFactor);
	ARG("-ns", _numSteps);

	SWITCH("-c", _coloredEdges);
	SWITCH("-d", _forceDirected);
	SWITCH("-u", _forceUndirected);

	return 0;
}

bool CLI::parseArguments()
{
	int inc, i;

	if (_argc == 1) {
		usage();
		return false;
	}

	for (i = 1; i < _argc; i++) {
		if (string(_argv[i]) == "-h" || string(_argv[i]) == "-help") {
			usage();
			return false;
		}
		if (string(_argv[i]) == "-v") {
			cout << APP_VERSION << endl;
			return false;
		}
	}

	for (i = 1; i < _argc; i += inc) {
		inc = argument(i);
		if (inc < 0) {
			if (i + 1 == _argc)
				cerr << "Missing argument value of: " << _argv[i] << endl;
			else
				cerr << "Invalid argument value: " << _argv[i + 1] << endl;
			return false;
		} else if (inc == 0)
			break;
	}

	if (i != _argc - 1) {
		cerr << "Invalid argument: " << _argv[i] << endl;
		return false;
	}

	_inputFileName = _argv[_argc - 1];

	return true;
}

bool CLI::setVertexAttribute()
{
	set<wstring> attributes;
	set<wstring>::iterator it;
	wstring attribute;
	Vertex *vtx;


	for (size_t i = 0; i < _graph->vertex_size(); i++) {
		vtx = _graph->vertex(i);
		for (map_iterator it = vtx->attributes().begin();
		  it != vtx->attributes().end(); it++)
			 attributes.insert(it->first);
	}

	if (_vertexAttribute.empty()) {
		if (attributes.empty())
			return true;
		attribute = *attributes.begin();
	} else {
		attribute = s2w(_vertexAttribute);
	}

	_graph->setVertexAttribute(attribute);

	return true;
}

bool CLI::setEdgeAttribute()
{
	set<wstring> attributes;
	set<wstring>::iterator it;
	wstring attribute;
	Edge *edg;


	for (size_t i = 0; i < _graph->edge_size(); i++) {
		edg = _graph->edge(i);
		for (map_iterator it = edg->attributes().begin();
		  it != edg->attributes().end(); it++) {
			attributes.insert(it->first);
		}
	}

	if (_edgeAttribute.empty()) {
		if (attributes.empty())
			return true;
		attribute = *attributes.begin();
	} else {
		attribute = s2w(_edgeAttribute);
	}

	_graph->setEdgeAttribute(attribute);

	return true;
}

bool CLI::setGraphProperties()
{
	if (!setVertexAttribute())
		return false;
	if (!setEdgeAttribute())
		return false;

	_graph->setDimensions(_dimensions);
	if (_forceDirected)
		_graph->setDirected(true);
	if (_forceUndirected)
		_graph->setDirected(false);

	_graph->setVertexColor(_vertexColor);
	_graph->setVertexSize(_vertexSize);

	_graph->setEdgeColor(_edgeColor);
	_graph->setEdgeSize(_edgeSize);
	_graph->setVertexFontSize(_vertexFontSize);
	_graph->setEdgeFontSize(_edgeFontSize);

	if (_legend)
		_coloredEdges = true;
	_graph->legend()->setSize(_legend);
	if (_coloredEdges)
		_graph->colorize();

	return true;
}

void CLI::setSAProperties()
{
	_sa->setNodeDistribution(_nodeDistribution);
	_sa->setEdgeLength(_edgeLength);
	_sa->setEdgeCrossings(_edgeCrossings);
	_sa->setInitTemp(_initTemp);
	_sa->setFinalTemp(_finalTemp);
	_sa->setCoolFactor(_coolFactor);
	_sa->setNumSteps(_numSteps);
}
