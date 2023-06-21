#include "gpio.h"
#include "libyocto_config.h"
#include "logger.h"

#include <assert.h>

#if defined(LIBYOCTO_DIGITAL_INPUT_GPIO) || defined(LIBYOCTO_DIGITAL_OUTPUT_GPIO)
#include <gpiod.h>

#if defined(RBOX630)
// Digital input order: 151, 36, 12, 13, 37, 152, 150, 153

struct gpiod_chip *gChips[3] = {NULL};
struct gpiod_line_bulk gInputs[3];

#define GPIO_INPUT0_COUNT 2
#define GPIO_INPUT1_COUNT 2
#define GPIO_INPUT2_COUNT 4

#elif defined(RADIPV3)
struct gpiod_chip *gChips[1] = {NULL};
struct gpiod_line_bulk gInputs[2];
struct gpiod_line_bulk gOutputs[2];

#define GPIO_INPUT0_COUNT 2 //TODO Put in arrays, move to libyocto_config?
#define GPIO_INPUT1_COUNT 1

#define GPIO_OUTPUT0_COUNT 4
#define GPIO_OUTPUT1_COUNT 1

#endif // RBOX630

#ifdef LIBYOCTO_DIGITAL_INPUT_GPIO
static int _requestInput(struct gpiod_chip *chip, struct gpiod_line_bulk *bulk, unsigned int *offsets, unsigned int numOffset, int flags)
{
    gpiod_line_bulk_init(bulk);

    if (gpiod_chip_get_lines(chip, offsets, numOffset, bulk) < 0)
    {
        LOGGER(LOGGER_LEVEL_ERROR, "Error getting lines for %s, offset %d", gpiod_chip_name(chip), numOffset);
        return -1;
    }

    if (gpiod_line_request_bulk_input_flags(bulk, "yocto_in", flags) < 0)
    {
        LOGGER(LOGGER_LEVEL_ERROR, "Error requesting lines for %s, offset %d", gpiod_chip_name(chip), numOffset);
        return -1;
    }

    return 0;
}
#endif  //LIBYOCTO_DIGITAL_INPUT_GPIO

#ifdef LIBYOCTO_DIGITAL_OUTPUT_GPIO
static int _requestOutput(struct gpiod_chip *chip, struct gpiod_line_bulk *bulk, unsigned int *offsets, unsigned int numOffset, int flags, const int *defaults)
{
    gpiod_line_bulk_init(bulk);

    if (gpiod_chip_get_lines(chip, offsets, numOffset, bulk) < 0)
    {
        LOGGER(LOGGER_LEVEL_ERROR, "Error getting lines for %s, offset %d", gpiod_chip_name(chip), numOffset);
        return -1;
    }

    if (gpiod_line_request_bulk_output_flags(bulk, "yocto_out", flags, defaults) < 0)
    {
        LOGGER(LOGGER_LEVEL_ERROR, "Error requesting lines for %s, offset %d", gpiod_chip_name(chip), numOffset);
        return -1;
    }

    return 0;
}
#endif  //LIBYOCTO_DIGITAL_OUTPUT_GPIO

int gpioInit()
{
#if defined(RBOX630)
    if ((gChips[0] = gpiod_chip_open("/dev/gpiochip0")) == NULL)
    {
        LOGGER(LOGGER_LEVEL_ERROR, "Error opening gpiochip0");
        goto error;
    }
    if ((gChips[1] = gpiod_chip_open("/dev/gpiochip1")) == NULL)
    {
        LOGGER(LOGGER_LEVEL_ERROR, "Error opening gpiochip1");
        goto error;
    }
    if ((gChips[2] = gpiod_chip_open("/dev/gpiochip4")) == NULL)
    {
        LOGGER(LOGGER_LEVEL_ERROR, "Error opening gpiochip4");
        goto error;
    }

    unsigned int offsets0[GPIO_INPUT0_COUNT] = {12, 13};
    if (_requestInput(gChips[0], &gInputs[0], offsets0, GPIO_INPUT0_COUNT, GPIOD_LINE_REQUEST_FLAG_ACTIVE_LOW) < 0)
    {
        goto error;
    }

    unsigned int offsets1[GPIO_INPUT1_COUNT] = {4, 5};  //{36, 37}
    if (_requestInput(gChips[1], &gInputs[1], offsets1, GPIO_INPUT1_COUNT, GPIOD_LINE_REQUEST_FLAG_ACTIVE_LOW) < 0)
    {
        goto error;
    }

    unsigned int offsets2[GPIO_INPUT2_COUNT] = {22, 23, 24, 25}; //{150, 151, 152, 153}
    if (_requestInput(gChips[2], &gInputs[2], offsets2, GPIO_INPUT2_COUNT, GPIOD_LINE_REQUEST_FLAG_ACTIVE_LOW) < 0)
    {
        goto error;
    }
#elif defined(RADIPV3)
    if ((gChips[0] = gpiod_chip_open("/dev/gpiochip3")) == NULL)
    {
        LOGGER(LOGGER_LEVEL_ERROR, "Error opening gpiochip3");
        goto error;
    }

    unsigned int offsetsInput0[GPIO_INPUT0_COUNT] = {24, 25};
    if (_requestInput(gChips[0], &gInputs[0], offsetsInput0, GPIO_INPUT0_COUNT, 0) < 0)
    {
        goto error;
    }

    unsigned int offsetsInput1[GPIO_INPUT1_COUNT] = {26};
    if (_requestInput(gChips[0], &gInputs[1], offsetsInput1, GPIO_INPUT1_COUNT, GPIOD_LINE_REQUEST_FLAG_ACTIVE_LOW) < 0)
    {
        goto error;
    }

    unsigned int offsetsOutput0[GPIO_OUTPUT0_COUNT] = {17, 18, 19, 20};
    int defaultsOutput0[GPIO_OUTPUT0_COUNT] = {0, 0, 0, 1};
    if (_requestOutput(gChips[0], &gOutputs[0], offsetsOutput0, GPIO_OUTPUT0_COUNT, 0, defaultsOutput0) < 0)
    {
        goto error;
    }

    unsigned int offsetsOutput1[GPIO_OUTPUT1_COUNT] = {21};
    int defaultsOutput1[GPIO_OUTPUT1_COUNT] = {1};
    if (_requestOutput(gChips[0], &gOutputs[1], offsetsOutput1, GPIO_OUTPUT1_COUNT, GPIOD_LINE_REQUEST_FLAG_ACTIVE_LOW, defaultsOutput1) < 0)
    {
        goto error;
    }
#endif // RADIPV3

    return 0;

error:
    gpioUninit();
    return -1;
}

static void freeBulk(struct gpiod_line_bulk bulks[], size_t count)
{
    for (size_t index = 0; index < count; index++)
    {
        struct gpiod_line_bulk *bulk = &bulks[index];

        int lineCount = gpiod_line_bulk_num_lines(bulk);
        LOGGER(LOGGER_LEVEL_DEBUG, "Freeing %d gpio bulk lines", lineCount);

        if (lineCount > 0)
        {
            gpiod_line_release_bulk(bulk);
        }
    }
}

void gpioUninit()
{
    // Free bulks
#ifdef LIBYOCTO_DIGITAL_INPUT_GPIO
    freeBulk(gInputs, sizeof(gInputs) / sizeof(gInputs[0]));
#endif
#ifdef LIBYOCTO_DIGITAL_OUTPUT_GPIO
    freeBulk(gOutputs, sizeof(gOutputs) / sizeof(gOutputs[0]));
#endif

    // Free chips
    for (size_t index = 0; index < sizeof(gChips) / sizeof(gChips[0]); index++)
    {
        assert(gChips[index] != NULL);

        LOGGER(LOGGER_LEVEL_DEBUG, "Freeing gpio chip at index %d", index);
        gpiod_chip_close(gChips[index]);
        gChips[index] = NULL;
    }
}

static int _getData(struct gpiod_line_bulk *bulk, __u8 *data, int offset)
{
    int lineCount = gpiod_line_bulk_num_lines(bulk);

    assert(lineCount + offset <= 8);

    if (lineCount == 0)
    {
        LOGGER(LOGGER_LEVEL_ERROR, "No gpio lines reserved");
        return -1;
    }

    int values[lineCount];
    if (gpiod_line_get_value_bulk(bulk, values) < 0)
    {
        LOGGER(LOGGER_LEVEL_ERROR, "Error reading gpio line");
        return -1;
    }

    // Transform value into data
    for (int index = 0; index < lineCount; index++)
    {
        *data |= values[index] << (index + offset);
    }

    return 0;
}
#endif // LIBYOCTO_DIGITAL_INPUT_GPIO || LIBYOCTO_DIGITAL_OUTPUT_GPIO

#ifdef LIBYOCTO_DIGITAL_INPUT_GPIO
int gpioInputReadAll(__u8 *data)
{
    *data = 0;

#if defined(RBOX630)
    if (_getData(&gInputs[0], data, 0) < 0)
    {
        return -1;
    }

    if (_getData(&gInputs[1], data, GPIO_INPUT0_COUNT) < 0)
    {
        return -1;
    }

    if (_getData(&gInputs[2], data, GPIO_INPUT0_COUNT + GPIO_INPUT1_COUNT) < 0)
    {
        return -1;
    }

    //Reorder the data
    *data = \
        ((*data & 0x01) << 2) | //12    -> 0x04
        ((*data & 0x02) << 2) | //13    -> 0x08
        ((*data & 0x04) >> 1) | //36    -> 0x02
        ((*data & 0x08) << 1) | //37    -> 0x10
        ((*data & 0x10) << 2) | //150   -> 0x40
        ((*data & 0x20) >> 5) | //151   -> 0x01
        ((*data & 0x40) >> 1) | //152   -> 0x20
        ((*data & 0x80));       //153   -> 0x80

#elif defined(RADIPV3)
    if (_getData(&gInputs[0], data, 0) < 0)
    {
        return -1;
    }

    if (_getData(&gInputs[1], data, GPIO_INPUT0_COUNT) < 0)
    {
        return -1;
    }
#endif // RADIPV3

    return 0;
}

int gpioInputRead(int index, __u8 *on)
{
    assert(index >= 0 && index < LIBYOCTO_DIGITAL_INPUT_COUNT);

    if (gpioInputReadAll(on) < 0)
    {
        return -1;
    }

    *on = (*on >> index) & 0x01;

    return 0;
}
#endif // LIBYOCTO_DIGITAL_INPUT_GPIO

#ifdef LIBYOCTO_DIGITAL_OUTPUT_GPIO
int gpioOutputReadAll(__u8 *data)
{
    *data = 0;

#ifdef RADIPV3
    if (_getData(&gOutputs[0], data, 0) < 0)
    {
        return -1;
    }

    if (_getData(&gOutputs[1], data, GPIO_OUTPUT0_COUNT) < 0)
    {
        return -1;
    }
#endif // RADIPV3

    return 0;
}

int gpioOutputRead(int index, __u8 *on)
{
    assert(index >= 0 && index < LIBYOCTO_DIGITAL_OUTPUT_COUNT);

    if (gpioOutputReadAll(on) < 0)
    {
        return -1;
    }

    *on = (*on >> index) & 0x01;

    return 0;
}

static int _writeData(struct gpiod_line_bulk *bulk, __u8 data, int offset)
{
    int lineCount = gpiod_line_bulk_num_lines(bulk);
    assert(lineCount + offset <= 8);

    if (lineCount == 0)
    {
        LOGGER(LOGGER_LEVEL_ERROR, "No gpio lines reserved");
        return -1;
    }

    int values[lineCount];
    for (int index = 0; index < lineCount; index++)
    {
        values[index] = (data >> (index + offset)) & 0x01;
    }

    if (gpiod_line_set_value_bulk(bulk, values) < 0)
    {
        LOGGER(LOGGER_LEVEL_ERROR, "Error reading gpio line");
        return -1;
    }

    return 0;
}

int gpioOutputWriteAll(__u8 data)
{
#ifdef RADIPV3
    if (_writeData(&gOutputs[0], data, 0) < 0)
    {
        return -1;
    }

    if (_writeData(&gOutputs[1], data, GPIO_OUTPUT0_COUNT) < 0)
    {
        return -1;
    }
#endif // RADIPV3

    return 0;
}

int gpioOutputWrite(int index, __u8 on)
{
    assert(index >= 0 && index < LIBYOCTO_DIGITAL_OUTPUT_COUNT);

    __u8 data;
    if (gpioOutputReadAll(&data) < 0)
    {
        return -1;
    }

    if (on)
    {
        data |= 0x01 << index;
    }
    else
    {
        data &= ~(0x01 << index);
    }

    if (gpioOutputWriteAll(data) < 0)
    {
        return -1;
    }

    return 0;
}
#endif // LIBYOCTO_DIGITAL_OUTPUT_GPIO
