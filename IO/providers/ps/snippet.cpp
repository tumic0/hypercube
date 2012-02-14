#include "snippet.h"
#include "default.h"
#include "ce.h"
#include "cyrillic.h"


PsSnippet* snippets[] = {
	new Default,
	new CE,
	new Cyrillic,
	0
};
