#include <zephyr/shell/shell.h>
#include <zephyr/drivers/sensor.h>
#include <our_driver/our_driver.h>
#include <ctype.h>

#define MAX_CMD_CNT (20u)
#define MAX_CMD_LEN (33u)

/* buffer holding dynamically created user commands */
static char dynamic_cmd_buffer[MAX_CMD_CNT][MAX_CMD_LEN];
/* commands counter */
static uint8_t dynamic_cmd_cnt;

typedef int cmp_t(const void *, const void *);
extern void qsort(void *a, size_t n, size_t es, cmp_t *cmp);

/* function required by qsort */
static int string_cmp(const void *p_a, const void *p_b)
{
	return strcmp((const char *)p_a, (const char *)p_b);
}

static int cmd_dynamic_set(const struct shell *sh,
			   size_t argc, char **argv)
{
	uint16_t cmd_len;
	uint8_t idx;

	ARG_UNUSED(argc);

	if (dynamic_cmd_cnt >= MAX_CMD_CNT) {
		shell_error(sh, "command limit reached");
		return -ENOEXEC;
	}

	cmd_len = strlen(argv[1]);

	if (cmd_len >= MAX_CMD_LEN) {
		shell_error(sh, "too long command");
		return -ENOEXEC;
	}

	for (idx = 0U; idx < cmd_len; idx++) {
		if (isalnum((int)(argv[1][idx])) == 0) {
			shell_error(sh,
				    "bad command name - please use only"
				    " alphanumerical characters");
			return -ENOEXEC;
		}
	}

	for (idx = 0U; idx < MAX_CMD_CNT; idx++) {
		if (!strcmp(dynamic_cmd_buffer[idx], argv[1])) {
			shell_error(sh, "duplicated command");
			return -ENOEXEC;
		}
	}

	sprintf(dynamic_cmd_buffer[dynamic_cmd_cnt++], "%s", argv[1]);

	qsort(dynamic_cmd_buffer, dynamic_cmd_cnt,
	      sizeof(dynamic_cmd_buffer[0]), string_cmp);

	shell_print(sh, "command added successfully");

	return 0;
}



static int cmd_channel_get_handler(const struct shell* sh, int arg, char** argv)
{
    const struct device* dev = DEVICE_DT_GET(DT_NODELABEL(our_driver0));
    if (!dev){
        shell_error(sh, "Couldn't find device %s", argv[1]);
        return -EFAULT;
    }
    struct sensor_value val;
    int ret = sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &val);
    if (ret){
        shell_error(sh, "Couldn't get channel, got this %d", ret);
        return -EFAULT;
    }

    struct our_driver_data *driver_data = dev->data;

    shell_info(sh, "%d", driver_data->counter);
    return 0;
}

static int cmd_sample_fetch_handler(const struct shell* sh, int arg, char** argv)
{
    const struct device* dev = DEVICE_DT_GET(DT_NODELABEL(our_driver0));
    if (!dev){
        shell_error(sh, "Couldn't find device %s", argv[1]);
        return -EFAULT;
    }
    int ret = sensor_sample_fetch(dev);
    if (ret){
        shell_error(sh, "Couldn't get channel, got this %d", ret);
        return -EFAULT;
    }
    return 0;
}

static int cmd_info_handler(const struct shell* sh, int arg, char** argv)
{
    const struct device* dev = DEVICE_DT_GET(DT_NODELABEL(our_driver0));
    if (!device_is_ready(dev)){
        shell_error(sh, "Couldn't find device %s", argv[1]);
        return -EFAULT;
    }
    shell_info(sh, "Device our_driver0 Initialized");
    return 0;
}

static int cmd_dynamic_show(const struct shell *sh,
			    size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	if (dynamic_cmd_cnt == 0U) {
		shell_warn(sh, "Please add some commands first.");
		return -ENOEXEC;
	}

	shell_print(sh, "Dynamic command list:");

	for (uint8_t i = 0; i < dynamic_cmd_cnt; i++) {
		shell_print(sh, "[%3d] %s", i, dynamic_cmd_buffer[i]);
	}

	return 0;
}



SHELL_STATIC_SUBCMD_SET_CREATE(
    sensor_subcmd,
    SHELL_CMD_ARG(read, NULL, "Get channel for device", cmd_channel_get_handler, 1, 0),
    SHELL_CMD_ARG(fetch, NULL, "Fetch data", cmd_sample_fetch_handler, 1, 0),
    SHELL_CMD_ARG(info, NULL, "About device", cmd_info_handler, 1, 0),
    SHELL_CMD_ARG(set, NULL,
		"Add a new dynamic command.\nExample usage: [ dynamic add test "
		"] will add a dynamic command 'test'.\nIn this example, command"
		" name length is limited to 32 chars. You can add up to 20"
		" commands. Commands are automatically sorted to ensure correct"
		" shell completion.",
		cmd_dynamic_set, 2, 0),
	SHELL_CMD_ARG(show, NULL,
		"Show all added dynamic commands.", cmd_dynamic_show, 1, 0),
    SHELL_SUBCMD_SET_END,
);

SHELL_CMD_REGISTER(sensor, &sensor_subcmd, "sensor subcommands", NULL);


