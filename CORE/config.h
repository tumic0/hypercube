#ifndef CONFIG_H_
#define CONFIG_H_

#define APP_VERSION				"0.1"
#define APP_ICON				":/icons/hypercube.png"

#define FONT_FAMILY				"Helvetica"
#define AVG_CHAR_WIDTH			0.66f

#define OPEN_FILE_ICON			":/icons/document-open.png"
#define SAVE_FILE_ICON			":/icons/document-save.png"
#define SAVE_AS_ICON			":/icons/document-save-as.png"
#define SAVE_ALL_ICON			":/icons/document-save-all.png"
#define CLOSE_FILE_ICON			":/icons/dialog-close.png"
#define TRANSFORM_GRAPH_ICON	":/icons/games-solve.png"
#define RELOAD_GRAPH_ICON		":/icons/view-refresh.png"
#define BIND_GRAPH_ICON			":/icons/document-encrypt.png"
#define PROJECT_GRAPH_ICON		":/icons/emblem-symbolic-link.png"

#ifdef SA_LOG_SUPPORT
#define SA_PROGRESS_LOG			"progress.log"
#define SA_OFFSET_LOG			"offset.log"
#define SA_ACCEPTANCE_LOG		"acceptance.log"
#endif

#define NODE_DISTRIBUTION		8000000
#define EDGE_LENGTH				4
#define EDGE_CROSSINGS			100000
#define INIT_TEMP				10000
#define FINAL_TEMP				100
#define COOL_FACTOR				0.75
#define NUM_STEPS				800

#define GRAPH_WIDTH				400
#define GRAPH_HEIGHT			400
#define EDGE_SIZE				1
#define VERTEX_SIZE				8
#define EDGE_COLOR				0x00FF00u
#define VERTEX_COLOR			0x000000u
#define VERTEX_FONT_SIZE		12
#define EDGE_FONT_SIZE			14
#define MIN_FONT_SIZE			8

#endif /* CONFIG_H_ */
