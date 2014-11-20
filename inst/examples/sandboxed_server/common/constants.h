/*
 * Copyright (c) 2014 Christian Authmann
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once


#define ris_socket_address "example_server.sock"

const uint32_t RIS_MAGIC_NUMBER = 0xF00BA5;
const char RIS_CMD_SETVALUE = 1;
const char RIS_CMD_GETVALUE = 2;
const char RIS_CMD_SETCALLBACK = 3;
const char RIS_CMD_RUN = 4;
const char RIS_CMD_GETCONSOLE = 5;
const char RIS_CMD_INITPLOT = 6;
const char RIS_CMD_GETPLOT = 7;
const char RIS_CMD_EXIT = 8;


const char RIS_REPLY_OK = 101;
const char RIS_REPLY_CALLBACK = 102;
const char RIS_REPLY_VALUE = 103;
const char RIS_REPLY_ERROR = 104;


/*
 * The Socket protocol is as follows:
 *
 * The Client initiates the connection by sending the "magic number"
 *
 * Then the Client sends a CMD, followed by the required parameters.
 * The server sends a REPLY, followed by a value, an error or any other
 * relevant payload.
 *
 * For the exact parameters of each command, see rinsideserver.cpp
 *
 * Communication is continued until the client terminates the connection. The server
 * should only terminate the connection when encountering an unrecoverable error.
 */
