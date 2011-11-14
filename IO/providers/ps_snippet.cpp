#include "ps_snippet.h"
#include "IO/providers/ps_snippets/default.h"
#include "IO/providers/ps_snippets/ce.h"
#include "IO/providers/ps_snippets/cyrillic.h"

PsSnippet* snippets[] = {
	new Default,
	new CE,
	new Cyrillic,
	0
};
