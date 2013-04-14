#ifndef MISC_H_
#define MISC_H_

#define MAX(a,b) \
	(((a) > (b)) ? (a) : (b))
#define MIN(a,b) \
	(((a) < (b)) ? (a) : (b))

#define ARRAY_SIZE(array) \
	(sizeof(array) / sizeof((array)[0]))

#endif /* MISC_H_ */
