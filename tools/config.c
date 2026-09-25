#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

struct entry {
	char key[128];
	char value[128];
	struct entry *next;
};
static struct entry *first, *last;

/* run application */
static int run(void) {

	FILE *fp = fopen("./config.mk", "rb");
	if (!fp) {

		fprintf(stderr, "Can't open file './config.mk': %s\n", strerror(errno));
		return 1;
	}
	char buffer[256] = {0};
	char key[128], value[128];

	while (!feof(fp) && fgets(buffer, sizeof(buffer), fp)) {

		size_t len = strlen(buffer);
		if (len && buffer[len-1] == '\n')
			buffer[--len] = 0;

		/* read entry */
		if (sscanf(buffer, "%128[^=]=%128s", key, value) != 2)
			continue;

		struct entry *entry = (struct entry *)
			malloc(sizeof(struct entry));
		memset(entry, 0, sizeof(struct entry));

		memcpy(entry->key, key, sizeof(key));
		memcpy(entry->value, value, sizeof(value));

		if (!first) first = entry;
		if (last) last->next = entry;
		last = entry;
	}
	fclose(fp);

	/* write config.h */
	fp = fopen("build/include/rook/config.h", "wb");
	if (!fp) {

		fprintf(stderr, "Can't open file 'build/include/rook/config.h': %s\n", strerror(errno));
		return 1;
	}
	fprintf(fp, "#ifndef ROOK_CONFIG_H\n#define ROOK_CONFIG_H\n\n");

	struct entry *entry = first;
	for (; entry; entry = entry->next)
		fprintf(fp, "#define RK_CONFIG_%s %s\n", entry->key, entry->value);

	fprintf(fp, "\n#endif /* ROOK_CONFIG_H */\n");
	fclose(fp);

	/* write config.sh */
	fp = fopen("build/config.sh", "wb");
	if (!fp) {

		fprintf(stderr, "Can't open file 'build/config.sh': %s\n", strerror(errno));
		return 1;
	}
	entry = first;
	for (; entry; entry = entry->next)
		fprintf(fp, "CONFIG_%s='%s'\n", entry->key, entry->value);

	fclose(fp);

	return 0;
}

/* clean up resources */
static void cleanup(void) {

	struct entry *entry = first;
	while (entry) {

		struct entry *next = entry->next;
		free(entry);
		entry = next;
	}
}

int main() {

	int code = run();
	cleanup();
	return code;
}
