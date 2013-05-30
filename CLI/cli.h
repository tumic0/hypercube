#ifndef CLI_H_
#define CLI_H_

#include <string>
#include "CORE/color.h"
#include "CORE/coordinates.h"

class Graph;
class SA;
class Encoding;


class CLI
{
public:
	CLI(int argc, char *argv[]);
	~CLI();
	int exec();

private:
	bool readGraph();
	bool writeGraph();
	bool parseArguments();
	void setGraphProperties();
	void setSAProperties();
	int argument(int i);
	void usage();

	Graph *_graph;
	SA *_sa;

	int _argc;
	char **_argv;

	std::string _inputFileName;
	std::string _outputFileName;
	std::string _format;
	std::string _encoding;
	std::string _vertexAttribute;
	std::string _edgeAttribute;

	Coordinates _dimensions;
	Color _vertexColor;
	Color _edgeColor;
	int _vertexSize;
	int _edgeSize;
	int _vertexFontSize;
	int _edgeFontSize;
	int _legend;

	float _nodeDistribution;
	float _edgeLength;
	float _edgeCrossings;
	float _initTemp;
	float _finalTemp;
	float _coolFactor;
	int _numSteps;

	bool _coloredEdges;
	bool _forceDirected;
	bool _forceUndirected;
};

#endif // CLI_H_
