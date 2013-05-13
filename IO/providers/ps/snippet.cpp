#include "snippet.h"
#include "western.h"
#include "ce.h"
#include "cyrillic.h"
#include "greek.h"


PsSnippet* snippets[] = {
	new Western,
	new CE,
	new Cyrillic,
	new Greek,
	0
};
