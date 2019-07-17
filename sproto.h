#ifndef sproto_h
#define sproto_h

#include <stddef.h>

#define SPROTO_TARRAY 0x80
#define CHUNK_SIZE 1000
#define SIZEOF_LENGTH 4
#define SIZEOF_HEADER 2
#define SIZEOF_FIELD 2
#define SIZEOF_INT64 ((int)sizeof(uint64_t))
#define SIZEOF_INT32 ((int)sizeof(uint32_t))

struct field {
	int tag;
	int type;
	const char * name;
	struct sproto_type * st;
	int key;
	int extra;
};

struct sproto_type {
	const char * name;
	int n;
	int base;
	int maxn;
	struct field *f;
};

struct protocol {
	const char *name;
	int tag;
	int confirm;	// confirm == 1 where response nil
	struct sproto_type * p[2];
};

struct chunk {
	struct chunk * next;
};

struct pool {
	struct chunk * header;
	struct chunk * current;
	int current_used;
};

struct sproto {
	struct pool memory;
	int type_n;
	int protocol_n;
	struct sproto_type * type;
	struct protocol * proto;
};

#define SPROTO_REQUEST 0
#define SPROTO_RESPONSE 1

// type (sproto_arg.type)
#define SPROTO_TINTEGER 0
#define SPROTO_TBOOLEAN 1
#define SPROTO_TSTRING 2
#define SPROTO_TSTRUCT 3

// sub type of string (sproto_arg.extra)
#define SPROTO_TSTRING_STRING 0
#define SPROTO_TSTRING_BINARY 1

#define SPROTO_CB_ERROR -1
#define SPROTO_CB_NIL -2
#define SPROTO_CB_NOARRAY -3

struct sproto * sproto_create(const void * proto, size_t sz);
void sproto_release(struct sproto *);

int sproto_prototag(const struct sproto *, const char * name);
const char * sproto_protoname(const struct sproto *, int proto);
// SPROTO_REQUEST(0) : request, SPROTO_RESPONSE(1): response
struct sproto_type * sproto_protoquery(const struct sproto *, int proto, int what);
int sproto_protoresponse(const struct sproto *, int proto);

struct sproto_type * sproto_type(const struct sproto *, const char * type_name);

int sproto_pack(const void * src, int srcsz, void * buffer, int bufsz);
int sproto_unpack(const void * src, int srcsz, void * buffer, int bufsz);

struct sproto_arg {
	void *ud;
	const char *tagname;
	int tagid;
	int type;
	struct sproto_type *subtype;
	void *value;
	int length;
	int index;	// array base 1
	int mainindex;	// for map
	int extra; // SPROTO_TINTEGER: decimal ; SPROTO_TSTRING 0:utf8 string 1:binary
};

typedef int (*sproto_callback)(const struct sproto_arg *args);

int sproto_decode(const struct sproto_type *, const void * data, int size, sproto_callback cb, void *ud);
int sproto_encode(const struct sproto_type *, void * buffer, int size, sproto_callback cb, void *ud);

// for debug use
void sproto_dump(struct sproto *);
const char * sproto_name(struct sproto_type *);

#endif
