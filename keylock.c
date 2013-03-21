/*
  Much stolen shamelessly from baskerville's useful keydouble.
  (baskerville@lavabit.com)

  Makes any keycode a 'locking' keycode -- like capslock, one press
  turns it on (sends a keypress but no release) and another press
  turns it off (sends the release).

  s@diiq.org
 */

#define _BSD_SOURCE

#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <X11/Xlibint.h>
#include <X11/extensions/XTest.h>
#include <X11/extensions/record.h>
#include <X11/keysym.h>

#define NUM_KEYCODES 256

typedef enum { false, true } bool;

/* from libxnee */
typedef union {
    unsigned char type;
    xEvent event;
    xResourceReq req;
    xGenericReply reply;
    xError error;
    xConnSetupPrefix setup;
} XRecordDatum;

Display *input_display, *output_display;
XRecordContext reccontext;
XRecordRange *recrange;

bool locking_keycodes[NUM_KEYCODES];
bool running;

void load_output_display();
void load_input_display();
void evtcallback(XPointer priv, XRecordInterceptData *hook);
void stop(int sigint);
void die(const char *errstr, ...);
int main(int argc, char *argv[]);

void load_output_display()
{
    if (!(output_display = XOpenDisplay(NULL)))
        die("cannot open display\n");

    XSynchronize(output_display, true);
}

void load_input_display()
{
    XRecordClientSpec reccspec;
    int event, error, major, minor;

    if (!(input_display = XOpenDisplay(NULL)))
        die("cannot open display\n");

    if (!XTestQueryExtension(input_display, &event, &error, &major, &minor))
        die("the xtest extension is not loaded\n");

    if (!XRecordQueryVersion(input_display, &major, &minor))
        die("the record extension is not loaded\n");

    if (!(recrange = XRecordAllocRange()))
        die("could not alloc the record range object\n");

    recrange->device_events.first = KeyPress;
    recrange->device_events.last = KeyRelease;
    reccspec = XRecordAllClients;

    if (!(reccontext = XRecordCreateContext(input_display, 0, &reccspec, 1, &recrange, 1)))
        die("could not create a record context");

    if (!XRecordEnableContextAsync(input_display, reccontext, evtcallback, NULL))
        die("cannot enable record context\n");
}

void evtcallback(XPointer priv, XRecordInterceptData *hook)
{
    if (hook->category != XRecordFromServer) {
        XRecordFreeData(hook);
        return;
    }

    XRecordDatum *data = (XRecordDatum *) hook->data;

    static bool keys_depressed[NUM_KEYCODES];

    int code = data->event.u.u.detail;
    int evttype = data->event.u.u.type;

    if (evttype == KeyRelease) {
        /* If it's a code we're interested in locking... */
        if (locking_keycodes[code]) {
            /* ...and it's already been locked, then send the release
               on through, and unlock. */
            if (keys_depressed[code]) {
                XTestFakeKeyEvent(output_display, code, false, CurrentTime);
                keys_depressed[code] = false;

            /* Otherwise, send a press to follow, but note that it's
               been locked. */
            } else {
                keys_depressed[code] = true;
                XTestFakeKeyEvent(output_display, code, false, CurrentTime);
                XTestFakeKeyEvent(output_display, code, true, CurrentTime);
            }
        }
    }

    XRecordFreeData(hook);
}

void stop(int signum)
{
    running = false;
}

void die(const char *errstr, ...)
{
    va_list ap;
    va_start(ap, errstr);
    vfprintf(stderr, errstr, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int i;
    if (argc < 2)
        die("usage: %s keycode ...\n", argv[0]);
    for (i = 0; i < NUM_KEYCODES; i++)
        locking_keycodes[i] = false;
    for (i = 1; i < argc; i++)
        locking_keycodes[atoi(argv[i])] = true;

    signal(SIGINT, stop);
    signal(SIGTERM, stop);
    signal(SIGHUP, stop);

    load_output_display();
    load_input_display();

    running = true;
    while (running) {
        usleep(100000);
        XRecordProcessReplies(input_display);
    }

    XRecordDisableContext(output_display, reccontext);
    XRecordFreeContext(output_display, reccontext);
    XFlush(output_display);
    XFree(recrange);
    XCloseDisplay(input_display);
    XCloseDisplay(output_display);

    return 0;
}
