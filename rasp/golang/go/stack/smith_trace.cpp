#include "smith_trace.h"
#include "printf/printf.h"
#include <go/symbol/line_table.h>
#include <cstring>

void _putchar(char) {}

void CSmithTrace::push(const go::Int &arg) {
    snprintf(args[count++], ARG_LENGTH, "%lld", arg);
}

void CSmithTrace::push(const go::Uint32 &arg) {
    snprintf(args[count++], ARG_LENGTH, "%u", arg);
}

void CSmithTrace::push(const go::string &arg) {
    char *buffer = args[count++];

    if (arg.empty())
        return;

    snprintf(buffer, ARG_LENGTH, "%.*s", arg.length, arg.data);
}

void CSmithTrace::push(const go::slice<go::string> &arg) {
    char *buffer = args[count++];

    for (int i = 0; i < arg.count * 2 - 1; i++) {
        auto length = strlen(buffer);

        if (i % 2) {
            if (length + 1 >= ARG_LENGTH)
                break;

            strcat(buffer, " ");
        } else {
            auto index = i/2;

            if (arg[index].empty())
                continue;

            if (length + arg[index].length >= ARG_LENGTH)
                break;

            strncat(buffer, arg[index].data, arg[index].length);
        }
    }
}

void CSmithTrace::push(const tcp_addr *arg) {
    char *buffer = args[count++];

    if (!arg)
        return;

    char address[1024] = {};

    switch (arg->ip.count) {
        case 4:
            snprintf(address, sizeof(address), "%d.%d.%d.%d", arg->ip[0], arg->ip[1], arg->ip[2], arg->ip[3]);
            break;

        case 16:
            snprintf(
                    address,
                    sizeof(address),
                    "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                    arg->ip[0], arg->ip[1], arg->ip[2], arg->ip[3],
                    arg->ip[4], arg->ip[5], arg->ip[6], arg->ip[7],
                    arg->ip[8], arg->ip[9], arg->ip[10], arg->ip[11],
                    arg->ip[12], arg->ip[13], arg->ip[14], arg->ip[15]
                    );
            break;

        default:
            return;
    }

    if (arg->zone.empty()) {
        snprintf(buffer, ARG_LENGTH, "%s:%lld", address, arg->port);
        return;
    }

    snprintf(buffer, ARG_LENGTH, "%s:%lld:%.*s", address, arg->port, arg->zone.length, arg->zone.data);
}

void CSmithTrace::push(const ip_addr *arg) {
    char *buffer = args[count++];

    if (!arg)
        return;

    char address[1024] = {};

    switch (arg->ip.count) {
        case 4:
            snprintf(address, sizeof(address), "%d.%d.%d.%d", arg->ip[0], arg->ip[1], arg->ip[2], arg->ip[3]);
            break;

        case 16:
            snprintf(
                    address,
                    sizeof(address),
                    "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                    arg->ip[0], arg->ip[1], arg->ip[2], arg->ip[3],
                    arg->ip[4], arg->ip[5], arg->ip[6], arg->ip[7],
                    arg->ip[8], arg->ip[9], arg->ip[10], arg->ip[11],
                    arg->ip[12], arg->ip[13], arg->ip[14], arg->ip[15]
            );
            break;

        default:
            return;
    }

    if (arg->zone.empty()) {
        snprintf(buffer, ARG_LENGTH, "%s", address);
        return;
    }

    snprintf(buffer, ARG_LENGTH, "%s:%.*s", address, arg->zone.length, arg->zone.data);
}

void CSmithTrace::push(const unix_addr *arg) {
    char *buffer = args[count++];

    if (!arg || arg->name.empty())
        return;

    if (arg->net.empty()) {
        snprintf(buffer, ARG_LENGTH, "%.*s", arg->name.length, arg->name.data);
        return;
    }

    snprintf(buffer, ARG_LENGTH, "%.*s:%.*s", arg->name.length, arg->name.data, arg->net.length, arg->net.data);
}

void CSmithTrace::push(const exec_cmd *arg) {
    if (!arg)
        return;

    push(arg->path);
    push(arg->args);
}

void CSmithTrace::traceback(void *sp) {
    for (auto &st : stackTrace) {
        void *pc = *(void **)sp;

        CFunction func = {};

        if (!gLineTable->findFunc(pc, func))
            break;

        st = {pc, func};

        if (func.isStackTop())
            break;

        int frameSize = func.getFrameSize(pc);

        if (frameSize == 0)
            break;

        sp = (char *)sp + frameSize + sizeof(unsigned long);
    }
}
