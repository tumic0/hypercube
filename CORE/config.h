#ifndef CONFIG_H_
#define CONFIG_H_

#define APP_HOMEPAGE       "http://tumic.wz.cz/hypercube"
#define APP_VERSION        "1.5.2"

#define FONT_FAMILY        "Helvetica"
#define AVG_CHAR_WIDTH     0.58f
#define DESCENT_RATIO      0.1f

#define LEGEND_MARGIN      10
#define LEGEND_RECT_RATIO  1.33f

#define NODE_DISTRIBUTION  8e6
#define EDGE_LENGTH        1
#define EDGE_CROSSINGS     1e4
#define INIT_TEMP          2000
#define FINAL_TEMP         20
#define COOL_FACTOR        0.75
#define NUM_STEPS          600

#define GRAPH_WIDTH        400
#define GRAPH_HEIGHT       300
#define EDGE_SIZE          2
#define VERTEX_SIZE        9
#define EDGE_COLOR         0xAAFF00u
#define VERTEX_COLOR       0x000000u
#define VERTEX_FONT_SIZE   14
#define EDGE_FONT_SIZE     14
#define MIN_FONT_SIZE      8

#define NODE_LABEL_ATTR    "label"
#define EDGE_LABEL_ATTR    "label"

#ifdef SA_LOG_SUPPORT
#define SA_PROGRESS_LOG    "progress.log"
#define SA_OFFSET_LOG      "offset.log"
#define SA_ACCEPTANCE_LOG  "acceptance.log"
#endif

#endif /* CONFIG_H_ */
