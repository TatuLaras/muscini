#include "jack_init.h"
#include "analyze.h"

#include <jack/jack.h>
#include <jack/midiport.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

jack_port_t *input_port;
jack_port_t *beat_midi_port;

jack_client_t *client;

static void signal_handler(int sig) {
    jack_client_close(client);
    fprintf(stderr, "signal received, exiting ...\n");
    exit(0);
    (void)sig;
}

int process(jack_nframes_t nframes, void *arg) {
    // Audio input
    jack_default_audio_sample_t *in =
        (jack_default_audio_sample_t *)jack_port_get_buffer(input_port,
                                                            nframes);
    analyze_feed_frames((float *)in, nframes, 1);

    // Midi events
    void *port_buf = jack_port_get_buffer(beat_midi_port, nframes);
    jack_midi_event_t in_event;
    jack_nframes_t event_count = jack_midi_get_event_count(port_buf);

    for (uint32_t i = 0; i < event_count; i++) {
        jack_midi_event_get(&in_event, port_buf, i);

        if ((in_event.buffer[0] & 0xf0) == 0x90) {
            analyze_trigger_beat();
            analyze_set_beat_triggering_mode(1);
        }
    }

    (void)arg;
    return 0;
}

void jack_shutdown(void *arg) {
    exit(1);
    (void)arg;
}

int jack_init(void) {
    const char *server_name = NULL;
    jack_options_t options = JackNullOption;

    const char *client_name = "muscini";

    jack_status_t status;
    client = jack_client_open(client_name, options, &status, server_name);

    if (client == NULL) {
        fprintf(stderr,
                "jack_client_open() failed, "
                "status = 0x%2.0x\n",
                status);
        if (status & JackServerFailed) {
            fprintf(stderr, "Unable to connect to JACK server\n");
        }
        return 1;
    }
    if (status & JackServerStarted) {
        fprintf(stderr, "JACK server started\n");
    }
    if (status & JackNameNotUnique) {
        client_name = jack_get_client_name(client);
        fprintf(stderr, "unique name `%s' assigned\n", client_name);
    }

    jack_set_process_callback(client, process, 0);
    // jack_set_port_connect_callback(client, port_connected, 0);
    jack_on_shutdown(client, jack_shutdown, 0);

    input_port = jack_port_register(client, "input1", JACK_DEFAULT_AUDIO_TYPE,
                                    JackPortIsInput, 0);
    beat_midi_port = jack_port_register(
        client, "MIDI beat", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);

    if (input_port == NULL) {
        fprintf(stderr, "ERROR: Failed to open JACK input port.\n");
        return 1;
    }

    if (jack_activate(client)) {
        fprintf(stderr, "cannot activate client");
        return 1;
    }

    signal(SIGQUIT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGHUP, signal_handler);
    signal(SIGINT, signal_handler);
    return 0;
}

void jack_deinit(void) {
    jack_client_close(client);
}
