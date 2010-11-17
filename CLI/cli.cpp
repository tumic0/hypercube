#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include "CORE/graph.h"
#include "CORE/sa.h"
#include "CORE/config.h"
#include "IO/io.h"
#include "cli.h"

using namespace std;


static char *replaceExtension(const char *fileName, const char *ext);


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

	_nodeDistribution = NODE_DISTRIBUTION;
	_edgeLength = EDGE_LENGTH;
	_edgeCrossings = EDGE_CROSSINGS;
	_initTemp = INIT_TEMP;
	_finalTemp = FINAL_TEMP;
	_coolFactor = COOL_FACTOR;
	_numSteps = NUM_STEPS;
}

int CLI::exec()
{
	if (!parseArguments())
		return EXIT_FAILURE;

	if (!readGraph())
		return EXIT_FAILURE;

	setGraphProperties();
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
	InputProvider **p = inputProviders;
	IO::Error error = IO::FormatError;

	while (*p) {
		error = (*p)->readGraph(_graph, _inputFileName.c_str());
		if (error != IO::FormatError)
			break;
		p++;
	}

	if (error) {
		if (error == IO::FormatError)
			cerr << "Unknown/invalid input format!" << endl;
		else if (error == IO::OpenError)
			cerr << "Error opening input file!" << endl;
		else if (error == IO::ReadError)
			cerr << "Error reading input file!" << endl;
		return false;
	}

	return true;
}

bool CLI::writeGraph()
{
	OutputProvider **p = outputProviders;
	IO::Error error;
	int found = 0;

	if (_format == string())
		_format = (char *)(*p)->type();
	if (_outputFileName == string())
		_outputFileName = string(replaceExtension(_inputFileName.c_str(),
		  _format.c_str()));

	while (*p) {
		if ((found = !strcmp(_format.c_str(), (*p)->type())))
			break;
		p++;
	}

	if (!found) {
		cerr << "Unknown output format: " << _format << endl;
		return false;
	}

	error = (*p)->writeGraph(_graph, _outputFileName.c_str());

	if (error) {
		if (error == IO::OpenError)
			cerr << "Error opening output file!" << endl;
		else if (error == IO::WriteError)
			cerr << "Error writing output file!" << endl;
		return false;
	}

	return true;
}

void CLI::usage()
{
	cout << "Usage:    hypercube-cli [OPTIONS] FILE" << endl;
	cout << endl;
	cout << "OPTIONS:" << endl;
	cout << " -s <dimensions>  set image size to <dimensions>" << endl;
	cout << " -f <format>      set output format to <format>" << endl;
	cout << " -o <file>        set the output file to <file>" << endl;
	cout << endl;
	cout << " -vc <color>      set vertex color to <color>" << endl;
	cout << " -ec <color>      set edge color to <color>" << endl;
	cout << " -vs <size>       set vertex size to <size>" << endl;
	cout << " -es <size>       set edge size to <size>" << endl;
	cout << " -vf <size>       set vertex font size to <size>" << endl;
	cout << " -ef <size>       set edge font size to <size>" << endl;
	cout << endl;
	cout << "option arguments:" << endl;
	cout << " <dimesnsions>    width,height" << endl;
	cout << " <color>          #RRGGBB" << endl;
	cout << " <format>         supported formats: ";

	OutputProvider **p = outputProviders;
	while (*p) {
		cout << (*p)->type() << " ";
		p++;
	}

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

	ARG("-f", _format);
	ARG("-o", _outputFileName);

	ARG("-s", _dimensions);
	ARG("-vc", _vertexColor);
	ARG("-ec", _edgeColor);
	ARG("-vs", _vertexSize);
	ARG("-es", _edgeSize);
	ARG("-vf", _vertexFontSize);
	ARG("-ef", _edgeFontSize);

	ARG("-nd", _nodeDistribution);
	ARG("-el", _edgeLength);
	ARG("-cr", _edgeCrossings);
	ARG("-it", _initTemp);
	ARG("-ft", _finalTemp);
	ARG("-cf", _coolFactor);
	ARG("-ns", _numSteps);

	return 0;
}

bool CLI::parseArguments()
{
	int inc, i;

	if (_argc == 1 || (_argc == 2 && string(_argv[1]) == "-h")) {
		usage();
		return false;
	}

	i = 1;
	while (i < _argc) {
		inc = argument(i);
		if (inc < 0) {
			if (i + 1 == _argc)
				cerr << "Missing argument value of: " << _argv[i] << endl;
			else
				cerr << "Invalid argument value: " << _argv[i + 1] << endl;
			return false;
		} else if (inc == 0)
			break;
		i += inc;
	}

	if (i != _argc - 1) {
		cerr << "Invalid argument: " << _argv[i] << endl;
		return false;
	}

	_inputFileName = _argv[_argc - 1];

	return true;
}

void CLI::setGraphProperties()
{
	_graph->setDimensions(_dimensions);

	_graph->setVertexColor(_vertexColor);
	_graph->setVertexSize(_vertexSize);
	_graph->setEdgeColor(_edgeColor);
	_graph->setEdgeSize(_edgeSize);
	_graph->setVertexFontSize(_vertexFontSize);
	_graph->setEdgeFontSize(_edgeFontSize);
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

char *replaceExtension(const char *fileName, const char *ext)
{
	int len;
	const char *cp;
	char *res;

	cp = strrchr(fileName, '.');
	len = (cp == NULL) ? strlen(fileName) : (cp - fileName);

	res = new char[len + strlen(ext) + 1];
	memcpy(res, fileName, len);
	res[len] = '.';
	memcpy(res + len + 1, ext, strlen(ext) + 1);

	return res;
}
