#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR_BUFFER_SIZE 512

#define min(a, b) (((a) < (b)) ? (a) : (b))

/* Uses the ##__VA_ARGS__ extension, but that's pretty much supported everywhere now... I think. */
#define error(fmt, ...)                                                                            \
    do                                                                                             \
    {                                                                                              \
        fflush(stdout);                                                                            \
        fprintf(stderr, "\n" fmt "\n", ##__VA_ARGS__);                                             \
        exit(EXIT_FAILURE);                                                                        \
    } while (0)

typedef enum _variant_type
{
    VARIANT_TYPE_UNUSED, /* Marks end of file */
    VARIANT_TYPE_FLOAT,
    VARIANT_TYPE_STRING,
    VARIANT_TYPE_VEC2F,
    VARIANT_TYPE_VEC3F,
    VARIANT_TYPE_UINT,
    VARIANT_TYPE_ENTITY,    /* Unused */
    VARIANT_TYPE_COMPONENT, /* Unused */
    VARIANT_TYPE_RECTF,
    VARIANT_TYPE_INT
} variant_type;

/* Guarantee a full & successful file read. */
size_t freadf(void* ptr, size_t size, size_t n, FILE* fp)
{
    size_t read = fread(ptr, size, n, fp);
    if (read != n)
        error("Error %d reading from file", ferror(fp));
    return read;
}

/* Read a string from VariantDB file and write to stdout. */
void handle_str(FILE* fp)
{
    char buf[STR_BUFFER_SIZE];
    uint32_t avail = 0;
    freadf(&avail, 4, 1, fp);
    while (avail > 0)
    {
        uint32_t size = min(STR_BUFFER_SIZE - 1, avail);
        memset(buf, '\0', STR_BUFFER_SIZE);
        freadf(buf, size, 1, fp);
        printf("%s", buf);
        /* Since we use freadf (guaranteed full read) we don't need to check the actual amount of
         * bytes read here. */
        avail -= size;
    }
}

/* Read float(s) from VariantDB file and write to stdout. Argument n must not be higher than 4. */
void handle_floats(FILE* fp, uint8_t n)
{
    /* Only accept args 1,2,3,4 (for float,vec2f,vec3f,rectf). */
    assert(n <= 4);
    float f[4];
    memset(f, '\0', 16);
    freadf(&f, n * 4, 1, fp);
    for (size_t i = 0; i < n; ++i)
    {
        if (i > 0)
            printf(",");
        printf("%.3f", f[i]);
    }
}

/* Read (u)int from VariantDB file and write to stdout. */
void handle_int(FILE* fp, int is_signed)
{
    uint32_t k = 0;
    freadf(&k, 4, 1, fp);
    printf(is_signed ? "%d" : "%u", k);
}

int main(int argc, char** argv)
{
    if (argc < 2)
        error("Input file path not provided. Usage: %s <path>", argv[0]);
    FILE* fp = fopen(argv[1], "rb");
    if (fp == NULL)
        error("Error opening file %s for reading", argv[1]);
    /* VariantDB files begin with a 4-byte version header, which should be equal to 1. */
    int32_t ver = 0;
    freadf(&ver, 4, 1, fp);
    if (ver != 1)
        error("Invalid version header, file is possibly corrupted");
    /* After that it's just a sequence of encoded key-value pairs. */
    while (!feof(fp))
    {
        /* Each key-value pair is prepended with a 4-byte value datatype specifier. If variant is of
         * type UNUSED, that means we've reached the end of our sequence. */
        uint32_t type = VARIANT_TYPE_UNUSED;
        freadf(&type, 4, 1, fp);
        if (type == VARIANT_TYPE_UNUSED)
            break;
        /* Next there's the key (string) and value (depends on type). */
        handle_str(fp);
        printf("|");
        switch (type)
        {
        case VARIANT_TYPE_FLOAT:
            handle_floats(fp, 1);
            break;
        case VARIANT_TYPE_STRING:
            handle_str(fp);
            break;
        case VARIANT_TYPE_VEC2F:
            handle_floats(fp, 2);
            break;
        case VARIANT_TYPE_VEC3F:
            handle_floats(fp, 3);
            break;
        case VARIANT_TYPE_UINT:
            handle_int(fp, 0);
            break;
        case VARIANT_TYPE_RECTF:
            handle_floats(fp, 4);
            break;
        case VARIANT_TYPE_INT:
            handle_int(fp, 1);
            break;
        default:
            error("Unknown variant type %d", type);
        }
        printf("\n");
    }
    return EXIT_SUCCESS;
}