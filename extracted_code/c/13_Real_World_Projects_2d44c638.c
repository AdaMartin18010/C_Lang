/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 1785
 * Language: c
 * Block ID: 2d44c638
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * CGI Handler - Execute external programs for dynamic content
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include "cgi_handler.h"

bool cgi_is_script_request(const char *uri, const cgi_config_t *config) {
    return (strncmp(uri, config->script_alias, strlen(config->script_alias)) == 0);
}

static void set_cgi_env(const http_request_t *req, const cgi_config_t *config) {
    char buf[1024];

    setenv("REQUEST_METHOD", http_method_text(req->method), 1);
    setenv("SCRIPT_NAME", req->uri, 1);
    setenv("PATH_INFO", "", 1);
    setenv("QUERY_STRING", "", 1);

    /* Parse query string */
    char *qmark = strchr(req->uri, '?');
    if (qmark) {
        *qmark = '\0';
        setenv("QUERY_STRING", qmark + 1, 1);
    }

    snprintf(buf, sizeof(buf), "%zu", req->body_len);
    setenv("CONTENT_LENGTH", buf, 1);

    const char *content_type = http_get_header(req, "content-type");
    setenv("CONTENT_TYPE", content_type ? content_type : "", 1);

    setenv("SERVER_NAME", "CHTTPServer", 1);
    setenv("SERVER_PORT", "8080", 1);
    setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
    setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);

    /* Custom env vars */
    for (int i = 0; i < config->env_count; i++) {
        putenv(config->env_vars[i]);
    }
}

int cgi_execute(const char *script_path, const http_request_t *req,
                char **output, size_t *output_len, const cgi_config_t *config) {
    int stdout_pipe[2];
    int stdin_pipe[2];

    if (pipe(stdout_pipe) < 0 || pipe(stdin_pipe) < 0) {
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        close(stdout_pipe[0]);
        close(stdout_pipe[1]);
        close(stdin_pipe[0]);
        close(stdin_pipe[1]);
        return -1;
    }

    if (pid == 0) {
        /* Child process */
        close(stdout_pipe[0]);
        close(stdin_pipe[1]);

        dup2(stdout_pipe[1], STDOUT_FILENO);
        dup2(stdin_pipe[0], STDIN_FILENO);

        close(stdout_pipe[1]);
        close(stdin_pipe[0]);

        /* Set CGI environment */
        set_cgi_env(req, config);

        /* Execute script */
        execl(script_path, script_path, NULL);

        /* If exec fails */
        perror("execl");
        _exit(1);
    }

    /* Parent process */
    close(stdout_pipe[1]);
    close(stdin_pipe[0]);

    /* Write request body to CGI */
    if (req->body_len > 0) {
        write(stdin_pipe[1], req->body, req->body_len);
    }
    close(stdin_pipe[1]);

    /* Read output with timeout */
    *output = malloc(config->max_output);
    if (!*output) {
        close(stdout_pipe[0]);
        kill(pid, SIGTERM);
        waitpid(pid, NULL, 0);
        return -1;
    }

    size_t total = 0;
    int status;
    time_t start = time(NULL);

    while (total < config->max_output) {
        /* Check timeout */
        if (time(NULL) - start > config->timeout) {
            kill(pid, SIGTERM);
            waitpid(pid, NULL, 0);
            free(*output);
            close(stdout_pipe[0]);
            return -1;
        }

        /* Non-blocking check if child exited */
        pid_t result = waitpid(pid, &status, WNOHANG);

        /* Try to read data */
        ssize_t n = read(stdout_pipe[0], *output + total,
                        config->max_output - total);
        if (n > 0) {
            total += (size_t)n;
        } else if (n == 0 && result != 0) {
            /* EOF and child exited */
            break;
        } else if (n < 0 && errno != EAGAIN && errno != EINTR) {
            break;
        }

        if (result != 0) {
            /* Child exited, drain remaining output */
            continue;
        }

        usleep(10000);  /* 10ms */
    }

    close(stdout_pipe[0]);

    /* Ensure child is reaped */
    if (waitpid(pid, &status, WNOHANG) == 0) {
        kill(pid, SIGTERM);
        waitpid(pid, NULL, 0);
    }

    *output_len = total;
    return (total > 0) ? 0 : -1;
}

int cgi_handle_request(const http_request_t *req, http_response_t *resp,
                       const cgi_config_t *config) {
    /* Build script path */
    char script_path[1024];
    const char *script_name = req->uri + strlen(config->script_alias);

    snprintf(script_path, sizeof(script_path), "%s/%s",
             config->cgi_path, script_name);

    /* Security: prevent directory traversal */
    if (strstr(script_name, "..") != NULL) {
        http_set_status(resp, HTTP_FORBIDDEN);
        http_set_body(resp, "<h1>403 Forbidden</h1>", 22);
        return -1;
    }

    /* Check script exists and is executable */
    if (access(script_path, X_OK) != 0) {
        http_set_status(resp, HTTP_NOT_FOUND);
        http_set_body(resp, "<h1>404 Not Found</h1>", 22);
        return -1;
    }

    /* Execute CGI */
    char *output = NULL;
    size_t output_len = 0;

    if (cgi_execute(script_path, req, &output, &output_len, config) != 0) {
        http_set_status(resp, HTTP_BAD_GATEWAY);
        http_set_body(resp, "<h1>502 Bad Gateway</h1>", 24);
        return -1;
    }

    /* Parse CGI output (headers + body) */
    char *body = memmem(output, output_len, "\r\n\r\n", 4);
    if (!body) {
        body = memmem(output, output_len, "\n\n", 2);
    }

    if (body) {
        /* Parse headers from CGI output */
        *body = '\0';
        body += (body[-1] == '\r') ? 4 : 2;

        /* Simple header parsing */
        char *line = strtok(output, "\r\n");
        while (line) {
            char *colon = strchr(line, ':');
            if (colon) {
                *colon = '\0';
                char *value = colon + 1;
                while (*value == ' ') value++;

                if (strcasecmp(line, "Status") == 0) {
                    int status = atoi(value);
                    http_set_status(resp, (http_status_t)status);
                } else {
                    http_add_header(resp, line, value);
                }
            }
            line = strtok(NULL, "\r\n");
        }

        http_set_body(resp, body, output_len - (body - output));
    } else {
        /* No headers, treat all as body */
        http_set_status(resp, HTTP_OK);
        http_set_body(resp, output, output_len);
    }

    http_add_header(resp, "Content-Type", "text/html");
    free(output);

    return 0;
}
