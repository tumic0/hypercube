#ifndef CONFIG_H_
#define CONFIG_H_

#define APP_HOMEPAGE			"http://tumic.wz.cz/hypercube"
#define APP_VERSION				"1.1.0"

#define FONT_FAMILY				"Helvetica"
#define AVG_CHAR_WIDTH			0.58f

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
