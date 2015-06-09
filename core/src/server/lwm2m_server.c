/************************************************************************************************************************
 Copyright (c) 2016, Imagination Technologies Limited and/or its affiliated group companies.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 following conditions are met:
     1. Redistributions of source code must retain the above copyright notice, this list of conditions and the
        following disclaimer.
     2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
        following disclaimer in the documentation and/or other materials provided with the distribution.
     3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote
        products derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
 USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
************************************************************************************************************************/


#include <poll.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>

#include "lwm2m_object_store.h"
#include "coap_abstraction.h"
#include "lwm2m_server_xml_handlers.h"
#include "lwm2m_xml_interface.h"
#include "lwm2m_core.h"
#include "lwm2m_serdes.h"
#include "lwm2m_object_defs.h"
#include "lwm2m_core.h"

#define DEFAULT_IP_ADDRESS "0.0.0.0"
#define DEFAULT_COAP_PORT (5683)
#define DEFAULT_IPC_PORT (54321)

typedef struct
{
    int CoapPort;
    int IpcPort;
    bool Verbose;
    bool Daemonise;
    char * Logfile;
    char * IPAddress;
    char * InterfaceName;
    int AddressFamily;
    int ContentType;
} Options;

static FILE * logFile = NULL;
static const char * version = VERSION;  // from Makefile
static volatile int quit = 0;

static void Lwm2m_CtrlCSignalHandler(int dummy)
{
    Lwm2m_Debug("Exit triggered\n");
    quit = 1;
}

// Fork off a daemon process, the parent will exit at this point
static void Daemonise(bool verbose)
{
    pid_t pid;

    // fork off the parent process
    pid = fork();

    if (pid < 0)
    {
        printf("Failed to start daemon\n");
        exit(EXIT_FAILURE);
    }

    if (pid > 0)
    {
        if (verbose)
            printf("Daemon running as %d\n", pid);
        exit(EXIT_SUCCESS);
    }

    umask(0);

    // create sid for child
    if (setsid() < 0)
    {
        printf("Failed to set sid\n");
        exit(EXIT_FAILURE);
    }

    // close off standard file descriptors
    close (STDIN_FILENO);
    close (STDOUT_FILENO);
    close (STDERR_FILENO);
}

static int Lwm2mServer_Start(Options * options)
{
    int xmlFd;

    if (options->Daemonise)
    {
        Daemonise(options->Verbose);
    }
    else
    {
        signal(SIGINT, Lwm2m_CtrlCSignalHandler);
    }

    signal(SIGTERM, Lwm2m_CtrlCSignalHandler);

    // open log files here
    if (options->Logfile != NULL)
    {
        errno = 0;
        logFile = fopen(options->Logfile, "at");
        if (logFile != NULL)
        {
            Lwm2m_SetOutput(logFile);

            dup2(fileno(logFile), STDOUT_FILENO);
        }
        else
        {
            Lwm2m_Error("Failed to open log file %s: %s\n", options->Logfile, strerror(errno));
        }
    }

    Lwm2m_SetLogLevel((options->Verbose) ? DebugLevel_Debug : DebugLevel_Info);
    Lwm2m_PrintBanner();
    Lwm2m_Info("LWM2M server - version %s\n", version);
    Lwm2m_Info("LWM2M server - CoAP port %d\n", options->CoapPort);
    Lwm2m_Info("LWM2M server - IPC port %d\n", options->IpcPort);

    if (options->InterfaceName != NULL)
    {
        Lwm2m_Info("LWM2M server - Using interface %s [IPv%d]\n", options->InterfaceName, options->AddressFamily == AF_INET? 4 : 6);
    }
    else if (strcmp(DEFAULT_IP_ADDRESS, options->IPAddress) != 0)
    {
        Lwm2m_Info("LWM2M server - IP Address %s\n", options->IPAddress);
    }

    char ipAddress[NI_MAXHOST];
    if (options->InterfaceName != NULL)
    {
        if (Lwm2mCore_GetIPAddressFromInterface(options->InterfaceName, options->AddressFamily, ipAddress, sizeof(ipAddress)) != 0)
        {
            return -1;
        }
        Lwm2m_Info("LWM2M server - Interface Address %s\n", ipAddress);
    }
    else
    {
        strcpy(ipAddress, options->IPAddress);
    }

    CoapInfo * coap = coap_Init(ipAddress, options->CoapPort, (options->Verbose) ? DebugLevel_Debug : DebugLevel_Info);
    if (coap == NULL)
    {
        printf("Unable to map address to network interface\n");
        if (logFile)
            fclose(logFile);
        return 0;
    }
    Lwm2mContextType * context = Lwm2mCore_Init(NULL, options->ContentType);  // NULL, don't map coap with objectStore

    // must happen after coap Init.
    Lwm2m_RegisterObjectTypes(context);

    // listen for UDP packets on port 12345 for now.
    xmlFd = xmlif_init(context, options->IpcPort);
    if (xmlFd < 0)
    {
        if (logFile)   
            fclose(logFile);
        return -1;
    }
    xmlif_RegisterHandlers();

    // wait for messages on both the "IPC" and coap interfaces
    while (!quit)
    {
        int result;
        struct pollfd fds[2];
        int nfds = 2;
        int timeout;

        fds[0].fd = coap->fd;
        fds[0].events = POLLIN;

        fds[1].fd = xmlFd;
        fds[1].events = POLLIN;

        timeout = Lwm2mCore_Process(context);

        result = poll(fds, nfds, timeout);

        if (result < 0)
        {
            if (errno == EINTR)
                continue;
            perror("poll:");
            break;
        }
        else if (result > 0)
        {
            if (fds[0].revents == POLLIN)
            {
                coap_HandleMessage();
            }
            if (fds[1].revents == POLLIN)
            {
                xmlif_process(fds[1].fd);
            }
        }
        coap_Process();
    }

    Lwm2m_Info("Server exiting\n");

    Lwm2mCore_Destroy(context);
    coap_Destroy();
    xmlif_destroy(xmlFd);

    if (logFile != NULL)
    {
        fclose(logFile);
    }

    return 0;
}

static void PrintUsage(void)
{
    printf("Imagination Technologies LWM2M Server - %s\n\n", version);

    printf("Usage: awa_serverd [options]\n\n");

    printf("Options:\n");
    printf("  --ip                : IP address for server\n");
    printf("  --interface         : Network interface for server\n");
    printf("  --addressFamily     : Address family for network interface. 4 for IPv4, 6 for IPv6\n");
    printf("  --port, -p          : port number for CoAP communications\n");
    printf("  --ipcPort, -i       : port number for IPC communications\n");
    printf("  --contentType, -m   : Content Type ID (default 1542 - TLV)\n");
    printf("  --logFile           : log filename\n");
    printf("  --daemonise, -d     : run as daemon\n");
    printf("  --verbose, -v       : enable verbose output\n");
    printf("  --help              : show usage\n\n");

    printf("Example:\n");
    printf("    awa_serverd --interface eth0 --addressFamily 4 --port 5683\n");

}

static int ParseOptions(int argc, char ** argv, Options * options)
{
    while (1)
    {
        int optionIndex = 0;

        static struct option longOptions[] =
        {
            {"ip",            required_argument, 0, 'a'},
            {"interface",     required_argument, 0, 'e'},
            {"addressFamily", required_argument, 0, 'f'},
            {"port",          required_argument, 0, 'p'},
            {"ipcPort",       required_argument, 0, 'i'},
            {"contentType",   required_argument, 0, 'm'},
            {"verbose",       no_argument,       0, 'v'},
            {"daemonise",     no_argument,       0, 'd'},
            {"logFile",       required_argument, 0, 'l'},
            {"help",          no_argument,       0, 'h'},
            {0,               0,                 0,  0 }
        };

        int c = getopt_long(argc, argv, "a:c:f:p:i:m:vdl:h", longOptions, &optionIndex);
        if (c == -1)
        {
            break;
        }

        switch (c)
        {
            case 'a':
                options->IPAddress = optarg;
                break;
            case 'e':
                options->InterfaceName = optarg;
                break;
            case 'f':
                options->AddressFamily = atoi(optarg) == 4 ? AF_INET : AF_INET6;
                break;
            case 'p':
                options->CoapPort = atoi(optarg);
                break;
            case 'i':
                options->IpcPort = atoi(optarg);
                break;
            case 'm':
                options->ContentType = atoi(optarg);
                break;
            case 'd':
                options->Daemonise = true;
                break;
            case 'v':
                options->Verbose = true;
                break;
            case 'l':
                options->Logfile = optarg;
                break;
            case 'h':
            default:
                PrintUsage();
                exit(EXIT_FAILURE);
        }
    }

    return 0;
}

int main(int argc, char ** argv)
{
    Options options =
    {
        .CoapPort = DEFAULT_COAP_PORT,
        .IpcPort = DEFAULT_IPC_PORT,
        .Verbose = false,
        .Daemonise = false,
        .Logfile = NULL,
        .IPAddress = DEFAULT_IP_ADDRESS,
        .InterfaceName = NULL,
        .AddressFamily = AF_INET,
        .ContentType = ContentType_ApplicationOmaLwm2mTLV,
    };

    if (ParseOptions(argc, argv, &options) == 0)
    {
        Lwm2mServer_Start(&options);
    }

    exit(EXIT_SUCCESS);
}