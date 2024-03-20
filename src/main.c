#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <euicc/interface.h>
#include <euicc/euicc.h>
#include <driver.h>

#include "applet.h"
#include "applet/chip.h"
#include "applet/profile.h"
#include "applet/notification.h"
#include "applet/version.h"

static struct applet_entry applet_apdu = {
    .name = "apdu",
    .main = NULL,
};
static struct applet_entry applet_http = {
    .name = "http",
    .main = NULL,
};

static int driver_applet_main(int argc, char **argv)
{
    applet_apdu.main = euicc_driver_main_apdu;
    applet_http.main = euicc_driver_main_http;
    static const struct applet_entry *applets[] = {
        &applet_apdu,
        &applet_http,
        NULL,
    };
    return applet_entry(argc, argv, applets);
}

struct applet_entry driver_applet = {
    .name = "driver",
    .main = driver_applet_main,
};

static const struct applet_entry *applets[] = {
    &driver_applet,
    &applet_chip,
    &applet_profile,
    &applet_notification,
    &applet_version,
    NULL,
};

static int euicc_ctx_inited = 0;
struct euicc_ctx euicc_ctx = {0};

void main_init_euicc()
{
    if (euicc_init(&euicc_ctx))
    {
        jprint_error("euicc_init", NULL);
        exit(-1);
    }
    euicc_ctx_inited = 1;
}

void main_fini_euicc()
{
    if (!euicc_ctx_inited)
    {
        return;
    }
    euicc_fini(&euicc_ctx);
    euicc_ctx_inited = 0;
}

int main(int argc, char **argv)
{
    int ret = 0;

    memset(&euicc_ctx, 0, sizeof(euicc_ctx));

    if (euicc_driver_init())
    {
        return -1;
    }

    euicc_ctx.apdu.interface = &euicc_driver_interface_apdu;
    euicc_ctx.http.interface = &euicc_driver_interface_http;

    ret = applet_entry(argc, argv, applets);

    main_fini_euicc();

    euicc_driver_fini();

    return ret;
}
